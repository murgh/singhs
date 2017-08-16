#ifndef TIMER_DELAY
#define TIMER_DELAY

#include "timerPin.hxx"
class timerDelayCalcArgs;

//2 X 2 delay values for input and output
//transition combinations
//Arc delay and transitions 
class timerDelay {
	public:
		timerDelay () {
			for (int i = 0; i < timerTrans; i++) {
				for (int j = 0; j < timerTrans; j++) {
					annotate (timerEarly,
							((timerTransition)i),
							((timerTransition)j),
							timerUndefDelay);
					annotate (timerLate,
							((timerTransition)i),
							((timerTransition)j),
							timerUndefDelay);
				}
			}
		}

		void annotate (timerAnalysisType el,
				timerTransition srcTran,
				timerTransition destTran,
				timerTime val) {
			if (el == timerEarly) {
				theEarlyDelay[srcTran][destTran] = val;
			} else {
				theLateDelay[srcTran][destTran] = val;
			}
		}


	private:
		timerTime theEarlyDelay[timerTrans][timerTrans];
		timerTime theLateDelay[timerTrans][timerTrans];
};		

class timerArcDelay : public timerDelay {
	public:
		timerArcDelay () : timerDelay () {
			for (int i = 0; i < timerTrans; i++) {
				for (int j = 0; j < timerTrans; j++) {
				  theEarlyTrans[i][j] = false; 
				  theLateTrans[i][j] = false; 
				}
			}
		  
		}

		void markValidTran (timerAnalysisType el,
				    timerTransition srcTran,
				    timerTransition destTran) {
			if (el == timerEarly)
				theEarlyTrans[srcTran][destTran] = true;
			if (el == timerLate)
				theLateTrans[srcTran][destTran] = true;
		}

		void annotateDelay (timerAnalysisType el,
				    timerTransition srcTran,
				    timerTransition destTran,
				    timerTime val) {
		   annotate (el, srcTran, destTran, val);
		   markValidTran (el, srcTran, destTran); 
		}

		bool isValidTran (timerAnalysisType el,
				  timerTransition srcTran,
				  timerTransition destTran) {
			return (el == timerEarly) ? theEarlyTrans[srcTran][destTran] :
				theLateTrans[srcTran][destTran];
		}

	private:

		bool      theEarlyTrans[timerTrans][timerTrans];
		bool      theLateTrans[timerTrans][timerTrans];
};

//The class containing the point time/for early and late
class timerPointTime {

	public:
		timerPointTime () { }

		void annotate (timerAnalysisType el, timerTransition rf, timerTime val) {
	    	  theTime [el][rf] = val;
		}	

		timerTime getTime (timerAnalysisType el, timerTransition rf) {
		  return theTime [el][rf];
		}
	private:
		timerTime theTime[timerAnalysis][timerTrans];
};

class timerDelayCalcArgs {
	public:
		//Data Members all are public
        	timerAnalysisType theEL;
		timerTime	  theStageInputTran;
		timerCap 	  theStageLoad;
		//timerTransition	  theSourceRF;
		//timerTransition	  theSinkRF;
		timerTime	  theStageDelay;
 		timerPinTag	* theSourceTag;
		timerPinTag	* theSinkTag;
		diganaVertex	  theSource;
		diganaVertex	  theSink;
		diganaGraph	* theTimer;

		timerDelayCalcArgs () {
		  theEL = timerAnalysis;
		  theStageInputTran = theStageLoad = theStageDelay = timerUndefDelay;
//                  theSourceRF = theSinkRF = timerTrans;
		  theSourceTag = NULL;
		  theSinkTag = NULL;
		}

		void setupStage (timerAnalysisType el, 
			    	 diganaVertex source, timerPinTag * sourceTag,
			    	 diganaVertex sink, timerPinTag * sinkTag,
				 timerCap load
			        ) {
		  theEL = el;
		  theSourceTag = sourceTag;
		  theSinkTag = sinkTag;
		  theSource  = source;
		  theSink = sink;
		  theStageLoad = load;
		}

};

void computeEdgeDelayAndPropagateArrival (timerDelayCalcArgs &); 

#endif
