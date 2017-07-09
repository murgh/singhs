#include "timer.hxx"

int verbose = 0;
std::map<std::string, timerClock *> timerConstraints::theClockMap;
void perform_timing_analysis (diganaGraph * graph) {
	TA_Timer * timer = new TA_Timer (graph);
	timer->TA_enumerate_clock_paths ();
	timer->TA_enumerate_data_paths ();
	timer->TA_Build_Required ();
	timer->TA_compute_slack ();
	timer->TA_write_paths ();
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
    diganaVertex pin = *vitr;
    timerPinProperty prop = getPinProp (pin);
    timerPinInfo * pinInfo = prop.getPinInfo ();
    if (verbose) pinInfo->print ();

    if (pinInfo->getIsClockSrc ()) 
      theClockPortList.push_back (pin);

    if (pinInfo->getIsDataStart ()) 
      theStartPointList.push_back (pin);

    if (pinInfo->getIsClockEnd ())
      theEndPointList.push_back (pin);	    
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
    //getPinInfo (source)->print ();
    thePinQueue.pop_front ();
    diganaGraphIterator::adjacency_iterator ai , aietr;
    ai.attach (source.getVertexId (), source.getParentGraph ());
    for (; ai != aietr; ++ai) {
      diganaVertex sink = *ai;
      propagatePinTags (source, sink);
      timerPinInfo * sinkInfo = getPinInfo (sink);
      if ((isClock && sinkInfo->getIsClockEnd ()) ||
          (!isClock && sinkInfo->getIsDataEnd ())) {
        //sinkInfo->print ();
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

//For each of the clock ports, populate the tags.
//Iterate on all the sink pins of the in virtual node and
//perform the requisite operation on clock ports.
void
TA_Timer::TA_enumerate_clock_paths () {
  printf ("Enumerating Clock Paths ....\n");
  std::list<diganaVertex>::iterator itr;
  for (itr = theClockPortList.begin (); itr != theClockPortList.end (); ++itr) {
    diganaVertex clockPort = *itr;
    propagatePinTagsFromStart (clockPort, true/*Clock Tags*/);          
  }  
}

void
TA_Timer::TA_enumerate_data_paths () {
  printf ("Enumerating Data Paths ....\n");
  std::list<diganaVertex>::iterator itr;
  for (itr = theStartPointList.begin (); itr != theStartPointList.end (); ++itr) {
    diganaVertex startPoint = *itr;
    propagatePinTagsFromStart (startPoint, false/*Data Tags*/);          
  }  
}

void
TA_Timer::TA_Build_Required () {
  printf ("Build Required time At End Points ....\n");
  std::list<diganaVertex>::iterator itr;
  for (itr = theEndPointList.begin (); itr != theEndPointList.end (); ++itr) {
    diganaVertex endPoint = *itr; 
    timerPinInfo * pinInfo = getPinInfo (endPoint);
    //if (pinInfo->getIdentity () == timerLatchClock)
      
     
  }  

}

void
TA_Timer::TA_compute_slack () {

}

void
TA_Timer::TA_write_paths () {

}
