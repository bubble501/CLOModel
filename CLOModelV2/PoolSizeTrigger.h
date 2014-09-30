#ifndef PoolSizeTrigger_h__
#define PoolSizeTrigger_h__

#include "AbstractTrigger.h"
class PoolSizeTrigger : public AbstractTrigger {
public:
	enum class TriggerSide : quint8 {
		Invalid = 0
		, Bigger = 0x1
		, Smaller = 0x2
		, Exactly = 0x4
		, BiggerOrEqual = Bigger | Exactly
		, SmallerOrEqual = Smaller | Exactly
	};
	PoolSizeTrigger(double TargetSize, TriggerSide sd = TriggerSide::Invalid, const QString& lab = QString());
	PoolSizeTrigger(const QString& lab = QString());
	PoolSizeTrigger(const PoolSizeTrigger& a) : m_TargetSize(a.m_TargetSize), m_Side(a.m_Side), AbstractTrigger(a) {}
	virtual bool Passing(double CurrentSize) const;
	virtual bool Failing(double CurrentSize) const { return !Passing(CurrentSize); }
	virtual QString ReadyToCalculate() const override;
	const double& GetTargetSize() const { return m_TargetSize; }
	void SetTargetSize(const double& val) { m_TargetSize = val; }
	const PoolSizeTrigger::TriggerSide& GetSide() const { return m_Side; }
	void SetSide(const PoolSizeTrigger::TriggerSide& val) { m_Side = val; }
protected:
	virtual QDataStream& WriteToStream(QDataStream& stream) const override;
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
	double m_TargetSize;
	TriggerSide m_Side;
};
#endif // PoolSizeTrigger_h__