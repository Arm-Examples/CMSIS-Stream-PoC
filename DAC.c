/*----------------------------------------------------------------------------
 * Name:    DAC.c
 * Purpose: MCB1700 low level DAC functions
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

#include <LPC17xx.H>                               /* LPC17xx definitions     */
#include "DAC.h"


/*----------------------------------------------------------------------------
   Initialize DAC & DAC pins
 *----------------------------------------------------------------------------*/
void DAC_Init (void) {

  LPC_PINCON->PINSEL1  &= ~(3 << 20);              /* Pin P0.26 used as GPIO */
  LPC_PINCON->PINSEL1  |=  (2 << 20);              /* Pin P0.26 used as AOUT (Speaker) */
  LPC_PINCON->PINMODE1 &= ~(3 << 20);              /* reset Pinmode */
  LPC_PINCON->PINMODE1 |= ~(2 << 20);              /* Pin has neither pull-up nor pull-down */

  LPC_DAC->DACR = 0x8000;                          /* DAC Output set to Middle Point */
}
