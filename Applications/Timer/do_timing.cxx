#include "timerPin.hxx"
#include "timerEdge.hxx"
#include "timerUtils.hxx"

extern diganaGraph * create_timing_graph (); 

int main ()
{
  diganaGraph * graph = create_timing_graph ();
  perform_timing_analysis (graph);
  return 0;
}
