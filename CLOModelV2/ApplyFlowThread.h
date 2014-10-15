#include <QThread>
#ifndef ApplyFlowThread_h__
#define ApplyFlowThread_h__
#include "MtgCashFlow.h"
#include "AssumptionSet.h"
class ApplyFlowThread : public QThread {
	Q_OBJECT
public:
	ApplyFlowThread(int ID, QObject* parent = 0);
	void SetBaseFlow(const MtgCashFlow& a) { BaseFlow = &a; }
private:
	const MtgCashFlow* BaseFlow;
	AssumptionSet AssumptionsToApply;
	int Identifier;
signals:
	void Calculated(int, MtgCashFlow);
public slots:
	void run();
	void stop() { exit(0); }
};
#endif // ApplyFlowThread_h__