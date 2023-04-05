#ifndef _DSP_NODES_H_
#define _DSP_NODES_H_

#include "cg_status.h"
#include "IIR.h"
#include "FIR.h"

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