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
       std::cout << "ERROR : Graph with name " 
	         << graph_Id.getName () 
		 << " already exists." 
		 << std::endl;  
     } 

     diganaGraph * graph = NULL;

     if (type == diganaNoGraphS || type == diganaUndirectedGraphS)
       graph = new diganaUndirectedGraph (graph_Id, type);
     else
       graph = new diganaDirectedGraph (graph_Id, type);
     
     std::pair<std::pair<int, std::string>, diganaGraph *> pair (graph_Id.get_identifier (), graph);

     Id_Graph_Map.insert (pair);

     string graph_name = graph_Id.getName();
     Name_Graph_Map.insert(std::pair<std::string,diganaGraph*>(graph_name , graph ) );     

     graph->setId (Graph_Count);

     Graph_Count++;

     return graph->getId ();
 }
void diganaGraphMgr::setId(std::string name , int id) {
  
    mapNameToGraph::iterator graph_itr_obj = get_graph_through_name(name);
    graph_itr_obj->second->setId(id);
    return;


  }
int diganaGraphMgr::getId(std::string name) {
  mapNameToGraph::iterator graph_itr_obj = get_graph_through_name(name);
  return graph_itr_obj->second->getId();

}

int diganaGraphMgr::add_vertex(std::string name  , std::string vertex_name){
   mapNameToGraph::iterator graph_itr_obj = get_graph_through_name(name);
   return graph_itr_obj->second->add_vertex(vertex_name);
}

void diganaGraphMgr::add_edge(std::string name , int source , int sink ){
   mapNameToGraph::iterator graph_itr_obj = get_graph_through_name(name);
   return graph_itr_obj->second->add_edge( source , sink);
}


int diganaGraphMgr::getVCount(std::string graph_name){
  mapNameToGraph::iterator name_itr = get_graph_through_name(graph_name);
  return name_itr->second->getVCount();
  
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


mapNameToGraph::iterator  diganaGraphMgr::get_graph_through_name(std::string graph_name)
  {
      mapNameToGraph::iterator name_itr = Name_Graph_Map.find(graph_name) ;
  

   try {
       if (name_itr == Name_Graph_Map.end() )
         throw 1;
     } catch (int) {
       std::cout << "ERROR : Graph with name " << graph_name << "does not exist " << std::endl;
     }


	return name_itr;
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

template <typename Value>
void diganaUndirectedGraph::register_vertex_property (std::string name) {
  typedef diganaUndirectedGraphType::vertex_descriptor Key;
  std::map<Key, Value> kvmap;
  boost::associative_property_map< std::map<Key, Value> > assomap (kvmap);
  properties->vertex_properties.property (name, assomap);
}

template <typename Value>
void diganaUndirectedGraph::register_edge_property (std::string name) {
  typedef diganaUndirectedGraphType::edge_descriptor Key;
  std::map<Key, Value> kvmap;
  boost::associative_property_map< std::map<Key, Value> > assomap (kvmap);
  properties->edge_properties.property (name, assomap);
}

template <typename Value>
void diganaDirectedGraph::register_vertex_property (std::string name) {
  typedef diganaDirectedGraphType::vertex_descriptor Key;
  std::map<Key, Value> kvmap;
  boost::associative_property_map< std::map<Key, Value> > assomap (kvmap);
  properties->vertex_properties.property (name, assomap);
}

template <typename Value>
void diganaDirectedGraph::register_edge_property (std::string name) {
  typedef diganaDirectedGraphType::edge_descriptor Key;
  std::map<Key, Value> kvmap;
  boost::associative_property_map< std::map<Key, Value> > assomap (kvmap);
  properties->edge_properties.property (name, assomap);
}
/*
template <typename Value>
void diganaGraph::register_graph_property (std::string name) {
  std::map<Key, Value> kvmap;
  boost::associative_property_map< std::map<Key, Value> > assomap (kvmap);
  graph_properties.property (name, assomap);
}
*/

template<typename Value>
void diganaGraphMgr::register_vertex_property (std::string graph_name, std::string property_name) {
   diganaGraph * graph = get_graph_through_name(graph_name)->second;  
   diganaDirectedGraph * g = (diganaDirectedGraph *) graph;
   g->register_vertex_property<Value> (property_name);
}

template<typename Value>
void diganaGraphMgr::register_edge_property (std::string graph_name, std::string property_name) {
   diganaGraph * graph = get_graph_through_name(graph_name)->second;  
   diganaUndirectedGraph * g = (diganaUndirectedGraph *) graph;
   g->register_edge_property<Value> (property_name);
}
//int main () { return 1; }
