
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
    diganaGraph * create_graph (diganaGraphObjectIdentifier & graphId, 
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
    template<typename Value> void register_vertex_property ( string graph_name , string property_name ) {
        int id = diganaGraphMgr::getGraphMgr().getId(graph_name);
        diganaGraphObjectIdentifier graph_Id ( id , graph_name);

        if (!diganaGraphMgr::getGraphMgr ().check_graph_identifier (graph_Id) ) {
                cout << "Invalid Graph Id" << endl;
                return;
        }
        return diganaGraphMgr::getGraphMgr().register_vertex_property<Value> (graph_name , property_name);
    }

    //Register Edge property API
    template<typename Value> void register_edge_property ( string graph_name , string property_name ) {
        int id = diganaGraphMgr::getGraphMgr().getId(graph_name);
        diganaGraphObjectIdentifier graph_Id ( id , graph_name);

        if (!diganaGraphMgr::getGraphMgr ().check_graph_identifier (graph_Id) ) {
                cout << "Invalid Graph Id" << endl;
                return;
        }

        return diganaGraphMgr::getGraphMgr().register_edge_property<Value> (graph_name , property_name);
    }

    //Get Vertex property
    template<typename Value> const Value get_vertex_property ( int vertex_id , string graph_name , string property_name) {
        int id = diganaGraphMgr::getGraphMgr().getId(graph_name);
        diganaGraphObjectIdentifier graph_Id ( id , graph_name);

        if (!diganaGraphMgr::getGraphMgr ().check_graph_identifier (graph_Id) ) {
                cout << "Invalid Graph Id" << endl;
                return;
        }

        if (!diganaGraphMgr::getGraphMgr().check_vertex_id (graph_Id , vertex_id)) {
                cout << "Invalid Vertex Id" << endl;
                return;
        }

        return diganaGraphMgr::getGraphMgr().get_vertex_property<Value> (vertex_id , graph_name , property_name);
    }

    //Get Edge property
    template<typename Value> const Value get_edge_property ( int source , int sink , string graph_name , string property_name) {
        int id = diganaGraphMgr::getGraphMgr().getId(graph_name);
        diganaGraphObjectIdentifier graph_Id ( id , graph_name);

        if (!diganaGraphMgr::getGraphMgr ().check_graph_identifier (graph_Id) ) {
                cout << "Invalid Graph Id" << endl;
                return;
        }

        if (!diganaGraphMgr::getGraphMgr().check_vertex_id (graph_Id , source)) {
                cout << "Invalid Source vertex Id" << endl;
                return;
        }

        if (!diganaGraphMgr::getGraphMgr().check_vertex_id (graph_Id , sink)) {
                cout << "Invalid Sink vertex Id" << endl;
                return;
        }

        return diganaGraphMgr::getGraphMgr().get_edge_property<Value> (sink , source , graph_name , property_name);
    }

    //Put Vertex Property
    template<typename Value> void put_vertex_property (int vertex_id, string graph_name, string property_name, Value data) {
        int id = diganaGraphMgr::getGraphMgr().getId(graph_name);
        diganaGraphObjectIdentifier graph_Id ( id , graph_name);

        if (!diganaGraphMgr::getGraphMgr ().check_graph_identifier (graph_Id) ) {
                cout << "Invalid Graph Id" << endl;
                return;
        }

        if (!diganaGraphMgr::getGraphMgr().check_vertex_id (graph_Id , vertex_id)) {
                cout << "Invalid Vertex Id" << endl;
                return;
        }

        return diganaGraphMgr::getGraphMgr().put_vertex_property<Value> ( vertex_id , graph_name , property_name , data);
    }

    //Put Edge Property
    template<typename Value> void put_edge_property ( int source , int sink , string graph_name , string property_name, Value data) {
        int id = diganaGraphMgr::getGraphMgr().getId(graph_name);
        diganaGraphObjectIdentifier graph_Id ( id , graph_name);

        if (!diganaGraphMgr::getGraphMgr ().check_graph_identifier (graph_Id) ) {
                cout << "Invalid Graph Id" << endl;
                return;
        }

        if (!diganaGraphMgr::getGraphMgr().check_vertex_id (graph_Id , source)) {
                cout << "Invalid Source vertex Id" << endl;
                return;
        }

        if (!diganaGraphMgr::getGraphMgr().check_vertex_id (graph_Id , sink)) {
                cout << "Invalid Sink vertex Id" << endl;
                return;
        }

        return diganaGraphMgr::getGraphMgr().put_vertex_property<Value> ( sink , source ,  graph_name , property_name , data);
    }


  private:

   diganaKernelMgr () {} 
   diganaKernelMgr (diganaKernelMgr const &);
   void operator=(diganaKernelMgr const &);
};
#endif
