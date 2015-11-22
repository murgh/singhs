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

template<typename Value>
void diganaKernelMgr::register_vertex_property (string graph_name , string property_name ) {

	int id = diganaGraphMgr::getGraphMgr().getId(graph_name);
	diganaGraphObjectIdentifier graph_Id ( id , graph_name);

	if (!diganaGraphMgr::getGraphMgr ().check_graph_identifier (graph_Id) ) {
		cout << "Invalid Graph Id" << endl;
		return;
	}
	return diganaGraphMgr::getGraphMgr().register_vertex_property<Value> (graph_name , property_name);
}

template<typename Value>
void diganaKernelMgr::register_edge_property (string graph_name , string property_name ) {

	int id = diganaGraphMgr::getGraphMgr().getId(graph_name);
	diganaGraphObjectIdentifier graph_Id ( id , graph_name);

	if (!diganaGraphMgr::getGraphMgr ().check_graph_identifier (graph_Id) ) {
		cout << "Invalid Graph Id" << endl;
		return;
	}
	
	return diganaGraphMgr::getGraphMgr().register_edge_property<Value> (graph_name , property_name);
}

template<typename Value>
const Value diganaKernelMgr::get_vertex_property ( int vertex_id , string graph_name , string property_name) {

	int id = diganaGraphMgr::getGraphMgr().getId(graph_name);
	diganaGraphObjectIdentifier graph_Id ( id , graph_name);

	if (!diganaGraphMgr::getGraphMgr ().check_graph_identifier (graph_Id) ) {
		cout << "Invalid Graph Id" << endl;
		return;
	}
	
	if (!diganaGraphMgr::getGraphMgr().check_vertex_id (graph_Id , vertex_id)) {
		cout << "Invalid Vertex Id" << endl;
		return;
	}

	return diganaGraphMgr::getGraphMgr().get_vertex_property<Value> (vertex_id , graph_name , property_name);
}

template<typename Value>
const Value diganaKernelMgr::get_edge_property ( int source , int sink , string graph_name , string property_name) {
	
	int id = diganaGraphMgr::getGraphMgr().getId(graph_name);
	diganaGraphObjectIdentifier graph_Id ( id , graph_name);

	if (!diganaGraphMgr::getGraphMgr ().check_graph_identifier (graph_Id) ) {
		cout << "Invalid Graph Id" << endl;
		return;
	}
	
	if (!diganaGraphMgr::getGraphMgr().check_vertex_id (graph_Id , source)) {
		cout << "Invalid Source vertex Id" << endl;
		return;
	}
	
	if (!diganaGraphMgr::getGraphMgr().check_vertex_id (graph_Id , sink)) {
		cout << "Invalid Sink vertex Id" << endl;
		return;
	}

	return diganaGraphMgr::getGraphMgr().get_edge_property<Value> (sink , source , graph_name , property_name);
}

template<typename Value>
void put_vertex_property ( int vertex_id , string graph_name , string property_name , Value data ) {

	int id = diganaGraphMgr::getGraphMgr().getId(graph_name);
	diganaGraphObjectIdentifier graph_Id ( id , graph_name);

	if (!diganaGraphMgr::getGraphMgr ().check_graph_identifier (graph_Id) ) {
		cout << "Invalid Graph Id" << endl;
		return;
	}
	
	if (!diganaGraphMgr::getGraphMgr().check_vertex_id (graph_Id , vertex_id)) {
		cout << "Invalid Vertex Id" << endl;
		return;
	}

	return diganaGraphMgr::getGraphMgr().put_vertex_property<Value> ( vertex_id , graph_name , property_name , data);
}

template<typename Value>
void put_edge_property ( int source , int sink , string graph_name , string property_name , Value data ) {

	int id = diganaGraphMgr::getGraphMgr().getId(graph_name);
	diganaGraphObjectIdentifier graph_Id ( id , graph_name);

	if (!diganaGraphMgr::getGraphMgr ().check_graph_identifier (graph_Id) ) {
		cout << "Invalid Graph Id" << endl;
		return;
	}
	
	if (!diganaGraphMgr::getGraphMgr().check_vertex_id (graph_Id , source)) {
		cout << "Invalid Source vertex Id" << endl;
		return;
	}

	if (!diganaGraphMgr::getGraphMgr().check_vertex_id (graph_Id , sink)) {
		cout << "Invalid Sink vertex Id" << endl;
		return;
	}

	return diganaGraphMgr::getGraphMgr().put_vertex_property<Value> ( sink , source ,  graph_name , property_name , data);
}



	

	

	
