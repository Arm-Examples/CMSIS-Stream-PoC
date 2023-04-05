#ifndef _DSP_NODES_H_
#define _DSP_NODES_H_

#include "cg_status.h"
#include "IIR.h"

template<typename OUT,int outputSize>
class ADC: public GenericSource<OUT,outputSize>
{
public:
    ADC(FIFOBase<OUT> &dst,
        dsp_context_t *dsp_context):
    GenericSource<OUT,outputSize>(dst),
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
        int32_t flags;
        osStatus_t status;
        OUT *output=this->getWriteBuffer();

        flags = osEventFlagsWait(m_dsp_context->DSP_Event, EVENT_DATA_TIM_OUT_SIG_IN, 0, osWaitForever);
        if (flags < 0) 
        {
           return(CG_BUFFER_UNDERFLOW);
        }

        memcpy(output,m_dsp_context->pDataSigModIn,sizeof(OUT)*outputSize);

        status = osMemoryPoolFree(m_dsp_context->DSP_MemPool, m_dsp_context->pDataSigModIn);     
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
class DAC:public GenericSink<IN, inputSize>
{
public:
    DAC(FIFOBase<IN> &src,
        dsp_context_t *dsp_context):
    GenericSink<IN,inputSize>(src),
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
			  uint32_t flags;
        IN *input=this->getReadBuffer();
        memcpy(m_dsp_context->pDataSigModOut,input,sizeof(IN)*inputSize);

        // data is ready
        m_dsp_context->pDataTimIrqIn = m_dsp_context->pDataSigModOut;
        flags = osEventFlagsSet(m_dsp_context->DSP_Event, EVENT_DATA_TIM_IN_SIG_OUT);
        if (flags < 0) 
        {
           return(CG_OS_ERROR);
        }
    
        // allocate next output buffer
        m_dsp_context->pDataSigModOut = (DSP_DataType*)osMemoryPoolAlloc(m_dsp_context->DSP_MemPool, 0); 
        if (m_dsp_context->pDataSigModOut == NULL) 
        {
           return(CG_MEMORY_ALLOCATION_FAILURE);
        }

        return(CG_SUCCESS);
    };

protected:
    dsp_context_t *m_dsp_context;

};


#endif