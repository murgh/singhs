#include<vector>
#include<map>
#include<string>
#include "graph.hxx"

#ifndef timerUtils
#define timerUtils

typedef float timerTime;
const timerTime timerUndefDelay = -1;

enum timerAnalysisType{ 
	timerEarly, 
	timerLate, 
	timerAnalysis
     } ;

enum timerTransition{ 
	timerRise, 
	timerFall, 
	timerTrans
     } ;

enum timerArcType{
	timerDelayArc,
	timerCheckArc,
	timerArcNone
     } ;

enum timerDelayArcType{
	timeCellArc,
	timerNetArc,
	timerDelayArcNone
     } ;

enum timerCheckArcType{
	timerSetup,
	timerHold,
	timerCheckNone
     } ;

enum timerPinIdentifier{
	timerIOPort,
	timerComboPin,
	timerLatchData,
	timerLatchClock,
	timerPinIDNone
     };

enum timerPinDirection{
	timerInput,
	timerOutput,
	timerDirNone
     };
class timerClock {
	public:
		int clockId;
		std::string clockName;
		std::vector<float> waveform;
		float period;
};
#endif
