#include "StressThread.h"
#include "Waterfall.h"
StressThread::StressThread(int IDx,int IDy, const QString& Xs,const QString& Ys,const QString& Cp,const QList<Mortgage*>& Mtgs,const Waterfall& Stct,const QDate& StDt,StressTest::StressVariability Xv,StressTest::StressVariability Yv,QObject* parent)
	:QThread(parent)
{
	Worker=new StressWorker( IDx, IDy,  Xs, Ys, Cp, Mtgs, Stct, StDt, Xv, Yv,this);
	connect(Worker,SIGNAL(ScenarioCalculated(int,int,Waterfall)),this,SIGNAL(ScenarioCalculated(int,int,Waterfall)));
	connect(this,SIGNAL(finished()),this,SLOT(deleteLater()));
}
StressThread::StressThread(int IDx, int IDy, const QString& Xs, const QString& Ys, const QString& Cp, const MtgCashFlow& Mtgs, const Waterfall& Stct, const QDate& StDt, StressTest::StressVariability Xv, StressTest::StressVariability Yv, QObject* parent)
:QThread(parent) {
	Worker = new StressWorker(IDx, IDy, Xs, Ys, Cp, Mtgs, Stct, StDt, Xv, Yv, this);
	connect(Worker, SIGNAL(ScenarioCalculated(int, int, Waterfall)), this, SIGNAL(ScenarioCalculated(int, int, Waterfall)));
	connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}
void StressThread::run(){
	Worker->Work();
	exec();
}
