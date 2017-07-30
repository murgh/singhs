
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
		void TA_compute_slack ();
		void TA_print_circuit (diganaGraph *);
		void TA_write_paths ();
		void TA_enumerate_clock_paths ();
		void TA_enumerate_data_paths ();
		void TA_Build_Required ();
		bool checkAndPerformTagSplitting (diganaVertex & timerPin, bool);
		void performBFSAndPropagatePinTags (diganaVertex pin, bool);
		void propagatePinTags (diganaVertex & sourcePin, diganaVertex & sinkPin);
		void propagatePinTagsFromStart (diganaVertex & sourcePin, bool);
		void computeRecursiveTagPath (timerPinTag * tag,
				              std::list <timerPinTag *> & theTagPath,
					      std::list <std::list<timerPinTag *> * > & tagPaths);
		void computeTagPaths (FILE * file, diganaVertex vtx);
		void writeTimingPath (FILE * file, std::list<diganaVertex> & timingPath, int);
		void buildTimingPathFromTagPath (diganaVertex endPoint,
						 std::list<timerPinTag *> * theTagPath,
						 std::list<diganaVertex> & timingPath);
		timerPinInfo * getPinInfo (diganaVertex & tPin) {
		  timerPinProperty P = tPin.get_property<timerPinProperty> ("Pin_Property");
		  return P.getPinInfo ();
		}	

	private:
		diganaGraph * theTimingGraph;
		std::vector<TA_Path *> thePathCollection; 
		std::set<timerClock *> theTimerClockSet;
		std::list<diganaVertex> theClockPortList;
		std::list<diganaVertex> theStartPointList;
		std::list<diganaVertex> theEndPointList;
		std::map<diganaVertex, 
			 std::pair<timerPinTag*, timerPinTag*> > theEndTagPair;

};
#endif //TIMER
