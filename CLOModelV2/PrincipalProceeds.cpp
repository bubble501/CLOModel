#include "PrincipalProceeds.h"
#include <QDataStream>
QDataStream& PrincipalRecip::LoadOldVersion(QDataStream& stream) {
	stream >> m_Scheduled >> m_Prepay;
	ResetProtocolVersion();
	return stream;
}
QDataStream& operator<<(QDataStream & stream, const PrincipalRecip& flows) {
	stream << flows.m_Scheduled << flows.m_Prepay;
	return stream;
}
QDataStream& operator>>(QDataStream & stream, PrincipalRecip& flows) {
	return flows.LoadOldVersion(stream);
}
