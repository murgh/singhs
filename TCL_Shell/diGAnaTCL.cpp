// example1.cc

#include <cpptcl.h>
#include <iostream>
#include <string>
#include <partition.hxx>
#include <distributor.hxx>
#include <kernel.hxx>
#include <boost/algorithm/string.hpp>

#define LENGTH_OF(array) sizeof(array)/sizeof(array[0])

using namespace std;

diganaPartitionMgr pMgr;

diganaDistributionMgr dMgr;

//diganaKernelMgr::diganaGetKernel ().create_graph (...);

void print_create_graph_usage()
{
	cout << "Command Usage : create_graph -name <Name/ID of the graph> -type <type of the graph>" << endl;
}


void create_graph ( string option1 , string value1 , string option2 , string value2 ) 
{
	if(option1.empty() || value1.empty() || option2.empty() || value2.empty()) 
	{
		print_create_graph_usage();
		return;
	}
	
        string graph_type , graph_name;
	string options [] = { option1 , option2 };
	string values [] = { value1 , value2 };
	int size = LENGTH_OF(options);
	
        for ( int i = 0 ; i < size ; i++ ) {

		if(options[i]=="-type") {
			graph_type = values[i];
		}
		else if(options[i]=="-name") {
			graph_name = values[i];
		}
		else {
			print_create_graph_usage();
			return;
		}
	}

  	cout << "Command : create_graph" << endl;
	cout << "name    : " << graph_name << endl;
	cout << "type    : " << graph_type << endl;

	
	diganaGraphObjectIdentifier graph_identifier ( 1 , graph_name );
	diganaGraphType type;

	if ( graph_type == "undirectional") {
		type = diganaUndirectedGraphS;
	}
	else if ( graph_type == "directional") {
		type = diganaDirectedGraphS;
	}
	else {
		cout << "Invalid Graph type" << endl;
		return;
	}
	
	int returned_id = diganaKernelMgr::diganaGetKernel().create_graph ( graph_identifier , type );
	cout << "Id provided to graph is " << returned_id << endl;
}

void print_create_node_usage() 
{
	cout << "Command usage : create_node -name <name of the node> -graph <name of the graph>" << endl;
}

void create_node (string option1 , string value1 , string option2 , string value2 ) 
{
	if (option1.empty() || value1.empty() || option2.empty() || value2.empty()) {
		print_create_node_usage();
		return;
	}
	
	string node_name , graph_name;
	string options [] = { option1 , option2 };
	string values [] = { value1 , value2 };
	int size = LENGTH_OF(options);

	for ( int i = 0 ; i < size ; i++ ) {

		if(options[i]=="-name"){
			node_name = values[i];
		}
		else if(options[i]=="-graph"){
			graph_name = values[i];
		}
		else {
			print_create_node_usage();
			return;
		}
	}
	

	cout << "Command    : create_node" << endl;
	cout << "name       : " << node_name << endl;
	cout << "graph      : " << graph_name << endl;

	diganaGraphObjectIdentifier graph_Id (1 , graph_name);
	diganaGraphObjectIdentifier node_Id ( 1 , node_name);
	int returned_id = diganaKernelMgr::diganaGetKernel().add_vertex ( graph_Id , node_Id );
	cout << "Id provided to node is " << returned_id << endl;
}

void print_create_edge_usage() 
{
	cout << "Command usage : create_edge -graph <name of the graph> -source <source node ID> -sink <sink node ID>" << endl;

}

void create_edge (string option1 , string value1 , string option2 , string value2 , string option3 , string value3) 
{
	if (option1.empty() || value1.empty() || option2.empty() || value2.empty() || option3.empty() || value3.empty()) {
		print_create_edge_usage();
		return;
	}
	
	string graph_name , source_id , sink_id;
	string options [] = { option1 , option2 , option3  };
	string values [] = { value1 , value2 , value3 };
	int size = LENGTH_OF(options);

	for ( int i = 0 ; i < size ; i++ ) {

		if(options[i]=="-graph"){
			graph_name = values[i];
		}
		else if(options[i]=="-source"){
			source_id = values[i];
		}
		else if(options[i]=="-sink"){
			sink_id = values[i];
		}
		else {
			print_create_edge_usage();
			return;
		}
	}
	
	cout << "Command    : create_edge" << endl;
	cout << "graph      : " << graph_name << endl;
	cout << "source     : " << source_id << endl;
	cout << "sink       : " << sink_id << endl;

	diganaGraphObjectIdentifier graph_Id ( 1 , graph_name);
	int source = atoi(source_id.c_str());
	int sink = atoi(sink_id.c_str());
	diganaKernelMgr::diganaGetKernel().add_edge ( graph_Id , source , sink );
}

//void register_node_property ( string option1 , string value1 , string option2 , string value2 , string	


void run_diGAna ()
{
   pMgr.print ();  	
   diganaKernelMgr::diganaGetKernel().print ();  	
   dMgr.print ();
     cout << "This is the code that I have added in run_diGAna " << endl;
}

CPPTCL_MODULE(Mymodule, i)
{
     i.def("hello_run", run_diGAna);
     i.def("create_graph",create_graph);
     i.def("create_node",create_node);
     i.def("create_edge",create_edge);
}
