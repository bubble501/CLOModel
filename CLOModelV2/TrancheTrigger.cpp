#include "TrancheTrigger.h"
#include <QDataStream>
#include "Tranche.h"
QDataStream& TrancheTrigger::WriteToStream(QDataStream& stream) const {
	stream
		<< m_TargetSeniority
		<< m_TargetSeniorityLevel
		<< static_cast<quint8>(m_SizeSide)
		<< static_cast<quint8>(m_SenioritySide)
		;
	return stream;
}
QDataStream& TrancheTrigger::LoadOldVersion(QDataStream& stream) {
	quint8 TempChar;
	stream
		>> m_TargetSeniority
		>> m_TargetSeniorityLevel
		>> TempChar
		;
	m_SizeSide = static_cast<TriggerSizeSide>(TempChar);
	stream >> TempChar;
	m_SenioritySide = static_cast<TriggerSenioritySide>(TempChar);
	return AbstractTrigger::LoadOldVersion(stream);
}

QString TrancheTrigger::ReadyToCalculate() const {
	if (
		m_TargetSeniority < 1 
		|| m_SizeSide == TriggerSizeSide::Invalid
		|| m_SenioritySide == TriggerSenioritySide::Invalid
		|| m_TargetSeniorityLevel <0
		|| m_TargetSize<0.0
	) return "Trigger " + m_TriggerLabel + " is invalid\n";
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

TrancheTrigger::TrancheTrigger(qint32 TargetSenior, qint32 TargetSeniorLevel, double TargetSize, TriggerSizeSide szd, TriggerSenioritySide ssd, const QString& lab)
	:m_TargetSeniority(TargetSenior)
	, m_TargetSeniorityLevel(TargetSeniorLevel)
	, m_SenioritySide(ssd)
	, m_SizeSide(szd)
	, m_TargetSize(TargetSize)
	, AbstractTrigger(AbstractTrigger::TriggerType::TrancheTrigger,lab)
{}

TrancheTrigger::TrancheTrigger(const QString& lab) 
	:m_TargetSeniority(0)
	, m_TargetSeniorityLevel(0)
	, m_SenioritySide(TriggerSenioritySide::Invalid)
	, m_SizeSide(TriggerSizeSide::Invalid)
	, m_TargetSize(0.0)
	, AbstractTrigger(AbstractTrigger::TriggerType::TrancheTrigger, lab) {}

bool TrancheTrigger::Passing(const QList<Tranche*>& currentTranches) const {
	if (!ReadyToCalculate().isEmpty()) return false;
	double RelevantAmount=0.0;
	for (auto i = currentTranches.constBegin(); i != currentTranches.constEnd(); ++i) {
		if ((*i)->GetProrataGroup(m_TargetSeniorityLevel) < m_TargetSeniority && (static_cast<quint8>(m_SenioritySide) & static_cast<quint8>(TriggerSenioritySide::Senior)))
			RelevantAmount += (*i)->GetCurrentOutstanding();
		if ((*i)->GetProrataGroup(m_TargetSeniorityLevel) > m_TargetSeniority && (static_cast<quint8>(m_SenioritySide) & static_cast<quint8>(TriggerSenioritySide::Junior)))
			RelevantAmount += (*i)->GetCurrentOutstanding();
		if ((*i)->GetProrataGroup(m_TargetSeniorityLevel) == m_TargetSeniority && (static_cast<quint8>(m_SenioritySide) & static_cast<quint8>(TriggerSenioritySide::Exactly)))
			RelevantAmount += (*i)->GetCurrentOutstanding();
	}
	bool Result = false;
	if (static_cast<quint8>(m_SizeSide) & static_cast<quint8>(TriggerSizeSide::Bigger)) {
		Result = Result || RelevantAmount > m_TargetSize;
	}
	if (static_cast<quint8>(m_SizeSide) & static_cast<quint8>(TriggerSizeSide::Smaller)) {
		Result = Result || RelevantAmount < m_TargetSize;
	}
	if (static_cast<quint8>(m_SizeSide) & static_cast<quint8>(TriggerSizeSide::Exactly)) {
		Result = Result || RelevantAmount == m_TargetSize;
	}
	return Result;
}

