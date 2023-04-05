/*----------------------------------------------------------------------------
 * Name:    DSP_FIR.h
 * Purpose: 
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2010 - 2023 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#ifndef _FIR_FUNC_H_
#define _FIR_FUNC_H_ 

#include "globalCGSettings.h"

#define NUMTAPS        16

extern void firInit_f32 (void);
extern void firExec_f32 (float32_t *pSrc, float32_t *pDst);


extern void firInit_q31 (void);
extern void firExec_q31 (q31_t *pSrc, q31_t *pDst);

extern void firInit_q15 (void);
extern void firExec_q15 (q15_t *pSrc, q15_t *pDst);


#endif // __FIR
