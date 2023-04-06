#ifndef _CONVERSION_NODES_H_ 
#define _CONVERSION_NODES_H_ 

/*

Conversions between float and fixed point.

*/

template<typename IN, int inputSize,
         typename OUT,int outputSize> 
class F32TOQ15;

// The node is specialized to the right
// input / output datatypes.
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