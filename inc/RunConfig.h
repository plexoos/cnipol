
#ifndef RunConfig_h
#define RunConfig_h

#include <set>
#include <string>

#include "TColor.h"
#include "TObject.h"
#include "TAttMarker.h"

#include "AsymCommon.h"
#include "Target.h"


class RunConfig : public TObject
{
private:

public:
   PolarimeterIdSet         fPolarimeters;
   TargetSet                fTargets;
   std::set<EMeasType>      fMeasTypes;
   std::set<ETargetOrient>  fTargetOrients;
   BeamIdSet                fBeams;
   std::set<ERingId>        fRings;
   BeamEnergySet            fBeamEnergies;
   SpinStateSet             fSpinStates;
   AsymTypeSet              fAsymTypes; //!

   RunConfig();
   ~RunConfig();

   static std::string AsString(EPolarimeterId polId);
   static std::string AsString(EMeasType measType);
   static std::string AsString(ETargetOrient targetOrient);
   static std::string AsString(EBeamId beamId);
   static std::string AsString(ERingId ringId);
   static std::string AsString(EBeamEnergy beamEnergy);
   static std::string AsString(ESpinState spinState);
   static std::string AsString(EAsymType asymType);
   static UShort_t    AsIndex(ESpinState spinState);
   static Color_t     AsColor(EBeamId beamId);
   static Color_t     AsColor(ERingId ringId);
   static Color_t     AsColor(EPolarimeterId polId);
   static Color_t     AsColor(ESpinState spin);
   static Color_t     AsColor(EAsymType asymType);
   static Color_t     AsColor(UShort_t chId);
   static Color_t     DetAsColor(int det);
   static TAttMarker  AsMarker(ETargetOrient targetOrient, EPolarimeterId polId);

   static EBeamId     GetBeamId(EPolarimeterId polId);
   static ERingId     GetRingId(EPolarimeterId polId);
   static EStreamId   GetStreamId(EPolarimeterId polId);

   static UShort_t    GetDetectorId(UShort_t chId);
   static UShort_t    GetDetectorChannelId(UShort_t chId);

   void SetBeamEnergies(BeamEnergySet beamEnergies);

    ClassDef(RunConfig, 0);
};

#endif
