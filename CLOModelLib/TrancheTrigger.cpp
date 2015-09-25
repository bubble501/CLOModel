#include "TrancheTrigger.h"
#include "Private/TrancheTrigger_p.h"
#include "Private/TrancheTriggerSettingWidget.h"
#include <QDataStream>
#include "Tranche.h"
DEFINE_PUBLIC_COMMONS(TrancheTrigger)
DEFINE_PUBLIC_COMMONS_COPY(TrancheTrigger)
TrancheTrigger::~TrancheTrigger()
{

}
TrancheTriggerPrivate::~TrancheTriggerPrivate()
{

}
TrancheTriggerPrivate::TrancheTriggerPrivate(TrancheTrigger *q,const TrancheTriggerPrivate& other)
	:AbstractTriggerPrivate(q,other)
    , m_TargetSeniorityLevel(other.m_TargetSeniorityLevel)
    , m_TargetSeniority(other.m_TargetSeniority)
    , m_SizeSide(other.m_SizeSide)
    , m_SenioritySide(other.m_SenioritySide)
    , m_TargetSize(other.m_TargetSize)
{}
TrancheTriggerPrivate::TrancheTriggerPrivate(TrancheTrigger *q)
	:AbstractTriggerPrivate(q)
    , m_TargetSeniority("0")
    , m_TargetSeniorityLevel("1")
    , m_TargetSize("0")
    , m_SenioritySide(TrancheTrigger::TriggerSenioritySide::Invalid)
    , m_SizeSide(TrancheTrigger::TriggerSizeSide::Invalid)
{
    m_TargetSeniorityLevel.SetShift(-1);
}
TrancheTrigger::TrancheTrigger(TrancheTriggerPrivate *d, const TrancheTrigger& other)
	:AbstractTrigger(d,other)
{}
TrancheTrigger& TrancheTrigger::operator=(const TrancheTrigger& other){
	Q_D(TrancheTrigger);
	AbstractTrigger::operator=(other);
    d->m_TargetSeniorityLevel = other.d_func()->m_TargetSeniorityLevel;
    d->m_TargetSeniority = other.d_func()->m_TargetSeniority;
    d->m_SizeSide = other.d_func()->m_SizeSide;
    d->m_SenioritySide = other.d_func()->m_SenioritySide;
    d->m_TargetSize = other.d_func()->m_TargetSize;
	return *this;
}
TrancheTrigger::TrancheTrigger(TrancheTriggerPrivate *d)
    :AbstractTrigger(d, AbstractTrigger::TriggerType::TrancheTrigger)
{}

TrancheTrigger::TrancheTrigger(QString TargetSenior, QString TargetSeniorLevel, QString TargetSize, TriggerSizeSide szd, TriggerSenioritySide ssd, const QString& lab)
    :TrancheTrigger(lab)
{
    Q_D(TrancheTrigger);
    d->m_TargetSeniority = TargetSenior;
    d->m_TargetSeniorityLevel = TargetSeniorLevel;
    d->m_SenioritySide = ssd;
    d->m_SizeSide = szd;
    d->m_TargetSize = TargetSize;
}

TrancheTrigger::TrancheTrigger(const QString& lab)
    :TrancheTrigger()
{
    Q_D(TrancheTrigger);
    d->m_TriggerLabel = lab;
}

QDataStream& TrancheTrigger::WriteToStream(QDataStream& stream) const {
    Q_D(const TrancheTrigger);
	stream
		<< d->m_TargetSeniority
        << d->m_TargetSeniorityLevel
        << static_cast<quint8>(d->m_SizeSide)
        << static_cast<quint8>(d->m_SenioritySide)
        << d->m_TargetSize
		;
	return stream;
}
QDataStream& TrancheTrigger::LoadOldVersion(QDataStream& stream) {
    Q_D(TrancheTrigger);
	quint8 TempChar;
    d->m_TargetSeniority.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->m_TargetSeniority;
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

bool TrancheTrigger::Failing(const QList<std::shared_ptr<Tranche> >& currentTranches, const QDate& CurrentPeriod) const
{
    return !Passing(currentTranches, CurrentPeriod);
}

bool TrancheTrigger::Failing(const QList<std::shared_ptr<Tranche> >& currentTranches, int CurrentPeriod) const
{
    return !Passing(currentTranches, CurrentPeriod);
}

QString TrancheTrigger::ReadyToCalculate() const
{
    Q_D(const TrancheTrigger);
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

const TrancheTrigger::TriggerSizeSide& TrancheTrigger::GetSizeSide() const
{
    Q_D(const TrancheTrigger);
    return d->m_SizeSide;
}

void TrancheTrigger::SetSizeSide(const TriggerSizeSide& val)
{
    Q_D( TrancheTrigger);
    d->m_SizeSide = val;
}

const TrancheTrigger::TriggerSenioritySide& TrancheTrigger::GetSenioritySide() const
{
    Q_D(const TrancheTrigger);
    return d->m_SenioritySide;
}

void TrancheTrigger::SetSenioritySide(const TriggerSenioritySide& val)
{
    Q_D( TrancheTrigger);
    d->m_SenioritySide = val;
}

const IntegerVector& TrancheTrigger::GetTargetSeniorityLevel() const
{
    Q_D(const TrancheTrigger);
    return d->m_TargetSeniorityLevel;
}

void TrancheTrigger::SetTargetSeniorityLevel(const QString& val)
{
    Q_D( TrancheTrigger);
    d->m_TargetSeniorityLevel = val;
}

const IntegerVector& TrancheTrigger::GetTargetSeniority() const
{
    Q_D(const TrancheTrigger);
    return d->m_TargetSeniority;
}

void TrancheTrigger::SetTargetSeniority(const QString& val)
{
    Q_D( TrancheTrigger);
    d->m_TargetSeniority = val;
}

const BloombergVector& TrancheTrigger::GetTargetSize() const
{
    Q_D(const TrancheTrigger);
    return d->m_TargetSize;
}

void TrancheTrigger::SetTargetSize(const QString& val)
{
    Q_D( TrancheTrigger);
    d->m_TargetSize = val;
}



bool TrancheTrigger::Passing(const QList<std::shared_ptr<Tranche> >& currentTranches, const QDate& CurrentPeriod) const
{
    Q_D(const TrancheTrigger);
	if (!ReadyToCalculate().isEmpty()) return false;
	double RelevantAmount=0.0;
	for (auto i = currentTranches.constBegin(); i != currentTranches.constEnd(); ++i) {
        if ((*i)->GetProrataGroup(d->m_TargetSeniorityLevel.GetValue(CurrentPeriod)) < d->m_TargetSeniority.GetValue(CurrentPeriod) && (static_cast<quint8>(d->m_SenioritySide)& static_cast<quint8>(TriggerSenioritySide::Senior)))
			RelevantAmount += (*i)->GetCashFlow().GetAmountOutstanding(CurrentPeriod);
        if ((*i)->GetProrataGroup(d->m_TargetSeniorityLevel.GetValue(CurrentPeriod)) > d->m_TargetSeniority.GetValue(CurrentPeriod) && (static_cast<quint8>(d->m_SenioritySide)& static_cast<quint8>(TriggerSenioritySide::Junior)))
			RelevantAmount += (*i)->GetCashFlow().GetAmountOutstanding(CurrentPeriod);
        if ((*i)->GetProrataGroup(d->m_TargetSeniorityLevel.GetValue(CurrentPeriod)) == d->m_TargetSeniority.GetValue(CurrentPeriod) && (static_cast<quint8>(d->m_SenioritySide)& static_cast<quint8>(TriggerSenioritySide::Exactly)))
			RelevantAmount += (*i)->GetCashFlow().GetAmountOutstanding(CurrentPeriod);
	}
	bool Result = false;
    if (static_cast<quint8>(d->m_SizeSide) & static_cast<quint8>(TriggerSizeSide::Bigger)) {
        Result = Result || RelevantAmount > d->m_TargetSize.GetValue(CurrentPeriod);
	}
    if (static_cast<quint8>(d->m_SizeSide) & static_cast<quint8>(TriggerSizeSide::Smaller)) {
        Result = Result || RelevantAmount < d->m_TargetSize.GetValue(CurrentPeriod);
	}
    if (static_cast<quint8>(d->m_SizeSide) & static_cast<quint8>(TriggerSizeSide::Exactly)) {
        Result = Result || RelevantAmount == d->m_TargetSize.GetValue(CurrentPeriod);
	}
	return Result;
}
bool TrancheTrigger::Passing(const QList<std::shared_ptr<Tranche> >& currentTranches, int CurrentPeriod) const
{
    Q_D(const TrancheTrigger);
	if (!ReadyToCalculate().isEmpty()) return false;
	double RelevantAmount = 0.0;
	for (auto i = currentTranches.constBegin(); i != currentTranches.constEnd(); ++i) {
        if ((*i)->GetProrataGroup(d->m_TargetSeniorityLevel.GetValue(CurrentPeriod)) < d->m_TargetSeniority.GetValue(CurrentPeriod) && (static_cast<quint8>(d->m_SenioritySide)& static_cast<quint8>(TriggerSenioritySide::Senior)))
			RelevantAmount += (*i)->GetCashFlow().GetAmountOutstanding(CurrentPeriod);
        if ((*i)->GetProrataGroup(d->m_TargetSeniorityLevel.GetValue(CurrentPeriod)) > d->m_TargetSeniority.GetValue(CurrentPeriod) && (static_cast<quint8>(d->m_SenioritySide)& static_cast<quint8>(TriggerSenioritySide::Junior)))
			RelevantAmount += (*i)->GetCashFlow().GetAmountOutstanding(CurrentPeriod);
        if ((*i)->GetProrataGroup(d->m_TargetSeniorityLevel.GetValue(CurrentPeriod)) == d->m_TargetSeniority.GetValue(CurrentPeriod) && (static_cast<quint8>(d->m_SenioritySide)& static_cast<quint8>(TriggerSenioritySide::Exactly)))
			RelevantAmount += (*i)->GetCashFlow().GetAmountOutstanding(CurrentPeriod);
	}
	bool Result = false;
    if (static_cast<quint8>(d->m_SizeSide)& static_cast<quint8>(TriggerSizeSide::Bigger)) {
        Result = Result || RelevantAmount > d->m_TargetSize.GetValue(CurrentPeriod);
	}
    if (static_cast<quint8>(d->m_SizeSide)& static_cast<quint8>(TriggerSizeSide::Smaller)) {
        Result = Result || RelevantAmount < d->m_TargetSize.GetValue(CurrentPeriod);
	}
    if (static_cast<quint8>(d->m_SizeSide)& static_cast<quint8>(TriggerSizeSide::Exactly)) {
        Result = Result || RelevantAmount == d->m_TargetSize.GetValue(CurrentPeriod);
	}
	return Result;
}

QString TrancheTrigger::ToString() const
{
    Q_D(const TrancheTrigger);
	return AbstractTrigger::ToString()
        + "\nTarget Seniority: " + d->m_TargetSeniority.GetVector()
        + "\nTarget Seniority Level: " + d->m_TargetSeniorityLevel.GetVector()
        + "\nSize Limit: " + d->m_TargetSize.GetVector()
        + "\nSize Side: " + TriggerSizeSideToString(d->m_SizeSide)
        + "\nSeniority Side: " + TriggerSenioritySideToString(d->m_SenioritySide);
}
QString TrancheTrigger::TriggerSizeSideToString(TriggerSizeSide a) const
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
QString TrancheTrigger::TriggerSenioritySideToString(TriggerSenioritySide a) const
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

bool TrancheTrigger::HasAnchor() const
{
    Q_D(const TrancheTrigger);
	return !(
        d->m_TargetSize.GetAnchorDate().isNull()
        || d->m_TargetSeniorityLevel.GetAnchorDate().isNull()
        || d->m_TargetSeniority.GetAnchorDate().isNull()
		);
}

void TrancheTrigger::FillMissingAnchorDate(const QDate& a)
{
    Q_D( TrancheTrigger);
    if (d->m_TargetSize.GetAnchorDate().isNull()) d->m_TargetSize.SetAnchorDate(a);
    if (d->m_TargetSeniorityLevel.GetAnchorDate().isNull()) d->m_TargetSeniorityLevel.SetAnchorDate(a);
    if (d->m_TargetSeniority.GetAnchorDate().isNull()) d->m_TargetSeniority.SetAnchorDate(a);
}

void TrancheTrigger::SetAnchorDate(const QDate& a)
{
    Q_D( TrancheTrigger);
    d->m_TargetSize.SetAnchorDate(a); 
    d->m_TargetSeniorityLevel.SetAnchorDate(a); 
    d->m_TargetSeniority.SetAnchorDate(a);
}

void TrancheTrigger::RemoveAnchorDate()
{
    Q_D( TrancheTrigger);
    d->m_TargetSize.RemoveAnchorDate(); 
    d->m_TargetSeniorityLevel.RemoveAnchorDate();
    d->m_TargetSeniority.RemoveAnchorDate();
}

TrancheTriggerSettingWidget* TrancheTrigger::createSettingsWidget(QWidget* parent /*= nullptr*/)
{
    return new TrancheTriggerSettingWidget(parent);
}

void TrancheTrigger::SetSizeMultiplier(double mult)
{
    Q_D( TrancheTrigger);
    ResetSizeMultiplier();
     if (mult != 0.0)
         d->m_TargetSize *= mult;

}

void TrancheTrigger::ResetSizeMultiplier()
{
    Q_D( TrancheTrigger);
    d->m_TargetSize.SetDivisor(1.0);
}
