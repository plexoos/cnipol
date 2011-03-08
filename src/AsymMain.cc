//  Asymmetry Analysis of RHIC pC Polarimeter
//  file name :   AsymMain.cc
//
//  Authors   :   Itaru Nakagawa
//                Dmitri Smirnov
//
//  Creation  :   11/17/2005
//

/**
 *
 * 18 Oct, 2010 - Dmitri Smirnov
 *    - Added -R flag, moved header files, cleaned up the code
 *
 */

#include "AsymMain.h"

using namespace std;


/** Main program */
int main(int argc, char *argv[])
{ //{{{
   // Create a stopwatch and start it
   TStopwatch stopwatch;
   TTimeStamp timestamp;

   // for get option
   extern char *optarg;
   //extern int optind;

   // Initialize Variables
   Initialization();

   // config directories
   confdir = getenv("CONFDIR");

   if ( confdir == NULL ){
      cerr << "environment CONFDIR is not defined" << endl;
      cerr << "e.g. export CONFDIR=/usr/local/cnipol/config" << endl;
      return 0;
   }

   char   cfile[32];
   //int  ramp_read = 0;  // ramp timing file read from argument:1 default:0
   char   enerange[20], cwidth[20], *ptr;
   stringstream sstr;
   int    option_index = 0;

   static struct option long_options[] = {
      {"run-name",            required_argument,   0,   'r'},
      {"raw",                 0,                   0,   TDatprocStruct::MODE_RAW},
      {"feedback",            0,                   0,   'b'},
      {"no-error-detector",   0,                   0,   'a'},
      {"update-db",           no_argument,         0,   0x0100},
      {"no-update-db",        no_argument,         0,   0x0200},
      {"pol-id",              required_argument,   0,   0x0300},
      {"log",                 optional_argument,   0,   'l'},
      {"alpha",               no_argument,         0,   TDatprocStruct::MODE_ALPHA},
      {"calib",               no_argument,         0,   TDatprocStruct::MODE_CALIB},
      {"scaler",              no_argument,         0,   TDatprocStruct::MODE_SCALER},
      {"target",              no_argument,         0,   TDatprocStruct::MODE_TARGET},
      {"profile",             no_argument,         0,   TDatprocStruct::MODE_PROFILE},
      {"quick",               no_argument,         0,   'q'},
      {"graph",               no_argument,         0,   TDatprocStruct::MODE_GRAPH},
      {"no-graph",            no_argument,         0,   TDatprocStruct::MODE_NO_GRAPH},
      {"mode-alpha",          no_argument,         0,   TDatprocStruct::MODE_ALPHA},
      {"mode-calib",          no_argument,         0,   TDatprocStruct::MODE_CALIB},
      {"mode-graph",          no_argument,         0,   TDatprocStruct::MODE_GRAPH},
      {"mode-no-graph",       no_argument,         0,   TDatprocStruct::MODE_NO_GRAPH},
      {"mode-normal",         no_argument,         0,   TDatprocStruct::MODE_NORMAL},
      {"mode-no-normal",      no_argument,         0,   TDatprocStruct::MODE_NO_NORMAL},
      {"mode-scaler",         no_argument,         0,   TDatprocStruct::MODE_SCALER},
      {"mode-raw",            no_argument,         0,   TDatprocStruct::MODE_RAW},
      {"mode-run",            no_argument,         0,   TDatprocStruct::MODE_RUN},
      {"mode-target",         no_argument,         0,   TDatprocStruct::MODE_TARGET},
      {"mode-profile",        no_argument,         0,   TDatprocStruct::MODE_PROFILE},
      {"mode-full",           no_argument,         0,   TDatprocStruct::MODE_FULL},
      {"set-calib",           required_argument,   0,   0x3000},
      {"set-calib-alpha",     required_argument,   0,   0x1000},
      {"set-calib-dl",        required_argument,   0,   0x2000},
      {0, 0, 0, 0}
   };

   int c;

   while ((c = getopt_long(argc, argv, "?hr:f:n:s:c:o:l::t:e:m:d:baCDTABZF:MNW:UGR:Sqg",
                           long_options, &option_index)) != -1)
   {
      switch (c) {

      case '?':
      case 'h':
         dproc.PrintUsage();
         exit(0);

      case 'r':
      case 'f':
         //sprintf(ifile, optarg);
         // if ifile lack of suffix ".data", attach ".data"
         //if (strstr(ifile, suffix) == NULL) strcat(ifile,suffix);
         gDataFileName = dproc.fAsymEnv["DATADIR"] +  "/" + optarg + ".data";

         // Add checks for runName suffix
         dproc.fRunId     = optarg;
         gRunInfo.runName = optarg;
         gRunDb.fRunName  = optarg;
         break;

      case 'n':
         gMaxEventsUser = atol(optarg);
         break;

      case 's':
         dproc.thinout = atol(optarg);
         break;

      case 'c':
         dproc.userCalibFile = optarg;
         break;

      case 'l':
         dproc.fFileStdLogName = (optarg != 0 ? optarg : "");
         break;

      case 't': // set timing shift in banana cut
         dproc.tshift = atoi(optarg);
         extinput.TSHIFT = 1;
         break;

      case 'd': // set timing shift in banana cut
         dproc.dx_offset = atoi(optarg);
         break;

      case 'e': // set energy range
         strcpy(enerange, optarg);

         if ((ptr = strrchr(enerange, ':'))) {
            ptr++;
            dproc.eneu = atoi(ptr);
            strtok(enerange, ":");
            dproc.enel = atoi(enerange);
            if (dproc.enel == 0 || dproc.enel < 0)     { dproc.enel = 0;}
            if (dproc.eneu == 0 || dproc.eneu > 12000) { dproc.eneu = 2000;}
            fprintf(stdout,"ENERGY RANGE LOWER:UPPER = %d:%d\n", dproc.enel, dproc.eneu);
         } else {
             cout << "Wrong specification for energy threshold" << endl;
             return 0;
         }
         break;

      case 'a':
         Flag.EXE_ANOMALY_CHECK=0;
         break;

      case 'F':
         sprintf(cfile, optarg);
         if (!strstr(cfile, "/")) {
             strcat(reConfFile, confdir);
             strcat(reConfFile, "/");
         }
         strcat(reConfFile, cfile);
         extinput.CONFIG = 1;
         break;

      case 'b':
         dproc.FEEDBACKMODE = Flag.feedback = 1;
         break;
      case 'D':
         dproc.DMODE = 1;
         break;
      case 'T':
         dproc.TMODE = 1;
         break;
      case 'A':
         dproc.AMODE = 1;
         break;
      case 'B':
         dproc.BMODE = 1;
         break;
      case 'Z':
         dproc.ZMODE = 1;
         break;
      case 'U':
         dproc.UPDATE = 1;
         break;
      case 'G':
         dproc.MMODE = 1;
         break;
      case 'N':
         dproc.NTMODE = 1;
         break;
      case 'W': // constant width banana cut
         dproc.CBANANA = 1;
         strcpy(cwidth, optarg);
         if ((ptr = strrchr(cwidth,':'))) {
            ptr++;
            dproc.widthu = atoi(ptr);
            strtok(cwidth,":");
            dproc.widthl = atoi(cwidth);
            fprintf(stdout,"CONSTANT BANANA CUT LOWER:UPPER = %d:%d\n",
                    dproc.widthl,dproc.widthu);
            if (dproc.widthu == dproc.widthl)
               fprintf(stdout, "WARNING: Banana Lower = Upper Cut\a\n");
         } else {
            fprintf(stdout, "Wrong specification constant banana cut\n");
            return 0;
         }
         fprintf(stdout,"BANANA Cut : %d <==> %d \n",
                 dproc.widthl,dproc.widthu);
         break;

      case 'm':
         dproc.CBANANA = 2;
         dproc.MassSigma = atof(optarg);
         extinput.MASSCUT = 1;
         break;

      case 'R':
         sstr << optarg;
         sstr >> dproc.SAVETREES;
         break;

      case 0x0100:
         dproc.UPDATE_DB = 1; break;

      case 0x0200:
         dproc.UPDATE_DB = 0; break;

      case 0x0300:
         gRunInfo.fPolId = atoi(optarg); break;

      case 0x1000:
         dproc.fAlphaCalibRun = optarg;
         break;

      case 0x2000:
         dproc.fDlCalibRun = optarg;
         break;

      case 0x3000:
         dproc.fAlphaCalibRun = optarg;
         dproc.fDlCalibRun    = optarg;
         break;

      case 'q':
         dproc.QUICK_MODE = 1; break;

      case 'g':
      case TDatprocStruct::MODE_GRAPH:
         dproc.fModes |= TDatprocStruct::MODE_GRAPH;
         break;

      case TDatprocStruct::MODE_NO_GRAPH:
         dproc.fModes &= ~TDatprocStruct::MODE_GRAPH;
         break;

      case 'C':
      case TDatprocStruct::MODE_ALPHA:
         dproc.fModes |= TDatprocStruct::MODE_ALPHA;
         dproc.CMODE = 1;
         dproc.RECONFMODE = 0;
         dproc.fModes &= ~TDatprocStruct::MODE_NORMAL; // turn off normal mode
         gAsymRoot.fEventConfig = new EventConfig();
         break;

      case TDatprocStruct::MODE_CALIB:
         dproc.fModes |= TDatprocStruct::MODE_CALIB;
         break;

      case TDatprocStruct::MODE_NO_NORMAL:
         dproc.fModes &= ~TDatprocStruct::MODE_NORMAL;
         break;

      case TDatprocStruct::MODE_SCALER:
         dproc.fModes |= TDatprocStruct::MODE_SCALER;
         break;

      case TDatprocStruct::MODE_RAW:
         dproc.fModes |= TDatprocStruct::MODE_RAW;
         dproc.RAWHISTOGRAM = 1;
         break;

      case TDatprocStruct::MODE_RUN:
         dproc.fModes |= TDatprocStruct::MODE_RUN; break;

      case TDatprocStruct::MODE_TARGET:
         dproc.fModes |= TDatprocStruct::MODE_TARGET; break;

      case TDatprocStruct::MODE_PROFILE:
         dproc.fModes |= TDatprocStruct::MODE_PROFILE;
         dproc.fModes |= TDatprocStruct::MODE_TARGET; // profile hists depend on target ones
         break;

      case TDatprocStruct::MODE_FULL:
         dproc.fModes |= TDatprocStruct::MODE_FULL; break;

      default:
         gSystem->Error("   main", "Invalid Option");
         dproc.PrintUsage();
         exit(0);
      }
   }

   dproc.ProcessOptions();

   // Extract RunID from input filename
   //int chrlen = strlen(ifile)-strlen(suffix) ; // f.e. 10100.101.data - .data = 10100.001
   //char RunID[chrlen];
   //strncpy(RunID, ifile, chrlen);
   //RunID[chrlen] = '\0'; // Without RunID[chrlen]='\0', RunID screwed up.

   gRunInfo.RUNID = strtod(gRunInfo.runName.c_str(), NULL); // return 0 when "RunID" contains alphabetical char.
   //printf("RUNID: %f\n", gRunInfo.RUNID);

   // Get PolarimetryID and RHIC Beam (Yellow or Blue) from RunID
   // ds: not needed anymore since we are getting this info from raw data for all runs
   //if (!dproc.CMODE) GetPolarimetryID_and_RHICBeam(RunID);

   // For normal runs, RUNID != 0. Then read run conditions from run.db.
   // Otherwise, data filename with characters skip readdb and reconfig routines
   // assuming these are energy calibration or test runs.
   //if (gRunInfo.RUNID)
   //   readdb(gRunInfo.RUNID);
   //else
   //   dproc.RECONFMODE = 0;
   
   // Read run info from database
   TStructRunDB *runDb = gAsymRunDb.Select(gRunDb.fRunName);

   // Read data file into memory
   RawDataProcessor *rawData = new RawDataProcessor(gDataFileName);

   rawData->ReadRecBegin(gRunInfo);

   // Replace gRunDb
   if (runDb) {
      printf("Run \"%s\" found in database\n", runDb->fRunName.c_str());
      gRunDb.UpdateFields(*runDb);
      //gRunDb.Print();
      gRunInfo.fPolId = gRunDb.fPolId;

   // the following is pretty messed up... needs a clean up and better logic
   } else {

      // Extract and overwrite (!) basic run info (gRunInfo) from raw data
      //ReadRecBegin(gRunInfo);

      printf("Run \"%s\" NOT found in database. Consider an update\n", gRunDb.fRunName.c_str());
      gAsymRunDb.Select(); // read all entries into memory
      //gAsymRunDb.Print();
      gAsymRunDb.Insert(&gRunDb);
      //gAsymRunDb.Print();
      gAsymRunDb.Dump(); // write to DB file

      gAsymRunDb.Clear();
      runDb = gAsymRunDb.Select(gRunDb.fRunName); // now read all available common info for this run
      gRunDb.UpdateFields(*runDb);
      gRunInfo.fPolId = gRunDb.fPolId;
   }

   // Overwrite the offline version (if set previously)
   gRunDb.SetAsymVersion(gRunInfo.fAsymVersion);

   // We should be done reading all common/default parameters from DB by now
   gRunDb.Print();

   dproc.Update(gRunDb);
   gRunInfo.Update(gRunDb);

   dproc.Print();
   //gRunInfo.Print();

   // Manually set disabled channels... deprecated
   //{{{
   /*
   // XXX temp fix: disable strips
   // blue1
   //gRunInfo.fDisabledChannels[11] = 1;
   //gRunInfo.fDisabledChannels[65] = 1;
   // blue2
   //int disabledStrips[] = {
   //   1, 1, 1, 1, 1, 1,  0, 0, 0, 0, 0, 0,   1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1,
   //   0, 0, 0, 0, 0, 0,  1, 1, 1, 1, 1, 1,   1, 1, 1, 1, 1, 1,  0, 0, 0, 0, 0, 0,
   //   1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1,   0, 0, 0, 0, 0, 0,  1, 1, 1, 1, 1, 1 };

   if (gRunInfo.fPolId == 1 || gRunInfo.fPolId == 2) { // downstream

      // disabled hamamatsu's
      int disabledStrips[] = {
         0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0,   1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1,
         0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0,
         1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1,   0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0
      };

      // disabled channels from crate 6
      //int disabledStrips[] = {
      //   0, 1, 0, 1, 0, 1,  0, 1, 0, 1, 0, 1,   1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1,
      //   0, 1, 0, 1, 0, 1,  0, 1, 0, 1, 0, 1,   0, 1, 0, 1, 0, 1,  0, 1, 0, 1, 0, 1,
      //   1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1,   0, 1, 0, 1, 0, 1,  0, 1, 0, 1, 0, 1
      //};

      memcpy(gRunInfo.fDisabledChannels, disabledStrips, sizeof(int)*72);

   } else if (gRunInfo.fPolId == 0) { // blue 1 upstream

      int disabledStrips[] = {
         0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 1,   0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 1,  0, 0, 0, 0, 0, 0
      };

      memcpy(gRunInfo.fDisabledChannels, disabledStrips, sizeof(int)*72);

   } else if (gRunInfo.fPolId == 3) { // yellow 2 upstream

      //int disabledStrips[] = {
      //   0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0,
      //   0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0,   1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1,
      //   0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0
      //};
      int disabledStrips[] = {
         0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0
      };

      memcpy(gRunInfo.fDisabledChannels, disabledStrips, sizeof(int)*72);
   }
   */
   //}}}

   //gAsymRunDb.PrintCommon();

   //gAsymRunDb.Print();
   //while(1) {};
   //exit(-1);

   // Find RunConfig object in the calibration files and update
   gAsymRoot.UpdateRunConfig();

   //printf("calib= %d\n", dproc.HasCalibBit());
   //gAsymRoot.fEventConfig->fCalibrator->PrintAsPhp();
   //return 0;

   // if output hbk file is not specified
   char hbk_outfile[256] = "out.hbook";
   fprintf(stdout, "hBOOK default file: %s \n", hbk_outfile);

   // Hbook Histogram Booking
   fprintf(stdout, "Booking HBOOK file\n");
   hist_book(hbk_outfile);

   // Root Histogram Booking
   char outRootFileName[256];

   sprintf(outRootFileName, "%s/%s/%s.root",
      dproc.fAsymEnv["CNIPOL_RESULTS_DIR"].c_str(), gRunInfo.runName.c_str(),
      gRunInfo.runName.c_str());

   fprintf(stdout, "Booking ROOT histograms: %s\n", outRootFileName);

   gAsymRoot.RootFile(outRootFileName);

   // Create tree if requested
   if (dproc.SAVETREES.any()) { gAsymRoot.CreateTrees(); }

   // If requested update for data (not alpha) calibration constants we need to
   // quickly do some pre-processing to extract parameters from the data
   // itself. For example, rough estimates of the dead layer and t0 are needed
   // to set preliminary cuts.

   if ( dproc.HasCalibBit() && !dproc.CMODE)
      readDataFast();

   //return 0;

   if (!dproc.QUICK_MODE) {

      //ds: XXX
      //gAsymRoot.PreProcess();

      // Main Event Loop
      readloop();

      gAsymRoot.PostProcess();
   }

   //gRunInfo.Print();

   // Delete Unnecessary ROOT Histograms
   gAsymRoot.DeleteHistogram();

   // Close histogram file
   hist_close(hbk_outfile);

   // Update calibration constants if requested
   if (dproc.HasCalibBit()) {
      gAsymRoot.Calibrate();
      //gAsymRoot.fEventConfig->fCalibrator->PrintAsPhp();
      //gAsymRoot.fEventConfig->fCalibrator->PrintAsConfig();
   }

   // Update calibration constants if requested
   //gRunDb.Print();

   if (dproc.UPDATE_DB) {
      // Select all runs from database
      gAsymRunDb.Select();
      gAsymRunDb.Insert(&gRunDb);
      gAsymRunDb.Dump();
   }

   // Stop stopwatch and save results
   stopwatch.Stop();
   dproc.procDateTime =  timestamp.GetSec();
   dproc.procTimeReal =  stopwatch.RealTime();
   dproc.procTimeCpu  =  stopwatch.CpuTime();

   printf("Processing started: %s\n",   timestamp.AsString("l"));
   printf("Process time: %f seconds\n", dproc.procTimeReal);

   //gAsymRoot.fEventConfig->PrintAsPhp();
   //gAsymRoot.fEventConfig->fCalibrator->PrintAsConfig();

   // Set pointers to global structures for later saving in ROOT file
   // if previously allocated delete object
   //delete gAsymRoot.fEventConfig->fConfigInfo;
   gAsymRoot.fEventConfig->fConfigInfo = cfginfo;

   // if previously allocated delete object
   delete gAsymRoot.fEventConfig->fRunInfo;
   gAsymRoot.fEventConfig->fRunInfo    = &gRunInfo;

   //delete gAsymRoot.fEventConfig->fDatproc;
   gAsymRoot.fEventConfig->fDatproc    = &dproc;

   delete gAsymRoot.fEventConfig->fRunDB;
   gAsymRoot.fEventConfig->fRunDB      = &gRunDb;

   delete gAsymRoot.fEventConfig->fAnaResult;
   //gAnaResults.PrintAsPhp();
   gAsymRoot.fEventConfig->fAnaResult  = &gAnaResults;

   gAsymRoot.fEventConfig->PrintAsPhp(dproc.GetRunInfoFile());
   gAsymRoot.fEventConfig->PrintAsConfig(dproc.GetRunConfFile());

   if (dproc.HasGraphBit())
      gAsymRoot.SaveAs("^.*$", dproc.GetImageDir());
      //gAsymRoot.SaveAs("profile", dproc.GetImageDir());

   // Closing ROOT File
   gAsymRoot.CloseROOTFile();

   return 1;
} //}}}


// ===================================
// for Bunch by Bunch base analysis
// ===================================
int BunchSelect(int bid)
{ //{{{
  int go = 0;
  //  int BunchList[11]={4,13,24,33,44,53,64,73,84,93,104};
  int BunchList[26]={3,6,13,16,23,26,33,36,43,46,53,56,63,66,
                     73,76,83,86,93,96,103,106};

  for (int i=0; i<14; i++) {
    //    BunchList[i]++;
    if (bid == BunchList[i]) {
      go=1;
      break;
    }
  }

  return go;
} //}}}


//
// Class name  :
// Method name : GetPolarimetryID_and_RHICBeam(char RunID[])
//
// Description : Identify Polarimety ID and RHIC Beam (blue or yellow)
// Input       : char RunID[]
// Return      :
//
int GetPolarimetryID_and_RHICBeam(char RunID[])
{ //{{{
  char ID = *(strrchr(RunID,'.')+1);

  switch (ID) {
  case '0':
    gRunInfo.fPolBeam      = 2;
    gRunInfo.PolarimetryID = 1; //   blue polarimeter-1
    gRunInfo.fPolStream    = 1;
    break;
  case '1':
    gRunInfo.fPolBeam      = 1;
    gRunInfo.PolarimetryID = 1; // yellow polarimeter-1
    gRunInfo.fPolStream    = 2;
    break;
    break;
  case '2':
    gRunInfo.fPolBeam      = 2;
    gRunInfo.PolarimetryID = 2; //   blue polarimeter-2
    gRunInfo.fPolStream    = 2;
    break;
  case '3':
    gRunInfo.fPolBeam      = 1;
    gRunInfo.PolarimetryID = 2; // yellow polarimeter-2
    gRunInfo.fPolStream    = 1;
    break;
  default:
    fprintf(stdout, "Unrecognized RHIC beam and Polarimeter-ID. Perhaps calibration data..?");
    break;
  }

  /*
  fprintf(stdout,"RUNINFO: RunID=%.3f fPolBeam=%d PolarimetryID=%d\n",
          gRunInfo.RUNID, gRunInfo.fPolBeam, gRunInfo.PolarimetryID);
  */

  return 0;
} //}}}


// =========================
// Read the parameter file
// =========================

// Ramp timing file
void ReadRampTiming(char *filename)
{ //{{{
   printf("\nReading ... cut parameter file : %s \n", filename);

   ifstream rtiming;
   rtiming.open(filename);

   if (!rtiming) {
      cerr << "failed to open ramp timing file" <<endl;
      exit(1);
   }

   memset(ramptshift, 0, sizeof(ramptshift));

   float runt;
   int index = 0;

   while (!rtiming.eof()) {
      rtiming >> runt >> ramptshift[index] ;
      index ++;
   }

   rtiming.close();
} //}}}


// Calibration parameter
void reConfig(TRecordConfigRhicStruct *cfginfo)
{ //{{{
    ifstream configFile;
    configFile.open(reConfFile);

    if (!configFile) {
       cout << "Failed to open Config File : " << reConfFile << endl;
       cout << "Proceed with original configuration from raw data file" << endl;
       return;
    }

    printf("**********************************\n");
    printf("** Configuration is overwritten **\n");
    printf("**********************************\n");

    cout << "Reading configuration info from : " << reConfFile <<endl;

    char  *tempchar;
    char   buffer[300];
    float  t0n, ecn, edeadn, a0n, a1n, ealphn, dwidthn, peden;
    float  c0n, c1n, c2n, c3n, c4n;
    int    stripn;
    int    linen = 0;

    while (!configFile.eof()) {

       configFile.getline(buffer, sizeof(buffer), '\n');

       if (strstr(buffer,"Channel")!=0) {

          tempchar = strtok(buffer,"l");
          stripn   = atoi(strtok(NULL, "="));
          t0n      = atof(strtok(NULL, " "));
          ecn      = atof(strtok(NULL, " "));
          edeadn   = atof(strtok(NULL, " "));
          a0n      = atof(strtok(NULL, " "));
          a1n      = atof(strtok(NULL, " "));
          ealphn   = atof(strtok(NULL, " "));
          dwidthn  = atof(strtok(NULL, " ")) + dproc.dx_offset; // extra thickness
          peden    = atof(strtok(NULL, " "));
          c0n      = atof(strtok(NULL, " "));
          c1n      = atof(strtok(NULL, " "));
          c2n      = atof(strtok(NULL, " "));
          c3n      = atof(strtok(NULL, " "));
          c4n      = atof(strtok(NULL, " "));

          cfginfo->data.chan[stripn-1].edead  = edeadn;
          cfginfo->data.chan[stripn-1].ecoef  = ecn;
          cfginfo->data.chan[stripn-1].t0     = t0n;
          cfginfo->data.chan[stripn-1].A0     = a0n;
          cfginfo->data.chan[stripn-1].A1     = a1n;
          cfginfo->data.chan[stripn-1].acoef  = ealphn;
          cfginfo->data.chan[stripn-1].dwidth = dwidthn;
          cfginfo->data.chan[stripn-1].pede   = peden;
          cfginfo->data.chan[stripn-1].C[0]   = c0n;
          cfginfo->data.chan[stripn-1].C[1]   = c1n;
          cfginfo->data.chan[stripn-1].C[2]   = c2n;
          cfginfo->data.chan[stripn-1].C[3]   = c3n;
          cfginfo->data.chan[stripn-1].C[4]   = c4n;

          cout << " Strip "    << stripn;
          cout << " Ecoef "    << ecn;
          cout << " T0 "       << t0n;
          cout << " A0 "       << a0n;
          cout << " A1 "       << a1n;
          cout << " Acoef "    << ealphn;
          cout << " Dwidth "   << dwidthn;
          cout << " Pedestal " << peden    << endl;
       }

       linen ++;
    }

    configFile.close();
} //}}}


//
// Class name  :
// Method name : ConfigureActiveStrip(int mask.detector)
//
// Description : Disable detector and configure active strips
//
// Input       : int mask.detector
// Return      : gRunInfo.ActiveDetector[i] remains masked strip configulation
//
void ConfigureActiveStrip(int mask)
{ //{{{
   // Disable Detector First
   for (int i=0; i<NDETECTOR; i++) {

      if ( (~mask>>i) & 1) {

         gRunInfo.ActiveDetector[i] = 0x000;

         for (int j=0; j<NSTRIP_PER_DETECTOR; j++) {
            gRunInfo.NActiveStrip--;
            gRunInfo.ActiveStrip[i*NSTRIP_PER_DETECTOR+j] = 0;
         }
      }
   }

   // Configure Active Strips
   int det, strip=0;

   for (int i=0; i<gRunInfo.NDisableStrip; i++) {

      det = gRunInfo.fDisabledChannels[i]/NSTRIP_PER_DETECTOR;

      // skip if the detector is already disabled
      if ((mask>>det)&1) {
         strip = gRunInfo.fDisabledChannels[i] - det*NSTRIP_PER_DETECTOR;
         gRunInfo.ActiveDetector[det] ^= int(pow(2,double(strip))); // mask strips of detector=det
         gRunInfo.ActiveStrip[strip+det*NSTRIP_PER_DETECTOR] = 0;
         gRunInfo.NActiveStrip--;
      }
   }

   // Active Detector and Strip Configulation
   printf("ReConfigured Active Detector =");

   for (int i=0; i<NDETECTOR; i++)  printf(" %1d", gRunInfo.ActiveDetector[i] ? 1 : 0 );
   printf("\n");
   //    printf("Active Strip Config =");
   //    for (int i=NDETECTOR-1; i>=0; i--) printf(" %x", gRunInfo.ActiveDetector[i]);
   //    printf("\n");

   printf("Reconfigured Active Strip Config =");

   for (int i=0; i<NSTRIP; i++) {
     if (i%NSTRIP_PER_DETECTOR == 0) printf(" ");
     printf("%d", gRunInfo.ActiveStrip[i]);
   }

   printf("\n");
} //}}}


//
// Class name  :
// Method name : DisabledDet
//
// Description : Check Disabled detector
// Input       : int strip nuumber
// Return      : 1 if disabled. otherwise 0
//
/*
int DisabledDet(int det)
{ //{{{
  // det(0,1,2,3,4,5} => {0, 1, 0, 0, 1, 0} => 18
  int DeadDet = tgt.VHtarget ? 18 : 0 ;
  //                            ^   ^
  //                       H-target V-target

  return DeadDet>>det & 1 ;

} //}}}
*/


// Description : Initialize variables
void Initialization()
{ //{{{
   for (int i=0; i<NSTRIP; i++) {
      feedback.mdev[i] = 0.;
      feedback.RMS[i]  = dproc.OneSigma;
   }
 
   //gRunInfo.TgtOperation = "fixed";
   strcpy(gRunInfo.TgtOperation, "fixed");
 
   // Initiarize Strip counters
   for (int i=0; i<NSTRIP; i++) {
 
      for (int j=0; j<3; j++) {
         cntr.reg.NStrip[j][i] = cntr.alt.NStrip[j][i] = 0;
         cntr.phx.NStrip[j][i] = cntr.str.NStrip[j][i] = 0;
      }
 
      for (int j=0; j<3; j++) {
         for(int kk=0; kk<MAXDELIM; kk++)
            cntr_tgt.reg.NStrip[kk][j][i] = 0;
      }
   }
} //}}}
