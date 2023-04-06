#ifndef _DSP_NODES_H_
#define _DSP_NODES_H_

#include "cg_status.h"
#include "IIR.h"
#include "FIR.h"

template<typename OUT,int outputSize>
class ADC;

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

        status = osMessageQueueGet(g_dsp_context.computeGraphInputQueue,
                &received,
                0,
                CG_TIMEOUT) ;

        if (status != osOK) 
        {
           return(CG_BUFFER_UNDERFLOW);
        }

        memcpy(output,received,sizeof(float32_t)*outputSize);

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

        sent = (float32_t*)osMemoryPoolAlloc(g_dsp_context.DSP_MemPool, 0);
        if (sent == NULL)
        {
           return(CG_MEMORY_ALLOCATION_FAILURE);
        }

        memcpy(sent,input,sizeof(float32_t)*inputSize);

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

template<typename IN, int inputSize,
         typename OUT,int outputSize> 
class F32TOQ15;

template<int inputSize>
class F32TOQ15<float32_t,inputSize,
               q15_t,inputSize>: 
          public GenericNode<float32_t,inputSize,q15_t,inputSize>
{
public:
    F32TOQ15(FIFOBase<float32_t> &src,FIFOBase<q15_t> &dst):
    GenericNode<float32_t,inputSize,q15_t,inputSize>(src,dst)
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
        q15_t *dst=this->getWriteBuffer();
        arm_float_to_q15(src, dst, inputSize); 
        return(CG_SUCCESS);
    };



};

template<typename IN, int inputSize,
         typename OUT,int outputSize> 
class F32TOQ31;

template<int inputSize>
class F32TOQ31<float32_t,inputSize,
               q31_t,inputSize>: 
          public GenericNode<float32_t,inputSize,q31_t,inputSize>
{
public:
    F32TOQ31(FIFOBase<float32_t> &src,FIFOBase<q31_t> &dst):
    GenericNode<float32_t,inputSize,q31_t,inputSize>(src,dst)
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
        q31_t *dst=this->getWriteBuffer();
        arm_float_to_q31(src, dst, inputSize); 
        return(CG_SUCCESS);
    };



};

template<typename IN, int inputSize,
         typename OUT,int outputSize> 
class Q31TOF32;

template<int inputSize>
class Q31TOF32<q31_t,inputSize,
               float32_t,inputSize>: 
          public GenericNode<q31_t,inputSize,float32_t,inputSize>
{
public:
    Q31TOF32(FIFOBase<q31_t> &src,FIFOBase<float32_t> &dst):
    GenericNode<q31_t,inputSize,float32_t,inputSize>(src,dst)
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
        q31_t *src=this->getReadBuffer();
        float32_t *dst=this->getWriteBuffer();
        arm_q31_to_float(src, dst, inputSize); 
        return(CG_SUCCESS);
    };



};

template<typename IN, int inputSize,
         typename OUT,int outputSize> 
class Q15TOF32;

template<int inputSize>
class Q15TOF32<q15_t,inputSize,
               float32_t,inputSize>: 
          public GenericNode<q15_t,inputSize,float32_t,inputSize>
{
public:
    Q15TOF32(FIFOBase<q15_t> &src,FIFOBase<float32_t> &dst):
    GenericNode<q15_t,inputSize,float32_t,inputSize>(src,dst)
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
        float32_t *dst=this->getWriteBuffer();
        arm_q15_to_float(src, dst, inputSize); 
        return(CG_SUCCESS);
    };



};
#endif