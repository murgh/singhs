#include "timer.hxx"
#include "timerUtils.hxx"
#include "sys/types.h"
#include "sys/sysinfo.h"
#include <unistd.h>
#include <ios>
#include <iostream>
#include <fstream>
#include <string>

#define PRINT_VM(S, V, R) \
            process_mem_usage (V, R); \
            printf ("The %s VM(%f) RES(%f)\n", S, V, R);

#define END "PO_PERR_L_reg/D"

void cleanTagPath (std::list <timerPinTag **> & list) {
  std::list <timerPinTag **>::iterator itr = list.begin ();
  timerPinTag ** path;
  for (itr = list.begin (); itr != list.end (); ++itr) {
    path = *itr;
    free (path);
  }
  list.clear ();
}

void cleanVtxPaths (std::list<int *> & list) {
  std::list <int *>::iterator itr = list.begin ();
  int * path;
  for (itr = list.begin (); itr != list.end (); ++itr) {
    path = *itr;
    free (path);
  }
  list.clear ();
}

clock_t init;
#define STUBS(C) printf ("%d Clock = %f\n", C, ((float)(clock () - init))/CLOCKS_PER_SEC);
int verbose = 0;
struct sysinfo memInfo;
long long globalMem = memInfo.totalram - memInfo.freeram;
std::map<std::string, timerClock *> timerConstraints::theClockMap;
int timerPinTag::theTagCount = 0;
int TA_Timer::theGlobalForwardMergedCount = 0;
TARepObj * TA_Timer::RepObj = NULL;
float totalTrace = 0.0, totalGetFanin = 0.0, totalWrite = 0.0, totalEnum = 0.0;
void perform_timing_analysis (diganaGraph * graph, int algo, int part) {
	double v1, v2, r1, r2 ;
	clock_t start, enums;
	start = clock ();
        init = clock ();
        process_mem_usage (v1, r1);
	sysinfo (&memInfo);
	TA_Timer * timer = NULL;
	if (algo == 1) timer = new Timer_Algo_1 (graph);
	if (algo == 2) timer = new Timer_Algo_2 (graph);
	timer->TA_enumerate_clock_paths ();
	timer->TA_enumerate_data_paths ();
	timer->TA_Build_Required (part);
	timer->TA_compute_slack ();
	timer->TA_print_circuit (graph);
	timer->TA_write_paths ();
        process_mem_usage (v2, r2);
	start = clock () - start;
	float cl = ((float)start)/CLOCKS_PER_SEC;
	printf ("*** Timing Analysis Done Time (%f sec) Virtual_MEM(%f) Residual_Mem(%f)***\n", cl, v2-v1, r2-r1);
        //printf ("Fanin = %f Trace = %f Write = %f Enum = %f\n", totalGetFanin, totalTrace, totalWrite, totalEnum);
}

timerPinProperty getPinProp (diganaVertex & vertex) {
	return vertex.get_property<timerPinProperty> ("Pin_Property");
}

int TA_Timer::getGlobalForwardMergedCount () {
	return theGlobalForwardMergedCount;
}

bool
TA_Timer::getOtherPinForDPin (diganaVertex endPin, diganaVertex & otherPin) {
  timerSourceVertexIterator ai (endPin);

  diganaVertex source;
  while (!ai.end ()) {
    source = ai.next ();	  
    if (getPinInfo(source)->getIsClockEnd () && 
        getPinInfo(source)->getIdentity () == timerLatchClock) {
      otherPin = source;
      return true;      
    } else
      continue;
  } 
  return false;
}


void TA_Timer::setGlobalForwardMergedCount (int val) {
	theGlobalForwardMergedCount = val;
}

void TA_Timer::clearRepConeMarking () {
  diganaGraphIterator::vertex_iterator vitr, eVitr;

  vitr.attach (theTimingGraph);
  for (; vitr != eVitr; ++vitr) { 
    diganaVertex vertex = *vitr;
    getPinInfo (vertex)->clearRepConeMarking ();
    getPinInfo (vertex)->clearRepPathMarking ();
    getPinInfo (vertex)->clearDFSSeen ();
    getPinInfo (vertex)->clearDFSDone ();
  }
}

void
TA_Timer::getFanOutEndPointSet (diganaVertex tPin, std::set<int> & endSet, bool markCone) {
  std::set<int> thePinSet;
  std::set<int>::iterator pinSetItr;
  std::list<diganaVertex> thePinQueue;
  thePinQueue.push_back (tPin);
  while ( !thePinQueue.empty () ) {
    diganaVertex source = thePinQueue.front ();	  
    if (markCone) getPinInfo (source)->setInRepCone ();
    thePinQueue.pop_front ();
    bool no_sink = true; 
    diganaGraphIterator::adjacency_iterator ai , aietr;
    ai.attach (source.getVertexId (), source.getParentGraph ()); 
    for (; ai != aietr; ++ai) {
      diganaVertex sink = *ai;
      pinSetItr = thePinSet.find (sink.getVertexId ());
      if (pinSetItr != thePinSet.end ())
      	continue;	      
      thePinQueue.push_back (sink);        
      thePinSet.insert (sink.getVertexId ());
      no_sink = false;
    }
    if (no_sink)
      endSet.insert (source.getVertexId ());
  } 
  thePinSet.clear ();
}

void
TA_Timer::getFanInStartPointSet (diganaVertex tPin, std::set<int> & startSet, bool markCone) {
  clock_t start = clock ();
  std::set<int> thePinSet;
  std::set<int>::iterator pinSetItr;
  std::list<diganaVertex> thePinQueue;
  thePinQueue.push_back (tPin);
  while ( !thePinQueue.empty () ) {
    diganaVertex sink = thePinQueue.front ();	  
    if (markCone) getPinInfo (sink)->setInRepCone ();
    thePinQueue.pop_front ();
    bool no_source = true; 
    timerSourceVertexIterator ai (sink);
    while (!ai.end ()) {
      diganaVertex source = ai.next ();
      if (getPinInfo (sink)->getIsLatchDataIn () && getPinInfo (source)->getIsLatchClock ())
	continue; 
      //pinSetItr = thePinSet.find (source.getVertexId ());
      //if (pinSetItr != thePinSet.end ())
      	//continue;	      
      thePinQueue.push_back (source);   
      //thePinSet.insert (source.getVertexId ());
      no_source = false;
    }
    if (no_source)
      startSet.insert (sink.getVertexId ());
  } 
  thePinSet.clear ();
  start = clock () - start;
  float cl = ((float)start)/CLOCKS_PER_SEC;
  totalGetFanin += cl;
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
    if (!pinInfo) {printf ("This is %d\n", pin.getVertexId ()); continue; }
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
Timer_Algo_1::tracePathFrom (diganaVertex pin, 
			     std::list<int> & currPath,
			     std::list<int *> & pathList,
			     bool isClockPath) {
  
  diganaGraphIterator::adjacency_iterator ai , aietr;
  ai.attach (pin.getVertexId (), pin.getParentGraph ()); 
  bool no_sink = true;
  if ( !((isClockPath && getPinInfo (pin)->getIsClockEnd ()) ||
         (!isClockPath && getPinInfo (pin)->getIsDataEnd ()) ) ) {

    for (; ai != aietr; ++ai) {
       no_sink = false;
       diganaVertex sink = *ai;
       getPinInfo (sink)->print ();
       if (!getPinInfo (sink)->isInRepCone () || getPinInfo (sink)->isInPath ())
	     continue;
       if (getPinInfo (pin)->getIsLatchClock () && getPinInfo (sink)->getIsLatchDataIn ())
	 continue;
       currPath.push_back (sink.getVertexId ());
       getPinInfo (sink)->setInPath ();
       tracePathFrom (sink, currPath, pathList, isClockPath);
       sink = diganaVertex (currPath.back (), pin.getParentGraph ());
       currPath.pop_back ();
       getPinInfo (sink)->clearRepPathMarking ();
    }
  }

  if (no_sink) {
    int size = currPath.size (), i = 0;
    int * path = (int *) malloc (sizeof (int) * (size +1));
    path [size] = -1;
    std::list<int>::iterator it;
    for (i = 0, it = currPath.begin (); it != currPath.end (); ++it, ++i) {
      path[i] = *it; 
    }
    pathList.push_back (path);
  }
}

void
Timer_Algo_1::traceFromSetPaths (std::set<int> & startSet, 
				 std::list<int *> & pathList, 
				 bool isClockPath) {
  clock_t start = clock ();
  std::list<int> path;
  std::set<int>::iterator itr;
  for (itr = startSet.begin (); itr != startSet.end (); ++itr) {
    diganaVertex start = diganaVertex (*itr, theTimingGraph);
    path.push_back(start.getVertexId ());
    tracePathFrom (start, path, pathList, isClockPath);    
    path.clear ();
  }
  start = clock () - start; 
  float cl = ((float)start)/CLOCKS_PER_SEC;
  totalTrace += cl;
}

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

//    if (!isClock && getPinInfo (source)->getIsDataEnd ())
//      processDataEndPoint (source, thePinStack);
    
    diganaGraphIterator::adjacency_iterator ai , aietr;
    ai.attach (source.getVertexId (), source.getParentGraph ());
    for (; ai != aietr; ++ai) {
      diganaVertex sink = *ai;
      if ( getPinInfo (sink)->isDFSSeen () || getPinInfo (sink)->isDFSDone () || getPinInfo (source)->getIsClockEnd ())
	continue;      
      thePinStack.push_back (sink);   
      getPinInfo (sink)->setDFSSeen ();
      hasValidSink = true;
      break;
    }
    if (!hasValidSink) {
      getPinInfo (source)->clearDFSSeen ();
      getPinInfo (source)->setDFSDone ();
      thePinStack.pop_back ();
    }
  }
}

void
Timer_Algo_1::TA_enumerate_clock_paths () {
  printf ("Enumerating Clock Paths ....\n");
  std::list<diganaVertex>::iterator itr;
  clearRepConeMarking ();
  for (itr = theClockPortList.begin (); itr != theClockPortList.end (); ++itr) {
    diganaVertex clockPort = *itr;
    performDFSAndPropagatePinTags (clockPort, true/*Clock Path*/); 
  }
}

void
Timer_Algo_1::TA_enumerate_data_paths () {
  printf ("Enumerating Data Paths...\n");
  int count = 0;
  std::list<diganaVertex>::iterator itr;
  clearRepConeMarking ();
  for (itr = theStartPointList.begin (); itr != theStartPointList.end (); ++itr) {
    diganaVertex dataPort = *itr;
    performDFSAndPropagatePinTags (dataPort, false/*Data Path*/); 
    if (count++ == 1000) printf (".");
  }
  printf ("\n");
}

void
Timer_Algo_1::TA_Build_Required (int part) {
}

void
Timer_Algo_1::TA_compute_slack () {
}

void
Timer_Algo_1::TA_print_circuit (diganaGraph * graph) {
}

void
Timer_Algo_1::TA_write_paths () {
  printf ("Writing Paths to file timing_report\n");
  FILE * file = fopen ("timing_report", "w");
  if (file == NULL) return;

  TA_Report_Paths (file);
}

void
Timer_Algo_1::writeTimingPath (FILE * file, int * timingPath, std::string header) {
  clock_t start = clock (); 
  fprintf (file, "%s", header.c_str ());
  bool theFirstNode = true;
  int i = 0;
  while (timingPath[i] != -1) {
    diganaVertex pin = diganaVertex (timingPath[i], theTimingGraph);
    i++;
    getPinInfo (pin)->write_timing_info (file);
  }
  start = clock () - start;
  float cl = ((float)start)/CLOCKS_PER_SEC;
  totalWrite += cl; 
}

int
Timer_Algo_1::TA_Report_Data_Path (FILE * file, int * path, int & pathCount) {
  char print_head [512];
  int i = 0;
  diganaVertex endPoint = getEndPoint (path);
  //Is the end point a D Latch
  if (getPinInfo(endPoint)->getIsDataEnd () && getPinInfo(endPoint)->getIdentity () == timerLatchData) {
    diganaVertex clockPin;
    getOtherPinForDPin (endPoint, clockPin);
    std::set<int> clockStartSet;
    std::list<int *> clockPath;
    getFanInStartPointSet (clockPin, clockStartSet, true); 
    traceFromSetPaths (clockStartSet, clockPath, true /*Clock*/);
    clearRepConeMarking ();
    std::list<int *>::iterator cItr;
    for (cItr = clockPath.begin (); cItr != clockPath.end (); ++cItr) {
      int * cPath = *cItr;
      sprintf (print_head, "\nData Path %d:\n", ++pathCount);
      writeTimingPath (file, path, print_head);	
      std::string otherPathHead = std::string ("Reference Path:\n");
      writeTimingPath (file, cPath, otherPathHead);
    }	 
    clockStartSet.clear ();
    cleanVtxPaths (clockPath);
  } else {
    sprintf (print_head, "\nData Path %d:\n", ++pathCount);
    writeTimingPath (file, path, print_head);	
  }	    
  return pathCount;
}

int
Timer_Algo_1::TA_Report_End_Path (FILE * file, diganaVertex endPoint, bool do_marking) {
  int pathCount = 0;
  std::set<int> startSet;
  std::list<int *> dataPath, clockPath;
  getFanInStartPointSet (endPoint, startSet, do_marking); 
  traceFromSetPaths (startSet, dataPath, false/*data*/);//Gather All Data Path To this end Point
  clearRepConeMarking ();
  std::list<int *>::iterator itr;
  for (itr = dataPath.begin (); itr != dataPath.end (); ++itr) {
    TA_Report_Data_Path (file, *itr, pathCount);	  
  }
  return pathCount;
}

diganaVertex
Timer_Algo_1::getEndPoint (int * path) {

  int i=0;
  while (path[i++] != -1);
  return diganaVertex (path[i-2], theTimingGraph);
}

diganaVertex
Timer_Algo_1::getStartPoint (int * path) {
  return diganaVertex (path[0], theTimingGraph);
}

void
Timer_Algo_1::TA_Report_Paths (FILE * file) {
  if (file == NULL) return;

  if (!RepObj) {
    std::list<diganaVertex>::iterator itr;
    int pathCount = 0;
    for (itr = theEndPointList.begin (); itr != theEndPointList.end (); ++itr) {
       diganaVertex endPoint = *itr;
       printf ("End point %s ", getPinInfo (endPoint)->getName ().c_str ()); 
       clearRepConeMarking ();
       printf ("Path Count %d\n", TA_Report_End_Path (file, endPoint));
    }
  } else {
    TARepObj * t = RepObj;
    do {
      fprintf (file, "\n");
      if (t->from == -1 && t->through == -1 && t->to != -1) {
	//To	
	TA_Report_To (t, file);	
      }

      if (t->from == -1 && t->through != -1 && t->to == -1) {
	//Through 
	TA_Report_Through (t, file);
      }
      
      if (t->from != -1 && t->through != -1 && t->to != -1) {
	//From Through To
	TA_Report_From_Through_To (t, file);
      }
    } while ( (t = t->next) );

  } 
}

void
Timer_Algo_1::TA_Report_To (TARepObj * obj, FILE * file) {
  clearRepConeMarking ();
  diganaVertex endPoint = diganaVertex (obj->to, theTimingGraph);
  fprintf (file, "*** To %s ***\n", getPinInfo (endPoint)->getName ().c_str ());
  TA_Report_End_Path (file, endPoint);
}

void
Timer_Algo_1::TA_Report_Through (TARepObj * obj, FILE * file) {
  clearRepConeMarking ();
  int pathCount = 0;
  diganaVertex throughPin = diganaVertex (obj->through, theTimingGraph);
  fprintf (file, "*** Through %s ***\n", getPinInfo (throughPin)->getName ().c_str ());
  std::set<int> startSet, endSet;
  std::list<int *> dataPath;
  //Cone Marking
  getFanInStartPointSet (throughPin, startSet, true);
  getFanOutEndPointSet (throughPin, endSet, true);
  traceFromSetPaths (startSet, dataPath, false/*data*/);//Gather All Data Path To this end Point
  clearRepConeMarking ();
  std::list<int*>::iterator itr;
  for (itr = dataPath.begin (); itr != dataPath.end (); ++itr) {
    TA_Report_Data_Path (file, *itr, pathCount);
  }
  startSet.clear();
  endSet.clear ();
  cleanVtxPaths (dataPath);
}

void
Timer_Algo_1::TA_Report_From_Through_To (TARepObj * obj, FILE * file) {
  clearRepConeMarking ();
  int pathCount = 0;
  diganaVertex startPoint = diganaVertex (obj->from, theTimingGraph);
  diganaVertex throughPin = diganaVertex (obj->through, theTimingGraph);
  diganaVertex endPoint = diganaVertex (obj->to, theTimingGraph);
  fprintf (file, "*** From %s Through %s To %s ***\n", getPinInfo (startPoint)->getName ().c_str (),
		  				       getPinInfo (throughPin)->getName ().c_str (),
						       getPinInfo (endPoint)->getName ().c_str ());
  std::set<int> startSet, endSet;
  std::list<int *> dataPath;
  //Cone Marking
  getFanInStartPointSet (throughPin, startSet, true);
  getFanOutEndPointSet (throughPin, endSet, true);
  traceFromSetPaths (startSet, dataPath, false/*data*/);//Gather All Data Path To this end Point
  clearRepConeMarking ();
  std::list<int*>::iterator itr;
  for (itr = dataPath.begin (); itr != dataPath.end (); ++itr) {
    if (!isValidFromToPath (*itr, startPoint, endPoint)) continue;
    TA_Report_Data_Path (file, *itr, pathCount);
  }
  startSet.clear();
  endSet.clear ();
  cleanVtxPaths (dataPath);

}

bool
Timer_Algo_1::isValidFromToPath (int * path, diganaVertex & startPoint, diganaVertex & endPoint) {

  diganaVertex pathEnd = getEndPoint (path);
  diganaVertex pathStart = getStartPoint (path);

  if (endPoint != pathEnd)
    return false;

  if (getPinInfo (startPoint)->getIsIOPort () && pathStart != startPoint)
    return false;

  if (getPinInfo (startPoint)->getIdentity () == timerLatchClock) {
    int i = 0;
    while (path[i] != -1) {
      if (path[i] == startPoint.getVertexId ())
	     return true; 
      i++;
    }
    return false;
  }

  return true;
}

//Timer Algorithm 2

int
Timer_Algo_2::getFanInStartTagSet (timerPinTag * tag, std::set<timerPinTag *> & startSet) {
  if (tag == NULL) {/*Start Tag*/
    return 0;
  }

  if (!tag->isDormant ())
    if (!getFanInStartTagSet (tag->getMasterTag(), startSet))
      startSet.insert (tag);

  if (tag->get_tag_container () != NULL) {
    timerPinTag * sibTag;
    timerPinTagContainer::Iterator itr (tag->get_tag_container ());
    while ( (sibTag = itr.next ()) ) {
      if (!sibTag->isDormant ()) {
        if (!getFanInStartTagSet (sibTag->getMasterTag(), startSet))
 	  startSet.insert (sibTag);
      }
    }
  }
  return 1;
}

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

bool
Timer_Algo_2::propagatePinTags (diganaVertex & sourcePin, diganaVertex & sinkPin) {
  timerPinInfo * sourcePinInfo = getPinInfo (sourcePin);   
  timerPinInfo * sinkPinInfo = getPinInfo (sinkPin);   
  bool propagateAhead = true;

  timerPinInfo::propagatePinInfo (sourcePinInfo, sinkPinInfo);

  if (sourcePinInfo->isTagSplitPoint ()) {
    if (sourcePinInfo->get_pin_tag () != sinkPinInfo->get_pin_tag ()->getMasterTag ())
      propagateAhead = false;	    
  } else {
    if (sinkPinInfo->get_pin_tag ()) {
      sinkPinInfo->get_pin_tag ()->merge_pin_tag (sourcePinInfo->get_pin_tag (), sinkPin);
      sourcePinInfo->get_pin_tag ()->setTimingPropagationPoint (sourcePin);
      propagateAhead = false;	
    } else {
      sinkPinInfo->assert_pin_tag (sourcePinInfo->get_pin_tag ());
    }
  }

  return propagateAhead;
}

//Perform the BFS through this pin and propagate the
//tags in the cone.
void
Timer_Algo_2::performBFSAndPropagatePinTags (diganaVertex pin, bool isClock) {
  std::list<diganaVertex> thePinQueue;
  thePinQueue.push_back (pin);
  while ( !thePinQueue.empty () ) {
    diganaVertex source = thePinQueue.front ();
    getPinInfo (source)->print ();
    thePinQueue.pop_front ();
    bool tagSplit = checkAndPerformTagSplitting (source, isClock);  
    diganaGraphIterator::adjacency_iterator ai , aietr;
    ai.attach (source.getVertexId (), source.getParentGraph ());
    for (; ai != aietr; ++ai) {
      bool propagateFurther = false;
      diganaVertex sink = *ai;

      propagateFurther = propagatePinTags (source, sink);

      timerPinInfo * sinkInfo = getPinInfo (sink);
      if (isClock && sinkInfo->getIsClockEnd ()) {
        sinkInfo->print ();
        diganaGraphIterator::adjacency_iterator ckAi , ckAietr;;
	ckAi.attach (sink.getVertexId (), sink.getParentGraph ());
    	for (; ckAi != ckAietr; ++ckAi) {
	  diganaVertex latchData = *ckAi;
	  timerPinInfo * latchPinInfo = getPinInfo (latchData);
	  if (latchPinInfo->getDirection () == timerInput) {//D Pin
            latchPinInfo->print ();
	    timerPinTag * pinTag = new timerPinTag (false, false, sink.getVertexId ());
	    pinTag->setMasterTag (sinkInfo->get_pin_tag ());
            latchPinInfo->assert_other_pin_tag (pinTag); 		  
	  } 
	  if (latchPinInfo->getDirection () == timerOutput) {//Q Pin
	    //Create a new pin tag
            latchPinInfo->print ();
	    timerPinTag * pinTag = new timerPinTag (false, true, sink.getVertexId ());
	    pinTag->setMasterTag (sinkInfo->get_pin_tag ());
	    latchPinInfo->assert_pin_tag (pinTag);
	  } 
	}
	sinkInfo->print ();
	continue;
      }

      if (!isClock && sinkInfo->getIsDataEnd ()) {
        sinkInfo->print ();
	continue;
      }

      if (!propagateFurther) {
        sinkInfo->print ();
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
Timer_Algo_2::TA_Build_Required (int part) {
  printf ("Build Required time At End Points ....\n");
  std::list<diganaVertex>::iterator itr;
  for (itr = theEndPointList.begin (); itr != theEndPointList.end (); ++itr) {
    diganaVertex endPoint = *itr; 
    timerPinInfo * pinInfo = getPinInfo (endPoint);
    timerPinTag * endTag = pinInfo->get_pin_tag ();
    if (part != -1) {	
      pinInfo->initEndTagTree (endTag);
      printf ("End Point %s(Weight=%d)\n", pinInfo->getName ().c_str (), pinInfo->getEndTagTree ()->getWeight ());   
    }
  }  
  printf ("Starting Uniquification ...\n");
  if (part == 1) { 
    Uniquify_end_tags ();
    exit (0);
  }
}

void
Timer_Algo_2::TA_compute_slack () {
  printf ("Compute Slacks ....\n");
  std::list<diganaVertex>::iterator itr;
  for (itr = theEndPointList.begin (); itr != theEndPointList.end (); ++itr) {
    diganaVertex endPoint = *itr;
    timerPinInfo * endPinInfo = getPinInfo (endPoint);
    //if ((endPinInfo->get_pin_tag () && endPinInfo->get_pin_other_tag ()))
      //printf ("Found the both tag sets %s\n", endPinInfo->getName ().c_str ());
    //else
      //printf ("Not found the both tag sets %s\n", endPinInfo->getName ().c_str ());

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
  printf ("Writing Paths to file timing_report\n");	
  FILE * file = fopen ("timing_report", "w");
  if (file == NULL) return; 

  if (RepObj) {
    TARepObj * t = RepObj;
    do {
      fprintf (file, "\n");
      if (t->from == -1 && t->through == -1 && t->to != -1) {
	//To	
	TA_Report_To (t, file);	
      }

      if (t->from == -1 && t->through != -1 && t->to == -1) {
	//Through 
	TA_Report_Through (t, file);
      }
      
      if (t->from != -1 && t->through != -1 && t->to != -1) {
	//From Through To
	TA_Report_From_Through_To (t, file);
      }
    } while ( (t = t->next) );
  } else {
    //Full end slack computation 
    int count = 0;
    std::list<diganaVertex>::iterator itr;
    for (itr = theEndPointList.begin (); itr != theEndPointList.end (); ++itr) {
      double virt, res;
      diganaVertex endPoint = *itr;
    //  if (strcmp (getPinInfo (endPoint)->getName().c_str (), END) != 0)
    //    continue;	    
      int v = 0; 
      computeTagPaths (file, endPoint, v);
      process_mem_usage (virt, res);
      printf ("**EndPoint %s path_count = %d VM(%f) RES(%f)**\n", getPinInfo (endPoint)->getName().c_str (), v, virt, res); 

      if (count++ == 1000) printf (".");
    }   
  }
  printf ("\n");
  fclose (file); 
}

void
Timer_Algo_2::printTagPath (std::list <timerPinTag **> & tagPaths) {
  std::list <timerPinTag **>::iterator itr;
  for (itr = tagPaths.begin (); itr != tagPaths.end (); ++itr) {
    timerPinTag ** path = (*itr);
    int i = 0;
    while (path[i] != NULL) {
      printf ("%d ", path[i]->getTagId ());
      i++;
    }
    printf ("\n");
  }
}

int
Timer_Algo_2::computeTagPaths (FILE * file, diganaVertex endPoint, 
				int & pathCount,
				std::set<timerPinTag *> * pruneFromSet,
				std::set<int> * pruneThroughSet) {

  bool hasRefPath = false;
  diganaVertex refEndPoint;  
  double v, r;
  
  std::list <timerPinTag **> tagPaths, refTagPaths;
  std::list<timerPinTag *> tagPath, refTagPath;
  computeRecursiveTagPath (getPinInfo (endPoint)->get_pin_tag (), tagPath, tagPaths);
  tagPath.clear ();
  //printTagPath (tagPaths);
  timerPinTag * otherTag = getPinInfo (endPoint)->get_pin_other_tag ();
  if (otherTag && otherTag->getMasterTag ())
  {	  
    refEndPoint = diganaVertex (otherTag->getSource (), theTimingGraph); 
    hasRefPath = true;
  }

  char print_head [512];
  std::list <timerPinTag **>::iterator itr, refItr;
  for (itr = tagPaths.begin (); itr != tagPaths.end (); ++itr) {
    if (pruneFromSet) {
      timerPinTag * startTag = (*itr)[0];
      std::set<timerPinTag *>::iterator itrTag = pruneFromSet->find (startTag);
      if (itrTag == pruneFromSet->end ())
	continue;
    }
    sprintf (print_head, "\nData Path %d:\n", ++pathCount);
    std::string pathHead = std::string (print_head);  
    std::list<diganaVertex> timingPath;
    std::list<diganaEdge> reportPath;
    buildTimingPathFromTagPath (endPoint, *itr, timingPath, timerLate);
    if (pruneThroughSet) {
      bool foundThrough = false;
      std::list<diganaVertex>::iterator i;
      std::set<int>::iterator it;
      for (i = timingPath.begin (); i != timingPath.end (); ++i) {
        it = pruneThroughSet->find ( (*i).getVertexId () );
        if (it != pruneThroughSet->end () ) {
	  foundThrough = true;	
	  break;
	}
      }
      if (!foundThrough) {
	timingPath.clear ();      
	--pathCount;
	continue;
      }
    }
    if (hasRefPath) {
      computeRecursiveTagPath (otherTag->getMasterTag (), refTagPath, refTagPaths);
      refTagPath.clear ();
      for (refItr = refTagPaths.begin (); refItr != refTagPaths.end (); ++refItr) {
         std::string otherPathHead = std::string ("Reference Path:\n");
         std::list<diganaVertex> refTimingPath; 	    
         std::list<diganaEdge> refReportPath; 	    
         buildTimingPathFromTagPath (refEndPoint, *refItr, refTimingPath, timerEarly);
         writeTimingPath (file, timingPath, pathHead);
         writeTimingPath (file, refTimingPath, otherPathHead);
	 computeSlackAndWrite (file, endPoint, refEndPoint);
	 refTimingPath.clear ();
      }
      cleanTagPath (refTagPaths);
      continue;
    }
    writeTimingPath (file, timingPath, pathHead);
    computeSlackAndWrite (file, endPoint, endPoint);
    timingPath.clear ();
  }
  cleanTagPath (tagPaths);
  return pathCount;
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
//  getPinInfo(lastNode)->print ();
//  getPinInfo (lastNode)->printDCInfo ();
}

void
Timer_Algo_2::computeSlackAndWrite (FILE * file, diganaVertex endPoint, diganaVertex refEndPoint) {
  //Iterate on the delay objects from endpoint and refEndPoint and compute slacks for each of them 
  return;
  timerPinDelayContainer::Iterator endItr (getPinInfo (endPoint)->getDCInfo ()),
	  			   refEndItr (getPinInfo (refEndPoint)->getDCInfo ());  

  fprintf (file, "Computing slack for Sig %s and Ref %s\n", getPinInfo (endPoint)->getName ().c_str (),
		  				      getPinInfo (refEndPoint)->getName ().c_str ());
  
}

void
Timer_Algo_2::computeRecursiveTagPath (timerPinTag * tag,
				   std::list <timerPinTag *> & theTagPath,
				   std::list <timerPinTag **> & tagPaths) { 
  if (tag == NULL) {
    //Reached the end, copy the path and push in tagPaths
    int size = theTagPath.size (), i = 0;
    timerPinTag ** path = (timerPinTag **) malloc (sizeof (timerPinTag *) * size+1);
    std::list <timerPinTag *>::iterator itr;
    //printf ("Begin Tag Path:\n");
    for (i = 0, itr = theTagPath.begin (); itr != theTagPath.end (); ++itr) {
      *(path + i++) = *itr;
      //printf ("\t%d\n", (*itr)->getTagId ());
    } 
    *(path + i) = NULL;
    //printf ("End Tag Path\n");
    tagPaths.push_back (path);
    return;
  }

  //Process the tag if its not dormant
  if (!tag->isDormant ())
  {
    theTagPath.push_front (tag);
    computeRecursiveTagPath (tag->getMasterTag(), theTagPath, tagPaths);
    theTagPath.pop_front ();
    //printf ("Working\n");
  } else {
    printf ("Dormant\n");
  }
  //Process the union tag set contained in this set
  if (tag->get_tag_container () != NULL) {
    timerPinTag * sibTag;
    timerPinTagContainer::Iterator itr (tag->get_tag_container ());
    while ( (sibTag = itr.next ()) ) {
      if (!sibTag->isDormant ())
      {
        theTagPath.push_front (sibTag);
        computeRecursiveTagPath (sibTag->getMasterTag(), theTagPath, tagPaths);
        theTagPath.pop_front ();
	//printf ("Working\n");
      }
      else
       printf ("Dormant\n");
    }
  }
}

void
Timer_Algo_2::buildTimingPathFromTagPath (diganaVertex endPoint,
			                  timerPinTag ** theTagPath, 
		 	    		  std::list<diganaVertex> & timingPath,
			    		  timerAnalysisType el) {

  bool isFirstStage = true;
  int tagPos = 0;
  timerPinTag * nextTag, * sinkTag;
  //- timerPinTag * tag = theTagPath->front ();
  //- theTagPath->pop_front ();
  timerPinTag * tag = theTagPath[tagPos];
  if (!tag) return;
  tagPos++;
  diganaVertex pin (tag->getSource (), theTimingGraph); 
  diganaVertex lastPin;
  timerDelayCalcArgs dcArgs;
  while ( true ) {
    if (verbose) getPinInfo (pin)->print ();
    //- nextTag = theTagPath->front ();
    nextTag = theTagPath[tagPos];
    timingPath.push_back (pin);
    if (pin == endPoint)
      return; 
    diganaGraphIterator::adjacency_iterator ai , aietr;
    ai.attach (pin.getVertexId (), theTimingGraph);
    for (; ai != aietr; ++ai) {
      diganaVertex sink = *ai;
      sinkTag = getPinInfo (sink)->get_pin_tag ();    
      //getPinInfo (sink)->print ();
      if ( timerPinTag::areTagsInUnion (sinkTag, tag) ) {
	lastPin = pin;
	pin = sink;
	break;
      }
      if ( timerPinTag::areTagsInUnion (sinkTag, nextTag) ) {
	lastPin = pin;
	pin = sink;
	tag = nextTag; 
        //- theTagPath->pop_front ();
	tagPos++;
	break;
      }
    } 

    //Compute Delay for the Arc
  //  dcArgs.setupStage (el, lastPin, getPinInfo (lastPin)->get_pin_tag (), pin, sinkTag, getPinInfo (pin)->getCap (), isFirstStage);      
  //  computeEdgeDelayAndPropagateArrival (dcArgs);
  //  isFirstStage = false;
  }
}

void
Timer_Algo_2::TA_Report_To (TARepObj * obj, FILE * file) {
  
  int pathCount = 0;
  diganaVertex endPoint = diganaVertex (obj->to, theTimingGraph); 
  fprintf (file, "*** To %s ***\n", getPinInfo (endPoint)->getName ().c_str ());
  //process_mem_usage (virt, res);
  computeTagPaths (file, endPoint, pathCount);
}

void
Timer_Algo_2::TA_Report_Through (TARepObj * obj, FILE * file) {
  
  diganaVertex throughPoint = diganaVertex (obj->through, theTimingGraph);
  fprintf (file, "*** Through %s ***\n", getPinInfo (throughPoint)->getName ().c_str ());
  std::set<int> endSet;
  std::set<timerPinTag *> startSet;
  std::set<int> throughSet;
  getFanOutEndPointSet (throughPoint, endSet, false);
  getFanInStartTagSet (getPinInfo (throughPoint)->get_pin_tag (), startSet);
  throughSet.insert (obj->through);
  int pathCount = 0; 
  std::set<int>::iterator endI;
  for (endI = endSet.begin (); endI != endSet.end (); ++endI) {
    diganaVertex endPoint = diganaVertex ((*endI), theTimingGraph);	  
    computeTagPaths (file, endPoint, pathCount, &startSet, &throughSet);
  } 

  endSet.clear ();
  startSet.clear ();
  throughSet.clear ();
}

void
Timer_Algo_2::TA_Report_From_Through_To (TARepObj * obj, FILE * file) {

  int pathCount = 0;
  diganaVertex endPoint = diganaVertex (obj->to, theTimingGraph);
  diganaVertex throughPoint = diganaVertex (obj->through, theTimingGraph);
  diganaVertex startPoint = diganaVertex (obj->from, theTimingGraph);
  fprintf (file, "*** From %s Through %s To %s ***\n", getPinInfo (startPoint)->getName ().c_str (),
		  				       getPinInfo (throughPoint)->getName ().c_str (),
						       getPinInfo (endPoint)->getName ().c_str ());
  std::set<timerPinTag *> startSet;
  std::set<int> throughSet;
  getFanInStartTagSet (getPinInfo (startPoint)->get_pin_tag (), startSet);
  throughSet.insert ( throughPoint.getVertexId () );
  
  computeTagPaths (file, endPoint, pathCount, &startSet, &throughSet);
  startSet.clear ();

}

void
Timer_Algo_2::TA_Report_Paths (FILE * file) {

}

void
Timer_Algo_2::Uniquify_end_tags ()
{
  int id;
  std::set<int> otherIds;
  std::set<int>::iterator otherIdItr;
  int size = theEndPointList.size ();
  UF theSet(size);
  std::list<diganaVertex>::iterator itr;
  for (id = 0, itr = theEndPointList.begin (); itr != theEndPointList.end (); ++itr, ++id) {
    diganaVertex endPoint = *itr; 
    timerPinInfo * pinInfo = getPinInfo (endPoint);
    timerPinTag * endTag = pinInfo->get_pin_tag ();
    endTag->setTreeId (id); 
    getPinInfo (endPoint)->getEndTagTree ()->propagateTreeId (otherIds, id);
    for (otherIdItr = otherIds.begin (); otherIdItr != otherIds.end (); ++otherIdItr)  
    {
      int oId = *otherIdItr;
      theSet.merge (oId, id);	 
    }
    otherIds.clear ();
  }
  printf ("The number of partitions = %d\n", theSet.count());
}

//For the container iterator
timerPinTagContainer::Iterator::Iterator (timerPinTagContainer * cont) {
  theIterSize = 0;
  theTagSet.clear ();
  buildExpandedTagSet (cont);	
  std::set<timerPinTag *>::iterator itr;
  for (itr = theTagSet.begin (); itr != theTagSet.end (); ++itr) {
    if ( (*itr)->isDormant ()) {
     theIterSize--;
     continue;
    }
    theTagList.push_back (*itr);
  }
  assert (theIterSize == theTagList.size ());
}

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

timerSourceVertexIterator::timerSourceVertexIterator (diganaVertex & v) {
  theTimingGraph = v.getParentGraph ();
  vecPos = -1;
  int size = getPinProp (v).getPinInfo ()->getSourceVertexSet ().size ();
  std::set<int>::iterator it;
  theSourceVertices = (int *) malloc (sizeof (int) * size);
  for (it = getPinProp (v).getPinInfo ()->getSourceVertexSet ().begin ();
       it != getPinProp (v).getPinInfo ()->getSourceVertexSet ().end (); ++it) {
    theSourceVertices[++vecPos] = *it;
  }
}

//For End Tag Tree

void timerPinTagTree::buildTagTree (timerPinTagTreeNode * master, timerPinTag * endTag)
{
  if (endTag == NULL) return;

  if (endTag->get_tag_container () != NULL)
  {
    timerPinTag * sibTag;
    timerPinTagContainer::Iterator itr (endTag->get_tag_container ());
    while ( (sibTag = itr.next ()) ) {
      if (!sibTag->isDormant ()) {
        buildTagTree (insert (master, sibTag), sibTag->getMasterTag ());
      }
    }
  } else {
   buildTagTree (insert (master, endTag), endTag->getMasterTag ());
  } 
}

timerPinTagTreeNode *
timerPinTagTree::insert (timerPinTagTreeNode * master, timerPinTag * tag)
{
  timerPinTagTreeNode * tagNode = NULL;
  std::map<timerPinTag *,timerPinTagTreeNode *>::iterator it;
  it = theTagTreeMap.find (tag); 
  if (it == theTagTreeMap.end()) {
    tagNode = new timerPinTagTreeNode (tag); 
    theTagTreeMap.insert (std::pair<timerPinTag *,timerPinTagTreeNode *> (tag, tagNode));
  } else {
    tagNode = it->second;
  }
  master->theNextSet.insert (tagNode);
  return tagNode;
}

void timerPinTagTree::propagateAndComputeWeight ()
{
  std::list<timerPinTagTreeNode *> Queue;
  Queue.push_back (theRoot);
  while (Queue.size ())
  {
    timerPinTagTreeNode * front = Queue.front ();
    int height = front->theHeight;
    Queue.pop_front ();
    std::set<timerPinTagTreeNode *>::iterator itr = front->theNextSet.begin ();
    for (; itr != front->theNextSet.end (); ++itr)
    {
      Queue.push_back (*itr);
      (*itr)->theHeight = height + 1;
      theWeight += (*itr)->theHeight;
    }
  }
}


void timerPinTagTree::propagateTreeId (std::set<int> & idSet, int treeId)
{
  std::list<timerPinTagTreeNode *> Queue;
  Queue.push_back (theRoot);
  while (Queue.size ())
  {
    timerPinTagTreeNode * front = Queue.front ();
    Queue.pop_front ();
    std::set<timerPinTagTreeNode *>::iterator itr = front->theNextSet.begin ();
    for (; itr != front->theNextSet.end (); ++itr)
    {
      Queue.push_back (*itr);
      if ((*itr)->theTag->getTreeId () == -1)
        (*itr)->theTag->setTreeId (treeId);
      else
	if ((*itr)->theTag->isDataPath ())
	  idSet.insert ((*itr)->theTag->getTreeId ());		
    }
  }
  
}
