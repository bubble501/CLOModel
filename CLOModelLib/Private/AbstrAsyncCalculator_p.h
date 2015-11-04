#ifndef AbstrAsyncCalculator_p_h__
#define AbstrAsyncCalculator_p_h__

#include "AbstrAsyncCalculator.h"
#include "Private/BackwardCompatibilityInterface_p.h"
#include <QSet>
#include <QHash>
#include <QMultiHash>
#include <QPointer>
#include <atomic>
#include <QTemporaryFile>
#include "MemoryMappedDevice.h"
class AbstrAsyncCalculatorPrivate : public BackwardInterfacePrivate
{
    DECLARE_PRIVATE_COMMONS(AbstrAsyncCalculator)
    DECLARE_PRIVATE_COMMONS_DATASTREAM(AbstrAsyncCalculator)
public:
    std::atomic_bool m_ContinueCalculation;
    qint32 BeesReturned;
    bool m_SequentialComputation;
    quint8 m_operativity;
    QTemporaryFile m_resultsFile;
    MemoryMappedDevice m_resultsMap;
    QHash<qint32, QPointer<QObject> > m_ThreadPool;
    QSet<qint32> BeesSent;
    QMultiHash<qint32, QString> m_errors;

};
#endif // AbstrAsyncCalculator_p_h__