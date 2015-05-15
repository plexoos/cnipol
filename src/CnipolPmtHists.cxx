#include "CnipolPmtHists.h"


ClassImp(CnipolPmtHists)

using namespace std;

/** Default constructor. */
CnipolPmtHists::CnipolPmtHists() : DrawObjContainer()
{
   BookHists();
}


CnipolPmtHists::CnipolPmtHists(TDirectory *dir) : DrawObjContainer(dir)
{
   BookHists();
}


/** Default destructor. */
CnipolPmtHists::~CnipolPmtHists()
{
}


/** */
void CnipolPmtHists::BookHists()
{
   char hName[256];

   fDir->cd();

   sprintf(hName, "hChVsDelim");
   o[hName] = new TH2I(hName, hName, 1, 0, 1, N_PMT_CHANNELS, FIRST_PMT_CHANNEL-0.5, FIRST_PMT_CHANNEL-0.5+N_PMT_CHANNELS);
   ((TH1*) o[hName])->SetOption("colz LOGZ");
   ((TH1*) o[hName])->SetTitle("; Time, s; Channel Id;");

   for (int iChId = FIRST_PMT_CHANNEL; iChId < FIRST_PMT_CHANNEL + N_PMT_CHANNELS; iChId++)
   {
      string sChId("  ");
      sprintf(&sChId[0], "%02d", iChId);

      sprintf(hName, "hTdc_ch%02d", iChId);
      o[hName] = new TH1I(hName, hName, 100, 0, 100);
      ((TH1*) o[hName])->SetOption("hist");
      ((TH1*) o[hName])->SetTitle("; TDC; Events;");

      sprintf(hName, "hAdcAmpltd_ch%02d", iChId);
      o[hName] = new TH1I(hName, hName, 255, 0, 255);
      ((TH1*) o[hName])->SetOption("hist NOIMG");
      ((TH1*) o[hName])->SetTitle("; Amplitude, ADC; Events;");

      sprintf(hName, "hTvsA_ch%02d", iChId);
      o[hName] = new TH2I(hName, hName, 255, 0, 255, 90, 0, 90);
      ((TH1*) o[hName])->SetOption("colz LOGZ NOIMG");
      ((TH1*) o[hName])->SetTitle("; Amplitude, ADC; TDC;");

      sprintf(hName, "hTvsI_ch%02d", iChId);
      o[hName] = new TH2I(hName, hName, 255, 0, 255, 90, 0, 90);
      ((TH1*) o[hName])->SetOption("colz LOGZ NOIMG");
      ((TH1*) o[hName])->SetTitle("; Integral, ADC; TDC;");
   }
}


/** */
void CnipolPmtHists::PreFill()
{
   TH2I *h = (TH2I*) o["hChVsDelim"];
   TAxis *axis = h->GetYaxis();
   h->SetBins(
      gNDelimeters*10, 0, gNDelimeters,
      axis->GetNbins(), axis->GetXmin(), axis->GetXmax()
      );
}


/** */
void CnipolPmtHists::Fill(ChannelEvent *ch)
{
   UChar_t chId = ch->GetChannelId();

   string sChId("  ");
   sprintf(&sChId[0], "%02d", chId);

   ((TH1*) o["hAdcAmpltd_ch" + sChId]) -> Fill(ch->GetAmpltd());
   ((TH1*) o["hTdc_ch"       + sChId]) -> Fill(ch->GetTdc());
   ((TH1*) o["hTvsA_ch"      + sChId]) -> Fill(ch->GetAmpltd(), ch->GetTdc());
   ((TH1*) o["hTvsI_ch"      + sChId]) -> Fill(ch->GetIntgrl(), ch->GetTdc());

   Double_t time = ch->GetRevolutionId() / (double)RHIC_REVOLUTION_FREQ;
   ((TH2*) o["hChVsDelim"])->Fill(time, chId);
}


/** */
void CnipolPmtHists::PostFill()
{
   Info("PostFill", "Starting...");

   fDir->cd();
   TH1D *px = ((TH2I*) o["hChVsDelim"])->ProjectionX();
   o[px->GetName()] = px;
   TH1D *py = ((TH2I*) o["hChVsDelim"])->ProjectionY();
   o[py->GetName()] = py;

   for (int iChId=N_SILICON_CHANNELS+1; iChId<=N_SILICON_CHANNELS+4; iChId++)
   {
      string sChId("  ");
      sprintf(&sChId[0], "%02d", iChId);

      //sprintf(hName, "hTdc_ch%02d", iChId);
      TFitResultPtr fitres = ((TH1*) o["hTdc_ch"+sChId])->Fit("gaus", "I M S R");

      if (fitres.Get()) {
         //chi2Ndf  = fitres->Ndf() > 0 ? fitres->Chi2()/fitres->Ndf() : -1;
         //sigma    = fitres->Value(0);
         if (iChId == 73) {
            gAnaMeasResult->fPmtV1T0    = fitres->Value(1);
            gAnaMeasResult->fPmtV1T0Err = fitres->FitResult::Error(1);
         } 

         if (iChId == 75) {
            gAnaMeasResult->fPmtS1T0    = fitres->Value(1);
            gAnaMeasResult->fPmtS1T0Err = fitres->FitResult::Error(1);
         } 
         //mean2    = fitres->Value(2);
         //mean2Err = fitres->FitResult::Error(2);
      }
   }
}
