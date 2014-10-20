#include "WaterfallCalcThread.h"
WaterfallCalcThread::WaterfallCalcThread(int ID, QObject* parent)
	:TemplAsyncThread<Waterfall>(ID,parent)
{}
void WaterfallCalcThread::run(){
	if(LocalFall.CalculateTranchesCashFlows())
		emit AnonimCalculated(Identifier);
	else
		emit ErrorCalculation(Identifier);
	exec();
}