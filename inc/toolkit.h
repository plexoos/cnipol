#ifndef cnipol_toolkit_h
#define cnipol_toolkit_h

#include "rhicpol.h"
#include "rpoldata.h"

#include "AsymGlobals.h"

namespace cnipol_toolkit {

// Description : calculate kinematics from ADC and TDC
// Input       : int Mode, processEvent *event, recordConfigRhicStruct *cfginfo, int st
// Return      : float &edepo, float &e, float &t, float &delt, float &Mass
void KinemaReconstruction(int Mode, processEvent *event, recordConfigRhicStruct *cfginfo, 
			 int st, float &edepo, float &e, float &t, float &delt, float &Mass);

// Description : output routine for spin tune measurements upon request by Mei Bai
// Input       : int bid, double si
void SpinTuneOutput(int bid, double si);

}

#endif
