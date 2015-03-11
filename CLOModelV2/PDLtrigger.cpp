#include "PDLTrigger.h"
#include <QDataStream>
#include "Tranche.h"
QDataStream& PDLTrigger::WriteToStream(QDataStream& stream) const
{
    stream
        << m_TargetSeniority
        << m_TargetSeniorityLevel
        << static_cast<quint8>(m_SizeSide)
        << static_cast<quint8>(m_SenioritySide)
        << m_TargetSize
        ;
    return stream;
}
QDataStream& PDLTrigger::LoadOldVersion(QDataStream& stream)
{
    quint8 TempChar;
    m_TargetSeniority.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_TargetSeniority;
    m_TargetSeniorityLevel.SetLoadProtocolVersion(m_LoadProtocolVersion);
    stream >> m_TargetSeniorityLevel
        >> TempChar
        ;
    m_SizeSide = static_cast<TriggerSizeSide>(TempChar);
    stream >> TempChar;
    m_SenioritySide = static_cast<TriggerSenioritySide>(TempChar);
    m_TargetSize.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_TargetSize;
    return AbstractTrigger::LoadOldVersion(stream);
}

QString PDLTrigger::ReadyToCalculate() const
{
    if (
        m_TargetSeniority.IsEmpty(1)
        || m_SizeSide == TriggerSizeSide::Invalid
        || m_SenioritySide == TriggerSenioritySide::Invalid
        || m_TargetSeniorityLevel.IsEmpty(0)
        || m_TargetSize.IsEmpty(0.0)
        )
        return "Trigger " + m_TriggerLabel + " is invalid\n";
    return QString();
}
PDLTrigger::PDLTrigger(const PDLTrigger& a)
    : m_TargetSeniorityLevel(a.m_TargetSeniorityLevel)
    , m_TargetSeniority(a.m_TargetSeniority)
    , m_SizeSide(a.m_SizeSide)
    , m_SenioritySide(a.m_SenioritySide)
    , m_TargetSize(a.m_TargetSize)
    , AbstractTrigger(a)
{}

PDLTrigger::PDLTrigger(QString TargetSenior, QString TargetSeniorLevel, QString TargetSize, TriggerSizeSide szd, TriggerSenioritySide ssd, const QString& lab)
    :m_TargetSeniority(TargetSenior)
    , m_TargetSeniorityLevel(TargetSeniorLevel)
    , m_SenioritySide(ssd)
    , m_SizeSide(szd)
    , m_TargetSize(TargetSize)
    , AbstractTrigger(AbstractTrigger::TriggerType::PDLTrigger, lab)
{
    m_TargetSeniorityLevel.SetShift(-1);
}

PDLTrigger::PDLTrigger(const QString& lab)
    :m_TargetSeniority("0")
    , m_TargetSeniorityLevel("1")
    , m_SenioritySide(TriggerSenioritySide::Invalid)
    , m_SizeSide(TriggerSizeSide::Invalid)
    , m_TargetSize("0")
    , AbstractTrigger(AbstractTrigger::TriggerType::PDLTrigger, lab)
{
    m_TargetSeniorityLevel.SetShift(-1);
}



QString PDLTrigger::ToString() const
{
    return AbstractTrigger::ToString()
        + "\nTarget Seniority: " + m_TargetSeniority.GetVector()
        + "\nTarget Seniority Level: " + m_TargetSeniorityLevel.GetVector()
        + "\nSize Limit: " + m_TargetSize.GetVector()
        + "\nSize Side: " + TriggerSizeSideToString(m_SizeSide)
        + "\nSeniority Side: " + TriggerSenioritySideToString(m_SenioritySide);
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
    return !(
        m_TargetSize.GetAnchorDate().isNull()
        || m_TargetSeniorityLevel.GetAnchorDate().isNull()
        || m_TargetSeniority.GetAnchorDate().isNull()
        );
}

void PDLTrigger::FillMissingAnchorDate(const QDate& a)
{
    if (m_TargetSize.GetAnchorDate().isNull()) m_TargetSize.SetAnchorDate(a);
    if (m_TargetSeniorityLevel.GetAnchorDate().isNull()) m_TargetSeniorityLevel.SetAnchorDate(a);
    if (m_TargetSeniority.GetAnchorDate().isNull()) m_TargetSeniority.SetAnchorDate(a);
}

void PDLTrigger::SetSizeMultiplier(double mult)
{
    return ResetSizeMultiplier();
    if (mult != 0.0) m_TargetSize *= mult;
}
