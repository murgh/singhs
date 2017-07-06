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

void
TA_Timer::checkAndPerformTagSplitting (diganaVertex & timingPin) {
	diganaGraphIterator::adjacency_iterator ai , aietr;
	diganaGraph * circuit = timingPin.getParentGraph ();
}

void
TA_Timer::TA_enumerate_paths () {
		
}

