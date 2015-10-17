#include <iostream>
#include "kernel.hxx"

int
diganaKernelMgr::print () {
  std::cout << "This is kernel manager" << std::endl;
  return 0;
}

int
diganaKernelMgr::create_graph (diganaGraphObjectIdentifier & graph_Id,
		               diganaGraphType graph_type) {
  return diganaGraphMgr::getGraphMgr ().create_graph (graph_Id, graph_type); 
}

int
diganaKernelMgr::add_vertex (diganaGraphObjectIdentifier & graph_Id,
		             diganaGraphObjectIdentifier & node_Id) {
  if (!diganaGraphMgr::getGraphMgr ().check_graph_identifier (graph_Id))
    return Null_Identifier_ID; 

  return diganaGraphMgr::getGraphMgr ().add_vertex (graph_Id.getName (), 
		                                    node_Id.getName ()); 
}

void
diganaKernelMgr::add_edge (diganaGraphObjectIdentifier & graph_Id,
		             int source_vertex, int sink_vertex) {
  if (!diganaGraphMgr::getGraphMgr ().check_graph_identifier (graph_Id) ||
      !diganaGraphMgr::getGraphMgr ().check_vertex_id (graph_Id, source_vertex) ||
      !diganaGraphMgr::getGraphMgr ().check_vertex_id (graph_Id, sink_vertex))
    return; 
  
  return diganaGraphMgr::getGraphMgr ().add_edge (graph_Id.getName (),
		                                  source_vertex, sink_vertex);
}
