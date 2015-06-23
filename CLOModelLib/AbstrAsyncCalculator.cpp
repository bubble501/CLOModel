#include "AbstrAsyncCalculator.h"
#include "Private/AbstrAsyncCalculator_p.h"
DEFINE_PUBLIC_QOBJECT_COMMONS(AbstrAsyncCalculator)
AbstrAsyncCalculatorPrivate::AbstrAsyncCalculatorPrivate(AbstrAsyncCalculator *q)
	:BackwardInterfacePrivate(q)
{}

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

QHash<qint32, void*>& AbstrAsyncCalculator::getResultVoid()
{
    Q_D(AbstrAsyncCalculator);
    return d->m_Result;
}

const void* AbstrAsyncCalculator::getResultVoid(qint32 key) const
{
    Q_D(const AbstrAsyncCalculator);
    return d->m_Result.value(key,nullptr);
}

const QHash<qint32, void*>& AbstrAsyncCalculator::getResultVoid() const
{
    Q_D(const AbstrAsyncCalculator);
    return d->m_Result;
}

void AbstrAsyncCalculator::insertResult(qint32 Key, void* val)
{
    Q_D(AbstrAsyncCalculator);
    d->m_Result.insert(Key, val);
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
