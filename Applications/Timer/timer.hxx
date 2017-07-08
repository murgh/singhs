
#ifndef TIMER
#define TIMER

#include <vector>
#include <list>
#include "../../Kernel/graph.hxx"
#include "timerUtils.hxx"
#include "timerLibData.hxx"
#include "timerEdge.hxx"
#include "timerPin.hxx"
#include "timerConstraints.hxx"

void perform_timing_analysis (diganaGraph * graph);

class TA_Path {
	public:
		std::list<int> theNodeList;
};

//The timer container
class TA_Timer {
	
	public:
		TA_Timer (diganaGraph * graph) { 
		  theTimingGraph = TA_create_timing_graph (graph);
		}
		~TA_Timer () {
		}
		diganaGraph * TA_create_timing_graph (diganaGraph *);
		void TA_enumerate_clock_paths ();
		void checkAndPerformTagSplitting (diganaVertex & timerPin, bool);
		void performBFSAndPropagatePinTags (diganaVertex pin, bool);
		void propagatePinTags (diganaVertex & sourcePin, diganaVertex & sinkPin);
		void propagatePinTagsFromStart (diganaVertex & sourcePin, bool);
		void buildClockPortList (std::list<diganaVertex> &);
		timerPinInfo * getPinInfo (diganaVertex & tPin) {
		  timerPinProperty P = tPin.get_property<timerPinProperty> ("Pin_Property");
		  return P.getPinInfo ();
		}	

	private:
		diganaGraph * theTimingGraph;
		diganaVertex theInVirtualNode, theOutVirtualNode;
		std::vector<TA_Path *> thePathCollection; 
		std::set<timerClock *> theTimerClockSet;
};
#endif //TIMER
