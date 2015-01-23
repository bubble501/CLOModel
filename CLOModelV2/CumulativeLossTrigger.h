#ifndef CumulativeLossTrigger_h__
#define CumulativeLossTrigger_h__
#include "AbstractTrigger.h"
#include "BloombergVector.h"
class CumulativeLossTrigger : public AbstractTrigger {
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
	CumulativeLossTrigger(QString TargetSize, TriggerSide sd, const QString& lab = QString());
	CumulativeLossTrigger(const QString& lab = QString());
	CumulativeLossTrigger(const CumulativeLossTrigger& a) : m_TargetSize(a.m_TargetSize), m_Side(a.m_Side), AbstractTrigger(a) {}
	virtual bool Passing(double CurrentSize, const QDate& CurrentPeriod) const;
	virtual bool Passing(double CurrentSize, int CurrentPeriod) const;
	virtual bool Failing(double CurrentSize, const QDate& CurrentPeriod) const { return !Passing(CurrentSize, CurrentPeriod); }
	virtual bool Failing(double CurrentSize, int CurrentPeriod) const { return !Passing(CurrentSize, CurrentPeriod); }
	void SetAnchorDate(const QDate& a) { m_TargetSize.SetAnchorDate(a); }
	void RemoveAnchorDate() { m_TargetSize.RemoveAnchorDate(); }
	bool HasAnchor()const { return !m_TargetSize.GetAnchorDate().isNull(); }
	virtual QString ReadyToCalculate() const override;
	const BloombergVector& GetTargetSize() const { return m_TargetSize; }
	void SetTargetSize(const QString& val) { m_TargetSize = val; }
	const CumulativeLossTrigger::TriggerSide& GetSide() const { return m_Side; }
	void SetSide(const CumulativeLossTrigger::TriggerSide& val) { m_Side = val; }
	virtual QString ToString() const override { return AbstractTrigger::ToString() + "\nLoss Limit: " + m_TargetSize.GetVector() + "\nSide: " + TriggerSideToString(m_Side); }
protected:
	virtual QDataStream& WriteToStream(QDataStream& stream) const override;
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
	BloombergVector m_TargetSize;
	TriggerSide m_Side;
};
#endif // CumulativeLossTrigger_h__