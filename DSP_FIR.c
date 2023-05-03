/*----------------------------------------------------------------------------
 * Name:    DSP_FIR.c
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

//#include <RTL.h>
#include <math.h>
#include "arm_math.h"
#include "DSP_FIR.h"

#define NUMTAPS       15
float32_t             firState_f32[NUMTAPS + FILTER_BLOCKSIZE];  
arm_fir_instance_f32  firInstance_f32; 
float32_t             firCoeff_f32[NUMTAPS] =
                                 {
         +0.0035982208,  /* coefficient of tap  0 */
         +0.0114398783,  /* coefficient of tap  1 */
         -0.0021152611,  /* coefficient of tap  2 */
         -0.0426875080,  /* coefficient of tap  3 */
         -0.0426658245,  /* coefficient of tap  4 */
         +0.0840898862,  /* coefficient of tap  5 */
         +0.2911828648,  /* coefficient of tap  6 */
         +0.3961329755,  /* coefficient of tap  7 */
         +0.2911828648,  /* coefficient of tap  8 */
         +0.0840898862,  /* coefficient of tap  9 */
         -0.0426658245,  /* coefficient of tap  10 */
         -0.0426875080,  /* coefficient of tap  11 */
         -0.0021152611,  /* coefficient of tap  12 */
         +0.0114398783,  /* coefficient of tap  13 */
         +0.0035982208,  /* coefficient of tap  14 */

                                 };

#define F32Q31(x)  ((q31_t)((float32_t)x * 2147483648UL))
#define Q31_POST_SHIFT 0
q31_t                 firState_q31[NUMTAPS + FILTER_BLOCKSIZE];  
arm_fir_instance_q31  firInstance_q31; 
q31_t                 firCoeff_q31[NUMTAPS] =
                                 {
         F32Q31(+0.0035982208),  /* coefficient of tap  0 */
         F32Q31(+0.0114398783),  /* coefficient of tap  1 */
         F32Q31(-0.0021152611),  /* coefficient of tap  2 */
         F32Q31(-0.0426875080),  /* coefficient of tap  3 */
         F32Q31(-0.0426658245),  /* coefficient of tap  4 */
         F32Q31(+0.0840898862),  /* coefficient of tap  5 */
         F32Q31(+0.2911828648),  /* coefficient of tap  6 */
         F32Q31(+0.3961329755),  /* coefficient of tap  7 */
         F32Q31(+0.2911828648),  /* coefficient of tap  8 */
         F32Q31(+0.0840898862),  /* coefficient of tap  9 */
         F32Q31(-0.0426658245),  /* coefficient of tap  10 */
         F32Q31(-0.0426875080),  /* coefficient of tap  11 */
         F32Q31(-0.0021152611),  /* coefficient of tap  12 */
         F32Q31(+0.0114398783),  /* coefficient of tap  13 */
         F32Q31(+0.0035982208),  /* coefficient of tap  14 */

                                 };

#define F32Q15(x)  ((q15_t)((float32_t)x * 32768UL))
#define Q15_POST_SHIFT 0
q15_t                 firState_q15[NUMTAPS + FILTER_BLOCKSIZE];  
arm_fir_instance_q15  firInstance_q15; 
q15_t                 firCoeff_q15[NUMTAPS] =
                                 {
         F32Q15(+0.0035982208),  /* coefficient of tap  0 */
         F32Q15(+0.0114398783),  /* coefficient of tap  1 */
         F32Q15(-0.0021152611),  /* coefficient of tap  2 */
         F32Q15(-0.0426875080),  /* coefficient of tap  3 */
         F32Q15(-0.0426658245),  /* coefficient of tap  4 */
         F32Q15(+0.0840898862),  /* coefficient of tap  5 */
         F32Q15(+0.2911828648),  /* coefficient of tap  6 */
         F32Q15(+0.3961329755),  /* coefficient of tap  7 */
         F32Q15(+0.2911828648),  /* coefficient of tap  8 */
         F32Q15(+0.0840898862),  /* coefficient of tap  9 */
         F32Q15(-0.0426658245),  /* coefficient of tap  10 */
         F32Q15(-0.0426875080),  /* coefficient of tap  11 */
         F32Q15(-0.0021152611),  /* coefficient of tap  12 */
         F32Q15(+0.0114398783),  /* coefficient of tap  13 */
         F32Q15(+0.0035982208),  /* coefficient of tap  14 */

                                 };

/*----------------------------------------------------------------------------
   Initialize the FIR data structure  
 *---------------------------------------------------------------------------*/
void firInit_f32 (void) {

  arm_fir_init_f32(&firInstance_f32, NUMTAPS, firCoeff_f32, firState_f32, FILTER_BLOCKSIZE); 
}

void firInit_q31 (void) {

  arm_fir_init_q31(&firInstance_q31, NUMTAPS, firCoeff_q31, firState_q31, FILTER_BLOCKSIZE); 
}

void firInit_q15 (void) {

  arm_fir_init_q15(&firInstance_q15, NUMTAPS, firCoeff_q15, firState_q15, FILTER_BLOCKSIZE); 
}

/*----------------------------------------------------------------------------
   Execute the FIR processing function  
 *---------------------------------------------------------------------------*/
void firExec_f32 (float32_t *pSrc, float32_t *pDst) {

  arm_fir_f32(&firInstance_f32, pSrc, pDst, FILTER_BLOCKSIZE); 
}

void firExec_q31 (q31_t *pSrc, q31_t *pDst) {

  arm_fir_q31(&firInstance_q31, pSrc, pDst, FILTER_BLOCKSIZE); 
}

void firExec_q15 (q15_t *pSrc, q15_t *pDst) {

  arm_fir_q15(&firInstance_q15, pSrc, pDst, FILTER_BLOCKSIZE); 
}

