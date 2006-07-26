// Header file for AsymProcess
// Author   : Itaru Nakagawa
// Creation : 07/26/2006         

#ifndef ASYM_PROCESS_H
#define ASYM_PROCESS_H


//
// Class name  : 
// Method name : event_process
//
// Description : main process event routine. This routine is called event by event basis
// Input       : processEvent *event, recordConfigRhicStruct *cfginfo
// Return      : 0
//
int event_process(processEvent *event, recordConfigRhicStruct *cfginfo);

//
// Class name  : 
// Method name : KinemaReconstruction
//
// Description : calculate kinematics from ADC and TDC
// Input       : int Mode, processEvent *event, recordConfigRhicStruct *cfginfo, int st
// Return      : float &edepo, float &e, float &t, float &delt, float &Mass
//
int KinemaReconstruction(int Mode, processEvent *event, recordConfigRhicStruct *cfginfo, 
			 int st, float &edepo, float &e, float &t, float &delt, float &Mass);


// Class name  : 
// Method name : float ekin(float edep, float dthick)
//
// Description : Function to convert energy deposit (edep) to 12C kinetic 
//             : energy for given deadlayer thickness (dthick)
// Input       : float edep, float dthick
// Return      : Ekin = p0 + p1*Edep + p2*Edep^2 + p3*Edep^3 + p4*Edep^4
//
float ekin(float edep, float dtheck);




#endif /* ASYM_PROCESS_H */