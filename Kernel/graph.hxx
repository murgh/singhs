//This is the graph header file for modelling the graph classes
#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/property_map/dynamic_property_map.hpp>
#include <string>
#include <fstream>
#include <map>

using namespace boost;
using namespace std;

enum diganaGraphType { 
                       diganaUndirectedGraphS, 
                       diganaDirectedGraphS, 
                       diganaNoGraphS
                     };

class diganaGraphObjectIdentifier;
class diganaGraphProperty;
class diganaGraph;
class diganaUndirectedGraph;
class diganaDirectedGraph;
class diganaGraphMgr; 

//Vertex Properties
typedef property<vertex_name_t, std::string, 
        property<vertex_index2_t, int> > diganaVertexProperties; 

//Edge Properties
typedef property<graph_name_t, std::string> diganaGraphProperties; 

//The undirected graph class
typedef boost::adjacency_list< listS, 
	                       listS,
			       undirectedS, 
	                       diganaVertexProperties, 
			       no_property, 
			       diganaGraphProperties, 
			       listS > 
			       diganaUndirectedGraphType;

//The directed graph class
typedef boost::adjacency_list< listS, 
	                       listS, 
			       directedS, 
			       diganaVertexProperties, 
			       no_property, 
			       diganaGraphProperties, 
			       listS > 
			       diganaDirectedGraphType;

//ID to Graph Map
typedef std::map< std::pair<int, std::string>, diganaGraph *> mapIdToGraph;
typedef std::map< std::string , diganaGraph *>  mapNameToGraph;
//The graph property manager class
class diganaGraphProperty {
  public:
   diganaGraphProperty () { } 
  private:
   boost::dynamic_properties vertex_properties;
   boost::dynamic_properties edge_properties;
   boost::dynamic_properties graph_properties;
};

//class object identifier to identify an object. Objects in our
//graph library are graph, vertices and edges. They can be 
//identified by either the names that they contain or their integer Ids
class diganaGraphObjectIdentifier {
  public:
   diganaGraphObjectIdentifier (int id, std::string str) :
                                identifier (std::pair<int, std::string> (id, str)) { }
   diganaGraphObjectIdentifier (const diganaGraphObjectIdentifier & object) {
     identifier = object.identifier;
   }
   diganaGraphObjectIdentifier () { }

   bool operator == (const diganaGraphObjectIdentifier &) const;
   void operator = (const diganaGraphObjectIdentifier &);
   std::string getName () const { return identifier.second;}
   int         getId   () const { return identifier.first; }  
   void        setName (std::string name) { identifier.second = name;}
   void        setId   (int id) { identifier.first = id; }  
   const std::pair<int, std::string> & get_identifier () const { return identifier; }

  private:
   std::pair<int, std::string> identifier;
};

//A singleton class, will contain all the
//graph manager information.
class diganaGraphMgr {
  public:
   int create_graph (diganaGraphObjectIdentifier & graph_Id, diganaGraphType type);
   bool graph_exists (diganaGraphObjectIdentifier & graph_Id);
   mapNameToGraph::iterator get_graph_through_name(std::string graph_name);
   int add_vertex(std::string , std::string);
   void add_edge(std::string , int , int );  
   int getId (std::string );
   void setId ( std::string , int  ); 
   int getVCount (std::string name); 
   template<typename Value> void register_vertex_property (std::string, std::string); 
   template<typename Value> void register_edge_property (std::string, std::string); 
/*   int insert_vertex (diganaGraphObjectIdentifier graph_Id, std::string name);
   void insert_edge (std::string graph,   
*/
  private:
   mapIdToGraph Id_Graph_Map;
   mapNameToGraph Name_Graph_Map;
   int Graph_Count;
};


//It is the base graph class 
class diganaGraph {
  public:
   //Default constructor
   diganaGraph (diganaGraphObjectIdentifier objId, 
                diganaGraphType t) : identifier(objId) , type (t) { 
     vertexCount = 0;
     edgeCount = 0;
   }

   //Copy constructor
   diganaGraph (const diganaGraph & object) {
     identifier = object.identifier;
     type = object.type;
   }
                                                    
   std::string getName () const { return identifier.getName (); } 
   int         getId   () const { return identifier.getId (); }
   void        setId   (int id) { identifier.setId (id); } 
   int         getVCount () const { return vertexCount; }
   void	       incVCount () { vertexCount++; }

   virtual int add_vertex (std::string) { return 0;}
   virtual void add_edge (int, int) { }

  private:
   diganaGraphObjectIdentifier identifier;
   diganaGraphType type;
   int vertexCount, edgeCount;
};


class diganaUndirectedGraph : public diganaGraph {
  public:
   diganaUndirectedGraph (diganaGraphObjectIdentifier objId,
                          diganaGraphType t) :
   diganaGraph (objId, t) { 
     properties = NULL;
   }
   int add_vertex (std::string);
   void add_edge (int, int);
   template<typename Value> void register_vertex_property (std::string); 
   template<typename Value> void register_edge_property (std::string); 
      
  private:
   diganaUndirectedGraphType graph;
   diganaGraphProperty * properties;
};

class diganaDirectedGraph : public diganaGraph {
  public:
   diganaDirectedGraph (diganaGraphObjectIdentifier objId,
                          diganaGraphType t) :
   diganaGraph (objId, t) { 
     properties = NULL;
   }
   int add_vertex (std::string);
   void add_edge (int, int);
   template<typename Value> void register_vertex_property (std::string); 
   template<typename Value> void register_edge_property (std::string); 

  private:
   diganaDirectedGraphType graph;
   diganaGraphProperty * properties;
};
