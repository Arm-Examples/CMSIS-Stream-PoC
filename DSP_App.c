/*----------------------------------------------------------------------------
 * Name:    DSP_App.c
 * Purpose: 
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2023 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include "arm_math.h"                   // ARM::CMSIS:DSP
#include "LPC17xx.h"                    // Device header
#include "Timer.h"
#include "ADC.h"
#include "DAC.h"
#include "DSP_IIR.h"
#include "DSP_FIR.h"
#include "cmsis_os2.h"                  // ARM::CMSIS:RTOS2:Keil RTX5
#include "EventRecorder.h"              // Keil.ARM Compiler::Compiler:Event Recorder
#include "dsp_scheduler.h"
#include "cg_status.h"

#include <stdio.h>
/*----------------------------------------------------------------------------
   defines & typedefs for messages
 *----------------------------------------------------------------------------*/

dsp_context_t g_dsp_context;

// Working buffer for the timer interrupt handler
static DSP_DataType      *pTimInputBuffer;
static DSP_DataType      *pTimOutputBuffer;

static uint32_t          dataTimIrqOutIdx;
static uint32_t          dataTimIrqInIdx;
                 

/*----------------------------------------------------------------------------
   Thread IDs
 *----------------------------------------------------------------------------*/
static osThreadId_t tid_SigMod;           /* thread id of Signal Modify thread       */

static const osThreadAttr_t t_attr = 
  { NULL, osThreadDetached, NULL, 0U, NULL, 0U, osPriorityNormal, 0U, 0U};

#define OutAmpl      511UL         /* Output Amplitute (Range  0 - 0.99)      */
#define TimerFreq  32000UL         /* 32KHz Timer Frequency (used for AD/DA)  */

/*----------------------------------------------------------------------------
   error Handler
 *----------------------------------------------------------------------------*/
void errHandler (uint32_t errCode) {

  errCode = errCode;
  while (1) ;                      /* stay here                               */
}

float32_t tmpFilterIn;             /* 'global'  to display in LogicAnalyzer   */
float32_t tmpFilterOut;            /* 'global'  to display in LogicAnalyzer   */
float32_t TOUT;
float32_t EOUT;

/* Event definitions for timer events in event recorder */
#define EvtTimer 0
#define Evt_AD   EventID (EventLevelOp,   EvtTimer, 0x01)
#define Evt_DA   EventID (EventLevelOp,   EvtTimer, 0x02)

/*----------------------------------------------------------------------------
   TIM2 IRQ Handler
 *----------------------------------------------------------------------------*/
void TIMER2_IRQHandler(void) {
  uint32_t adGdr;
  osStatus_t status;
  
  float32_t     tmp;

  if (g_dsp_context.error != 0)
  {
    return;
  }

  if (LPC_TIM2->IR & (1UL <<  0)) { /* check MR0 Interrupt                       */

    /* -- signal Input Section ------------------------------------------------- */
    adGdr = LPC_ADC->ADGDR;
    if (adGdr & (1UL << 31)) {			/* Data available ?                          */
      /* scale value and move it in positive/negative range. (12bit Ad = 0xFFF)
         filter in range is -1.0 < value < 1.0                                   */
      tmpFilterIn =  ((float32_t)((adGdr >> 4) & 0xFFF) / (0xFFF / 2)) - 1;

      tmp = tmpFilterIn;
      pTimInputBuffer->Sample[dataTimIrqOutIdx++] = tmp;

      if (dataTimIrqOutIdx >= DSP_BLOCKSIZE) {
        
        // push new filled buffer to compute graph input queue
        EventRecord2(1+EventLevelOp, 0, 0);
        status = osMessageQueuePut  ( g_dsp_context.computeGraphInputQueue,
           &pTimInputBuffer,
           0,
           0);

        if (status != osOK)
        {
          g_dsp_context.error = CG_BUFFER_OVERFLOW;
          return;
        }

        // allocate next buffer for receiving new samples
        pTimInputBuffer = osMemoryPoolAlloc(g_dsp_context.DSP_MemPool, 0);        
        if (pTimInputBuffer == NULL) {
          g_dsp_context.error = CG_MEMORY_ALLOCATION_FAILURE;
          return;
        }    
        
        dataTimIrqOutIdx = 0;
      }
    }
    else 
    {
      errHandler(__LINE__);
    } 
    
    /* -- signal Output Section ------------------------------------------------ */
    if (dataTimIrqInIdx == 0) 
    {
      // Get a new buffer from the compute graph output queue
      status = osMessageQueueGet  ( g_dsp_context.computeGraphOutputQueue,
                &pTimOutputBuffer,
                0,
                0) ;
      if (status != osOK)
      {
        g_dsp_context.error = CG_BUFFER_UNDERFLOW;
        return;
      }
    }
    // If there are remaining samples in the buffer, read a new
    // one and send it to the D/A
    if ((dataTimIrqInIdx > 0) || (pTimOutputBuffer != NULL)) {


      tmp = pTimOutputBuffer->Sample[dataTimIrqInIdx++];
      tmpFilterOut = tmp;

      /* move value in positive range and scale it.   (10bit DA = 0x3FF)
         filter OUT range is -1.0 < value < 1.0                */
      LPC_DAC->DACR = (((uint32_t)((tmpFilterOut + 1) * (0x03FF / 2))) & 0x03FF) <<  6;

      // If all the buffer has been sent, free the buffer
      // from the memory pool
      if (dataTimIrqInIdx >= DSP_BLOCKSIZE) { 
        EventRecord2(2+EventLevelOp, 0, 0);
        // free input buffer       
        status = osMemoryPoolFree(g_dsp_context.DSP_MemPool, pTimOutputBuffer);  
        pTimOutputBuffer = NULL;
        if (status != osOK) {
          g_dsp_context.error = CG_MEMORY_ALLOCATION_FAILURE;
          return;
        }         
        dataTimIrqInIdx = 0;
      }       
    }

    LPC_TIM2->IR |= (1UL <<  0);              /* clear MR0 Interrupt  flag   */
  }
}

/*---------------------------------------------------------------------------
   'Signal Modify' Thread
 *---------------------------------------------------------------------------*/
void SigMod (void __attribute__((unused)) *arg) {
 
  int error;
	int res;
	
  /* Enable events for Compute Graph and timer */
	res = EventRecorderEnable (EventRecordAll,EvtSched,EvtSched);
  if (!res)
  {
      printf("Error enabling event recorder for scheduler\n");
      return;
  }

  res = EventRecorderEnable (EventRecordAll,EvtTimer,EvtTimer);
  if (!res)
  {
      printf("Error enabling event recorder for timer\n");
      return;
  }
	 
  printf("Start compute graph\r\n");
  uint32_t nbIterations = dsp_scheduler(&error,&g_dsp_context);

  printf("Nb iterations = %d\r\n",nbIterations);
  printf("Error code = %d\r\n",error);

}

/*----------------------------------------------------------------------------
   RTX_Features_Init
 *---------------------------------------------------------------------------*/
void RTX_Features_Init (void) {

  osStatus_t status;
  g_dsp_context.error = 0;

  /*

  Output queue : AUDIO_QUEUE_DEPTH
  Input queue : 1

  Input and output buffers being worked on in the
  interrupt handler (but not yet in the queue) 2

  Total : 2 + 1 + AUDIO_QUEUE_DEPTH buffers

  */
  g_dsp_context.DSP_MemPool = osMemoryPoolNew (2 + 1 + AUDIO_QUEUE_DEPTH, sizeof(DSP_DataType), NULL);
  if (g_dsp_context.DSP_MemPool == NULL) {
    errHandler(__LINE__);
  }


  tid_SigMod = osThreadNew(SigMod, NULL, &t_attr); /* start task Signal Modify */ 
  
  pTimInputBuffer = osMemoryPoolAlloc(g_dsp_context.DSP_MemPool, 0); /* allocate memory       */
  pTimOutputBuffer = NULL;
  
  g_dsp_context.computeGraphInputQueue = osMessageQueueNew (1,
        sizeof(DSP_DataType*),
        NULL);

  g_dsp_context.computeGraphOutputQueue = osMessageQueueNew (AUDIO_QUEUE_DEPTH,
        sizeof(DSP_DataType*),
        NULL);

  /* Pre-fill input queue */
  DSP_DataType *buf = osMemoryPoolAlloc(g_dsp_context.DSP_MemPool, 0);;
  if (buf != NULL)
  {
   status = osMessageQueuePut  ( g_dsp_context.computeGraphInputQueue,
        &buf,
        0,
        0);
   if (status != osOK)
   {
     errHandler(__LINE__);
   }
  }

  /* Pre-fill output queue */
  for(int i =0;i<AUDIO_QUEUE_DEPTH-1;i++)
  {
     DSP_DataType *buf = osMemoryPoolAlloc(g_dsp_context.DSP_MemPool, 0);;
     if (buf != NULL)
     {
      status = osMessageQueuePut  ( g_dsp_context.computeGraphOutputQueue,
           &buf,
           0,
           0);
      if (status != osOK)
      {
        errHandler(__LINE__);
      }
     }
  }

  dataTimIrqOutIdx = 0;
  dataTimIrqInIdx  = 0;
}

/*----------------------------------------------------------------------------
 *        Main: Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/
int main (void) {

  //EventRecorderInitialize(EventRecordAll, 1);
  TIM2_Init (TimerFreq);                /* initialize Timer 2 (used for DAC) */
  ADC_Init ();                          /* initialize ADC Controller         */
  DAC_Init ();                          /* initialize DAC Controller         */

  osKernelInitialize();            /* Initialize CMSIS-RTOS                  */
  RTX_Features_Init();             /* initialize threads, events and memory  */
  TIM2_Start();                    /* Start Timer 2 (used for Signal IN/OUT) */  
  osKernelStart();                 /* Start thread execution                 */
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
