#include "timer.hxx"

int verbose = 0;
std::map<std::string, timerClock *> timerConstraints::theClockMap;
void perform_timing_analysis (diganaGraph * graph) {
	TA_Timer * timer = new TA_Timer (graph);
	std::list<diganaVertex> theClockPortList, theStartPointList;
	timer->buildClockPortAndStartPointList (theClockPortList, theStartPointList);
	timer->TA_enumerate_clock_paths (theClockPortList);
	//timer->TA_Build_Required ();
	timer->TA_enumerate_data_paths (theStartPointList);
	//timer->TA_compute_slack ();
	//timer->TA_write_paths ();
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
	diganaVertex adjPin;
	int count = 0;
	diganaGraphIterator::adjacency_iterator ai , aietr;
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


//Iterate over each of the source tag and put it into the sink tag set
void
TA_Timer::propagatePinTags (diganaVertex & sourcePin, diganaVertex & sinkPin) {
  timerPinInfo * sourcePinInfo = getPinInfo (sourcePin);   
  timerPinInfo * sinkPinInfo = getPinInfo (sinkPin);   

  timerPinInfo::propagatePinInfo (sourcePinInfo, sinkPinInfo);
  timerPinTagContainer::Iterator sourceItr (sourcePinInfo->get_pin_tag_container ());
  timerPinTag * tag;
  while ( (tag = sourceItr.next ()) )
    sinkPinInfo->assert_pin_tag (tag);
}

//Perform the BFS through this pin and propagate the
//tags in the cone.
void
TA_Timer::performBFSAndPropagatePinTags (diganaVertex pin, bool isClock) {
  std::list<diganaVertex> thePinQueue;
  thePinQueue.push_back (pin);
  while ( !thePinQueue.empty () ) {
    diganaVertex source = thePinQueue.front ();
    getPinInfo (source)->print ();
    thePinQueue.pop_front ();
    diganaGraphIterator::adjacency_iterator ai , aietr;
    ai.attach (source.getVertexId (), source.getParentGraph ());
    for (; ai != aietr; ++ai) {
      diganaVertex sink = *ai;
      propagatePinTags (source, sink);
      timerPinInfo * sinkInfo = getPinInfo (sink);
      if ((isClock && sinkInfo->getIsClockEnd ()) ||
          (!isClock && sinkInfo->getIsDataEnd ())) {
        sinkInfo->print ();
	continue;
      }
      thePinQueue.push_back (sink);
    }
  }
} 

//For a given start point start propagating the pin tags on the cone
void
TA_Timer::propagatePinTagsFromStart (diganaVertex & startPin, bool isClock) {
  timerPinInfo * startPinInfo = getPinInfo (startPin); 
  timerPinTag pinTag (isClock, true, startPin.getVertexId ());
  timerPinTag * tag = NULL; 
  if ( (tag = startPinInfo->getPinTag (pinTag) ) == NULL) {
    tag = new timerPinTag (isClock, true, startPin.getVertexId ());
    startPinInfo->assert_pin_tag (tag);
  }
  performBFSAndPropagatePinTags (startPin, isClock);
}

//Given a delay arc propagate the delays and slews for a given cTag 
//void
//TA_Timer::propagateDelaysAndSlews (diganaEdge & delayArc) {

//}
void
TA_Timer::buildClockPortAndStartPointList (std::list<diganaVertex> & theClockPortList,
					   std::list<diganaVertex> & theStartPointList) {
  diganaGraphIterator::adjacency_iterator ai , aietr;

  ai.attach (theInVirtualNode);
  for (; ai != aietr; ++ai) {
    diganaVertex startPin = *ai;
    timerPinInfo * startPinInfo = getPinInfo (startPin);
    
    if (startPinInfo->getIsClockSrc ())
      theClockPortList.push_back (startPin);

    if (startPinInfo->getIsDataStart ())
      theStartPointList.push_back (startPin);
  }
}

//For each of the clock ports, populate the tags.
//Iterate on all the sink pins of the in virtual node and
//perform the requisite operation on clock ports.
void
TA_Timer::TA_enumerate_clock_paths (std::list<diganaVertex> & theClockPortList) {
  printf ("Enumerating Clock Paths ....\n");
  std::list<diganaVertex>::iterator itr;
  for (itr = theClockPortList.begin (); itr != theClockPortList.end (); ++itr) {
    diganaVertex clockPort = *itr;
    propagatePinTagsFromStart (clockPort, true/*Clock Tags*/);          
  }  
}

void
TA_Timer::TA_enumerate_data_paths (std::list<diganaVertex> & theStartPointList) {
  printf ("Enumerating Data Paths ....\n");

}
