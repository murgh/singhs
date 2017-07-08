#include "timer.hxx"

int verbose = 0;
std::map<std::string, timerClock *> timerConstraints::theClockMap;
void perform_timing_analysis (diganaGraph * graph) {
	TA_Timer * timer = new TA_Timer (graph);
	timer->TA_enumerate_paths ();
}

timerPinProperty getPinProp (diganaVertex & vertex) {
	return vertex.get_property<timerPinProperty> ("Pin_Property");
}

//Given a verilog read, convert the graph into a timing graph
diganaGraph *
TA_Timer::TA_create_timing_graph (diganaGraph * graph) {
	if (verbose) printf ("Creating the timing graph \n");
	diganaGraphIterator::vertex_iterator vitr, eVitr;
	vitr.attach (graph);
	for (; vitr != eVitr; ++vitr) {
	   diganaVertex vtx = *vitr;
	   timerPinProperty prop = getPinProp (vtx);
	   timerPinInfo * pinInfo = prop.getPinInfo ();
	   if (pinInfo->getIdentity () == timerPinVirtualNode)
	   {
	     if (pinInfo->getDirection () == timerInput)
	       theInVirtualNode = vtx;
	     else
	       theOutVirtualNode = vtx;
	   }
        }

	vitr.attach (graph);
	for (; vitr != eVitr; ++vitr) {
	   diganaVertex vtx = *vitr;
	   timerPinProperty prop = getPinProp (vtx);
	   timerPinInfo * pinInfo = prop.getPinInfo ();
	   if (pinInfo->getIdentity () == timerIOPort)
	   {
	     int source = -1, sink = -1;
	     if (pinInfo->getDirection () == timerInput)
	     {
   	       if (verbose) printf ("\nStart IO Port\n");
	       source = theInVirtualNode.getVertexId ();
	       sink = vtx.getVertexId ();
	       if (verbose) pinInfo->print ();
	     }
	     else
	     {
   	       if (verbose) printf ("\nEnd IO Port\n");
	       source = vtx.getVertexId ();
	       sink = theOutVirtualNode.getVertexId ();
	       if (verbose) pinInfo->print ();
	     }
	     if (verbose) printf ("%d %d\n", source, sink);
	     graph->add_edge (source, sink);
	   }
	   if (pinInfo->getIdentity () == timerLatchData)
           {
	     int source = -1, sink = -1;
	     if (pinInfo->getDirection () == timerInput)
	     {
	       //End Point
   	       if (verbose) printf ("\nEnd latch pin\n");
	       source = vtx.getVertexId ();
               sink = theOutVirtualNode.getVertexId ();
	       if (verbose) pinInfo->print ();
	     }
	     else
	     {
	       //Start Point
   	       if (verbose) printf ("\nStart latch pin\n");
	       source = theInVirtualNode.getVertexId ();
               sink = vtx.getVertexId ();
	       if (verbose) pinInfo->print ();
	     }
	     if (verbose) printf ("%d %d\n", source, sink);
	     graph->add_edge (source, sink);
	   }		   
	}
   	if (verbose) printf ("End Creating the timing graph \n");
}

//Check and perform the tag splitting.
void
TA_Timer::checkAndPerformTagSplitting (diganaVertex & sourcePin, bool isClock) {
	diganaGraph * circuit = sourcePin.getParentGraph ();
	diganaGraphIterator::adjacency_iterator ai , aietr;
	diganaVertex adjPin;
	int count = 0;
	ai.attach (sourcePin.getVertexId (), circuit);
	for (; ai != aietr && count <= 1 ; ++ai, ++count);
	if (count == 1) 
	  return;//No tag splitting is needed
	timerPinInfo * srcPinInfo = getPinInfo (sourcePin);
	timerPinInfo * sinkPinInfo;
	srcPinInfo->setTagSplitPoint ();
	ai.attach (sourcePin.getVertexId (), circuit);
	for (; ai != aietr; ++ai) {
           //Create a new tag on each of the sink pins
	   diganaVertex sinkPin = *ai;
	   sinkPinInfo = getPinInfo (sinkPin);
	   timerPinTagContainer * masterTag = sinkPinInfo->get_pin_tag_container ();
	   timerPinTag * tag = new timerPinTag (isClock, true, sourcePin.getVertexId ());	
	   tag->setMasterTag (masterTag);
	   sinkPinInfo->assert_pin_tag (tag);
	}
}

void
TA_Timer::mergePropagatedTags (diganaVertex & sinkPin) {

}

//Given a delay arc propagate the delays and slews for a given cTag 
//void
//TA_Timer::propagateDelaysAndSlews (diganaEdge & delayArc) {

//}

void
TA_Timer::TA_enumerate_paths () {
		
}

