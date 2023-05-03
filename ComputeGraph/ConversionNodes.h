#ifndef _CONVERSION_NODES_H_ 
#define _CONVERSION_NODES_H_ 

/*

Conversions between float and fixed point.

*/

template<typename IN, int inputSize,
         typename OUT,int outputSize> 
class ToFixedPoint;

// The node is specialized to the right
// input / output datatypes.
template<int inputSize>
class ToFixedPoint<float32_t,inputSize,
               q15_t,inputSize>: 
          public GenericNode<float32_t,inputSize,q15_t,inputSize>
{
public:
    ToFixedPoint(FIFOBase<float32_t> &src,FIFOBase<q15_t> &dst):
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
class ToFloat;


template<int inputSize>
class ToFloat<q15_t,inputSize,
               float32_t,inputSize>: 
          public GenericNode<q15_t,inputSize,float32_t,inputSize>
{
public:
    ToFloat(FIFOBase<q15_t> &src,FIFOBase<float32_t> &dst):
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