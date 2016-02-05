#include <iostream>
#include "kernel.hxx"

using namespace std;

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
    {
    cout << "Invalid graph identifier" << std::endl;
    return Null_Identifier_ID; 
    }
  return diganaGraphMgr::getGraphMgr ().add_vertex (graph_Id.getName (), 
		                                    node_Id.getName ()); 
}

void
diganaKernelMgr::add_edge (diganaGraphObjectIdentifier & graph_Id,
		             int source_vertex, int sink_vertex) {
  if (!diganaGraphMgr::getGraphMgr ().check_graph_identifier (graph_Id))
	{     cout << "Error: Invalid Graph id" << endl ;
     	      return;
	}
  if(!diganaGraphMgr::getGraphMgr ().check_vertex_id (graph_Id, source_vertex))
        {     cout << "Error: Invalid source vertex id "<< source_vertex << endl ;    
     	      return;
	}
  if(!diganaGraphMgr::getGraphMgr ().check_vertex_id (graph_Id, sink_vertex))
    	{     cout << "Error: Invalid sink vertex id " << sink_vertex  << endl ;
              return; 
        }
  
  return diganaGraphMgr::getGraphMgr ().add_edge (graph_Id.getName (),
		                                  source_vertex, sink_vertex);
}

int diganaKernelMgr::get_vertex_id (diganaGraphObjectIdentifier & graphId, std::string name) {
  if (!diganaGraphMgr::getGraphMgr ().check_graph_identifier (graphId))
	{     cout << "Error: Invalid Graph id" << endl ;
     	      return -1;
	}
  return diganaGraphMgr::getGraphMgr ().getVertexId (graphId, name); 
}
