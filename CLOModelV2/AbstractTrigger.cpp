#include "AbstractTrigger.h"
#include <QDataStream>
QDataStream& operator>>(QDataStream & stream, AbstractTrigger& flows) {
	return flows.LoadOldVersion(stream);
}
QDataStream& operator<<(QDataStream & stream, const AbstractTrigger& flows) {
	flows.WriteToStream(stream);
	stream << static_cast<quint8>(flows.m_TriggerType) << flows.m_TriggerLabel;
	return stream;
}
QDataStream& AbstractTrigger::LoadOldVersion(QDataStream& stream) {
	quint8 TempChar;
	stream >> TempChar >> m_TriggerLabel;
	m_TriggerType = static_cast<AbstractTrigger::TriggerType>(TempChar);
	ResetProtocolVersion();
	return stream;
}

QString AbstractTrigger::TriggerTypeToString(TriggerType a)const {
	switch (a){
	case TriggerType::DateTrigger: return "Date Trigger";
	case TriggerType::VectorTrigger: return "Vector Trigger";
	case TriggerType::PoolSizeTrigger: return "Pool Size Trigger";
	case TriggerType::TrancheTrigger: return "Tranche Trigger";
	case TriggerType::DelinquencyTrigger: return "Delinquency Trigger";
	default: return QString();
	}
}

