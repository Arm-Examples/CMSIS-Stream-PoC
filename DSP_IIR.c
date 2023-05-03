/*----------------------------------------------------------------------------
 * Name:    DSP_IIR.c
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
#include "DSP_IIR.h"

#define NUMSTAGES       2
/*----------------------------------------------------------------------------
   Declare IIR state buffers and structure  
 *---------------------------------------------------------------------------*/
float32_t                     iirState_f32[NUMSTAGES * 4];  
arm_biquad_casd_df1_inst_f32  iirInstance_f32; 
float32_t                     iirCoeff_f32[NUMSTAGES * 5] =
                                 {
         +0.0099235803,  /* section  1   B0 */
         +0.0150912266,  /* section  1   B1 */
         +0.0099235803,  /* section  1   B2 */
         +1.2472278118,  /* section  1  -A1 */
         -0.4341690797,  /* section  1  -A2 */
         +1.0000000000,  /* section  2   B0 */
         +0.2646127681,  /* section  2   B1 */
         +1.0000000000,  /* section  2   B2 */
         +1.3249359956,  /* section  2  -A1 */
         -0.7530817299,  /* section  2  -A2 */

                                 };

#define F32Q30(x)  ((q31_t)((float32_t)x * 1073741824UL))
#define Q31_POST_SHIFT 1
/*----------------------------------------------------------------------------
   Declare IIR state buffers and structure  
 *---------------------------------------------------------------------------*/
q31_t                     iirState_q31[NUMSTAGES * 4];  
arm_biquad_casd_df1_inst_q31  iirInstance_q31; 
q31_t                     iirCoeff_q31[NUMSTAGES * 5] =
                                 {
         F32Q30(+0.0099235803),  /* section  1   B0 */
         F32Q30(+0.0150912266),  /* section  1   B1 */
         F32Q30(+0.0099235803),  /* section  1   B2 */
         F32Q30(+1.2472278118),  /* section  1  -A1 */
         F32Q30(-0.4341690797),  /* section  1  -A2 */
         F32Q30(+1.0000000000),  /* section  2   B0 */
         F32Q30(+0.2646127681),  /* section  2   B1 */
         F32Q30(+1.0000000000),  /* section  2   B2 */
         F32Q30(+1.3249359956),  /* section  2  -A1 */
         F32Q30(-0.7530817299),  /* section  2  -A2 */

                                 };

#define F32Q14(x)  ((q15_t)((float32_t)x * 16384UL))
#define Q15_POST_SHIFT 1
/*----------------------------------------------------------------------------
   Declare IIR state buffers and structure  
 *---------------------------------------------------------------------------*/
q15_t                     iirState_q15[NUMSTAGES * 4];  
arm_biquad_casd_df1_inst_q15  iirInstance_q15; 
q15_t                     iirCoeff_q15[NUMSTAGES * 6] =
                                 {
         F32Q14(+0.0099235803),  /* section  1   B0 */
         F32Q14(+0.0000000000),  
         F32Q14(+0.0150912266),  /* section  1   B1 */
         F32Q14(+0.0099235803),  /* section  1   B2 */
         F32Q14(+1.2472278118),  /* section  1  -A1 */
         F32Q14(-0.4341690797),  /* section  1  -A2 */
         F32Q14(+1.0000000000),  /* section  2   B0 */
         F32Q14(+0.0000000000),  
         F32Q14(+0.2646127681),  /* section  2   B1 */
         F32Q14(+1.0000000000),  /* section  2   B2 */
         F32Q14(+1.3249359956),  /* section  2  -A1 */
         F32Q14(-0.7530817299),  /* section  2  -A2 */

                                 };

/*----------------------------------------------------------------------------
   Initialize the IIR data structure  
 *---------------------------------------------------------------------------*/
void iirInit_f32 (void) {

  arm_biquad_cascade_df1_init_f32(&iirInstance_f32, NUMSTAGES, iirCoeff_f32, iirState_f32); 
}

void iirInit_q31 (void) {

  arm_biquad_cascade_df1_init_q31(&iirInstance_q31, NUMSTAGES, iirCoeff_q31, iirState_q31, Q31_POST_SHIFT); 
}

void iirInit_q15 (void) {

  arm_biquad_cascade_df1_init_q15(&iirInstance_q15, NUMSTAGES, iirCoeff_q15, iirState_q15, Q15_POST_SHIFT); 
}

/*----------------------------------------------------------------------------
   Execute the IIR processing function  
 *---------------------------------------------------------------------------*/
void iirExec_f32 (float32_t *pSrc, float32_t *pDst) {

  arm_biquad_cascade_df1_f32(&iirInstance_f32, pSrc, pDst, FILTER_BLOCKSIZE); 
}

void iirExec_q31 (q31_t *pSrc, q31_t *pDst) {

  arm_biquad_cascade_df1_q31(&iirInstance_q31, pSrc, pDst, FILTER_BLOCKSIZE); 
}


void iirExec_q15 (q15_t *pSrc, q15_t *pDst) {

  arm_biquad_cascade_df1_q15(&iirInstance_q15, pSrc, pDst, FILTER_BLOCKSIZE); 
}



