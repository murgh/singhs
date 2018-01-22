#include "timerUtils.hxx"
#include "timerConstraints.hxx"
#include "timer.hxx"
#include "timerDelay.hxx"
#include "timerLibData.hxx"
#include <map>

#ifndef timerPin_H
#define timerPin_H

#define UDEF_PinTagGroupId -1

class timerPinTime;
class timerPinInfo;
class timerPinTag;
class timerPinTagContainer;
class timerPointTime;
class timerPinDelay;
class timerPinDelayContainer;

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

			Iterator (timerPinTagContainer * cont); 

			~Iterator () {
			  theTagSet.clear ();
			  theTagList.clear ();
			  theIterSize = 0;
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
		  theRootId = theTagId;
		  theIsDormant = false;
		  theTagTreeId = -1;
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
		  theIsDormant = tag.theIsDormant;
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

		void setMasterTag (timerPinTag * tag) 
		{ 
			theMasterTag = tag;
		        theRootId = theMasterTag->theRootId;	
		}
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

		static bool checkDormancyLoop (timerPinTag * tag1, timerPinTag * tag2) {
		  //If one of the tags is dormant and other tag is not
		  //If the non-dormant tag comes in the master list of the dormant tag
		  //they cannot be in union
		  timerPinTag * temp = NULL;
		  if (tag1->isDormant () && !tag2->isDormant ()) {
		    temp = tag1;
		    while ( (temp = temp->theMasterTag) )
		      if (temp == tag2)
		  	return true;	      
		  }
		  if (tag2->isDormant () && !tag1->isDormant ()) {
		    temp = tag2;
		    while ( (temp = temp->theMasterTag) )
		      if (temp == tag1)
		  	return true;	      
		  }
		  return false;
		}

		static bool areTagsInUnion (timerPinTag * tag1, timerPinTag * tag2) {
		  if (!tag1 || !tag2) return false;
		  timerPinTag * tag1MasterTo = tag1;
		  timerPinTag * tag2MasterTo = tag2;
		  bool areInUnion = false;
		  if (checkDormancyLoop (tag1, tag2)) return areInUnion;
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
		  if (to_merge == this) return;
		  timerPinTag * temp = to_merge->theMasterTag;
		  while ( temp ) {
		    if (temp == this) {
			to_merge->setDormant ();
			break;
		    }
		    temp = temp->theMasterTag;
		  }

		  if (!theTagContainer) {
		    theTagContainer = new timerPinTagContainer;	  
		  }	  
		  theTagContainer->addTag (to_merge); 
		  to_merge->setMergeToTag (this);
		  //to_merge->incrementForwardMergeCount (mergePin);
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

		int getTagId () const { return theTagId; }

		void setRootId (int id) { theRootId = id; }
		int getRootId () const { return theRootId; }

		void setDormant () { theIsDormant = true; }
		bool isDormant () { return theIsDormant; }

		void setTreeId (int id) { theTagTreeId = id; }
		int getTreeId () { return theTagTreeId; }

	private:
		//Not taking the polarity in consideration for now
		//bool	     thePositivePolarity;
		bool		theArrival; //Arrival tag, if false it acts as required tag
		bool 	     	theClockPath;
		int	     	theSourceId;
		int		theTagId;
		int		theRootId;
		int 		theForwardMergeCount;
		bool		theIsDormant;
		diganaVertex	theTimingPropagationPoint;
		timerPinTag   * theMasterTag; //For Split Tags
		timerPinTag   * theMergeToTag; //Tag to which this tag is merged
		timerPinTagContainer * theTagContainer;//Union set of tags.
		timerPinTime  * thePinTime;//The pin time info
		int		theMergeLevel;//Merge level		
		static int	theTagCount;
		int		theTagTreeId;
};

class timerPinTagTreeNode {
  public:
    timerPinTagTreeNode (timerPinTag * tag = NULL)
    {
       theTag = tag;
       theHeight = 0;
       theNextSet.clear ();
    }

    timerPinTag * theTag;
    int theHeight;
    std::set<timerPinTagTreeNode *> theNextSet;
};

class timerPinTagTree {
  public:
	  timerPinTagTree (timerPinTag * endTag)
	  {
	    theRoot = new timerPinTagTreeNode ();	
	    buildTagTree (theRoot, endTag);
	    propagateAndComputeWeight ();
	    theTagTreeMap.clear ();
	  }
          
	  timerPinTagTreeNode* insert (timerPinTagTreeNode * master, timerPinTag * tag);

	  void buildTagTree (timerPinTagTreeNode * master, timerPinTag * endTag);

	  void propagateAndComputeWeight ();

	  void propagateTreeId (std::set<int> & idSet, int treeId);

	  int getWeight () { return theWeight; }
  private:
	  int theWeight;
	  timerPinTagTreeNode * theRoot;
	  std::map<timerPinTag *, timerPinTagTreeNode *> theTagTreeMap;
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
			theDelayCalcContainer = NULL;
			theSourceVertices.clear ();
			theTagTree = NULL;
			clearRepConeMarking ();
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
			theDelayCalcContainer = NULL;
			theTagTree = NULL;
			theSourceVertices.clear ();
			clearRepConeMarking ();
		}

		void addSourceVertex (int src) { theSourceVertices.insert (src); }

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

		bool getIsLatchClock () { return (theIdentity == timerLatchClock); }
		bool getIsLatchDataIn () { return (theIdentity == timerLatchData && theDirection == timerInput); }
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

		void print (int i = 0) {
			if (i == 0) return;
		  printf ("Pin(%s) ", thePinName.c_str ());
		  printf ("isClock(%d) isData(%d) ", theIsClock, theIsData); 
		  printf ("type(%s) ", get_identifier_name ().c_str ()); 
		  printf ("dir(%s) cap(%f) ", get_direction ().c_str (), getCap ()); 
		  if (thePinTag) printf (" %s ", thePinTag->isDormant () ? "dormant" : "non_dormant");
		  if (theIsSplitPoint) printf ("splitPoint ");
		  if (thePinTag) thePinTag->print (std::string ("TAG"));
		  if (thePinTag && thePinTag->getMasterTag ()) thePinTag->getMasterTag ()->print (std::string ("M_Tag"));
		  if (thePinTag && thePinTag->getMergeToTag ()) thePinTag->getMergeToTag ()->print (std::string ("F_Tag"));
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

		void clearDFSDone () { theDFSDone = false; }
		void setDFSDone () { theDFSDone = true; }
		bool isDFSDone () { return theDFSDone; }

		void setReferencePin (diganaVertex ref) { theReferencePin = ref; }
		diganaVertex gerReferencePin () { return theReferencePin; }

		timerCap getCap () {
		  if (theDirection != timerOutput)
		    return -1;

		  if (theIdentity == timerIOPort) {
		    return .4;//Standard output load for a while 
		  } else {
		    return theLibPin->getCap ();
		  }	  
		}

		void cleanDCInfo (); 
		void addDCInfo (/*timerClock * clock,*/timerPinDelay * pinDelay); 
		timerPinDelayContainer * getDCInfo () { return theDelayCalcContainer; }
		void printDCInfo (); 

		std::set<int> & getSourceVertexSet () { return theSourceVertices; }

		void setInRepCone () { theIsInReportCone = true; }
		bool isInRepCone () { return theIsInReportCone; }
		void clearRepConeMarking () { theIsInReportCone = false; }

		void setInPath () { theIsInPath = true; }
		bool isInPath () { return theIsInPath; }
		void clearRepPathMarking () { theIsInPath = false; }

		void initEndTagTree (timerPinTag * endTag) { theTagTree = new timerPinTagTree (endTag); } 
		timerPinTagTree * getEndTagTree () { return theTagTree; }

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
		bool		theDFSDone;
		diganaVertex		theReferencePin;
		timerPinDelayContainer * theDelayCalcContainer;
		std::set<int>	theSourceVertices;
		bool		theIsInReportCone;
		bool		theIsInPath;
		timerPinTagTree * theTagTree;

		void assert_Input_Delay (timerPinTag & ctag, timerClock * clock, timerTime value) {
		  timerPinTag * cTagN = new timerPinTag (ctag); 
		  cTagN->setArrivalTag (); 
		  assert_pin_tag (cTagN);
		  for (int el = timerEarly; el != timerAnalysis; ++el) {
		    for (int rf = timerRise; rf != timerTrans; ++rf) {
		      cTagN->annotatePinArrival (clock, ((timerAnalysisType)el), ((timerTransition)rf), value);
		      cTagN->annotatePinTransition (clock, ((timerAnalysisType)el), ((timerTransition)rf), .004);
		    }
		  }
		  //printf ("InputDelay : %s %f\n", thePinName.c_str (), value);
		}	

		void assert_Output_Delay (timerPinTag & ctag, timerClock * clock, timerTime value) {
		  timerPinTag * cTagN = new timerPinTag (ctag);
		  cTagN->setRequiredTag (); 
		  assert_other_pin_tag (cTagN);
		  for (int el = timerEarly; el != timerAnalysis; ++el) {
		    for (int rf = timerRise; rf != timerTrans; ++rf) {
		      cTagN->annotatePinArrival (clock, ((timerAnalysisType)el), ((timerTransition)rf), value);
		    }
		  }
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
