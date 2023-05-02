# Include definitions from the Python package to
# define datatype for the IOs and to have access to the
# Graph class
from cmsisdsp.cg.scheduler import *

# Import definitions used to describe the graph
from nodes import *
from params import * 


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
sampleType=CType(Q15)

# The ADC node generating float data
# (format defined in TIMER2_IRQHandler)
adc=ADC("adc",DSP_BLOCK_SIZE,
        dsp_context_variable="dsp_context")


# DAC node consuming float data 
# (format defined in TIMER2_IRQHandler)
dac=DAC("dac",DSP_BLOCK_SIZE,
        dsp_context_variable="dsp_context")


# Create a Graph object
the_graph = Graph()

the_graph.connect(adc.o,dac.i)


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

with open("Pictures/graph_latency_study.dot","w") as f:
    sched.graphviz(f,config=conf)
