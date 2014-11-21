#include "ChannelData.h"

ClassImp(ChannelData)


/** Default constructor. */
ChannelData::ChannelData() : TObject(), fAmpltd(UCHAR_MAX), fIntgrl(UCHAR_MAX),
   fTdc(UCHAR_MAX), fTdcAMax(UCHAR_MAX)
{
}


/** Default destructor. */
ChannelData::~ChannelData()
{
}


ChannelData::ChannelData(UChar_t ampltd, UChar_t intgrl, UChar_t tdc, UChar_t tdcMax) : TObject(),
   fAmpltd(ampltd), fIntgrl(intgrl), fTdc(tdc), fTdcAMax(tdcMax)
{
}


/** */
void ChannelData::Print(const Option_t* opt) const
{
   printf("%12d %12d %12d %12d", fAmpltd, fTdc, fIntgrl, fTdcAMax);
}
