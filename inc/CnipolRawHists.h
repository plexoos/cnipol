
#ifndef CnipolRawHists_h
#define CnipolRawHists_h

#include "TDirectoryFile.h"

#include "AsymHeader.h"

#include "ChannelEvent.h"
#include "DrawObjContainer.h"


/**
 * 
 */
class CnipolRawHists : public DrawObjContainer
{
protected:

   TH1 *fhTvsA_ch[N_SILICON_CHANNELS];        ///< 
   TH1 *fhTvsI_ch[N_SILICON_CHANNELS];
   TH1 *fhIvsA_ch[N_SILICON_CHANNELS];
   TH1 *fhTvsACumul_ch[N_SILICON_CHANNELS];
   TH1 *fhTvsA_ch_b[N_SILICON_CHANNELS][N_BUNCHES];
   TH1 *fhTvsACumul_ch_b[N_SILICON_CHANNELS][N_BUNCHES];
   TH1 *fhWfdCounts;

public:

   CnipolRawHists();
   CnipolRawHists(TDirectory *dir);
   ~CnipolRawHists();

   void BookHists();
   void FillPassOne(ChannelEvent *ch);
   void FillDerivedPassOne();
   void FillDerivedPassOne(DrawObjContainer &oc);

   TH1* GetHTvsA_ch(UShort_t chId);

   ClassDef(CnipolRawHists, 1)
};

#endif
