#ifndef DelinquencyTrigger_h__
#define DelinquencyTrigger_h__
#include "AbstractTrigger.h"
#include "BloombergVector.h"
class DelinquencyTrigger : public AbstractTrigger {
public:
	DelinquencyTrigger(const BloombergVector& Target, const QString& lab = QString());
	DelinquencyTrigger(const QString& Target, const QString& lab);
	DelinquencyTrigger(const QString& lab = QString());
	DelinquencyTrigger(const DelinquencyTrigger& a) : m_Target(a.m_Target), AbstractTrigger(a) {}
	virtual bool Passing(double CurrentDelinq,const QDate& CurrentPeriod) const;
	virtual bool Passing(double CurrentDelinq, int CurrentPeriod) const;
	virtual bool Failing(double CurrentDelinq, const QDate& CurrentPeriod) const { return !Passing(CurrentDelinq, CurrentPeriod); }
	virtual bool Failing(double CurrentDelinq, int CurrentPeriod) const { return !Passing(CurrentDelinq, CurrentPeriod); }
	virtual QString ReadyToCalculate() const override;
	const BloombergVector& GetTarget() const { return m_Target; }
	void SetTarget(const BloombergVector& val) { m_Target = val; }
	void SetTarget(const QString& val) { m_Target = val; }
	void SetAnchorDate(const QDate& a) { m_Target.SetAnchorDate(a); }
	void RemoveAnchorDate() { m_Target.RemoveAnchorDate(); }
	virtual QString ToString() const override { return AbstractTrigger::ToString() + "\nDelinquency Limit: " + m_Target.GetVector(); }
protected:
	virtual QDataStream& WriteToStream(QDataStream& stream) const override;
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
	BloombergVector m_Target;
};
#endif // DelinquencyTrigger_h__