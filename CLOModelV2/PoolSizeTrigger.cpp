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
PoolSizeTrigger::PoolSizeTrigger(double TargetSize, TriggerSide sd, const QString& lab)
	:AbstractTrigger(TriggerType::PoolSizeTrigger, lab)
	, m_Side(sd)
	, m_TargetSize(TargetSize) {

}
PoolSizeTrigger::PoolSizeTrigger(const QString& lab)
	: AbstractTrigger(TriggerType::PoolSizeTrigger, lab)
	, m_Side(TriggerSide::Invalid) {

}
QString PoolSizeTrigger::ReadyToCalculate() const {
	if (m_TargetSize<0.0 || m_Side == TriggerSide::Invalid) return "Trigger " + m_TriggerLabel + " is invalid\n";
	return QString();
}
bool PoolSizeTrigger::Passing(double CurrentSize) const {
	if (m_TargetSize<0.0) return false;
	bool Result = false;
	if (static_cast<quint8>(m_Side)& static_cast<quint8>(TriggerSide::Bigger)) {
		Result = Result || CurrentSize > m_TargetSize;
	}
	if (static_cast<quint8>(m_Side)& static_cast<quint8>(TriggerSide::Smaller)) {
		Result = Result || CurrentSize < m_TargetSize;
	}
	if (static_cast<quint8>(m_Side)& static_cast<quint8>(TriggerSide::Exactly)) {
		Result = Result || CurrentSize == m_TargetSize;
	}
	return Result;
}
