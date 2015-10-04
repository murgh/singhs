//This is the graph header file for modelling the graph classes
#include <boost/graph/adjacency_list.hpp>
#include <string>
#include <fstream>
#include <map>

using namespace boost;
using namespace std;

enum diganaGraphType { 
                       diganaUndirectedGraph, 
                       diganaDirectedGraph, 
                       diganaNoGraphType
                     };
class diganaGraphObjectIdentifier;
class diganaGraphProperty;
class diganaGraph;
class diganaUndirectedGraph;
class diganaDirectedGraph;
class diganaGraphMgr; 

//The undirected graph class
typedef boost::adjacency_list<listS, listS, undirectedS, no_property, no_property, listS> diganaUndirectedGraphType;

//The directed graph class
typedef boost::adjacency_list<listS, listS, directedS, no_property, no_property, listS> diganaDirectedGraphType;

//ID to Graph Map
typedef std::map< std::pair<int, std::string>, diganaGraph> mapIdToGraph;

//The graph property manager class
class diganaGraphProperty {

};

//class object identifier to identify an object. Objects in our
//graph library are graph, vertices and edges. They can be 
//identified by either the names that they contain or their integer Ids
class diganaGraphObjectIdentifier {
  public:
   diganaGraphObjectIdentifier (int id, std::string str) :
                                identifier (std::pair<int, std::string> (id, str)) { }
   bool operator == (const diganaGraphObjectIdentifier &) const;
   void operator = (const diganaGraphObjectIdentifier &);
   std::string getName () const { return identifier.second;}
   int         getId   () const { return identifier.first; }  
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
/*   int insert_vertex (diganaGraphObjectIdentifier graph_Id, std::string name);
   void insert_edge (std::string graph,   
*/
  private:
   mapIdToGraph Id_Graph_Map;
   int Graph_Count;
};


//It is the base graph class 
class diganaGraph {
  public:
   std::string getName () const { return identifier.getName (); } 
   int         getId   () const { return identifier.getId (); }
  
  private:
   diganaGraphProperty properties;
   diganaGraphObjectIdentifier identifier;
};


class diganaUndirectedGraph : public diganaGraph {

  
  private:
   diganaUndirectedGraphType graph;
};

class diganaDirectedGraph : public diganaGraph {

  private:
   diganaDirectedGraphType graph;
};


