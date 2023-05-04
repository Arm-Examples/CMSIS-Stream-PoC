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
static unsigned int schedule[28]=
{ 
0,9,5,3,10,2,0,9,5,3,10,2,6,4,1,7,8,0,9,5,3,10,2,6,4,1,7,8,
};

CG_BEFORE_FIFO_BUFFERS
/***********

FIFO buffers

************/
#define FIFOSIZE0 256
#define FIFOSIZE1 256
#define FIFOSIZE2 256
#define FIFOSIZE3 1
#define FIFOSIZE4 256
#define FIFOSIZE5 256
#define FIFOSIZE6 512
#define FIFOSIZE7 1
#define FIFOSIZE8 1
#define FIFOSIZE9 1

#define BUFFERSIZE1 256
CG_BEFORE_BUFFER
float32_t dsp_buf1[BUFFERSIZE1]={0};

#define BUFFERSIZE2 256
CG_BEFORE_BUFFER
q15_t dsp_buf2[BUFFERSIZE2]={0};

#define BUFFERSIZE3 256
CG_BEFORE_BUFFER
float32_t dsp_buf3[BUFFERSIZE3]={0};

#define BUFFERSIZE4 1
CG_BEFORE_BUFFER
q15_t dsp_buf4[BUFFERSIZE4]={0};

#define BUFFERSIZE5 256
CG_BEFORE_BUFFER
q15_t dsp_buf5[BUFFERSIZE5]={0};

#define BUFFERSIZE6 256
CG_BEFORE_BUFFER
q15_t dsp_buf6[BUFFERSIZE6]={0};

#define BUFFERSIZE7 512
CG_BEFORE_BUFFER
q15_t dsp_buf7[BUFFERSIZE7]={0};

#define BUFFERSIZE8 1
CG_BEFORE_BUFFER
q15_t dsp_buf8[BUFFERSIZE8]={0};

#define BUFFERSIZE9 1
CG_BEFORE_BUFFER
q15_t dsp_buf9[BUFFERSIZE9]={0};

#define BUFFERSIZE10 1
CG_BEFORE_BUFFER
q15_t dsp_buf10[BUFFERSIZE10]={0};


CG_BEFORE_SCHEDULER_FUNCTION
uint32_t dsp_scheduler(int *error,dsp_context_t *dsp_context)
{
    int cgStaticError=0;
    uint32_t nbSchedule=0;

    CG_BEFORE_FIFO_INIT;
    /*
    Create FIFOs objects
    */
    FIFO<float32_t,FIFOSIZE0,1,0> fifo0(dsp_buf1);
    FIFO<q15_t,FIFOSIZE1,1,0> fifo1(dsp_buf2);
    FIFO<float32_t,FIFOSIZE2,1,0> fifo2(dsp_buf3);
    FIFO<q15_t,FIFOSIZE3,1,0> fifo3(dsp_buf4);
    FIFO<q15_t,FIFOSIZE4,1,0> fifo4(dsp_buf5);
    FIFO<q15_t,FIFOSIZE5,1,0> fifo5(dsp_buf6);
    FIFO<q15_t,FIFOSIZE6,0,0> fifo6(dsp_buf7);
    FIFO<q15_t,FIFOSIZE7,1,0> fifo7(dsp_buf8);
    FIFO<q15_t,FIFOSIZE8,1,0> fifo8(dsp_buf9);
    FIFO<q15_t,FIFOSIZE9,1,0> fifo9(dsp_buf10);

    CG_BEFORE_NODE_INIT;
    /* 
    Create node objects
    */
    ADC<float32_t,256> adc(fifo0,dsp_context);
    LogicAnalyzer<q15_t,1> amplitude_log(fifo9,&EOUT);
    DAC<float32_t,256> dac(fifo2,dsp_context);
    Duplicate2<q15_t,256,q15_t,256,q15_t,256> dup0(fifo4,fifo5,fifo6);
    Duplicate2<q15_t,1,q15_t,1,q15_t,1> dup1(fifo7,fifo8,fifo9);
    IIR<q15_t,256,q15_t,256> iir(fifo1,fifo4);
    RMS<q15_t,384,q15_t,1> rms(fifo6,fifo7);
    Threshold<q15_t,1,q15_t,1> threshold(fifo8,fifo3,13763);
    LogicAnalyzer<q15_t,1> threshold_log(fifo3,&TOUT);
    ToFixedPoint<float32_t,256,q15_t,256> toFixedPoint(fifo0,fifo1);
    ToFloat<q15_t,256,float32_t,256> toFloat(fifo5,fifo2);

    /* Run several schedule iterations */
    CG_BEFORE_SCHEDULE;
    while(cgStaticError==0)
    {
        /* Run a schedule iteration */
        EventRecord2 (Evt_Scheduler, nbSchedule, 0);
        CG_BEFORE_ITERATION;
        for(unsigned long id=0 ; id < 28; id++)
        {
            EventRecord2 (Evt_Node, schedule[id], 0);
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
                   cgStaticError = amplitude_log.run();
                }
                break;

                case 2:
                {
                   cgStaticError = dac.run();
                }
                break;

                case 3:
                {
                   cgStaticError = dup0.run();
                }
                break;

                case 4:
                {
                   cgStaticError = dup1.run();
                }
                break;

                case 5:
                {
                   cgStaticError = iir.run();
                }
                break;

                case 6:
                {
                   cgStaticError = rms.run();
                }
                break;

                case 7:
                {
                   cgStaticError = threshold.run();
                }
                break;

                case 8:
                {
                   cgStaticError = threshold_log.run();
                }
                break;

                case 9:
                {
                   cgStaticError = toFixedPoint.run();
                }
                break;

                case 10:
                {
                   cgStaticError = toFloat.run();
                }
                break;

                default:
                break;
            }
            CG_AFTER_NODE_EXECUTION;
            if (cgStaticError<0)
            {
                EventRecord2 (Evt_Error, cgStaticError, 0);
            }
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
