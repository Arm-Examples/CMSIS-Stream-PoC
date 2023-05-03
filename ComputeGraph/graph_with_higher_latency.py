# Include definitions from the Python package to
# define datatype for the IOs and to have access to the
# Graph class
from cmsisdsp.cg.scheduler import *

# Import definitions used to describe the graph
from nodes import *
from params import * 

import argparse

parser = argparse.ArgumentParser(description='Options for graph generation')
parser.add_argument('--fir', action='store_true', help="FIR mode (default IIR)")
parser.add_argument('--f32', action='store_true', help="F32 mode (default Q15)")

args = parser.parse_args()

##############################
#
# Description of the graph 
#
DSP_BLOCK_SIZE = 256
FILTER_BLOCKSIZE = 192
AUDIO_QUEUE_DEPTH = 3
ENERGY_BLOCK_SIZE = 384

write_c_params(dsp_blocksize=DSP_BLOCK_SIZE,
               filter_blocksize = FILTER_BLOCKSIZE,
               audio_queue_depth=AUDIO_QUEUE_DEPTH)

# Define the datatype we are using for all the IOs in this
# example
# The DAC / ADC are using float so conversions nodes
# have to be added to the graph when needed
if args.f32:
   sampleType=CType(F32)
else:
   sampleType=CType(Q15)

# Convertion nodes since graph is in Q15
to_fixed=ToFixedPoint("toFixedPoint",sampleType,FILTER_BLOCKSIZE)
to_float=ToFloat("toFloat",sampleType,FILTER_BLOCKSIZE)

# The ADC node generating float data
# (format defined in TIMER2_IRQHandler)
adc=ADC("adc",DSP_BLOCK_SIZE,
         dsp_context_variable="dsp_context")

# Filter : can be FIR or IIR.
# Warning, FILTER_BLOCKSIZE value used here must be consistent
# with the one used in C
if args.fir:
   dsp_filter=FIR("fir",sampleType,FILTER_BLOCKSIZE)
else:
   dsp_filter=IIR("iir",sampleType,FILTER_BLOCKSIZE)

energy=RMS("rms",sampleType,ENERGY_BLOCK_SIZE)
threshold=Threshold("threshold",sampleType,
                    threshold_value=0x3600)

energy_log = LogicAnalyzer("energy_log",sampleType,
                           c_global_variable="EOUT")

threshold_log = LogicAnalyzer("threshold_log",sampleType,
                              c_global_variable="TOUT")

# DAC node consuming float data 
# (format defined in TIMER2_IRQHandler)
dac=DAC("dac",DSP_BLOCK_SIZE,
         dsp_context_variable="dsp_context")


# Create a Graph object
the_graph = Graph()


# Connect the source to the processing node
if sampleType.ctype != "float32_t":
   the_graph.connect(adc.o,to_fixed.i)
   the_graph.connect(to_fixed.o,dsp_filter.i)
else:
   the_graph.connect(adc.o,dsp_filter.i) 
   
# Connect the processing node to the sink
if sampleType.ctype != "float32_t":
   the_graph.connect(dsp_filter.o,to_float.i)
   the_graph.connect(to_float.o,dac.i)
else:
   the_graph.connect(dsp_filter.o,dac.i)

the_graph.connect(dsp_filter.o,energy.i)
the_graph.connect(energy.o,threshold.i)
the_graph.connect(threshold.o,threshold_log.i)

the_graph.connect(energy.o,energy_log.i)

  
##############################
#
# Generation of the code and 
# graphviz representation
#
print("Generate graphviz and code")

conf=Configuration()

# Argument of the scheduler function.
# Variables used in the initialization of some nodes
conf.cOptionalArgs=["dsp_context_t *dsp_context"]

# Name of files included by the scheduler
conf.appNodesCName="DspNodes.h"
conf.customCName="globalCGSettings.h"
conf.schedulerCFileName="dsp_scheduler"
# Prefix for FIFO buffers
conf.prefix="dsp_"
# Name of scheduler function
conf.schedName="dsp_scheduler"

# Enable event recorder tracing in the scheduler
conf.eventRecorder=True

#conf.dumpSchedule  = True
#conf.displayFIFOBuf = True

sched = the_graph.computeSchedule(config=conf)

print("Schedule length = %d" % sched.scheduleLength)
print("Memory usage %d bytes" % sched.memory)

sched.ccode(".",conf)

with open("Pictures/graph_with_higher_latency.dot","w") as f:
    sched.graphviz(f,config=conf)
