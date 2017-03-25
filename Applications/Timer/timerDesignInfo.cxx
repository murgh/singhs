#include "timerPin.hxx"
#include "timerEdge.hxx"
#include "timerUtils.hxx"
#include "timerLibData.hxx"

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
