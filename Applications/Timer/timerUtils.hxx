#include <vector>
#include <map>
#include <string>
#include "../../Kernel/graph.hxx"

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
	timeTriggerArc,
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
	timerPinVirtualNode,
	timerPinIDNone
     };

enum timerPinDirection{
	timerInput,
	timerOutput,
	timerInOut,
	timerDirNone
     };

enum timerArcUnateness {
	timerNegUnate,
	timerPosUnate,
	timerNonUnate
};
#endif
