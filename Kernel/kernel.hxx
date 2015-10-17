
#ifndef KERNEL
#define KERNEL
#include"graph.hxx"

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

  private:

   diganaKernelMgr () {} 
   diganaKernelMgr (diganaKernelMgr const &);
   void operator=(diganaKernelMgr const &);
};
#endif
