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

class UF    {
    int *id, cnt, *sz;
public:
	// Create an empty union find data structure with N isolated sets.
    UF(int N)   {
        cnt = N;
	id = new int[N];
	sz = new int[N];
        for(int i=0; i<N; i++)	{
            id[i] = i;
	    sz[i] = 1;
	}
    }
    ~UF()	{
	delete [] id;
	delete [] sz;
    }
	// Return the id of component corresponding to object p.
    int find(int p)	{
        int root = p;
        while (root != id[root])
            root = id[root];
        while (p != root) {
            int newp = id[p];
            id[p] = root;
            p = newp;
        }
        return root;
    }
	// Replace sets containing x and y with their union.
    void merge(int x, int y)	{
        int i = find(x);
        int j = find(y);
        if (i == j) return;
		
		// make smaller root point to larger one
        if   (sz[i] < sz[j])	{ 
		id[i] = j; 
		sz[j] += sz[i]; 
	} else	{ 
		id[j] = i; 
		sz[i] += sz[j]; 
	}
        cnt--;
    }
	// Are objects x and y in the same set?
    bool connected(int x, int y)    {
        return find(x) == find(y);
    }
	// Return the number of disjoint sets.
    int count() {
        return cnt;
    }
};
#endif
