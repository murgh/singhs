#include "timer.hxx"
#include "timerPin.hxx"

#define timerLUTSIZE 7

static float ** getTheLookUpTable () {
  float ** LUT = new float * [timerLUTSIZE];
  for (int i = 0; i < timerLUTSIZE; i++) {
    LUT[i] = new float [timerLUTSIZE];
  }  
  return LUT;
}

static void setLUTVAL (float ** LUT, float Master[timerLUTSIZE][timerLUTSIZE]) {
  for (int i = 0; i < timerLUTSIZE; i++)
   for (int j = 0; j < timerLUTSIZE; j++)
	   LUT[i][j] = Master[i][j];
}

float theDelayIdxSlew[timerLUTSIZE] = {0.000932129,0.00354597,0.0127211,0.0302424,0.0575396,0.0958408,0.146240};
float theDelayIdxLoad[timerLUTSIZE] = {1.143777657,1.505078657,2.223411657,3.651611657,6.491141657,12.13666166,23.36116166};
float theDelayLUTM[timerLUTSIZE][timerLUTSIZE] = 
{
	{0.069796,0.072514,0.075300,0.080644,0.090981,0.111430,0.152431},
	{0.064617,0.066822,0.068993,0.073062,0.080667,0.095404,0.124879},
	{0.067222,0.069424,0.071595,0.075667,0.083272,0.098009,0.127491},
	{0.069668,0.071872,0.074044,0.078115,0.085722,0.100455,0.129937},
	{0.071063,0.073265,0.075436,0.079505,0.087109,0.101842,0.131321},
	{0.070972,0.073175,0.075343,0.079413,0.087012,0.101752,0.131220},
	{0.069007,0.071216,0.073390,0.077447,0.085046,0.099773,0.129252},
};

float theTransIdxSlew[timerLUTSIZE] = {0.000932129,0.00354597,0.0127211,0.0302424,0.0575396,0.0958408,0.146240};
float theTransIdxLoad[timerLUTSIZE] = {1.143777657,1.505078657,2.223411657,3.651611657,6.491141657,12.13666166,23.36116166};
float theTransitionLUTM[timerLUTSIZE][timerLUTSIZE] = 
{
	{0.003993,0.005243,0.006537,0.009192,0.014867,0.027162,0.052670},
	{0.003993,0.005246,0.006539,0.009190,0.014869,0.027162,0.052661},
	{0.003989,0.005242,0.006539,0.009190,0.014868,0.027163,0.052667},
	{0.003991,0.005244,0.006540,0.009189,0.014870,0.027166,0.052664},
	{0.003986,0.005243,0.006534,0.009188,0.014870,0.027164,0.052668},
	{0.003993,0.005250,0.006540,0.009194,0.014869,0.027168,0.052652},
	{0.003996,0.005249,0.006545,0.009193,0.014870,0.027163,0.052670},
};

float theCheckIdxSigSlew[timerLUTSIZE] = {0.000932129,0.0331496,0.146240,0.288450,0.598760,1.134565,1.56734};
float theCheckIdxRefSlew[timerLUTSIZE] = {0.000932129,0.0331496,0.146240,0.288450,0.598760,1.134565,1.56734};
float theCheckLUTM[timerLUTSIZE][timerLUTSIZE] = 
{
	{0.003993,0.005243,0.006537,0.009192,0.014867,0.027162,0.052670},
	{0.064617,0.066822,0.068993,0.073062,0.080667,0.095404,0.124879},
	{0.003989,0.005242,0.006539,0.009190,0.014868,0.027163,0.052667},
	{0.003991,0.005244,0.006540,0.009189,0.014870,0.027166,0.052664},
	{0.003986,0.005243,0.006534,0.009188,0.014870,0.027164,0.052668},
	{0.003993,0.005250,0.006540,0.009194,0.014869,0.027168,0.052652},
	{0.003996,0.005249,0.006545,0.009193,0.014870,0.027163,0.052670},
};

float ** theDelayLUT = NULL, ** theTransitionLUT = NULL, ** theCheckLUT = NULL;

static void populateGlobalLUTs () {
  if (theDelayLUT && theTransitionLUT && theCheckLUT) return;
  theDelayLUT = getTheLookUpTable ();
  setLUTVAL (theDelayLUT, theDelayLUTM);
  theTransitionLUT = getTheLookUpTable ();
  setLUTVAL (theTransitionLUT, theTransitionLUTM);
  theCheckLUT = getTheLookUpTable ();
  setLUTVAL (theCheckLUT, theCheckLUTM);
}

timerTime
computeArrival (timerTime first, timerTime second) {
  if (first == timerUndefDelay || second == timerUndefDelay)
    return timerUndefDelay;

  return (first + second);
}

void
timerLibArc::populateLUT () {
  if (theLUT) return;
  populateGlobalLUTs ();
  theLUT = new timerArcLUT (theArcType, timerLUTSIZE); 
  if (theArcType == timerCheckArc) {
    theLUT->populateDelayLUT (timerRise, theCheckLUT);
    theLUT->populateDelayLUT (timerFall, theCheckLUT);
  } else {
    theLUT->populateDelayLUT (timerRise, theDelayLUT);
    theLUT->populateDelayLUT (timerFall, theDelayLUT);
  }
  theLUT->populateTransitionLUT (timerRise, theTransitionLUT);
  theLUT->populateTransitionLUT (timerFall, theTransitionLUT);
}

bool
timerArcInfo::getDelayAndTransition (int el, 
		        	     int srcRF,
		        	     int destRF,
		        	     timerTime sourceTran,
		        	     timerCap stageLoad,
				     timerTime delayAndTrans[2]) {

  delayAndTrans[0] = delayAndTrans[1] = timerUndefDelay;

  if (!theLibArc) {
    delayAndTrans[0] = 0;
    delayAndTrans[1] = sourceTran;
    if (srcRF == destRF)
      return true;
    else
      return false;
  }

  timerLUT * delayLUT = theLibArc->getDelayLUT ( ((timerTransition) destRF) );
  timerLUT * tranLUT = theLibArc->getTranLUT ( ((timerTransition) destRF) );
  timerArcUnateness unateness = theLibArc->getUnateness ();
  timerArcTimingType timingType = theLibArc->getTimingType ();
  if (theLibArc->isCombinational () || theLibArc->isTrigger ()) { 
    if (unateness == timerPosUnate) {
      if (srcRF != destRF)
	return false;
    } 
    
    if (unateness == timerNegUnate) {
      if (srcRF == destRF)
	return false;
    }

    if ( (timingType == timerRisingEdge && ((timerTransition) srcRF) == timerFall) ||
	 (timingType == timerFallingEdge && ((timerTransition) srcRF) == timerRise) ) {
      return false;	
    }

    delayAndTrans[0] =  delayLUT->getLUTDelay (sourceTran, ((timerTime) stageLoad));
    delayAndTrans[1] =  tranLUT->getLUTDelay (sourceTran, ((timerTime) stageLoad));
    return true;
  }

}

void
timerArcInfo::getCheckValue (int el,
			     int sigRF,
			     int refRF,
			     timerTime sigTran,
			     timerTime refTran,
			     timerTime & check) {
  if (!theLibArc) {
    check = timerUndefDelay;
    return;
  }

  timerLUT * delayLUT = theLibArc->getDelayLUT ( ((timerTransition) sigRF) );
  check =  delayLUT->getLUTDelay (sigTran, refTran);
}

/*
void timerArcInfo::ComputeAndAnnotateDelay (timerDelayCalcArgs & args) {
  timerPinTag * sourceTag = args.theSourceTag;
  timerPinTag * sinkTag = args.theSinkTag;
  diganaVertex source = args.theSource;
  diganaVertex sink = args.theSink;
  diganaEdge edge (source, sink);
  timerCap stageLoad = args.theStageLoad;
  timerTime sourceTran, sourceArr, arcDelayAndTran[2];
	
  //Foreach clock info do iteration
  std::list<timerClock *> tagClocks;
  sourceTag->populateClockObjects (tagClocks);	
  std::list<timerClock *>::iterator itr;
  for (itr = tagClocks.begin (); itr != tagClocks.end (); ++itr) {
    timerClock * clock = *itr;
    for (int el = timerEarly; el != timerAnalysis; ++el) {
      for (int srcRF = timerRise; srcRF != timerTrans; ++srcRF) {
        for (int destRF = timerRise; destRF != timerTrans; ++destRF) {
          sourceTran = sourceTag->getTransition (clock, el, srcRF);
          sourceArr = sourceTag->getArrival (clock, el, srcRF);
          if (sourceTran == timerUndefTran) 
            continue;
          getDelayAndTransition (el, srcRF, destRF, sourceTran, stageLoad, arcDelayAndTran);
          sinkTag->annotatePinArrival (clock, ((timerAnalysisType) el), ((timerTransition) destRF), computeArrival (arcDelayAndTran[0], sourceArr)); 
          sinkTag->annotatePinTransition (clock, ((timerAnalysisType) el), ((timerTransition) destRF), arcDelayAndTran[1]);
        }
      }
    }
    printf ("DC : pin(%s) clock(%s) er(%f) ef(%f) lr(%f) lf(%f)\n", TA_Timer::getPinInfo (args.theSink)->getName ().c_str (), clock->getName ().c_str (), 
							       sinkTag->getArrival (clock, 0, 0),//rise,rise  
							       sinkTag->getArrival (clock, 0, 1),//rise,fall  
							       sinkTag->getArrival (clock, 1, 0),//fall,rise  
							       sinkTag->getArrival (clock, 1, 0));//fall,fall
  }
}
*/

void timerArcInfo::ComputeAndAnnotateDelay (timerDelayCalcArgs & args) {
  timerPinTag * sourceTag = args.theSourceTag;
  timerPinTag * sinkTag = args.theSinkTag;
  diganaVertex source = args.theSource;
  diganaVertex sink = args.theSink;
  diganaEdge edge (source, sink);
  timerCap stageLoad = args.theStageLoad;
  timerTime sourceTran, sourceArr, arcDelayAndTran[2];
  timerPinDelayContainer::Iterator itr (TA_Timer::getPinInfo (args.theSource)->getDCInfo ());
  timerPinDelay * delay, * sinkDelay;

  //Iterate on the source delay object and compute for this edge
  while ( (delay = itr.next ()) ) {
    for (int destRF = timerRise; destRF != timerTrans; ++destRF) {
       int srcRF = (int) (delay->getTranType ());
       sourceTran = delay->getTran ();
       if ( getDelayAndTransition (args.theEL, srcRF, destRF, sourceTran, stageLoad, arcDelayAndTran) ) {
         sinkDelay = new timerPinDelay (*delay); 
         sinkDelay->setDelay (computeArrival (delay->getDelay () , arcDelayAndTran[0]) );
         sinkDelay->setTran (arcDelayAndTran[1]);
	 sinkDelay->setTranType (((timerTransition) destRF));
         TA_Timer::getPinInfo (args.theSink)->addDCInfo (sinkDelay);
       }
    }
  }
}

void computeCheckValue (timerDelayCalcArgs & args) {
  diganaEdge edge (args.theSource, args.theSink);

}

//Based on the 
void computeEdgeDelayAndPropagateArrival (timerDelayCalcArgs & args) {
  diganaEdge edge (args.theSource, args.theSink);
  if (args.theFirstStage) {
    //Build the delay container on the first stage source pin
    std::list<timerClock *>::iterator itr;
    std::list<timerClock *> tagClocks;
    args.theSourceTag->populateClockObjects (tagClocks);	
    for (itr = tagClocks.begin (); itr != tagClocks.end (); ++itr) {
      timerClock * clock = *itr;
      timerTime riseDel, fallDel, riseTran, fallTran;
      for (int srcRF = 0; srcRF != timerTrans; ++srcRF) {
        timerPinDelay * delayObj = new timerPinDelay;
        delayObj->setDelay (args.theSourceTag->getArrival (clock, args.theEL, ((timerTransition) srcRF)));
        delayObj->setTran (args.theSourceTag->getTransition (clock, args.theEL, ((timerTransition) srcRF)));
	delayObj->setTranType (((timerTransition) srcRF));
        delayObj->setClock (clock);
        TA_Timer::getPinInfo (args.theSource)->addDCInfo (delayObj);
      }
    }
  }
  TA_Timer::getArcInfo (edge)->ComputeAndAnnotateDelay (args); //Call in a loop
  TA_Timer::getPinInfo (args.theSource)->cleanDCInfo ();
}

void timerPinTime::annotateArrival (timerClock * clock, 
				    timerAnalysisType el,
				    timerTransition rf,
				    timerTime del) {
  std::map<timerClock *, std::pair <timerPointTime *, timerPointTime *> >::iterator itr = theClockTimeMap.find (clock);
  timerPointTime * delay = NULL; 
  if (itr != theClockTimeMap.end ()) {
    std::pair <timerPointTime *, timerPointTime *> & delTranPair = itr->second;
    delay = delTranPair.first;
    if (!delay) delay = new timerPointTime ();
    delTranPair.first = delay;
  } else {
    //Pair is not yet created
    std::pair <timerPointTime *, timerPointTime *> delTranPair (NULL, NULL);
    delay = new timerPointTime ();
    delTranPair.first = delay;
    theClockTimeMap.insert (std::pair <timerClock *, std::pair <timerPointTime *, timerPointTime *> > (clock, delTranPair));	     
  }
  delay->annotate (el, rf, del);
}

void timerPinTime::annotateTransition (timerClock * clock,
				       timerAnalysisType el,
				       timerTransition rf,
				       timerTime tran) {
  std::map<timerClock *, std::pair <timerPointTime *, timerPointTime *> >::iterator itr = theClockTimeMap.find (clock);
  timerPointTime * transition = NULL; 
  if (itr != theClockTimeMap.end ()) {
    std::pair <timerPointTime *, timerPointTime *> & delTranPair = itr->second;
    transition = delTranPair.second;
    if (!transition) transition = new timerPointTime ();
    delTranPair.second = transition;
  } else {
    //Pair is not yet created
    std::pair <timerPointTime *, timerPointTime *> delTranPair (NULL, NULL);
    transition = new timerPointTime ();
    delTranPair.second = transition;
    theClockTimeMap.insert (std::pair <timerClock *, std::pair <timerPointTime *, timerPointTime *> > (clock, delTranPair));	     
  }
  transition->annotate (el, rf, tran);
}

void timerPinTime::populateClockObjects (std::list<timerClock *> & clockList) {
  std::map<timerClock *, std::pair <timerPointTime *, timerPointTime *> >::iterator itr;
  for (itr = theClockTimeMap.begin (); itr != theClockTimeMap.end (); ++itr) 
    clockList.push_back (itr->first);
}

timerTime timerPinTime::getArrival (timerClock * clock, int el, int rf) {
  std::map<timerClock *, std::pair <timerPointTime *, timerPointTime *> >::iterator itr = theClockTimeMap.find (clock);
  if (itr == theClockTimeMap.end ())
    return timerUndefDelay;

  std::pair <timerPointTime *, timerPointTime *> & delTranPair = itr->second;
  if (!delTranPair.first) return timerUndefDelay;
  return delTranPair.first->getTime (((timerAnalysisType)el), ((timerTransition)rf)); 
}

timerTime timerPinTime::getTransition (timerClock * clock, int el, int rf) { 
  std::map<timerClock *, std::pair <timerPointTime *, timerPointTime *> >::iterator itr = theClockTimeMap.find (clock);
  if (itr == theClockTimeMap.end ())
    return timerUndefTran;

  std::pair <timerPointTime *, timerPointTime *> & delTranPair = itr->second;
  if (!delTranPair.second) return timerUndefDelay;
  return delTranPair.second->getTime (((timerAnalysisType)el), ((timerTransition)rf)); 
}

void timerPinInfo::cleanDCInfo () { 
  if (theDelayCalcContainer) 
    delete theDelayCalcContainer; 
  theDelayCalcContainer = NULL;
}

void timerPinInfo::addDCInfo (timerPinDelay * pinDelay) {
  if (!theDelayCalcContainer)
    theDelayCalcContainer = new timerPinDelayContainer;

  theDelayCalcContainer->addPinDelay (pinDelay);
}

void timerPinInfo::printDCInfo () {
  if (!theDelayCalcContainer) return;
  
  theDelayCalcContainer->print ();
}

