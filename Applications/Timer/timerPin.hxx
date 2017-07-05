#include "timerUtils.hxx"
#include "timerConstraints.hxx"

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
		bool getIsClockSrc () const { ((theIdentity == timerIOPort) && theIsClock); }
		bool getIsIOPort () const { return (theIdentity == timerIOPort); }
		std::string getName () const { return thePinName;} 
		timerPinDirection getDirection () const { return theDirection; }
		timerPinIdentifier getIdentity () const { return theIdentity; }

		void addPinTimeInfo (timerClock *, timerPinTimeArgs &, bool); 

		std::string get_direction () {
		  if (theDirection == timerInput) return std::string ("input");
		  if (theDirection == timerOutput) return std::string ("output");
		  if (theDirection == timerInOut) return std::string ("inout");
		  return std::string ("none");
		}

                std::string get_identifier_name () {
		  if (theIdentity == timerIOPort) return std::string ("IO_Port");
		  if (theIdentity == timerComboPin) return std::string ("ComboPin");
		  if (theIdentity == timerLatchData) return std::string ("LatchData");
		  if (theIdentity == timerLatchClock) return std::string ("LatchClock");
		  if (theIdentity == timerPinVirtualNode) return std::string ("VirtualNode");
		  if (theIdentity == timerPinIDNone) return std::string ("NONE");
		  return std::string ("NONE");
		}

		void print () {
		  printf ("Pin Info :\n");
		  printf ("Name : %s\n", thePinName.c_str ());
		  printf ("isClock,isData : %d,%d\n", theIsClock, theIsData); 
		  printf ("identifier : %s\n", get_identifier_name ().c_str ()); 
		  printf ("direction : %s\n", get_direction ().c_str ()); 
		}

		void assert_IO_Delay (timerClock * clock, timerTime value, bool isInput) {
		  return (isInput) ? assert_Input_Delay (clock, value) : 
		  		     assert_Output_Delay (clock, value);	  
		}

		void writePin (timerAnalysisType el, timerTransition tran) {
		  //Pin Name, Arrival, Required, Slack, Clock
		  //printf ("%20s|%5f|%5f|%5f|%10s\n", thePinName.c_str (), 
				  		     //theArrival);
		}

	private:
		std::string thePinName;
		bool	    theIsClock;
		bool	    theIsData;
		timerPinIdentifier theIdentity;
		timerPinDirection theDirection;
		std::map<timerClock *, timerPinTime*> theArrival;
		std::map<timerClock *, timerPinTime*> theRequired;

		void assert_Input_Delay (timerClock * clock, timerTime value) {
		  std::map<timerClock *, timerPinTime*>::iterator it = theArrival.begin ();
		  if (it != theArrival.end ()) {
		    timerPinTime * time = it->second;
		    time->setTime(timerEarly, timerFall, value);		     
		    time->setTime(timerEarly, timerRise, value);		     
		    time->setTime(timerLate, timerFall, value);		     
		    time->setTime(timerLate, timerRise, value);		     
		    return;
		  }
		  timerPinTime * time = new timerPinTime (clock, value);
		  theArrival.insert (std::pair<timerClock *, timerPinTime*> (clock, time) );
		  //printf ("InputDelay : %s %f\n", thePinName.c_str (), value);
		}	

		void assert_Output_Delay (timerClock * clock, timerTime value) {
		  std::map<timerClock *, timerPinTime*>::iterator it = theRequired.begin ();
		  if (it != theRequired.end ()) {
		    timerPinTime * time = it->second;
		    time->setTime(timerEarly, timerFall, value);		     
		    time->setTime(timerEarly, timerRise, value);		     
		    time->setTime(timerLate, timerFall, value);		     
		    time->setTime(timerLate, timerRise, value);		     
		    return;
		  }
		  timerPinTime * time = new timerPinTime (clock, value);
		  theRequired.insert (std::pair<timerClock *, timerPinTime*> (clock, time) );
		  //printf ("OutputDelay : %s %f\n", thePinName.c_str (), value);
		}	

};

class timerPinProperty : public diganaDynamicGraphProperty {
	public:
		timerPinProperty () :
			thePinInfo (NULL) { }
		timerPinProperty (timerPinInfo * pinInfo) :
			thePinInfo (pinInfo) { }
		timerPinProperty operator = (const timerPinProperty & other)
		{
			thePinInfo = other.thePinInfo;
			return *this;
		}
		timerPinInfo * getPinInfo () const { return thePinInfo; }

	private:
		timerPinInfo * thePinInfo;
};
#endif
