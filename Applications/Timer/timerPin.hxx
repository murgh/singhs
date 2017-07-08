#include "timerUtils.hxx"
#include "timerConstraints.hxx"
#include "timer.hxx"

#ifndef timerPin_H
#define timerPin_H

class timerPinTime;
class timerPinInfo;
class timerPinTagContainer; 

class timerPinTag {

	public:
		timerPinTag (bool clockTag, bool arr, int srcId) {
		  theArrival = arr;
		  theClockPath = clockTag;
		  theSourceId = srcId; 
		  theMasterTag = NULL;
		}

		timerPinTag (const timerPinTag & tag) {
		  theArrival = tag.theArrival;
		  theClockPath = tag.theClockPath;
		  theSourceId = tag.theSourceId; 
		  theMasterTag = tag.theMasterTag;
		}

		bool operator == (const timerPinTag & other) {
		  return (
			  theSourceId == other.theSourceId
			 );
		}

		int getSource () const { return theSourceId; }
		bool isClockPath () const { return theClockPath; }
		bool isDataPath () const { return !theClockPath; }

		void setClockPath () { theClockPath = true; }
		void setDataPath () { theClockPath = false; }

		bool isSplitTag () { return (theMasterTag != NULL); }

		bool isArrivalTag () { return theArrival; }
		bool isRequiredTag () { return !theArrival; }

		void setArrivalTag () { theArrival = true; }
		void setRequiredTag () { theArrival = false; }

		void setMasterTag (timerPinTagContainer * tag) { theMasterTag = tag; }
		timerPinTagContainer * getMasterTag () { return theMasterTag; }

	private:
		//Not taking the polarity in consideration for now
		//bool	     thePositivePolarity;
		bool		theArrival; //Arrival tag, if false it acts as required tag
		bool 	     	theClockPath;
		int	     	theSourceId;
		timerPinTagContainer * theMasterTag; //For Split Tags
};

class timerPinTagContainer {

	public:
		timerPinTagContainer () {
		  theTagSet.clear ();
		  theTagSetSize = 0;
		}

		void addTag (timerPinTag * tag) {
		  theTagSet.push_back (tag);
		  theTagSetSize++;
		}

		int getTagSetSize () { return theTagSetSize; }
		std::list<timerPinTag *> & getTagSet () { return theTagSet; }

	class Iterator {
	 
		public:
			Iterator (timerPinTagContainer * cont) {
			  theIterSize = cont->getTagSetSize ();
			  theTagList = cont->getTagSet ();
			}

			timerPinTag * next () {
			  if (theIterSize == 0) 
			    return NULL;
			  timerPinTag * tag = theTagList.front ();
			  theTagList.pop_front ();
			  theTagList.push_back (tag);
			  theIterSize--;
			  return tag;
			}

		private:
			int theIterSize;
			std::list<timerPinTag *> theTagList;
	};
	private:
		std::list<timerPinTag *> theTagSet;
		int			 theTagSetSize;
};

struct timerPinTimeArgs {
	public:
		timerTime time;
		timerAnalysisType el;
		timerTransition trans;
};

typedef std::pair<timerPinTag *, timerPinTime *> timerClockTime;

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
		  thePinTagContainer = NULL;
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
		  thePinTagContainer = NULL;
		  theArrival.clear ();
		  theRequired.clear ();
		}

		void setIsClock () {theIsClock = true;}
		void setIsData () {theIsData = true;}
		void setDirection (timerPinDirection dir) { theDirection = dir; }
		void setIdentity (timerPinIdentifier id) { theIdentity = id; }


		bool getIsClock () const { return theIsClock; }
		bool getIsData () const { return theIsData; }

		//Is Clock source only if it is an IO port having clock bit true
		bool getIsClockSrc () const { return (
				(theIdentity == timerIOPort) && theIsClock); 
					    }

		//Data path start point if it is a data IO port OR Latch Q Pin
		bool getIsDataStart () const { return (
				(theIdentity == timerIOPort && !theIsData)
				                      || 
				(theIdentity == timerLatchData && theDirection == timerOutput) 
						      ); 
					     }

		//Clock Path end point if it is a latch Clock pin or an IO out port
		bool getIsClockEnd () const { return (
				(theIdentity == timerLatchClock) 
						     ||
				(theIdentity == timerIOPort && theDirection == timerOutput)
						     ); 
					   } 

		//Data end point if it is Output port OR is a latch D pin
		bool getIsDataEnd () const { return (
				(theIdentity == timerIOPort && theDirection == timerOutput) 
				                     || 
				(theIdentity == timerLatchData && theDirection == timerInput) 
						    ); 
					   }

		bool getIsIOPort () const { return (theIdentity == timerIOPort); }
		std::string getName () const { return thePinName;} 
		timerPinDirection getDirection () const { return theDirection; }
		timerPinIdentifier getIdentity () const { return theIdentity; }

		void setTagSplitPoint () { theIsSplitPoint = true; }
		bool isTagSplitPoint () { return theIsSplitPoint; }

		void addPinTimeInfo (timerClock *, timerPinTimeArgs &, bool); 

		//Propagate the requisite pin info from a source pin to sink pin
		static void 
		propagatePinInfo (timerPinInfo * source, timerPinInfo * sink) {
		  if (source->getIsClock ()) sink->setIsClock (); 
		  if (source->getIsData ()) sink->setIsData (); 
		}

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
		  printf ("Pin(%s) ", thePinName.c_str ());
		  printf ("isClock(%d) isData(%d) ", theIsClock, theIsData); 
		  printf ("type(%s) ", get_identifier_name ().c_str ()); 
		  printf ("dir(%s)\n", get_direction ().c_str ()); 
		}

		void assert_IO_Delay (timerPinTag & cTag, timerTime value, bool isInput) {
		  if (isInput)
	            cTag.setArrivalTag ();
		  else
		    cTag.setRequiredTag ();

		  return (isInput) ? assert_Input_Delay (cTag, value) : 
		  		     assert_Output_Delay (cTag, value);	  
		}

		void writePin (timerAnalysisType el, timerTransition tran) {
		}

		void assert_Clock (timerPinTag & cTag, timerTime time) {
		  assert_Input_Delay (cTag, time);
		}

		timerPinTagContainer * get_pin_tag_container () {
		  return thePinTagContainer; 
		}

		timerPinTagContainer * get_pin_tag_container_pvt () {
		  if (!thePinTagContainer)
		    thePinTagContainer = new timerPinTagContainer;
		  return thePinTagContainer;
		}

		void assert_pin_tag (timerPinTag * cTag) {
		  get_pin_tag_container_pvt ()->addTag (cTag); 
		}

		timerPinTag * getPinTag (timerPinTag & tag) {
		  if (!thePinTagContainer)
		    return NULL;
		  timerPinTag * storedTag;
		  timerPinTagContainer::Iterator itr (thePinTagContainer);
		  while ( (storedTag = itr.next ()) ) {
		    if (*storedTag == tag)
		      return storedTag;
		  }   
		  return NULL;
		}
	private:
		std::string thePinName;
		bool	    theIsClock;
		bool	    theIsData;
		bool	    theIsSplitPoint;
		timerPinIdentifier theIdentity;
		timerPinDirection theDirection;
		timerPinTagContainer * thePinTagContainer;
		std::list<timerClockTime> theArrival; //List of arrival tags and arrival time
		std::list<timerClockTime> theRequired;//List of required tags and required

		void assert_Input_Delay (timerPinTag & cTag, timerTime value) {
		  timerClockTime timerInfo;
		  /*if ( isTagPresent (cTag, timerInfo) ) { 
		    timerInfo.second->setTime(timerEarly, timerFall, value);		     
		    timerInfo.second->setTime(timerEarly, timerRise, value);		     
		    timerInfo.second->setTime(timerLate, timerFall, value);		     
		    timerInfo.second->setTime(timerLate, timerRise, value);		     
		    return;
		  }*/
		  timerPinTime * time = new timerPinTime (value);
		  timerPinTag * cTagN = new timerPinTag (cTag);
		  assert_pin_tag (cTagN);
		  theArrival.push_front (timerClockTime (cTagN, time) );
		  //printf ("InputDelay : %s %f\n", thePinName.c_str (), value);
		}	

		void assert_Output_Delay (timerPinTag & cTag, timerTime value) {
		  timerClockTime timerInfo;
		  /*if ( isTagPresent (cTag, timerInfo) ) { 
		    timerInfo.second->setTime(timerEarly, timerFall, value);		     
		    timerInfo.second->setTime(timerEarly, timerRise, value);		     
		    timerInfo.second->setTime(timerLate, timerFall, value);		     
		    timerInfo.second->setTime(timerLate, timerRise, value);		     
		    return;
		  }*/
		  timerPinTime * time = new timerPinTime (value);
		  timerPinTag * cTagN = new timerPinTag (cTag);
		  assert_pin_tag (cTagN);
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
