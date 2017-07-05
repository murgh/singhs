
#ifndef TIMER
#define TIMER

#include <vector>
#include <list>
#include "../../Kernel/graph.hxx"
#include "timerUtils.hxx"
#include "timerLibData.hxx"
#include "timerEdge.hxx"
#include "timerPin.hxx"

void perform_timing_analysis (diganaGraph * graph);

class TA_Path {
	public:
		std::list<diganaEdge> theEdgeList;
};

class TA_Timer {
	
	public:
		TA_Timer (diganaGraph * graph) { 
		  theTimingGraph = TA_create_timing_graph (graph);
		}
		~TA_Timer () {
		}
		diganaGraph * TA_create_timing_graph (diganaGraph *);
		void TA_enumerate_paths ();
	private:
		diganaGraph * theTimingGraph;
		diganaVertex theInVirtualNode, theOutVirtualNode;
		std::vector<TA_Path *> thePathCollection; 
		std::set<timerClock *> theTimerClockSet;
};
#endif //TIMER
