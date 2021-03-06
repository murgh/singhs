#include "timer.hxx"
#include "timerPin.hxx"
#include "timerEdge.hxx"
#include "timerUtils.hxx"
#include "timerLibData.hxx"
#include "timerLibData.hxx"
#include "timerConstraints.hxx"
#include "timerDelay.hxx"
#include "../../Kernel/graph.hxx"

std::list<timerLibData *> theLibraryInfo;

int edge_count = 0;

char pin_type [6][30] = {
  "timerIOPort\0",
  "timerComboPin\0",
  "timerLatchData\0",
  "timerLatchClock\0",
  "timerPinVirtualNode\0",
  "timerPinIDNone\0"
};	

void create_node (char * circuit_name,
		  char * name, 
		  bool isClock, 
		  bool isData,
		  timerPinIdentifier pinId,
		  timerPinDirection pinDir) {
	diganaGraphObjectIdentifier id;
	id.setName (circuit_name);
	diganaGraph * graph = diganaGraphMgr::getGraphMgr ().get_graph (id); 
	diganaVertex V = diganaVertex (graph->add_vertex (std::string (name)), graph);        	
	timerPinInfo * pinInfo = new timerPinInfo (name, isClock, isData, pinId, pinDir);
	V.put_property<timerPinProperty> ("Pin_Property", timerPinProperty (pinInfo));
}	

diganaGraph * get_circuit (char * circuit) {
	diganaGraphObjectIdentifier id;
	id.setName (circuit);
	diganaGraph * graph = diganaGraphMgr::getGraphMgr ().get_graph (id); 
	if (!graph) 
	  printf ("Circuit %s doesn't exist \n", circuit);
	return graph;
}

diganaGraph * create_circuit (char * circuit, int size) {
	diganaGraphObjectIdentifier id;
	id.setName (circuit);
	diganaGraph * graph = diganaGraphMgr::getGraphMgr ().get_graph (id); 
	if (!graph) {
	  printf ("Creating New Graph \n");
 	  graph = diganaGraphMgr::getGraphMgr ().create_graph (id, diganaDirectedGraphS, size);
	  graph->register_vertex_property <timerPinProperty> ("Pin_Property");
	  graph->register_edge_property <timerArcProperty> ("Arc_Property");
	} 
	return graph;
}

void add_clock (diganaGraph * circuit, char * name, int period, int nodeId, int isVirtual) {
	if (nodeId == -1) return;
	timerClock * clock;
	if ( (clock = timerConstraints::is_clock_in_clock_map (name)) )
	  return;
	if (!isVirtual) {
	  diganaVertex V = diganaVertex (nodeId, circuit);
	  timerPinProperty P = V.get_property<timerPinProperty> ("Pin_Property"); 
	  timerPinInfo * pinInfo = P.getPinInfo ();
	  if (!pinInfo->getIsIOPort ()) {
	    //printf ("WARNING : The clock source %s is not a primary port, no clock created\n", 
			  //pinInfo->getName ().c_str ());
	    return;
	  }
          pinInfo->setIsClock ();
	  std::pair<int, std::string> info = 
		  std::pair<int, std::string> (nodeId, pinInfo->getName ());
	  clock = new timerClock (name, period, info); 
	  timerPinTag clockTag(true, true, nodeId);
	  pinInfo->assert_Clock (clockTag, clock, 0);//0 arrival
	} else {
	  clock = new timerClock (name, period, true); 
	}
	timerConstraints::add_clock_in_clock_map (clock);
}

void add_IO_delay (diganaGraph * circuit, float value, int nodeId, int input) {
	if (nodeId == -1) return;
	diganaVertex V = diganaVertex (nodeId, circuit);
	timerPinProperty P = V.get_property<timerPinProperty> ("Pin_Property"); 
	timerPinInfo * pinInfo = P.getPinInfo ();
	if (!pinInfo->getIsIOPort ()) {
	  printf ("WARNING : The object %s is not a primary port, no IO delay asserted\n", 
			  pinInfo->getName ().c_str ());
	  return;
	}
	timerClock * clock;
	if ( !(clock = timerConstraints::is_clock_in_clock_map (std::string ("**"))) ) {

	  clock = new timerClock (std::string ("**"), 10, true);//Default virtual clock
	  timerConstraints::add_clock_in_clock_map (clock);
	}
	pinInfo->setIsData ();
	timerPinTag clockTag (false, true, nodeId);
	pinInfo->assert_IO_Delay (clockTag, clock, ((timerTime) value), input);
}

int add_pin (diganaGraph * circuit, char * name, int node_count) {
	static int count = 0;
	//int vId = circuit->add_vertex (node_count);
	diganaVertex V = diganaVertex (node_count, circuit);
	timerPinInfo * pinInfo = new timerPinInfo (name);
	V.put_property<timerPinProperty> ("Pin_Property", timerPinProperty (pinInfo));
	count++;
	if (count % 10000 == 0) printf ("Added %d Nodes\n", count);
	return node_count;
}

void add_pin_direction_io (diganaGraph * circuit, 
			   int id, char * dir, 
			   int isIO, char * type) {
	std::string pin_type = std::string (type);
	timerPinDirection pinDir; 
	timerPinIdentifier identity = isIO ? timerIOPort : timerPinIDNone;
	if (identity == timerPinIDNone) {
	  if (strcmp (type, "timerComboPin") == 0) { 
	    identity = timerComboPin; 
	  } else if (strcmp (type, "timerLatchClock") == 0) { 
	    identity = timerLatchClock; 
	  } else if (strcmp (type, "timerLatchData") == 0) {
	    identity = timerLatchData; 
	  } else if (strcmp (type, "timerPinVirtualNode") == 0) {
	    identity = timerPinVirtualNode;
	  }
	}

	std::string direction (dir);
	if (direction == "in")
		pinDir = timerInput;
	else if (direction == "out")
		pinDir = timerOutput;
	else if (direction == "inout")
		pinDir = timerInOut;
	else
		pinDir = timerDirNone;

	diganaVertex V = diganaVertex (id, circuit);
	timerPinProperty P = V.get_property<timerPinProperty> ("Pin_Property"); 
	P.getPinInfo ()->setDirection (pinDir);
	P.getPinInfo ()->setIdentity (identity);
}

void add_timing_arc (diganaGraph * circuit, int source, int sink, timerLibArc * arc) {
	circuit->add_edge (source, sink);
	diganaEdge E = diganaEdge (source, sink, circuit);        	
	timerArcInfo * arcInfo = new timerArcInfo (arc);
	E.put_property<timerArcProperty> ("Arc_Property", timerArcProperty (arcInfo));
	diganaVertex srcNode = diganaVertex (source, circuit);
	diganaVertex sinkNode = diganaVertex (sink, circuit);
	timerPinProperty srcP = srcNode.get_property<timerPinProperty> ("Pin_Property");
	timerPinProperty sinkP = sinkNode.get_property<timerPinProperty> ("Pin_Property");
	srcP.getPinInfo ()->setLibPin (arc->getSource ());
	sinkP.getPinInfo ()->setLibPin (arc->getSink ());
	sinkP.getPinInfo ()->addSourceVertex (source);
	edge_count++;
	//if (count % 1000 == 0)
		//printf ("Created %d Edges\n", count);
}

void add_timing_arc (diganaGraph * circuit, int source, int sink) {
	circuit->add_edge (source, sink);
	diganaVertex sinkNode = diganaVertex (sink, circuit);
	timerPinProperty sinkP = sinkNode.get_property<timerPinProperty> ("Pin_Property");
	diganaVertex srcNode = diganaVertex (source, circuit);
	timerPinProperty srcP = srcNode.get_property<timerPinProperty> ("Pin_Property");
	sinkP.getPinInfo ()->addSourceVertex (source);
	diganaEdge E = diganaEdge (source, sink, circuit);        	
	timerArcInfo * arcInfo = new timerArcInfo ();
	E.put_property<timerArcProperty> ("Arc_Property", timerArcProperty (arcInfo));
}


timerLibData *  add_or_get_library (char * libNameS) {
	std::string libName = std::string (libNameS);
	std::list<timerLibData *>::iterator it = theLibraryInfo.begin ();
	for (; it != theLibraryInfo.end (); ++it)
		if ((*it)->getLibName () == libName)
			return *it;
	timerLibData * libData = new timerLibData (libName);	
	theLibraryInfo.push_back (libData);
	//printf ("***Info : Added Library %s\n", libNameS);
	return libData;
}

timerLibCell * add_or_get_cell (timerLibData * lib, char * cellName) {
	if (!lib) return NULL;
	//printf ("***Info : Added Library Cell %s\n", cellName);
	//return lib->add_or_get_cell (std::string (cellName));
	timerLibCell * libCell = lib->add_or_get_cell (std::string (cellName));
	//printf ("Lib Cell = %p %s\n", libCell, libCell->getName ().c_str ());
	return libCell;
}

timerLibPin * add_or_get_pin (timerLibCell * cell, char * pinName) {
	if (!cell) return NULL;
	//printf ("Cell = %p \n", cell);
	//printf ("***Info : Added Pin %s in Cell %s\n", pinName, cell->getName ().c_str ());
	//return cell->add_or_get_pin (std::string (pinName));
	timerLibPin * cellPin = cell->add_or_get_pin (std::string (pinName));
	//printf ("Cell = %p Pin = %p\n", cell, cellPin);
	return cellPin;
}



void mark_clock (timerLibPin * pin) {
	if (!pin) return;	
	pin->setIsClock ();
}

void add_pin_cap (timerLibPin * pin, float val) {
	if (!pin) return;	
	pin->setCap (val);
}

void add_pin_direction (timerLibPin * pin, char * val) {
	if (!pin) return;	
	pin->setDirection (std::string (val));
}

timerLibArc * add_timing_arc (timerLibCell * cell, char * source, char * sink) {
	if (!cell) return NULL;
	return cell->add_or_get_timing_arc (source, sink);
}

timerLibArc * get_timing_arc (timerLibCell * cell, char * source, char * sink) {
	if (!cell) return NULL;
	return cell->get_timing_arc (source, sink);
}

char * get_pin_type (timerLibCell * cell, char * pin) {
	if (!cell) return pin_type[timerPinIDNone];
	timerPinIdentifier pinType = cell->get_pin_type (cell, pin);
	if (pinType == timerLatchData)
		return pin_type[timerLatchData];
	else if (pinType == timerComboPin)
		return pin_type[timerComboPin];
	else if (pinType == timerLatchClock)
		return pin_type[timerLatchClock];
	else
		return pin_type[timerPinIDNone];
}

void add_timing_type (timerLibArc * arc, char * timing_type) {
	if (!arc) return;
	std::string type (timing_type);
	arc->setArcType (type);
}

void add_timing_sense (timerLibArc * arc, char * timing_sense) {
	if (!arc) return;
	std::string tSense (timing_sense);

	if (tSense == "positive_unate")
		arc->setUnateness (timerPosUnate);
	else if (tSense == "negative_unate")
		arc->setUnateness (timerNegUnate);
	else
		arc->setUnateness (timerNonUnate);
}

void addReportObject (int from, int through, int to, int printTime) {
	TARepObj * obj = new TARepObj;
	obj->from = from;
	obj->through = through;
	obj->to = to;
	obj->printTime = printTime;
	obj->next = NULL;
	TA_Timer::addRepObj (obj);
}

//Timer Report Call

void perform_timing_analysis (char * circuit, int algo, int part) {
	printf ("Performing Timing Analysis on %d edges\n", edge_count);
	diganaGraph * graph = get_circuit (circuit);
	perform_timing_analysis (graph, algo, part);
}
