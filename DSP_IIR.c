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
 * Copyright (c) 2010 - 2016 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

//#include <RTL.h>
#include <math.h>
#include "arm_math.h"
#include "DSP_IIR.h"



#ifdef __IIR

#define F2Q31(x)  ((q31_t)((float32_t)x * 2147483648UL))
#define F2Q15(x)  ((q15_t)((float32_t)x *      32768UL))

#define F2Q30(x)  ((q31_t)((float32_t)x * 1073741824UL))
#define F2Q14(x)  ((q15_t)((float32_t)x *      16384UL))


/*----------------------------------------------------------------------------
   Declare IIR state buffers and structure  
 *---------------------------------------------------------------------------*/
#ifdef __FLOAT32
float32_t                     iirState_f32[NUMSTAGES * 4];  
arm_biquad_casd_df1_inst_f32  iirInstance_f32; 
float32_t                     iirCoeff_f32[NUMSTAGES * 5] =
                                 {
         0.4938164725899697E-01,  /* section  1  B0 */
         0.8426557574421167E-01,  /* section  1  B1 */
         0.4938164725899697E-01,  /* section  1  B2 */
         0.1253551801666617E+01,  /* section  1 -A1 */     //  <---------- !! That is the reason why we use q14, q30
        -0.4365806737914682E+00,  /* section  1 -A2 */
         0.1536796065047383E+00,  /* section  2  B0 */
         0.1158730611205101E+00,  /* section  2  B1 */
         0.1536796065047383E+00,  /* section  2  B2 */
         0.1318815267644823E+01,  /* section  2 -A1 */     //  <---------- !!
        -0.7469483343884349E+00   /* section  2 -A2 */
                                 };
#endif

#ifdef __Q31
q31_t                         iirState_q31[NUMSTAGES * 4];  
arm_biquad_casd_df1_inst_q31  iirInstance_q31; 
q31_t                         iirCoeff_q31[NUMSTAGES * 5] =
                                 {
  F2Q30( 0.4938164725899697E-01), /* section  1  B0 */
  F2Q30( 0.8426557574421167E-01), /* section  1  B1 */
  F2Q30( 0.4938164725899697E-01), /* section  1  B2 */
  F2Q30( 0.1253551801666617E+01), /* section  1 -A1 */
  F2Q30(-0.4365806737914682E+00), /* section  1 -A2 */
  F2Q30( 0.1536796065047383E+00), /* section  2  B0 */
  F2Q30( 0.1158730611205101E+00), /* section  2  B1 */
  F2Q30( 0.1536796065047383E+00), /* section  2  B2 */
  F2Q30( 0.1318815267644823E+01), /* section  2 -A1 */
  F2Q30(-0.7469483343884349E+00)  /* section  2 -A2 */
                                 };
#endif

#ifdef __Q15
q15_t                         iirState_q15[NUMSTAGES * 4];  
arm_biquad_casd_df1_inst_q15  iirInstance_q15; 
q15_t                         iirCoeff_q15[NUMSTAGES * 6] =
                                 {
  F2Q14( 0.4938164725899697E-01), /* section  1  B0 */
  F2Q14( 0.0),
  F2Q14( 0.8426557574421167E-01), /* section  1  B1 */
  F2Q14( 0.4938164725899697E-01), /* section  1  B2 */
  F2Q14( 0.1253551801666617E+01), /* section  1 -A1 */
  F2Q14(-0.4365806737914682E+00), /* section  1 -A2 */
  F2Q14( 0.1536796065047383E+00), /* section  2  B0 */
  F2Q14( 0.0),
  F2Q14( 0.1158730611205101E+00), /* section  2  B1 */
  F2Q14( 0.1536796065047383E+00), /* section  2  B2 */
  F2Q14( 0.1318815267644823E+01), /* section  2 -A1 */
  F2Q14(-0.7469483343884349E+00)  /* section  2 -A2 */
                                 };
#endif


/*----------------------------------------------------------------------------
   Initialize the IIR data structure  
 *---------------------------------------------------------------------------*/
#ifdef __FLOAT32
void iirInit_f32 (void) {

  arm_biquad_cascade_df1_init_f32(&iirInstance_f32, NUMSTAGES, iirCoeff_f32, iirState_f32); 
}
#endif

#ifdef __Q31
void iirInit_q31 (void) {

  arm_biquad_cascade_df1_init_q31(&iirInstance_q31, NUMSTAGES, iirCoeff_q31, iirState_q31, 1); 
}
#endif

#ifdef __Q15
void iirInit_q15 (void) {

  arm_biquad_cascade_df1_init_q15(&iirInstance_q15, NUMSTAGES, iirCoeff_q15, iirState_q15, 1); 
}
#endif

/*----------------------------------------------------------------------------
   Execute the IIR processing function  
 *---------------------------------------------------------------------------*/
#ifdef __FLOAT32
void iirExec_f32 (float32_t *pSrc, float32_t *pDst) {

  arm_biquad_cascade_df1_f32(&iirInstance_f32, pSrc, pDst, DSP_BLOCKSIZE); 
}
#endif

#ifdef __Q31
void iirExec_q31 (q31_t *pSrc, q31_t *pDst) {

  arm_biquad_cascade_df1_q31(&iirInstance_q31, pSrc, pDst, DSP_BLOCKSIZE); 
}
#endif
#ifdef __Q15
void iirExec_q15 (q15_t *pSrc, q15_t *pDst) {

  arm_biquad_cascade_df1_q15(&iirInstance_q15, pSrc, pDst, DSP_BLOCKSIZE); 
}
#endif

#endif // __IIR

