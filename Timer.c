/*----------------------------------------------------------------------------
 * Name:    Timer.c
 * Purpose: 
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2010 - 2016 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include "LPC17xx.h"                    /* LPC17xx definitions                */


/*----------------------------------------------------------------------------
 Define Timer  settings
 *----------------------------------------------------------------------------*/
#define __TIMXCLK             (SystemCoreClock / 4)
#define __MRx(__CLK, __FREQ)  ((__CLK / __FREQ) - 1)


/*----------------------------------------------------------------------------
   Timer 2 Initialization                               (PCLK1 36MHz)
 *----------------------------------------------------------------------------*/
void TIM2_Init (uint32_t outFreq) {

  LPC_SC->PCLKSEL1 &= ~(3UL << 12);                  /* Timer2 clock = CCLK / 4     */  
  LPC_SC->PCONP     |= (1UL << 22);                  /* enable Timer 2 clock        */

  LPC_TIM2->CTCR  =  0;                              /* select Timer Mode           */

  LPC_TIM2->MR0   = __MRx(__TIMXCLK, outFreq);       /* set prescaler               */
  LPC_TIM2->MCR   = ((1UL <<  0) |                   /* enable Interrupt on MR0     */
                     (1UL <<  1) );                  /* enable Reset on MR0         */

  NVIC_EnableIRQ(TIMER2_IRQn);                       /* enable Timer 2 interrupt    */
}

/*----------------------------------------------------------------------------
   TIM2 start
 *----------------------------------------------------------------------------*/
void TIM2_Start (void) {

  LPC_TIM2->TCR  |=  (1UL <<  0);                    /* enable Timer                */
}
