//  Asymmetry Analysis of RHIC pC Polarimeter
//  Error/Anomaly Finding Routine
//  file name :   AsymErrorDetector.cc
// 
//  Author    :   Itaru Nakagawa
//  Creation  :   08/01/2006
//                

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <iostream.h>
#include "TMinuit.h"
#include "TString.h"
#include "TMath.h"
#include "rhicpol.h"
#include "rpoldata.h"
#include "Asym.h"
#include "AsymROOT.h"
#include "WeightedMean.h"
#include "AsymErrorDetector.h"
#include "AsymCalc.h"

StructBunchCheck bnchchk;
StructStripCheck strpchk;



//
// Class name  : 
// Method name : int InvariantMassCorrelation()
//
// Description : Check the Mass vs. 12C Kinetic Enegy Correlation. Apply linear fit on
//             : Mass .vs Energy scatter plot and record resulting slops for all strips
// Input       : int st
// Return      : 
//
int 
InvariantMassCorrelation(int st){

  char htitle[100],histname[100];

  // Function for Fitting
  TF1 * g1 = new TF1("g1","gaus",5,16);
  TF1 * f1 = new TF1("f1","pol1",200,1000);
  f1->SetLineColor(2);

  // Mass vs. Energy correlation
  sprintf(histname,"mass_vs_e_ecut_st%d",st);
  mass_vs_e_ecut[st]->Write();
  TH2F * hslice = (TH2F*) gDirectory->Get(histname);
  hslice->SetName("hslice");

  // slice histogram for fit
  hslice->FitSlicesY(g1);

  TH1D *hslice_1 = (TH1D*)gDirectory->Get("hslice_1");
  sprintf(histname,"mass_vs_energy_corr_st%d",st);
  hslice_1->SetName(histname);
  sprintf(htitle,"%8.3f:Invariant Mass vs. Energy Correlation Fit (Str%d)",runinfo.RUNID,st+1);
  hslice_1->SetTitle(htitle);
  hslice_1->GetXaxis()->SetTitle("12C Kinetic Energy [keV]");
  hslice_1->GetYaxis()->SetTitle("Invariant Mass [GeV]");
  hslice_1->Fit("f1","Q");

  //Get Fitting results
  for (int j=0;j<2; j++) {
    strpchk.ecorr.p[j][st]    = f1->GetParameter(j);
    strpchk.ecorr.perr[j][st] = strpchk.ecorr.p[j][st] != 0 ? f1->GetParError(j) : 0;
  }

  // Delete unnecessary histograms generated by FitSliceY();
  TH1D *hslice_0 = (TH1D*)gDirectory->Get("hslice_0");
  TH1D *hslice_2 = (TH1D*)gDirectory->Get("hslice_2");
  TH1D *hslice_chi2 = (TH1D*)gDirectory->Get("hslice_chi2");
  hslice_0->Delete(); hslice_2->Delete(); hslice_chi2->Delete();

  // Make graph of p1 paramter as a function of strip number when strip number is the last one
  if (st==NSTRIP-1) {
    float strip[NSTRIP],ex[NSTRIP];
    for (int k=0;k<NSTRIP;k++) {strip[k]=k+1;ex[k]=0;}

    mass_e_correlation_strip = new TGraphErrors(NSTRIP, strip, strpchk.ecorr.p[1], ex, strpchk.ecorr.perr[1]);
    sprintf(htitle,"Run%8.3f : P[1] distribution for Mass vs. Energy Correlation", runinfo.RUNID);
    mass_e_correlation_strip -> SetTitle(htitle);
    mass_e_correlation_strip -> GetXaxis()->SetTitle("Strip Number");
    mass_e_correlation_strip -> GetYaxis()->SetTitle("slope [GeV/keV]");
    mass_e_correlation_strip -> SetMarkerStyle(20);
    mass_e_correlation_strip -> SetMarkerColor(2);
    TLine * lp = new TLine(0, strpchk.p1.allowance, NSTRIP+1, strpchk.p1.allowance);
    TLine * ln = new TLine(0, strpchk.p1.allowance, NSTRIP+1, strpchk.p1.allowance);
    lp -> SetLineStyle(2);
    ln -> SetLineStyle(2);
    mass_e_correlation_strip -> GetListOfFunctions()->Add(lp);
    mass_e_correlation_strip -> GetListOfFunctions()->Add(ln);

  }

  return 0;

}


//
// Class name  : 
// Method name : BananaFit()
//
// Description : fit banana with kinematic function. This routine is incomplete.
//             : 1. fix hard corded runconst.E2T consntant 1459.43. 
//             : 2. delete hbananan_1 histograms
// Input       : int st
// Return      : 
//
void 
BananaFit(int st){

  TF1 * functof = new TF1("functof", "1459.43/sqrt(x)",200,1500);

  char hname[100];
  sprintf(hname,"t_vs_e_st%d",st);
  t_vs_e[st]->GetListOfFunctions()->Add(functof);
  t_vs_e[st]->Write();
  TH2F * hbanana = (TH2F*) gDirectory->Get(hname);
  hbanana->SetName("hbanana");

  // Get centers of banana
  hbanana->FitSlicesY();

  TH1D *hbanana_1 = (TH1D*) gDirectory->Get("hbanana_1");
  sprintf(hname,"banana_center_st%d",st);
  hbanana_1->SetName(hname);

  // Delete unnecessary histograms
  TH1D *hbanana_0    = (TH1D*) gDirectory->Get("hbanana_0");
  TH1D *hbanana_2    = (TH1D*) gDirectory->Get("hbanana_2");
  TH1D *hbanana_chi2 = (TH1D*) gDirectory->Get("hbanana_chi2");
  hbanana_0->Delete();  hbanana_2->Delete();  hbanana_chi2->Delete();

  return;
};



//
// Class name  : 
// Method name : StripAnomaryDetector()
//
// Description : find suspicious strips
// Input       : 
// Return      : 
//
int
StripAnomalyDetector(){


  int counter=0;
  float sigma=0;
  strpchk.average[0] = WeightedMean(feedback.RMS,feedback.err,NSTRIP);
  TLine * strpave = new TLine(-0.5,strpchk.average[0],NSTRIP+0.5,strpchk.average[0]);
  strpave -> SetLineStyle(1);
  strpave -> SetLineColor(4);
  mass_sigma_vs_strip -> GetListOfFunctions() -> Add(strpave);

  
  TF1 *f1 = new TF1("f1","pol1",0,1000);

  strpchk.dev.max  = fabs(feedback.mdev[0]);
  strpchk.chi2.max = feedback.chi2[0];

  for (int i=0; i<NSTRIP; i++) {
    printf("Anomary Check for strip=%d ...\r",i);

    // t vs. Energy (this routine is incomplete)
    //    BananaFit(i);

    // MASS vs. Energy correlation
    InvariantMassCorrelation(i);
    if (!i) strpchk.p1.max   = fabs(strpchk.ecorr.p[1][i]);  // initialize max w/ strip 0
    if (fabs(strpchk.ecorr.p[1][i]) > strpchk.p1.max ) {
      strpchk.p1.max = fabs(strpchk.ecorr.p[1][i]);
      strpchk.p1.st  = i;
    }

    // Maximum devistion of peak from 12C_MASS
    if (fabs(feedback.mdev[i]) > strpchk.dev.max) {
      strpchk.dev.max  = fabs(feedback.mdev[i]);
      strpchk.dev.st   = i;
    }
    // Gaussian Mass fit Largest chi2
    if (feedback.chi2[i] > strpchk.chi2.max) {
      strpchk.chi2.max  = fabs(feedback.chi2[i]);
      strpchk.chi2.st   = i;
    }

    // Calculate one sigma of RMS distribution
    if (feedback.err[i]){
      sigma += (feedback.RMS[i]-strpchk.average[0])*(feedback.RMS[i]-strpchk.average[0])
	/feedback.err[i]/feedback.err[i];
      counter++;

    }

  }
  sigma=sqrt(sigma)/counter; 

  strpchk.p1.allowance   = errdet.MASS_ENERGY_CORR_ALLOWANCE;
  strpchk.dev.allowance  = errdet.MASS_DEV_ALLOWANCE;
  strpchk.chi2.allowance = errdet.MASS_CHI2_ALLOWANCE;
  float devlimit=strpchk.dev.allowance+strpchk.average[0];
  TLine * strpdev = new TLine(-0.5, devlimit, NSTRIP+0.5, devlimit);
  strpdev -> SetLineStyle(2);
  strpdev -> SetLineColor(2);
  mass_sigma_vs_strip -> GetListOfFunctions() -> Add(strpdev);

  float chi2limit={strpchk.chi2.allowance};
  TLine * strpchi2 = new TLine(-0.5, chi2limit, NSTRIP+0.5, chi2limit);
  strpchi2 -> SetLineStyle(2);
  strpchi2 -> SetLineColor(2);
  strpchi2 -> SetLineWidth(5);
  mass_chi2_vs_strip -> GetListOfFunctions() -> Add(strpchi2);


  // register and count suspicious strips 
  anal.anomaly.nstrip=0;
  for (int i=0;i<NSTRIP; i++) {
    if ((fabs(feedback.RMS[i])-strpchk.average[0]>strpchk.dev.allowance)   // large chi2 or deviation of peak position from average 
	||(feedback.chi2[i] > strpchk.chi2.allowance)           // chi2 of Gaussian fit on Inv. Mass peak
	||(fabs(strpchk.ecorr.p[1][i]) > strpchk.p1.allowance)) // mass vs. 12C kinetic energy correlation
      {
	anal.anomaly.st[anal.anomaly.nstrip]=i;
	++anal.anomaly.nstrip;
      }
  }
  
  UnrecognizedAnomaly(anal.anomaly.st,anal.anomaly.nstrip,runinfo.DisableStrip,runinfo.NDisableStrip,
		      anal.unrecog.anomaly.st, anal.unrecog.anomaly.nstrip);


  return 0;

};


//
// Class name  : 
// Method name : DrawLine()
//
// Description : DrawLines in TH1F histogram
//             : Assumes x=x0=x1, y0=0, y1=y1
// Input       : TH1F * h, float x, float y1, int color
// Return      : 
//
void 
DrawLine(TH1F * h, float x, float y1, int color, int lwidth){

  TLine * l = new TLine(x, 0, x, y1);
  l -> SetLineStyle(2);
  l -> SetLineColor(color);
  l -> SetLineWidth(lwidth);
  h -> GetListOfFunctions()->Add(l);

  return;
}

//
// Class name  : 
// Method name : DrawLine()
//
// Description : DrawLines in TH1F histogram
//             : Assumes  (x1,x2) y=y0=y1
// Input       : TH2F * h, float x0, float x1, float y, int color, int lstyle
// Return      : 
//
void 
DrawLine(TH2F * h, float x0, float x1, float y, int color, int lstyle, int lwidth){

  TLine * l = new TLine(x0, y, x1, y);
  l -> SetLineStyle(lstyle);
  l -> SetLineColor(color);
  l -> SetLineWidth(lwidth);
  h -> GetListOfFunctions()->Add(l);

  return;
}



//
// Class name  : 
// Method name : BunchAsymmetryGaussianFit()
//
// Description : find suspicious bunch thru Gaussian fit on bunch asymmetry histograms
//             : the bunches deviates more than sigma from fitted Gaussian width will be
//             : registered as problematic bunch ID
// Input       : TH1F * h1, TH2F * h2, float A[], float dA[]
// Return      : 
//
int 
BunchAsymmetryGaussianFit(TH1F * h1, TH2F * h2, float A[], float dA[]){

  TF1 * g = new TF1("g","gaus");
  g -> SetLineColor(2);

  h1->Fit("g","Q");
  float hight = g -> GetParameter(0);
  float mean  = g -> GetParameter(1);
  float sigma = g -> GetParameter(2);

  // get sigma from Gaussian fit and calculate allowance limit
  bnchchk.asym[0].allowance = mean - errdet.BUNCH_ASYM_SIGMA_ALLOWANCE*sigma;
  bnchchk.asym[1].allowance = mean + errdet.BUNCH_ASYM_SIGMA_ALLOWANCE*sigma;

  // draw lines to mean asymmetery vs. bunch histograms
  DrawLine(h2, -0.5, NBUNCH-0.5,  mean, 4, 4, 2);
  DrawLine(h2, -0.5, NBUNCH-0.5, -mean, 2, 2, 2);

  // axis titles
  h1 -> GetYaxis() -> SetTitle("Counts devided by statistical error");
  h1 -> GetXaxis() -> SetTitle("Raw Asymmetry");


  // local anamaly bunch array and counter
  struct StructBUNCH {
    float A[NBUNCH];
    int bunch[NBUNCH];
    int nbunch;
    float dev;
  } local;

  // Anomaly bunch finding
  char text[20];
  local.nbunch=0;
  for (int bid=0;bid<NBUNCH;bid++) {
    local.bunch[bid] = -999; // default not to appear in plots

    if ((fillpat[bid])&&(A[bid]!=-ASYM_DEFAULT)) {

      if (spinpat[bid] == 1)  local.dev =  fabs(A[bid] - mean);  
      if (spinpat[bid] == -1) local.dev =  fabs(A[bid] - (-1)*mean);  

      if (local.dev/dA[bid] > errdet.BUNCH_ASYM_SIGMA_ALLOWANCE) {
	
	local.bunch[local.nbunch] = bid;
	local.A[local.nbunch] = A[bid];
	local.nbunch++;
	printf(" WARNING: bunch # %d asym sigma %6.1f exeeds %6.1f limit from average\n", 
	       bid+1, local.dev/dA[bid], errdet.BUNCH_ASYM_SIGMA_ALLOWANCE);

	// comment in h2 histogram
	sprintf(text,"%6.1f sigma (%d)", local.dev/dA[bid],bid);
	TText * t = new TText(bid+2, A[bid], text);
	h2 -> GetListOfFunctions()->Add(t);
	
      }

    } // end-of-if(fillpat[bid])

  }// end-of-for(bid) loop


  if (local.nbunch){
    // global registration
    cout << "bofore registration: nbunch=" << local.bunch << endl;
    for (int i=0; i<local.nbunch; i++) cout << local.bunch[i] << " " ;
    cout << endl;
    RegisterAnomaly(local.bunch, local.nbunch, anal.anomaly.bunch, anal.anomaly.nbunch,
		    anal.anomaly.bunch, anal.anomaly.nbunch);
    cout << "after registration: nbunch=" << anal.anomaly.nbunch << endl;
    for (int i=0; i<anal.anomaly.nbunch; i++) cout << anal.anomaly.bunch[i] << " " ;
    cout << endl;

    // Superpose h2 histogram
    float bindex[local.nbunch];
    for (int i=0;i<local.nbunch;i++) bindex[i]=local.bunch[i];
    TGraph * gr = new TGraph(local.nbunch, bindex, local.A);
    gr -> SetMarkerStyle(24);
    gr -> SetMarkerSize(MSIZE);
    gr -> SetMarkerColor(3);

    // append suspicious bunch in h2 hitogram
    h2 -> GetListOfFunctions() -> Add(gr,"P");
  }

  return 0;

}

//
// Class name  : 
// Method name : BunchAsymmetryAnomaly()
//
// Description : find suspicious bunch thru Gaussian fit on bunch asymmetry histograms
//             : the bunches deviates more than sigma from fitted Gaussian width will be
//             : registered as problematic bunch ID
// Input       : 
// Return      : 
//
int 
BunchAsymmetryAnomaly(){

  printf("BunchAsymmetryAnomaly(): check for x90\n");
  BunchAsymmetryGaussianFit(asym_bunch_x90, asym_vs_bunch_x90, basym.Ax90[0], basym.Ax90[1]);
  printf("BunchAsymmetryAnomaly(): check for x45\n");
  BunchAsymmetryGaussianFit(asym_bunch_x45, asym_vs_bunch_x45, basym.Ax45[0], basym.Ax45[1]);
  printf("BunchAsymmetryAnomaly(): check for y45\n");
  BunchAsymmetryGaussianFit(asym_bunch_y45, asym_vs_bunch_y45, basym.Ay45[0], basym.Ay45[1]);


  return 0;

}


//
// Class name  : 
// Method name : BunchAnomaryDetector()
//
// Description : find suspicious bunch thru following two checks of bunch by bunch
//             : Asymmetry anomaly check
//             : counting rate anomaly check
// Input       : 
// Return      : 
//
int
BunchAnomalyDetector(){

  // Initiarize anomaly bunch counter
  anal.anomaly.nbunch=0;

  // Find anomaly bunches from unusual deviation from average asymmetry
  BunchAsymmetryAnomaly();

  // Find Hot bunches from counting rates per bunch
  HotBunchFinder();

  // check unrecognized anomaly
  UnrecognizedAnomaly(anal.anomaly.bunch, anal.anomaly.nbunch, runinfo.DisableBunch,runinfo.NDisableBunch,
		      anal.unrecog.anomaly.bunch, anal.unrecog.anomaly.nbunch);

  return 0;
}


//
// Class name  : 
// Method name : HotBunchFinder()
//
// Description : find hot bunch 
// Input       : 
// Return      : 
//
int
HotBunchFinder(){

  float NBcounts[NBUNCH];
  float err[NBUNCH], bindex[NBUNCH];
  float max, min;
  int init_flag=1;


  for (int bnch=0; bnch<NBUNCH; bnch++){

    // inistiarization
    bindex[bnch]=bnch; err[bnch]=1; NBcounts[bnch]=0; 

    // sum over detector yields
    for (int det=0; det<NDETECTOR; det++) { NBcounts[bnch] += Ncounts[det][bnch];}

    // calculate min and max range of the histogram
    if ((NBcounts[bnch])&&(init_flag) ) {min=NBcounts[bnch]; init_flag=0;}
    if ((NBcounts[bnch] < min)&&(!NBcounts[bnch])) min = NBcounts[bnch];
    if (max < NBcounts[bnch]) max = NBcounts[bnch];

  }

  // define rate distribution and fill the histogram
  char hname[100];
  sprintf(hname,"%8.3f : Rate Distribution / bunch", runinfo.RUNID);
  bunch_rate = new TH1F("bunch_rate",hname, 100, min*0.9, max*1.1);
  for (int bnch=0;bnch<NBUNCH;bnch++) { 
    if (NBcounts[bnch]) bunch_rate->Fill(NBcounts[bnch]);
  }

  // define rate vs. bunch plot 
  ErrDet->cd();
  TGraph * gr = new TGraph(NBUNCH, bindex, NBcounts);
  gr -> SetMarkerSize(MSIZE);
  gr -> SetMarkerStyle(20);
  gr -> SetMarkerColor(4);
  rate_vs_bunch = new TH2F("rate_vs_bunch", hname, NBUNCH, -0.5, NBUNCH+0.5, 50, min, max*1.3);
  rate_vs_bunch -> GetListOfFunctions() -> Add(gr,"P");
  rate_vs_bunch -> GetXaxis()->SetTitle("Bunch Number");
  rate_vs_bunch -> GetYaxis()->SetTitle("Yield/Bunch");

  // define gaussian function 
  TF1 * g1 = new TF1("g1","gaus");
  g1->SetLineColor(2);

  // apply gaussian fit on rate distribution
  bunch_rate->Fit(g1);
  
  // get mean from gaussian fit
  float ave = g1->GetParameter(1);
  DrawLine(rate_vs_bunch, -0.5, NBUNCH+0.5, ave, 1, 1, 1);

  // get sigma from Gaussian fit and calculate allowance limit
  float sigma=g1->GetParameter(2);
  bnchchk.rate.allowance = ave + errdet.BUNCH_RATE_SIGMA_ALLOWANCE*sigma;

  // draw lines to 1D and 2D histograms
  DrawLine(rate_vs_bunch, -0.5, NBUNCH+0.5, bnchchk.rate.allowance, 2, 2, 2);
  DrawLine(bunch_rate, bnchchk.rate.allowance, g1->GetParameter(0), 2, 2);

  // anomaly bunch registration
  char text[16]; 
  for (int bnch=0;bnch<NBUNCH;bnch++) {
    if (NBcounts[bnch] > bnchchk.rate.allowance) {
      anal.anomaly.bunch[anal.anomaly.nbunch] = bnch;
      anal.anomaly.nbunch++;
      printf("WARNING: bunch # %d yeild exeeds %6.1f sigma from average\n", bnch, bnchchk.rate.allowance);
      
      // comment in h2 histogram
      sprintf(text,"Bunch %d",bnch);
      TText * t = new TText(bnch+2, NBcounts[bnch], text);
      rate_vs_bunch -> GetListOfFunctions()->Add(t);

    }
  }

	

  return 0;
}



//
// Class name  : 
// Method name : RegisterAnomaly(float *x, int nx, int *y, int ny, int *z, int &nz)
//
// Description : converts float array x[] into integer and call RegisterAnomaly(int,...)
//             : 
// Input       : float x[], int nx, int y[], int ny,
// Return      : result of (x[]&y[]) -> array z[], and nz
//
int
RegisterAnomaly(float x[], int nx, int y[], int ny, int z[], int &nz){

  int X[nx];
  for (int i=0;i<nx;i++) X[i]=int(x[i]);
  RegisterAnomaly(X, nx, y, ny, z, nz);

  return 0;

}

//
// Class name  : 
// Method name : RegisterAnomaly(int *x, int nx, int *y, int ny, int *z, int &nz)
//
// Description : Check whether anomalies are recognized or not.
//             : Take AND of array x[] and y[], exclude double counting
// Input       : int x[], int nx, int y[], int ny,
// Return      : result of (x[]&y[]) -> array z[], and nz
//
int
RegisterAnomaly(int x[], int nx, int y[], int ny, int z[], int &nz){

  // if ny=0, then copy x[] -> z[]. the main loop doesn't work for ny=0
  if (!ny) {
    for (int i=0;i<nx;i++) z[i]=x[i];
    nz=nx;
    return 0;
  }

  // main loop
  nz=0; int J=0;
  for(int i=0; i<nx; i++) {

    for (int j=J; j<ny ; j++){
      if (y[j]<x[i]) {
	z[nz]=y[j]; nz++; J++; 
      } else if (y[j]==x[i]) {
	z[nz]=y[j]; nz++; J++; 
	break;
      }	else {
	z[nz]=x[i]; nz++;
	break;
      }
    } // end-of-for(j)

  }//end-of-for(i)

  // above loop doesn't precess the largest number in x[] or y[] array
  if (x[nx-1]>y[ny-1]) {z[nz]=x[nx-1]; nz++;}
  if (y[ny-1]>x[nx-1]) {z[nz]=y[ny-1]; nz++;}

  return 0;

}


//
// Class name  : 
// Method name : UnrecognizedAnomaly(int *x, int nx, int *y, int ny, int *z, int &nz){
//
// Description : Check whether anomalies are recognized or not.
//             : Take AND of array x[] and y[], returns false of the tests
// Input       : int *x, int nx, int *y, int ny,
// Return      : unrecongnized (strip/bunch) ID in array z, and nz
//
int
UnrecognizedAnomaly(int x[], int nx, int y[], int ny, int z[], int &nz){

  int match[nx];
  for (int i=0;i<nx;i++) match[i]=x[i];

  // Check for mathing between two arrays x[nx], y[ny]
  for (int i=0; i<ny; i++){
    for (int j=0; j<nx; j++){
      if (y[i]==match[j]){ match[j]=-1; break;}
    }
  }

  nz=0;
  for (int i=0;i<nx; i++) {
    if (match[i]!=-1) { z[nz]=match[i] ;nz++ ;}
  }


  return 0;

}



//
// Class name  : 
// Method name : checkForBadBunches()
//
// Description : check for bad bunches
// Input       : 
// Return      : 
//
void checkForBadBunches()
{

  // counter initiariztion
  anal.anomaly.nbunch=0;
  bnchchk.rate.allowance=errdet.BUNCH_RATE_SIGMA_ALLOWANCE;


	printf("checking for bad bunches\n");
	
	double avg;
	double sigma;
	for(int i=0;i<NDETECTOR;i++)
	{
		avg=0.;
		for(int j=0;j<120;j++)
		{
			avg+=Ncounts[i][j];
		}
		avg=avg/120.;
		
		sigma=0.;
		for(int j=0;j<120;j++)
		{
			sigma+=((Ncounts[i][j]-avg)*(Ncounts[i][j]-avg));
		}
		sigma=sigma/120.;
		sigma=sqrt(sigma);
		
		for(int j=0;j<120;j++)
		{
			if((Ncounts[i][j]-avg)> bnchchk.rate.allowance*sigma)
			{
			  anal.anomaly.bunch[anal.anomaly.nbunch]=j+1;
			  anal.anomaly.nbunch++;
			  printf("WARNING: bunch # %d has very many counts in detector # %d\n", j+1, i+1);

			}
		}
		
	}


	UnrecognizedAnomaly(anal.anomaly.bunch,anal.anomaly.nbunch,runinfo.DisableBunch,runinfo.NDisableBunch,
			    anal.unrecog.anomaly.bunch, anal.unrecog.anomaly.nbunch);


}


			
