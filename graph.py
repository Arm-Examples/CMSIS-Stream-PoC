# Include definitions from the Python package to
# define datatype for the IOs and to have access to the
# Graph class
from cmsisdsp.cg.scheduler import *

# Include definitions from the Python package
from cmsisdsp.cg.scheduler import GenericNode,GenericSink,GenericSource

# When true the graph is just connecting the adc to the dac
LATENCY_STUDY = False

##############################
#
# Define new types of Nodes 
#

class F32TOQ15(GenericNode):
    def __init__(self,name,inLength):
        GenericNode.__init__(self,name)
        self.addInput("i",CType(F32),inLength)
        self.addOutput("o",CType(Q15),inLength)

    @property
    def typeName(self):
        return "F32TOQ15"

class F32TOQ31(GenericNode):
    def __init__(self,name,inLength):
        GenericNode.__init__(self,name)
        self.addInput("i",CType(F32),inLength)
        self.addOutput("o",CType(Q31),inLength)

    @property
    def typeName(self):
        return "F32TOQ31"

class Q15TOF32(GenericNode):
    def __init__(self,name,inLength):
        GenericNode.__init__(self,name)
        self.addInput("i",CType(Q15),inLength)
        self.addOutput("o",CType(F32),inLength)

    @property
    def typeName(self):
        return "Q15TOF32"

class Q31TOF32(GenericNode):
    def __init__(self,name,inLength):
        GenericNode.__init__(self,name)
        self.addInput("i",CType(Q31),inLength)
        self.addOutput("o",CType(F32),inLength)

    @property
    def typeName(self):
        return "Q31TOF32"

class IIR(GenericNode):
    def __init__(self,name,theType,inLength,outLength):
        GenericNode.__init__(self,name)
        self.addInput("i",theType,inLength)
        self.addOutput("o",theType,outLength)

    @property
    def typeName(self):
        return "IIR"

class FIR(GenericNode):
    def __init__(self,name,theType,inLength,outLength):
        GenericNode.__init__(self,name)
        self.addInput("i",theType,inLength)
        self.addOutput("o",theType,outLength)

    @property
    def typeName(self):
        return "FIR"

class DAC(GenericSink):
    def __init__(self,name,inLength):
        GenericSink.__init__(self,name)
        self.addInput("i",CType(F32),inLength)

    @property
    def typeName(self):
        return "DAC"

class ADC(GenericSource):
    def __init__(self,name,outLength):
        GenericSource.__init__(self,name)
        self.addOutput("o",CType(F32),outLength)

    @property
    def typeName(self):
        return "ADC"

##############################
#
# Description of the graph 
#
DSP_BLOCK_SIZE = 256
FILTER_BLOCKSIZE = 192
#FILTER_BLOCKSIZE = 256

# Define the datatype we are using for all the IOs in this
# example
# The DAC / ADC are using float so convertions nodes
# have to be added to the graph when needed
sampleType=CType(Q15)

# Convertion nodes since graph is in Q15
f32_to_q15=F32TOQ15("toQ15",FILTER_BLOCKSIZE)
q15_to_f32=Q15TOF32("toF32",FILTER_BLOCKSIZE)

# The ADC node generating float data (format defined
# in TIMER2_IRQHandler)
adc=ADC("adc",DSP_BLOCK_SIZE)
adc.addVariableArg("dsp_context");

# Filter : can be FIR or IIR.
# Warning, FILTER_BLOCKSIZE value used here must be consistent
# with the one used in C
dsp_filter=IIR("iir",sampleType,FILTER_BLOCKSIZE,FILTER_BLOCKSIZE)
#dsp_filter=FIR("fir",sampleType,FILTER_BLOCKSIZE,FILTER_BLOCKSIZE)

# DAC node consuming float data (format defined in TIMER2_IRQHandler)
dac=DAC("dac",DSP_BLOCK_SIZE)
dac.addVariableArg("dsp_context");

# Create a Graph object
the_graph = Graph()

if LATENCY_STUDY:
    the_graph.connect(adc.o,dac.i)
    # This graph can be used to study the latency
    # between the input / output of the system.
    # It can be used with the square signal
    # in the logic analyzer of uVision.
    # The square signal is defined to change of value every
    # 256 samples so corresponding to one audio buffer
else:
   # Connect the source to the processing node
   the_graph.connect(adc.o,f32_to_q15.i)
   the_graph.connect(f32_to_q15.o,dsp_filter.i)
   
   # Connect the processing node to the sink
   the_graph.connect(dsp_filter.o,q15_to_f32.i)
   the_graph.connect(q15_to_f32.o,dac.i)




##############################
#
# Generation of the code and 
# graphviz representation
#
print("Generate graphviz and code")

conf=Configuration()

conf.cOptionalArgs=["dsp_context_t *dsp_context"]

conf.appNodesCName="DspNodes.h"
conf.customCName="globalCGSettings.h"
conf.schedulerCFileName="dsp_scheduler"
# Prefix for FIFO buffers
conf.prefix="dsp_"
# Name of scheduler function
conf.schedName="dsp_scheduler"

conf.eventRecorder=False


sched = the_graph.computeSchedule(config=conf)

print("Schedule length = %d" % sched.scheduleLength)
print("Memory usage %d bytes" % sched.memory)

sched.ccode("ComputeGraph",conf)

with open("ComputeGraph/dsp.dot","w") as f:
    sched.graphviz(f)
