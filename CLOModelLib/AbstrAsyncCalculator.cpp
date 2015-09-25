#include "AbstrAsyncCalculator.h"
#include "Private/AbstrAsyncCalculator_p.h"
#include <QThread>
DEFINE_PUBLIC_QOBJECT_COMMONS(AbstrAsyncCalculator)
AbstrAsyncCalculatorPrivate::~AbstrAsyncCalculatorPrivate(){}
AbstrAsyncCalculatorPrivate::AbstrAsyncCalculatorPrivate(AbstrAsyncCalculator *q)
	:BackwardInterfacePrivate(q)
{}

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
    d->m_ContinueCalculation = false;
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

QHash<qint32, std::shared_ptr<void> >& AbstrAsyncCalculator::getResultVoid()
{
    Q_D(AbstrAsyncCalculator);
    return d->m_Result;
}

const std::shared_ptr<void> AbstrAsyncCalculator::getResultVoid(qint32 key) const
{
    Q_D(const AbstrAsyncCalculator);
    return d->m_Result.value(key, std::shared_ptr<void>());
}

const QHash<qint32, std::shared_ptr<void> >& AbstrAsyncCalculator::getResultVoid() const
{
    Q_D(const AbstrAsyncCalculator);
    return d->m_Result;
}

void AbstrAsyncCalculator::insertResult(qint32 Key, void* val)
{
    Q_D(AbstrAsyncCalculator);
    d->m_Result.insert(Key, std::shared_ptr<void>(val));
}

bool AbstrAsyncCalculator::ContinueCalculation() const
{
    Q_D(const AbstrAsyncCalculator);
    return  d->m_ContinueCalculation;
}

void AbstrAsyncCalculator::ContinueCalculation(bool val)
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

QList<qint32> AbstrAsyncCalculator::GetResultsKeys() const
{
    Q_D(const AbstrAsyncCalculator);
    return d->m_Result.keys();
}

void AbstrAsyncCalculator::SendPctSignal(double a)
{
    emit ProgressPct(static_cast<int>(a*100.0));
}

void AbstrAsyncCalculator::StopCalculation()
{
    Q_D(AbstrAsyncCalculator);
    d->m_ContinueCalculation = false; 
    emit Stopped();
}
