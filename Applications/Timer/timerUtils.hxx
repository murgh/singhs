#include <vector>
#include <map>
#include <string>
#include "../../Kernel/graph.hxx"

#ifndef timerUtils
#define timerUtils


long get_nanos(void); 
void process_mem_usage(double& vm_usage, double& resident_set);


typedef float timerTime;
typedef float timerCap;
const timerTime timerUndefDelay = -1;
const timerTime timerUndefTran = -1;

enum timerAnalysisType{ 
	timerEarly = 0, 
	timerLate, 
	timerAnalysis
     } ;

enum timerTransition{ 
	timerRise = 0, 
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

enum timerArcTimingType {
	timerSetupRising,
	timerSetupFalling,
	timerHoldRising,
	timerHoldFalling,
	timerRisingEdge,
	timerFallingEdge,
	timerArcTypeNone
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
