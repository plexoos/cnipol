/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef DeadLayerCalibrator_h
#define DeadLayerCalibrator_h

#include <map>
#include <set>
#include <vector>
#include <float.h>
#include <math.h>

#include "TF1.h"
#include "TF2.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TMath.h"

#include "AsymHeader.h"

#include "Calibrator.h"


/**
 *
 */
class DeadLayerCalibrator : public Calibrator
{
public:


public:

   DeadLayerCalibrator();
   ~DeadLayerCalibrator();

   virtual void Calibrate(DrawObjContainer *c);
   TFitResultPtr Calibrate(TH1 *h, TH1D *hMeanTime);
   virtual void Print(const Option_t* opt="") const;

   static Double_t BananaFitFunc(Double_t *x, Double_t *par);
   static Double_t BananaFitFunc2(Double_t *x, Double_t *par);

   ClassDef(DeadLayerCalibrator, 1)
};

#endif