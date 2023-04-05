#ifndef _FIR_H_ 
#define _FIR_H_

extern "C" {
#include "DSP_FIR.h"
}

template<typename IN, int inputSize,
         typename OUT,int outputSize> 
class FIR;

template<int inputSize>
class FIR<float32_t,inputSize,
          float32_t,inputSize>: 
          public GenericNode<float32_t,inputSize,float32_t,inputSize>
{
public:
    FIR(FIFOBase<float32_t> &src,FIFOBase<float32_t> &dst):
    GenericNode<float32_t,inputSize,float32_t,inputSize>(src,dst)
    {
        firInit_f32();
    };

    int prepareForRunning() final
    {
        
        return(CG_SUCCESS);
    };

    int run() final
    {
        float32_t *src=this->getReadBuffer();
        float32_t *dst=this->getWriteBuffer();
        firExec_f32 (src,dst);
        return(CG_SUCCESS);
    };



};

template<int inputSize>
class FIR<q31_t,inputSize,
          q31_t,inputSize>: 
          public GenericNode<q31_t,inputSize,q31_t,inputSize>
{
public:
    FIR(FIFOBase<q31_t> &src,FIFOBase<q31_t> &dst):
    GenericNode<q31_t,inputSize,q31_t,inputSize>(src,dst)
    {
        firInit_q31();
    };

    int prepareForRunning() final
    {
        
        return(CG_SUCCESS);
    };

    int run() final
    {
        q31_t *src=this->getReadBuffer();
        q31_t *dst=this->getWriteBuffer();
        firExec_q31 (src,dst);
        return(CG_SUCCESS);
    };



};

template<int inputSize>
class FIR<q15_t,inputSize,
           q15_t,inputSize>: 
           public GenericNode<q15_t,inputSize,q15_t,inputSize>
{
public:
    FIR(FIFOBase<q15_t> &src,FIFOBase<q15_t> &dst):
    GenericNode<q15_t,inputSize,q15_t,inputSize>(src,dst)
    {
        firInit_q15();
    };

    int prepareForRunning() final
    {
        
        return(CG_SUCCESS);
    };

    int run() final
    {
        q15_t *src=this->getReadBuffer();
        q15_t *dst=this->getWriteBuffer();
        firExec_q15 (src,dst);
        return(CG_SUCCESS);
    };



};

#endif