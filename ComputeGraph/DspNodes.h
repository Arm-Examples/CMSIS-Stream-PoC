#ifndef _DSP_NODES_H_
#define _DSP_NODES_H_

#include "cg_status.h"

using namespace arm_cmsis_stream;

#include "IIR.h"
#include "FIR.h"
#include "ConversionNodes.h"

/*

Nodes to interface the compute graph with the timer
interrupt.

Those nodes are flexible enough to handle the cases
where the source or sink need to be scheduled
several times
(For instance, if one call to the source does not
generate enough data for the following node).

*/


template<typename OUT,int outputSize>
class ADC;

// Time interrupt is only generating float
// The template is specialized for float only
template<int outputSize>
class ADC<float32_t,outputSize>: 
 public GenericSource<float32_t,outputSize>
{
public:
    ADC(FIFOBase<float32_t> &dst,
        dsp_context_t *dsp_context):
    GenericSource<float32_t,outputSize>(dst),
    m_dsp_context(dsp_context)
    {
    };

    int prepareForRunning() final
    {
        if (this->willOverflow())
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(0);
    };

    int run() final
    {
        osStatus_t status;
        float32_t *received;
        float32_t *output=this->getWriteBuffer();

        // Wait for a new buffer from the interrupt handler
        status = osMessageQueueGet(g_dsp_context.computeGraphInputQueue,
                &received,
                0,
                CG_TIMEOUT) ;

        if (status != osOK) 
        {
           return(CG_BUFFER_UNDERFLOW);
        }

        // Copy the buffer to the compute graph
        memcpy(output,received,sizeof(float32_t)*outputSize);

        // Free the received buffer from the memory pool
        status = osMemoryPoolFree(m_dsp_context->DSP_MemPool, received);     
        if (status != osOK) 
        {
            return(CG_MEMORY_ALLOCATION_FAILURE);
        }
        
        return(CG_SUCCESS);
    };

protected:
    dsp_context_t *m_dsp_context;
};

template<typename IN, int inputSize>
class DAC;

// The timer interrupt is only consuming float
// so the template is specialized to float only
template<int inputSize>
class DAC<float32_t,inputSize>:
public GenericSink<float32_t, inputSize>
{
public:
    DAC(FIFOBase<float32_t> &src,
        dsp_context_t *dsp_context):
    GenericSink<float32_t,inputSize>(src),
    m_dsp_context(dsp_context)
    { 
    };

    
    int prepareForRunning() final
    {
        if (this->willUnderflow())
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(0);
    };


    int run() final
    {
	    osStatus_t status;
        float32_t *input=this->getReadBuffer();
        float32_t *sent;

        // Get a new buffer from the memory pool
        sent = (float32_t*)osMemoryPoolAlloc(g_dsp_context.DSP_MemPool, 0);
        if (sent == NULL)
        {
           return(CG_MEMORY_ALLOCATION_FAILURE);
        }

        // Copy the input of the node to the new buffer
        memcpy(sent,input,sizeof(float32_t)*inputSize);

        // Put the buffer into the output queue
        // for use by the timer interrupt handler
        status = osMessageQueuePut  ( g_dsp_context.computeGraphOutputQueue,
           &sent,
           0,
           0);
        if (status != osOK)
        {
            return(CG_BUFFER_OVERFLOW);
        }

       
        return(CG_SUCCESS);
    };

protected:
    dsp_context_t *m_dsp_context;

};

template<typename IN, int inputSize> 
class LogicAnalyzer;

template<int inputSize>
class LogicAnalyzer<float32_t,inputSize>: 
public GenericSink<float32_t,inputSize>
{
public:
    LogicAnalyzer(FIFOBase<float32_t> &src, float32_t *out):
    GenericSink<float32_t,inputSize>(src),mOut(out)
    {
    };

    int prepareForRunning() final
    {
        if (this->willUnderflow())
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(CG_SUCCESS);
    };

    int run() final
    {
        float32_t *src=this->getReadBuffer();

        mOut[0] = src[0];

        return(CG_SUCCESS);
    };

protected:
    float32_t *mOut;

};



template<int inputSize>
class LogicAnalyzer<q15_t,inputSize>: 
public GenericSink<q15_t,inputSize>
{
public:
    LogicAnalyzer(FIFOBase<q15_t> &src, float32_t *out):
    GenericSink<q15_t,inputSize>(src),mOut(out)
    {
    };

    int prepareForRunning() final
    {
        if (this->willUnderflow())
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(CG_SUCCESS);
    };

    int run() final
    {
        q15_t *src=this->getReadBuffer();

        arm_q15_to_float(src,mOut,1);

        return(CG_SUCCESS);
    };

protected:
    float32_t *mOut;

};

template<typename IN, int inputSize,
         typename OUT,int outputSize> 
class Threshold;

template<int inputSize>
class Threshold<float32_t,inputSize,
                float32_t,inputSize>: 
          public GenericNode<float32_t,inputSize,
                             float32_t,inputSize>
{
public:
    Threshold(FIFOBase<float32_t> &src,
              FIFOBase<float32_t> &dst,
              float32_t threshold):
    GenericNode<float32_t,inputSize,
                float32_t,inputSize>(src,dst),mThreshold(threshold)
    {
    };

    int prepareForRunning() final
    {
        if (this->willUnderflow() ||
            this->willOverflow())
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(CG_SUCCESS);
    };

    int run() final
    {
        float32_t *src=this->getReadBuffer();
        float32_t *dst=this->getWriteBuffer();

        if (src[0] >= mThreshold)
        {
            dst[0] = 1.0f;
        }
        else
        {
            dst[0] = 0.0f;
        }
        
        return(CG_SUCCESS);
    };

protected:
    float32_t mThreshold;

};


template<int inputSize>
class Threshold<q15_t,inputSize,
                q15_t,inputSize>: 
          public GenericNode<q15_t,inputSize,
                             q15_t,inputSize>
{
public:
    Threshold(FIFOBase<q15_t> &src,
              FIFOBase<q15_t> &dst,
              q15_t threshold):
    GenericNode<q15_t,inputSize,
                q15_t,inputSize>(src,dst),mThreshold(threshold)
    {
    };

    int prepareForRunning() final
    {
        if (this->willUnderflow() ||
            this->willOverflow())
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(CG_SUCCESS);
    };

    int run() final
    {
        q15_t *src=this->getReadBuffer();
        q15_t *dst=this->getWriteBuffer();

        if (src[0] >= mThreshold)
        {
            dst[0] = 0x7FFF;
        }
        else
        {
            dst[0] = 0;
        }
        
        return(CG_SUCCESS);
    };

protected:
    q15_t mThreshold;

};

template<typename IN, int inputSize,
         typename OUT,int outputSize> 
class RMS;

template<int inputSize>
class RMS<float32_t,inputSize,
          float32_t,1>: 
public GenericNode<float32_t,inputSize,
                   float32_t,1>
{
public:
    RMS(FIFOBase<float32_t> &src,
        FIFOBase<float32_t> &dst):
    GenericNode<float32_t,inputSize,
                float32_t,1>(src,dst)
    {
    };

    int prepareForRunning() final
    {
        if (this->willUnderflow() ||
            this->willOverflow())
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(CG_SUCCESS);
    };

    int run() final
    {
        float32_t *src=this->getReadBuffer();
        float32_t *dst=this->getWriteBuffer();
        float32_t mean;
        
        arm_mean_f32(src,inputSize,&mean);
        mean = -mean;
        arm_offset_f32(src,mean,src,inputSize);
        arm_rms_f32(src,inputSize,dst);
        
        return(CG_SUCCESS);
    };
};

template<int inputSize>
class RMS<q15_t,inputSize,
          q15_t,1>: 
public GenericNode<q15_t,inputSize,
                   q15_t,1>
{
public:
    RMS(FIFOBase<q15_t> &src,
        FIFOBase<q15_t> &dst):
    GenericNode<q15_t,inputSize,
                q15_t,1>(src,dst)
    {
    };

    int prepareForRunning() final
    {
        if (this->willUnderflow() ||
            this->willOverflow())
        {
           return(CG_SKIP_EXECUTION_ID_CODE); // Skip execution
        }

        return(CG_SUCCESS);
    };

    int run() final
    {
        q15_t *src=this->getReadBuffer();
        q15_t *dst=this->getWriteBuffer();
        q15_t mean;
        
        arm_mean_q15(src,inputSize,&mean);
        mean = -mean;
        arm_offset_q15(src,mean,src,inputSize);
        arm_rms_q15(src,inputSize,dst);
        
        return(CG_SUCCESS);
    };
};



#endif