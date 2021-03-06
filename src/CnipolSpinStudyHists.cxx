#include <algorithm>
#include <math.h>

#include "CnipolSpinStudyHists.h"

#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TGraphErrors.h"
#include "TStyle.h"

#include "utils/utils.h"

#include "AsymGlobals.h"
#include "AnaMeasResult.h"
#include "TargetInfo.h"
#include "CnipolAsymHists.h"


ClassImp(CnipolSpinStudyHists)

using namespace std;

/** Default constructor. */
CnipolSpinStudyHists::CnipolSpinStudyHists() : DrawObjContainer()
{
   BookHists();
}


CnipolSpinStudyHists::CnipolSpinStudyHists(TDirectory *dir) : DrawObjContainer(dir)
{
   BookHists();
}


/** Default destructor. */
CnipolSpinStudyHists::~CnipolSpinStudyHists()
{
}


/** */
void CnipolSpinStudyHists::BookHists()
{
   fDir->cd();

   string      shName;
   TH1        *hist;
   TAttMarker  styleMarker;

   styleMarker.SetMarkerStyle(kFullCircle);
   styleMarker.SetMarkerSize(1);
   styleMarker.SetMarkerColor(kGreen+2);

   shName = "hChVsOscillPhase";
   hist = new TH2I(shName.c_str(), shName.c_str(), 8, 0, _TWO_PI, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
   hist->SetTitle("; Oscill. Phase; Channel Id;");
   hist->SetOption("colz NOIMG");
   o[shName] = hist;

   shName = "hDetVsOscillPhase";
   hist = new TH2I(shName.c_str(), shName.c_str(), 8, 0, _TWO_PI, N_DETECTORS, 0.5, N_DETECTORS+0.5);
   hist->SetTitle("; Oscill. Phase; Detector Id;");
   hist->SetOption("colz NOIMG");
   o[shName] = hist;

   shName = "hAsymVsOscillPhase_X45";
   hist = new TH2C(shName.c_str(), shName.c_str(), 8, 0, _TWO_PI, 100, 0, 1);
   hist->SetTitle("; Oscill. Phase; Asymmetry;");
   hist->SetOption("E1");
   o[shName] = hist;

   shName = "hAsymVsOscillPhase_Y45";
   hist = new TH2C(shName.c_str(), shName.c_str(), 8, 0, _TWO_PI, 100, 0, 1);
   hist->SetTitle("; Oscill. Phase; Asymmetry;");
   hist->SetOption("DUMMY");
   o[shName] = hist;

   SpinStateSetIter iSS = gRunConfig.fSpinStates.begin();
   for (; iSS!=gRunConfig.fSpinStates.end(); ++iSS)
   {
      string sSS = gRunConfig.AsString(*iSS);

      shName = "hChVsOscillPhase_" + sSS;
      hist = new TH2I(shName.c_str(), shName.c_str(), 8, 0, _TWO_PI, N_SILICON_CHANNELS, 0.5, N_SILICON_CHANNELS+0.5);
      hist->SetTitle("; Oscill. Phase; Channel Id;");
      hist->SetOption("colz NOIMG");
      o[shName] = hist;

      // Detector Id
      shName = "hDetVsOscillPhase_" + sSS;
      hist = new TH2I(shName.c_str(), shName.c_str(), 8, 0, _TWO_PI, N_DETECTORS, 0.5, N_DETECTORS+0.5);
      hist->SetTitle("; Oscill. Phase; Detector Id;");
      hist->SetOption("colz NOIMG");
      o[shName] = hist;
   }

   AsymTypeSetIter iAsymType = gRunConfig.fAsymTypes.begin();
   for (; iAsymType!=gRunConfig.fAsymTypes.end(); ++iAsymType)
   {
      string  sAsymType = gRunConfig.AsString(*iAsymType);
      Color_t color     = RunConfig::AsColor(*iAsymType);

      styleMarker.SetMarkerColor(color);

      // Create graphs for different spin states
      hist = (TH1*) o.find("hAsymVsOscillPhase_X45")->second;
      shName = "gr" + string(hist->GetName()) + "_" + sAsymType;
      TGraphErrors *grAsymVsOscillPhase_X45_ = new TGraphErrors();
      grAsymVsOscillPhase_X45_->SetName(shName.c_str());
      styleMarker.Copy(*grAsymVsOscillPhase_X45_);

      hist->GetListOfFunctions()->Add(grAsymVsOscillPhase_X45_, "p");

      shName = "hAsymVsOscillPhase_X45_" + sAsymType;
      hist = utils::ConstructTH1CWithTGraphErrors(shName.c_str(), "; Oscill. Phase; Asymmetry;", (TStyle*) &styleMarker, 8, 0, _TWO_PI);
      o[shName] = hist;

      // Create graphs for different spin states
      hist = (TH1*) o.find("hAsymVsOscillPhase_Y45")->second;
      shName = "gr" + string(hist->GetName()) + "_" + sAsymType;
      TGraphErrors *grAsymVsOscillPhase_Y45_ = new TGraphErrors();
      grAsymVsOscillPhase_Y45_->SetName(shName.c_str());
      styleMarker.Copy(*grAsymVsOscillPhase_Y45_);

      hist->GetListOfFunctions()->Add(grAsymVsOscillPhase_Y45_, "p");

      shName = "hAsymVsOscillPhase_Y45_" + sAsymType;
      hist = utils::ConstructTH1CWithTGraphErrors(shName.c_str(), "; Oscill. Phase; Asymmetry;", (TStyle*) &styleMarker, 8, 0, _TWO_PI);
      o[shName] = hist;
   }
}


/** */
void CnipolSpinStudyHists::Fill(ChannelEvent *ch)
{
   UChar_t  chId = ch->GetChannelId();
   UChar_t  bId  = ch->GetBunchId() + 1;
   string   sSS  = gRunConfig.AsString( gMeasInfo->GetBunchSpin(bId) );
   //Double_t time = ((Double_t) ch->GetRevolutionId())/RHIC_REVOLUTION_FREQ;

   const std::vector<UInt_t>& spinFlipperMarkers = gMeasInfo->GetSpinFlipperMarkers();
   UInt_t spinFlipperMarkerRevId = spinFlipperMarkers.size() > 0 ? spinFlipperMarkers[0] : 0;

   Double_t phase = _TWO_PI * (gMeasInfo->GetAcDipolePeriod()/_RHIC_AC_DIPOLE_MAX_PERIOD) * ((Double_t) ch->GetRevolutionId() + (bId-1)/120. - spinFlipperMarkerRevId);
   phase = fmod(phase, _TWO_PI);

   //Double_t delta_phase = gMeasInfo->GetSpinFlipperPhase();

   ((TH2*) o.find("hChVsOscillPhase_" + sSS)->second) -> Fill(phase, chId);
}


/** */
void CnipolSpinStudyHists::FillDerived()
{
   Info("FillDerived()", "Called");

   TH2* hChVsOscillPhase  = (TH2*) o["hChVsOscillPhase"];
   TH2* hDetVsOscillPhase = (TH2*) o["hDetVsOscillPhase"];

   SpinStateSetIter iSS = gRunConfig.fSpinStates.begin();
   for ( ; iSS!=gRunConfig.fSpinStates.end(); ++iSS)
   {
      string sSS = gRunConfig.AsString(*iSS);

      TH2* hChVsOscillPhase_       = (TH2*) o["hChVsOscillPhase_"  + sSS];
      TH2* hDetVsOscillPhase_      = (TH2*) o["hDetVsOscillPhase_" + sSS];

      for (int iCh=1; iCh<=N_SILICON_CHANNELS; iCh++)
      {
         // Fill detector histograms from the channel ones
         // Skip disabled channels
         if (gMeasInfo->IsDisabledChannel(iCh)) continue;

         UShort_t iDet = RunConfig::GetDetectorId(iCh);

         for (int iBin=1; iBin<=hDetVsOscillPhase_->GetNbinsX(); iBin++)
         {
            Double_t bc_ch  = hChVsOscillPhase_->GetBinContent(iBin, iCh);
            Double_t bc_det = hDetVsOscillPhase_->GetBinContent(iBin, iDet);
            hDetVsOscillPhase_->SetBinContent(iBin, iDet, bc_det + bc_ch);
         }
      }

      hChVsOscillPhase->Add(hChVsOscillPhase_);
      hDetVsOscillPhase->Add(hDetVsOscillPhase_);
   }
}


/** */
void CnipolSpinStudyHists::PostFill()
{
   TH2& hDetVsOscillPhase_up   = (TH2&) *o.find("hDetVsOscillPhase_up")->second;
   TH2& hDetVsOscillPhase_down = (TH2&) *o.find("hDetVsOscillPhase_down")->second;
   TH1& hAsymVsOscillPhase_X45 = (TH1&) *o.find("hAsymVsOscillPhase_X45")->second;
   TH1& hAsymVsOscillPhase_Y45 = (TH1&) *o.find("hAsymVsOscillPhase_Y45")->second;

   gAsymCalculator.CalcOscillPhaseAsymSqrtFormula(hDetVsOscillPhase_up, hDetVsOscillPhase_down, hAsymVsOscillPhase_X45, AsymCalculator::X45Dets);
   gAsymCalculator.CalcOscillPhaseAsymSqrtFormula(hDetVsOscillPhase_up, hDetVsOscillPhase_down, hAsymVsOscillPhase_Y45, AsymCalculator::Y45Dets);

   TF1 fitFunc("fitFunc", "[0] + [1]*sin(x + [2])", 0, _TWO_PI);

   hAsymVsOscillPhase_X45.Fit(&fitFunc);
   //hAsymVsOscillPhase_Y45.Fit(&fitFunc);
   fitFunc.SetParNames("Offset", "Amplitude", "Phase");

   AsymTypeSetIter iAsymType = gRunConfig.fAsymTypes.begin();
   for (; iAsymType!=gRunConfig.fAsymTypes.end(); ++iAsymType)
   {
      string sAsymType        = gRunConfig.AsString(*iAsymType);
      TGraphErrors* graphErrs;

      // Fit every asymmetry type graph for X45
      string shName = "gr" + string(hAsymVsOscillPhase_X45.GetName()) + "_" + sAsymType;
      graphErrs = (TGraphErrors*) hAsymVsOscillPhase_X45.GetListOfFunctions()->FindObject(shName.c_str());
      graphErrs->Fit(&fitFunc);

      // Add graphs for each bin to a single histograms
      TH1* hist = (TH1*) o["hAsymVsOscillPhase_X45_" + sAsymType];
      hist->GetListOfFunctions()->Add(graphErrs->Clone(), "p");

      utils::UpdateLimitsFromGraphs(hist, 2);

      // Fit every asymmetry type graph for Y45
      shName = "gr" + string(hAsymVsOscillPhase_Y45.GetName()) + "_" + sAsymType;
      graphErrs = (TGraphErrors*) hAsymVsOscillPhase_Y45.GetListOfFunctions()->FindObject(shName.c_str());
      graphErrs->Fit(&fitFunc);

      // Add graphs for each bin to a single histograms
      hist = (TH1*) o["hAsymVsOscillPhase_Y45_" + sAsymType];
      hist->GetListOfFunctions()->Add(graphErrs->Clone(), "p");

      utils::UpdateLimitsFromGraphs(hist, 2);
   }

   utils::UpdateLimitsFromGraphs(&hAsymVsOscillPhase_X45, 2);
   utils::UpdateLimitsFromGraphs(&hAsymVsOscillPhase_Y45, 2);
}
