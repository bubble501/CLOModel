#ifndef DateTrigger_h__
#define DateTrigger_h__
#include "AbstractTrigger.h"
#include <QDate>
class DateTrigger : public AbstractTrigger {
public:
	enum class TriggerSide : quint8 {
		Invalid=0
		, BeforeExcluding = 0x1
		, AfterExcluding = 0x2
		, Exactly = 0x4
		, BeforeIncluding = BeforeExcluding | Exactly
		, AfterIncluding = AfterExcluding | Exactly
	};
	DateTrigger(const QString& lab = QString());
	DateTrigger(const QDate& dte, TriggerSide sd = TriggerSide::Invalid, const QString& lab = QString());
	DateTrigger(const DateTrigger& a) : m_LimitDate(a.m_LimitDate), m_Side(a.m_Side), AbstractTrigger(a) {}
	virtual bool Passing(const QDate& CurrentDate) const;
	virtual bool Failing(const QDate& CurrentDate) const { return !Passing(CurrentDate); }
	const QDate& GetLimitDate() const { return m_LimitDate; }
	void SetLimitDate(const QDate& val) { m_LimitDate = val; }
	const DateTrigger::TriggerSide& GetSide() const { return m_Side; }
	void SetSide(const DateTrigger::TriggerSide& val) { m_Side = val; }
	virtual QString ReadyToCalculate() const override;
	virtual QString ToString() { return AbstractTrigger::ToString() + "\nLimit Date: " + m_LimitDate.toString("yyyy-MM-dd") + QString("\nSide: %1").arg(static_cast<quint8>(m_Side)); }
protected:
	virtual QDataStream& WriteToStream(QDataStream& stream) const override;
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
	QDate m_LimitDate;
	TriggerSide m_Side;
};
#endif // DateTrigger_h__

