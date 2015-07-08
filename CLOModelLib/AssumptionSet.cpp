#include "AssumptionSet.h"
#include "Private/AssumptionSet_p.h"
#include <QHash>
#include <QStringList>
#include "BloombergVector.h"
#include "IntegerVector.h"
DEFINE_PUBLIC_COMMONS(AssumptionSet)
DEFINE_PUBLIC_COMMONS_COPY(AssumptionSet)
AssumptionSetPrivate::AssumptionSetPrivate(AssumptionSet *q,const AssumptionSetPrivate& other)
	:BackwardInterfacePrivate(q,other)
    , m_CDRscenario(other.m_CDRscenario)
    , m_CPRscenario(other.m_CPRscenario)
    , m_LSscenario(other.m_LSscenario)
    , m_RecLagScenario(other.m_RecLagScenario)
    , m_DelinqScenario(other.m_DelinqScenario)
    , m_DelinqLagScenario(other.m_DelinqLagScenario)
{}
AssumptionSetPrivate::AssumptionSetPrivate(AssumptionSet *q)
	:BackwardInterfacePrivate(q)
{}
AssumptionSet::AssumptionSet(AssumptionSetPrivate *d, const AssumptionSet& other)
	:BackwardInterface(d,other)
{}
AssumptionSet& AssumptionSet::operator=(const AssumptionSet& a)
{
    Q_D(AssumptionSet);
    d->m_CDRscenario = a.d_func()->m_CDRscenario;
    d->m_CPRscenario = a.d_func()->m_CPRscenario;
    d->m_LSscenario = a.d_func()->m_LSscenario;
    d->m_RecLagScenario = a.d_func()->m_RecLagScenario;
    d->m_DelinqScenario = a.d_func()->m_DelinqScenario;
    d->m_DelinqLagScenario = a.d_func()->m_DelinqLagScenario;
    return *this;
}
AssumptionSet::AssumptionSet(AssumptionSetPrivate *d)
	:BackwardInterface(d)
{}

AssumptionSet::AssumptionSet(
	const QString& CPRscenario
	, const QString& CDRscenario
	, const QString& LSscenario
	, const QString& RecLagScenario
	, const QString& DelinqScenario
	, const QString& DelinqLagScenario
) 
    :AssumptionSet()
{
    Q_D(AssumptionSet);
    d->m_CDRscenario = CDRscenario;
    d->m_CPRscenario = CPRscenario;
    d->m_LSscenario = LSscenario;
    d->m_RecLagScenario = RecLagScenario;
    d->m_DelinqScenario = DelinqScenario;
    d->m_DelinqLagScenario = DelinqLagScenario;
}

AssumptionSet::AssumptionSet(const QString& a) 
    :AssumptionSet()
{
	operator=(a);
}

const QString& AssumptionSet::GetCDRscenario() const
{
    Q_D(const AssumptionSet);
    return d->m_CDRscenario;
}

void AssumptionSet::SetCDRscenario(const QString& val)
{
    Q_D(AssumptionSet);
    d->m_CDRscenario = val;
}

const QString& AssumptionSet::GetDelinqLagScenario() const
{
    Q_D(const AssumptionSet);
    return d->m_DelinqLagScenario;
}

void AssumptionSet::SetDelinqLagScenario(const QString& val)
{
    Q_D(AssumptionSet);
    d->m_DelinqLagScenario = val;
}

const QString& AssumptionSet::GetDelinqScenario() const
{
    Q_D(const AssumptionSet);
    return d->m_DelinqScenario;
}

void AssumptionSet::SetDelinqScenario(const QString& val)
{
    Q_D( AssumptionSet);
    d->m_DelinqScenario = val;
}

const QString& AssumptionSet::GetRecLagScenario() const
{
    Q_D(const AssumptionSet);
    return d->m_RecLagScenario;
}

void AssumptionSet::SetRecLagScenario(const QString& val)
{
    Q_D(AssumptionSet);
    d->m_RecLagScenario = val;
}

const QString& AssumptionSet::GetLSscenario() const
{
    Q_D(const AssumptionSet);
    return d->m_LSscenario;
}

void AssumptionSet::SetLSscenario(const QString& val)
{
    Q_D( AssumptionSet);
    d->m_LSscenario = val;
}

const QString& AssumptionSet::GetCPRscenario() const
{
    Q_D(const AssumptionSet);
    return d->m_CPRscenario;
}

void AssumptionSet::SetCPRscenario(const QString& val)
{
    Q_D(AssumptionSet);
    d->m_CPRscenario = val;
}



AssumptionSet& AssumptionSet::operator=(const QString& a) {
    Q_D(AssumptionSet);
	QStringList Parts = a.split("#,#",QString::SkipEmptyParts);
	if (Parts.size() == 6) {
        d->m_CPRscenario = Parts[0].trimmed().toUpper();
        d->m_CDRscenario = Parts[1].trimmed().toUpper();
        d->m_LSscenario = Parts[2].trimmed().toUpper();
        d->m_RecLagScenario = Parts[3].trimmed().toUpper();
        d->m_DelinqScenario = Parts[4].trimmed().toUpper();
        d->m_DelinqLagScenario = Parts[5].trimmed().toUpper();
	}
	return *this;
}

QString AssumptionSet::ToString() const {
    Q_D(const AssumptionSet);
    return d->m_CPRscenario + "#,#" +
        d->m_CDRscenario + "#,#" +
        d->m_LSscenario + "#,#" +
        d->m_RecLagScenario + "#,#" +
        d->m_DelinqScenario + "#,#" +
        d->m_DelinqLagScenario
		;
}

QDataStream& AssumptionSet::LoadOldVersion(QDataStream& stream) {
    Q_D(AssumptionSet);
	stream
		>> d->m_CDRscenario
        >> d->m_CPRscenario
        >> d->m_LSscenario
        >> d->m_RecLagScenario
        >> d->m_DelinqScenario
        >> d->m_DelinqLagScenario
	;
	return stream;
}

bool AssumptionSet::IsValid() const {
    Q_D(const AssumptionSet);
    if (BloombergVector(d->m_CDRscenario).IsEmpty(0.0, 1.0)) return false;
    if (BloombergVector(d->m_CPRscenario).IsEmpty(0.0, 1.0)) return false;
    if (BloombergVector(d->m_LSscenario).IsEmpty(0.0, 1.0)) return false;
    if (BloombergVector(d->m_DelinqScenario).IsEmpty(0.0, 1.0)) return false;
    if (IntegerVector(d->m_RecLagScenario).IsEmpty(0)) return false;
    if (IntegerVector(d->m_DelinqLagScenario).IsEmpty(0)) return false;
	return true;
}

bool operator==(const AssumptionSet &e1, const AssumptionSet &e2) {
	return
        e1.d_func()->m_CDRscenario == e2.d_func()->m_CDRscenario &&
        e1.d_func()->m_CPRscenario == e2.d_func()->m_CPRscenario &&
        e1.d_func()->m_LSscenario == e2.d_func()->m_LSscenario &&
        e1.d_func()->m_RecLagScenario == e2.d_func()->m_RecLagScenario &&
        e1.d_func()->m_DelinqScenario == e2.d_func()->m_DelinqScenario &&
        e1.d_func()->m_DelinqLagScenario == e2.d_func()->m_DelinqLagScenario
	;
}
uint qHash(const AssumptionSet& key, uint seed) {
	return qHash(key.ToString(), seed);
}

QDataStream& operator<<(QDataStream & stream, const AssumptionSet& flows) {
	stream
        << flows.d_func()->m_CDRscenario
        << flows.d_func()->m_CPRscenario
        << flows.d_func()->m_LSscenario
        << flows.d_func()->m_RecLagScenario
        << flows.d_func()->m_DelinqScenario
        << flows.d_func()->m_DelinqLagScenario
	;
	return stream;
}

QDataStream& operator>>(QDataStream & stream, AssumptionSet& flows) {
	return flows.LoadOldVersion(stream);
}
