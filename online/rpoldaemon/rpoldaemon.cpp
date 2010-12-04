/********************************************************
 *	RHIC Polarimeter Daemon Program			*
 *	I. Alekseev and D. Svirida			*
 *		2001-2010				*
 * The code runs all the time waiting for the command	*
 * to start measurement. Takes care of two polarimeters *
 * of the same DAQ system. Uses script to run actual    *
 * measurement. The script must take the following      *
 * arguments: polarimeter_name command runId   		*
 ********************************************************/
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <stdarg.h>
#include <cdevRequestObject.h>
#include <cdevCallback.h>
#include <cdevSystem.h>
#include "rhicpol.h"
#include "rcdev.h"

char myName[2][20] = {"Upstream", "Downstream"};
char polCDEVName[4][20] = {"polarimeter.blu1", "polarimeter.blu2", "polarimeter.yel1", "polarimeter.yel2"};
char specCDEVName[2][20] = {"ringSpec.blue", "ringSpec.yellow"};
int  myDev[2][2] = {{0, 3}, {1, 2}};				// polCDEVName for Upstream/Downstream

FILE *LogFile;
char LogFileName[256] = "/dev/stdout";
char ScriptName[256] = "rpolMeasure.sh";
int iVerbose = 0;

pid_t ChildPid = 0;
int MyPolarimeter = -1;		// Upstream/downstream
int CurrentPolarimeter = -1;	// we can run one of two our polarimeters only 
int Status = 0;			// Our error status
int iStop  = 0;			// we should Stop

//	this function removes \n from the standard ctime function output
char * cctime(time_t *itime)
{
    char *str;
    str = ctime(itime);
    str[strlen(str)-1] = '\0';
    return str;
}

//	Set CDEV:statusStringS
int UpdateMessage(int polarim, char * fmt, ...)
{
    cdevData data; 
    char str[80];
    int irc;
    va_list ap;
    
    va_start(ap, fmt);
    vsprintf(str, fmt, ap);
    
    irc = 0;
    cdevDevice & pol = cdevDevice::attachRef(polCDEVName[polarim]);
    data.insert("value", str);
    DEVSEND(pol, "set statusStringS", &data, NULL, LogFile, irc);
    return irc;
}

//	Set CDEV:DataAcquisitionS
int SetState(int polarim, char * state)
{
    cdevData data; 
    int irc;

    irc = 0;
    cdevDevice & pol = cdevDevice::attachRef(polCDEVName[polarim]);
    data.insert("value", state);
    DEVSEND(pol, "set dataAcquisitionS", &data, NULL, LogFile, irc);
    return irc;
}

//	Send our status to CDEV:statusS 
int SetStatus(int polarim) 
{
    cdevData data; 
    int irc;

    irc = 0;
    cdevDevice & pol = cdevDevice::attachRef(polCDEVName[polarim]);
    data.insert("value", Status);
    DEVSEND(pol, "set statusS", &data, NULL, LogFile, irc);
    return irc;
}

//	Start measurement
void StartMeasurement(int polarim, char *cmd) 
{
    int irc;
    cdevData data;
    pid_t pid;
    time_t tm;
    double runId;
    int fillNumber;
    char str[20];

    struct {
	int fill;
	int polarim;
	int run;
    } ID;

    irc = 0;
    // Already started - ignore start if we are already running
    if (CurrentPolarimeter >= 0) return;
    
    CurrentPolarimeter = polarim;	// this polarimeter will be running
    Status = 0;
    SetStatus(polarim);			// clear old errors

    // first get necessary parameters - we need to create run number
    cdevDevice & pol = cdevDevice::attachRef(polCDEVName[polarim]);	// the source of the old run number
    cdevDevice & spec = cdevDevice::attachRef(specCDEVName[polarim/2]);	// the source of the fill number
    
    DEVSEND(pol, "get runIdS", NULL, &data, LogFile, irc);
    data.get("value", &runId);
    DEVSEND(spec, "get fillNumberM", NULL, &data, LogFile, irc);
    data.get("value", &fillNumber);
    if (irc != 0 || fillNumber <= 0) {
	Status |= (STATUS_ERROR | WARN_NOBEAM | ERR_NOADO);
	UpdateMessage(polarim, "Run parameters unaccessible");
	SetStatus(polarim);
	SetState(polarim, "Stop");
	CurrentPolarimeter = -1;
	return;
    }

    // decode runId to ID
    ID.fill = (int) (runId + 0.0001);
    ID.polarim = (int) (10.0*(runId + 0.0001 - ID.fill));
    ID.run = (int) (1000.0*(runId + 0.0001 - ID.fill - 0.1*ID.polarim));

    if (ID.fill == fillNumber) {
	 ID.run++;
    } else {
	 ID.fill = fillNumber;
	 ID.polarim = polarim;
	 ID.run = 1;
    }

    if (toupper(cmd[0]) == 'T') {
	runId = 99999.999;
    } else {
	runId = ID.fill + 0.1*ID.polarim + 0.001*ID.run;
    }
    
    tm=time(NULL);
    fprintf(LogFile,"RHICDAEMON-INFO : %s: Starting %s for %s RunID=%8.3f\n", cctime(&tm), cmd, polCDEVName[polarim], runId);
    fflush(LogFile);
    UpdateMessage(polarim, "Starting...");
//	Start the script to measure polarization
    pid = fork ();
    if (pid == 0) {
    	/* This is the child process.  Execute the shell command. */
        fprintf(LogFile,"RHICDAEMON-INFO : %s %s %8.3f\n", polCDEVName[polarim], cmd, runId);
	fflush(LogFile);
	execl(SHELL, SHELL, ScriptName, polCDEVName[polarim], cmd, str);
        fprintf(LogFile,"RHICDAEMON-ERROR : unable to run script %s\n", ScriptName);
	fflush(LogFile);
	Status |= STATUS_ERROR | ERR_FAILSTART;
	UpdateMessage(polarim, "Start FAILED");
        _exit(EXIT_FAILURE);	// we shell never be here ...
    } else if (pid < 0) {
	/* The fork failed.  Report failure.  */
	Status |= STATUS_ERROR | ERR_FAILSTART;
	fprintf(LogFile,"RHICADO-ERR : Unable to run the measurement script\n");
	fflush(LogFile);
	UpdateMessage(polarim, "Start FAILED");
	SetState(polarim, "Stop");
	SetStatus(polarim);
	CurrentPolarimeter = -1;
	ChildPid = 0;
    } else {
         /* Everything is OK so far, this is the parent process - catch pid and save run number */
	ChildPid = pid;
	data.insert("value", runId);
	DEVSEND(pol, "set runIdS", NULL, &data, LogFile, irc);
	if (irc) {
    	    fprintf(LogFile,"RHICDAEMON-ERROR: %s set runIdS(%8.3f) irc= %d\n", polCDEVName[polarim], runId, irc);
	    fflush(LogFile);
	}
    }
}

//	Stop measurement (with data readout and analysis etc.)
void StopMeasurement(int polarim)
{
    time_t tm;

    if (CurrentPolarimeter != polarim) return;	// Nothing to do
    if (ChildPid) kill(ChildPid, SIGINT);	// Inform the script that it is stopping
    
    UpdateMessage(CurrentPolarimeter,"Stopping...");    
    SetStatus(CurrentPolarimeter);
    
    tm=time(NULL);
    fprintf(LogFile,"RHICDAEMON-INFO : %s: Stop for %s, status: %8.8X\n", cctime(&tm), polCDEVName[CurrentPolarimeter], Status);
    fflush(LogFile);
}

//	Cancel measurement - stop immediately
void CancelMeasurement(int polarim)
{
    time_t tm;

    if (CurrentPolarimeter < 0) {
	SetState(polarim, "Stop");		// return the button to Stop
	return;
    }

    if (ChildPid) kill(ChildPid, SIGTERM);
    ChildPid = 0;    
    
    tm=time(NULL);
    fprintf(LogFile,"RHICDAEMON-INFO : %s: Cancel for %s,\n", cctime(&tm), polCDEVName[CurrentPolarimeter]);
    fflush(LogFile);

    Status |= WARN_CANCELLED;
    SetStatus(CurrentPolarimeter);
    UpdateMessage(CurrentPolarimeter,"Cancelled");
    
    CurrentPolarimeter = -1;
}

/* Asynchronous get handler */
void handleGetAsync(int status, void* arg, cdevRequestObject& req, cdevData& data)
{
    char cmd[256];
    int i;

    cdevDevice& device = req.device();
    //	Find from which polarimeter. We will look for all possible, ignoring here error of getting from other DAQ.
    //	This should never happen...
    for (i=0; i<4; i++) if (strcmp(polCDEVName[i], device.name()) == 0) break;
    if (i == 4) {
	fprintf(LogFile,"RHICDAEMON-WARN : Unexpected source of event %s\n", device.name());
	fflush(LogFile);
	return;
    }
    data.get("value", cmd, sizeof(cmd));	// get the command
//	Only the first letter of the command matters...
    switch (toupper(cmd[0])) {
    case 'S': 	// Stop
	StopMeasurement(i);
	break;
    case 'C': 	// Cancel
	CancelMeasurement(i);
	break;
    default :
	StartMeasurement(i, cmd);
	break;
    }
}

//	exit on signals grecefully
void exit_handle(int sig)
{
    iStop = sig;
}

//	the child ended - the measuremnt is done
void child_handle(int sig)
{
    int pid, status;
    while (1) {
	pid = waitpid(WAIT_ANY, &status, WNOHANG);
	if (pid <= 0) break;
	ChildPid = 0;			// the measurement was over
	if (CurrentPolarimeter > 0) SetState(CurrentPolarimeter, "Stop");
	CurrentPolarimeter = -1;
    }
}

int main(int argc, char** argv)
{
    int i, irc;
    time_t itime;
    
//	process comman line options
    while ((i = getopt(argc, argv, "UDhvl:r:")) != -1) switch(i) {
    case 'U' :
	MyPolarimeter = 0;
	break;
    case 'D' :
	MyPolarimeter = 1;
	break;
    case 'v' : 
	iVerbose++;
	break;
    case 'h' :
    case '?' :
	printf( "\n\n\t\tPolarimeter daemon\n"
	        "Usage: rpoldaemon -U|D [-options]. Possible options are:\n"
                "U/D - Upstream/Downstream DAQ;\n"
		"h,? - print this message and exit;\n"
		"r [filename]: script to run for polarization measurement;\n"
		"l [filename]: log file name;\n"
		"v: more printout (you may use more 'v' to get more). No effect no, may be in future;\n");
	return 0;
    case 'l' :
	strncpy(LogFileName, optarg, sizeof(LogFileName));
	break;
    case 'r' :
	strncpy(ScriptName, optarg, sizeof(ScriptName));
	break;
    }
    if (MyPolarimeter < 0) {
	printf("You must select Upstream or Downstream! Exitting ...\n");
	return 0;
    }
// Create logfile.
    LogFile = fopen(LogFileName, "at");
    if (LogFile == NULL) LogFile = stdout;
//	catch these signals to exit grecefully
    signal(SIGINT, exit_handle);	// normal exit
    signal(SIGHUP, exit_handle);	
    signal(SIGTERM, exit_handle);
    signal(SIGQUIT, exit_handle);
    signal(SIGCHLD, child_handle);	// end of measurement
//	print something
    itime = time(NULL);
    fprintf(LogFile, "RHICDAEMON-INFO : %s: %s Polarimeter daemon started.\n", cctime(&itime), myName[MyPolarimeter]);
    fflush(LogFile);
    
    /* Initialize state */
    if (SetState(myDev[MyPolarimeter][0], "Stop") || SetState(myDev[MyPolarimeter][1], "Stop")) {
	fprintf(LogFile,"RHICDAEMON-FATAL : CDEV not accessible, exiting after 60 s pause...\n");
	fflush(LogFile);
	sleep(60);
	return -2;
    }
    CurrentPolarimeter = -1;
    
    cdevSystem   & defSystem = cdevSystem::defaultSystem();
    cdevCallback cb(handleGetAsync, NULL);
    
    cdevDevice & dev0 = cdevDevice::attachRef(polCDEVName[myDev[MyPolarimeter][0]]);
    dev0.sendCallback("monitorOn dataAcquisitionS", NULL, cb);
    cdevDevice & dev1 = cdevDevice::attachRef(polCDEVName[myDev[MyPolarimeter][1]]);
    dev1.sendCallback("monitorOn dataAcquisitionS", NULL, cb);

    iStop = 0;

    for (;;) {
	if (iStop) break;
        defSystem.pend((double)0.2);
    }

    itime = time(NULL);
    fprintf(LogFile, "RHICDAEMON-INFO : %s: Exiting %s polarimeter daemon.\n\n", cctime(&itime), myName[MyPolarimeter]);
    fflush(LogFile);
    fclose(LogFile);
    signal(SIGINT, SIG_DFL);
    signal(SIGHUP, SIG_DFL);
    signal(SIGTERM, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    signal(SIGCHLD, SIG_DFL);
    return 0;
}