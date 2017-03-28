#include "timerPin.hxx"
#include "timerEdge.hxx"
#include "timerUtils.hxx"
#include "timerLibData.hxx"
#include "timerLibData.hxx"
#include "../../Kernel/graph.hxx"

std::list<timerLibData *> theLibraryInfo;

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

diganaGraph * get_or_create_circuit (char * circuit) {
	diganaGraphObjectIdentifier id;
	id.setName (circuit);
	diganaGraph * graph = diganaGraphMgr::getGraphMgr ().get_graph (id); 
	if (!graph) {
 	  graph = diganaGraphMgr::getGraphMgr ().create_graph (id, diganaDirectedGraphS);
	  graph->register_vertex_property <timerPinProperty> ("Pin_Property");
	} 
	return graph;
}

void add_pin (diganaGraph * circuit, char * name) {
	int vId = circuit->add_vertex (diganaGraphObjectIdentifier (name));
	diganaVertex V = diganaVertex (vId, circuit);
	timerPinInfo * pinInfo = new timerPinInfo (name);
	V.put_property<timerPinProperty> ("Pin_Property", timerPinProperty (pinInfo));
}

void add_pin_direction (diganaGraph * circuit, char * name, char * dir) {
	timerPinDirection pinDir; 
	std::string direction (dir);
	if (direction == "in")
		pinDir = timerInput;
	else if (direction == "out")
		pinDir = timerOutput;
	else if (direction == "inout")
		pinDir = timerInOut;
	else
		pinDir = timerDirNone;

	diganaVertex V = diganaVertex (circuit->getVertexId (name), circuit);
	timerPinProperty P = V.get_property<timerPinProperty> ("Pin_Property"); 
	P.getPinInfo ()->setDirection (pinDir);
}

//To be added
//void add_pin_identifier (graph * circuit, char * name, char * id) {

//}

void create_edge (char * circuit_name,
		  char * source,
		  char * sink) {
	int srcId, sinkId;
	diganaGraphObjectIdentifier id;
	id.setName (circuit_name);
	diganaGraph * graph = diganaGraphMgr::getGraphMgr ().get_graph (id); 
        srcId = graph->getVertexId (source);
        sinkId = graph->getVertexId (sink);
	graph->add_edge (srcId, sinkId);
	
}


timerLibData *  add_or_get_library (char * libNameS) {
	std::string libName = std::string (libNameS);
	std::list<timerLibData *>::iterator it = theLibraryInfo.begin ();
	for (; it != theLibraryInfo.end (); ++it)
		if ((*it)->getLibName () == libName)
			return *it;
	timerLibData * libData = new timerLibData (libName);	
	theLibraryInfo.push_back (libData);
	printf ("***Info : Added Library %s\n", libNameS);
	return libData;
}

timerLibCell * add_or_get_cell (timerLibData * lib, char * cellName) {
	if (!lib) return NULL;
	printf ("***Info : Added Library Cell %s\n", cellName);
	//return lib->add_or_get_cell (std::string (cellName));
	timerLibCell * libCell = lib->add_or_get_cell (std::string (cellName));
	printf ("Lib Cell = %p %s\n", libCell, libCell->getName ().c_str ());
	return libCell;
}

timerLibPin * add_or_get_pin (timerLibCell * cell, char * pinName) {
	if (!cell) return NULL;
	printf ("Cell = %p \n", cell);
	printf ("***Info : Added Pin %s in Cell %s\n", pinName, cell->getName ().c_str ());
	//return cell->add_or_get_pin (std::string (pinName));
	timerLibPin * cellPin = cell->add_or_get_pin (std::string (pinName));
	printf ("Cell = %p Pin = %p\n", cell, cellPin);
	return cellPin;
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
