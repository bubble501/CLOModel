#ifndef ScenarioApplier_p_h__
#define ScenarioApplier_p_h__

#include "Private/AbstrAsyncCalculator_p.h"
#include "ScenarioApplier.h"
#include <QHash>

class ScenarioApplierPrivate : public AbstrAsyncCalculatorPrivate
{
    DECLARE_PRIVATE_COMMONS(ScenarioApplier)
    DECLARE_PRIVATE_COMMONS_DATASTREAM(ScenarioApplier)
public:
    MtgCashFlow m_BaseFlows;
    QHash<qint32, std::shared_ptr<AssumptionSet> > m_Scenarios;
};
#endif // ScenarioApplier_p_h__