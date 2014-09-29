#include "AbstractTrigger.h"
#include <QDataStream>
QDataStream& operator>>(QDataStream & stream, AbstractTrigger& flows) {
	return flows.LoadOldVersion(stream);
}
QDataStream& operator<<(QDataStream & stream, const AbstractTrigger& flows) {
	stream << static_cast<quint8>(flows.m_TriggerType) << flows.m_TriggerLabel;
	return flows.WriteToStream(stream);
}
QDataStream& AbstractTrigger::LoadOldVersion(QDataStream& stream) {
	quint8 TempChar;
	stream >> TempChar >> m_TriggerLabel;
	m_TriggerType = static_cast<AbstractTrigger::TriggerType>(TempChar);
	ResetProtocolVersion();
	return stream;
}

