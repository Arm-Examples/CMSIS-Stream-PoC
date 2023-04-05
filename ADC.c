/*----------------------------------------------------------------------------
 * Name:    ADC.c
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
 
 *----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
  Initialize the AD controller in IRQ mode
 *----------------------------------------------------------------------------*/
void ADC_Init (void) {

  LPC_PINCON->PINMODE1 &= ~(3<<18);      /* P0.25                             */
  LPC_PINCON->PINMODE1 |=  (1<<18);      /* has neither pull-up nor pull-down */
  LPC_PINCON->PINSEL1  &= ~(3<<18);      /* P0.25 is GPIO                     */
  LPC_PINCON->PINSEL1  |=  (1<<18);      /* P0.25 is AD0.2                    */

  LPC_SC->PCONP       |=  (1<<12);      /* Enable power to ADC block          */

  LPC_ADC->ADCR        =  (1<< 2) |     /* select AD0.2 pin                   */
                          (4<< 8) |     /* ADC clock is 25MHz/5               */
                          (1<<16) |     /* Burst mode                         */
                          (1<<21);      /* enable ADC                         */ 

//  LPC_ADC->ADINTEN     =  (1<< 8);      /* global enable interrupt            */

//  NVIC_EnableIRQ(ADC_IRQn);             /* enable ADC Interrupt               */
}


/*----------------------------------------------------------------------------
  Start AD Conversion
 *----------------------------------------------------------------------------*/
void ADC_Start (void) {

  LPC_ADC->ADCR |=  (1<<24);            /* Start A/D Conversion               */
}
