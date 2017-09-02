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

class timerPinDelay {

	public:
		timerPinDelay () {
		  theDelay = timerUndefDelay;
		  theTran = timerUndefDelay;
		  theTranType = timerTrans;
		  theClock = NULL;
		}

		timerPinDelay (const timerPinDelay & other) {
		  theDelay = other.theDelay;
		  theTran = other.theTran;
		  theTranType = other.theTranType;
		  theClock = other.theClock;
		}

		void operator = (const timerPinDelay & other) {
		  theDelay = other.theDelay;
		  theTran = other.theTran;
		  theTranType = other.theTranType;
		  theClock = other.theClock;
		}

		void setDelay (timerTime t) { theDelay = t; }
		void setTran (timerTime t) { theTran = t; }
		void setClock (timerClock * clock) { theClock = clock; }
		void setTranType (timerTransition tran) { theTranType = tran; }

		timerTime getDelay () { return theDelay; }
		timerTime getTran () { return theTran; }
		timerClock * getClock () { return theClock; }
		timerTransition getTranType () { return theTranType; }

		void print () {
		  printf ("rf(%c) del(%f) tran(%f) clock(%s)\n", (theTranType == timerRise) ? 'R' : 'F', 
				  				theDelay, theTran, theClock->getName ().c_str ());
		}
	private:
		timerTime theDelay;
		timerTime theTran;
		timerTransition theTranType;
		timerClock * theClock;
};

class timerPinDelayContainer {

	public:
		timerPinDelayContainer () {
		  theContainer = NULL;
		}

		~timerPinDelayContainer () {
		  /*
		  std::map<timerClock *, std::list<timerPinDelay *> *>::iterator itr; 
		  for (itr = theContainer.begin (); itr != theContainer.end (); ++itr) {
		    std::list<timerPinDelay *> * delayList = itr->second;
		    delayList->clear ();
		    delete delayList;
		  }
		  */	  
		  if (!theContainer) return;
		  theContainer->clear ();
		  delete theContainer;	
		  theContainer  = NULL;
		}

		void addPinDelay (/*timerClock * clock, */timerPinDelay * delay) {
		/*
		  std::map<timerClock *, std::list<timerPinDelay *> *>::iterator itr = theContainer.find (clock);
		  std::list<timerPinDelay *> * delayList = NULL;
		  if (itr == theContainer.end ()) {
		    delayList = new std::list<timerPinDelay *>;
		  } else {
		    delayList = itr->second; 
		  } 
		  delayList->push_back (delay);
		*/
		  if (!theContainer)
		    theContainer = new std::list<timerPinDelay *>;
		  theContainer->push_back (delay);
		}

		void print () {
		  if (!theContainer) return;
		  std::list<timerPinDelay *>::iterator itr = theContainer->begin ();
		  for (; itr != theContainer->end (); ++itr) {
	             timerPinDelay * delayObj = *itr;
     		     delayObj->print ();			     
		  }
		  printf ("\n");
		}

		class Iterator {

			public:
				Iterator (timerPinDelayContainer * container) {
				  theList = container->theContainer; 				  
				  theSize = theList->size ();
				}

				timerPinDelay * next () {
				  if (!theList || theList->empty () || theSize == 0)
				    return NULL;
				  timerPinDelay * delayObj = theList->front ();
				  theList->pop_front ();  
				  theList->push_back (delayObj);
				  theSize--;
				  return delayObj;
				}

			private:
				std::list<timerPinDelay *> * theList;
				int  theSize;
		};

	private:
		//std::map<timerClock *, std::list<timerPinDelay *> *> theContainer;
		std::list<timerPinDelay *> * theContainer;
};

class timerDelayCalcArgs {
	public:
		//Data Members all are public
        	timerAnalysisType theEL;
		timerTime	  theStageInputTran;
		timerTime	  theStageOutputTran;
		timerCap 	  theStageLoad;
		timerTransition	  theSourceRF;
		timerTransition	  theSinkRF;
		timerTime	  theStageDelay;
 		timerPinTag	* theSourceTag;
		timerPinTag	* theSinkTag;
		diganaVertex	  theSource;
		diganaVertex	  theSink;
		diganaGraph	* theTimer;
		timerClock	* theClock;
		bool		  theFirstStage;

		timerDelayCalcArgs () {
		  theEL = timerAnalysis;
		  theStageInputTran = theStageOutputTran = timerUndefDelay; 
		  theStageLoad = theStageDelay = timerUndefDelay;
                  theSourceRF = theSinkRF = timerTrans;
		  theSourceTag = NULL;
		  theSinkTag = NULL;
		}

		void setupStage (timerAnalysisType el, 
			    	 diganaVertex source, timerPinTag * sourceTag,
			    	 diganaVertex sink, timerPinTag * sinkTag,
				 timerCap load, bool firstStage
			        ) {
		  theEL = el;
		  theSourceTag = sourceTag;
		  theSinkTag = sinkTag;
		  theSource  = source;
		  theSink = sink;
		  theStageLoad = load;
		  theFirstStage = firstStage;
		}

		void setupStage (timerAnalysisType el,
				 timerClock * clock,
				 timerTransition srcRF,
				 timerTime inputTran,
				 diganaVertex source,
				 diganaVertex sink,
				 timerCap load
				) {
		  theEL = el;
		  theClock = clock;
		  theSourceRF = srcRF;
		  theStageInputTran = inputTran;
		  theSource  = source;
		  theSink = sink;
		  theStageLoad = load;
		}

};

void computeEdgeDelayAndPropagateArrival (timerDelayCalcArgs &); 

void computeCheckValue (timerDelayCalcArgs & args);

#endif
