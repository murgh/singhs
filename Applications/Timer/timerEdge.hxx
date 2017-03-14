#include "timerUtils.hxx"

#ifndef TIMER_EDGE
#define TIMER_EDGE

//2 X 2 delay values for input and output
//transition combinations
class timerArcDelay {
	public:
		timerArcDelay () {
			for (int i = 0; i < timerTrans; i++)
				for (int j = 0; j < timerTrans; j++)
					annotate (((timerTransition)i), 
						  ((timerTransition)j), 
						  timerUndefDelay); 
		}

		void annotate (timerTransition srcTran,
			       timerTransition destTran,
		       	       timerTime val) {
			arcDelay[srcTran][destTran] = val;
			if (val == timerUndefDelay)
			  validTrans[srcTran][destTran] = true;
			else
			  validTrans[srcTran][destTran] = false;
		}		

		bool isValidTran (timerTransition srcTran,
				  timerTransition destTran) {
			return validTrans[srcTran][destTran];
		}

	private:
		timerTime arcDelay[timerTrans][timerTrans];
		bool	  validTrans[timerTrans][timerTrans];
};

class timerArcData {
	public:
		timerArcData (timerArcType aType) {
			arcType = aType;
		}
		void setCheckType (timerCheckArcType check) {
			arcSubType.checkArcType = check;
			theIsCheck = true;
		}

		void setDelayType (timerDelayArcType delayT) {
			arcSubType.delayArcType = delayT;
			theIsCheck = false;
		}

		timerCheckArcType getCheckType () const {
			return arcSubType.checkArcType;
		}

		timerDelayArcType getDelayType () const {
			return arcSubType.delayArcType;
		}

		bool isCheckArc () const {
			return theIsCheck;
		}
  	private:
	  	timerArcType arcType;
		bool	     theIsCheck;
	  	union {
			timerDelayArcType delayArcType;
			timerCheckArcType checkArcType;
	  	} arcSubType;
};

//Main timer info container class
class timerArcInfo {

	public:
		timerArcInfo () {
		}

		void annotateDelay (timerTransition srcTran, 
				    timerTransition destTran,
				    timerTime val) {
			timerDelay->annotate (srcTran, destTran, val);
		}

		bool isValidTran (timerTransition srcTran,
				  timerTransition destTran) {
		       return timerDelay->isValidTran (srcTran, destTran);
		}	       

	private:
		timerArcData*	arcInfo;
		timerArcDelay*  timerDelay;
};

class timerArcProperty : public diganaDynamicGraphProperty {
	public:
		timerArcInfo * theArcInfo;
};

#endif
