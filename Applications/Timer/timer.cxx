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
	timerPinInfo pin_properties [29] =
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
		timerPinInfo ("cb4/Y", true/*clk*/, false/*data*/, timerComboPin, timerOutput)
	};
	graph->register_vertex_property<timerPinProperty> ("Pin_Property");
	graph->register_edge_property<timerArcProperty> ("Arc_Property");

	diganaVertex V;
	/*Create the Graph*/
	for (int i = 0; i < 29; i++) {
		graph->add_vertex (i);
		timerPinProperty timerProp (&(pin_properties[i]));
		V = diganaVertex (i, graph);
		V.put_property<timerPinProperty> ("Pin_Property", timerProp);		
	}

	return graph;
}
