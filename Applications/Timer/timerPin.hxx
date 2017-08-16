#include "timerUtils.hxx"
#include "timerConstraints.hxx"
#include "timer.hxx"
#include "timerDelay.hxx"
#include "timerLibData.hxx"

#ifndef timerPin_H
#define timerPin_H

#define UDEF_PinTagGroupId -1

class timerPinTime;
class timerPinInfo;
class timerPinTag;
class timerPinTagContainer;
class timerPointTime;

//The main class to contain the pin timing information
//for timer contains the clock and arrival/required time
//regarding it.
class timerPinTime {

	public:
		timerPinTime () {
		  theClockTimeMap.clear ();
		}

		void annotateArrival (timerClock * clock, 
				    timerAnalysisType el,
				    timerTransition rf,
				    timerTime del); 

	        void annotateTransition (timerClock * clock,
				         timerAnalysisType el,
				         timerTransition rf,
				         timerTime tran); 

		void populateClockObjects (std::list<timerClock *> & clockList);		

		timerTime getArrival (timerClock * clock, int el, int rf);

		timerTime getTransition (timerClock * clock, int el, int rf);

	private:
		//Map of the clock delay and transitions on a tag pin 
		std::map<timerClock *, std::pair <timerPointTime *, timerPointTime *> > theClockTimeMap;
};

class timerPinTagContainer {

	public:
		timerPinTagContainer () {
		  theTagSet.clear ();
		  theTagSetSize = 0;
		}

		void addTag (timerPinTag * tag) {
		  std::pair<std::set<timerPinTag *>::iterator, bool> ret = theTagSet.insert (tag);
		  if (ret.second == true)
		    theTagSetSize++;
		}

		int getTagSetSize () { return theTagSetSize; }
		std::set<timerPinTag *> & getTagSet () { return theTagSet; }

	class Iterator {
	 
		public:
			//Later this can be cached : TODO
			void buildExpandedTagSet (timerPinTagContainer * cont); 

			Iterator (timerPinTagContainer * cont) {
			  theIterSize = 0;
			  theTagSet.clear ();
			  buildExpandedTagSet (cont);	
			  std::set<timerPinTag *>::iterator itr;
			  for (itr = theTagSet.begin (); itr != theTagSet.end (); ++itr) {
			    theTagList.push_back (*itr);
			  }
			  assert (theIterSize == theTagList.size ());
			}

			~Iterator () {
			  theTagSet.clear ();
			  theTagList.clear ();
			}

			timerPinTag * next () {
			  if (theIterSize == 0) 
			    return NULL;
			  timerPinTag * tag = theTagList.front ();
			  theTagList.pop_front ();
			  theIterSize--;
			  return tag;
			}

		private:
			int theIterSize;
			std::set<timerPinTag *> theTagSet;
			std::list<timerPinTag *> theTagList;
	};
	private:
		std::set<timerPinTag *>  theTagSet;
		int			 theTagSetSize;
};

class timerPinTag {

	public:
		timerPinTag (bool clockTag, bool arr, int srcId) {
		  theArrival = arr;
		  theClockPath = clockTag;
		  theSourceId = srcId; 
		  theMergeToTag = NULL;
		  theMasterTag = NULL;
		  theTagContainer = NULL;
		  theTagId = theTagCount++;
		  theForwardMergeCount = 0;
		  thePinTime = NULL;
		}

		timerPinTag (const timerPinTag & tag) {
		  theArrival = tag.theArrival;
		  theClockPath = tag.theClockPath;
		  theSourceId = tag.theSourceId; 
		  theMergeToTag = tag.theMergeToTag;
		  theMasterTag = tag.theMasterTag;
		  theTagContainer = tag.theTagContainer;
		  theTagId = tag.theTagId;
		  theForwardMergeCount = tag.theForwardMergeCount;
		  thePinTime = tag.thePinTime;
		}

		bool operator == (const timerPinTag & other) {
		  return (
			  theSourceId == other.theSourceId
			 );
		}

		void print (std::string tag) {
		 printf ("%s(ID(%d) MergeCount(%d)) ", tag.c_str (), theTagId, theForwardMergeCount);
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

		void setMasterTag (timerPinTag * tag) { theMasterTag = tag; }
		timerPinTag * getMasterTag () { return theMasterTag; }

		//If the tag is merged to another tag this will keep the forward link
		void setMergeToTag (timerPinTag * tag) { theMergeToTag = tag; }
		timerPinTag * getMergeToTag () { return theMergeToTag; }
/*
		void getUnionParentAndDistance (timerPinTag *& unionP, int & distance) {
		  distance = 0;
		  timerPinTag * currentTag = this;
		  unionP = currentTag;
		  while ( (currentTag = currentTag->theUnionParent) ) {
		    distance++;
		    unionP = currentTag;
		  }
		}

		static void performTagUnion (timerPinTag * tag1, timerPinTag * tag2) {
		  if (tag1 == tag2)
			  return;
		  timerPinTag * tag1Parent, * tag2Parent;
	          int tag1Distance, tag2Distance;
	          tag1->getUnionParentAndDistance (tag1Parent, tag1Distance);
	          tag2->getUnionParentAndDistance (tag2Parent, tag2Distance);
		  if (tag1Parent == tag2Parent)
			 return; 
		  if (tag1Distance > tag2Distance) {
		    tag2Parent->theUnionParent = tag1Parent;
		  } else {
		    tag1Parent->theUnionParent = tag2Parent;
		  }
		}

		static void performTagUnion (timerPinTag * tag1, timerPinTag * tag2) {
		  if (tag1 == tag2)
		    return;
		  timerPinTagContainer * tag1Cont = tag1->theTagContainer; 
		  timerPinTagContainer * tag2Cont = tag2->theTagContainer; 
		  //Both tag container empty
		  if (tag1Cont == NULL && tag2Cont == NULL) {
		    tag1Cont = new timerPinTagContainer; 
		    tag1Cont->addTag (tag1);
		    tag1Cont->addTag (tag2);
		    tag1->theTagContainer = tag1Cont;
		    tag2->theTagContainer = tag1Cont;
		    return;
		  }
		  //One if the container is empty
		  if (!tag1Cont) {
		    tag1->theTagContainer = tag2Cont;
		    tag2Cont->addTag (tag1);
		    return;
		  }

		  //One if the container is empty
		  if (!tag2Cont) {
		    tag2->theTagContainer = tag1Cont;
		    tag1Cont->addTag (tag2);
		    return;
		  }

		  //Both containers exist
		}

*/
		void incrementForwardMergeCount (diganaVertex mergePin) { 
		  theForwardMergeCount++; 
		/*
		  if (theForwardMergeCount <= TA_Timer::getGlobalForwardMergedCount ()) {
		    theTimingPropagationPoint = mergePin;
		  }*/ 
		  triggerIncrementForwardMergeCount (mergePin);
		}
		
		//Iterate on all the tags merged in it and omcrement 
		void triggerIncrementForwardMergeCount (diganaVertex mergePin) {
		  if (!theTagContainer)
		    return;

                  timerPinTagContainer::Iterator itr (theTagContainer);
		  timerPinTag * tag;
		  while ( (tag = itr.next ()) ) 
		    tag->incrementForwardMergeCount (mergePin);
		}

		timerPinTagContainer * get_tag_container () { return theTagContainer; }

		static bool areTagsInUnion (timerPinTag * tag1, timerPinTag * tag2) {
		  timerPinTag * tag1MasterTo = tag1;
		  timerPinTag * tag2MasterTo = tag2;
		  bool areInUnion = false;
		  //Arrival and Required tags cannot be union
		  if (tag1->theArrival != tag2->theArrival) return areInUnion;
		  while (!areInUnion && (tag1MasterTo || tag2MasterTo)) {
		    if (tag1MasterTo == tag2 || tag2MasterTo == tag1)
			    areInUnion = true;
		    tag1MasterTo = (tag1MasterTo) ? tag1MasterTo->theMergeToTag : NULL;
		    tag2MasterTo = (tag2MasterTo) ? tag2MasterTo->theMergeToTag : NULL;
		  }
		  return areInUnion;
		}

		void merge_pin_tag (timerPinTag * to_merge, diganaVertex mergePin) {
		  //If tag container is not present create it and 
		  //merge the tag to be merged in the container
		  if (!theTagContainer) {
		    theTagContainer = new timerPinTagContainer;	  
		  }	  
		  theTagContainer->addTag (to_merge); 
		  to_merge->setMergeToTag (this);
		  to_merge->incrementForwardMergeCount (mergePin);
		}	

		void setForwardMergeCount (int count) { theForwardMergeCount = count; }
		int getForwardMergeCount () { return theForwardMergeCount; }

		void setTimingPropagationPoint (diganaVertex vtx) { theTimingPropagationPoint = vtx; }
		diganaVertex getTimingPropagationPoint () { return theTimingPropagationPoint; }

		void annotatePinArrival (timerClock * clock, timerAnalysisType el, timerTransition rf, timerTime time) {
		  if (!thePinTime) thePinTime = new timerPinTime ();
		  thePinTime->annotateArrival (clock, el, rf, time);
		}

		void annotatePinTransition (timerClock * clock, timerAnalysisType el, timerTransition rf, timerTime transition) {
		  if (!thePinTime) thePinTime = new timerPinTime ();
		  thePinTime->annotateTransition (clock, el, rf, transition);
		}

		void populateClockObjects (std::list<timerClock *> & clockList) {
		  thePinTime->populateClockObjects (clockList);
		}		

		timerTime getArrival (timerClock * clock, int el, int rf) {
		  return thePinTime->getArrival (clock, el, rf);
		}

		timerTime getTransition (timerClock * clock, int el, int rf) {
		  return thePinTime->getTransition (clock, el, rf); 
		}

		void incrMergeLevel () { theMergeLevel++; }
		int getMergeLevel () { return theMergeLevel; }

	private:
		//Not taking the polarity in consideration for now
		//bool	     thePositivePolarity;
		bool		theArrival; //Arrival tag, if false it acts as required tag
		bool 	     	theClockPath;
		int	     	theSourceId;
		int		theTagId;
		int 		theForwardMergeCount;
		diganaVertex	theTimingPropagationPoint;
		timerPinTag   * theMasterTag; //For Split Tags
		timerPinTag   * theMergeToTag; //Tag to which this tag is merged
		timerPinTagContainer * theTagContainer;//Union set of tags.
		timerPinTime  * thePinTime;//The pin time info
		int		theMergeLevel;//Merge level		
		static int	theTagCount;
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
			theIsSplitPoint = false;
			thePinTag = NULL;
			theOtherPinTag = NULL;
			theLibPin = NULL;
		}

		timerPinInfo (std::string name, 
				bool isClock, 
				bool isData,
				timerPinIdentifier identity,
				timerPinDirection direction) {
			thePinName = name;
			theIsClock = isClock;
			theIsData = isData;
			theIsSplitPoint = false;
			theIdentity = identity;
			theDirection = direction;
			thePinTag = NULL;
			theOtherPinTag = NULL;
			theLibPin = NULL;
		}

		void setIsClock () {theIsClock = true;}
		void setIsData () {theIsData = true;}
		void setDirection (timerPinDirection dir) { theDirection = dir; }
		void setIdentity (timerPinIdentifier id) { theIdentity = id; }

		void setLibPin (timerLibPin * pin) { theLibPin = pin; }
		timerLibPin * getLibPin () { return theLibPin; }

		bool getIsClock () const { return theIsClock; }
		bool getIsData () const { return theIsData; }

		//Is Clock source only if it is an IO port having clock bit true
		bool getIsClockSrc () const { return (
				(theIdentity == timerIOPort) && theIsClock); 
		}

		//Data path start point if it is a data IO port OR Latch Q Pin
		bool getIsDataStart () const { return (
				(theIdentity == timerIOPort && theDirection == timerInput && theIsData)
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
		  printf ("dir(%s) ", get_direction ().c_str ()); 
		  if (theIsSplitPoint) printf ("splitPoint ");
		  if (thePinTag) thePinTag->print (std::string ("TAG"));
		  if (thePinTag && thePinTag->getMasterTag ()) thePinTag->getMasterTag()->print (std::string ("M_Tag"));
		  if (theOtherPinTag) theOtherPinTag->print (std::string ("OtherTAG"));
		  if (theOtherPinTag && theOtherPinTag->getMasterTag ()) theOtherPinTag->getMasterTag ()->print (std::string ("M_OtherTAG"));
		  printf ("\n");
		}

		void write_timing_info (FILE * file) {
		  fprintf (file, "%s\n", thePinName.c_str ());
		}

		void assert_IO_Delay (timerPinTag & ctag, timerClock * clock, timerTime value, bool isInput) {

		  return (isInput) ? assert_Input_Delay (ctag, clock, value) : 
		  		     assert_Output_Delay (ctag, clock, value);	  
		}

		void writePin (timerAnalysisType el, timerTransition tran) {
		}

		void assert_Clock (timerPinTag & ctag, timerClock * clock, timerTime time) {
		  assert_Input_Delay (ctag, clock, time);
		}

		timerPinTag * get_pin_tag () {
		  return thePinTag; 
		}

		timerPinTag * get_pin_other_tag () {
		  return theOtherPinTag; 
		}

		void assert_other_pin_tag (timerPinTag * tag) {
		  theOtherPinTag = tag;
		}

		void assert_pin_tag (timerPinTag * cTag) {
		  thePinTag = cTag; 
		}

		void clearDFSSeen () { theDFSSeen = false; }
		void setDFSSeen () { theDFSSeen = true; }
		bool isDFSSeen () { return theDFSSeen; }

		void setReferencePin (diganaVertex ref) { theReferencePin = ref; }
		diganaVertex gerReferencePin () { return theReferencePin; }

	private:
		std::string thePinName;
		bool	    theIsClock;
		bool	    theIsData;
		bool	    theIsSplitPoint;
		timerPinIdentifier theIdentity;
		timerPinDirection theDirection;
		timerPinTag * thePinTag;
		timerPinTag * theOtherPinTag;
		timerLibPin * theLibPin;
		bool		theDFSSeen;
		diganaVertex		theReferencePin;

		void assert_Input_Delay (timerPinTag & ctag, timerClock * clock, timerTime value) {
		  timerPinTag * cTagN = new timerPinTag (ctag); 
		  cTagN->setArrivalTag (); 
		  assert_pin_tag (cTagN);
		  cTagN->annotatePinArrival (clock, timerEarly, timerRise, value);
		  //printf ("InputDelay : %s %f\n", thePinName.c_str (), value);
		}	

		void assert_Output_Delay (timerPinTag & ctag, timerClock * clock, timerTime value) {
		  timerPinTag * cTagN = new timerPinTag (ctag);
		  cTagN->setRequiredTag (); 
		  assert_other_pin_tag (cTagN);
		  cTagN->annotatePinArrival (clock, timerEarly, timerRise, value);
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
