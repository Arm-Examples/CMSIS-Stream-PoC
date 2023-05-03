import numpy as np
from scipy import signal
import matplotlib.pyplot as plt
import matplotlib.ticker
import argparse

parser = argparse.ArgumentParser(description='Generate IIR coefficients')
parser.add_argument('-p', action='store_true', help="Plot transfer function")

args = parser.parse_args()


sampling_frequency = 32000
passband_edge_frequency = 3200 
stopband_edge_frequency = 9600 
gpass = 0.1
gstop = 60
iir_type = 'ellip'


sweep_low = 2500 
sweep_high = 9700
sweep_middle = (sweep_high + sweep_low) / 2.0

wp = passband_edge_frequency / (sampling_frequency / 2.0)
ws = stopband_edge_frequency / (sampling_frequency / 2.0)


header = """/*----------------------------------------------------------------------------
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

#define NUMSTAGES       %d"""

q31_conversion_macro="""#define F32Q%s(x)  ((q31_t)((float32_t)x * %dUL))
#define Q31_POST_SHIFT %d"""
q15_conversion_macro="""#define F32Q%s(x)  ((q15_t)((float32_t)x * %dUL))
#define Q15_POST_SHIFT %d"""

def q31_macro_name(nb):
    q = 31 - nb
    return(f"F32Q{q:02d}")

def q15_macro_name(nb):
    q = 15 - nb
    return(f"F32Q{q:02d}")

def q31_conversion_macro_definition(nb):
    q = 31 - nb
    val = (2**31) >> nb
    return(q31_conversion_macro % (f"{q:02d}",val,nb))

def q15_conversion_macro_definition(nb):
    q = 15 - nb
    val = (2**15) >> nb
    return(q15_conversion_macro % (f"{q:02d}",val,nb))

f32_iirdef = """/*----------------------------------------------------------------------------
   Declare IIR state buffers and structure  
 *---------------------------------------------------------------------------*/
float32_t                     iirState_f32[NUMSTAGES * 4];  
arm_biquad_casd_df1_inst_f32  iirInstance_f32; 
float32_t                     iirCoeff_f32[NUMSTAGES * 5] =
                                 {
%s
                                 };
"""

q15_iirdef = """/*----------------------------------------------------------------------------
   Declare IIR state buffers and structure  
 *---------------------------------------------------------------------------*/
q15_t                     iirState_q15[NUMSTAGES * 4];  
arm_biquad_casd_df1_inst_q15  iirInstance_q15; 
q15_t                     iirCoeff_q15[NUMSTAGES * 6] =
                                 {
%s
                                 };
"""

q31_iirdef = """/*----------------------------------------------------------------------------
   Declare IIR state buffers and structure  
 *---------------------------------------------------------------------------*/
q31_t                     iirState_q31[NUMSTAGES * 4];  
arm_biquad_casd_df1_inst_q31  iirInstance_q31; 
q31_t                     iirCoeff_q31[NUMSTAGES * 5] =
                                 {
%s
                                 };
"""

end_of_file="""/*----------------------------------------------------------------------------
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


"""

F32 = 0
Q31 = 1 
Q15 = 2 

def compute_normalization(x):
    nb = 0 
    while(x>=1.0):
        x = x / 2
        nb = nb + 1 
    return(nb)

def get_value(dt,shift,val):
    if dt == F32:
        return(f"{val:+0.10f}")
    if dt == Q31:
        return(f"{q31_macro_name(shift)}({val:+0.10f})")
    if dt == Q15:
        return(f"{q15_macro_name(shift)}({val:+0.10f})")

def gen_c_array(dt,shift,coef):
    section = 1
    while True:
        try:
            val = next(coef)
            yield(f"         {get_value(dt,shift,val)},  /* section  {section}   B0 */\n")
            
            if (dt == Q15):
                yield(f"         {get_value(dt,shift,0)},  \n")

            val = next(coef)
            yield(f"         {get_value(dt,shift,val)},  /* section  {section}   B1 */\n")
            
            val = next(coef)
            yield(f"         {get_value(dt,shift,val)},  /* section  {section}   B2 */\n")
            
            val = next(coef)
            yield(f"         {get_value(dt,shift,val)},  /* section  {section}  -A1 */\n")
            
            val = next(coef)
            yield(f"         {get_value(dt,shift,val)},  /* section  {section}  -A2 */\n")
            section = section + 1
        except StopIteration:
            break

def gen_c_code(dt,coef_list):
    max_value = np.max(np.abs(np.array(coef_list)))
    coef = iter(coef_list)
    shift = compute_normalization(max_value)
    code = gen_c_array(dt,shift,coef)
    return(shift,"".join(code))


if args.p:
   system = signal.iirdesign(wp, ws, gpass, gstop,ftype=iir_type)
   w, h = signal.freqz(*system)
   
   fig, ax1 = plt.subplots()
   ax1.set_title('Digital filter frequency response')
   ax1.plot(w, 20 * np.log10(abs(h)), 'b')
   ax1.set_ylabel('Amplitude [dB]', color='b')
   ax1.set_xlabel('Frequency [rad/sample]')
   ax1.grid(True)
   ax1.set_ylim([-120, 20])

   # Draw axis for frequency in the middle of the sine sweep
   x = 2.0*np.pi * sweep_middle / sampling_frequency
   ax1.axvline(x , color = 'r')

   x = 2.0*np.pi * sweep_low / sampling_frequency
   ax1.axvline(x , color = 'y')

   x = 2.0*np.pi * sweep_high / sampling_frequency
   ax1.axvline(x , color = 'y')
   
   

   ax2 = ax1.twinx()
   angles = np.unwrap(np.angle(h))
   ax2.plot(w, angles, 'g')
   ax2.set_ylabel('Angle (radians)', color='g')
   ax2.grid(True)
   ax2.axis('tight')
   ax2.set_ylim([-6, 1])
   nticks = 8
   ax1.yaxis.set_major_locator(matplotlib.ticker.LinearLocator(nticks))
   ax2.yaxis.set_major_locator(matplotlib.ticker.LinearLocator(nticks))
   
   plt.show()

else:
   sos = signal.iirdesign(wp, ws, gpass, gstop,ftype=iir_type,output='sos')
   num_stages = len(sos)
   #print(f"Num stages = {num_stages}")
   nb_coefficients = num_stages*5
   
   
   coefs=np.reshape(np.hstack((sos[:,:3],-sos[:,4:])),nb_coefficients)
   
   with open("DSP_IIR.c","w") as f:
        print(header % num_stages,file=f)
        
        shift, f32_code = gen_c_code(F32,coefs)
        print(f32_iirdef % f32_code,file=f)
        
        shift,q31_code = gen_c_code(Q31,coefs)
        print(q31_conversion_macro_definition(shift),file=f)
        print(q31_iirdef % q31_code,file=f)
        
        shift,q15_code = gen_c_code(Q15,coefs)
        print(q15_conversion_macro_definition(shift),file=f)
        print(q15_iirdef % q15_code,file=f)
        
        print(end_of_file,file=f)   