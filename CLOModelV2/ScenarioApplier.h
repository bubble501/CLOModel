#ifndef ScenarioApplier_h__
#define ScenarioApplier_h__
#include <QObject>
#include <QList>
#include <QHash>
#include "MtgCashFlow.h"
#include "CommonFunctions.h"
#include "BackwardCompatibilityInterface.h"
#include "ApplyFlowThread.h"
#include "TemplAsyncCalculator.h"
class MtgCashFlow;
class AssumptionSet;
class ScenarioApplier : public TemplAsyncCalculator<ApplyFlowThread, MtgCashFlow> {
	Q_OBJECT
public:
	ScenarioApplier(QObject* parent = 0);
	virtual ~ScenarioApplier() { Reset(); }
	const MtgCashFlow& GetBaseFlows() const { return m_BaseFlows; }
	virtual void SetBaseFlows(const MtgCashFlow& val) { RETURN_WHEN_RUNNING(true, ) m_BaseFlows = val; }
	virtual void ClearScenarios();
	virtual const AssumptionSet* GetAssumption(qint32 idx)const { return m_Scenarios.value(idx, nullptr); }
	virtual QList<qint32> GetAssumptionKeys()const { return m_Scenarios.keys(); }
	virtual void AddAssumption(const AssumptionSet& a, qint32 idx);
	using  TemplAsyncCalculator<ApplyFlowThread, MtgCashFlow>::GetResult;
	const MtgCashFlow* GetResult(const AssumptionSet& a)const { return GetResult(FindAssumption(a)); }
	virtual QString ReadyToCalculate() const override;
	virtual void Reset() override;
	virtual int NumBees() const override { return m_Scenarios.size(); }
public slots:
	virtual bool StartCalculation() override;
protected:
	virtual void BeeReturned(int Ident, const MtgCashFlow& a) override;
	MtgCashFlow m_BaseFlows;
	QHash<qint32, AssumptionSet*> m_Scenarios;
	qint32 FindAssumption(const AssumptionSet& a)const;
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
	friend QDataStream& operator<<(QDataStream & stream, const ScenarioApplier& flows);
	friend QDataStream& operator>>(QDataStream & stream, ScenarioApplier& flows);
};
QDataStream& operator<<(QDataStream & stream, const ScenarioApplier& flows);
QDataStream& operator>>(QDataStream & stream, ScenarioApplier& flows);
#endif // ScenarioApplier_h__