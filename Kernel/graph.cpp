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
       if (graph_exists (graph_Id))
         throw 1; 
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
