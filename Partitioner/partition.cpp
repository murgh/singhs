#include <iostream>
#include <partition.hxx>
#include <graph.hxx>

int
diganaPartitionMgr::print () {
	std::cout << "Hello I am the partition manager class" << std::endl;
	return 0;
}

class prop : public diganaDynamicGraphProperty {
	int a;
	int b;
};
typedef struct prop prop;

int 
diganaPartitionMgr::test_property_interface () {
  diganaGraphObjectIdentifier id;
  id.setName ("my_graph");
  diganaGraph * graph = diganaGraphMgr::getGraphMgr ().create_graph (id, diganaDirectedGraphS);
  int i = 0;
  while (i < 10) { 
    graph->add_vertex (i); 
    i++;
  }
  //Strike 10 edges
  i = 0;
  while (i < 9) {
    graph->add_edge (i, i+1);
    i++;
  }
  graph->register_vertex_property<int> ("length");
  graph->register_vertex_property<prop> ("cood");
  graph->register_edge_property<double> ("delay");
  i = 0;
  while (i < 9) {
    diganaVertex v(i, graph);
    diganaEdge e(i, i+1, graph);
    v.put_property<int> ("length", i+5);
    e.put_property<double> ("delay", i-10);
   
    i++;
  }
  return 0;
}
