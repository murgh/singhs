
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

extern void add_clock (diganaGraph * circuit, char * name, int period, int nodeId, int isVirtual);

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

		virtual void TA_compute_slack () { };
		virtual void TA_print_circuit (diganaGraph *) { };
		virtual void TA_write_paths () { };
		virtual void TA_enumerate_clock_paths () { };
		virtual void TA_enumerate_data_paths () { };
		virtual void TA_Build_Required () { };

		diganaGraph * TA_create_timing_graph (diganaGraph *);

		static timerPinInfo * getPinInfo (diganaVertex & tPin) {
		  timerPinProperty P = tPin.get_property<timerPinProperty> ("Pin_Property");
		  return P.getPinInfo ();
		}	
		static timerArcInfo * getArcInfo (diganaEdge & edge) {
		  timerArcProperty A = edge.get_property<timerArcProperty> ("Arc_Property");
		  return A.getArcInfo ();
		}	

		static int getGlobalForwardMergedCount ();  
		static void setGlobalForwardMergedCount (int val); 

	friend class Timer_Algo_1;
	friend class Timer_Algo_2;

	private:
		diganaGraph * theTimingGraph;
		std::set<timerClock *> theTimerClockSet;
		std::list<diganaVertex> theClockPortList;
		std::list<diganaVertex> theStartPointList;
		std::list<diganaVertex> theEndPointList;
		static int theGlobalForwardMergedCount;
};

//Algo_1 : Use pure DFS
class Timer_Algo_1 : public TA_Timer {

	public:
		Timer_Algo_1 (diganaGraph * graph) : TA_Timer (graph) { }

		virtual void TA_compute_slack ();
		virtual void TA_print_circuit (diganaGraph *);
		virtual void TA_write_paths ();
		virtual void TA_enumerate_clock_paths ();
		virtual void TA_enumerate_data_paths ();
		virtual void TA_Build_Required ();
		void performDFSAndPropagatePinTags (diganaVertex startPoint, bool isClock);
		void processClockEndPoint (diganaVertex endPoint, std::list<diganaVertex> & timingPoints);
		void processDataEndPoint (diganaVertex endPoint, std::list<diganaVertex> & timingPoints);

	private:
		std::map<timerPinInfo *, std::list<std::list<diganaVertex> * > * > theClockEndPointPathMap;
};

//Algo_2 : Use timer pin tags for tracking timing data
class Timer_Algo_2 : public TA_Timer {
	
	public:
		Timer_Algo_2 (diganaGraph * graph) : TA_Timer (graph) { }

		virtual void TA_compute_slack ();
		virtual void TA_print_circuit (diganaGraph *);
		virtual void TA_write_paths ();
		virtual void TA_enumerate_clock_paths ();
		virtual void TA_enumerate_data_paths ();
		virtual void TA_Build_Required ();

		bool checkAndPerformTagSplitting (diganaVertex & timerPin, bool);
		void performBFSAndPropagatePinTags (diganaVertex pin, bool);
		void propagatePinTags (diganaVertex & sourcePin, diganaVertex & sinkPin);
		void propagatePinTagsFromStart (diganaVertex & sourcePin, bool);
		void computeRecursiveTagPath (timerPinTag * tag,
				              std::list <timerPinTag *> & theTagPath,
					      std::list <std::list<timerPinTag *> * > & tagPaths);
		void computeTagPaths (FILE * file, diganaVertex vtx);
		void writeTimingPath (FILE * file, std::list<diganaVertex> & timingPath, std::string);
		void buildTimingPathFromTagPath (diganaVertex endPoint,
						 std::list<timerPinTag *> * theTagPath,
						 std::list<diganaVertex> & timingPath);
};
#endif //TIMER
