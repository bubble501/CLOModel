#include "AbstrAsyncCalculator.h"
#include "Private/AbstrAsyncCalculator_p.h"
#include "Private/InternalItems.h"
#include <QThread>
#include <QDir>
DEFINE_PUBLIC_QOBJECT_COMMONS(AbstrAsyncCalculator)
AbstrAsyncCalculatorPrivate::~AbstrAsyncCalculatorPrivate(){}
AbstrAsyncCalculator::~AbstrAsyncCalculator() {}
AbstrAsyncCalculatorPrivate::AbstrAsyncCalculatorPrivate(AbstrAsyncCalculator *q)
	:BackwardInterfacePrivate(q)
    , m_resultsFile(TEMP_FILES_DIR)
{
    ENSURE_DIR_EXIST(TEMP_FILES_DIR);
    m_resultsFile.open();
    m_resultsMap.setDevice(&m_resultsFile);
}

quint8 AbstrAsyncCalculator::operativity() const
{
    Q_D(const AbstrAsyncCalculator);
    return d->m_operativity;
}

void AbstrAsyncCalculator::setOperativity(quint8 val)
{
    Q_ASSERT(val <= 100);
    RETURN_WHEN_RUNNING(true,)
    Q_D(AbstrAsyncCalculator);
    d->m_operativity = val;
}

AbstrAsyncCalculator::AbstrAsyncCalculator(AbstrAsyncCalculatorPrivate *d, QObject* parent)
	:QObject(parent)
    ,BackwardInterface(d)
{
    d->m_SequentialComputation = false;
    d->BeesReturned = 0;
    setContinueCalculation(false);
    connect(this, &AbstrAsyncCalculator::Progress, this, &AbstrAsyncCalculator::SendPctSignal);
}



QHash<qint32, QPointer<QObject> >& AbstrAsyncCalculator::getThreadPool()
{
    Q_D(AbstrAsyncCalculator);
    return d->m_ThreadPool;
}

const QHash<qint32, QPointer<QObject> >& AbstrAsyncCalculator::getThreadPool() const
{
    Q_D(const AbstrAsyncCalculator);
    return d->m_ThreadPool;
}

const QSet<qint32>& AbstrAsyncCalculator::getBeeSent() const
{
    Q_D(const AbstrAsyncCalculator);
    return d->BeesSent;
}

QSet<qint32>& AbstrAsyncCalculator::getBeeSent()
{
    Q_D(AbstrAsyncCalculator);
    return d->BeesSent;
}

const qint32& AbstrAsyncCalculator::getBeesReturned() const
{
    Q_D(const AbstrAsyncCalculator);
    return d->BeesReturned;
}

qint32& AbstrAsyncCalculator::getBeesReturned()
{
    Q_D(AbstrAsyncCalculator);
    return d->BeesReturned;
}


bool AbstrAsyncCalculator::ContinueCalculation() const
{
    Q_D(const AbstrAsyncCalculator);
    return  d->m_ContinueCalculation;
}

void AbstrAsyncCalculator::setContinueCalculation(bool val)
{
    Q_D(AbstrAsyncCalculator);
    d->m_ContinueCalculation = val;
}

int AbstrAsyncCalculator::availableThreads() const
{
    Q_D(const AbstrAsyncCalculator);
    return static_cast<int>(
        static_cast<double>(QThread::idealThreadCount()) 
        * static_cast<double>(d->m_operativity) 
        / 100.0
        );
}


MemoryMappedDevice& AbstrAsyncCalculator::getDevice()
{
    Q_D(AbstrAsyncCalculator);
    return d->m_resultsMap;
}

const MemoryMappedDevice& AbstrAsyncCalculator::getDevice() const
{
    Q_D(const AbstrAsyncCalculator);
    return d->m_resultsMap;
}

void AbstrAsyncCalculator::SetSequentialComputation(bool a)
{
    Q_D(AbstrAsyncCalculator);
    d->m_SequentialComputation = a;
}

bool AbstrAsyncCalculator::GetSequentialComputation() const
{
    Q_D(const AbstrAsyncCalculator);
    return d->m_SequentialComputation;
}

QList<qint32> AbstrAsyncCalculator::GetResultKeys() const
{
    Q_D(const AbstrAsyncCalculator);
    return d->m_resultsMap.keys();
}

const QMultiHash<int, QString>& AbstrAsyncCalculator::errors() const
{
    Q_D(const AbstrAsyncCalculator);
    return d->m_errors;
}

void AbstrAsyncCalculator::clearErrors()
{
    Q_D(AbstrAsyncCalculator);
    d->m_errors.clear();
}

QString AbstrAsyncCalculator::allErrors() const
{
    Q_D(const AbstrAsyncCalculator);

    QString result;
    for (auto i = d->m_errors.constBegin(); i != d->m_errors.constEnd(); ++i) {
        if (i != d->m_errors.constBegin())
            result += '\n';
        result += i.value();
    }
    return result;
}

void AbstrAsyncCalculator::SendPctSignal(double a)
{
    emit ProgressPct(static_cast<int>(a*100.0));
}

void AbstrAsyncCalculator::addError(int id, QString err)
{
    Q_D(AbstrAsyncCalculator);
    d->m_errors.insert(id,err);
}

void AbstrAsyncCalculator::StopCalculation()
{
    setContinueCalculation(false); 
    emit Stopped();
}
