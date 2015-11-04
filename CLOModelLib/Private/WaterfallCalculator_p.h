#ifndef WaterfallCalculator_p_h__
#define WaterfallCalculator_p_h__

#include "Private/AbstrAsyncCalculator_p.h"
#include "WaterfallCalculator.h"
#include "MemoryMappedDevice.h"
class WaterfallCalculatorPrivate : public AbstrAsyncCalculatorPrivate
{
    DECLARE_PRIVATE_COMMONS(WaterfallCalculator)
    DECLARE_PRIVATE_COMMONS_DATASTREAM(WaterfallCalculator)
public:
    MemoryMappedDevice m_cascades;
    QTemporaryFile m_cascadesFile;
};





#endif // WaterfallCalculator_p_h__