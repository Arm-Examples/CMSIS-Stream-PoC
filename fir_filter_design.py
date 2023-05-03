import numpy as np
from scipy import signal
import matplotlib.pyplot as plt
import matplotlib.ticker
import argparse

parser = argparse.ArgumentParser(description='Generate FIR coefficients')
parser.add_argument('-p', action='store_true', help="Plot transfer function")

args = parser.parse_args()


sampling_frequency = 32000
passband_edge_frequency = 3200 
stopband_edge_frequency = 9600 
gpass = 0.1
gstop = 60
nb_taps = 15

sweep_low = 2500 
sweep_high = 9700
sweep_middle = (sweep_high + sweep_low) / 2.0

wp = passband_edge_frequency / (sampling_frequency / 2.0)
ws = stopband_edge_frequency / (sampling_frequency / 2.0)


header = """/*----------------------------------------------------------------------------
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

#define NUMTAPS       %d"""

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

f32_firdef = """float32_t             firState_f32[NUMTAPS + FILTER_BLOCKSIZE];  
arm_fir_instance_f32  firInstance_f32; 
float32_t             firCoeff_f32[NUMTAPS] =
                                 {
%s
                                 };
"""

q15_firdef = """q15_t                 firState_q15[NUMTAPS + FILTER_BLOCKSIZE];  
arm_fir_instance_q15  firInstance_q15; 
q15_t                 firCoeff_q15[NUMTAPS] =
                                 {
%s
                                 };
"""

q31_firdef = """q31_t                 firState_q31[NUMTAPS + FILTER_BLOCKSIZE];  
arm_fir_instance_q31  firInstance_q31; 
q31_t                 firCoeff_q31[NUMTAPS] =
                                 {
%s
                                 };
"""

end_of_file="""/*----------------------------------------------------------------------------
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
    tap_nb = 0
    for tap in coef:
        yield(f"         {get_value(dt,shift,tap)},  /* coefficient of tap  {tap_nb} */\n")
        tap_nb = tap_nb + 1
        

def gen_c_code(dt,coef_list):
    max_value = np.max(np.abs(np.array(coef_list)))
    coef = iter(coef_list)
    shift = compute_normalization(max_value)
    code = gen_c_array(dt,shift,coef)
    return(shift,"".join(code))


if args.p:
   taps = signal.remez(nb_taps, bands=[0,wp,ws,1.0],desired=[1,0],fs=2.0)
   #system = signal.iirdesign(wp, ws, gpass, gstop,ftype=iir_type)
   #w, h = signal.freqz(taps, [1], worN=2000, fs=sampling_frequency)

   w, h = signal.freqz(taps, [1])

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
   coefs = signal.remez(nb_taps, bands=[0,wp,ws,1.0],desired=[1,0],fs=2.0)
   
   
   with open("DSP_FIR.c","w") as f:
        print(header % nb_taps,file=f)
        
        shift, f32_code = gen_c_code(F32,coefs)
        print(f32_firdef % f32_code,file=f)
        
        shift,q31_code = gen_c_code(Q31,coefs)
        print(q31_conversion_macro_definition(shift),file=f)
        print(q31_firdef % q31_code,file=f)
        
        shift,q15_code = gen_c_code(Q15,coefs)
        print(q15_conversion_macro_definition(shift),file=f)
        print(q15_firdef % q15_code,file=f)
        
        print(end_of_file,file=f)   