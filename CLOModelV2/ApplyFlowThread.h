#ifndef ApplyFlowThread_h__
#define ApplyFlowThread_h__
#include "MtgCashFlow.h"
#include "AssumptionSet.h"
#include "TemplAsyncThread.h"
class ApplyFlowThread : public TemplAsyncThread<MtgCashFlow>{
	Q_OBJECT
public:
	ApplyFlowThread(int ID, QObject* parent = 0);
	void SetBaseFlow(const MtgCashFlow& a) { BaseFlow = &a; }
	void SetAssumption(const AssumptionSet& a) { AssumptionsToApply = a; }
private:
	const MtgCashFlow* BaseFlow;
	AssumptionSet AssumptionsToApply;
public slots:
	virtual void run() override;
protected slots:
	virtual void EmitFromAnonim()override { 
		emit Calculated(Identifier, m_Result);
	}
signals:
	void Calculated(int, MtgCashFlow);
};
#endif // ApplyFlowThread_h__