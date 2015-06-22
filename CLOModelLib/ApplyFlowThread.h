#ifndef ApplyFlowThread_h__
#define ApplyFlowThread_h__
#include "MtgCashFlow.h"
#include "AssumptionSet.h"
#include "TemplAsyncThread.h"
class ApplyFlowThreadPrivate;
class ApplyFlowThread : public TemplAsyncThread<MtgCashFlow>{
	Q_OBJECT
protected:
    Q_DECLARE_PRIVATE(ApplyFlowThread)
    AbstrAsyncThreadPrivate* d_ptr;
    ApplyFlowThread(ApplyFlowThreadPrivate* d, int ID, QObject* parent = nullptr);
public:
	ApplyFlowThread(int ID, QObject* parent = nullptr);
	void SetBaseFlow(const MtgCashFlow& a);
	void SetAssumption(const AssumptionSet& a);
public slots:
	virtual void run() override;
protected slots:
	virtual void EmitFromAnonim()override;
signals:
	void Calculated(int, MtgCashFlow);
};
#endif // ApplyFlowThread_h__