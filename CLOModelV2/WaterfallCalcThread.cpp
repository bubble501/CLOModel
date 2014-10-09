#include "WaterfallCalcThread.h"
WaterfallCalcThread::WaterfallCalcThread(int ID, QObject* parent)
	:QThread(parent)
	,Identifier(ID)
{
	connect(this,SIGNAL(finished()),this,SLOT(deleteLater()));
}
void WaterfallCalcThread::run(){
	if(LocalFall.CalculateTranchesCashFlows())
		emit Calculated(Identifier,LocalFall);
	else
		emit ErrorInCalculation(Identifier);
	exec();
}