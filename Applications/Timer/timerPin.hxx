#include "timerUtils.hxx"

#ifndef timerPin_H
#define timerPin_H

class timerPinTime;
class timerPinInfo;

struct timerPinTimeArgs {
	public:
		timerTime time;
		timerAnalysisType el;
		timerTransition trans;
};

//The main class to contain the pin timing information
//for timer contains the clock and arrival/required time
//regarding it.
class timerPinTime {

	public:
		timerPinTime (timerClock * clock) {
		  theClock = clock;
		  for (int i = timerEarly; i < timerAnalysis; i++) 
			  for (int j = timerRise; j < timerTrans; j++) 
				  theTime [i][j] = timerUndefDelay;
		}

		timerPinTime (timerClock * clock, timerTime time) {
		  theClock = clock;
		  for (int i = timerEarly; i < timerAnalysis; i++)
			  for (int j = timerRise; j < timerTrans; j++) 
				  theTime [i][j] = time;
		}

		void addPinTimeInfo (timerPinTimeArgs & arg) {
			theTime [arg.el][arg.trans] = arg.time;
		}

		void setTime (timerAnalysisType el, timerTransition tran, timerTime val) {
			theTime [el][tran] = val;
		}

		timerTime getTime (timerAnalysisType el, timerTransition tran) {
			return theTime [el][tran];
		}
		timerPinTime & operator = (const timerPinTime & time) {
		  for (int i = timerEarly; i < timerAnalysis; i++) 
			  for (int j = timerRise; j < timerTrans; j++) 
				  theTime [i][j] = time.theTime [i][j];
	          return *this;		
		}	
		timerClock* getClock () const { return theClock; }

	private:
		timerTime     theTime[timerAnalysis][timerTrans];
		timerClock*   theClock;
};

//The main timer pin info container class
//contains all the relevant information to
//be stored on a timer pin
class timerPinInfo {

	public:
		timerPinInfo (std::string name) {
		  thePinName = name;
		  theIsClock = false;
		  theIsData = false;
		  theArrival.clear ();
		  theRequired.clear ();
		}

		timerPinInfo (std::string name, 
			      bool isClock, 
			      bool isData,
			      timerPinIdentifier identity,
			      timerPinDirection direction) {
		  thePinName = name;
	  	  theIsClock = isClock;
		  theIsData = isData;
		  theIdentity = identity;
		  theDirection = direction;
		  theArrival.clear ();
		  theRequired.clear ();
		}

		void setIsClock () {theIsClock = true;}
		void setIsData () {theIsData = true;}
		void setDirection (timerPinDirection dir) { theDirection = dir; }
		void setIdentity (timerPinIdentifier id) { theIdentity = id; }


		bool getIsClock () const { return theIsClock; }
		bool getIsData () const { return theIsData; }
		timerPinDirection getDirection () const { return theDirection; }
		timerPinIdentifier getIdentity () const { return theIdentity; }

		void addPinTimeInfo (timerClock *, timerPinTimeArgs &, bool); 

	private:
		std::string thePinName;
		bool	    theIsClock;
		bool	    theIsData;
		timerPinIdentifier theIdentity;
		timerPinDirection theDirection;
		std::vector<timerPinTime*> theArrival;
		std::vector<timerPinTime*> theRequired;
};

class timerPinProperty : public diganaDynamicGraphProperty {
	public:
		timerPinProperty () :
			thePinInfo (NULL) { }
		timerPinProperty (timerPinInfo * pinInfo) :
			thePinInfo (pinInfo) { }
		timerPinInfo * getPinInfo () const { return thePinInfo; }

	private:
		timerPinInfo * thePinInfo;
};
#endif
