
#ifndef KERNEL
#define KERNEL
#include"graph.hxx"

using namespace std;
//Singleton class object is allowed
class diganaKernelMgr {
  public:
    static diganaKernelMgr & diganaGetKernel () {
      static diganaKernelMgr kernelInst;
      return kernelInst;
    }

    int print ();

    //Graph APIs for kernel

    //Create graph API
    int create_graph (diganaGraphObjectIdentifier & graphId, 
		             diganaGraphType graph_type); 

    //Add vertex API
    int add_vertex (diganaGraphObjectIdentifier & graphId,
		           diganaGraphObjectIdentifier & nodeId);

    //Add edge API
    void add_edge (diganaGraphObjectIdentifier & graphId,
		          int source_vertex_id, int sink_vertex_id);

    //Get the index of a node in graph given it's name
    int get_vertex_id (diganaGraphObjectIdentifier & graphId, std::string name); 
 
    //Register Vertex property API
    template<typename Value> void register_vertex_property ( string graph_name , string property_name );

    //Register Edge property API
    template<typename Value> void register_edge_property ( string graph_name , string property_name );

    //Put Vertex property
    template<typename Value> const Value get_vertex_property ( int vertex_id , string graph_name , string property_name);

    //Put Edge property
    template<typename Value> const Value get_edge_property ( int source , int sink , string graph_name , string property_name);

    //Get Vertex Property
    template<typename Value> void put_vertex_property (int vertex, string graph_name, string property_name);

    //Get Edge Property
    template<typename Value> void put_edge_property ( int source , int sink , string graph_name , string property);


  private:

   diganaKernelMgr () {} 
   diganaKernelMgr (diganaKernelMgr const &);
   void operator=(diganaKernelMgr const &);
};
#endif
