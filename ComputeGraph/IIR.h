#ifndef _IIR_H_ 
#define _IIR_H_

extern "C" {
#include "DSP_IIR.h"
}

template<typename IN, int inputSize,
         typename OUT,int outputSize> 
class IIR;

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