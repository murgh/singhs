//This is the graph header file for modelling the graph classes

#ifndef DIGANA_GRAPH
#define DIGANA_GRAPH


#include <boost/graph/adjacency_list.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/property_map/dynamic_property_map.hpp>
#include <boost/variant.hpp>
#include <string>
#include <fstream>
#include <map>
#include <iostream>
#include <sstream>
#include <iomanip> 

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

typedef boost::variant <diganaDirectedGraph, diganaUndirectedGraph> diganaGraphVariant;

//Vertex Properties
typedef property<vertex_name_t, std::string, 
        property<vertex_index2_t, int> > diganaVertexProperties; 

//Edge Properties
typedef property<graph_name_t, std::string> diganaGraphProperties; 

//The undirected graph class
typedef boost::adjacency_list< listS, 
	                       vecS,
			       undirectedS, 
	                       diganaVertexProperties, 
			       no_property, 
			       diganaGraphProperties, 
			       listS > 
			       diganaUndirectedGraphType;

//The directed graph class
typedef boost::adjacency_list< listS, 
	                       vecS, 
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
 { 
   typedef std::pair <std::pair <void *, void *>, void *> PropMapVal;
   typedef std::map <std::string, PropMapVal> PropNameMap; 
   typedef void (*set_property_fn) (std::string, std::string);
   typedef std::string (*get_property_fn) (std::string);
   typedef void (*print_property_fn) (std::string);

   public:
     virtual void register_tcl_callbacks () {
	     return;
     }
   protected:  
     //Register the sub property
     void register_sp (std::string name, PropMapVal fnPtrs) {
       if (!isSubPropertyRegistered (name))
	 nameFnMap.insert (std::pair <std::string, PropMapVal>(name, fnPtrs));
     }

     //Get sub property
     std::string get_sp (std::string name) {
       if (!isSubPropertyRegistered (name)) {
	 printf ("Error : Property %s is not registered\n", name.c_str ());
	 return std::string ("");
       }	 
       PropMapVal & fnPair = nameFnMap.find (name)->second;
       get_property_fn function = (get_property_fn) fnPair.first.first;	
       return function (name);//Get API
     }

     //Set sup property
     void set_sp (std::string name, std::string prop) {
       if (isSubPropertyRegistered (name)) {
	 printf ("Error : Property %s is not registered\n", name.c_str ());
	 return;
       }	 
       PropMapVal & fnPair = nameFnMap.find (name)->second;
       set_property_fn function = (set_property_fn) fnPair.first.second;
       function (name, prop);//Set API
     }

     //Print the sub property
     void print_sp (std::string name) {
       if (!isSubPropertyRegistered (name)) {
	 printf ("Error : Property %s is not registered\n", name.c_str ());
	 return; 
       }	 
       PropMapVal & fnPair = nameFnMap.find (name)->second;
       print_property_fn function = (print_property_fn) fnPair.second; 
       function (name);
     }

   private:
     PropNameMap nameFnMap;

     bool isSubPropertyRegistered (std::string name, bool issueError = false) {
       PropNameMap::iterator it;
       it = nameFnMap.find (name);
       if (it != nameFnMap.end ())
	 return true;

       return false;
     }
 };

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
   diganaGraphObjectIdentifier (int id) :
	identifier (std::pair<int, std::string> (id, std::string())) { }
   diganaGraphObjectIdentifier (std::string str) :
	identifier (std::pair<int, std::string> (Null_Identifier_ID, str)) { }

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
   diganaGraph * create_graph (diganaGraphObjectIdentifier & graph_Id, diganaGraphType type, int size = 0);
   diganaGraph * get_graph (diganaGraphObjectIdentifier graph_Id);
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
   int  getVertexId (diganaGraphObjectIdentifier &, std::string);
   void dfs(std::string , int );
   template<typename Value> void register_vertex_property (std::string, std::string); 
   template<typename Value> void register_edge_property (std::string, std::string); 

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
                diganaGraphType t, int size) : identifier(objId) , type (t) { 
     vertexCount = size;
     edgeCount = 0;
     properties = NULL;
     theVertexNameIdxMap = false;
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
   int         getVertexId (std::string); 
   void mapVertexIdAndName (diganaGraphObjectIdentifier);
   void init_property () { if (!properties) properties = new diganaGraphProperty; }
   diganaGraphProperty * get_properties () { return properties; }
   bool isNameIdMapInvoked () { return theVertexNameIdxMap; }
   void setNameIdMapInvoked () { theVertexNameIdxMap = true; }

   virtual int add_vertex (diganaGraphObjectIdentifier) { return 0;}
   virtual bool check_if_edge_exists( int , int ){ return 0; };
   virtual void remove_vertex (int){}
   virtual void add_edge (diganaGraphObjectIdentifier, diganaGraphObjectIdentifier) { }
   // virtual void dfs(int ){}
   //virtual void print_vertex_prop(int){}

   template<typename Value> void register_vertex_property (std::string name) {
     init_property ();
     if (type == diganaUndirectedGraphS) {
       properties->register_vertex_property<
	        diganaUndirectedGraphType::vertex_descriptor, Value> (name);
     } else if (type == diganaDirectedGraphS) {
       properties->register_vertex_property<
	        diganaDirectedGraphType::vertex_descriptor, Value> (name);
     }
   }


   template<typename Value> void register_edge_property (std::string name) {
     init_property ();
     if (type == diganaUndirectedGraphS) {
       properties->register_edge_property<
	        diganaUndirectedGraphType::edge_descriptor, Value> (name);
     } else if (type == diganaDirectedGraphS) {
       properties->register_edge_property<
	        diganaDirectedGraphType::edge_descriptor, Value> (name);
     }
   }

  private:
   diganaGraphObjectIdentifier identifier;
   diganaGraphType type;
   int vertexCount, edgeCount;
   std::map<std::string, int> vertex_name_index_map;
   diganaGraphProperty * properties;
   bool theVertexNameIdxMap;
};


class diganaUndirectedGraph : public diganaGraph {
  public:
   diganaUndirectedGraph (diganaGraphObjectIdentifier objId,
                          diganaGraphType t, int size) :
   diganaGraph (objId, t, size) { 
     graph = diganaUndirectedGraphType (size);		
     property_map<diganaUndirectedGraphType, vertex_name_t>::type VertexName =
	                                                      get(vertex_name, graph);
     property_map<diganaUndirectedGraphType, vertex_index2_t>::type VertexIdx2 =
                                                                   get(vertex_index2, graph);	   
     for (int i = 0; i < size; i++) {
	std::string name = std::to_string (i);
        boost::graph_traits <diganaUndirectedGraphType>::vertex_descriptor v = boost::vertex (i, graph);
	VertexIdx2[v] = i;
	VertexName[v] = name;
	//mapVertexIdAndName (diganaGraphObjectIdentifier(VertexIdx2[v], name));
	//setNameIdMapInvoked ();
     }
   }
   int add_vertex (diganaGraphObjectIdentifier);
   bool check_if_edge_exists( int , int );
   void remove_vertex(int);
   void add_edge (diganaGraphObjectIdentifier, diganaGraphObjectIdentifier);
  // void dfs(int);
   diganaUndirectedGraphType & getGraph () { return graph; }
   //void print_vertex_prop(int);
      
  private:
   diganaUndirectedGraphType graph;
};

class diganaDirectedGraph : public diganaGraph {
  public:
   diganaDirectedGraph (diganaGraphObjectIdentifier objId,
                          diganaGraphType t, int size) :
   diganaGraph (objId, t, size) { 
     graph = diganaDirectedGraphType (size);
     property_map<diganaDirectedGraphType, vertex_name_t>::type VertexName =
	                                                      get(vertex_name, graph);
     property_map<diganaDirectedGraphType, vertex_index2_t>::type VertexIdx2 =
                                                              get(vertex_index2, graph);	   
     for (int i = 0; i < size; i++) {
	std::string name = std::to_string (i);     
        boost::graph_traits <diganaDirectedGraphType>::vertex_descriptor v = boost::vertex (i, graph);
	VertexIdx2[v] = i;
	VertexName[v] = name;
	//mapVertexIdAndName (diganaGraphObjectIdentifier(VertexIdx2[v], name));
	//setNameIdMapInvoked ();
     }
   }

   int add_vertex (diganaGraphObjectIdentifier);
   bool check_if_edge_exists( int , int );
   void remove_vertex(int);
   void add_edge (diganaGraphObjectIdentifier, diganaGraphObjectIdentifier);
//   void dfs(int);
   diganaDirectedGraphType & getGraph () { return graph; }
   //void print_vertex_prop(int);

  private:
   diganaDirectedGraphType graph;
};


class diganaVertex {
  private:
   int vertexID;
   diganaGraph * parent_graph;


  public:
   diganaVertex () :
              vertexID (-1),
              parent_graph (NULL) 
   { }

   diganaVertex (int vid, diganaGraph * g) : 
              vertexID (vid),
              parent_graph (g) 
   { }

   diganaVertex (const diganaVertex & v) : 
              vertexID (v.vertexID),
              parent_graph (v.parent_graph) 
   { }

   ~diganaVertex () {
     vertexID = -1;
     parent_graph = NULL;
   } 

   diganaVertex operator = (const diganaVertex & v) {
	vertexID = v.vertexID;
	parent_graph = v.parent_graph;
	return *this;
   }
   
   bool operator == (const diganaVertex & v) {
	return (vertexID == v.vertexID && parent_graph == v.parent_graph);
   }
   
   template<typename Value> Value get_property (std::string name) { 
	    
     Value val;
     boost::dynamic_properties & d_property = 
            parent_graph->get_properties ()->vertex_properties (); 
     if (parent_graph->getType () == diganaUndirectedGraphS) {
       diganaUndirectedGraphType::vertex_descriptor key = 
	                     boost::vertex(vertexID, ((diganaUndirectedGraph *)parent_graph)->getGraph ()); 
       val =  boost::get<Value> (name, d_property, key);
     } else if (parent_graph->getType () == diganaDirectedGraphS) {
       diganaDirectedGraphType::vertex_descriptor key = 
	                     boost::vertex(vertexID, ((diganaDirectedGraph *)parent_graph)->getGraph ()); 
       val =  boost::get<Value> (name, d_property, key);
	     
     }
     return val;
   }

   template<typename Value> void put_property (std::string name, Value val) { 

     boost::dynamic_properties & d_property = 
            parent_graph->get_properties ()->vertex_properties (); 
     if (parent_graph->getType () == diganaUndirectedGraphS) {
       diganaUndirectedGraphType::vertex_descriptor key = 
	                     boost::vertex(vertexID, ((diganaUndirectedGraph *) parent_graph)->getGraph ()); 
       boost::put (name, d_property, key, val);
     } else if (parent_graph->getType () == diganaDirectedGraphS) {
       diganaDirectedGraphType::vertex_descriptor key = 
	                     boost::vertex(vertexID, ((diganaDirectedGraph *) parent_graph)->getGraph ()); 
       boost::put (name, d_property, key, val);
     }
   }

   diganaGraph * getParentGraph () { return parent_graph; }
   int getVertexId () { return vertexID; }

   bool operator != (diganaVertex & other) {
    if ((this->parent_graph == other.parent_graph) &&
	(this->vertexID == other.vertexID)) 
      return false;
    return true;
   }
   private:
    //new cannot be called for this object it will always be a local variable.
    void * operator new   (size_t);
    void * operator new[] (size_t);
    void   operator delete   (void *);
    void   operator delete[] (void*);
};

class diganaEdge {
  private:
   int source;
   int sink;
   diganaGraph * parent_graph;
  public:
   diganaEdge () :
              source (-1),
              sink (-1),
              parent_graph (NULL) 
   { }

   diganaEdge (int sourceid, int sinkid, diganaGraph * g) : 
              source (sourceid),
              sink (sinkid),
              parent_graph (g) 
   { }

   diganaEdge (diganaVertex src, diganaVertex snk) :
	      source (src.getVertexId ()),
      	      sink (snk.getVertexId ()),
	      parent_graph (src.getParentGraph ())
   { assert (src.getParentGraph () == snk.getParentGraph ()); }	      

   diganaEdge (const diganaEdge & e) : 
              source (e.source),
              sink (e.sink),
              parent_graph (e.parent_graph) 
   { }

   ~diganaEdge () {
     source = sink = -1;
     parent_graph = NULL;
   } 
   
   template<typename Value> Value get_property (std::string name) { 

     Value val;
     boost::dynamic_properties & d_property = 
            parent_graph->get_properties ()->edge_properties (); 
     if (parent_graph->getType () == diganaUndirectedGraphS) {
       diganaUndirectedGraphType::edge_descriptor key = 
	        boost::edge(boost::vertex (source, ((diganaUndirectedGraph *) parent_graph)->getGraph ()), 
		            boost::vertex (sink, ((diganaUndirectedGraph *) parent_graph)->getGraph ()), 
			    ((diganaUndirectedGraph *) parent_graph)->getGraph ()).first;
       val = boost::get<Value> (name, d_property, key);
     } else if (parent_graph->getType () == diganaDirectedGraphS) {
       diganaDirectedGraphType::edge_descriptor key = 
	        boost::edge(boost::vertex (source, ((diganaDirectedGraph *)parent_graph)->getGraph ()), 
		            boost::vertex (sink, ((diganaDirectedGraph *)parent_graph)->getGraph ()), 
			    ((diganaDirectedGraph *)parent_graph)->getGraph ()).first;
       val = boost::get<Value> (name, d_property, key);
     }
     return val;
   }

   template<typename Value> void put_property (std::string name, Value val) { 

     boost::dynamic_properties & d_property = 
            parent_graph->get_properties ()->edge_properties (); 
     if (parent_graph->getType () == diganaUndirectedGraphS) {
       diganaUndirectedGraphType::edge_descriptor key = 
	        boost::edge(boost::vertex (source, ((diganaUndirectedGraph *)parent_graph)->getGraph ()), 
		            boost::vertex (sink, ((diganaUndirectedGraph *)parent_graph)->getGraph ()), 
			    ((diganaUndirectedGraph *) parent_graph)->getGraph ()).first;
       boost::put (name, d_property, key, val);
     } else if (parent_graph->getType () == diganaDirectedGraphS) {
       diganaDirectedGraphType::edge_descriptor key = 
	        boost::edge(boost::vertex (source, ((diganaDirectedGraph *)parent_graph)->getGraph ()), 
		            boost::vertex (sink, ((diganaDirectedGraph *) parent_graph)->getGraph ()), 
			    ((diganaDirectedGraph *) parent_graph)->getGraph ()).first;
       boost::put (name, d_property, key, val);
     }
   }

   int get_source_id(){return source;}
   int get_sink_id(){return sink;}
   private:
    //new cannot be called for this object it will always be a local variable.
    void * operator new   (size_t);
    void * operator new[] (size_t);
    void   operator delete   (void *);
    void   operator delete[] (void*);
};

namespace diganaGraphIterator {
  class adjacency_iterator {
   private :
    typename boost::graph_traits<diganaDirectedGraphType>::adjacency_iterator dir_begin, dir_end;
    typename boost::graph_traits<diganaUndirectedGraphType>::adjacency_iterator u_begin, u_end;
    diganaVertex attached_vertex;

   public:
    adjacency_iterator () : attached_vertex (diganaVertex ()) 
    {}

    void attach (int vertex, diganaGraph * graph) {
     if (graph->getType () == diganaUndirectedGraphS) {
       diganaUndirectedGraphType::vertex_descriptor v = boost::vertex (vertex, ((diganaUndirectedGraph *)graph)->getGraph ()); 
       boost::tie (u_begin, u_end) = boost::adjacent_vertices (v, ((diganaUndirectedGraph *)graph)->getGraph ());
     } else if (graph->getType () == diganaDirectedGraphS) {
       diganaDirectedGraphType::vertex_descriptor v = boost::vertex (vertex, ((diganaDirectedGraph *)graph)->getGraph ()); 
       boost::tie (dir_begin, dir_end) = boost::adjacent_vertices (v, ((diganaDirectedGraph *)graph)->getGraph ());
     }
     attached_vertex = diganaVertex (vertex, graph);
    }	  

    void attach (diganaVertex & V) {
      attach (V.getVertexId (), V.getParentGraph ());
    }

    diganaVertex operator * () {
     diganaVertex vertex;
     diganaGraph * graph = attached_vertex.getParentGraph ();
     if (graph->getType () == diganaUndirectedGraphS) {
       diganaUndirectedGraphType::vertex_descriptor uv = *u_begin;
       typename property_map<diganaUndirectedGraphType, vertex_index2_t>::type VIdx2 =
             boost::get(vertex_index2, ((diganaUndirectedGraph *)(attached_vertex.getParentGraph ()))->getGraph ());
       vertex = diganaVertex(VIdx2[uv], attached_vertex.getParentGraph ());   
     } else if (graph->getType () == diganaDirectedGraphS) {
       diganaDirectedGraphType::vertex_descriptor dv = *dir_begin;
       typename property_map<diganaDirectedGraphType, vertex_index2_t>::type VIdx2 =
             boost::get(vertex_index2, ((diganaDirectedGraph *)(attached_vertex.getParentGraph ()))->getGraph ());
       vertex = diganaVertex(VIdx2[dv], attached_vertex.getParentGraph ());   
     }
     return vertex;
    }

    diganaVertex operator ++ () {
     diganaGraph * graph = attached_vertex.getParentGraph ();
     if (graph->getType () == diganaUndirectedGraphS) ++u_begin;
     if (graph->getType () == diganaDirectedGraphS) ++dir_begin;
     return *(*this);
    }
  
    bool operator != (adjacency_iterator & other) {
     diganaGraph * graph = attached_vertex.getParentGraph ();
     if (graph->getType () == diganaUndirectedGraphS && u_begin != u_end) return true;
     if (graph->getType () == diganaDirectedGraphS && dir_begin != dir_end) return true;
     return false;
    }
  };

  class vertex_iterator  {
   private :
    typename boost::graph_traits<diganaDirectedGraphType>::vertex_iterator dir_begin, dir_end;
    typename boost::graph_traits<diganaUndirectedGraphType>::vertex_iterator u_begin, u_end;
    diganaGraph * graph;

   public:
    vertex_iterator () : graph(NULL) 
    {}

    void attach (diganaGraph * g) {
     graph = g;
     if (graph->getType () == diganaUndirectedGraphS) {
       boost::tie (u_begin, u_end) = boost::vertices (((diganaUndirectedGraph *)graph)->getGraph ());
     } else if (graph->getType () == diganaDirectedGraphS) {
       boost::tie (dir_begin, dir_end) = boost::vertices (((diganaDirectedGraph *)graph)->getGraph ());
     }
    }	  

    diganaVertex operator * () {
     diganaVertex vertex;
     if (graph->getType () == diganaUndirectedGraphS) {
       diganaUndirectedGraphType::vertex_descriptor uv = *u_begin;
       typename property_map<diganaUndirectedGraphType, vertex_index2_t>::type VIdx2 =
             boost::get(vertex_index2, ((diganaUndirectedGraph *)graph)->getGraph ());
       vertex = diganaVertex(VIdx2[uv], graph);   
     } else if (graph->getType () == diganaDirectedGraphS) {
       diganaDirectedGraphType::vertex_descriptor dv = *dir_begin;
       typename property_map<diganaDirectedGraphType, vertex_index2_t>::type VIdx2 =
             boost::get(vertex_index2, ((diganaDirectedGraph *) graph)->getGraph ());
       vertex = diganaVertex(VIdx2[dv], graph);   
     }
     return vertex;
    }

    diganaVertex operator ++ () {
     if (graph->getType () == diganaUndirectedGraphS) ++u_begin;
     if (graph->getType () == diganaDirectedGraphS) ++dir_begin;
     return *(*this);
    }
  
    bool operator != (vertex_iterator & other) {
     if (graph->getType () == diganaUndirectedGraphS && u_begin != u_end) return true;
     if (graph->getType () == diganaDirectedGraphS && dir_begin != dir_end) return true;
     return false;
    }
  };
	


  class edge_iterator {
   private :
    diganaGraph* graph;
    typename boost::graph_traits<diganaUndirectedGraphType>::edge_iterator u_begin , u_end;
    typename boost::graph_traits<diganaDirectedGraphType>::edge_iterator dir_begin , dir_end;
   public:
    void attach (diganaGraph * g) {
      graph = g;
      if (graph->getType () == diganaUndirectedGraphS) {
       	boost::tie (u_begin, u_end) = boost::edges (((diganaUndirectedGraph *)graph)->getGraph ());
      } 
      if (graph->getType () == diganaDirectedGraphS) {
        boost::tie (dir_begin, dir_end) = boost::edges (((diganaDirectedGraph *)graph)->getGraph ());
      } 
    }

    diganaEdge operator * () {
      diganaEdge edgex;  	
      if (graph->getType () == diganaUndirectedGraphS) {
       	diganaUndirectedGraphType::edge_descriptor ue = *u_begin;
	diganaUndirectedGraphType::vertex_descriptor u = boost::source(ue , ((diganaUndirectedGraph *) graph)->getGraph() );
        diganaUndirectedGraphType::vertex_descriptor v = boost::target(ue , ((diganaUndirectedGraph *) graph)->getGraph() );
	 typename property_map<diganaUndirectedGraphType, vertex_index2_t>::type VIdx2 =
             boost::get(vertex_index2, ((diganaUndirectedGraph *)graph)->getGraph ());
	int sink = VIdx2[v];
	int source = VIdx2[u];
	diganaEdge edgex(source , sink ,((diganaUndirectedGraph *) graph));
	return edgex;
      } else if (graph->getType () == diganaDirectedGraphS) {
        diganaDirectedGraphType::edge_descriptor de = *dir_begin;
        diganaDirectedGraphType::vertex_descriptor u = boost::source(de , ((diganaDirectedGraph *) graph)->getGraph() );
        diganaDirectedGraphType::vertex_descriptor v = boost::target(de , ((diganaDirectedGraph *) graph)->getGraph() );
         typename property_map<diganaDirectedGraphType, vertex_index2_t>::type VIdx2 =
             boost::get(vertex_index2, ((diganaDirectedGraph *)graph)->getGraph ());
        int sink = VIdx2[v];
        int source = VIdx2[u];
        diganaEdge edgex(source , sink ,((diganaDirectedGraph *) graph));
        return edgex;
      } 
     return edgex; 
   }

   diganaEdge operator ++ () {
     if (graph->getType () == diganaUndirectedGraphS) ++u_begin;    
     if (graph->getType () == diganaDirectedGraphS) ++dir_begin;
     return *(*this);
   }

    bool operator != (edge_iterator & other) {
      if (graph->getType () == diganaUndirectedGraphS && u_begin != u_end) return true;
      if (graph->getType () == diganaDirectedGraphS && dir_begin != dir_end) return true;
      return false;
    }
  };

  class in_edge_iterator {
   private :
    diganaGraph* graph;
    diganaVertex attached_vertex;
    typename boost::graph_traits<diganaUndirectedGraphType>::in_edge_iterator in_begin_u , in_end_u;

   public :
    in_edge_iterator () {
      graph = NULL;
    }

    void attach(int vertex_id , diganaGraph* g){
      graph = g;
      if (graph->getType () == diganaUndirectedGraphS) {
        diganaUndirectedGraphType::vertex_descriptor v = boost::vertex (vertex_id, ((diganaUndirectedGraph *)graph)->getGraph ()); 
	boost::tie (in_begin_u, in_end_u) = boost::in_edges ( v , ((diganaUndirectedGraph *)graph)->getGraph ());
      }

      attached_vertex = diganaVertex (vertex_id, graph);
    }

    diganaEdge operator * () {
      diganaEdge edgex;
      if (graph->getType () == diganaUndirectedGraphS) {
        diganaUndirectedGraphType::edge_descriptor ue = *in_begin_u;
        diganaUndirectedGraphType::vertex_descriptor u = boost::source(ue , ((diganaUndirectedGraph *) graph)->getGraph() );
        diganaUndirectedGraphType::vertex_descriptor v = boost::target(ue , ((diganaUndirectedGraph *) graph)->getGraph() );
         typename property_map<diganaUndirectedGraphType, vertex_index2_t>::type VIdx2 =
             boost::get(vertex_index2, ((diganaUndirectedGraph *)graph)->getGraph ());
        int sink = VIdx2[v];
        int source = VIdx2[u];
        diganaEdge edgex(source , sink ,((diganaUndirectedGraph *) graph));
        return edgex;
      }
      return edgex;
    }

    diganaEdge operator ++ () {
      if (graph->getType () == diganaUndirectedGraphS) ++in_begin_u;
      return *(*this);
    }

    bool operator != (in_edge_iterator & other) {
      if (graph->getType () == diganaUndirectedGraphS && in_begin_u != in_end_u) return true;
      return false;
    }
  };



  class out_edge_iterator {
	
   private :
    diganaGraph* graph;
    diganaVertex attached_vertex;
    typename boost::graph_traits<diganaDirectedGraphType>::out_edge_iterator out_begin_d , out_end_d;
    typename boost::graph_traits<diganaUndirectedGraphType>::out_edge_iterator out_begin_u , out_end_u;

   public :
    out_edge_iterator () {
      graph = NULL;
    }

    void attach(int vertex_id , diganaGraph* g){
      graph = g;
      if (graph->getType () == diganaDirectedGraphS) {
        diganaDirectedGraphType::vertex_descriptor v = boost::vertex (vertex_id, ((diganaDirectedGraph *)graph)->getGraph ());
        boost::tie (out_begin_d, out_end_d) = boost::out_edges ( v , ((diganaDirectedGraph *)graph)->getGraph ());
      } else if (graph->getType () == diganaUndirectedGraphS) {
        diganaDirectedGraphType::vertex_descriptor v = boost::vertex (vertex_id, ((diganaDirectedGraph *)graph)->getGraph ());
        boost::tie (out_begin_u, out_end_u) = boost::out_edges ( v , ((diganaUndirectedGraph *)graph)->getGraph ());
      }

      attached_vertex = diganaVertex (vertex_id, graph);
    }

    diganaEdge operator * () {
          diganaEdge edgex;
          if (graph->getType () == diganaDirectedGraphS) {
            diganaDirectedGraphType::edge_descriptor ue = *out_begin_d;
            diganaDirectedGraphType::vertex_descriptor u = boost::source(ue , ((diganaDirectedGraph *) graph)->getGraph() );
            diganaDirectedGraphType::vertex_descriptor v = boost::target(ue , ((diganaDirectedGraph *) graph)->getGraph() );
             typename property_map<diganaDirectedGraphType, vertex_index2_t>::type VIdx2 =
                 boost::get(vertex_index2, ((diganaDirectedGraph *)graph)->getGraph ());
            int sink = VIdx2[v];
            int source = VIdx2[u];
            diganaEdge edgex(source , sink ,((diganaDirectedGraph *) graph));
            return edgex;
          } else if (graph->getType () == diganaUndirectedGraphS) {
            diganaUndirectedGraphType::edge_descriptor ue = *out_begin_u;
            diganaUndirectedGraphType::vertex_descriptor u = boost::source(ue , ((diganaUndirectedGraph *) graph)->getGraph() );
            diganaUndirectedGraphType::vertex_descriptor v = boost::target(ue , ((diganaUndirectedGraph *) graph)->getGraph() );
             typename property_map<diganaUndirectedGraphType, vertex_index2_t>::type VIdx2 =
                 boost::get(vertex_index2, ((diganaUndirectedGraph *)graph)->getGraph ());
            int sink = VIdx2[v];
            int source = VIdx2[u];
            diganaEdge edgex(source , sink ,((diganaUndirectedGraph *) graph));
            return edgex;
          }
          return edgex;
    }

       diganaEdge operator ++ () {
         if (graph->getType () == diganaDirectedGraphS) ++out_begin_d;
         if (graph->getType () == diganaUndirectedGraphS) ++out_begin_u;

         return *(*this);
       }

       bool operator != (out_edge_iterator & other) {
         if (graph->getType () == diganaDirectedGraphS && out_begin_d != out_end_d) return true;
         if (graph->getType () == diganaUndirectedGraphS && out_begin_u != out_end_u) return true;
         return false;
       }
  };
}
#endif //DIGANA_GRAPH
