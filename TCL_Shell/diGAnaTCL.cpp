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


void hello(string option, int times)
{
     int i;
     cout << "Hello C++/Tcl!" << endl;
     if (option == "-p") { 
       for (i =0; i < times; i++) {
          cout << "This is the code that I added " << endl;
          cout << "This is printing what we passing - " << i << endl;
       }
     }
}


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

}

void print_create_node_usage() 
{
	cout << "Command usage : create_node -name <name of the node> -graph <name of the graph> -properties <TCL list of properties>" << endl;
}

void create_node (string option1 , string value1 , string option2 , string value2 , string option3 , string value3 ) 
{
	if (option1.empty() || value1.empty() || option2.empty() || value2.empty() || option3.empty() || value3.empty()) {
		print_create_node_usage();
		return;
	}
	
	string node_name , graph_name , property_list;
	vector<string> properties;
	string options [] = { option1 , option2 , option3 };
	string values [] = { value1 , value2 , value3 };
	int size = LENGTH_OF(options);

	for ( int i = 0 ; i < size ; i++ ) {

		if(options[i]=="-name"){
			node_name = values[i];
		}
		else if(options[i]=="-graph"){
			graph_name = values[i];
		}
		else if(options[i]=="-properties"){
			property_list = values[i];
		}
		else {
			print_create_node_usage();
			return;
		}
	}
	
	boost::split(properties , property_list , boost::is_any_of(" "));

	cout << "Command    : create_node" << endl;
	cout << "name       : " << node_name << endl;
	cout << "graph      : " << graph_name << endl;
	cout << "properties : " << properties[0] << endl;

	int vector_size = properties.size();
	for ( int i = 1 ; i < vector_size ; i++ ) {
		cout << "           : " << properties[i] << endl;
	}
}

void print_create_edge_usage() 
{
	cout << "Command usage : create_edge -graph <name of the graph> -source <source node ID> -sink <sink node ID> -properties <TCL list of properties>" << endl;

}

void create_edge (string option1 , string value1 , string option2 , string value2 , string option3 , string value3 , string option4 , string value4 ) 
{
	if (option1.empty() || value1.empty() || option2.empty() || value2.empty() || option3.empty() || value3.empty() || option4.empty() || value4.empty()) {
		print_create_edge_usage();
		return;
	}
	
	string graph_name , source_id , sink_id , property_list;
	vector<string> properties;
	string options [] = { option1 , option2 , option3 , option4 };
	string values [] = { value1 , value2 , value3 , value4 };
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
		else if(options[i]=="-properties"){
			property_list = values[i];
		}
		else {
			print_create_edge_usage();
			return;
		}
	}
	
	boost::split(properties , property_list , boost::is_any_of(" "));

	cout << "Command    : create_edge" << endl;
	cout << "graph      : " << graph_name << endl;
	cout << "source     : " << source_id << endl;
	cout << "sink       : " << sink_id << endl;
	cout << "properties : " << properties[0] << endl;

	int vector_size = properties.size();
	for ( int i = 1 ; i < vector_size ; i++ ) {
		cout << "           : " << properties[i] << endl;
	}
}

void run_diGAna ()
{
   pMgr.print ();  	
   diganaKernelMgr::diganaGetKernel().print ();  	
   dMgr.print ();
     cout << "This is the code that I have added in run_diGAna " << endl;
}

CPPTCL_MODULE(Mymodule, i)
{
     i.def("hello", hello);
     i.def("hello_run", run_diGAna);
     i.def("create_graph",create_graph);
     i.def("create_node",create_node);
     i.def("create_edge",create_edge);
}
