#include "DateTrigger.h"
#include "Private/DateTrigger_p.h"
#include "Private/DateTriggerSettingWidget.h"
#include <QDataStream>
DEFINE_PUBLIC_COMMONS(DateTrigger)
DEFINE_PUBLIC_COMMONS_COPY(DateTrigger)
DateTrigger::~DateTrigger()
{

}
DateTriggerPrivate::~DateTriggerPrivate()
{

}
DateTriggerPrivate::DateTriggerPrivate(DateTrigger *q,const DateTriggerPrivate& other)
	:AbstractTriggerPrivate(q,other)
    , m_LimitDate(other.m_LimitDate)
    , m_Side(other.m_Side)
{}
DateTriggerPrivate::DateTriggerPrivate(DateTrigger *q)
	:AbstractTriggerPrivate(q)
{}
DateTrigger::DateTrigger(DateTriggerPrivate *d, const DateTrigger& other)
	:AbstractTrigger(d,other)
{}
DateTrigger& DateTrigger::operator=(const DateTrigger& other){
	Q_D(DateTrigger);
	AbstractTrigger::operator=(other);
    d->m_LimitDate = other.d_func()->m_LimitDate;
    d->m_Side = other.d_func()->m_Side;
	return *this;
}
DateTrigger::DateTrigger(DateTriggerPrivate *d)
    :AbstractTrigger(d, TriggerType::DateTrigger)
{
    d->m_Side = TriggerSide::Invalid;
}

DateTrigger::DateTrigger(const QDate& dte, TriggerSide sd, const QString& lab)
    : DateTrigger(lab)
{
    Q_D(DateTrigger);
    d->m_LimitDate = dte;
    d->m_Side = sd;
}

DateTrigger::DateTrigger(const QString& lab)
    :DateTrigger()
{
    Q_D(DateTrigger);
    d->m_TriggerLabel = lab;
}



bool DateTrigger::Passing(const QDate& CurrentDate) const {
    Q_D(const DateTrigger);
	if (d->m_LimitDate.isNull()) return false;
	bool Result = false;
    if (static_cast<quint8>(d->m_Side)& static_cast<quint8>(TriggerSide::BeforeExcluding)) {
        Result = Result || CurrentDate < d->m_LimitDate;
	}
    if (static_cast<quint8>(d->m_Side)& static_cast<quint8>(TriggerSide::AfterExcluding)) {
        Result = Result || CurrentDate > d->m_LimitDate;
	}
    if (static_cast<quint8>(d->m_Side)& static_cast<quint8>(TriggerSide::Exactly)) {
        Result = Result || CurrentDate == d->m_LimitDate;
	}
	return Result;
}

bool DateTrigger::Failing(const QDate& CurrentDate) const
{
    return !Passing(CurrentDate);
}

const QDate& DateTrigger::GetLimitDate() const
{
    Q_D(const DateTrigger);
    return d->m_LimitDate;
}

void DateTrigger::SetLimitDate(const QDate& val)
{
    Q_D(DateTrigger);
    d->m_LimitDate = val;
}

const DateTrigger::TriggerSide& DateTrigger::GetSide() const
{
    Q_D(const DateTrigger);
    return d->m_Side;
}

void DateTrigger::SetSide(const TriggerSide& val)
{
    Q_D(DateTrigger);
    d->m_Side = val;
}

QDataStream& DateTrigger::WriteToStream(QDataStream& stream) const
{
    Q_D(const DateTrigger);
	stream
        << d->m_LimitDate
        << static_cast<quint8>(d->m_Side)
		;
	return stream;
}
QDataStream& DateTrigger::LoadOldVersion(QDataStream& stream) {
    Q_D(DateTrigger);
	quint8 TempChar;
	stream
        >> d->m_LimitDate
		>> TempChar
		;
    d->m_Side = static_cast<TriggerSide>(TempChar);
	return AbstractTrigger::LoadOldVersion(stream);
}



QString DateTrigger::ReadyToCalculate() const
{
    Q_D(const DateTrigger);
    if (d->m_LimitDate.isNull() || d->m_Side == TriggerSide::Invalid) return "Trigger " + d->m_TriggerLabel + " is invalid\n";
	return QString();
}
QString DateTrigger::ToString() const
{
    Q_D(const DateTrigger);
    return AbstractTrigger::ToString() + "\nLimit Date: " + d->m_LimitDate.toString("yyyy-MM-dd") + "\nSide: " + TriggerSideToString(d->m_Side);
}

DateTriggerSettingWidget* DateTrigger::createSettingsWidget(QWidget* parent /*= nullptr*/)
{
    return new DateTriggerSettingWidget(parent);
}

QString DateTrigger::TriggerSideToString(TriggerSide a) const
{
	switch (a) {
	case TriggerSide::Invalid: return "Invalid";
	case TriggerSide::BeforeExcluding: return "Before Excluding";
	case TriggerSide::AfterExcluding: return "After Excluding";
	case TriggerSide::Exactly: return "Exactly";
	case TriggerSide::BeforeIncluding: return "Before Including";
	case TriggerSide::AfterIncluding: return "After Including";
	default: return QString();
	}
}
