#ifndef _DSP_NODES_H_
#define _DSP_NODES_H_

#include "cg_status.h"
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


#endif