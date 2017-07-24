#include "timer.hxx"

int verbose = 0;
std::map<std::string, timerClock *> timerConstraints::theClockMap;
int timerPinTag::theTagCount = 0;
void perform_timing_analysis (diganaGraph * graph) {
	TA_Timer * timer = new TA_Timer (graph);
	timer->TA_enumerate_clock_paths ();
	timer->TA_enumerate_data_paths ();
	timer->TA_Build_Required ();
	timer->TA_compute_slack ();
	timer->TA_print_circuit (graph);
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

    if (pinInfo->getIsClockSrc ()) { 
      theClockPortList.push_back (pin);
      pinInfo->setIsClock ();	
    }

    if (pinInfo->getIsDataStart ()) { 
      theStartPointList.push_back (pin);
      pinInfo->setIsData ();	
    }

    if (pinInfo->getIsDataEnd ()) {
      theEndPointList.push_back (pin);
      pinInfo->setIsData ();	
    }      
  }
  if (verbose) printf ("End Creating the timing graph \n");
  return graph;
}

//Check and perform the tag splitting.
bool
TA_Timer::checkAndPerformTagSplitting (diganaVertex & sourcePin, bool isClock) {
	diganaGraph * circuit = sourcePin.getParentGraph ();
	diganaVertex adjPin;
	int count = 0;
	diganaGraphIterator::adjacency_iterator ai , aietr;
	ai.attach (sourcePin.getVertexId (), circuit);
	for (; ai != aietr && count <= 1 ; ++ai, ++count);
	if (count == 1) 
	  return false;//No tag splitting is needed
	timerPinInfo * srcPinInfo = getPinInfo (sourcePin);
	if (srcPinInfo->isTagSplitPoint ())//Already a tag split is happened now we should 
	  return false;			      //propagate tags
	timerPinInfo * sinkPinInfo;
	srcPinInfo->setTagSplitPoint ();
	ai.attach (sourcePin.getVertexId (), circuit);
	for (; ai != aietr; ++ai) {
           //Create a new tag on each of the sink pins
	   diganaVertex sinkPin = *ai;
	   sinkPinInfo = getPinInfo (sinkPin);
	   timerPinTag * masterTag = srcPinInfo->get_pin_tag ();
	   timerPinTag * tag = new timerPinTag (isClock, true, sourcePin.getVertexId ());	
	   tag->setMasterTag (masterTag);
	   if (!sinkPinInfo->get_pin_tag ()) {
	     sinkPinInfo->assert_pin_tag (tag);
	   } else {
	     sinkPinInfo->get_pin_tag ()->merge_pin_tag (tag);	
	   }
	   timerPinInfo::propagatePinInfo (srcPinInfo, sinkPinInfo);
	}
	return true;
}

void
TA_Timer::propagatePinTags (diganaVertex & sourcePin, diganaVertex & sinkPin) {
  timerPinInfo * sourcePinInfo = getPinInfo (sourcePin);   
  timerPinInfo * sinkPinInfo = getPinInfo (sinkPin);   

  timerPinInfo::propagatePinInfo (sourcePinInfo, sinkPinInfo);
  if (sourcePinInfo->get_pin_tag () && sinkPinInfo->get_pin_tag () &&
      (sourcePinInfo->get_pin_tag () != sinkPinInfo->get_pin_tag ())) {
    sinkPinInfo->get_pin_tag ()->merge_pin_tag (sourcePinInfo->get_pin_tag ());
    return;
  }
  if (!sinkPinInfo->get_pin_tag ()) {
    sinkPinInfo->assert_pin_tag (sourcePinInfo->get_pin_tag ());
  }
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
    bool tagSplit = checkAndPerformTagSplitting (source, isClock);  
    diganaGraphIterator::adjacency_iterator ai , aietr;
    ai.attach (source.getVertexId (), source.getParentGraph ());
    for (; ai != aietr; ++ai) {
      diganaVertex sink = *ai;
      if (!tagSplit) propagatePinTags (source, sink);
      timerPinInfo * sinkInfo = getPinInfo (sink);

      if (isClock && sinkInfo->getIsClockEnd ()) {
        //sinkInfo->print ();
        diganaGraphIterator::adjacency_iterator ckAi , ckAietr;;
	ckAi.attach (sink.getVertexId (), sink.getParentGraph ());
    	for (; ckAi != ckAietr; ++ckAi) {
	  diganaVertex latchData = *ckAi;
	  timerPinInfo * latchPinInfo = getPinInfo (latchData);
	  if (latchPinInfo->getDirection () == timerInput) {//D Pin
            //latchPinInfo->print ();
            latchPinInfo->assert_other_pin_tag (sinkInfo->get_pin_tag ()); 		  
	  } 
	  if (latchPinInfo->getDirection () == timerOutput) {//Q Pin
	    //Create a new pin tag
            //latchPinInfo->print ();
	    timerPinTag * pinTag = new timerPinTag (false, true, latchData.getVertexId ());
	    pinTag->setMasterTag (sinkInfo->get_pin_tag ());
	    latchPinInfo->assert_pin_tag (pinTag);
	  } 
	}
	//sinkInfo->print ();
	continue;
      }

      if (!isClock && sinkInfo->getIsDataEnd ()) {
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
  timerPinTag * tag = NULL; 
  if ( (tag = startPinInfo->get_pin_tag ()) == NULL ) {
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
  //for (itr = theEndPointList.begin (); itr != theEndPointList.end (); ++itr) {
    //diganaVertex endPoint = *itr; 
    //timerPinInfo * pinInfo = getPinInfo (endPoint);
    //if (pinInfo->getIdentity () == timerLatchClock)
      
     
  //}  

}

void
TA_Timer::TA_compute_slack () {
  printf ("Compute Slacks ....\n");
  std::list<diganaVertex>::iterator itr;
  for (itr = theEndPointList.begin (); itr != theEndPointList.end (); ++itr) {
    diganaVertex endPoint = *itr;
    timerPinInfo * endPinInfo = getPinInfo (endPoint);
    if ((endPinInfo->get_pin_tag () && endPinInfo->get_pin_other_tag ()))
      printf ("Found the both tag sets %s\n", endPinInfo->getName ().c_str ());
    else
      printf ("Not found the both tag sets %s\n", endPinInfo->getName ().c_str ());

    //endPinInfo->print ();
  }
}

void
TA_Timer::TA_print_circuit (diganaGraph * graph) {
  diganaGraphIterator::vertex_iterator vitr, eVitr;

  vitr.attach (graph);
  for (; vitr != eVitr; ++vitr) {
    diganaVertex pin = *vitr;
    getPinInfo (pin)->print (); 
  }

}

void
TA_Timer::TA_write_paths () {
 /*
 Go to each end point
 Pick the end point tag
 while (tag = tag->master)
 {
   push tag on stack 
 }
 while ( tag = pop )
 {
   node from tag
   put_node_on_path (node);
   foreach next_node adjacent to node
   {
     if (tag == tag_on_next_node or 
         tag_on_top_of_stack == tag_on_next_node)
     {
       put_node_on_path (next_node); 	
     }
   }
 }
 
 */
  FILE * file = fopen ("timing_report", "w");
  if (file == NULL) return; 
 
  std::list<diganaVertex>::iterator itr;
  for (itr = theEndPointList.begin (); itr != theEndPointList.end (); ++itr) {
    //std::stack<diganaVertex> stack;
    diganaVertex endPoint = *itr;
    computeTagPaths (file, endPoint);
  }   
  fclose (file); 
}

void
TA_Timer::computeTagPaths (FILE * file, diganaVertex endPoint) {

  	
  fprintf (file, "EndPoint %s\n", getPinInfo (endPoint)->getName().c_str ());
  std::list <std::list<timerPinTag *> *> tagPaths;
  std::list<timerPinTag *> tagPath;
  computeRecursiveTagPath (getPinInfo (endPoint)->get_pin_tag (), tagPath, tagPaths);
  int count = 0;
  std::list <std::list<timerPinTag *> * >::iterator itr;
  for (itr = tagPaths.begin (); itr != tagPaths.end (); ++itr) {
    std::list<diganaVertex> timingPath;
    buildTimingPathFromTagPath (endPoint, *itr, timingPath);
    writeTimingPath (file, timingPath, count++);
  }
}

void
TA_Timer::writeTimingPath (FILE * file, std::list<diganaVertex> & timingPath, int Nth) {
  fprintf (file, "Path%d --\n", Nth);
  std::list<diganaVertex>::iterator itr;
  for (itr = timingPath.begin (); itr != timingPath.end (); ++itr) {
    diganaVertex pin = *itr;
    getPinInfo (pin)->write_timing_info (file);
  }
}

void
TA_Timer::computeRecursiveTagPath (timerPinTag * tag,
				   std::list <timerPinTag *> & theTagPath,
				   std::list <std::list<timerPinTag *> * > & tagPaths) { 
  if (tag == NULL) {
    //Reached the end, copy the path and push in tagPaths
    std::list <timerPinTag *> * tagPath = new std::list <timerPinTag *>;
    std::list <timerPinTag *>::iterator itr;
    for (itr = theTagPath.begin (); itr != theTagPath.end (); ++itr) {
      tagPath->push_back (*itr); 
    } 
    tagPaths.push_back (tagPath);
    return;
  }

  if (tag->get_tag_container () == NULL) {//No tag set for this tag 
    theTagPath.push_front (tag);
    computeRecursiveTagPath (tag->getMasterTag(), theTagPath, tagPaths);
    theTagPath.pop_front ();
  } else { //A union tag set is there
    timerPinTag * sibTag;
    timerPinTagContainer::Iterator itr (tag->get_tag_container ());
    while ( (sibTag = itr.next ()) ) {
      theTagPath.push_front (sibTag);
      computeRecursiveTagPath (sibTag->getMasterTag(), theTagPath, tagPaths);
      theTagPath.pop_front ();
    }
  } 
}

void
TA_Timer::buildTimingPathFromTagPath (diganaVertex endPoint,
			    std::list<timerPinTag *> * theTagPath, 
		 	    std::list<diganaVertex> & timingPath) {

  timerPinTag * nextTag;
  timerPinTag * tag = theTagPath->front ();
  theTagPath->pop_front ();
  diganaVertex pin (tag->getSource (), theTimingGraph); 
  while ( true ) {	
    getPinInfo (pin)->print ();
    nextTag = theTagPath->front ();
    timingPath.push_back (pin);
    if (pin == endPoint)
      return; 
    diganaGraphIterator::adjacency_iterator ai , aietr;
    ai.attach (pin.getVertexId (), theTimingGraph);
    for (; ai != aietr; ++ai) {
      diganaVertex sink = *ai;
      timerPinTag * sinkTag = getPinInfo (sink)->get_pin_tag ();    
      if ( timerPinTag::areTagsInUnion (sinkTag, tag) ) {
	pin = sink;
	break;
      }
      if ( timerPinTag::areTagsInUnion (sinkTag, nextTag) ) {
	pin = sink;
	tag = nextTag; 
        theTagPath->pop_front ();
	break;
      }
    } 
  }
}

//For the container iterator
void timerPinTagContainer::Iterator::buildExpandedTagList (timerPinTagContainer * cont) {
  if (!cont) return;
  std::list<timerPinTag *>::iterator itr;
  for (itr = cont->getTagSet ().begin (); itr != cont->getTagSet ().end (); ++itr) {
    timerPinTag * tag = *itr;
    theTagList.push_back (tag);
    theIterSize++;
    buildExpandedTagList (tag->get_tag_container ());
  }
}
