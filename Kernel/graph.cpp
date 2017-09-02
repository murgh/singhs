#include "graph.hxx"
#include <iostream>
#include <stack>
#include <string>
#include <sstream>

using namespace std;

template <typename T>
string ToString(T val)
 {
    stringstream stream;
    stream << val;    
    return stream.str ();
 }

template <>
string ToString(int val)
 {
    std::string s = std::to_string (val); 
    return s;
 }


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
diganaGraph *
diganaGraphMgr::create_graph (diganaGraphObjectIdentifier & graph_Id, diganaGraphType type, int size)
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
       graph = new diganaUndirectedGraph (graph_Id, type, size);
     else
       graph = new diganaDirectedGraph (graph_Id, type, size);
     
     string graph_name = graph_Id.getName();
     Name_Graph_Map.insert(std::pair<std::string,diganaGraph*>(graph_name , graph ) );     

     graph->setId (Graph_Count);
     graph_Id.setId (Graph_Count);
     Id_Graph_Map.insert ( std::pair<int,diganaGraph*>(graph->getId () , graph ) );

     Graph_Count++;

     return graph;
 }

diganaGraph *
diganaGraphMgr::get_graph (diganaGraphObjectIdentifier graph_Id) {
   mapNameToGraph::iterator itr = get_graph_through_name(graph_Id.getName ());	 
   if (itr == Name_Graph_Map.end ())
     return NULL;

   return itr->second;   

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

bool diganaGraphMgr::check_if_edge_exists(std::string graph_name , int source , int sink){
  mapNameToGraph::iterator graph_itr_obj = get_graph_through_name(graph_name);
  //if invalid name is provided return -1
   if (graph_itr_obj == Name_Graph_Map.end() )
    {std::cout << "Invalid Graph Name" << std::endl;
     return 0;}
   else
   return graph_itr_obj->second->check_if_edge_exists(source , sink);

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

int
diganaGraphMgr::getVertexId (diganaGraphObjectIdentifier & graph_Id, std::string name) {

  if (!get_graph_through_id (graph_Id.getId ())->second)
    return -1;

  return get_graph_through_id (graph_Id.getId ())->second->getVertexId (name);
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
diganaUndirectedGraph::add_vertex (diganaGraphObjectIdentifier id) {
   
   /*if (!id.is_valid ()) {
     std::cout << "ERROR : Invalid Vertex ID" << std::endl;
     return -1;
   }
   */
   std::string name = id.getName ();
   if (name.empty ()) name = ToString<int> (id.getId ());
   boost::graph_traits <diganaUndirectedGraphType>::vertex_descriptor v =
	                                           boost::add_vertex (graph);
   property_map<diganaUndirectedGraphType, vertex_name_t>::type VertexName =
	                                                    get(vertex_name, graph);
   property_map<diganaUndirectedGraphType, vertex_index2_t>::type VertexIdx2 =
	                                                    get(vertex_index2, graph);
   VertexIdx2[v] = getVCount();
   VertexName[v] = name;
   incVCount ();
   mapVertexIdAndName (diganaGraphObjectIdentifier(VertexIdx2[v], name));
   setNameIdMapInvoked ();
   //printf ("%s %d %x\n", name.c_str (), id.getId (), v); 
   return VertexIdx2[v];
}

bool diganaUndirectedGraph::check_if_edge_exists(int source , int sink) {
  
   if(source > getVCount() || sink > getVCount() ){
   std::cout << "Invalid vertex index " << std::endl;
    return 0;
   }
  boost::graph_traits <diganaUndirectedGraphType>::vertex_descriptor v = boost::vertex(source , graph);
  boost::graph_traits <diganaUndirectedGraphType>::vertex_descriptor u = boost::vertex(sink , graph);
  
  return boost::edge( u , v , graph ).second;
}

bool diganaDirectedGraph::check_if_edge_exists(int source , int sink) {
  
   if(source > getVCount() || sink > getVCount() ){
   std::cout << "Invalid vertex index " << std::endl;
    return 0;
   }
  boost::graph_traits <diganaDirectedGraphType>::vertex_descriptor v = boost::vertex(source , graph);
  boost::graph_traits <diganaDirectedGraphType>::vertex_descriptor u = boost::vertex(sink , graph);
   
  return boost::edge( u , v , graph ).second;
}


//Addedge for Undirected Graph
void
diganaUndirectedGraph::add_edge (diganaGraphObjectIdentifier source_id, diganaGraphObjectIdentifier sink_id) {
   int source = getVertexId (ToString<int> (source_id.getId ()));
   int sink = getVertexId (ToString<int> (sink_id.getId ()));
   if (!isNameIdMapInvoked ()) {
     source = source_id.getId ();
     sink = sink_id.getId ();
   }

   boost::graph_traits <diganaUndirectedGraphType>::vertex_descriptor u =
	                                               boost::vertex(source, graph);

   boost::graph_traits <diganaUndirectedGraphType>::vertex_descriptor v = 
	                                               boost::vertex(sink, graph);

   boost::add_edge (u, v, graph);
   boost::graph_traits <diganaUndirectedGraphType>::edge_descriptor e = 
                                                  boost::edge(u, v, graph).first;
}

//Addvertex for Directed Graph
int
diganaDirectedGraph::add_vertex (diganaGraphObjectIdentifier id) {

   /*if (!id.is_valid ()) {
     std::cout << "ERROR : Invalid Vertex ID" << std::endl;
     return -1;
   }
   */
   std::string name = id.getName ();
   if (name.empty ()) name = ToString<int> (id.getId ());
   boost::graph_traits <diganaDirectedGraphType>::vertex_descriptor v =
	                                           boost::add_vertex (graph);
   property_map<diganaDirectedGraphType, vertex_name_t>::type VertexName =
	                                                    get(vertex_name, graph);
   property_map<diganaDirectedGraphType, vertex_index2_t>::type VertexIdx2 =
	                                                    get(vertex_index2, graph);
   VertexIdx2[v] = getVCount();
   VertexName[v] = name;
   incVCount ();
   mapVertexIdAndName (diganaGraphObjectIdentifier(VertexIdx2[v], name));
   setNameIdMapInvoked ();
   return VertexIdx2[v];
}

void diganaGraphMgr::remove_vertex(std::string graph_name , int vertex_index ) {
   mapNameToGraph::iterator graph_itr_obj = get_graph_through_name(graph_name);
   if (graph_itr_obj == Name_Graph_Map.end() )
   { std::cout << "Invalid Graph Name" << std::endl;
     return ;}

   else
   return graph_itr_obj->second->remove_vertex(vertex_index);

   
}
void diganaDirectedGraph::remove_vertex( int vertex_index ) {
   
   boost::graph_traits <diganaDirectedGraphType>::vertex_descriptor v = boost::vertex(vertex_index , graph);
   if(vertex_index > getVCount() ){
   std::cout << "Invalid vertex index " << std::endl;
    return;
   }
   boost::clear_vertex(v , graph);
   boost::remove_vertex(v , graph);
}

void diganaUndirectedGraph::remove_vertex( int vertex_index ) {

   boost::graph_traits <diganaUndirectedGraphType>::vertex_descriptor v = boost::vertex(vertex_index , graph);
   if(vertex_index > getVCount() ){
   std::cout << "Invalid vertex index " << std::endl;
   }
   boost::clear_vertex(v , graph);
   boost::remove_vertex(v , graph);
}


//Addedge for Directed Graph
void
diganaDirectedGraph::add_edge (diganaGraphObjectIdentifier source_id, diganaGraphObjectIdentifier sink_id) {
   int source = getVertexId (ToString<int> (source_id.getId ()));
   int sink = getVertexId (ToString<int> (sink_id.getId ()));
   if (!isNameIdMapInvoked ()) {
     source = source_id.getId ();
     sink = sink_id.getId ();
   }

   boost::graph_traits <diganaDirectedGraphType>::vertex_descriptor u =
	                                               boost::vertex(source, graph);

   boost::graph_traits <diganaDirectedGraphType>::vertex_descriptor v = 
	                                               boost::vertex(sink, graph);

   boost::add_edge (u, v, graph);
}

void diganaGraph::mapVertexIdAndName (diganaGraphObjectIdentifier id) {
   printf ("Mapped name %s and id %d\n", id.getName ().c_str (), id.getId ());
   vertex_name_index_map.insert (std::pair<std::string, int> (id.getName (), id.getId ()));
}

int diganaGraph::getVertexId (std::string name) {
   std::map<std::string, int>::iterator it;
   it = vertex_name_index_map.find (name);
   if (it != vertex_name_index_map.end ())
     return vertex_name_index_map[name];

   return -1;   
}

void
diganaGraphMgr::dfs(std::string graph_name , int vertex_id) {

  diganaGraph * graph = NULL;
  mapNameToGraph::iterator graph_itr_obj = get_graph_through_name(graph_name);
  //if invalid name is provided return -1
   if (graph_itr_obj == Name_Graph_Map.end() ){
   cout << "Invalid Graph Name" << endl;
   return; }
   else{
   graph = graph_itr_obj->second;
   graph->register_vertex_property<bool> ("Visited");
   graph->register_vertex_property<bool> ("Explored");
   graph->register_edge_property<bool> ("Explored");

   diganaVertex v;
   diganaGraphIterator::vertex_iterator itr, eItr;
   itr.attach(graph_itr_obj->second);
   for (; itr != eItr; ++itr) {
    v = *itr;
    v.put_property<bool>("Explored", 0);
    v.put_property<bool>("Visited", 0);
   }

   diganaGraphIterator::adjacency_iterator ai , aietr;
   diganaGraphIterator::edge_iterator ei , eitr;
   ai.attach(vertex_id ,graph_itr_obj->second );
   std::stack<int> vstack;
   vstack.push(vertex_id);

   while (!vstack.empty()) {
    
    int vid = vstack.top();

   ai.attach(vid ,graph_itr_obj->second );
    vstack.pop();
    v = diganaVertex (vid, graph);
    v.put_property<bool> ("Explored", 1);
    if(v.get_property<bool> ("Explored") == 1) {
	cout << vid << "\n" ;}
 
    for(; ai != aietr ; ++ai) {
    v = *ai;
    if (v.get_property<bool> ("Explored") == 0 && v.get_property<bool> ("Visited") == 0) {
     v.put_property ("Visited", 1);
     vstack.push(v.getVertexId());
    }

    }}

    diganaEdge e;
    ei.attach (graph_itr_obj->second);
    int i = 0;
    for (; ei != eitr; ++ei) {
      e = *ei;
      if (i % 2 == 0)
        e.put_property <bool> ("Explored", true);
      else
        e.put_property <bool> ("Explored", false);
    }
    ei.attach (graph_itr_obj->second );
    for (; ei != eitr; ++ei) {
      e = *ei;
      if (e.get_property <bool> ("Explored") == true)
        cout << "The edge is between " << e.get_source_id () << " and " << e.get_sink_id () << "\n";
    }
}}

//adding vector of vector class : needs to be implemented as graph property for 
// preserving coarsened edges in multilevel partitioning
typedef std::pair <int , int >  ip ; 
class vector_of_vector {
public : 
std::vector< std::vector< ip > > vov ;

void insert_elems ( std::vector <ip> internal_vector )
        {
		{
		vov.push_back( internal_vector ); 
		}

    return ;
 }
void iterate_elems () {


for(std::vector<std::vector< ip > >::iterator it = vov.begin(); it != vov.end(); ++it)
	{
		
	for(std::vector< ip >::iterator jt = it->begin(); jt != it->end(); ++jt)
		{
		   std::cout << "source " << jt->first << " and sink " << jt->second << "\n"; 
	           std::cout << std::endl;
		}
	std::cout << std::endl;
	}
return ; 
}


};
