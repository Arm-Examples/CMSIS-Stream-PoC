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
 * Copyright (c) 2016 Keil - An ARM Company. All rights reserved.
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

#define EVENT_DATA_TIM_OUT_SIG_IN    1
#define EVENT_DATA_TIM_IN_SIG_OUT    2


/*----------------------------------------------------------------------------
   defines & typedefs for messages
 *----------------------------------------------------------------------------*/
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

osMemoryPoolId_t  DSP_MemPool;
osEventFlagsId_t  DSP_Event;
DSP_DataType      *pDataTimIrqOut;
DSP_DataType      *pDataTimIrqIn;
DSP_DataType      *pDataSigModOut;
DSP_DataType      *pDataSigModIn;
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
  
#ifdef __FLOAT32
  float32_t tmp;
#endif
#ifdef __Q31
  q31_t     tmp;
#endif
#ifdef __Q15
  q15_t     tmp;
#endif

  if (LPC_TIM2->IR & (1UL <<  0)) { /* check MR0 Interrupt                       */

    /* -- signal Input Section ------------------------------------------------- */
    adGdr = LPC_ADC->ADGDR;
    if (adGdr & (1UL << 31)) {			/* Data available ?                          */
      /* scale value and move it in positive/negative range. (12bit Ad = 0xFFF)
         filter in range is -1.0 < value < 1.0                                   */
      tmpFilterIn =  ((float32_t)((adGdr >> 4) & 0xFFF) / (0xFFF / 2)) - 1;
#ifdef __FLOAT32
	    tmp  = tmpFilterIn; 
      pDataTimIrqOut->Sample[dataTimIrqOutIdx++] = tmp;
#endif
#ifdef __Q31
      arm_float_to_q31(&tmpFilterIn, &tmp, 1); 
      pDataTimIrqOut->Sample[dataTimIrqOutIdx++] = tmp;
#endif
#ifdef __Q15
      arm_float_to_q15(&tmpFilterIn, &tmp, 1); 
      pDataTimIrqOut->Sample[dataTimIrqOutIdx++] = tmp;
#endif

      if (dataTimIrqOutIdx >= DSP_BLOCKSIZE) {
        
        // set buffer and event
        EventRecord2(1+EventLevelOp, 0, 0);
        pDataSigModIn = pDataTimIrqOut;
        flags = osEventFlagsSet(DSP_Event, EVENT_DATA_TIM_OUT_SIG_IN);
        if (flags < 0) {
          errHandler(__LINE__);
        } 
        
        // allocate next output buffer
        pDataTimIrqOut = osMemoryPoolAlloc(DSP_MemPool, 0);        
        if (pDataTimIrqOut == NULL) {
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
      flags = osEventFlagsWait(DSP_Event, EVENT_DATA_TIM_IN_SIG_OUT, 0, 0);        
    }
    if ((dataTimIrqInIdx > 0) || (flags==EVENT_DATA_TIM_IN_SIG_OUT)) {

#ifdef __FLOAT32
      tmp = pDataTimIrqIn->Sample[dataTimIrqInIdx++];
      tmpFilterOut = tmp;
#endif
#ifdef __Q31
      tmp = pDataTimIrqIn->Sample[dataTimIrqInIdx++];
      arm_q31_to_float(&tmp, &tmpFilterOut, 1);
#endif
#ifdef __Q15
      tmp = pDataTimIrqIn->Sample[dataTimIrqInIdx++];
      arm_q15_to_float(&tmp, &tmpFilterOut, 1);
#endif
      /* move value in positive range and scale it.   (10bit DA = 0x3FF)
         filter OUT range is -1.0 < value < 1.0                */
      LPC_DAC->DACR = (((uint32_t)((tmpFilterOut + 1) * (0x03FF / 2))) & 0x03FF) <<  6;

      if (dataTimIrqInIdx >= DSP_BLOCKSIZE) { 
        EventRecord2(2+EventLevelOp, 0, 0);
        // free input buffer       
        status = osMemoryPoolFree(DSP_MemPool, pDataTimIrqIn);  
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
  int32_t flags;
  osStatus_t status;

  for (;;) {

    // wait for data
    flags = osEventFlagsWait(DSP_Event, EVENT_DATA_TIM_OUT_SIG_IN, 0, osWaitForever);
    if (flags < 0) {
      errHandler(__LINE__);
    }

#ifdef __IIR
  #ifdef __FLOAT32
    iirExec_f32 (pDataSigModIn->Sample, pDataSigModOut->Sample);
  #endif
  #ifdef __Q31
    iirExec_q31 (pDataSigModIn->Sample, pDataSigModOut->Sample);
  #endif
  #ifdef __Q15
    iirExec_q15 (pDataSigModIn->Sample, pDataSigModOut->Sample);
  #endif
#endif
#ifdef __FIR
  #ifdef __FLOAT32
    firExec_f32 (pDataSigModIn->Sample, pDataSigModOut->Sample);
  #endif
  #ifdef __Q31
    firExec_q31 (pDataSigModIn->Sample, pDataSigModOut->Sample);
  #endif
  #ifdef __Q15
    firExec_q15 (pDataSigModIn->Sample, pDataSigModOut->Sample);
  #endif
#endif

    // free input buffer
    status = osMemoryPoolFree(DSP_MemPool, pDataSigModIn);     
    if (status != osOK) {
      errHandler(__LINE__);
    }
    
    // data is ready
    pDataTimIrqIn = pDataSigModOut;
    flags = osEventFlagsSet(DSP_Event, EVENT_DATA_TIM_IN_SIG_OUT);
    if (flags < 0) {
      errHandler(__LINE__);
    }
    
    // allocate next output buffer
    pDataSigModOut = osMemoryPoolAlloc(DSP_MemPool, 0); 
    if (pDataSigModOut == NULL) {
      errHandler(__LINE__);
    }
  }
}

/*----------------------------------------------------------------------------
   RTX_Features_Init
 *---------------------------------------------------------------------------*/
void RTX_Features_Init (void) {

  DSP_MemPool = osMemoryPoolNew (4, sizeof(DSP_DataType), NULL);
  if (DSP_MemPool == NULL) {
    errHandler(__LINE__);
  }

  DSP_Event = osEventFlagsNew(NULL);
  if (DSP_Event == NULL) {
    errHandler(__LINE__);
  }
  
  tid_SigMod = osThreadNew(SigMod, NULL, &t_attr); /* start task Signal Modify */ 
  
  pDataTimIrqOut = osMemoryPoolAlloc(DSP_MemPool, 0); /* allocate memory       */
  pDataSigModOut = osMemoryPoolAlloc(DSP_MemPool, 0); /* allocate memory       */
  
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

#ifdef __IIR
  #ifdef __FLOAT32
    iirInit_f32 ();
  #endif
  #ifdef __Q31
    iirInit_q31 ();
  #endif
  #ifdef __Q15
    iirInit_q15 ();
  #endif
#endif
#ifdef __FIR
  #ifdef __FLOAT32
    firInit_f32 ();
  #endif
  #ifdef __Q31
    firInit_q31 ();
  #endif
  #ifdef __Q15
    firInit_q15 ();
  #endif
#endif

  osKernelInitialize();            /* Initialize CMSIS-RTOS                  */
  RTX_Features_Init();             /* initialize threads, events and memory  */
  TIM2_Start();                    /* Start Timer 2 (used for Signal IN/OUT) */  
  osKernelStart();                 /* Start thread execution                 */
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
