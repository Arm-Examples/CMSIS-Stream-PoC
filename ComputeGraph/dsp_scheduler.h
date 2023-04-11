/*

Generated with CMSIS-DSP Compute Graph Scripts.
The generated code is not covered by CMSIS-DSP license.

The support classes and code is covered by CMSIS-DSP license.

*/

#ifndef _DSP_SCHEDULER_H_ 
#define _DSP_SCHEDULER_H_

#ifdef   __cplusplus
extern "C"
{
#endif

#include "EventRecorder.h"

#define EvtSched 0x01 

#define Evt_Scheduler   EventID (EventLevelAPI,   EvtSched, 0x00)
#define Evt_Node        EventID (EventLevelAPI,   EvtSched, 0x01)
#define Evt_Error       EventID (EventLevelError,   EvtSched, 0x02)


extern uint32_t dsp_scheduler(int *error,dsp_context_t *dsp_context);

#ifdef   __cplusplus
}
#endif

#endif

