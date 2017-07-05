#include "timerPin.hxx"
#include "timerEdge.hxx"
#include "timerUtils.hxx"
#define ADD_GRAPH_NODE_AND_PROPERTY(G, I, P) \
	G->add_vertex (I); \
	V = diganaVertex (I, G); \
 	V.put_property<timerPinProperty> ("Pin_Property", timerPinProperty (P));

extern void perform_timing_analysis (diganaGraph *);

diganaGraph * create_timing_graph ()
{
 diganaGraphObjectIdentifier id;
 id.setName ("Circuit");;
 diganaGraph * graph = 
 	diganaGraphMgr::getGraphMgr ().create_graph (id, diganaDirectedGraphS);
 graph->register_vertex_property<timerPinProperty> ("Pin_Property");
 graph->register_edge_property<timerArcProperty> ("Arc_Property");
 diganaVertex V;
 timerPinInfo * prop;
 prop = new timerPinInfo ("out[0]", false, true, timerIOPort, timerOutput);
 ADD_GRAPH_NODE_AND_PROPERTY (graph, 0, prop);
 prop = new timerPinInfo ("out[1]", false, true, timerIOPort, timerOutput);
 ADD_GRAPH_NODE_AND_PROPERTY (graph, 1, prop);
 prop = new timerPinInfo ("clk", false, true, timerIOPort, timerInput);
 ADD_GRAPH_NODE_AND_PROPERTY (graph, 2, prop);
 prop = new timerPinInfo ("in[0]", false, true, timerIOPort, timerInput);
 ADD_GRAPH_NODE_AND_PROPERTY (graph, 3, prop);
 prop = new timerPinInfo ("in[1]", false, true, timerIOPort, timerInput);
 ADD_GRAPH_NODE_AND_PROPERTY (graph, 4, prop);
 prop = new timerPinInfo ("in[2]", false, true, timerIOPort, timerInput);
 ADD_GRAPH_NODE_AND_PROPERTY (graph, 5, prop);
 prop = new timerPinInfo ("in[3]", false, true, timerIOPort, timerInput);
 ADD_GRAPH_NODE_AND_PROPERTY (graph, 6, prop);
 prop = new timerPinInfo ("o", false, true, timerIOPort, timerOutput);
 ADD_GRAPH_NODE_AND_PROPERTY (graph, 7, prop);
 prop = new timerPinInfo ("_16_/A1", false, true, timerComboPin, timerInput);
 ADD_GRAPH_NODE_AND_PROPERTY (graph, 8, prop);
 prop = new timerPinInfo ("_16_/A2", false, true, timerComboPin, timerInput);
 ADD_GRAPH_NODE_AND_PROPERTY (graph, 9, prop);
 prop = new timerPinInfo ("_16_/ZN", false, true, timerComboPin, timerOutput);
 ADD_GRAPH_NODE_AND_PROPERTY (graph, 10, prop);
 prop = new timerPinInfo ("dff/Q", false, true, timerLatchData, timerOutput);
 ADD_GRAPH_NODE_AND_PROPERTY (graph, 11, prop);
 prop = new timerPinInfo ("dff/D", false, true, timerLatchData, timerInput);
 ADD_GRAPH_NODE_AND_PROPERTY (graph, 12, prop);
 prop = new timerPinInfo ("dff/CK", true, false, timerLatchClock, timerInput);
 ADD_GRAPH_NODE_AND_PROPERTY (graph, 13, prop);
 prop = new timerPinInfo ("_19_/A1", false, true, timerComboPin, timerInput);
 ADD_GRAPH_NODE_AND_PROPERTY (graph, 14, prop);
 prop = new timerPinInfo ("_19_/A2", false, true, timerComboPin, timerInput);
 ADD_GRAPH_NODE_AND_PROPERTY (graph, 15, prop);
 prop = new timerPinInfo ("_19_/ZN", false, true, timerComboPin, timerOutput);
 ADD_GRAPH_NODE_AND_PROPERTY (graph, 16, prop);
 prop = new timerPinInfo ("_14_/A1", false, true, timerComboPin, timerInput);
 ADD_GRAPH_NODE_AND_PROPERTY (graph, 17, prop);
 prop = new timerPinInfo ("_14_/A2", false, true, timerComboPin, timerInput);
 ADD_GRAPH_NODE_AND_PROPERTY (graph, 18, prop);
 prop = new timerPinInfo ("_14_/ZN", false, true, timerComboPin, timerOutput);
 ADD_GRAPH_NODE_AND_PROPERTY (graph, 19, prop);
 prop = new timerPinInfo ("_18_/A1", false, true, timerComboPin, timerInput);
 ADD_GRAPH_NODE_AND_PROPERTY (graph, 20, prop);
 prop = new timerPinInfo ("_18_/A2", false, true, timerComboPin, timerInput);
 ADD_GRAPH_NODE_AND_PROPERTY (graph, 21, prop);
 prop = new timerPinInfo ("_18_/ZN", false, true, timerComboPin, timerOutput);
 ADD_GRAPH_NODE_AND_PROPERTY (graph, 22, prop);
 prop = new timerPinInfo ("_17_/A1", false, true, timerComboPin, timerInput);
 ADD_GRAPH_NODE_AND_PROPERTY (graph, 23, prop);
 prop = new timerPinInfo ("_17_/A2", false, true, timerComboPin, timerInput);
 ADD_GRAPH_NODE_AND_PROPERTY (graph, 24, prop);
 prop = new timerPinInfo ("_17_/ZN", false, true, timerComboPin, timerOutput);
 ADD_GRAPH_NODE_AND_PROPERTY (graph, 25, prop);
 prop = new timerPinInfo ("_13_/A", false, true, timerComboPin, timerInput);
 ADD_GRAPH_NODE_AND_PROPERTY (graph, 26, prop);
 prop = new timerPinInfo ("_13_/ZN", false, true, timerComboPin, timerOutput);
 ADD_GRAPH_NODE_AND_PROPERTY (graph, 27, prop);
 prop = new timerPinInfo ("_15_/A1", false, true, timerComboPin, timerInput);
 ADD_GRAPH_NODE_AND_PROPERTY (graph, 28, prop);
 prop = new timerPinInfo ("_15_/A2", false, true, timerComboPin, timerInput);
 ADD_GRAPH_NODE_AND_PROPERTY (graph, 29, prop);
 prop = new timerPinInfo ("_15_/ZN", false, true, timerComboPin, timerOutput);
 ADD_GRAPH_NODE_AND_PROPERTY (graph, 30, prop);
 graph->add_edge (2, 13);
 graph->add_edge (11, 7);
 graph->add_edge (30, 8);
 graph->add_edge (16, 12);
 graph->add_edge (22, 14);
 graph->add_edge (25, 15);
 graph->add_edge (27, 17);
 graph->add_edge (19, 28);
 graph->add_edge (8, 10);
 graph->add_edge (9, 10);
 graph->add_edge (13, 11);
 graph->add_edge (13, 12);
 graph->add_edge (13, 13);
 graph->add_edge (14, 16);
 graph->add_edge (15, 16);
 graph->add_edge (17, 19);
 graph->add_edge (18, 19);
 graph->add_edge (20, 22);
 graph->add_edge (21, 22);
 graph->add_edge (23, 25);
 graph->add_edge (24, 25);
 graph->add_edge (26, 27);
 graph->add_edge (28, 30);
 graph->add_edge (29, 30);
 prop = new timerPinInfo ("V_IN", false, false, timerPinVirtualNode, timerInput),
 ADD_GRAPH_NODE_AND_PROPERTY (graph, 31, prop);
 prop = new timerPinInfo ("V_OUT", false, false, timerPinVirtualNode, timerOutput);
 ADD_GRAPH_NODE_AND_PROPERTY (graph, 32, prop);
 return graph;
}

int main ()
{
 diganaGraph * graph = create_timing_graph ();
 perform_timing_analysis (graph);
 return 0;
}