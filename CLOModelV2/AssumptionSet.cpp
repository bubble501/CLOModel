#include "AssumptionSet.h"
#include <QHash>
#include <QStringList>

AssumptionSet::AssumptionSet(
	const QString& CPRscenario
	, const QString& CDRscenario
	, const QString& LSscenario
	, const QString& RecLagScenario
	, const QString& DelinqScenario
	, const QString& DelinqLagScenario
) 
	: m_CDRscenario(CDRscenario)
	, m_CPRscenario(CPRscenario)
	, m_LSscenario(LSscenario)
	, m_RecLagScenario(RecLagScenario)
	, m_DelinqScenario(DelinqScenario)
	, m_DelinqLagScenario(DelinqLagScenario)
{}

AssumptionSet::AssumptionSet(const AssumptionSet& a) 
	: m_CDRscenario(a.m_CDRscenario)
	, m_CPRscenario(a.m_CPRscenario)
	, m_LSscenario(a.m_LSscenario)
	, m_RecLagScenario(a.m_RecLagScenario)
	, m_DelinqScenario(a.m_DelinqScenario)
	, m_DelinqLagScenario(a.m_DelinqLagScenario)
{}

AssumptionSet::AssumptionSet(const QString& a) {
	operator=(a);
}

AssumptionSet& AssumptionSet::operator=(const AssumptionSet& a) {
	m_CDRscenario = a.m_CDRscenario;
	m_CPRscenario = a.m_CPRscenario;
	m_LSscenario = a.m_LSscenario;
	m_RecLagScenario = a.m_RecLagScenario;
	m_DelinqScenario = a.m_DelinqScenario;
	m_DelinqLagScenario = a.m_DelinqLagScenario;
	return *this;
}

AssumptionSet& AssumptionSet::operator=(const QString& a) {
	QStringList Parts = a.split("#,#",QString::SkipEmptyParts);
	if (Parts.size() == 6) {
		m_CPRscenario = Parts[0].trimmed().toUpper();
		m_CDRscenario = Parts[1].trimmed().toUpper();
		m_LSscenario = Parts[2].trimmed().toUpper();
		m_RecLagScenario = Parts[3].trimmed().toUpper();
		m_DelinqScenario = Parts[4].trimmed().toUpper();
		m_DelinqLagScenario = Parts[5].trimmed().toUpper();
	}
	return *this;
}

QString AssumptionSet::ToString() const {
	return m_CPRscenario + "#,#" +
		m_CDRscenario + "#,#" +
		m_LSscenario + "#,#" +
		m_RecLagScenario + "#,#" +
		m_DelinqScenario + "#,#" +
		m_DelinqLagScenario
		;
}

QDataStream& AssumptionSet::LoadOldVersion(QDataStream& stream) {
	stream
		>> m_CDRscenario
		>> m_CPRscenario
		>> m_LSscenario
		>> m_RecLagScenario
		>> m_DelinqScenario
		>> m_DelinqLagScenario
	;
	return stream;
}

bool operator==(const AssumptionSet &e1, const AssumptionSet &e2) {
	return
		e1.m_CDRscenario == e2.m_CDRscenario &&
		e1.m_CPRscenario == e2.m_CPRscenario &&
		e1.m_LSscenario == e2.m_LSscenario &&
		e1.m_RecLagScenario == e2.m_RecLagScenario &&
		e1.m_DelinqScenario == e2.m_DelinqScenario &&
		e1.m_DelinqLagScenario == e2.m_DelinqLagScenario
	;
}
uint qHash(const AssumptionSet& key, uint seed) {
	return qHash(key.ToString(), seed);
}

QDataStream& operator<<(QDataStream & stream, const AssumptionSet& flows) {
	stream
		<< flows.m_CDRscenario
		<< flows.m_CPRscenario
		<< flows.m_LSscenario
		<< flows.m_RecLagScenario
		<< flows.m_DelinqScenario
		<< flows.m_DelinqLagScenario
	;
	return stream;
}

QDataStream& operator>>(QDataStream & stream, AssumptionSet& flows) {
	return flows.LoadOldVersion(stream);
}
