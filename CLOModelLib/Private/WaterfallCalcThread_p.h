#ifndef WaterfallCalcThread_p_h__
#define WaterfallCalcThread_p_h__
#include "Private/AbstrAsyncThread_p.h"
#include "WaterfallCalcThread.h"
#include "Waterfall.h"
class WaterfallCalcThreadPrivate : public AbstrAsyncThreadPrivate
{
    DECLARE_PRIVATE_COMMONS(WaterfallCalcThread)
public:
    Waterfall LocalFall;
};
#endif // WaterfallCalcThread_p_h__