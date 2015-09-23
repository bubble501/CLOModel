#ifndef AbstrAsyncCalculator_p_h__
#define AbstrAsyncCalculator_p_h__

#include "AbstrAsyncCalculator.h"
#include "Private/BackwardCompatibilityInterface_p.h"
#include <QSet>
#include <QHash>
#include <QPointer>
#include <atomic>
class AbstrAsyncCalculatorPrivate : public BackwardInterfacePrivate
{
    DECLARE_PRIVATE_COMMONS(AbstrAsyncCalculator)
    DECLARE_PRIVATE_COMMONS_DATASTREAM(AbstrAsyncCalculator)
public:
    std::atomic_bool m_ContinueCalculation;
    QHash<qint32, QPointer<QObject> > m_ThreadPool;
    QHash<qint32, std::shared_ptr<void> > m_Result;
    QSet<qint32> BeesSent;
    qint32 BeesReturned;
    bool m_SequentialComputation;
    quint8 m_operativity;
    
};
#endif // AbstrAsyncCalculator_p_h__