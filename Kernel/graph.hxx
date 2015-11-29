//This is the graph header file for modelling the graph classes

#ifndef DIGANA_GRAPH
#define DIGANA_GRAPH


#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/property_map/dynamic_property_map.hpp>
#include <string>
#include <fstream>
#include <map>

using namespace boost;


enum diganaGraphType { 
                       diganaUndirectedGraphS, 
                       diganaDirectedGraphS, 
                       diganaNoGraphS
                     };
const int Null_Identifier_ID = -1;

class diganaGraphObjectIdentifier;
class diganaGraphProperty;
class diganaGraph;
class diganaUndirectedGraph;
class diganaDirectedGraph;
class diganaGraphMgr; 

//
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
typedef std::map< int, diganaGraph *> mapIdToGraph;
typedef std::map< std::string , diganaGraph *>  mapNameToGraph;
typedef std::map< int , diganaGraph *>  mapNumIdToGraph; 

//The graph property manager class
class diganaGraphPropertyMap { 
  public:
   virtual ~diganaGraphPropertyMap () { }
};

template<typename Key, typename Value>
class diganaGraphPropertyMapData : public diganaGraphPropertyMap {
  private:
    std::map<Key, Value> kvmap;
    boost::associative_property_map< std::map<Key, Value> > assomap;

  public:
    diganaGraphPropertyMapData () {
      boost::associative_property_map< std::map<Key, Value> > temp (kvmap);
      assomap = temp;
    }        
 
   ~diganaGraphPropertyMapData () {
      kvmap.clear ();
   } 

   boost::associative_property_map< std::map<Key, Value> > & associative_map () { return assomap; }
};

class diganaGraphProperty {
  public:
   diganaGraphProperty () { } 
   boost::dynamic_properties & vertex_properties () { return vertex_properties_; }
   boost::dynamic_properties & edge_properties () { return edge_properties_; }
   boost::dynamic_properties & graph_properties () { return graph_properties_; }

   template<typename Key, typename Value> void register_vertex_property (std::string name) {
     diganaGraphPropertyMapData<Key, Value> * propMapData = new diganaGraphPropertyMapData<Key, Value>;
     vertex_propert_map_.insert ( std::pair<std::string, diganaGraphPropertyMap*> (name, propMapData));  
     vertex_properties_.property (name, propMapData->associative_map ());
   } 

   template<typename Key, typename Value> void register_edge_property (std::string name) {
     diganaGraphPropertyMapData<Key, Value> * propMapData = new diganaGraphPropertyMapData<Key, Value>;
     edge_propert_map_.insert ( std::pair<std::string, diganaGraphPropertyMap*> (name, propMapData));  
     edge_properties_.property (name, propMapData->associative_map ());
   } 

  private:
   boost::dynamic_properties vertex_properties_;
   boost::dynamic_properties edge_properties_;
   boost::dynamic_properties graph_properties_;
   std::map<std::string, diganaGraphPropertyMap *> vertex_propert_map_;
   std::map<std::string, diganaGraphPropertyMap *> edge_propert_map_;
};

//The base class for the dynamic property type if any application wants to
//add a new property on the graph.
class diganaDynamicGraphProperty : public std::string
 { };

//Class object identifier to identify an object. Objects in our
//graph library are graph, vertices and edges. They can be 
//identified by either the names that they contain or their integer Ids
class diganaGraphObjectIdentifier {
  public:
   diganaGraphObjectIdentifier (int id, std::string str) :
                                identifier (std::pair<int, std::string> (id, str)) { }
   diganaGraphObjectIdentifier (const diganaGraphObjectIdentifier & object) {
     identifier = object.identifier;
   }
   diganaGraphObjectIdentifier () :
	identifier (std::pair<int, std::string> (Null_Identifier_ID, std::string())) { }

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
   static diganaGraphMgr & getGraphMgr () {
     static diganaGraphMgr graphMgrInst;
     return graphMgrInst; 
   }
   int create_graph (diganaGraphObjectIdentifier & graph_Id, diganaGraphType type);
   bool graph_exists (diganaGraphObjectIdentifier & graph_Id);
   mapNameToGraph::iterator get_graph_through_name(std::string graph_name);
   mapIdToGraph::iterator get_graph_through_id(int graph_id);
   int add_vertex(std::string , std::string);
   void remove_vertex(std::string , int);
   bool check_if_edge_exists(std::string , int , int  );
   void add_edge(std::string , int , int );  
   int getId (std::string);
   void setId (std::string , int); 
   int getVCount (std::string name); 
   bool check_graph_identifier (diganaGraphObjectIdentifier &);
   bool check_vertex_id (diganaGraphObjectIdentifier &, int);
   template<typename Value> void register_vertex_property (std::string, std::string); 
   template<typename Value> void register_edge_property (std::string, std::string); 
   template<typename Value> const Value get_vertex_property (int, std::string, std::string); 
   template<typename Value> void put_vertex_property (int, std::string, std::string, Value); 
   template<typename Value> const Value get_edge_property (int, int, std::string, std::string); 
   template<typename Value> void put_edge_property (int, int, std::string, std::string, Value); 

/*   int insert_vertex (diganaGraphObjectIdentifier graph_Id, std::string name);
   void insert_edge (std::string graph,   
*/

  private:
   //Private constructor to not let the object being created
   //for this class
   diganaGraphMgr () {
     Graph_Count = 0;
   }
   diganaGraphMgr (diganaGraphMgr const &);
   void operator = (diganaGraphMgr const &);

   mapIdToGraph Id_Graph_Map;
   mapNameToGraph Name_Graph_Map;
   mapNumIdToGraph Num_Id_Graph_Map;
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
   diganaGraphType getType () const { return type; }

   virtual int add_vertex (std::string) { return 0;}
   virtual bool check_if_edge_exists( int , int ){ return 0; };
   virtual void remove_vertex (int){}
   virtual void add_edge (int, int) { }
   //virtual void print_vertex_prop(int){}

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
   bool check_if_edge_exists( int , int );
   void remove_vertex(int);
   void add_edge (int, int);
   void init_property () { if (!properties) properties = new diganaGraphProperty; }
   //void print_vertex_prop(int);
   template<typename Value> void register_vertex_property (std::string); 
   template<typename Value> void register_edge_property (std::string); 
   template<typename Value> const Value get_vertex_property (int, std::string); 
   template<typename Value> void put_vertex_property (int, std::string, Value); 
   template<typename Value> const Value get_edge_property (int, int, std::string); 
   template<typename Value> void put_edge_property (int, int, std::string, Value); 
      
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
   bool check_if_edge_exists( int , int );
   void remove_vertex(int);
   void add_edge (int, int);
   void init_property () { if (!properties) properties = new diganaGraphProperty; }
   //void print_vertex_prop(int);
   template<typename Value> void register_vertex_property (std::string); 
   template<typename Value> void register_edge_property (std::string); 
   template<typename Value> const Value get_vertex_property (int, std::string); 
   template<typename Value> void put_vertex_property (int, std::string, Value); 
   template<typename Value> const Value get_edge_property (int, int, std::string); 
   template<typename Value> void put_edge_property (int, int, std::string, Value); 

  private:
   diganaDirectedGraphType graph;
   diganaGraphProperty * properties;
};
#endif //DIGANA_GRAPH
