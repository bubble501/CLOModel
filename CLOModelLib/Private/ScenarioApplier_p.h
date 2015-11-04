#ifndef ScenarioApplier_p_h__
#define ScenarioApplier_p_h__

#include "Private/AbstrAsyncCalculator_p.h"
#include "ScenarioApplier.h"
#include "MemoryMappedDevice.h"
#include <QTemporaryFile>
class ScenarioApplierPrivate : public AbstrAsyncCalculatorPrivate
{
    DECLARE_PRIVATE_COMMONS(ScenarioApplier)
    DECLARE_PRIVATE_COMMONS_DATASTREAM(ScenarioApplier)
public:
    MtgCashFlow m_BaseFlows;
    MemoryMappedDevice m_Scenarios;
    QTemporaryFile m_ScenariosFile;
};
#endif // ScenarioApplier_p_h__