#include "PDLTrigger.h"
#include "Private/PDLTrigger_p.h"
#include <QDataStream>
DEFINE_PUBLIC_COMMONS(PDLTrigger)
DEFINE_PUBLIC_COMMONS_COPY(PDLTrigger)

PDLTriggerPrivate::PDLTriggerPrivate(PDLTrigger *q)
    :AbstractTriggerPrivate(q)
{}
PDLTrigger::PDLTrigger(PDLTriggerPrivate *d, const PDLTrigger& other)
    : AbstractTrigger(d, other)
{
    d->m_TargetSeniorityLevel = other.d_func()->m_TargetSeniorityLevel;
    d->m_TargetSeniority = other.d_func()->m_TargetSeniority;
    d->m_SizeSide = other.d_func()->m_SizeSide;
    d->m_SenioritySide = other.d_func()->m_SenioritySide;
    d->m_TargetSize = other.d_func()->m_TargetSize;
}
PDLTrigger& PDLTrigger::operator=(const PDLTrigger& other)
{
    Q_D(PDLTrigger);
    AbstractTrigger::operator=(other);
    d->m_TargetSeniorityLevel = other.d_func()->m_TargetSeniorityLevel;
    d->m_TargetSeniority = other.d_func()->m_TargetSeniority;
    d->m_SizeSide = other.d_func()->m_SizeSide;
    d->m_SenioritySide = other.d_func()->m_SenioritySide;
    d->m_TargetSize = other.d_func()->m_TargetSize;
    return *this;
}
PDLTrigger::PDLTrigger(PDLTriggerPrivate *d)
    :AbstractTrigger(d, TriggerType::PDLTrigger)
{
    d->m_TargetSeniority = "0";
    d->m_TargetSeniorityLevel = "1";
    d->m_SenioritySide = TriggerSenioritySide::Invalid;
    d->m_SizeSide = TriggerSizeSide::Invalid;
    d->m_TargetSize = "0";
    d->m_TargetSeniorityLevel.SetShift(-1);
}



PDLTrigger::PDLTrigger(QString TargetSenior, QString TargetSeniorLevel, QString TargetSize, TriggerSizeSide szd, TriggerSenioritySide ssd, const QString& lab)
    :PDLTrigger(lab)
{
    Q_D(PDLTrigger);
    d->m_TargetSeniority=TargetSenior;
    d->m_TargetSeniorityLevel=TargetSeniorLevel;
    d->m_SenioritySide=ssd;
    d->m_SizeSide=szd;
    d->m_TargetSize=TargetSize;
}

PDLTrigger::PDLTrigger(const QString& lab)
    : PDLTrigger()
{
    Q_D(PDLTrigger);
    d->m_TriggerLabel = lab;
}

QDataStream& PDLTrigger::WriteToStream(QDataStream& stream) const
{
    Q_D(const PDLTrigger);
    stream
        << d->m_TargetSeniority
        << d->m_TargetSeniorityLevel
        << static_cast<quint8>(d->m_SizeSide)
        << static_cast<quint8>(d->m_SenioritySide)
        << d->m_TargetSize
        ;
    return stream;
}
QDataStream& PDLTrigger::LoadOldVersion(QDataStream& stream)
{
    Q_D( PDLTrigger);
    quint8 TempChar;
    d->m_TargetSeniority.SetLoadProtocolVersion(loadProtocolVersion()); 
    stream >> d->m_TargetSeniority;
    d->m_TargetSeniorityLevel.SetLoadProtocolVersion(loadProtocolVersion());
    stream >> d->m_TargetSeniorityLevel
        >> TempChar
        ;
    d->m_SizeSide = static_cast<TriggerSizeSide>(TempChar);
    stream >> TempChar;
    d->m_SenioritySide = static_cast<TriggerSenioritySide>(TempChar);
    d->m_TargetSize.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->m_TargetSize;
    return AbstractTrigger::LoadOldVersion(stream);
}

bool PDLTrigger::Passing(const QList<Tranche*>& currentTranches, double Assets, int CurrentPeriod) const
{
    Q_D(const PDLTrigger);
    return d->PassingTemplate(currentTranches, Assets, CurrentPeriod);
}

bool PDLTrigger::Passing(const QList<Tranche*>& currentTranches, double Assets, const QDate& CurrentPeriod) const
{
    Q_D(const PDLTrigger);
    return d->PassingTemplate(currentTranches, Assets, CurrentPeriod);
}

bool PDLTrigger::Failing(const QList<Tranche*>& currentTranches, double Assets, int CurrentPeriod) const
{
    return !Passing(currentTranches, Assets, CurrentPeriod);
}

bool PDLTrigger::Failing(const QList<Tranche*>& currentTranches, double Assets, const QDate& CurrentPeriod) const
{
    return !Passing(currentTranches, Assets, CurrentPeriod);
}

QString PDLTrigger::ReadyToCalculate() const
{
    Q_D(const PDLTrigger);
    if (
        d->m_TargetSeniority.IsEmpty(1)
        || d->m_SizeSide == TriggerSizeSide::Invalid
        || d->m_SenioritySide == TriggerSenioritySide::Invalid
        || d->m_TargetSeniorityLevel.IsEmpty(0)
        || d->m_TargetSize.IsEmpty(0.0)
        )
        return "Trigger " + d->m_TriggerLabel + " is invalid\n";
    return QString();
}

const PDLTrigger::TriggerSizeSide& PDLTrigger::GetSizeSide() const
{
    Q_D(const PDLTrigger);
    return d->m_SizeSide;
}

void PDLTrigger::SetSizeSide(const TriggerSizeSide& val)
{
    Q_D( PDLTrigger);
    d->m_SizeSide = val;
}

const PDLTrigger::TriggerSenioritySide& PDLTrigger::GetSenioritySide() const
{
    Q_D(const PDLTrigger);
    return d->m_SenioritySide;
}

void PDLTrigger::SetSenioritySide(const TriggerSenioritySide& val)
{
    Q_D( PDLTrigger);
    d->m_SenioritySide = val;
}

const IntegerVector& PDLTrigger::GetTargetSeniorityLevel() const
{
    Q_D(const PDLTrigger);
    return d->m_TargetSeniorityLevel;
}

QString PDLTrigger::ToString() const
{
    Q_D(const PDLTrigger);
    return AbstractTrigger::ToString()
        + "\nTarget Seniority: " + d->m_TargetSeniority.GetVector()
        + "\nTarget Seniority Level: " + d->m_TargetSeniorityLevel.GetVector()
        + "\nSize Limit: " + d->m_TargetSize.GetVector()
        + "\nSize Side: " + TriggerSizeSideToString(d->m_SizeSide)
        + "\nSeniority Side: " + TriggerSenioritySideToString(d->m_SenioritySide);
}
QString PDLTrigger::TriggerSizeSideToString(TriggerSizeSide a) const
{
    switch (a) {
    case TriggerSizeSide::Invalid: return "Invalid";
    case TriggerSizeSide::Bigger: return "Bigger";
    case TriggerSizeSide::Smaller: return "Smaller";
    case TriggerSizeSide::Exactly: return "Exactly";
    case TriggerSizeSide::BiggerOrEqual: return "Bigger or Equal";
    case TriggerSizeSide::SmallerOrEqual: return "Smaller or Equal";
    default: return QString();
    }
}
QString PDLTrigger::TriggerSenioritySideToString(TriggerSenioritySide a) const
{
    switch (a) {
    case TriggerSenioritySide::Invalid: return "Invalid";
    case TriggerSenioritySide::Senior: return "Senior";
    case TriggerSenioritySide::Junior: return "Junior";
    case TriggerSenioritySide::Exactly: return "Exactly";
    case TriggerSenioritySide::SeniorOrEqual: return "Senior or Equal";
    case TriggerSenioritySide::JuniorOrEqual: return "Junior or Equal";
    default: return QString();
    }
}

bool PDLTrigger::HasAnchor() const
{
    Q_D(const PDLTrigger);
    return !(
        d->m_TargetSize.GetAnchorDate().isNull()
        || d->m_TargetSeniorityLevel.GetAnchorDate().isNull()
        || d->m_TargetSeniority.GetAnchorDate().isNull()
        );
}

void PDLTrigger::FillMissingAnchorDate(const QDate& a)
{
    Q_D( PDLTrigger);
    if (d->m_TargetSize.GetAnchorDate().isNull())
        d->m_TargetSize.SetAnchorDate(a);
    if (d->m_TargetSeniorityLevel.GetAnchorDate().isNull())
        d->m_TargetSeniorityLevel.SetAnchorDate(a);
    if (d->m_TargetSeniority.GetAnchorDate().isNull())
        d->m_TargetSeniority.SetAnchorDate(a);
}

void PDLTrigger::SetAnchorDate(const QDate& a)
{
    Q_D( PDLTrigger);
    d->m_TargetSize.SetAnchorDate(a);
    d->m_TargetSeniorityLevel.SetAnchorDate(a);
    d->m_TargetSeniority.SetAnchorDate(a);
}

void PDLTrigger::RemoveAnchorDate()
{
    Q_D( PDLTrigger);
    d->m_TargetSize.RemoveAnchorDate();
    d->m_TargetSeniorityLevel.RemoveAnchorDate(); 
    d->m_TargetSeniority.RemoveAnchorDate();
}

void PDLTrigger::SetTargetSeniorityLevel(const QString& val)
{
    Q_D( PDLTrigger);
    d->m_TargetSeniorityLevel = val;
}

const IntegerVector& PDLTrigger::GetTargetSeniority() const
{
    Q_D(const PDLTrigger);
    return d->m_TargetSeniority;
}

void PDLTrigger::SetTargetSeniority(const QString& val)
{
    Q_D( PDLTrigger);
    d->m_TargetSeniority = val;
}

const BloombergVector& PDLTrigger::GetTargetSize() const
{
    Q_D(const PDLTrigger);
    return d->m_TargetSize;
}

void PDLTrigger::SetTargetSize(const QString& val)
{
    Q_D( PDLTrigger);
    d->m_TargetSize = val;
}

void PDLTrigger::SetSizeMultiplier(double mult)
{
    Q_D( PDLTrigger);
    return ResetSizeMultiplier();
    if (mult != 0.0) 
        d->m_TargetSize *= mult;
}

void PDLTrigger::ResetSizeMultiplier()
{
    Q_D( PDLTrigger);
    d->m_TargetSize.SetDivisor(1.0);
}
