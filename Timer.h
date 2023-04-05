/*----------------------------------------------------------------------------
 * Name:    Timer.h
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
#ifndef __TIMER_H__
#define __TIMER_H__



extern void TIM2_Init (uint32_t outFreq);
extern void TIM2_Start(void);

extern void TIM3_Init (uint32_t outFreq);
extern void TIM3_Start(void);


#endif // __TIMER_H__
