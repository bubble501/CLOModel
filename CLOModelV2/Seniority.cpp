#include "Seniority.h"
#include <QStringList>
bool Seniority::SetCurrentLevel(int a) {
	if (a < 0 || a >= m_SeniorityScale.size()) return false; 
	m_CurrnetLevel = a;
	return true;
}

bool Seniority::AddSeniorityLevel(quint32 a) {
	if (a == 0) return false; 
	if (m_SeniorityScale.isEmpty()) m_CurrnetLevel = 0;
	m_SeniorityScale.append(a); 
	return true;
}

bool Seniority::SetSeniorityScale(const QString& a) {
	Clear();
	QStringList TempLevels = a.split(SeniorityStringSeparator);
	bool AllGood = true;
	quint32 TempInt;
	for (auto i = TempLevels.constBegin(); i != TempLevels.constEnd() && AllGood; ++i) {
		TempInt=i->toUInt(&AllGood);
		if (AllGood) AllGood = AddSeniorityLevel(TempInt);
	}
	return AllGood;
}
QDataStream& operator<<(QDataStream & stream, const Seniority& flows) {
	stream
		<< flows.m_SeniorityScale
		<< flows.m_CurrnetLevel
		;
	return stream;
}
QDataStream& Seniority::LoadOldVersion(QDataStream& stream) {
	stream
		>> m_SeniorityScale
		>> m_CurrnetLevel
		;
	ResetProtocolVersion();
	return stream;
}

QString Seniority::ToString() const {
	QString Result = "";
	foreach(const quint32& SingleLevel, m_SeniorityScale) {
		if (!Result.isEmpty()) Result.append(SeniorityStringSeparator);
		Result.append(QString::number(SingleLevel));
	}
	return Result;
}

QDataStream& operator>>(QDataStream & stream, Seniority& flows) {
	return flows.LoadOldVersion(stream);
}