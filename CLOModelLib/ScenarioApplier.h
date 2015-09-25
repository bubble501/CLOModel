#ifndef ScenarioApplier_h__
#define ScenarioApplier_h__
#include <QList>
#include "MtgCashFlow.h"
#include "ApplyFlowThread.h"
#include "TemplAsyncCalculator.h"
#include <memory>
class ScenarioApplierPrivate;
class AssumptionSet;
class ScenarioApplier;
CLOMODELLIB_EXPORT QDataStream&  operator<<(QDataStream & stream, const ScenarioApplier& flows);
CLOMODELLIB_EXPORT QDataStream&  operator>>(QDataStream & stream, ScenarioApplier& flows);
class CLOMODELLIB_EXPORT ScenarioApplier : public TemplAsyncCalculator<ApplyFlowThread, MtgCashFlow>
{
	Q_OBJECT
protected:
    inline ScenarioApplierPrivate* d_func() { return reinterpret_cast<ScenarioApplierPrivate *>(qGetPtrHelper(BackwardInterface::d_ptr)); }
    inline const ScenarioApplierPrivate* d_func() const { return reinterpret_cast<const ScenarioApplierPrivate *>(qGetPtrHelper(BackwardInterface::d_ptr)); }
    friend class ScenarioApplierPrivate;
    ScenarioApplier(ScenarioApplierPrivate *d, QObject* parent = nullptr);
    virtual void BeeReturned(int Ident, const MtgCashFlow& a) override;
    qint32 FindAssumption(const AssumptionSet& a)const;
    virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
public slots:
    virtual bool StartCalculation() override;
public:
    ScenarioApplier(QObject* parent = nullptr);
	virtual ~ScenarioApplier();
	const MtgCashFlow& GetBaseFlows() const;
	virtual void SetBaseFlows(const MtgCashFlow& val);
	virtual void ClearScenarios();
	virtual const std::shared_ptr<AssumptionSet> GetAssumption(qint32 idx)const;
	virtual QList<qint32> GetAssumptionKeys()const;
	virtual void AddAssumption(const AssumptionSet& a, qint32 idx);
	using  TemplAsyncCalculator<ApplyFlowThread, MtgCashFlow>::GetResult;
    const std::shared_ptr<MtgCashFlow> GetResult(const AssumptionSet& a)const;
	virtual QString ReadyToCalculate() const override;
	virtual void Reset() override;
	virtual int NumBees() const override;
	friend CLOMODELLIB_EXPORT QDataStream& operator<<(QDataStream & stream, const ScenarioApplier& flows);
	friend CLOMODELLIB_EXPORT QDataStream& operator>>(QDataStream & stream, ScenarioApplier& flows);
};

#endif // ScenarioApplier_h__