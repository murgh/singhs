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

#define END "CoreOutputReg_reg_31_/D"

void process_mem_usage(double& vm_usage, double& resident_set)
{
   using std::ios_base;
  using std::ifstream;
  using std::string;

  vm_usage     = 0.0;
  resident_set = 0.0;

  ifstream stat_stream("/proc/self/stat",ios_base::in);

  string pid, comm, state, ppid, pgrp, session, tty_nr;
  string tpgid, flags, minflt, cminflt, majflt, cmajflt;
  string utime, stime, cutime, cstime, priority, nice;
  string O, itrealvalue, starttime;  

  unsigned long vsize;
  long rss;

  stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
	      >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
	      >> utime >> stime >> cutime >> cstime >> priority >> nice
	      >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest

  stat_stream.close();

  long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
  vm_usage     = vsize / 1024.0;
  resident_set = rss * page_size_kb;   
}

void cleanTagPath (std::list <timerPinTag **> & list) {
  std::list <timerPinTag **>::iterator itr = list.begin ();
  timerPinTag ** path;
  for (itr = list.begin (); itr != list.end (); ++itr) {
    path = *itr;
    free (path);
  }
  list.clear ();
}

int verbose = 0;
struct sysinfo memInfo;
long long globalMem = memInfo.totalram - memInfo.freeram;
std::map<std::string, timerClock *> timerConstraints::theClockMap;
int timerPinTag::theTagCount = 0;
int TA_Timer::theGlobalForwardMergedCount = 0;
TARepObj * TA_Timer::RepObj = NULL;
void perform_timing_analysis (diganaGraph * graph) {
	double v1, v2, r1, r2;
        process_mem_usage (v1, r1);
	sysinfo (&memInfo);
	//TA_Timer * timer = new Timer_Algo_2 (graph);
	TA_Timer * timer = new Timer_Algo_1 (graph);
	timer->TA_enumerate_clock_paths ();
	timer->TA_enumerate_data_paths ();
	timer->TA_Build_Required ();
	timer->TA_compute_slack ();
	timer->TA_print_circuit (graph);
	timer->TA_write_paths ();
        process_mem_usage (v2, r2);
	printf ("*** Timing Analysis Done Virtual_MEM(%f) Residual_Mem(%f)***\n", v2-v1, r2-r1);
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

void TA_Timer::clearRepConeMarking () {
  diganaGraphIterator::vertex_iterator vitr, eVitr;

  vitr.attach (theTimingGraph);
  for (; vitr != eVitr; ++vitr) { 
    diganaVertex vertex = *vitr;
    getPinInfo (vertex)->clearRepConeMarking ();
  }
}

void
TA_Timer::getFanOutEndPointSet (diganaVertex tPin, std::set<int> & endSet) {
  std::list<diganaVertex> thePinQueue;
  thePinQueue.push_back (tPin);
  while ( !thePinQueue.empty () ) {
    diganaVertex source = thePinQueue.front ();	  
    thePinQueue.pop_front ();
    bool no_sink = true; 
    diganaGraphIterator::adjacency_iterator ai , aietr;
    ai.attach (source.getVertexId (), source.getParentGraph ()); 
    for (; ai != aietr; ++ai) {
      diganaVertex sink = *ai;
      thePinQueue.push_back (sink);        
      no_sink = false;
    }
    if (no_sink)
      endSet.insert (source.getVertexId ());
  } 
}

void
TA_Timer::getFanInStartPointSet (diganaVertex tPin, std::set<int> & startSet) {
  std::list<diganaVertex> thePinQueue;
  thePinQueue.push_back (tPin);
  while ( !thePinQueue.empty () ) {
    diganaVertex sink = thePinQueue.front ();	  
    thePinQueue.pop_front ();
    bool no_source = true; 
    timerSourceVertexIterator ai (tPin);
    while (!ai.end ()) {
      diganaVertex source = ai.next ();
      thePinQueue.push_back (source);        
      no_source = false;
    }
    if (no_source)
      startSet.insert (sink.getVertexId ());
  } 

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
	return;
  printf ("Enumerating Data Paths");
  int count = 0;
  std::list<diganaVertex>::iterator itr;
  for (itr = theStartPointList.begin (); itr != theStartPointList.end (); ++itr) {
    diganaVertex dataPort = *itr;
    performDFSAndPropagatePinTags (dataPort, false/*Data Path*/); 
    if (count++ == 1000) printf (".");
  }
  printf ("\n");
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

void
Timer_Algo_1::TA_Report_Paths (FILE * file) {
  if (file == NULL) return;

  if (!RepObj) {

  } else {

  } 
}

void
Timer_Algo_1::TA_Report_To (TARepObj * obj, FILE * file) {
  clearRepConeMarking ();
}

void
Timer_Algo_1::TA_Report_Through (TARepObj * obj, FILE * file) {
  clearRepConeMarking ();

}

void
Timer_Algo_1::TA_Report_From_Through_To (TARepObj * obj, FILE * file) {
  clearRepConeMarking ();

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
      process_mem_usage (virt, res);
      printf ("The VM(%f) RES(%f)\n", virt, res);
      computeTagPaths (file, endPoint, v);
      process_mem_usage (virt, res);
      printf ("**EndPoint %s path_count = %d VM(%f) RES(%f)**\n", getPinInfo (endPoint)->getName().c_str (), v, virt, res); 

      if (count++ == 1000) printf (".");
    }   
  }
  printf ("\n");
  fclose (file); 
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
  getFanOutEndPointSet (throughPoint, endSet);
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

//For the container iterator
void timerPinTagContainer::Iterator::buildExpandedTagSet (timerPinTagContainer * cont) {
  if (!cont) return;
  std::set<timerPinTag *>::iterator itr;
  for (itr = cont->getTagSet ().begin (); itr != cont->getTagSet ().end (); ++itr) {
    timerPinTag * tag = *itr;
    if (tag->isDormant ())
      continue;	    
    std::pair<std::set<timerPinTag *>::iterator, bool> ret = theTagSet.insert (tag);
    if (ret.second == true)
      theIterSize++;
    buildExpandedTagSet (tag->get_tag_container ());
  }
}

timerSourceVertexIterator::timerSourceVertexIterator (diganaVertex v) {
  theTimingGraph = v.getParentGraph ();
  vecPos = 0;
  std::set<int>::iterator it;
  for (it = getPinProp (v).getPinInfo ()->getSourceVertexSet ().begin ();
       it != getPinProp (v).getPinInfo ()->getSourceVertexSet ().end (); ++it) {
    theSourceVertices[vecPos++] = *it;
  }
}
