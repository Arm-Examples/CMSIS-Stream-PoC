# Include definitions from the Python package to
# define datatype for the IOs and to have access to the
# Graph class
from cmsisdsp.cg.scheduler import *

# Include definitions from the Python package
from cmsisdsp.cg.scheduler import GenericNode,GenericSink,GenericSource

### Define new types of Nodes 

class F32TOQ15(GenericNode):
    def __init__(self,name,inLength):
        GenericNode.__init__(self,name)
        self.addInput("i",CType(F32),inLength)
        self.addOutput("o",CType(Q15),inLength)

    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "F32TOQ15"

class F32TOQ31(GenericNode):
    def __init__(self,name,inLength):
        GenericNode.__init__(self,name)
        self.addInput("i",CType(F32),inLength)
        self.addOutput("o",CType(Q31),inLength)

    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "F32TOQ31"

class Q15TOF32(GenericNode):
    def __init__(self,name,inLength):
        GenericNode.__init__(self,name)
        self.addInput("i",CType(Q15),inLength)
        self.addOutput("o",CType(F32),inLength)

    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "Q15TOF32"

class Q31TOF32(GenericNode):
    def __init__(self,name,inLength):
        GenericNode.__init__(self,name)
        self.addInput("i",CType(Q31),inLength)
        self.addOutput("o",CType(F32),inLength)

    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "Q31TOF32"

class IIR(GenericNode):
    """
    Definition of an IIR node for the graph

    Parameters
    ----------
    name : str
         Name of the C variable identifying this node 
         in the C code
    theType : CGStaticType
            The datatype for the input and output
    inLength : int
             The number of samples consumed by input
    outLength : int 
              The number of samples produced on output
    """
    def __init__(self,name,theType,inLength,outLength):
        GenericNode.__init__(self,name)
        self.addInput("i",theType,inLength)
        self.addOutput("o",theType,outLength)

    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "IIR"

class FIR(GenericNode):
    """
    Definition of an FIR node for the graph

    Parameters
    ----------
    name : str
         Name of the C variable identifying this node 
         in the C code
    theType : CGStaticType
            The datatype for the input and output
    inLength : int
             The number of samples consumed by input
    outLength : int 
              The number of samples produced on output
    """
    def __init__(self,name,theType,inLength,outLength):
        GenericNode.__init__(self,name)
        self.addInput("i",theType,inLength)
        self.addOutput("o",theType,outLength)

    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "FIR"

class DAC(GenericSink):
    """
    Definition of a Sink node for the graph

    Parameters
    ----------
    name : str
         Name of the C variable identifying this node 
         in the C code
    theType : CGStaticType
            The datatype for the input
    inLength : int
             The number of samples consumed by input
    """
    def __init__(self,name,inLength):
        GenericSink.__init__(self,name)
        self.addInput("i",CType(F32),inLength)

    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "DAC"

class ADC(GenericSource):
    """
    Definition of a Source node for the graph

    Parameters
    ----------
    name : str
         Name of the C variable identifying this node 
         in the C code
    theType : CGStaticType
            The datatype for the output
    outLength : int 
              The number of samples produced on output
    """
    def __init__(self,name,outLength):
        GenericSource.__init__(self,name)
        self.addOutput("o",CType(F32),outLength)

    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "ADC"

DSP_BLOCK_SIZE = 256

# Define the datatype we are using for all the IOs in this
# example
sampleType=CType(Q15)

f32_to_q15=F32TOQ15("toQ15",DSP_BLOCK_SIZE)
q15_to_f32=Q15TOF32("toF32",DSP_BLOCK_SIZE)

# Instantiate a Source node with a float datatype and
# working with packet of 5 samples (each execution of the
# source in the C code will generate 5 samples)
# "source" is the name of the C variable that will identify
# this node
adc=ADC("adc",DSP_BLOCK_SIZE)
adc.addVariableArg("dsp_context");
# Instantiate a Processing node using a float data type for
# both the input and output. The number of samples consumed
# on the input and produced on the output is 7 each time
# the node is executed in the C code
# "processing" is the name of the C variable that will identify
# this node
iir=IIR("iir",sampleType,DSP_BLOCK_SIZE,DSP_BLOCK_SIZE)
# Instantiate a Sink node with a float datatype and consuming
# 5 samples each time the node is executed in the C code
# "sink" is the name of the C variable that will identify
# this node
dac=DAC("dac",DSP_BLOCK_SIZE)
dac.addVariableArg("dsp_context");

# Create a Graph object
the_graph = Graph()

# Connect the source to the processing node
the_graph.connect(adc.o,f32_to_q15.i)
the_graph.connect(f32_to_q15.o,iir.i)

# Connect the processing node to the sink
the_graph.connect(iir.o,q15_to_f32.i)
the_graph.connect(q15_to_f32.o,dac.i)



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
