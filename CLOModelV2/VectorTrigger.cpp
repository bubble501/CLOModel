#include "VectorTrigger.h"
#include <QDataStream>
bool VectorTrigger::Passing(const QDate& CurrentDate) const {
	if (m_TrigVector.IsEmpty()) return false;
	return m_TrigVector.GetValue(CurrentDate);
}
bool VectorTrigger::Passing(int VectorIndex) const {
	if (m_TrigVector.IsEmpty()) return false;
	return m_TrigVector.GetValue(VectorIndex);
}

QDataStream& VectorTrigger::WriteToStream(QDataStream& stream) const {
	stream << m_TrigVector;
	return stream;
}
QDataStream& VectorTrigger::LoadOldVersion(QDataStream& stream) {
	m_TrigVector.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_TrigVector;
	return AbstractTrigger::LoadOldVersion(stream);
}

VectorTrigger::VectorTrigger(const QString& Vec, const QString& lab)
	:AbstractTrigger(TriggerType::VectorTrigger, lab)
	, m_TrigVector(Vec)
{}
VectorTrigger::VectorTrigger(const BoolVector& Vec, const QString& lab)
	: AbstractTrigger(TriggerType::VectorTrigger, lab)
	, m_TrigVector(Vec) 
{}

VectorTrigger::VectorTrigger(const QString& lab)
	:AbstractTrigger(TriggerType::VectorTrigger, lab)
{}
QString VectorTrigger::ReadyToCalculate() const {
	if (m_TrigVector.IsEmpty()) return "Trigger " + m_TriggerLabel + " is invalid\n";
	return QString();
}