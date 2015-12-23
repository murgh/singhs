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
   diganaUndirectedGraphType & getGraph () { return graph; }
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
   diganaDirectedGraphType & getGraph () { return graph; }
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
   
   template<typename Value> Value get_property (std::string Name) { 
            return diganaGraphMgr::getGraphMgr ().get_vertex_property<Value> (vertexID, Name);    
   }

   template<typename Value> void put_property (std::string Name, Value val) { 
            diganaGraphMgr::getGraphMgr ().put_vertex_property<Value> (vertexID, Name, val);    
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

   diganaEdge (const diganaEdge & e) : 
              source (e.source),
              sink (e.sink),
              parent_graph (e.parent_graph) 
   { }

   ~diganaEdge () {
     source = sink = -1;
     parent_graph = NULL;
   } 
   
   template<typename Value> Value get_property (std::string Name) { 
            return diganaGraphMgr::getGraphMgr ().get_edge_property<Value> (source, sink, Name);    
   }

   template<typename Value> void put_property (std::string Name, Value val) { 
            diganaGraphMgr::getGraphMgr ().put_edge_property<Value> (source, sink, Name, val);    
   }

   private:
    //new cannot be called for this object it will always be a local variable.
    void * operator new   (size_t);
    void * operator new[] (size_t);
    void   operator delete   (void *);
    void   operator delete[] (void*);
};

namespace diganaGraphIterator {
   typedef 
    boost::variant<
     typename boost::graph_traits<diganaDirectedGraphType>::vertex_iterator,
     typename boost::graph_traits<diganaDirectedGraphType>::edge_iterator,
     typename boost::graph_traits<diganaDirectedGraphType>::in_edge_iterator,
     typename boost::graph_traits<diganaDirectedGraphType>::out_edge_iterator,
     typename boost::graph_traits<diganaUndirectedGraphType>::vertex_iterator,
     typename boost::graph_traits<diganaUndirectedGraphType>::adjacency_iterator,
     typename boost::graph_traits<diganaUndirectedGraphType>::edge_iterator,
     typename boost::graph_traits<diganaUndirectedGraphType>::in_edge_iterator,
     typename boost::graph_traits<diganaUndirectedGraphType>::out_edge_iterator,
     typename boost::graph_traits<diganaDirectedGraphType>::adjacency_iterator
    > iterators; 

   typedef std::pair<iterators, iterators> Iter_Pair;

   typedef boost::variant<
     typename boost::graph_traits <diganaDirectedGraphType>::vertex_descriptor,
     typename boost::graph_traits <diganaUndirectedGraphType>::vertex_descriptor
   > vertex_descriptor;

   typedef boost::variant<
     typename boost::graph_traits <diganaDirectedGraphType>::edge_descriptor,
     typename boost::graph_traits <diganaUndirectedGraphType>::edge_descriptor
   > edge_descriptor;

   typedef variant <diganaVertex, diganaEdge> GraphObject;

  class iterator {

   private:
    iterators begin_, end_;
    bool isAttached_;

   public:
    iterator () : isAttached_ (false) { }

    void check_attached () {
       try {
        if (isAttached_ == false)
          throw 1;
       } catch (int e) {
         std::cout << "Iterator left unattached\n";
         return;
       } 
     return;
    }

    template<typename Graph_Type, typename Iter_Type>
    void attach (diganaGraph * graph) {
      isAttached_ = true;
      Graph_Type * uG = (Graph_Type *) graph;
      Iter_Type i, i_end;   
      begin_ = i;
      end_ = i_end;
    }

    template<typename Graph_Type, typename Iter_Type> 
    void attach (int vid, diganaGraph * graph) {
      isAttached_ = true;
      Graph_Type * uG = (Graph_Type *) graph;
      vertex_descriptor v = boost::vertex(vid, uG->getGraph ());
      Iter_Type i, i_end;
      begin_ = i;
      end_ = i_end;
    }

    template<typename Graph_Type, typename Iter_Type> 
    void attach (int srcid, int sinkid, diganaGraph * graph) {
      isAttached_ = true;
      Graph_Type * uG = (Graph_Type *) graph;
      edge_descriptor e = boost::edge(srcid, sinkid, uG->getGraph ());
      Iter_Type i, i_end;
      begin_ = i;
      end_ = i_end;
    }

    template <typename Graph, typename GraphType> 
    static Iter_Pair vertices (diganaGraph * graph) {
      Graph * uG = (Graph *) graph;
      return boost::vertices (uG->getGraph ()); 
    }

    template <typename Graph, typename GraphType> 
    static Iter_Pair edges (diganaGraph * graph) {
      Graph * uG = (Graph *) graph;
      return boost::edges (uG->getGraph ()); 
    }

    template <typename Graph, typename GraphType> 
    static Iter_Pair adjacent_vertices (int vertex, diganaGraph * graph) {
      Graph * uG = (Graph *) graph;
      typename GraphType::vertex_descriptor v = boost::vertex (vertex, uG->getGraph ());
      return boost::adjacent_vertices (v, uG->getGraph ()); 
    }

    template <typename Graph, typename GraphType> 
    static Iter_Pair in_edges (int vertex, diganaGraph * graph) {
      Graph * uG = (Graph *) graph;
      typename GraphType::vertex_descriptor v = boost::vertex (vertex, uG->getGraph ());
      return boost::in_edges (v, uG->getGraph ()); 
    }

    template <typename Graph, typename GraphType> 
    static Iter_Pair out_edges (int vertex, diganaGraph * graph) {
      Graph * uG = (Graph *) graph;
      typename GraphType::vertex_descriptor v = boost::vertex (vertex, uG->getGraph ());
      return boost::out_edges (v, uG->getGraph ()); 
    }

    template <typename Graph, typename GraphType, typename Iter_Type> 
    static diganaVertex get_vertex (iterators itr, diganaGraph * graph) {
      Graph * uG = (Graph *) graph;
      Iter_Type & i = boost::get<Iter_Type> (itr);
      typename GraphType::vertex_descriptor v = *i;
      typename property_map<GraphType, vertex_index2_t>::type VIdx2 = 
	                                   boost::get(vertex_index2, uG->getGraph ());
      return diganaVertex (VIdx2[v], graph); 
    }

    iterators & begin () { return begin_; }
    iterators & end () { return end_; }
    friend class adjacent_vertices_iterator; 
  }; 

  class adjacent_vertices_iterator {

   private:
    iterator itr;
    diganaVertex attached_Vertex; 

   public:
    adjacent_vertices_iterator () : itr () { }
  
    //Attach API
    void attach (int vertex, diganaGraph * graph) {
      try {
        if (graph->getType () == diganaUndirectedGraphS) {         
          itr.attach<diganaUndirectedGraph, 
                 typename boost::graph_traits<diganaUndirectedGraphType>::adjacency_iterator> (vertex, graph);
          boost::tie (itr.begin_, itr.end_) = iterator::adjacent_vertices<diganaUndirectedGraph, diganaUndirectedGraphType> (vertex, graph);

        } else if (graph->getType () == diganaDirectedGraphS) {         
          itr.attach<diganaDirectedGraph, 
                 typename boost::graph_traits<diganaDirectedGraphType>::adjacency_iterator> (vertex, graph);
          boost::tie (itr.begin_, itr.end_) = iterator::adjacent_vertices<diganaDirectedGraph, diganaDirectedGraphType> (vertex, graph);
        } else {
          throw 1;
        }
      } catch (int e) {
        std::cout << "Ambiguous graph type, must be directed or undirected graph\n"; 
        return;
      } 
      attached_Vertex = diganaVertex (vertex, graph);
    }

    //Increment Operator
    diganaVertex operator ++ () {
      itr.check_attached ();
      if (itr.begin () == itr.end ()) return diganaVertex ();
      diganaGraph * graph = attached_Vertex.getParentGraph ();
      if (graph->getType () == diganaUndirectedGraphS) {
        typename boost::graph_traits<diganaUndirectedGraphType>::adjacency_iterator & beg =
          boost::get<typename boost::graph_traits<diganaUndirectedGraphType>::adjacency_iterator> (itr.begin_); 
        ++beg;
        return iterator::get_vertex<diganaUndirectedGraph, diganaUndirectedGraphType, 
                                    typename boost::graph_traits<diganaUndirectedGraphType>::adjacency_iterator> (beg, graph); 
      } else if (graph->getType () == diganaDirectedGraphS) {
        typename boost::graph_traits<diganaDirectedGraphType>::adjacency_iterator & beg =
          boost::get<typename boost::graph_traits<diganaDirectedGraphType>::adjacency_iterator> (itr.begin_); 
        ++beg;
        diganaDirectedGraphType::vertex_descriptor v = *beg;
	diganaDirectedGraphType & g = ((diganaDirectedGraph *) graph)->getGraph ();
        typename property_map<diganaDirectedGraphType, vertex_index2_t>::type VIdx2 =
	                                            boost::get(vertex_index2, g);
        return diganaVertex (VIdx2[v], graph); 
      } 
      return diganaVertex (); 
    }

    //Dereference operator
    diganaVertex operator * () {
      itr.check_attached ();
      if (itr.begin () == itr.end ()) return diganaVertex ();
      diganaGraph * graph = attached_Vertex.getParentGraph ();
      if (graph->getType () == diganaUndirectedGraphS) {
        typename boost::graph_traits<diganaUndirectedGraphType>::adjacency_iterator & beg =
          boost::get<typename boost::graph_traits<diganaUndirectedGraphType>::adjacency_iterator> (itr.begin ()); 
        return iterator::get_vertex<diganaUndirectedGraph, diganaUndirectedGraphType, 
                                    typename boost::graph_traits<diganaUndirectedGraphType>::adjacency_iterator> (beg, graph); 
      } else if (graph->getType () == diganaDirectedGraphS) {
        typename boost::graph_traits<diganaDirectedGraphType>::adjacency_iterator beg =
          boost::get<typename boost::graph_traits<diganaDirectedGraphType>::adjacency_iterator> (itr.begin ()); 
        diganaDirectedGraphType::vertex_descriptor v = *beg;
	diganaDirectedGraphType & g = ((diganaDirectedGraph *) graph)->getGraph ();
        typename property_map<diganaDirectedGraphType, vertex_index2_t>::type VIdx2 =
	                                            boost::get(vertex_index2, g);
        return diganaVertex (VIdx2[v], graph); 
      }
      return diganaVertex ();
    }    
    
    bool operator != ( adjacent_vertices_iterator & other) {
     diganaVertex this_vertex = *(*this);
     diganaVertex other_vertex = *other;
     return (this_vertex != other_vertex) ? true : false;
    }
  };
}

class adjacent_iterator_directed {
 private :
  typename boost::graph_traits<diganaDirectedGraphType>::adjacency_iterator begin, end;
  diganaVertex attached_vertex;

 public:
  adjacent_iterator_directed () { }

  void attach (int vertex, diganaDirectedGraph * graph) {
   diganaDirectedGraphType::vertex_descriptor v = boost::vertex (vertex, graph->getGraph ()); 
   boost::tie (begin, end) = boost::adjacent_vertices (v, graph->getGraph ());
   attached_vertex = diganaVertex (vertex, graph);
  }	  
  diganaVertex operator ++ () {
   begin++;
   diganaDirectedGraphType::vertex_descriptor v = *begin;
   typename property_map<diganaDirectedGraphType, vertex_index2_t>::type VIdx2 =
	 boost::get(vertex_index2, ((diganaDirectedGraph *)(attached_vertex->getParentGraph ()))->getGraph ());
   return diganaVertex (VIdx2[v], graph); 
   
  }
};
#endif //DIGANA_GRAPH
