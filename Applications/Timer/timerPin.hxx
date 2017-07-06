#include "timerUtils.hxx"
#include "timerConstraints.hxx"
#include "timer.hxx"

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

typedef std::pair<timerClockTag *, timerPinTime *> timerClockTime;

//The main class to contain the pin timing information
//for timer contains the clock and arrival/required time
//regarding it.
class timerPinTime {

	public:
		timerPinTime () {
		  for (int i = timerEarly; i < timerAnalysis; i++) 
			  for (int j = timerRise; j < timerTrans; j++) 
				  theTime [i][j] = timerUndefDelay;
		}

		timerPinTime (timerTime time) {
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

	private:
		timerTime     theTime[timerAnalysis][timerTrans];
};

//The main timer pin info container class
//contains all the relevant information to
//be stored on a timer pin
//Each timer p
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

		void assert_IO_Delay (timerClockTag & cTag, timerTime value, bool isInput) {
		  if (isInput)
	            cTag.setArrivalTag ();
		  else
		    cTag.setRequiredTag ();

		  return (isInput) ? assert_Input_Delay (cTag, value) : 
		  		     assert_Output_Delay (cTag, value);	  
		}

		void writePin (timerAnalysisType el, timerTransition tran) {
		  //Pin Name, Arrival, Required, Slack, Clock
		  //printf ("%20s|%5f|%5f|%5f|%10s\n", thePinName.c_str (), 
				  		     //theArrival);
		}

		bool isTagPresent (timerClockTag & tag, timerClockTime & timeInfo) {
		  std::list<timerClockTime>::iterator it, eItr;
		  if (tag.isArrivalTag ()) { 
		    it = theArrival.begin ();
		    eItr = theArrival.end ();
	         	    
		  } else {
		    it = theRequired.begin ();  
		    eItr = theRequired.end ();
		  }

		  for (; it != eItr; ++it) {
		    timeInfo = *it;
		    if (*timeInfo.first == tag)
		      return true;
		  }

		  return false;
		}

		void assert_Clock (timerClockTag & cTag, timerTime value) {
		  timerClockTime timerInfo;
		  if ( isTagPresent (cTag, timerInfo) ) { 
		    timerInfo.second->setTime(timerEarly, timerFall, value);		     
		    timerInfo.second->setTime(timerEarly, timerRise, value);		     
		    timerInfo.second->setTime(timerLate, timerFall, value);		     
		    timerInfo.second->setTime(timerLate, timerRise, value);		     
		    return;
		  }
		  timerPinTime * time = new timerPinTime (value);
		  timerClockTag * cTagN = new timerClockTag (cTag);
		  theArrival.push_front (timerClockTime (cTagN, time) );
		}

	private:
		std::string thePinName;
		bool	    theIsClock;
		bool	    theIsData;
		timerPinIdentifier theIdentity;
		timerPinDirection theDirection;
		std::list<timerClockTime> theArrival; //List of arrival tags and arrival time
		std::list<timerClockTime> theRequired;//List of required tags and required

		void assert_Input_Delay (timerClockTag & cTag, timerTime value) {
		  timerClockTime timerInfo;
		  if ( isTagPresent (cTag, timerInfo) ) { 
		    timerInfo.second->setTime(timerEarly, timerFall, value);		     
		    timerInfo.second->setTime(timerEarly, timerRise, value);		     
		    timerInfo.second->setTime(timerLate, timerFall, value);		     
		    timerInfo.second->setTime(timerLate, timerRise, value);		     
		    return;
		  }
		  timerPinTime * time = new timerPinTime (value);
		  timerClockTag * cTagN = new timerClockTag (cTag);
		  theArrival.push_front (timerClockTime (cTagN, time) );
		  //printf ("InputDelay : %s %f\n", thePinName.c_str (), value);
		}	

		void assert_Output_Delay (timerClockTag & cTag, timerTime value) {
		  timerClockTime timerInfo;
		  if ( isTagPresent (cTag, timerInfo) ) { 
		    timerInfo.second->setTime(timerEarly, timerFall, value);		     
		    timerInfo.second->setTime(timerEarly, timerRise, value);		     
		    timerInfo.second->setTime(timerLate, timerFall, value);		     
		    timerInfo.second->setTime(timerLate, timerRise, value);		     
		    return;
		  }
		  timerPinTime * time = new timerPinTime (value);
		  timerClockTag * cTagN = new timerClockTag (cTag);
		  theRequired.push_front (timerClockTime (cTagN, time) );
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
