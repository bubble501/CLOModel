#include "CumulativeLossTrigger.h"
#include "Private/CumulativeLossTrigger_p.h"
#include <QDataStream>
DEFINE_PUBLIC_COMMONS(CumulativeLossTrigger)
DEFINE_PUBLIC_COMMONS_COPY(CumulativeLossTrigger)
CumulativeLossTriggerPrivate::CumulativeLossTriggerPrivate(CumulativeLossTrigger *q,const CumulativeLossTriggerPrivate& other)
	:AbstractTriggerPrivate(q,other)
    , m_Side(other.m_Side)
    , m_TargetSize(other.m_TargetSize)
{}
CumulativeLossTriggerPrivate::CumulativeLossTriggerPrivate(CumulativeLossTrigger *q)
	:AbstractTriggerPrivate(q)
{}
CumulativeLossTrigger::CumulativeLossTrigger(CumulativeLossTriggerPrivate *d, const CumulativeLossTrigger& other)
	:AbstractTrigger(d,other)
{}
CumulativeLossTrigger& CumulativeLossTrigger::operator=(const CumulativeLossTrigger& other){
	Q_D(CumulativeLossTrigger);
	AbstractTrigger::operator=(other);
    d->m_Side = other.d_func()->m_Side;
    d->m_TargetSize = other.d_func()->m_TargetSize;
	return *this;
}
CumulativeLossTrigger::CumulativeLossTrigger(CumulativeLossTriggerPrivate *d)
    :AbstractTrigger(d, TriggerType::CumulativeLossTrigger)
{
    d->m_TargetSize.SetDivisor(1.0);
    d->m_Side = TriggerSide::Invalid;
}

QDataStream& CumulativeLossTrigger::WriteToStream(QDataStream& stream) const {
    Q_D(const CumulativeLossTrigger);
	stream
		<< d->m_TargetSize
        << static_cast<quint8>(d->m_Side)
		;
	return stream;
}
QDataStream& CumulativeLossTrigger::LoadOldVersion(QDataStream& stream) {
    Q_D(CumulativeLossTrigger);
	quint8 TempChar;
    d->m_TargetSize.SetLoadProtocolVersion(loadProtocolVersion());
	stream
        >> d->m_TargetSize
		>> TempChar
		;
    d->m_Side = static_cast<TriggerSide>(TempChar);
	return AbstractTrigger::LoadOldVersion(stream);
}
CumulativeLossTrigger::CumulativeLossTrigger(QString TargetSize, TriggerSide sd, const QString& lab)
    :CumulativeLossTrigger(lab)
{
    Q_D(CumulativeLossTrigger);
    d->m_Side = sd;
    d->m_TargetSize = TargetSize;
}
CumulativeLossTrigger::CumulativeLossTrigger(const QString& lab)
    : CumulativeLossTrigger()
{
    Q_D(CumulativeLossTrigger);
    d->m_TriggerLabel = lab;
}

QString CumulativeLossTrigger::ReadyToCalculate() const
{
    Q_D(const CumulativeLossTrigger);
    if (d->m_TargetSize.IsEmpty(0.0) || d->m_Side == TriggerSide::Invalid) return "Trigger " + d->m_TriggerLabel + " is invalid\n";
	return QString();
}

const BloombergVector& CumulativeLossTrigger::GetTargetSize() const
{
    Q_D(const CumulativeLossTrigger);
    return d->m_TargetSize;
}

void CumulativeLossTrigger::SetTargetSize(const QString& val)
{
    Q_D( CumulativeLossTrigger);
    d->m_TargetSize = val;
}

const CumulativeLossTrigger::TriggerSide& CumulativeLossTrigger::GetSide() const
{
    Q_D(const CumulativeLossTrigger);
    return d->m_Side;
}

void CumulativeLossTrigger::SetSide(const TriggerSide& val)
{
    Q_D(CumulativeLossTrigger);
    d->m_Side = val;
}

QString CumulativeLossTrigger::ToString() const
{
    Q_D(const CumulativeLossTrigger);
    return AbstractTrigger::ToString() + "\nLoss Limit: " + d->m_TargetSize.GetVector() + "\nSide: " + TriggerSideToString(d->m_Side);
}

bool CumulativeLossTrigger::Passing(double CurrentSize, const QDate& CurrentPeriod) const
{
    Q_D(const CumulativeLossTrigger);
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
bool CumulativeLossTrigger::Passing(double CurrentSize, int CurrentPeriod) const
{
    Q_D(const CumulativeLossTrigger);
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

bool CumulativeLossTrigger::Failing(double CurrentSize, const QDate& CurrentPeriod) const
{
    return !Passing(CurrentSize, CurrentPeriod);
}

bool CumulativeLossTrigger::Failing(double CurrentSize, int CurrentPeriod) const
{
    return !Passing(CurrentSize, CurrentPeriod);
}

void CumulativeLossTrigger::SetAnchorDate(const QDate& a)
{
    Q_D( CumulativeLossTrigger);
    d->m_TargetSize.SetAnchorDate(a);
}

void CumulativeLossTrigger::RemoveAnchorDate()
{
    Q_D( CumulativeLossTrigger);
    d->m_TargetSize.RemoveAnchorDate();
}

bool CumulativeLossTrigger::HasAnchor() const
{
    Q_D(const CumulativeLossTrigger);
    return !d->m_TargetSize.GetAnchorDate().isNull();
}

QString CumulativeLossTrigger::TriggerSideToString(TriggerSide a) const
{
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