#ifndef _GLOBAL_CG_SETTINGS_H_
#define _GLOBAL_CG_SETTINGS_H_

#include "cmsis_os2.h" 

#define DSP_BLOCKSIZE 256

#define AUDIO_QUEUE_DEPTH 3
#define FILTER_BLOCKSIZE 192

//#define AUDIO_QUEUE_DEPTH 2
//#define FILTER_BLOCKSIZE 256

#define CG_TIMEOUT 10


#ifdef   __cplusplus
extern "C"
{
#endif

typedef struct _DSP_DataType {
  float32_t     Sample[DSP_BLOCKSIZE];
} DSP_DataType;


typedef struct _DSP_context {
osMemoryPoolId_t  DSP_MemPool;
osMessageQueueId_t computeGraphInputQueue;
osMessageQueueId_t computeGraphOutputQueue;
DSP_DataType      *pTimInputBuffer;
DSP_DataType      *pTimOutputBuffer;
int error;
} dsp_context_t;

extern dsp_context_t g_dsp_context;

#ifdef   __cplusplus
}
#endif

#endif