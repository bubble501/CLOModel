#include "WaterfallCalcThread.h"
#include "Private/WaterfallCalcThread_p.h"
WaterfallCalcThreadPrivate::WaterfallCalcThreadPrivate(WaterfallCalcThread *q)
    :AbstrAsyncThreadPrivate(q)
{}
WaterfallCalcThread::WaterfallCalcThread(int Identity, QObject *parent)
    : WaterfallCalcThread(new WaterfallCalcThreadPrivate(this), Identity, parent)
{}
WaterfallCalcThread::WaterfallCalcThread(WaterfallCalcThreadPrivate* d, int ID, QObject* parent)
    : TemplAsyncThread<Waterfall>(d, ID, parent)
{}
void WaterfallCalcThread::SetWaterfall(const Waterfall& a)
{
    Q_D(WaterfallCalcThread);
    d->LocalFall = a;
}

void WaterfallCalcThread::EmitFromAnonim()
{
    Q_D(WaterfallCalcThread);
    emit Calculated(d->Identifier, d->LocalFall);
}

void WaterfallCalcThread::run()
{
    Q_D(WaterfallCalcThread);
    if (d->LocalFall.CalculateTranchesCashFlows())
        emit AnonimCalculated(d->Identifier);
	else
        emit ErrorCalculation(d->Identifier);
	exec();
}