#include "WaterfallCalcThread.h"
WaterfallCalcThread::WaterfallCalcThread(int ID, QObject* parent/* =0 */)
	:QThread(parent)
	,Identifier(ID)
{
	connect(this,SIGNAL(finished()),this,SLOT(deleteLater()));
}
void WaterfallCalcThread::run(){
	LocalFall.CalculateTranchesCashFlows();
	emit Calculated(Identifier,LocalFall);
	exec();
}