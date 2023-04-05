/*

Generated with CMSIS-DSP Compute Graph Scripts.
The generated code is not covered by CMSIS-DSP license.

The support classes and code is covered by CMSIS-DSP license.

*/


#include "arm_math.h"
#include "globalCGSettings.h"
#include "GenericNodes.h"
#include "DspNodes.h"
#include "dsp_scheduler.h"

#if !defined(CHECKERROR)
#define CHECKERROR       if (cgStaticError < 0) \
       {\
         goto errorHandling;\
       }

#endif

#if !defined(CG_BEFORE_ITERATION)
#define CG_BEFORE_ITERATION
#endif 

#if !defined(CG_AFTER_ITERATION)
#define CG_AFTER_ITERATION
#endif 

#if !defined(CG_BEFORE_SCHEDULE)
#define CG_BEFORE_SCHEDULE
#endif

#if !defined(CG_AFTER_SCHEDULE)
#define CG_AFTER_SCHEDULE
#endif

#if !defined(CG_BEFORE_BUFFER)
#define CG_BEFORE_BUFFER
#endif

#if !defined(CG_BEFORE_FIFO_BUFFERS)
#define CG_BEFORE_FIFO_BUFFERS
#endif

#if !defined(CG_BEFORE_FIFO_INIT)
#define CG_BEFORE_FIFO_INIT
#endif

#if !defined(CG_BEFORE_NODE_INIT)
#define CG_BEFORE_NODE_INIT
#endif

#if !defined(CG_AFTER_INCLUDES)
#define CG_AFTER_INCLUDES
#endif

#if !defined(CG_BEFORE_SCHEDULER_FUNCTION)
#define CG_BEFORE_SCHEDULER_FUNCTION
#endif

#if !defined(CG_BEFORE_NODE_EXECUTION)
#define CG_BEFORE_NODE_EXECUTION
#endif

#if !defined(CG_AFTER_NODE_EXECUTION)
#define CG_AFTER_NODE_EXECUTION
#endif

CG_AFTER_INCLUDES


/*

Description of the scheduling. 

*/
static unsigned int schedule[3]=
{ 
0,2,1,
};

CG_BEFORE_FIFO_BUFFERS
/***********

FIFO buffers

************/
#define FIFOSIZE0 256
#define FIFOSIZE1 256

#define BUFFERSIZE1 256
CG_BEFORE_BUFFER
q15_t dsp_buf1[BUFFERSIZE1]={0};

#define BUFFERSIZE2 256
CG_BEFORE_BUFFER
q15_t dsp_buf2[BUFFERSIZE2]={0};


CG_BEFORE_SCHEDULER_FUNCTION
uint32_t dsp_scheduler(int *error,dsp_context_t *dsp_context)
{
    int cgStaticError=0;
    uint32_t nbSchedule=0;

    CG_BEFORE_FIFO_INIT;
    /*
    Create FIFOs objects
    */
    FIFO<q15_t,FIFOSIZE0,1,0> fifo0(dsp_buf1);
    FIFO<q15_t,FIFOSIZE1,1,0> fifo1(dsp_buf2);

    CG_BEFORE_NODE_INIT;
    /* 
    Create node objects
    */
    ADC<q15_t,256> adc(fifo0,dsp_context);
    DAC<q15_t,256> dac(fifo1,dsp_context);
    IIR<q15_t,256,q15_t,256> iir(fifo0,fifo1);

    /* Run several schedule iterations */
    CG_BEFORE_SCHEDULE;
    while(cgStaticError==0)
    {
        /* Run a schedule iteration */
        CG_BEFORE_ITERATION;
        for(unsigned long id=0 ; id < 3; id++)
        {
            CG_BEFORE_NODE_EXECUTION;

            switch(schedule[id])
            {
                case 0:
                {
                   cgStaticError = adc.run();
                }
                break;

                case 1:
                {
                   cgStaticError = dac.run();
                }
                break;

                case 2:
                {
                   cgStaticError = iir.run();
                }
                break;

                default:
                break;
            }
            CG_AFTER_NODE_EXECUTION;
            CHECKERROR;
        }
       CG_AFTER_ITERATION;
       nbSchedule++;
    }

errorHandling:
    CG_AFTER_SCHEDULE;
    *error=cgStaticError;
    return(nbSchedule);
}
