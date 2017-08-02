#include "timerUtils.hxx"
#include "timerPin.hxx"
#include "timer.hxx"
#include "timerDelay.hxx"

#ifndef TIMER_EDGE
#define TIMER_EDGE

class timerArcData {
	public:
		timerArcData (timerArcType aType) {
			arcType = aType;
		}

  	private:
	  	timerArcType arcType;
		bool	     theIsCheck;
};

//Main timer info container class
class timerArcInfo {

	public:
		timerArcInfo (timerLibArc * arc) {
		  theLibArc = arc;
		  theTagDelayMap.clear ();
		}

		timerArcInfo () {
		  theLibArc = NULL;
		  theTagDelayMap.clear ();
		}

		void annotateDelay (timerPinTag * tag,
				    timerAnalysisType el,
				    timerTransition srcTran, 
				    timerTransition destTran,
				    timerTime val) {

		   std::map<timerPinTag *, timerArcDelay *>::iterator itr = theTagDelayMap.find (tag);	
		   timerArcDelay * delay = NULL;
		   if (itr != theTagDelayMap.end ()) {
		     delay = itr->second;
		   } else {
		     delay = new timerArcDelay;
		     theTagDelayMap.insert (std::pair<timerPinTag *, timerArcDelay *> (tag, delay));
		   } 

		   delay->annotate (el, srcTran, destTran, val);
		}

		bool isValidTran (timerPinTag * tag,
				  timerAnalysisType el,
				  timerTransition srcTran,
				  timerTransition destTran) {
		   std::map<timerPinTag *, timerArcDelay *>::iterator itr = theTagDelayMap.find (tag);	
		   timerArcDelay * delay = NULL;
		   if (itr != theTagDelayMap.end ()) {
		     delay = itr->second;
		     delay->isValidTran (el, srcTran, destTran);
		   } else {
		     return false;
		   }
		}	       

		void computeDelay (timerDelayCalcArgs & args) {
		  //For net arcs : if (!theLibArc) 
		  theLibArc->ComputeDelay (args);
		}	
		void computeTransition (timerDelayCalcArgs & args) {
		  theLibArc->ComputeDelay (args);
		}	
		void computeCheck (timerDelayCalcArgs & args) {
		  theLibArc->ComputeDelay (args);
		}	

	private:
		timerArcData *	theArcData;
		//Might not be needed here, we shall revisit
		std::map<timerPinTag *, timerArcDelay *> theTagDelayMap;
		timerLibArc * theLibArc; 
};

class timerArcProperty : public diganaDynamicGraphProperty {
	public:
		timerArcProperty (timerArcInfo * aInfo) {
		   theArcInfo = aInfo;
		}

		timerArcProperty () {
		   theArcInfo = NULL;
		   	
		}

		timerArcInfo * getArcInfo () const { return theArcInfo; }

	private:	
		timerArcInfo * theArcInfo;
};

#endif
