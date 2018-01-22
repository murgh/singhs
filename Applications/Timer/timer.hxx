
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

void perform_timing_analysis (diganaGraph * graph, int algo, int part);

typedef struct TARepObj {
  int from;
  int through;  
  int to;

  TARepObj * next;

} TARepObj;


class TA_Path {

	public:
		timerPinTag * tag;
		TA_Path * next;
		static void releaseTagPath (TA_Path * start) {
		  TA_Path * temp = start;
		  while (temp) {
			TA_Path * del = temp;
			temp = temp->next;
			delete temp;
		  }
		}
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
		virtual void TA_Build_Required (int) { };
		virtual void TA_Report_Paths (FILE * file) { };
		virtual void TA_Report_To (TARepObj * obj, FILE * file) { }
		virtual void TA_Report_Through (TARepObj * obj, FILE * file) { }
		virtual void TA_Report_From_Through_To (TARepObj * obj, FILE * file) { }

		diganaGraph * TA_create_timing_graph (diganaGraph *);

		void getFanOutEndPointSet (diganaVertex tPin, std::set<int> & endSet, bool markCone);
		void getFanInStartPointSet (diganaVertex tPin, std::set<int> & startSet, bool markCone);

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

		static void addRepObj (TARepObj * r) {
			if (!RepObj)
				RepObj = r;
			else {
				r->next = RepObj;
				RepObj = r;
			}
		}

		static void printReport () {
		   TARepObj * t = RepObj;
		   while (t) {
			printf ("%d %d %d\n", t->from, t->through, t->to);
			t = t->next;
		   }

		}

		void clearRepConeMarking ();

		bool getOtherPinForDPin (diganaVertex endDPin, diganaVertex & otherPin);

	friend class Timer_Algo_1;
	friend class Timer_Algo_2;

	private:
		diganaGraph * theTimingGraph;
		std::set<timerClock *> theTimerClockSet;
		std::list<diganaVertex> theClockPortList;
		std::list<diganaVertex> theStartPointList;
		std::list<diganaVertex> theEndPointList;
		static int theGlobalForwardMergedCount;
		static TARepObj * RepObj;
};

//Algo_1 : Use pure DFS
class Timer_Algo_1 : public TA_Timer {

	public:
		Timer_Algo_1 (diganaGraph * graph) : TA_Timer (graph) {
		  printf ("\n\n\t\t *** Performing Static Timing Analysis using DFS Based Enumeration *** \n\n");
		}

		virtual void TA_compute_slack ();
		virtual void TA_print_circuit (diganaGraph *);
		virtual void TA_write_paths ();
		virtual void TA_enumerate_clock_paths ();
		virtual void TA_enumerate_data_paths ();
		virtual void TA_Build_Required (int);
		void performDFSAndPropagatePinTags (diganaVertex startPoint, bool isClock);
		void processClockEndPoint (diganaVertex endPoint, std::list<diganaVertex> & timingPoints);
		void processDataEndPoint (diganaVertex endPoint, std::list<diganaVertex> & timingPoints);
		virtual void TA_Report_Paths (FILE * file);
		virtual void TA_Report_To (TARepObj * obj, FILE * file);
		virtual void TA_Report_Through (TARepObj * obj, FILE * file);
		virtual void TA_Report_From_Through_To (TARepObj * obj, FILE * file);
		void tracePathFrom (diganaVertex pin, std::list<int> & currPath, std::list<int *> & pathList, bool clockPath);
		void traceFromSetPaths (std::set<int> & pinSet, std::list<int *> & pathList, bool clockPath);
		void writeTimingPath (FILE * file, int * timingPath, std::string);
		int TA_Report_End_Path (FILE * file, diganaVertex endPoint, bool do_marking = true);
		int TA_Report_Data_Path (FILE * file, int * path, int & pathCount);
		diganaVertex getEndPoint (int * path);
		diganaVertex getStartPoint (int * path);
		bool isValidFromToPath (int * path, diganaVertex & startPoint, diganaVertex & endPoint);


	private:
		std::map<timerPinInfo *, std::list<std::list<diganaVertex> * > * > theClockEndPointPathMap;
};

//Algo_2 : Use timer pin tags for tracking timing data
class Timer_Algo_2 : public TA_Timer {
	
	public:
		Timer_Algo_2 (diganaGraph * graph) : TA_Timer (graph) { 
		  printf ("\n\n\t\t *** Performing Static Timing Analysis using End Tag Based Enumeration *** \n\n");
		}

		virtual void TA_compute_slack ();
		virtual void TA_print_circuit (diganaGraph *);
		virtual void TA_write_paths ();
		virtual void TA_enumerate_clock_paths ();
		virtual void TA_enumerate_data_paths ();
		virtual void TA_Build_Required (int);

		bool checkAndPerformTagSplitting (diganaVertex & timerPin, bool);
		void performBFSAndPropagatePinTags (diganaVertex pin, bool);
		bool propagatePinTags (diganaVertex & sourcePin, diganaVertex & sinkPin);
		void propagatePinTagsFromStart (diganaVertex & sourcePin, bool);
		void computeRecursiveTagPath (timerPinTag * tag,
				              std::list <timerPinTag *> & theTagPath,
					      std::list <timerPinTag **> & tagPaths);
		int computeTagPaths (FILE * file, diganaVertex vtx, int & pathCount, 
						  std::set<timerPinTag *> * pruneFromSet = NULL,
						  std::set<int> * pruneThroughSet = NULL);
		void writeTimingPath (FILE * file, std::list<diganaVertex> & timingPath, std::string);
		void buildTimingPathFromTagPath (diganaVertex endPoint,
						 timerPinTag ** theTagPath,
						 std::list<diganaVertex> & timingPath, timerAnalysisType el);
		void computeSlackAndWrite (FILE * file, diganaVertex endPoint, diganaVertex refEndPoint);
		virtual void TA_Report_Paths (FILE * file);
		virtual void TA_Report_To (TARepObj * obj, FILE * file);
		virtual void TA_Report_Through (TARepObj * obj, FILE * file);
		virtual void TA_Report_From_Through_To (TARepObj * obj, FILE * file);
		int getFanInStartTagSet (timerPinTag * tag, std::set<timerPinTag *> & startSet); 
		void printTagPath (std::list <timerPinTag **> & tagPaths);
		void Uniquify_end_tags ();
};

class timerSourceVertexIterator {

  public:
	  timerSourceVertexIterator (diganaVertex & v); 

	  ~timerSourceVertexIterator () {
	    vecPos = -1;
	    free (theSourceVertices);
	    theSourceVertices = NULL;
	  }

	  bool end () { return (vecPos == -1); }

	  diganaVertex next () { 
	    int id = theSourceVertices[vecPos--];
	    return diganaVertex (id, theTimingGraph); 
	  }

  private:
	  int * theSourceVertices;
	  diganaGraph * theTimingGraph;
	  int vecPos;
};
#endif //TIMER
