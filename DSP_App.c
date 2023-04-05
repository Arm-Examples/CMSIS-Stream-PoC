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

#define EVENT_DATA_TIM_OUT_SIG_IN    1
#define EVENT_DATA_TIM_IN_SIG_OUT    2


/*----------------------------------------------------------------------------
   defines & typedefs for messages
 *----------------------------------------------------------------------------*/

dsp_context_t g_dsp_context;

uint32_t          dataTimIrqOutIdx;
uint32_t          dataTimIrqInIdx;
                 
/*----------------------------------------------------------------------------
   TimeStamp source for Event Recorder
 *----------------------------------------------------------------------------*/
static uint32_t TimeStamp;

uint32_t EventRecorderTimerInit (void) {
  TimeStamp = 0U;
  return 0U;
}
 
uint32_t EventRecorderTimerGet (void) {
  return (TimeStamp++);
}

/*----------------------------------------------------------------------------
   Thread IDs
 *----------------------------------------------------------------------------*/
osThreadId_t tid_SigMod;           /* thread id of Signal Modify thread       */

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

/*----------------------------------------------------------------------------
   TIM2 IRQ Handler
 *----------------------------------------------------------------------------*/
void TIMER2_IRQHandler(void) {
  uint32_t adGdr;
  int32_t flags;
  osStatus_t status;
  
  float32_t     tmp;

  if (LPC_TIM2->IR & (1UL <<  0)) { /* check MR0 Interrupt                       */

    /* -- signal Input Section ------------------------------------------------- */
    adGdr = LPC_ADC->ADGDR;
    if (adGdr & (1UL << 31)) {			/* Data available ?                          */
      /* scale value and move it in positive/negative range. (12bit Ad = 0xFFF)
         filter in range is -1.0 < value < 1.0                                   */
      tmpFilterIn =  ((float32_t)((adGdr >> 4) & 0xFFF) / (0xFFF / 2)) - 1;

      tmp = tmpFilterIn;
      g_dsp_context.pDataTimIrqOut->Sample[dataTimIrqOutIdx++] = tmp;

      if (dataTimIrqOutIdx >= DSP_BLOCKSIZE) {
        
        // set buffer and event
        EventRecord2(1+EventLevelOp, 0, 0);
        g_dsp_context.pDataSigModIn = g_dsp_context.pDataTimIrqOut;
        flags = osEventFlagsSet(g_dsp_context.DSP_Event, EVENT_DATA_TIM_OUT_SIG_IN);
        if (flags < 0) {
          errHandler(__LINE__);
        } 
        
        // allocate next output buffer
        g_dsp_context.pDataTimIrqOut = osMemoryPoolAlloc(g_dsp_context.DSP_MemPool, 0);        
        if (g_dsp_context.pDataTimIrqOut == NULL) {
          errHandler(__LINE__);
        }    
        
        dataTimIrqOutIdx = 0;
      }
    }
    else {
      errHandler(__LINE__);
    } 
    
    /* -- signal Output Section ------------------------------------------------ */
    if (dataTimIrqInIdx == 0) {
      // check if data is available
      flags = osEventFlagsWait(g_dsp_context.DSP_Event, EVENT_DATA_TIM_IN_SIG_OUT, 0, 0);        
    }
    if ((dataTimIrqInIdx > 0) || (flags==EVENT_DATA_TIM_IN_SIG_OUT)) {


      tmp = g_dsp_context.pDataTimIrqIn->Sample[dataTimIrqInIdx++];
      tmpFilterOut = tmp;

      /* move value in positive range and scale it.   (10bit DA = 0x3FF)
         filter OUT range is -1.0 < value < 1.0                */
      LPC_DAC->DACR = (((uint32_t)((tmpFilterOut + 1) * (0x03FF / 2))) & 0x03FF) <<  6;

      if (dataTimIrqInIdx >= DSP_BLOCKSIZE) { 
        EventRecord2(2+EventLevelOp, 0, 0);
        // free input buffer       
        status = osMemoryPoolFree(g_dsp_context.DSP_MemPool, g_dsp_context.pDataTimIrqIn);  
        if (status != osOK) {
          errHandler(__LINE__);
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
  //printf("Start compute graph\r\n");
  uint32_t nbIterations = dsp_scheduler(&error,&g_dsp_context);

  //printf("Nb iterations = %d\r\n",nbIterations);
  //printf("Error code = %d\r\n",error);

}

/*----------------------------------------------------------------------------
   RTX_Features_Init
 *---------------------------------------------------------------------------*/
void RTX_Features_Init (void) {

  g_dsp_context.DSP_MemPool = osMemoryPoolNew (4, sizeof(DSP_DataType), NULL);
  if (g_dsp_context.DSP_MemPool == NULL) {
    errHandler(__LINE__);
  }

  g_dsp_context.DSP_Event = osEventFlagsNew(NULL);
  if (g_dsp_context.DSP_Event == NULL) {
    errHandler(__LINE__);
  }
  
  tid_SigMod = osThreadNew(SigMod, NULL, &t_attr); /* start task Signal Modify */ 
  
  g_dsp_context.pDataTimIrqOut = osMemoryPoolAlloc(g_dsp_context.DSP_MemPool, 0); /* allocate memory       */
  g_dsp_context.pDataSigModOut = osMemoryPoolAlloc(g_dsp_context.DSP_MemPool, 0); /* allocate memory       */
  
  dataTimIrqOutIdx = 0;
  dataTimIrqInIdx  = 0;
}

/*----------------------------------------------------------------------------
 *        Main: Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/
int main (void) {

  EventRecorderInitialize(EventRecordAll, 1);
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
