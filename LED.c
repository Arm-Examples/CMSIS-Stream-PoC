/*----------------------------------------------------------------------------
 * Name:    LED.c
 * Purpose: MCB1700 low level LED functions
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

#include <LPC17xx.H>                    /* LPC17xx definitions                */
#include "LED.h"

#define LED_NUM     8                   /* Number of user LEDs                */
const uint32_t led_mask[] = { 1UL << 28, 1UL << 29, 1UL << 31, 1UL <<  2, 
                              1UL <<  3, 1UL <<  4, 1UL <<  5, 1UL <<  6 };

/*----------------------------------------------------------------------------
  initialize LED Pins
 *----------------------------------------------------------------------------*/
void LED_Init (void) {
  LPC_GPIO1->FIODIR |= 0xB0000000;                 /* LEDs on PORT1  */
  LPC_GPIO2->FIODIR |= 0x0000007C;                 /* LEDs on PORT2  */
}


/*----------------------------------------------------------------------------
  Switch LED on. LED = Bitposition
 *----------------------------------------------------------------------------*/
void LED_On  (int led) {

  if (led < 3) LPC_GPIO1->FIOSET = led_mask[led];
  else         LPC_GPIO2->FIOSET = led_mask[led];
}

/*----------------------------------------------------------------------------
  Switch Led off. LED = Bitposition
 *----------------------------------------------------------------------------*/
void LED_Off (int led) {

  if (led < 3) LPC_GPIO1->FIOCLR = led_mask[led];
  else         LPC_GPIO2->FIOCLR = led_mask[led];
}

/*----------------------------------------------------------------------------
  Output value to LEDs
 *----------------------------------------------------------------------------*/
void LED_Out (int led) {
  int32_t  i;

  for (i = 0; i < LED_NUM; i++) {
    if (led & (1<<i)) {
      if (i < 3) LPC_GPIO1->FIOSET = led_mask[led];
      else       LPC_GPIO2->FIOSET = led_mask[led];
    }
    else {
      if (i < 3) LPC_GPIO1->FIOCLR = led_mask[led];
      else       LPC_GPIO2->FIOCLR = led_mask[led];
    }
  }
}
