#include <QObject>
#include "Waterfall.h"
#include "Mortgage.h"
#include "StressTest.h"
#include <QHash>
#include <QString>
class MtgCalculator;
class StressWorker:public QObject{
	Q_OBJECT
private:
	int Identity[2];
	QString XSpann;
	QString YSpann;
	QString ConstantPar;
	MtgCalculator* Loans;
	MtgCashFlow BaseCashFlows;
	bool UseFastVersion;
	Waterfall LocalStructure;
	QDate StartDate;
	StressTest::StressVariability StressDimension[2];
	void WorkSlow();
	void WorkFast();
public:
	StressWorker(int IDx, int IDy, const QString& Xs, const QString& Ys, const QString& Cp, const QHash<qint32, Mortgage*>& Mtgs, const Waterfall& Stct, const QDate& StDt, StressTest::StressVariability Xv, StressTest::StressVariability Yv, QObject* parent = 0);
	StressWorker(int IDx, int IDy, const QString& Xs, const QString& Ys, const QString& Cp, const MtgCashFlow& Mtgs, const Waterfall& Stct, const QDate& StDt, StressTest::StressVariability Xv, StressTest::StressVariability Yv, QObject* parent = 0);
	~StressWorker() { if (Loans) delete Loans; }
	void Work();
signals:
	void ScenarioCalculated(int,int,const Waterfall&);
private slots:
	void LoansCalculated();
};