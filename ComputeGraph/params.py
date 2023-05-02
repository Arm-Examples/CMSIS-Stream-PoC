settings = """#ifndef _CPARAMS_H_
#define _CPARAMS_H_ 

// Generated from Python. Don't edit
#define DSP_BLOCKSIZE %d
#define FILTER_BLOCKSIZE %d
#define AUDIO_QUEUE_DEPTH %d

#endif
"""

def write_c_params(dsp_blocksize=128,filter_blocksize=128,audio_queue_depth=2):
    if audio_queue_depth < 2:
        audio_queue_depth = 2 

    with open("params.h","w") as f:
        print(settings % (dsp_blocksize,filter_blocksize,audio_queue_depth), file = f)
