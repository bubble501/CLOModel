#include <QObject>
#include "Waterfall.h"
#include "Mortgage.h"
#include "StressTest.h"
#include <QHash>
#include <QString>
class StressWorker:public QObject{
	Q_OBJECT
private:
	int Identity[2];
	QString XSpann;
	QString YSpann;
	QString ConstantPar;
	QList<Mortgage> Loans;
	Waterfall LocalStructure;
	QDate StartDate;
	StressTest::StressVariability StressDimension[2];
public:
	StressWorker(int IDx,int IDy, const QString& Xs,const QString& Ys,const QString& Cp,const QList<Mortgage*>& Mtgs,const Waterfall& Stct,const QDate& StDt,StressTest::StressVariability Xv,StressTest::StressVariability Yv,QObject* parent=0);
	void Work();
signals:
	void ScenarioCalculated(int,int,Waterfall);
};