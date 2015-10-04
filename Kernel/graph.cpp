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

int
diganaGraphMgr::create_graph (diganaGraphObjectIdentifier & graph_Id, diganaGraphType type)
 {
     try {
       if (graph_exists (graph_Id))
         throw; 

     } catch (...) {
       std::cout << "Graph with name " << graph_Id.getName () << " already exists." << std::endl;  
     } 
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
