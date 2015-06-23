#include "PoolSizeTrigger.h"
#include "Private/PoolSizeTrigger_p.h"
#include <QDataStream>
DEFINE_PUBLIC_COMMONS(PoolSizeTrigger)
DEFINE_PUBLIC_COMMONS_COPY(PoolSizeTrigger)

PoolSizeTriggerPrivate::PoolSizeTriggerPrivate(PoolSizeTrigger *q)
    :AbstractTriggerPrivate(q)
{}
PoolSizeTrigger::PoolSizeTrigger(PoolSizeTriggerPrivate *d, const PoolSizeTrigger& other)
    : AbstractTrigger(d, other)
{
    d->m_TargetSize=other.d_func()->m_TargetSize;
    d->m_Side=other.d_func()->m_Side;
}
PoolSizeTrigger& PoolSizeTrigger::operator=(const PoolSizeTrigger& other)
{
    Q_D(PoolSizeTrigger);
    AbstractTrigger::operator=(other);
    d->m_TargetSize = other.d_func()->m_TargetSize;
    d->m_Side = other.d_func()->m_Side;
    return *this;
}
PoolSizeTrigger::PoolSizeTrigger(PoolSizeTriggerPrivate *d)
    :AbstractTrigger(d, TriggerType::PoolSizeTrigger)
{
    d->m_Side = TriggerSide::Invalid;
    d->m_TargetSize.SetDivisor(1.0);
}


QDataStream& PoolSizeTrigger::WriteToStream(QDataStream& stream) const {
    Q_D(const PoolSizeTrigger);
	stream
		<< d->m_TargetSize
        << static_cast<quint8>(d->m_Side)
		;
	return stream;
}
QDataStream& PoolSizeTrigger::LoadOldVersion(QDataStream& stream) {
    Q_D(PoolSizeTrigger);
	quint8 TempChar;
    d->m_TargetSize.SetLoadProtocolVersion(loadProtocolVersion());
	stream
        >> d->m_TargetSize
		>> TempChar
		;
    d->m_Side = static_cast<TriggerSide>(TempChar);
	return AbstractTrigger::LoadOldVersion(stream);
}
PoolSizeTrigger::PoolSizeTrigger(QString TargetSize, TriggerSide sd, const QString& lab)
    :PoolSizeTrigger(lab)
{
    Q_D(PoolSizeTrigger);
    d->m_Side = sd;
    d->m_TargetSize = TargetSize;
}
PoolSizeTrigger::PoolSizeTrigger(const QString& lab)
    : PoolSizeTrigger()
{
    Q_D(PoolSizeTrigger);
    d->m_TriggerLabel=lab;
}

bool PoolSizeTrigger::Failing(double CurrentSize, int CurrentPeriod) const
{
    return !Passing(CurrentSize, CurrentPeriod);
}

bool PoolSizeTrigger::Failing(double CurrentSize, const QDate& CurrentPeriod) const
{
    return !Passing(CurrentSize, CurrentPeriod);
}

void PoolSizeTrigger::SetAnchorDate(const QDate& a)
{
    Q_D( PoolSizeTrigger);
    d->m_TargetSize.SetAnchorDate(a);
}

void PoolSizeTrigger::RemoveAnchorDate()
{
    Q_D( PoolSizeTrigger);
    d->m_TargetSize.RemoveAnchorDate();
}

bool PoolSizeTrigger::HasAnchor() const
{
    Q_D(const PoolSizeTrigger);
    return !d->m_TargetSize.GetAnchorDate().isNull();
}

QString PoolSizeTrigger::ReadyToCalculate() const
{
    Q_D(const PoolSizeTrigger);
    if (d->m_TargetSize.IsEmpty(0.0) || d->m_Side == TriggerSide::Invalid) 
        return "Trigger " + d->m_TriggerLabel + " is invalid\n";
	return QString();
}

const BloombergVector& PoolSizeTrigger::GetTargetSize() const
{
    Q_D(const PoolSizeTrigger);
    return d->m_TargetSize;
}

void PoolSizeTrigger::SetTargetSize(const QString& val)
{
    Q_D( PoolSizeTrigger);
    d->m_TargetSize = val;
}

const PoolSizeTrigger::TriggerSide& PoolSizeTrigger::GetSide() const
{
    Q_D(const PoolSizeTrigger);
    return d->m_Side;
}

void PoolSizeTrigger::SetSide(const TriggerSide& val)
{
    Q_D( PoolSizeTrigger);
    d->m_Side = val;
}

QString PoolSizeTrigger::ToString() const
{
    Q_D(const PoolSizeTrigger);
    return AbstractTrigger::ToString() + "\nSize Limit: " + d->m_TargetSize.GetVector() + "\nSide: " + TriggerSideToString(d->m_Side);
}

bool PoolSizeTrigger::Passing(double CurrentSize, const QDate& CurrentPeriod) const
{
    Q_D(const PoolSizeTrigger);
    if (d->m_TargetSize.GetValue(CurrentPeriod)<0.0) 
        return false;
	bool Result = false;
    if (static_cast<quint8>(d->m_Side)& static_cast<quint8>(TriggerSide::Bigger)) {
        Result = Result || CurrentSize > d->m_TargetSize.GetValue(CurrentPeriod);
	}
    if (static_cast<quint8>(d->m_Side)& static_cast<quint8>(TriggerSide::Smaller)) {
        Result = Result || CurrentSize < d->m_TargetSize.GetValue(CurrentPeriod);
	}
    if (static_cast<quint8>(d->m_Side)& static_cast<quint8>(TriggerSide::Exactly)) {
        Result = Result || CurrentSize == d->m_TargetSize.GetValue(CurrentPeriod);
	}
	return Result;
}
bool PoolSizeTrigger::Passing(double CurrentSize, int CurrentPeriod) const {
    Q_D(const PoolSizeTrigger);
    if (d->m_TargetSize.GetValue(CurrentPeriod) < 0.0) return false;
	bool Result = false;
    if (static_cast<quint8>(d->m_Side)& static_cast<quint8>(TriggerSide::Bigger)) {
        Result = Result || CurrentSize > d->m_TargetSize.GetValue(CurrentPeriod);
	}
    if (static_cast<quint8>(d->m_Side)& static_cast<quint8>(TriggerSide::Smaller)) {
        Result = Result || CurrentSize < d->m_TargetSize.GetValue(CurrentPeriod);
	}
    if (static_cast<quint8>(d->m_Side)& static_cast<quint8>(TriggerSide::Exactly)) {
        Result = Result || CurrentSize == d->m_TargetSize.GetValue(CurrentPeriod);
	}
	return Result;
}

QString PoolSizeTrigger::TriggerSideToString(TriggerSide a) const {
	switch (a) {
	case TriggerSide::Invalid: return "Invalid";
	case TriggerSide::Bigger: return "Bigger";
	case TriggerSide::Smaller: return "Smaller";
	case TriggerSide::Exactly: return "Exactly";
	case TriggerSide::BiggerOrEqual: return "Bigger or Equal";
	case TriggerSide::SmallerOrEqual: return "Smaller or Equal";
	default: return QString();
	}
}
