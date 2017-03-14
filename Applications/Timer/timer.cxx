#include "timerPin.hxx"
#include "timerEdge.hxx"
#include "timerUtils.hxx"

void
timerPinInfo::addPinTimeInfo (timerClock * clock, timerPinTimeArgs & time, bool isArrival) {
	std::vector<timerPinTime*> & timeData = (isArrival) ? theArrival : theRequired;
	timerPinTime * pinTime;
	std::vector<timerPinTime*>::iterator timeIter = timeData.begin ();
	for (; timeIter != timeData.end (); ++timeIter) {
		pinTime = *timeIter;
	        if (pinTime->getClock () == clock) {
		  pinTime->addPinTimeInfo (time);
		  return;
		}	  
	}
        pinTime = new timerPinTime (clock);   	
	pinTime->addPinTimeInfo (time);
        timeData.push_back (pinTime);
}

diganaGraph * create_test_timing_graph () {
	diganaGraphObjectIdentifier id;
	id.setName ("Circuit");
	diganaGraph * graph = 
		diganaGraphMgr::getGraphMgr ().create_graph (id, diganaDirectedGraphS);   
	timerPinInfo pin_properties [31] =
	{
		timerPinInfo ("IN1", false/*clk*/, true/*data*/, timerIOPort, timerInput),
		timerPinInfo ("IN2", false/*clk*/, true/*data*/, timerIOPort, timerInput),
		timerPinInfo ("Clock", true/*clk*/, false/*data*/, timerIOPort, timerInput),
		timerPinInfo ("Out", false/*clk*/, true/*data*/, timerIOPort, timerInput),
		timerPinInfo ("db1/A", false/*clk*/, true/*data*/, timerComboPin, timerInput),
		timerPinInfo ("db1/Y", false/*clk*/, true/*data*/, timerComboPin, timerOutput),
		timerPinInfo ("db2/A", false/*clk*/, true/*data*/, timerComboPin, timerInput),
		timerPinInfo ("db2/Y", false/*clk*/, true/*data*/, timerComboPin, timerOutput),
		timerPinInfo ("dOR/A", false/*clk*/, true/*data*/, timerComboPin, timerInput),
		timerPinInfo ("dOR/B", false/*clk*/, true/*data*/, timerComboPin, timerInput),
		timerPinInfo ("dOR/Y", false/*clk*/, true/*data*/, timerComboPin, timerOutput),
		timerPinInfo ("DF1/D", false/*clk*/, true/*data*/, timerLatchData, timerInput),
		timerPinInfo ("DF1/CP", true/*clk*/, false/*data*/, timerLatchClock, timerInput),
		timerPinInfo ("DF1/Q", false/*clk*/, true/*data*/, timerLatchData, timerOutput),
		timerPinInfo ("db3/A", false/*clk*/, true/*data*/, timerComboPin, timerInput),
		timerPinInfo ("db3/Y", false/*clk*/, true/*data*/, timerComboPin, timerOutput),
		timerPinInfo ("DF2/D", false/*clk*/, true/*data*/, timerLatchData, timerInput),
		timerPinInfo ("DF2/CP", true/*clk*/, false/*data*/, timerLatchClock, timerInput),
		timerPinInfo ("DF2/Q", false/*clk*/, true/*data*/, timerLatchData, timerOutput),
		timerPinInfo ("db4/A", false/*clk*/, true/*data*/, timerComboPin, timerInput),
		timerPinInfo ("db4/Y", false/*clk*/, true/*data*/, timerComboPin, timerOutput),
		timerPinInfo ("cb1/A", true/*clk*/, false/*data*/, timerComboPin, timerInput),
		timerPinInfo ("cb1/Y", true/*clk*/, false/*data*/, timerComboPin, timerOutput),
		timerPinInfo ("cb2/A", true/*clk*/, false/*data*/, timerComboPin, timerInput),
		timerPinInfo ("cb2/Y", true/*clk*/, false/*data*/, timerComboPin, timerOutput),
		timerPinInfo ("cb3/A", true/*clk*/, false/*data*/, timerComboPin, timerInput),
		timerPinInfo ("cb3/Y", true/*clk*/, false/*data*/, timerComboPin, timerOutput),
		timerPinInfo ("cb4/A", true/*clk*/, false/*data*/, timerComboPin, timerInput),
		timerPinInfo ("cb4/Y", true/*clk*/, false/*data*/, timerComboPin, timerOutput),
		timerPinInfo ("V_IN", false/*clk*/, false/*data*/, timerPinVirtualNode, timerInput),
		timerPinInfo ("V_OUT", false/*clk*/, false/*data*/, timerPinVirtualNode, timerOutput)
	};
	graph->register_vertex_property<timerPinProperty> ("Pin_Property");
	graph->register_edge_property<timerArcProperty> ("Arc_Property");

	diganaVertex V;
	/*Create Nodes for Graph*/
	for (int i = 0; i < 31; i++) {
		graph->add_vertex (i);
		timerPinProperty timerProp (&(pin_properties[i]));
		V = diganaVertex (i, graph);
		V.put_property<timerPinProperty> ("Pin_Property", timerProp);		
	}

	/*Create Edges for Graph*/
	/*Data Edges*/
	graph->add_edge (29, 0); 
	graph->add_edge (29, 1); 
	graph->add_edge (29, 2); 
	graph->add_edge (3, 30); 
	graph->add_edge (0, 4); 
	graph->add_edge (4, 5); 
	graph->add_edge (5, 8); 
	graph->add_edge (8, 10); 
	graph->add_edge (1, 6); 
	graph->add_edge (6, 7); 
	graph->add_edge (7, 9); 
	graph->add_edge (9, 10); 
	graph->add_edge (10, 11); 
	graph->add_edge (12, 13); 
	graph->add_edge (13, 14); 
	graph->add_edge (14, 15); 
	graph->add_edge (15, 16); 
	graph->add_edge (17, 18); 
	graph->add_edge (18, 19); 
	graph->add_edge (19, 20); 
	graph->add_edge (20, 3); 

	/*Clock*/
	graph->add_edge (2, 21); 
	graph->add_edge (21, 22); 
	graph->add_edge (22, 23); 
	graph->add_edge (23, 24); 
	graph->add_edge (24, 12); 
	graph->add_edge (12, 11); 
	graph->add_edge (24, 25); 
	graph->add_edge (25, 26); 
	graph->add_edge (26, 27); 
	graph->add_edge (27, 28); 
	graph->add_edge (28, 17); 
	graph->add_edge (17, 16); 


	return graph;
}

int main () {

	diganaGraph * graph = create_test_timing_graph ();
	return 0;
}
