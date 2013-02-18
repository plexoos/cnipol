/*****************************************************************************
 *                                                                           *
 *                                                                           *
 *****************************************************************************/


#ifndef CnipolAlphaHists_h
#define CnipolAlphaHists_h

#include "TClass.h"
#include "TDirectoryFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TSystem.h"

#include "AsymHeader.h"

#include "DrawObjContainer.h"
#include "ChannelEvent.h"


/** */
class CnipolAlphaHists : public DrawObjContainer
{

public:

   CnipolAlphaHists();
   CnipolAlphaHists(TDirectory *dir);
   virtual ~CnipolAlphaHists();

   void BookHists();
   void Fill(ChannelEvent *ch);
   void FillPassOne(ChannelEvent *ch);
   void FillPreProcess(ChannelEvent *ch);
   void PostFill();
   void PostFillPassOne(DrawObjContainer *oc=0);

   ClassDef(CnipolAlphaHists, 1)
};

#endif
