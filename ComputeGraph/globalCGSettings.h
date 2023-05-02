#ifndef _GLOBAL_CG_SETTINGS_H_
#define _GLOBAL_CG_SETTINGS_H_

#include "cmsis_os2.h" 
#include "EventRecorder.h"

// Generated from Python to ensure the
// settings are consistent between C and Python
// The file contains:
// DSP block size 
//     Size of blocks produced and consumed by the
//     timer interrupt handler.
// Filter block size
//     Size of blocks processed by the filter
// Audio queue depth
//      Additional output buffering because of latencies
//      See below explanation
#include "params.h"


/* EXPLANATION for AUDIO_QUEUE_DEPTH

AUDIO_QUEUE_DEPTH is 1 + the number of sinks or sources in
sequence in the scheduling.

If the source / sink are interleaved, AUDIO_QUEUE_DEPTH
is 2.

Sometimes, to be able to advance in the computation of 
the graph, several executions of the source are needed because
the following nodes require more sample than what is produced
by one execution of the source.

Or, the compute graph may generate more data than required by
the sink and it would require several executions of the sink 
to consume it.

In those cases, in the scheduling you may see several calls to
the source without any sink in the middle. Or,
several calls to the sink without any call to source in the 
middle.

It is what we call a sequence of sources or sinks.

In those cases, the compute graph may be in advance 
compared to what is needed by the interrupt handler.

The interrupt handler is always producing one input buffer and
consuming one output buffer.

If the graph is calling the sink twice, 2 buffers are produced
to be consumed by the timer interrupt. We need a queue to hold
those buffers since the interrupt handler will just process them
one per one.

Similarly, if 2 calls to source are required, the sink 
will receive a useful buffer only after those 2 calls.
But the timer interrupt is expecting an input / output buffer
at each execution to maintain the realtime.
So the timer interrupt must have some buffer to read 
even if the sink has not yet been executed. The output queue
connecting the sink to the timer interrupt must be pre-loaded
with some buffers in advance.

AUDI_QUEUE_DEPTH is controlling the length of the output queue
connecting a sink to the timer interrupt. It is also controlling
the number of buffers to pre-load in the queue.
*/

/*
TIMEOUT for the source node
A new packet is received every 8 ms in 
this example and OS ticks are every 1 ms

10 ms timeout is chosen for the source
A buffer underflow error will be generated in case
of underflow
*/

#define CG_TIMEOUT 10


#ifdef   __cplusplus
extern "C"
{
#endif
	
/* Globals to interface with logic analyzer*/
extern float32_t TOUT;
extern float32_t EOUT;

// Datatype for an dsp packet
typedef struct _DSP_DataType {
  float32_t     Sample[DSP_BLOCKSIZE];
} DSP_DataType;

// Data structure used to interface the timer interrupt
// handler, the application and the compute graph source
// and sink
typedef struct _DSP_context {
osMemoryPoolId_t  DSP_MemPool;
osMessageQueueId_t computeGraphInputQueue;
osMessageQueueId_t computeGraphOutputQueue;
int error;
} dsp_context_t;

extern dsp_context_t g_dsp_context;

#ifdef   __cplusplus
}
#endif

#endif