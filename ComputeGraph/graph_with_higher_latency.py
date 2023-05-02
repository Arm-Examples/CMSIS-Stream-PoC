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

# Convertion nodes since graph is in Q15
f32_to_q15=F32TOQ15("toQ15",FILTER_BLOCKSIZE)
q15_to_f32=Q15TOF32("toF32",FILTER_BLOCKSIZE)

# The ADC node generating float data
# (format defined in TIMER2_IRQHandler)
adc=ADC("adc",DSP_BLOCK_SIZE,
         dsp_context_variable="dsp_context")

# Filter : can be FIR or IIR.
# Warning, FILTER_BLOCKSIZE value used here must be consistent
# with the one used in C
dsp_filter=IIR("iir",sampleType,FILTER_BLOCKSIZE)
#dsp_filter=FIR("fir",sampleType,FILTER_BLOCKSIZE)

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
the_graph.connect(adc.o,f32_to_q15.i)
the_graph.connect(f32_to_q15.o,dsp_filter.i)
   
# Connect the processing node to the sink
the_graph.connect(dsp_filter.o,q15_to_f32.i)
the_graph.connect(q15_to_f32.o,dac.i)

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
