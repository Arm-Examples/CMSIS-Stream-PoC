#ifndef _IIR_H_ 
#define _IIR_H_

extern "C" {
#include "DSP_IIR.h"
}

template<typename IN, int inputSize,
         typename OUT,int outputSize> 
class IIR;

template<int inputSize>
class IIR<float32_t,inputSize,
          float32_t,inputSize>: 
          public GenericNode<float32_t,inputSize,float32_t,inputSize>
{
public:
    IIR(FIFOBase<float32_t> &src,FIFOBase<float32_t> &dst):
    GenericNode<float32_t,inputSize,float32_t,inputSize>(src,dst)
    {
        iirInit_f32();
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
        iirExec_f32 (src,dst);
        return(CG_SUCCESS);
    };



};

template<int inputSize>
class IIR<q31_t,inputSize,
          q31_t,inputSize>: 
          public GenericNode<q31_t,inputSize,q31_t,inputSize>
{
public:
    IIR(FIFOBase<q31_t> &src,FIFOBase<q31_t> &dst):
    GenericNode<q31_t,inputSize,q31_t,inputSize>(src,dst)
    {
        iirInit_q31();
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
        q31_t *dst=this->getWriteBuffer();
        iirExec_q31 (src,dst);
        return(CG_SUCCESS);
    };



};

template<int inputSize>
class IIR<q15_t,inputSize,
           q15_t,inputSize>: 
           public GenericNode<q15_t,inputSize,q15_t,inputSize>
{
public:
    IIR(FIFOBase<q15_t> &src,FIFOBase<q15_t> &dst):
    GenericNode<q15_t,inputSize,q15_t,inputSize>(src,dst)
    {
        iirInit_q15();
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
        iirExec_q15 (src,dst);
        return(CG_SUCCESS);
    };



};

#endif