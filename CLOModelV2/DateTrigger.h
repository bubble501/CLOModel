#ifndef AndTrigger_h__
#define AndTrigger_h__
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
	DateTrigger(const QDate& dte, TriggerSide sd = TriggerSide::AfterExcluding, const QString& lab = QString());
	virtual bool Passing(const QDate& CurrentDate) const;
	virtual bool Failing(const QDate& CurrentDate) const { return !Passing(CurrentDate); }
	const QDate& GetLimitDate() const { return m_LimitDate; }
	void SetLimitDate(const QDate& val) { m_LimitDate = val; }
	const DateTrigger::TriggerSide& GetSide() const { return m_Side; }
	void SetSide(const DateTrigger::TriggerSide& val) { m_Side = val; }
protected:
	virtual QDataStream& WriteToStream(QDataStream& stream) const override;
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
	QDate m_LimitDate;
	TriggerSide m_Side;
};
#endif // AndTrigger_h__

