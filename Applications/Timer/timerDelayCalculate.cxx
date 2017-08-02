#include "timer.hxx"

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

void timerLibArc::ComputeDelay (timerDelayCalcArgs & args) {
	args.theStageDelay = 0.0;
}	

void timerLibArc::ComputeTransition (timerDelayCalcArgs & args) {
	args.theStageDelay = 0.0;
}	

void timerLibArc::ComputeCheck (timerDelayCalcArgs & args) {
	args.theStageDelay = 0.0;
}	

