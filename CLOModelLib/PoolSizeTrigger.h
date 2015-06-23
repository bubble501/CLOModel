#ifndef PoolSizeTrigger_h__
#define PoolSizeTrigger_h__

#include "AbstractTrigger.h"
class PoolSizeTriggerPrivate;
class BloombergVector;
class PoolSizeTrigger : public AbstractTrigger {
    DECLARE_PUBLIC_COMMONS(PoolSizeTrigger)
    DECLARE_PUBLIC_COMMONS_COPY(PoolSizeTrigger)
public:
	enum class TriggerSide : quint8 {
		Invalid = 0
		, Bigger = 0x1
		, Smaller = 0x2
		, Exactly = 0x4
		, BiggerOrEqual = Bigger | Exactly
		, SmallerOrEqual = Smaller | Exactly
	};
	QString TriggerSideToString(TriggerSide a) const;
	PoolSizeTrigger(QString TargetSize, TriggerSide sd, const QString& lab = QString());
	PoolSizeTrigger(const QString& lab);
	virtual bool Passing(double CurrentSize, const QDate& CurrentPeriod) const;
	virtual bool Passing(double CurrentSize, int CurrentPeriod) const;
	virtual bool Failing(double CurrentSize, const QDate& CurrentPeriod) const;
	virtual bool Failing(double CurrentSize, int CurrentPeriod) const;
	void SetAnchorDate(const QDate& a);
	void RemoveAnchorDate();
	bool HasAnchor()const;
	virtual QString ReadyToCalculate() const override;
	const BloombergVector& GetTargetSize() const;
	void SetTargetSize(const QString& val);
	const PoolSizeTrigger::TriggerSide& GetSide() const;
	void SetSide(const PoolSizeTrigger::TriggerSide& val);
	virtual QString ToString() const override;
protected:
	virtual QDataStream& WriteToStream(QDataStream& stream) const override;
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
};
#endif // PoolSizeTrigger_h__