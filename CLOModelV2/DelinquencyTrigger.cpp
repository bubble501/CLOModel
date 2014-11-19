#include "DelinquencyTrigger.h"
#include <QDataStream>
QDataStream& DelinquencyTrigger::WriteToStream(QDataStream& stream) const {
	stream
		<< m_Target
		;
	return stream;
}
QDataStream& DelinquencyTrigger::LoadOldVersion(QDataStream& stream) {
	m_Target.SetLoadProtocolVersion(m_LoadProtocolVersion);
	stream
		>> m_Target
		;
	return AbstractTrigger::LoadOldVersion(stream);
}
DelinquencyTrigger::DelinquencyTrigger(const BloombergVector& Target, const QString& lab)
	:AbstractTrigger(TriggerType::DelinquencyTrigger, lab)
	, m_Target(Target) {}
DelinquencyTrigger::DelinquencyTrigger(const QString& Target, const QString& lab)
	: AbstractTrigger(TriggerType::DelinquencyTrigger, lab)
	, m_Target(Target) {}
DelinquencyTrigger::DelinquencyTrigger(const QString& lab)
	: AbstractTrigger(TriggerType::DelinquencyTrigger, lab)
{}
bool DelinquencyTrigger::Passing(double CurrentSize, const QDate& CurrentPeriod) const {
	LOGDEBUG(QString("Delinquent Share: %1% Target: %2%").arg(CurrentSize*100.0).arg(m_Target.GetValue(CurrentPeriod)*100.0));
	return m_Target.GetValue(CurrentPeriod) > CurrentSize;
}
bool DelinquencyTrigger::Passing(double CurrentSize, int CurrentPeriod) const {
	LOGDEBUG(QString("Delinquent Share: %1% Target: %2%").arg(CurrentSize*100.0).arg(m_Target.GetValue(CurrentPeriod)*100.0));
	return m_Target.GetValue(CurrentPeriod) > CurrentSize;
}
QString DelinquencyTrigger::ReadyToCalculate() const {
	if (m_Target.IsEmpty(0.0, 1.0)) 
		return "Invalid Delinquency Trigger Range"; 
	return QString();
}
