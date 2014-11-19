#include "TriggersResults.h"
#include <QDataStream>
void TriggersResults::ClearResults() {
	for (auto i = m_Results.begin(); i != m_Results.end(); ++i)
		delete i.value();
	m_Results.clear();
}

TriggersResults::TrigRes TriggersResults::GetResult(quint32 TrigType, const QDate& RefDate)const {
	const QMap<QDate, bool>* Tempmap = m_Results.value(TrigType, nullptr);
	if (Tempmap) {
		QMap<QDate, bool>::const_iterator MapIter = Tempmap->constFind(RefDate);
		if (MapIter == Tempmap->constEnd()) return TrigRes::trNA;
		return (MapIter.value() ? TrigRes::trTrue : TrigRes::trFalse);
	}
	return TrigRes::trNA;
}

void TriggersResults::SetResult(quint32 TrigType, const QDate& RefDate, bool Res) {
	QHash<quint32, QMap<QDate, bool>* >::iterator hashIter = m_Results.find(TrigType);
	if (hashIter==m_Results.end()) {
		hashIter=m_Results.insert(TrigType, new QMap<QDate, bool>());
	}
	hashIter.value()->operator[](RefDate) = Res;
}

QDataStream& TriggersResults::LoadOldVersion(QDataStream& stream) {
	ClearResults();
	qint32 tempInt, tempKey;
	QMap<QDate, bool> Tempmap;
	stream >> tempInt;
	for (qint32 i = 0; i < tempInt; i++) {
		Tempmap.clear();
		stream >> tempKey >> Tempmap;
		m_Results.insert(tempKey, new QMap<QDate, bool>(Tempmap));
	}
	ResetProtocolVersion();
	return stream;
}

TriggersResults::TriggersResults(const TriggersResults& a) {
	for (auto i = a.m_Results.constBegin(); i != a.m_Results.constEnd(); ++i) {
		m_Results.insert(i.key(), new QMap<QDate, bool>(*(i.value())));
	}
}

TriggersResults& TriggersResults::operator=(const TriggersResults& a) {
	ClearResults();
	for (auto i = a.m_Results.constBegin(); i != a.m_Results.constEnd(); ++i) {
		m_Results.insert(i.key(), new QMap<QDate, bool>(*(i.value())));
	}
	return *this;
}


QDataStream& operator<<(QDataStream & stream, const TriggersResults& flows) {
	stream << static_cast<qint32>(flows.m_Results.size());
	for (auto i = flows.m_Results.constBegin(); i != flows.m_Results.constEnd(); ++i) {
		stream << i.key() << *(i.value());
	}
	return stream;
}

QDataStream& operator>>(QDataStream & stream, TriggersResults& flows) {
	return flows.LoadOldVersion(stream);
}
