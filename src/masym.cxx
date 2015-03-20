#include <cstdlib>
#include <fstream>
#include <time.h>
#include <set>
#include <string>

#include "masym.h"

#include "TEnv.h"
#include "TROOT.h"

#include "CnipolAsymHists.h"
#include "MAsymFillHists.h"
#include "MAsymRunHists.h"
#include "MAsymRateHists.h"
#include "MAsymPmtHists.h"

#include "AnaGlobResult.h"

#include "AsymDbSql.h"
#include "AsymGlobals.h"
#include "MAsymAnaInfo.h"
#include "MeasInfo.h"

#include "utils/utils.h"



/** */
int main(int argc, char *argv[])
{
   setbuf(stdout, NULL);

   // Create a default one
   gMeasInfo = new MeasInfo();

   // Do not attempt to recover files
   gEnv->SetValue("TFile.Recover", 0);

   MAsymAnaInfo mAsymAnaInfo;
   mAsymAnaInfo.ProcessOptions(argc, argv);
   mAsymAnaInfo.VerifyOptions();

   gMAsymAnaInfo = &mAsymAnaInfo;

   AnaGlobResult anaGlobResult;
   anaGlobResult.Configure(mAsymAnaInfo);

   gROOT->Macro(CNIPOL_ROOT_DIR "/contrib/styles/style_masym.C");

   gRunConfig.fBeamEnergies.clear();
   gRunConfig.fBeamEnergies.insert(kINJECTION);
   gRunConfig.fBeamEnergies.insert(kBEAM_ENERGY_255);

   std::string filelist     = mAsymAnaInfo.GetMListFullPath();

   MAsymRoot mAsymRoot(mAsymAnaInfo);
   mAsymRoot.SetAnaGlobResult(&anaGlobResult);

   Info("masym", "Starting first pass...");

   // Container with measurements passed QA cuts. Used to save time on opening
   // input files in the second pass
   std::set<EventConfig> gGoodMeass;

   // Fill chain with all input files from filelist
   TObject *o;
   TIter   *next = new TIter(utils::getFileList(filelist));

   // Loop over the runs and record the time of the last flattop measurement in the fill
   while (next && (o = (*next)()) )
   {
      std::string measId   = std::string(((TObjString*) o)->GetName());
      std::string fileName = mAsymAnaInfo.GetResultsDir() + "/" + measId + "/" + measId + mAsymAnaInfo.GetSuffix() + ".root";

      TFile f(fileName.c_str(), "READ");

      if (f.IsZombie()) {
         Error("masym", "File not found or zombie: %s. Skipping...", fileName.c_str());
         f.Close();
         continue;
      }

      Info("masym", "Found file: %s", fileName.c_str());

      EventConfig *gMM = (EventConfig*) f.FindObjectAny("measConfig");

      if (!gMM) {
         Error("masym", "MM not found. Skipping...");
         f.Close();
         continue;
      }

      char strTime[80];
      strftime(strTime, 80, "%X", localtime(&gMM->fMeasInfo->fStartTime));

      Double_t    runId           = gMM->fMeasInfo->RUNID;
      UInt_t      fillId          = (UInt_t) runId;
      EBeamEnergy beamEnergy      = gMM->fMeasInfo->GetBeamEnergy();

      Float_t polarization    = 0;
      Float_t polarizationErr = -1;

      TFitResultPtr fitResPolarPhi = gMM->fAnaMeasResult->fFitResPolarPhi;

      if (fitResPolarPhi.Get()) {
         polarization    = fitResPolarPhi->Value(0) * 100;
         polarizationErr = fitResPolarPhi->FitResult::Error(0) * 100;
      }

      Float_t profileRatio     = gMM->fAnaMeasResult->fProfilePolarR.first;
      Float_t profileRatioErr  = gMM->fAnaMeasResult->fProfilePolarR.second;

      // the cut on polarization value should be removed
      if (polarization < 10 || polarization > 99 || polarizationErr > 30 ||
	  //  gRunConfig.fBeamEnergies.find(beamEnergy) == gRunConfig.fBeamEnergies.end() ||
          gMM->fMeasInfo->fMeasType != kMEASTYPE_SWEEP ||
          (TMath::Abs(profileRatio) > 5.000) ||                             // exclude very large values
          (TMath::Abs(profileRatio) > 1.000 && profileRatioErr < 0.05) ||   // exclude large values with small errors
          (TMath::Abs(profileRatio) > 1.000 && profileRatioErr > 0.50) ||   // exclude large values with large errors
          profileRatioErr < 0.01 || profileRatioErr > 10 ||                 // exclude too small and too big errors. probably from failed fits?
          ( fillId >= 17064 && fillId <= 17084 )
         )
      {
         Warning("masym", "Measurement %9.3f did not pass basic QA check", runId);
         printf("%8.3f, %s, %3d, %f, %f, %s, %f, %f\n",
                runId, strTime, beamEnergy, polarization, polarizationErr,
                RunConfig::AsString(gMM->fMeasInfo->fMeasType).c_str(), profileRatio,
                profileRatioErr );

         f.Close();
         continue;
      }

      mAsymRoot.UpdMinMax(*gMM);

      // Check that asym hist container exists in this file
      DrawObjContainer *gHIn = new DrawObjContainer(&f);
      gHIn->d["asym"] = new CnipolAsymHists();
      gHIn->ReadFromDir();

      // To calculate normalization factors for p-Carbon we need to save all
      // p-Carbon measurements in the first pass
      anaGlobResult.AddMeasResult(*gMM, gHIn);

      delete gHIn;

      f.Close();

      gGoodMeass.insert(*gMM);
   }

   // Update global run parameters before anything else
   gRunConfig.SetBeamEnergies(anaGlobResult.GetBeamEnergies());

   // Create graphic containers
   DrawObjContainer *gH = new DrawObjContainer(&mAsymRoot);

   gH->d["fills"] = new MAsymFillHists(new TDirectoryFile("fills", "fills", "", &mAsymRoot));
   gH->d["rate"]  = new MAsymRateHists(new TDirectoryFile("rate",  "rate",  "", &mAsymRoot));
   gH->d["runs"]  = new MAsymRunHists (new TDirectoryFile("runs",  "runs",  "", &mAsymRoot));
   gH->d["pmt"]   = new MAsymPmtHists (new TDirectoryFile("pmt",   "pmt",   "", &mAsymRoot));

   // Adjust min/max fill for histogram limits
   ((MAsymRunHists*) gH->d["runs"])->SetMinMaxFill(anaGlobResult.GetMinFill(), anaGlobResult.GetMaxFill());
   ((MAsymRunHists*) gH->d["runs"])->SetMinMaxTime(anaGlobResult.GetMinTime(), anaGlobResult.GetMaxTime());
   ((MAsymRunHists*) gH->d["runs"])->AdjustMinMaxFill();

   // Process run/fill results, i.e. calculate fill average, ...
   Info("masym", "Analyzing measurements...");

   anaGlobResult.AddHJMeasResult();
   anaGlobResult.Process(gH);

   Info("masym", "Starting second pass...");

   // Now process only good runs
   std::set<EventConfig>::const_iterator iMeas = gGoodMeass.begin();

   for ( ; iMeas!=gGoodMeass.end(); ++iMeas) {
      // Overwrite the default gMeasInfo with the saved one
      gMeasInfo = iMeas->fMeasInfo;

      Info("masym", "Processing measurement: %.3f", iMeas->fMeasInfo->RUNID);

      gH->Fill((const EventConfig&) *iMeas);
   }

   gH->PostFill(anaGlobResult);
   gH->PostFill();
   gH->UpdateLimits();

   mAsymRoot.SetHists(*gH);

   if (mAsymAnaInfo.HasGraphBit())
      mAsymRoot.SaveAs("^.*$", mAsymAnaInfo.GetImageDir());

   if (mAsymAnaInfo.fFlagUpdateDb) {
      AsymDbSql asymDbSql;
      asymDbSql.OpenConnection();
      anaGlobResult.UpdateInsertDb(asymDbSql);
      asymDbSql.CloseConnection();
   }
   Info("masym", "HERE I AM3...");
   gH->Write();

   mAsymRoot.Print();
   mAsymRoot.Close();

   //   anaGlobResult.Print();

   return EXIT_SUCCESS;
}
