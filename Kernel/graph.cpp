#include "graph.hxx"
#include <iostream>

using namespace std;

bool
diganaGraphObjectIdentifier::operator == (const diganaGraphObjectIdentifier & operand2) const
 { 
    return (identifier == operand2.identifier);
 }

void
diganaGraphObjectIdentifier::operator = (const diganaGraphObjectIdentifier & operand)
 { 
    identifier = operand.identifier;
 }

//Graph manager API to create a grpah, given the object identifier.
int
diganaGraphMgr::create_graph (diganaGraphObjectIdentifier & graph_Id, diganaGraphType type)
 {
     try {
       if (graph_exists (graph_Id)) throw 1; 
     } catch (int) {
       std::cout << "ERROR : Graph with name " << graph_Id.getName () << " already exists." << std::endl;  
     } 

     diganaGraph * graph = NULL;

     if (type == diganaNoGraphS || type == diganaUndirectedGraphS)
       graph = new diganaUndirectedGraph (graph_Id, type);
     else
       graph = new diganaDirectedGraph (graph_Id, type);
     
     std::pair<std::pair<int, std::string>, diganaGraph *> pair (graph_Id.get_identifier (), graph);

     Id_Graph_Map.insert (pair);


     graph->setId (Graph_Count);

     Graph_Count++;

     return graph->getId ();
 }

//Finds out if the graph exists or not already with the graph manager
bool
diganaGraphMgr::graph_exists (diganaGraphObjectIdentifier & graph_Id)
 {
   mapIdToGraph::iterator itr = Id_Graph_Map.find (graph_Id.get_identifier ());
   if (itr != Id_Graph_Map.end ())
     return true;

   return false;
 }

//Addvertex for Undirected Graph
int
diganaUndirectedGraph::add_vertex (std::string name) {

   boost::graph_traits <diganaUndirectedGraphType>::vertex_descriptor v =
	                                           boost::add_vertex (graph);
   property_map<diganaUndirectedGraphType, vertex_name_t>::type VertexName =
	                                                    get(vertex_name, graph);
   property_map<diganaUndirectedGraphType, vertex_index2_t>::type VertexIdx2 =
	                                                    get(vertex_index2, graph);
   VertexIdx2[v] = getVCount();
   VertexName[v] = name;
   incVCount ();
   return VertexIdx2[v];
}

//Addedge for Undirected Graph
void
diganaUndirectedGraph::add_edge (int source, int sink) {
   boost::graph_traits <diganaUndirectedGraphType>::vertex_descriptor u =
	                                               boost::vertex(source, graph);

   boost::graph_traits <diganaUndirectedGraphType>::vertex_descriptor v = 
	                                               boost::vertex(sink, graph);

   boost::add_edge (u, v, graph);
}

//Addvertex for Directed Graph
int
diganaDirectedGraph::add_vertex (std::string name) {

   boost::graph_traits <diganaDirectedGraphType>::vertex_descriptor v =
	                                           boost::add_vertex (graph);
   property_map<diganaDirectedGraphType, vertex_name_t>::type VertexName =
	                                                    get(vertex_name, graph);
   property_map<diganaDirectedGraphType, vertex_index2_t>::type VertexIdx2 =
	                                                    get(vertex_index2, graph);
   VertexIdx2[v] = getVCount();
   VertexName[v] = name;
   incVCount ();
   return VertexIdx2[v];
}

//Addedge for Directed Graph
void
diganaDirectedGraph::add_edge (int source, int sink) {
   boost::graph_traits <diganaDirectedGraphType>::vertex_descriptor u =
	                                               boost::vertex(source, graph);

   boost::graph_traits <diganaDirectedGraphType>::vertex_descriptor v = 
	                                               boost::vertex(sink, graph);

   boost::add_edge (u, v, graph);
}

