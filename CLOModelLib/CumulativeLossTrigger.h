#ifndef CumulativeLossTrigger_h__
#define CumulativeLossTrigger_h__
#include "AbstractTrigger.h"
class CumulativeLossTriggerPrivate;
class CumulativeLossTrigger : public AbstractTrigger {
    DECLARE_PUBLIC_COMMONS(CumulativeLossTrigger)
    DECLARE_PUBLIC_COMMONS_COPY(CumulativeLossTrigger)
public:
	enum TriggerSide : quint8 {
		Invalid = 0
		, Bigger = 0x1
		, Smaller = 0x2
		, Exactly = 0x4
		, BiggerOrEqual = Bigger | Exactly
		, SmallerOrEqual = Smaller | Exactly
	};
	QString TriggerSideToString(TriggerSide a) const;
	CumulativeLossTrigger(QString TargetSize, TriggerSide sd, const QString& lab = QString());
	CumulativeLossTrigger(const QString& lab);
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
	const CumulativeLossTrigger::TriggerSide& GetSide() const;
	void SetSide(const CumulativeLossTrigger::TriggerSide& val);
	virtual QString ToString() const override;
protected:
	virtual QDataStream& WriteToStream(QDataStream& stream) const override;
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
	
};
#endif // CumulativeLossTrigger_h__