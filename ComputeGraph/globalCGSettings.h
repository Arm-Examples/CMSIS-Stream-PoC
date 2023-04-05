#ifndef _GLOBAL_CG_SETTINGS_H_
#define _GLOBAL_CG_SETTINGS_H_

#include "cmsis_os2.h" 
#define DSP_BLOCKSIZE 256


#ifdef   __cplusplus
extern "C"
{
#endif

typedef struct _DSP_DataType {
#ifdef __FLOAT32
  float32_t Sample[DSP_BLOCKSIZE];
#endif
#ifdef __Q31
  q31_t     Sample[DSP_BLOCKSIZE];
#endif
#ifdef __Q15
  q15_t     Sample[DSP_BLOCKSIZE];
#endif
} DSP_DataType;


typedef struct DSP_context {
osMemoryPoolId_t  DSP_MemPool;
osEventFlagsId_t  DSP_Event;
DSP_DataType      *pDataTimIrqIn;
DSP_DataType      *pDataTimIrqOut;
DSP_DataType      *pDataSigModOut;
DSP_DataType      *pDataSigModIn;
} dsp_context_t;

extern dsp_context_t g_dsp_context;

#define EVENT_DATA_TIM_OUT_SIG_IN    1
#define EVENT_DATA_TIM_IN_SIG_OUT    2

#ifdef   __cplusplus
}
#endif

#endif