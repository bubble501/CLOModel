#include "ApplyFlowThread.h"
#include "Private/ApplyFlowThread_p.h"
ApplyFlowThread::ApplyFlowThread(int ID, QObject* parent)
	:TemplAsyncThread<MtgCashFlow>(ID,parent)
{}

void ApplyFlowThread::SetBaseFlow(const MtgCashFlow& a)
{
    Q_D(ApplyFlowThread);
    d->BaseFlow = &a;
}

void ApplyFlowThread::SetAssumption(const AssumptionSet& a)
{
    Q_D(ApplyFlowThread);
    d->AssumptionsToApply = a;
}

void ApplyFlowThread::run()
{
    Q_D(ApplyFlowThread);
    *castResult() = d->BaseFlow->ApplyScenario(
        d->AssumptionsToApply.GetCPRscenario()
        , d->AssumptionsToApply.GetCDRscenario()
        , d->AssumptionsToApply.GetLSscenario()
	);
    emit AnonimCalculated(d->Identifier);
	exec();
}

void ApplyFlowThread::EmitFromAnonim()
{
    Q_D(ApplyFlowThread);
    emit Calculated(d->Identifier, *castResult());
}
