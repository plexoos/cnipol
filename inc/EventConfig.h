#ifndef EventConfig_h
#define EventConfig_h

#include <stdio.h>
#include <vector>

#include "TObject.h"
#include "TRandom.h"

#include "rpoldata.h"
#include "rhicpol.h"

#include "AsymGlobals.h"
#include "AsymAnaInfo.h"
#include "MeasInfo.h"
#include "AnaMeasResult.h"

#include "Calibrator.h"

class MseMeasInfoX;


/** */
class EventConfig : public TObject
{
public:

   TRandom       *fRandom;
   MeasInfo      *fMeasInfo;
   AsymAnaInfo   *fAnaInfo;
   Calibrator    *fCalibrator;
   AnaMeasResult *fAnaMeasResult;
   MseMeasInfoX  *fMseMeasInfoX;  //!  it is better to leave this one out as
                                  // cint cannot properly generate a streamer
                                  // for it due to a conflict with mysql++
                                  // class definitions

public:

   EventConfig();
   ~EventConfig();

   MeasInfo*      GetMeasInfo() const;
   AsymAnaInfo*   GetAnaInfo() const;
   Calibrator*    GetCalibrator() const;
   AnaMeasResult* GetAnaMeasResult() const;
   MseMeasInfoX*  GetMseMeasInfoX() const;

   void        Print(const Option_t* opt="") const;
   void        PrintAsPhp(FILE *f=stdout) const;
   void        PrintAsConfig(FILE *f=stdout) const;
   std::string GetSignature() const;

   bool operator<(const EventConfig &mc) const;

   ClassDef(EventConfig, 5)
};

#endif
