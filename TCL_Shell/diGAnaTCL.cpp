// example1.cc

#include <cpptcl.h>
#include <iostream>
#include <string>
#include <partition.hxx>
#include <distributor.hxx>
#include <kernel.hxx>

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
}
