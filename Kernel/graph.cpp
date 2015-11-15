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
       std::cout << "GraphMgr_ERROR : Graph with name " 
	         << graph_Id.getName () 
		 << " already exists." 
		 << std::endl;  
     } 

     diganaGraph * graph = NULL;

     if (type == diganaNoGraphS || type == diganaUndirectedGraphS)
       graph = new diganaUndirectedGraph (graph_Id, type);
     else
       graph = new diganaDirectedGraph (graph_Id, type);
     
     string graph_name = graph_Id.getName();
     Name_Graph_Map.insert(std::pair<std::string,diganaGraph*>(graph_name , graph ) );     

     graph->setId (Graph_Count);
     graph_Id.setId (Graph_Count);
     Id_Graph_Map.insert ( std::pair<int,diganaGraph*>(graph->getId () , graph ) );

     Graph_Count++;

     return graph->getId ();
 }

void diganaGraphMgr::setId(std::string name , int id) {
    //graph id must be positive
    if(id > 0)
    {
      mapNameToGraph::iterator graph_itr_obj = get_graph_through_name(name);
      mapIdToGraph::iterator id_itr =  get_graph_through_id(id)  ; 
      	if (id_itr == Id_Graph_Map.end() ) {
  	  cout << "Set graph id for " << name << " to " << id << endl;
    	  graph_itr_obj->second->setId(id);
          id_itr->second->setId(id);
          return;	  
      	 }
         else{
	 std::cout << "Invalid id : Graph with given ID "
                   << id
                   << " alreay exists"	
		   << std::endl;
         return;
         }
     } 
     else
     { 
      		std::cout << "Invalid id : Enter value greater than 1" 			<< std::endl ;
     }
}

int diganaGraphMgr::getId(std::string name) {
  
  mapNameToGraph::iterator graph_itr_obj = get_graph_through_name(name);
//if invalid name is provided : return -1
  if (graph_itr_obj == Name_Graph_Map.end() )
  return -1;
  else 
  return graph_itr_obj->second->getId();

  }

int diganaGraphMgr::add_vertex(std::string name, std::string vertex_name){
   mapNameToGraph::iterator graph_itr_obj = get_graph_through_name(name);
  //if invalid name is provided return -1
   if (graph_itr_obj == Name_Graph_Map.end() )
   return -1;
   else
   return graph_itr_obj->second->add_vertex(vertex_name);
}

void diganaGraphMgr::add_edge(std::string name , int source , int sink ){
   mapNameToGraph::iterator graph_itr_obj = get_graph_through_name(name);
//check for invalid graph name
   if (graph_itr_obj == Name_Graph_Map.end())
   {cout << "Invalid graph name" << endl;
   return ;}
   else 
   return graph_itr_obj->second->add_edge( source , sink);
}


int diganaGraphMgr::getVCount(std::string graph_name){

  mapNameToGraph::iterator name_itr = get_graph_through_name(graph_name);
  if (name_itr == Name_Graph_Map.end() )
   return -1;
   else
  return name_itr->second->getVCount();
  
}

//Finds out if the graph exists or not already with the graph manager
bool
diganaGraphMgr::graph_exists (diganaGraphObjectIdentifier & graph_Id)
 {
   mapNameToGraph::iterator itr = get_graph_through_name(graph_Id.getName ());	 
   if (itr != Name_Graph_Map.end ())
     return true;

   return false;
 }


//given graph name extract graph from graph - name map
mapNameToGraph::iterator
diganaGraphMgr::get_graph_through_name(std::string graph_name) {
  mapNameToGraph::iterator name_itr = Name_Graph_Map.find(graph_name) ;
  

  try {
     if (name_itr == Name_Graph_Map.end() )
       throw 1;
  } catch (int) {
    std::cout << "ERROR : Graph with name " 
	      << graph_name 
	      << "does not exist " 
  	      << std::endl;
  }

  return name_itr;
}

mapIdToGraph::iterator
diganaGraphMgr::get_graph_through_id(int graph_id) {
  mapIdToGraph::iterator id_itr = Id_Graph_Map.find(graph_id) ;
  

  try {
    if (id_itr == Id_Graph_Map.end() )
      throw 1;
  } catch (int) {
    std::cout << "ERROR : Graph with name " << graph_name << "does not exist " << std::endl;
  }

  return id_itr;
}

bool
diganaGraphMgr::check_vertex_id (diganaGraphObjectIdentifier & graph_Id, int vertex_id) {
  
  if (!get_graph_through_id (graph_Id.getId ())->second)
    return false;
  if (vertex_id == Null_Identifier_ID ||
      vertex_id > get_graph_through_id (graph_Id.getId ())->second->getVCount ()) {
    std::cout << "Error : Vertex ID "
	      << vertex_id
	      << " is not correct ID." << std::endl;
    return false;
  }

  return true;
}

bool
diganaGraphMgr::check_graph_identifier (diganaGraphObjectIdentifier & graph_Id) {
  std::string graph_name = graph_Id.getName() ;
  if (graph_Id.getId () != Null_Identifier_ID) {
    if (!graph_name.empty()) {
      try {
       if (get_graph_through_name (graph_Id.getName ())->second !=
	   get_graph_through_id (graph_Id.getId ())->second )                      
	 throw 1;
       } catch (int) {   
         std::cout << "ERROR : Graph Name : " 
		   << graph_Id.getName ()
		   << "and Graph Id : "
		   << graph_Id.getId ()
		   << "does not match."
		   << std::endl;
         return false;
       }
    
    if (get_graph_through_name (graph_Id.getName ())->second ==
            get_graph_through_id (graph_Id.getId ())->second )
    return true;
    } else { //Only graph integer id is provided 
      //Name of the graph must be found 
      graph_Id.setName ( get_graph_through_id (graph_Id.getId ())->second->getName () );
      return true; 
    }
  } else {
    if (graph_Id.getName ().empty ()) {//Empty graph name
        std::cout << "ERROR : ID provided " 
                    << graph_Id.getId ()  
                    << "is not correct and graph name is not provided" 
                << std::endl;
      return false;
    }   
  }  
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

template <typename Value>
void diganaUndirectedGraph::register_vertex_property (std::string name) {
  init_property ();
  properties->register_vertex_property<diganaUndirectedGraphType::vertex_descriptor, Value> (name);
}

template <typename Value>
void diganaUndirectedGraph::put_vertex_property (int vertex, std::string property_name, Value value) {
  typedef diganaUndirectedGraphType::vertex_descriptor Key;
  Key k =  boost::vertex(vertex, graph);
  boost::dynamic_properties & d_property = properties->vertex_properties ();
  boost::put (property_name, d_property, k, value);   
}

template <typename Value>
const Value diganaUndirectedGraph::get_vertex_property (int vertex, std::string property_name) {
  typedef diganaUndirectedGraphType::vertex_descriptor Key;
  Key k =  boost::vertex(vertex, graph);
  boost::dynamic_properties & d_property = properties->vertex_properties ();
  return boost::get<Value> (property_name, d_property, k);   
}

template <typename Value>
void diganaUndirectedGraph::register_edge_property (std::string name) {
  init_property ();
  properties->register_edge_property<diganaUndirectedGraphType::edge_descriptor, Value> (name);
}

template <typename Value>
void diganaUndirectedGraph::put_edge_property (int source, int sink, std::string property_name, Value value) {
  typedef diganaUndirectedGraphType::edge_descriptor Key;
  Key k =  boost::edge(source, sink, graph);
  boost::dynamic_properties & d_property = properties->edge_properties ();
  boost::put (property_name, d_property, k, value);   
}

template <typename Value>
const Value diganaUndirectedGraph::get_edge_property (int source, int sink, std::string property_name) {
  typedef diganaUndirectedGraphType::edge_descriptor Key;
  Key k =  boost::edge(source, sink, graph);
  boost::dynamic_properties & d_property = properties->edge_properties ();
  return boost::get<Value> (property_name, d_property, k);   
}


template <typename Value>
void diganaDirectedGraph::register_vertex_property (std::string name) {
  init_property ();
  properties->register_vertex_property<diganaDirectedGraphType::vertex_descriptor, Value> (name);
}

template <typename Value>
void diganaDirectedGraph::put_vertex_property (int vertex, std::string property_name, Value value) {
  typedef diganaDirectedGraphType::vertex_descriptor Key;
  Key k =  boost::vertex(vertex, graph);
  boost::dynamic_properties & d_property = properties->vertex_properties ();
  boost::put (property_name, d_property, k, value);   
}

template <typename Value>
const Value diganaDirectedGraph::get_vertex_property (int vertex, std::string property_name) {
  typedef diganaDirectedGraphType::vertex_descriptor Key;
  Key k =  boost::vertex(vertex, graph);
  boost::dynamic_properties & d_property = properties->vertex_properties ();
  return boost::get<Value> (property_name, d_property, k);   
}

template <typename Value>
void diganaDirectedGraph::register_edge_property (std::string name) {
  init_property ();
  properties->register_edge_property<diganaDirectedGraphType::edge_descriptor, Value> (name);
}

template <typename Value>
void diganaDirectedGraph::put_edge_property (int source, int sink, std::string property_name, Value value) {
  typedef diganaDirectedGraphType::edge_descriptor Key;
  Key k =  boost::edge(source, sink, graph);
  boost::dynamic_properties & d_property = properties->edge_properties ();
  boost::put (property_name, d_property, k, value);   
}

template <typename Value>
const Value diganaDirectedGraph::get_edge_property (int source, int sink, std::string property_name) {
  typedef diganaDirectedGraphType::edge_descriptor Key;
  Key k =  boost::edge(source, sink, graph);
  boost::dynamic_properties & d_property = properties->edge_properties ();
  return boost::get<Value> (property_name, d_property, k);   
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
  if (graph->getType () == diganaUndirectedGraphS) {
    ((diganaUndirectedGraph *) graph)->register_vertex_property<Value> (property_name);
  } else {
    ((diganaDirectedGraph *) graph)->register_vertex_property<Value> (property_name);
  }
}

template<typename Value>
void diganaGraphMgr::register_edge_property (std::string graph_name, std::string property_name) {
  diganaGraph * graph = get_graph_through_name(graph_name)->second;  
  if (graph->getType () == diganaUndirectedGraphS) {
    ((diganaUndirectedGraph *) graph)->register_edge_property<Value> (property_name);
  } else {
    ((diganaDirectedGraph *) graph)->register_edge_property<Value> (property_name);
  }
}

template<typename Value>
void diganaGraphMgr::put_vertex_property (int vertex, std::string graph_name, std::string property_name, Value info) {
  diganaGraph * graph = get_graph_through_name(graph_name)->second;  
  if (graph->getType () == diganaUndirectedGraphS) {
    ((diganaUndirectedGraph *) graph)->put_vertex_property<Value> (vertex, property_name, info);
  } else {
    ((diganaDirectedGraph *) graph)->put_vertex_property<Value> (vertex, property_name, info);
  }
}

template<typename Value>
void diganaGraphMgr::put_edge_property (int sink, int source, std::string graph_name, std::string property_name, Value info) {
  diganaGraph * graph = get_graph_through_name(graph_name)->second;  
  if (graph->getType () == diganaUndirectedGraphS) {
    ((diganaUndirectedGraph *) graph)->put_edge_property<Value> (sink, source, property_name, info);
  } else {
    ((diganaDirectedGraph *) graph)->put_edge_property<Value> (sink, source, property_name, info);
  }
}

template<typename Value>
const Value diganaGraphMgr::get_vertex_property (int vertex, std::string graph_name, std::string property_name) {
  diganaGraph * graph = get_graph_through_name(graph_name)->second;  
  if (graph->getType () == diganaUndirectedGraphS) {
    return ((diganaUndirectedGraph *) graph)->get_vertex_property<Value> (vertex, property_name);
  } else {
    return ((diganaDirectedGraph *) graph)->get_vertex_property<Value> (vertex, property_name);
  }
}

template<typename Value>
const Value diganaGraphMgr::get_edge_property (int sink, int source, std::string graph_name, std::string property_name) {
  diganaGraph * graph = get_graph_through_name(graph_name)->second;  
  if (graph->getType () == diganaUndirectedGraphS) {
    return ((diganaUndirectedGraph *) graph)->get_edge_property<Value> (sink, source, property_name);
  } else {
    return ((diganaDirectedGraph *) graph)->get_edge_property<Value> (sink, source, property_name);
  }
}

