#include "TrancheTrigger.h"
#include <QDataStream>
#include "Tranche.h"
QDataStream& TrancheTrigger::WriteToStream(QDataStream& stream) const {
	stream
		<< m_TargetSeniority
		<< m_TargetSeniorityLevel
		<< static_cast<quint8>(m_SizeSide)
		<< static_cast<quint8>(m_SenioritySide)
		<< m_TargetSize
		;
	return stream;
}
QDataStream& TrancheTrigger::LoadOldVersion(QDataStream& stream) {
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

QString TrancheTrigger::ReadyToCalculate() const {
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
TrancheTrigger::TrancheTrigger(const TrancheTrigger& a) 
	: m_TargetSeniorityLevel(a.m_TargetSeniorityLevel)
	, m_TargetSeniority(a.m_TargetSeniority)
	, m_SizeSide(a.m_SizeSide)
	, m_SenioritySide(a.m_SenioritySide)
	, m_TargetSize(a.m_TargetSize)
	, AbstractTrigger(a) 
{}

TrancheTrigger::TrancheTrigger(QString TargetSenior, QString TargetSeniorLevel, QString TargetSize, TriggerSizeSide szd, TriggerSenioritySide ssd, const QString& lab)
	:m_TargetSeniority(TargetSenior)
	, m_TargetSeniorityLevel(TargetSeniorLevel)
	, m_SenioritySide(ssd)
	, m_SizeSide(szd)
	, m_TargetSize(TargetSize)
	, AbstractTrigger(AbstractTrigger::TriggerType::TrancheTrigger,lab)
{
	m_TargetSeniorityLevel.SetShift(-1);
}

TrancheTrigger::TrancheTrigger(const QString& lab) 
	:m_TargetSeniority("0")
	, m_TargetSeniorityLevel("1")
	, m_SenioritySide(TriggerSenioritySide::Invalid)
	, m_SizeSide(TriggerSizeSide::Invalid)
	, m_TargetSize("0")
	, AbstractTrigger(AbstractTrigger::TriggerType::TrancheTrigger, lab)
{
	m_TargetSeniorityLevel.SetShift(-1);
}

bool TrancheTrigger::Passing(const QList<Tranche*>& currentTranches, const QDate& CurrentPeriod) const {
	if (!ReadyToCalculate().isEmpty()) return false;
	double RelevantAmount=0.0;
	for (auto i = currentTranches.constBegin(); i != currentTranches.constEnd(); ++i) {
		if ((*i)->GetProrataGroup(m_TargetSeniorityLevel.GetValue(CurrentPeriod)) < m_TargetSeniority.GetValue(CurrentPeriod) && (static_cast<quint8>(m_SenioritySide)& static_cast<quint8>(TriggerSenioritySide::Senior)))
			RelevantAmount += (*i)->GetCurrentOutstanding();
		if ((*i)->GetProrataGroup(m_TargetSeniorityLevel.GetValue(CurrentPeriod)) > m_TargetSeniority.GetValue(CurrentPeriod) && (static_cast<quint8>(m_SenioritySide)& static_cast<quint8>(TriggerSenioritySide::Junior)))
			RelevantAmount += (*i)->GetCurrentOutstanding();
		if ((*i)->GetProrataGroup(m_TargetSeniorityLevel.GetValue(CurrentPeriod)) == m_TargetSeniority.GetValue(CurrentPeriod) && (static_cast<quint8>(m_SenioritySide)& static_cast<quint8>(TriggerSenioritySide::Exactly)))
			RelevantAmount += (*i)->GetCurrentOutstanding();
	}
	bool Result = false;
	if (static_cast<quint8>(m_SizeSide) & static_cast<quint8>(TriggerSizeSide::Bigger)) {
		Result = Result || RelevantAmount > m_TargetSize.GetValue(CurrentPeriod);
	}
	if (static_cast<quint8>(m_SizeSide) & static_cast<quint8>(TriggerSizeSide::Smaller)) {
		Result = Result || RelevantAmount < m_TargetSize.GetValue(CurrentPeriod);
	}
	if (static_cast<quint8>(m_SizeSide) & static_cast<quint8>(TriggerSizeSide::Exactly)) {
		Result = Result || RelevantAmount == m_TargetSize.GetValue(CurrentPeriod);
	}
	return Result;
}
bool TrancheTrigger::Passing(const QList<Tranche*>& currentTranches, int CurrentPeriod) const {
	if (!ReadyToCalculate().isEmpty()) return false;
	double RelevantAmount = 0.0;
	for (auto i = currentTranches.constBegin(); i != currentTranches.constEnd(); ++i) {
		if ((*i)->GetProrataGroup(m_TargetSeniorityLevel.GetValue(CurrentPeriod)) < m_TargetSeniority.GetValue(CurrentPeriod) && (static_cast<quint8>(m_SenioritySide)& static_cast<quint8>(TriggerSenioritySide::Senior)))
			RelevantAmount += (*i)->GetCurrentOutstanding();
		if ((*i)->GetProrataGroup(m_TargetSeniorityLevel.GetValue(CurrentPeriod)) > m_TargetSeniority.GetValue(CurrentPeriod) && (static_cast<quint8>(m_SenioritySide)& static_cast<quint8>(TriggerSenioritySide::Junior)))
			RelevantAmount += (*i)->GetCurrentOutstanding();
		if ((*i)->GetProrataGroup(m_TargetSeniorityLevel.GetValue(CurrentPeriod)) == m_TargetSeniority.GetValue(CurrentPeriod) && (static_cast<quint8>(m_SenioritySide)& static_cast<quint8>(TriggerSenioritySide::Exactly)))
			RelevantAmount += (*i)->GetCurrentOutstanding();
	}
	bool Result = false;
	if (static_cast<quint8>(m_SizeSide)& static_cast<quint8>(TriggerSizeSide::Bigger)) {
		Result = Result || RelevantAmount > m_TargetSize.GetValue(CurrentPeriod);
	}
	if (static_cast<quint8>(m_SizeSide)& static_cast<quint8>(TriggerSizeSide::Smaller)) {
		Result = Result || RelevantAmount < m_TargetSize.GetValue(CurrentPeriod);
	}
	if (static_cast<quint8>(m_SizeSide)& static_cast<quint8>(TriggerSizeSide::Exactly)) {
		Result = Result || RelevantAmount == m_TargetSize.GetValue(CurrentPeriod);
	}
	return Result;
}

QString TrancheTrigger::ToString() const {
	return AbstractTrigger::ToString()
		+ "\nTarget Seniority: " + m_TargetSeniority.GetVector()
		+ "\nTarget Seniority Level: " + m_TargetSeniorityLevel.GetVector()
		+ "\nSize Limit: " + m_TargetSize.GetVector()
		+ "\nSize Side: " + TriggerSizeSideToString(m_SizeSide)
		+ "\nSeniority Side: " + TriggerSenioritySideToString(m_SenioritySide);
}
QString TrancheTrigger::TriggerSizeSideToString(TriggerSizeSide a) const {
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
QString TrancheTrigger::TriggerSenioritySideToString(TriggerSenioritySide a) const {
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

bool TrancheTrigger::HasAnchor() const{
	return !(
		m_TargetSize.GetAnchorDate().isNull()
		|| m_TargetSeniorityLevel.GetAnchorDate().isNull()
		|| m_TargetSeniority.GetAnchorDate().isNull()
		);
}

void TrancheTrigger::FillMissingAnchorDate(const QDate& a) {
	if (m_TargetSize.GetAnchorDate().isNull()) m_TargetSize.SetAnchorDate(a);
	if (m_TargetSeniorityLevel.GetAnchorDate().isNull()) m_TargetSeniorityLevel.SetAnchorDate(a);
	if (m_TargetSeniority.GetAnchorDate().isNull()) m_TargetSeniority.SetAnchorDate(a);
}

void TrancheTrigger::SetSizeMultiplier(double mult) {
	if (mult == 0.0) return ResetSizeMultiplier();  
	m_TargetSize*=mult;
}
