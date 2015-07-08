#ifndef DateTrigger_h__
#define DateTrigger_h__
#include "AbstractTrigger.h"
class DateTriggerPrivate;
class DateTriggerSettingWidget;
class QWidget;
class CLOMODELLIB_EXPORT DateTrigger : public AbstractTrigger
{
    DECLARE_PUBLIC_COMMONS(DateTrigger)
    DECLARE_PUBLIC_COMMONS_COPY(DateTrigger)
public:
	enum TriggerSide : quint8 {
		Invalid=0
		, BeforeExcluding = 0x1
		, AfterExcluding = 0x2
		, Exactly = 0x4
		, BeforeIncluding = BeforeExcluding | Exactly
		, AfterIncluding = AfterExcluding | Exactly
	};
	QString TriggerSideToString(TriggerSide a) const;
	DateTrigger(const QString& lab);
	DateTrigger(const QDate& dte, TriggerSide sd = TriggerSide::Invalid, const QString& lab = QString());
	virtual bool Passing(const QDate& CurrentDate) const;
	virtual bool Failing(const QDate& CurrentDate) const;
	const QDate& GetLimitDate() const;
	void SetLimitDate(const QDate& val);
	const DateTrigger::TriggerSide& GetSide() const;
	void SetSide(const DateTrigger::TriggerSide& val);
	virtual QString ReadyToCalculate() const override;
	virtual QString ToString() const override;
    static DateTriggerSettingWidget* createSettingsWidget(QWidget* parent = nullptr);
protected:
	virtual QDataStream& WriteToStream(QDataStream& stream) const override;
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
};
#endif // DateTrigger_h__

