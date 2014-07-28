#ifndef StressThread_h__
#define StressThread_h__

#include <QThread>
#include <QHash>
#include "StressTest.h"
#include "StressWorker.h"
#include "Waterfall.h"
class Mortgage;
class StressThread : public QThread{
	Q_OBJECT
private:
	StressWorker* Worker;
public slots:
	void run();
	void stop(){exit(0);}
signals:
	void ScenarioCalculated(int,int,const Waterfall&);
public:
	~StressThread(){
		bool i=true;
	}
	StressThread(int IDx,int IDy, const QString& Xs,const QString& Ys,const QString& Cp,const QList<Mortgage*>& Mtgs,const Waterfall& Stct,const QDate& StDt,StressTest::StressVariability Xv,StressTest::StressVariability Yv,QObject* parent=0);
	StressThread(int IDx, int IDy, const QString& Xs, const QString& Ys, const QString& Cp, const MtgCashFlow& Mtgs, const Waterfall& Stct, const QDate& StDt, StressTest::StressVariability Xv, StressTest::StressVariability Yv, QObject* parent = 0);
};
#endif // StressThread_h__