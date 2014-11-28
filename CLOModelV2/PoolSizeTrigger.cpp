#include "PoolSizeTrigger.h"
#include <QDataStream>
QDataStream& PoolSizeTrigger::WriteToStream(QDataStream& stream) const {
	stream
		<< m_TargetSize
		<< static_cast<quint8>(m_Side)
		;
	return stream;
}
QDataStream& PoolSizeTrigger::LoadOldVersion(QDataStream& stream) {
	quint8 TempChar;
	stream
		>> m_TargetSize
		>> TempChar
		;
	m_Side = static_cast<TriggerSide>(TempChar);
	return AbstractTrigger::LoadOldVersion(stream);
}
PoolSizeTrigger::PoolSizeTrigger(QString TargetSize, TriggerSide sd, const QString& lab)
	:AbstractTrigger(TriggerType::PoolSizeTrigger, lab)
	, m_Side(sd)
	, m_TargetSize(TargetSize) 
{
	m_TargetSize.SetDivisor(1.0);
}
PoolSizeTrigger::PoolSizeTrigger(const QString& lab)
	: AbstractTrigger(TriggerType::PoolSizeTrigger, lab)
	, m_Side(TriggerSide::Invalid) 
{
	m_TargetSize.SetDivisor(1.0);
}
QString PoolSizeTrigger::ReadyToCalculate() const {
	if (m_TargetSize.IsEmpty(0.0) || m_Side == TriggerSide::Invalid) return "Trigger " + m_TriggerLabel + " is invalid\n";
	return QString();
}
bool PoolSizeTrigger::Passing(double CurrentSize, const QDate& CurrentPeriod) const {
	if (m_TargetSize.GetValue(CurrentPeriod)<0.0) return false;
	bool Result = false;
	if (static_cast<quint8>(m_Side)& static_cast<quint8>(TriggerSide::Bigger)) {
		Result = Result || CurrentSize > m_TargetSize.GetValue(CurrentPeriod);
	}
	if (static_cast<quint8>(m_Side)& static_cast<quint8>(TriggerSide::Smaller)) {
		Result = Result || CurrentSize < m_TargetSize.GetValue(CurrentPeriod);
	}
	if (static_cast<quint8>(m_Side)& static_cast<quint8>(TriggerSide::Exactly)) {
		Result = Result || CurrentSize == m_TargetSize.GetValue(CurrentPeriod);
	}
	return Result;
}
bool PoolSizeTrigger::Passing(double CurrentSize, int CurrentPeriod) const {
	if (m_TargetSize.GetValue(CurrentPeriod) < 0.0) return false;
	bool Result = false;
	if (static_cast<quint8>(m_Side)& static_cast<quint8>(TriggerSide::Bigger)) {
		Result = Result || CurrentSize > m_TargetSize.GetValue(CurrentPeriod);
	}
	if (static_cast<quint8>(m_Side)& static_cast<quint8>(TriggerSide::Smaller)) {
		Result = Result || CurrentSize < m_TargetSize.GetValue(CurrentPeriod);
	}
	if (static_cast<quint8>(m_Side)& static_cast<quint8>(TriggerSide::Exactly)) {
		Result = Result || CurrentSize == m_TargetSize.GetValue(CurrentPeriod);
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
