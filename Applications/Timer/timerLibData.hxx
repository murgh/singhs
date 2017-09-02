#include "timerUtils.hxx"
#include "timerDelay.hxx"

#ifndef _DESIGN_
#define _DESIGN_

class timerLibData;
class timerLibCell;
class timerLibPin;
class timerLibArc;
class timerDelayCalcArgs;


//Global cell delays, transition and check arc tables, for now to ease the liberty parsing
class timerLUT {
     public:
       timerLUT (int size) { 
         theLUTSize = size;
	 theIndex1 = new float[size];
	 theIndex2 = new float[size];
         theLUT = new float*[size];
	 for (int i = 0; i < size; i++) {
	   theLUT[i] = new float[size];
	 }
       }

       void setIndex1 (float * idx) {
	 for (int i = 0; i < theLUTSize; i++)
	   theIndex1[i] = idx[i];
       }

       void setIndex2 (float * idx) {
	 for (int i = 0; i < theLUTSize; i++)
	   theIndex2[i] = idx[i];
       }

       void setLUT (int idx1, float * idx) {
	 for (int i = 0; i < theLUTSize; i++)
	   theLUT [idx1][i] = idx[i];
       }

       int getSize () { return theLUTSize; }

       //Get the delay or transition
       timerTime getLUTDelay (timerTime inSlew, timerTime outLoadorSlew) {
	 return 1.0;
       }

     private:	
       int theLUTSize;
       float * theIndex1;
       float * theIndex2;	       
       float ** theLUT; 
};

class timerArcLUT {

  public:
    timerArcLUT (timerArcType arcType, int size) {
      theDelayLUT[0] = theDelayLUT[1] = NULL;
      theTransition[0] = theTransition[1] = NULL;
      if (arcType == timerDelayArc || arcType == timeTriggerArc) {
	theDelayLUT[0] = new timerLUT (size);
	theDelayLUT[1] = new timerLUT (size);
	theTransition[0] = new timerLUT (size);
	theTransition[1] = new timerLUT (size);
      }	
      if (arcType == timerCheckArc) {
	theDelayLUT[0] = new timerLUT (size);
	theDelayLUT[1] = new timerLUT (size);
      }      
    }

    void populateDelayLUT (timerTransition tran, timerTime ** LUT) {
      for (int i = 0; i < theDelayLUT[tran]->getSize (); i++)
        theDelayLUT[tran]->setLUT (i, LUT[i]);	
    }

    void populateTransitionLUT (timerTransition tran, timerTime ** LUT) {
      if (!theTransition[tran])
        return;
      for (int i = 0; i < theTransition[tran]->getSize (); i++)
        theTransition[tran]->setLUT (i, LUT[i]);	
    }

    timerLUT * getDelayLUT (timerTransition tran) { return theDelayLUT[tran]; }
    timerLUT * getTranLUT (timerTransition tran) { return theTransition[tran]; }

  private:
    timerLUT * theDelayLUT[timerTrans];
    timerLUT * theTransition[timerTrans];
};

class timerLibPin {

	public:
		timerLibPin (std::string name) {
			theName = name;
			theDir = timerDirNone;
			thePinCap = -1;
			theFanin = -1;
			theFanout = -1;
			theIsClock = false;
		}

		//Set APIs
		void setCap (float c) { thePinCap = c; }
	   	void setFanin (int fin) { theFanin = fin; }
		void setFanout (int fout) { theFanout = fout; }	
		void setDirection (timerPinDirection dir) { theDir = dir; }
		void setDirection (std::string dir) {
			if (dir == "input")
			  setDirection (timerInput);
			else if (dir == "output")
			  setDirection (timerOutput);
			else if (dir == "inout")
			  setDirection (timerInOut);
			else
			  setDirection (timerDirNone);
		}
		void setIsClock () { theIsClock = true;}
		bool getIsClock () { return theIsClock; }

		void setPinAttr (std::string attribute, void * val) {
			if (attribute == "direction")
		          setDirection (std::string (reinterpret_cast<char *> (val)));
			else if (attribute == "capacitance")
			  setCap (*(reinterpret_cast<float*> (val)));
			else if (attribute == "fanin")
			  setFanin (*(reinterpret_cast<int*> (val)));
			else if (attribute == "fanout")
			  setFanout (*(reinterpret_cast<int*> (val)));
		}

		//Get APIs
		float getCap () { return thePinCap; }
		int getFanin () { return theFanin; }
		int getFanout () { return theFanout; }
		timerPinDirection getDirection () { return theDir; }

		std::string getName () { return theName; }

	private:
		std::string theName;
		timerPinDirection theDir;
		float thePinCap;
		int theFanin;
		int theFanout;
		bool theIsClock;
};

class timerLibArc {

	public:
		timerLibArc () { 
		  theLUT = NULL; 
		  theArcType = timerDelayArc;
		}

		timerLibArc (timerLibPin * src, timerLibPin * snk) { 
		  theSource = src;
		  theSink = snk;
		  theLUT = NULL;
		  theArcType = timerDelayArc;
		}

		bool operator == (const timerLibArc & otherArc) {
			if (theSource == otherArc.theSource &&
			    theSink == otherArc.theSink)
				return true;
			return false;
		}

		//Set APIs
		void setSource (timerLibPin * s) { theSource = s; }
		void setSink (timerLibPin * s) { theSink = s; }
		void setUnateness (timerArcUnateness u) { 
		  theUnateness = u; 
		  populateLUT ();
		}
		void setArcType (std::string at) { 
		  if (
		      at == std::string ("setup_rising") ||
		      at == std::string ("setup_falling") ||
		      at == std::string ("hold_rising") ||
		      at == std::string ("hold_falling") 
		     )
			theArcType = timerCheckArc;  
		  if (
		      at == std::string ("rising_edge") ||
		      at == std::string ("falling_edge")
		     )
			theArcType = timeTriggerArc;  

		  setTimingType (at);
		  populateLUT ();
		}

		void setTimingType (std::string at) {
		    theTimingType = timerArcTypeNone;
		    if (at == std::string ("setup_rising")) { theTimingType = timerSetupRising; return; }
		    if (at == std::string ("setup_falling")) { theTimingType = timerSetupFalling; return; }
		    if (at == std::string ("hold_rising")) { theTimingType = timerHoldRising; return; }
		    if (at == std::string ("hold_falling")) { theTimingType = timerHoldFalling; return; }
		    if (at == std::string ("rising_edge")) { theTimingType = timerRisingEdge; return; }
		    if (at == std::string ("falling_edge")) { theTimingType = timerFallingEdge; return; }
		}

		timerArcTimingType getTimingType () { return theTimingType; }

		//Get APIs
		timerLibPin * getSource () { return theSource; }
		timerLibPin * getSink () { return theSink; }
		timerArcUnateness getUnateness () { return theUnateness; }
		bool isCombinational () { return (theArcType == timerDelayArc); }
		bool isCheck () { return (theArcType == timerCheckArc); }
		bool isTrigger () { return (theArcType == timeTriggerArc); }

		void populateLUT ();

		timerLUT * getDelayLUT (timerTransition tran) { return theLUT->getDelayLUT (tran); } 
		timerLUT * getTranLUT (timerTransition tran) { return theLUT->getTranLUT (tran); } 

	private:
		timerLibPin * theSource;
		timerLibPin * theSink;
		timerArcUnateness theUnateness;
	        timerArcType theArcType; 	
		timerArcLUT * theLUT; 
		timerArcTimingType theTimingType;

};

class timerLibCell {
	public:
		timerLibCell (std::string name) { 
			theName = name;
			theTimingArcList.clear ();
			thePinList.clear ();
		}

		timerLibPin * add_or_get_pin (std::string pinName) {
			std::list<timerLibPin *>::iterator it = thePinList.begin ();
			for (; it != thePinList.end (); ++it)
				if ((*it)->getName () == pinName)
					return *it;
			timerLibPin * pin = new timerLibPin (pinName);	
			thePinList.push_back (pin);
			return pin;
		}

		timerLibArc * add_or_get_timing_arc  (std::string source, std::string sink) {
			timerLibPin * srcPin = add_or_get_pin (source);
			timerLibPin * sinkPin = add_or_get_pin (sink);
			timerLibArc arc (srcPin, sinkPin);
			std::list<timerLibArc *>::iterator it = theTimingArcList.begin ();
			for (; it != theTimingArcList.end (); ++it)
				if (*(*it) == arc)
					return *it;
			timerLibArc * newArc = new timerLibArc (srcPin, sinkPin);	
			theTimingArcList.push_back (newArc);
			return newArc;
		}

		timerLibArc * get_timing_arc (std::string source, std::string sink) {
			timerLibPin * srcPin = add_or_get_pin (source);
			timerLibPin * sinkPin = add_or_get_pin (sink);
			timerLibArc arc (srcPin, sinkPin);
			std::list<timerLibArc *>::iterator it = theTimingArcList.begin ();
			for (; it != theTimingArcList.end (); ++it)
				if (*(*it) == arc)
					return *it;
			return NULL;

		}

		std::list<timerLibArc *> & get_timing_arc_list () {
			return theTimingArcList;
		}

		std::string getName () { return theName; }

		timerPinIdentifier get_pin_type (timerLibCell * cell, char * pinName) {
			std::list<timerLibPin *>::iterator itr;
			itr = thePinList.begin ();
			timerLibPin * pin = NULL;
			for (; itr != thePinList.end (); ++itr) {
				pin = *itr;
				if (strcmp (pinName, pin->getName ().c_str ())	== 0)
					break;
				pin = NULL;
			}
			if (pin == NULL)
			  return timerPinIDNone;
			if (pin->getIsClock ())
			  return timerLatchClock; 	
			bool isClockSource = false;
			std::list<timerLibArc *>::iterator arcIter;
			arcIter = theTimingArcList.begin ();
			for (; arcIter != theTimingArcList.end (); ++arcIter) {
				timerLibArc * arc = *arcIter;
				if (arc->getSink () == pin) {
					if (arc->getSource ()->getIsClock ()) {
						isClockSource = true;
					}
				}
			}
			if (isClockSource) {
				return timerLatchData;
			} else {
				return timerComboPin;
			}
		}

	private:
		std::string theName;
		std::list<timerLibPin *> thePinList; 
		std::list<timerLibArc *> theTimingArcList;
};

class timerLibData {

	public:
		timerLibData (std::string name) {
			theLibName = name;
		}

		timerLibCell * add_cell (std::string name) {
			std::map <std::string, timerLibCell *>::iterator itr;
			itr = theNameCellMap.find (name);
			if (itr != theNameCellMap.end ())
				return (*itr).second;
			timerLibCell * cell = new timerLibCell (name);
			theNameCellMap.insert (
			  	std::pair<std::string, timerLibCell *> (name, cell));
			return cell;
		}

		timerLibCell * getCell (std::string name) {
			std::map <std::string, timerLibCell *>::iterator itr;
			itr = theNameCellMap.find (name);
			if (itr != theNameCellMap.end ())
			       return (*itr).second; 
			return NULL;	
		}

		timerLibCell * add_or_get_cell (std::string name) {
			return (getCell (name) ? getCell (name) : add_cell (name));
		}

		char * getLibName () { return const_cast<char *> (theLibName.c_str ()); }

	private:
		std::string theLibName;
		std::map <std::string, timerLibCell *> theNameCellMap;
};

#endif
