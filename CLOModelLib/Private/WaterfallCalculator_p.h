#ifndef WaterfallCalculator_p_h__
#define WaterfallCalculator_p_h__

#include "Private/AbstrAsyncCalculator_p.h"
#include "WaterfallCalculator.h"
#include <QHash>
class WaterfallCalculatorPrivate : public AbstrAsyncCalculatorPrivate
{
    DECLARE_PRIVATE_COMMONS(WaterfallCalculator)
    DECLARE_PRIVATE_COMMONS_DATASTREAM(WaterfallCalculator)
public:
    QHash<qint32, QString > m_CascadesPath;
    void clearTempDir();
};





#endif // WaterfallCalculator_p_h__