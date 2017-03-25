#include "timerUtils.hxx"

#ifndef _DESIGN_
#define _DESIGN_

class timerLibData;
class timerLibCell;
class timerLibPin;
class timerLibArc;

class timerLibPin {

	public:
		timerLibPin (std::string name) {
			theName = name;
			theDir = timerDirNone;
			thePinCap = -1;
			theFanin = -1;
			theFanout = -1;
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
};

class timerLibArc {

	public:
		timerLibArc () { }

		timerLibArc (timerLibPin * src, timerLibPin * snk) { 
			theSource = src;
			theSink = snk;
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
		void setUnateness (timerArcUnateness u) { theUnateness = u; }
		void setArcType (std::string at) { theArcType = at; }

		//Get APIs
		timerLibPin * getSource () { return theSource; }
		timerLibPin * getSink () { return theSink; }
		timerArcUnateness getUnateness () { return theUnateness; }
		bool isCombinational () { return false; }
		bool isCheck () { return false; }
		bool isTrigger () { return false; }

	private:
		timerLibPin * theSource;
		timerLibPin * theSink;
		timerArcUnateness theUnateness;
	        std::string theArcType; 	

};

class timerLibCell {
	public:
		timerLibCell (std::string name) { 
			theName = name;
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

		std::string getName () { return theName; }

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
