#include "timerUtils.hxx"

#ifndef CONSTR
#define CONSTR
class timerClock {

  public:
	  timerClock (std::string name, int period, bool virtualClock = true) {

	    theVirtual = virtualClock;;
	    std::pair<int, int> waveform = std::pair<int, int> (0, period/2);
	    std::pair<int, std::string> srcInfo = std::pair<int, std::string> (-1, "");
	    init (name, period, waveform, srcInfo); 
	  } 

	  timerClock (std::string name, int period, std::pair<int, std::string> & srcInfo) 
	  { 
	    theVirtual = false;
	    std::pair<int, int> waveform = std::pair<int, int> (0, period/2);
	    init (name, period, waveform, srcInfo); 
	  }

	  timerClock (std::string name, int period, 
		      std::pair<int, int> & waveform,
		      std::pair<int, std::string> & srcInfo) 
	  { 
	    theVirtual = false;
	    init (name, period, waveform, srcInfo); 
	  }

	  std::string getName () { return theName; }
	  std::pair<int, int> & getWaveform () { return theWaveform; }
	  int getPeriod () { return thePeriod; }
	  int getSourceId () { return theVirtual ? -1 : theSourceInfo.first; }
	  std::string getSourceName () { return theVirtual ? std::string ("") : theSourceInfo.second; }

  private:
	  std::string theName;
	  unsigned int theId;
 	  std::pair<int, int> theWaveform;
	  unsigned int thePeriod;	  
	  std::pair<int, std::string> theSourceInfo;
	  bool theVirtual;

          void init (std::string name, 
		     int period,
	             std::pair<int, int> & waveform,
	             std::pair<int, std::string> & srcInfo) {
            static unsigned int ID = 0;
	    theName = name;
	    thePeriod = period;
	    theWaveform = waveform;
	    theSourceInfo = srcInfo;
	    theId = ID++;
	  }
};

class timerConstraints {
	public:
	static std::map<std::string, timerClock *> theClockMap;	

	static timerClock * is_clock_in_clock_map (std::string clockName) {
	  timerClock * clock = NULL;
	  std::map<std::string, timerClock *>::iterator it = theClockMap.find(clockName);
	  if (it != theClockMap.end ()) {
	    clock = it->second;  
	    printf ("WARNING : Clock %s is already defined on pin/port %s\n", 
		     clock->getName ().c_str (), clock->getSourceName ().c_str ());	
	    return clock;
	  } 
	  return clock;
	}

	static void add_clock_in_clock_map (timerClock * clock) {
	  theClockMap.insert (std::pair<std::string, timerClock *> (clock->getName (), clock));
	}

	static void clear_clock_map () {
	  theClockMap.clear ();
	}
};
#endif
