#include "timer.hxx"
#include "timerUtils.hxx"

int verbose = 0;
std::map<std::string, timerClock *> timerConstraints::theClockMap;
int timerPinTag::theTagCount = 0;
int TA_Timer::theGlobalForwardMergedCount = 0;
void perform_timing_analysis (diganaGraph * graph) {
	TA_Timer * timer = new Timer_Algo_2 (graph);
	//TA_Timer * timer = new Timer_Algo_1 (graph);
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

int TA_Timer::getGlobalForwardMergedCount () {
	return theGlobalForwardMergedCount;
}

void TA_Timer::setGlobalForwardMergedCount (int val) {
	theGlobalForwardMergedCount = val;
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

//Timer Algorithm 1
void 
Timer_Algo_1::processClockEndPoint (diganaVertex endPoint, std::list<diganaVertex> & pinList) {
  std::map<timerPinInfo *, std::list<std::list<diganaVertex> * > * >::iterator itr;
  itr = theClockEndPointPathMap.find (getPinInfo (endPoint));
  std::list<std::list<diganaVertex> * > * pathList;  
  if (itr == theClockEndPointPathMap.end ()) {
    pathList = new std::list<std::list<diganaVertex>*>;
  } else {
    pathList = itr->second;
  }
  std::list<diganaVertex> * new_path = new std::list<diganaVertex>;
  std::list<diganaVertex>::iterator pItr = pinList.begin ();
  diganaVertex pin;
  for (; pItr != pinList.end (); ++pItr) {
    pin = *pItr;
    new_path->push_back (pin);
  }
  pathList->push_back (new_path);
  diganaGraphIterator::adjacency_iterator ai , aietr;
  ai.attach (endPoint.getVertexId (), endPoint.getParentGraph ());
  for (; ai != aietr; ++ai) {
     diganaVertex sink = *ai;
     timerPinInfo * sinkPinInfo = getPinInfo (sink);
     if ((sinkPinInfo->getIsDataStart () && !sinkPinInfo->getIsIOPort ()) //Q Pin    
		     ||
         (sinkPinInfo->getIsDataEnd () && !sinkPinInfo->getIsIOPort ())) //D Pin    
       sinkPinInfo->setReferencePin (endPoint);	
  }
}

void
Timer_Algo_1::performDFSAndPropagatePinTags (diganaVertex startPoint, bool isClock) {
  std::list<diganaVertex> thePinStack;
  thePinStack.push_back (startPoint);
  while ( !thePinStack.empty () ) {
    bool hasValidSink = false;
    diganaVertex source = thePinStack.back ();
    getPinInfo (source)->print ();
    if (isClock && getPinInfo (source)->getIsClockEnd ()) 
      processClockEndPoint (source, thePinStack);

    if (!isClock && getPinInfo (source)->getIsDataEnd ())
      processDataEndPoint (source, thePinStack);
    
    diganaGraphIterator::adjacency_iterator ai , aietr;
    ai.attach (source.getVertexId (), source.getParentGraph ());
    for (; ai != aietr; ++ai) {
      diganaVertex sink = *ai;
      if ( getPinInfo (sink)->isDFSSeen () )
	continue;      
      thePinStack.push_back (sink);   
      getPinInfo (sink)->setDFSSeen ();
      hasValidSink = true;
      break;
    }
    if (!hasValidSink) {
      getPinInfo (source)->clearDFSSeen ();
      thePinStack.pop_back ();
    }
  }
}

void
Timer_Algo_1::TA_enumerate_clock_paths () {
  printf ("Enumerating Clock Paths ....\n");
  std::list<diganaVertex>::iterator itr;
  for (itr = theClockPortList.begin (); itr != theClockPortList.end (); ++itr) {
    diganaVertex clockPort = *itr;
    performDFSAndPropagatePinTags (clockPort, true/*Clock Path*/); 
  }
}

void
Timer_Algo_1::TA_enumerate_data_paths () {
  printf ("Enumerating Data Paths ....\n");
  std::list<diganaVertex>::iterator itr;
  for (itr = theStartPointList.begin (); itr != theStartPointList.end (); ++itr) {
    diganaVertex dataPort = *itr;
    performDFSAndPropagatePinTags (dataPort, false/*Data Path*/); 
  }
}

void
Timer_Algo_1::TA_Build_Required () {
}

void
Timer_Algo_1::TA_compute_slack () {
}

void
Timer_Algo_1::TA_print_circuit (diganaGraph * graph) {
}

void
Timer_Algo_1::TA_write_paths () {
}

//Timer Algorithm 2

//Check and perform the tag splitting.
bool
Timer_Algo_2::checkAndPerformTagSplitting (diganaVertex & sourcePin, bool isClock) {
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
	   tag->setForwardMergeCount (masterTag->getForwardMergeCount ());
	   if (!sinkPinInfo->get_pin_tag ()) {
	     sinkPinInfo->assert_pin_tag (tag);
	   } else {
	     sinkPinInfo->get_pin_tag ()->merge_pin_tag (tag, sinkPin);	
	     tag->setTimingPropagationPoint (sourcePin);
	   }
	   timerPinInfo::propagatePinInfo (srcPinInfo, sinkPinInfo);
	}
	return true;
}

void
Timer_Algo_2::propagatePinTags (diganaVertex & sourcePin, diganaVertex & sinkPin) {
  timerPinInfo * sourcePinInfo = getPinInfo (sourcePin);   
  timerPinInfo * sinkPinInfo = getPinInfo (sinkPin);   

  timerPinInfo::propagatePinInfo (sourcePinInfo, sinkPinInfo);
  if (sourcePinInfo->get_pin_tag () && sinkPinInfo->get_pin_tag () &&
      (sourcePinInfo->get_pin_tag () != sinkPinInfo->get_pin_tag ()) &&
      (sourcePinInfo->get_pin_tag () != sinkPinInfo->get_pin_tag ()->getMasterTag ())) {
    sinkPinInfo->get_pin_tag ()->merge_pin_tag (sourcePinInfo->get_pin_tag (), sinkPin);
    sourcePinInfo->get_pin_tag ()->setTimingPropagationPoint (sourcePin);
    return;
  }
  if (!sinkPinInfo->get_pin_tag ()) {
    sinkPinInfo->assert_pin_tag (sourcePinInfo->get_pin_tag ());
  }
}

//Perform the BFS through this pin and propagate the
//tags in the cone.
void
Timer_Algo_2::performBFSAndPropagatePinTags (diganaVertex pin, bool isClock) {
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
	    timerPinTag * pinTag = new timerPinTag (false, false, sink.getVertexId ());
	    pinTag->setMasterTag (sinkInfo->get_pin_tag ());
            latchPinInfo->assert_other_pin_tag (pinTag); 		  
	  } 
	  if (latchPinInfo->getDirection () == timerOutput) {//Q Pin
	    //Create a new pin tag
            //latchPinInfo->print ();
	    timerPinTag * pinTag = new timerPinTag (false, true, sink.getVertexId ());
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
Timer_Algo_2::propagatePinTagsFromStart (diganaVertex & startPin, bool isClock) {
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
Timer_Algo_2::TA_enumerate_clock_paths () {
  printf ("Enumerating Clock Paths ....\n");
  std::list<diganaVertex>::iterator itr;
  for (itr = theClockPortList.begin (); itr != theClockPortList.end (); ++itr) {
    diganaVertex clockPort = *itr;
    propagatePinTagsFromStart (clockPort, true/*Clock Tags*/);          
  }  
}

void
Timer_Algo_2::TA_enumerate_data_paths () {
  printf ("Enumerating Data Paths ....\n");
  std::list<diganaVertex>::iterator itr;
  for (itr = theStartPointList.begin (); itr != theStartPointList.end (); ++itr) {
    diganaVertex startPoint = *itr;
    propagatePinTagsFromStart (startPoint, false/*Data Tags*/);          
  }  
}

void
Timer_Algo_2::TA_Build_Required () {
  printf ("Build Required time At End Points ....\n");
  std::list<diganaVertex>::iterator itr;
  //for (itr = theEndPointList.begin (); itr != theEndPointList.end (); ++itr) {
    //diganaVertex endPoint = *itr; 
    //timerPinInfo * pinInfo = getPinInfo (endPoint);
    //if (pinInfo->getIdentity () == timerLatchClock)
      
     
  //}  

}

void
Timer_Algo_2::TA_compute_slack () {
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
Timer_Algo_2::TA_print_circuit (diganaGraph * graph) {
  diganaGraphIterator::vertex_iterator vitr, eVitr;

  vitr.attach (graph);
  for (; vitr != eVitr; ++vitr) {
    diganaVertex pin = *vitr;
    getPinInfo (pin)->print (); 
  }

}

void
Timer_Algo_2::TA_write_paths () {
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
Timer_Algo_2::computeTagPaths (FILE * file, diganaVertex endPoint) {

  bool hasRefPath = false;
  diganaVertex refEndPoint;  
  fprintf (file, "\n**EndPoint %s **\n", getPinInfo (endPoint)->getName().c_str ());
  std::list <std::list<timerPinTag *> *> tagPaths, refTagPaths;
  std::list<timerPinTag *> tagPath, refTagPath;
  computeRecursiveTagPath (getPinInfo (endPoint)->get_pin_tag (), tagPath, tagPaths);
  timerPinTag * otherTag = getPinInfo (endPoint)->get_pin_other_tag ();
  if (otherTag && otherTag->getMasterTag ())
  {	  
    refEndPoint = diganaVertex (otherTag->getSource (), theTimingGraph); 
    hasRefPath = true;
  }

  int count = 0;
  char print_head [512];
  std::list <std::list<timerPinTag *> * >::iterator itr, refItr;
  for (itr = tagPaths.begin (); itr != tagPaths.end (); ++itr) {
    sprintf (print_head, "\nData Path %d:\n", ++count);
    std::string pathHead = std::string (print_head);  
    std::list<diganaVertex> timingPath;
    buildTimingPathFromTagPath (endPoint, *itr, timingPath, timerLate);
    if (hasRefPath) {
      computeRecursiveTagPath (otherTag->getMasterTag (), refTagPath, refTagPaths);
      for (refItr = refTagPaths.begin (); refItr != refTagPaths.end (); ++refItr) {
         std::string otherPathHead = std::string ("Reference Path:\n");
         std::list<diganaVertex> refTimingPath; 	    
         buildTimingPathFromTagPath (refEndPoint, *refItr, refTimingPath, timerEarly);
         writeTimingPath (file, timingPath, pathHead);
         writeTimingPath (file, refTimingPath, otherPathHead);
      }
      refTagPaths.clear ();
      continue;
    }
    writeTimingPath (file, timingPath, pathHead);
  }
}

void
Timer_Algo_2::writeTimingPath (FILE * file, std::list<diganaVertex> & timingPath, std::string header) {
  fprintf (file, "%s", header.c_str ());
  std::list<diganaVertex>::iterator itr;
  bool theFirstNode = true;
  diganaVertex lastNode;
  for (itr = timingPath.begin (); itr != timingPath.end (); ++itr) {
    diganaVertex pin = *itr;
    getPinInfo (pin)->write_timing_info (file);
    if (theFirstNode) {
      theFirstNode = false;
      lastNode = pin;      
      continue;
    } 
    lastNode = pin;      
  }
}

void
Timer_Algo_2::computeRecursiveTagPath (timerPinTag * tag,
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

  //Process the tag
  theTagPath.push_front (tag);
  computeRecursiveTagPath (tag->getMasterTag(), theTagPath, tagPaths);
  theTagPath.pop_front ();
  //Process the union tag set contained in this set
  if (tag->get_tag_container () != NULL) {
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
Timer_Algo_2::buildTimingPathFromTagPath (diganaVertex endPoint,
			    std::list<timerPinTag *> * theTagPath, 
		 	    std::list<diganaVertex> & timingPath,
			    timerAnalysisType el) {

  timerPinTag * nextTag;
  timerPinTag * tag = theTagPath->front ();
  theTagPath->pop_front ();
  diganaVertex pin (tag->getSource (), theTimingGraph); 
  timerDelayCalcArgs dcArgs;
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
	dcArgs.setupStage (el, pin, tag, sink, sinkTag, getPinInfo (sink)->getLibPin ()->getCap ());      
        computeEdgeDelayAndPropagateArrival (dcArgs);
	pin = sink;
	break;
      }
      if ( timerPinTag::areTagsInUnion (sinkTag, nextTag) ) {
	dcArgs.setupStage (el, pin, tag, sink, nextTag, getPinInfo (sink)->getLibPin ()->getCap ());      
        computeEdgeDelayAndPropagateArrival (dcArgs);
	pin = sink;
	tag = nextTag; 
        theTagPath->pop_front ();
	break;
      }
    } 
  }
}

//For the container iterator
void timerPinTagContainer::Iterator::buildExpandedTagSet (timerPinTagContainer * cont) {
  if (!cont) return;
  std::set<timerPinTag *>::iterator itr;
  for (itr = cont->getTagSet ().begin (); itr != cont->getTagSet ().end (); ++itr) {
    timerPinTag * tag = *itr;
    std::pair<std::set<timerPinTag *>::iterator, bool> ret = theTagSet.insert (tag);
    if (ret.second == true)
      theIterSize++;
    buildExpandedTagSet (tag->get_tag_container ());
  }
}
