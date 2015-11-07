// example1.cc

#include <cpptcl.h>
#include <iostream>
#include <string>
#include <partition.hxx>
#include <distributor.hxx>
#include <kernel.hxx>

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
}
