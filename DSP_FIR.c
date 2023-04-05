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
 * Copyright (c) 2010 - 2016 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

//#include <RTL.h>
#include <math.h>
#include "arm_math.h"
#include "DSP_FIR.h"

#ifdef __FIR

#define F2Q15(x)  ((q15_t)(x *      32768UL))
#define F2Q31(x)  ((q31_t)(x * 2147483648UL))


/*----------------------------------------------------------------------------
   Declare FIR state buffers and structure  
 *---------------------------------------------------------------------------*/

#ifdef __FLOAT32
float32_t             firState_f32[NUMTAPS + DSP_BLOCKSIZE];  
arm_fir_instance_f32  firInstance_f32; 
float32_t             firCoeff_f32[NUMTAPS] =
                                 {
         0.0000000000000000E+00,  /* coefficient of tap     0 */
         0.2482329029589891E-02,  /* coefficient of tap     1 */
         0.0000000000000000E+00,  /* coefficient of tap     2 */
        -0.2923260955139994E-01,  /* coefficient of tap     3 */
        -0.3787793451920152E-01,  /* coefficient of tap     4 */
         0.7545200968161226E-01,  /* coefficient of tap     5 */
         0.2858998239971697E+00,  /* coefficient of tap     6 */
         0.3974407715722918E+00,  /* coefficient of tap     7 */
         0.2858998239971697E+00,  /* coefficient of tap     8 */
         0.7545200968161226E-01,  /* coefficient of tap     9 */
        -0.3787793451920152E-01,  /* coefficient of tap    10 */
        -0.2923260955139994E-01,  /* coefficient of tap    11 */
         0.0000000000000000E+00,  /* coefficient of tap    12 */
         0.2482329029589891E-02,  /* coefficient of tap    13 */
         0.0000000000000000E+00,  /* coefficient of tap    14 */
         0.0
                                  };
#endif

#ifdef __Q31
q31_t                 firState_q31[NUMTAPS + DSP_BLOCKSIZE];  
arm_fir_instance_q31  firInstance_q31; 
q31_t                 firCoeff_q31[NUMTAPS] =
                                 {
  F2Q31( 0.0000000000000000E+00), /* coefficient of tap     0 */
  F2Q31( 0.2482329029589891E-02), /* coefficient of tap     1 */
  F2Q31( 0.0000000000000000E+00), /* coefficient of tap     2 */
  F2Q31(-0.2923260955139994E-01), /* coefficient of tap     3 */
  F2Q31(-0.3787793451920152E-01), /* coefficient of tap     4 */
  F2Q31( 0.7545200968161226E-01), /* coefficient of tap     5 */
  F2Q31( 0.2858998239971697E+00), /* coefficient of tap     6 */
  F2Q31( 0.3974407715722918E+00), /* coefficient of tap     7 */
  F2Q31( 0.2858998239971697E+00), /* coefficient of tap     8 */
  F2Q31( 0.7545200968161226E-01), /* coefficient of tap     9 */
  F2Q31(-0.3787793451920152E-01), /* coefficient of tap    10 */
  F2Q31(-0.2923260955139994E-01), /* coefficient of tap    11 */
  F2Q31( 0.0000000000000000E+00), /* coefficient of tap    12 */
  F2Q31( 0.2482329029589891E-02), /* coefficient of tap    13 */
  F2Q31( 0.0000000000000000E+00), /* coefficient of tap    14 */
  F2Q31( 0.0)
                                  };
#endif

#ifdef __Q15
q15_t                 firState_q15[NUMTAPS + DSP_BLOCKSIZE];  
arm_fir_instance_q15  firInstance_q15; 
q15_t                 firCoeff_q15[NUMTAPS] =
                                 {
  F2Q15( 0.0000000000000000E+00), /* coefficient of tap     0 */
  F2Q15( 0.2482329029589891E-02), /* coefficient of tap     1 */
  F2Q15( 0.0000000000000000E+00), /* coefficient of tap     2 */
  F2Q15(-0.2923260955139994E-01), /* coefficient of tap     3 */
  F2Q15(-0.3787793451920152E-01), /* coefficient of tap     4 */
  F2Q15( 0.7545200968161226E-01), /* coefficient of tap     5 */
  F2Q15( 0.2858998239971697E+00), /* coefficient of tap     6 */
  F2Q15( 0.3974407715722918E+00), /* coefficient of tap     7 */
  F2Q15( 0.2858998239971697E+00), /* coefficient of tap     8 */
  F2Q15( 0.7545200968161226E-01), /* coefficient of tap     9 */
  F2Q15(-0.3787793451920152E-01), /* coefficient of tap    10 */
  F2Q15(-0.2923260955139994E-01), /* coefficient of tap    11 */
  F2Q15( 0.0000000000000000E+00), /* coefficient of tap    12 */
  F2Q15( 0.2482329029589891E-02), /* coefficient of tap    13 */
  F2Q15( 0.0000000000000000E+00), /* coefficient of tap    14 */
  F2Q15( 0.0)
                                  };
#endif


/*----------------------------------------------------------------------------
   Initialize the FIR data structure  
 *---------------------------------------------------------------------------*/
#ifdef __FLOAT32
void firInit_f32 (void) {

  arm_fir_init_f32(&firInstance_f32, NUMTAPS, firCoeff_f32, firState_f32, DSP_BLOCKSIZE); 
}
#endif

#ifdef __Q31
void firInit_q31 (void) {

  arm_fir_init_q31(&firInstance_q31, NUMTAPS, firCoeff_q31, firState_q31, DSP_BLOCKSIZE); 
}
#endif

#ifdef __Q15
void firInit_q15 (void) {

  arm_fir_init_q15(&firInstance_q15, NUMTAPS, firCoeff_q15, firState_q15, DSP_BLOCKSIZE); 
}
#endif

/*----------------------------------------------------------------------------
   Execute the FIR processing function  
 *---------------------------------------------------------------------------*/
#ifdef __FLOAT32
void firExec_f32 (float32_t *pSrc, float32_t *pDst) {

  arm_fir_f32(&firInstance_f32, pSrc, pDst, DSP_BLOCKSIZE); 
}
#endif

#ifdef __Q31
void firExec_q31 (q31_t *pSrc, q31_t *pDst) {

  arm_fir_q31(&firInstance_q31, pSrc, pDst, DSP_BLOCKSIZE); 
}
#endif

#ifdef __Q15
void firExec_q15 (q15_t *pSrc, q15_t *pDst) {

  arm_fir_q15(&firInstance_q15, pSrc, pDst, DSP_BLOCKSIZE); 
}
#endif

#endif // __FIR

