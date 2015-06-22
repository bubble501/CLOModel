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
    d->m_ContinueCalculation = false;
    connect(this, &AbstrAsyncCalculator::Progress, this, &AbstrAsyncCalculator::SendPctSignal);
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
