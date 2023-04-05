# Include definitions from the Python package to
# define datatype for the IOs and to have access to the
# Graph class
from cmsisdsp.cg.scheduler import *

# Include definitions from the Python package
from cmsisdsp.cg.scheduler import GenericNode,GenericSink,GenericSource

### Define new types of Nodes 

class FIR(GenericNode):
    """
    Definition of a ProcessingNode for the graph

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

class IIR(GenericNode):
    """
    Definition of a ProcessingNode for the graph

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
    def __init__(self,name,theType,inLength):
        GenericSink.__init__(self,name)
        self.addInput("i",theType,inLength)

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
    def __init__(self,name,theType,outLength):
        GenericSource.__init__(self,name)
        self.addOutput("o",theType,outLength)

    @property
    def typeName(self):
        """The name of the C++ class implementing this node"""
        return "ADC"

DSP_BLOCK_SIZE = 256

# Define the datatype we are using for all the IOs in this
# example
sampleType=CType(Q15)

# Instantiate a Source node with a float datatype and
# working with packet of 5 samples (each execution of the
# source in the C code will generate 5 samples)
# "source" is the name of the C variable that will identify
# this node
adc=ADC("adc",sampleType,DSP_BLOCK_SIZE)
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
dac=DAC("dac",sampleType,DSP_BLOCK_SIZE)
dac.addVariableArg("dsp_context");

# Create a Graph object
the_graph = Graph()

# Connect the source to the processing node
the_graph.connect(adc.o,iir.i)
# Connect the processing node to the sink
the_graph.connect(iir.o,dac.i)


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
