#include "DelinquencyTrigger.h"
#include "Private/DelinquencyTrigger_p.h"
#include <QDataStream>
DEFINE_PUBLIC_COMMONS(DelinquencyTrigger)
DEFINE_PUBLIC_COMMONS_COPY(DelinquencyTrigger)
DelinquencyTriggerPrivate::DelinquencyTriggerPrivate(DelinquencyTrigger *q,const DelinquencyTriggerPrivate& other)
	:AbstractTriggerPrivate(q,other)
    , m_Target(other.m_Target)
{}
DelinquencyTriggerPrivate::DelinquencyTriggerPrivate(DelinquencyTrigger *q)
	:AbstractTriggerPrivate(q)
{}
DelinquencyTrigger::DelinquencyTrigger(DelinquencyTriggerPrivate *d, const DelinquencyTrigger& other)
	:AbstractTrigger(d,other)
{}
DelinquencyTrigger& DelinquencyTrigger::operator=(const DelinquencyTrigger& other){
	Q_D(DelinquencyTrigger);
	AbstractTrigger::operator=(other);
    d->m_Target = other.d_func()->m_Target;
	return *this;
}
DelinquencyTrigger::DelinquencyTrigger(DelinquencyTriggerPrivate *d)
    :AbstractTrigger(d, TriggerType::DelinquencyTrigger)
{}


QDataStream& DelinquencyTrigger::WriteToStream(QDataStream& stream) const {
    Q_D(const DelinquencyTrigger);
	stream
		<< d->m_Target
		;
    return stream;
}
QDataStream& DelinquencyTrigger::LoadOldVersion(QDataStream& stream) {
    Q_D( DelinquencyTrigger);
    d->m_Target.SetLoadProtocolVersion(loadProtocolVersion());
	stream >> d->m_Target;
	return AbstractTrigger::LoadOldVersion(stream);
}
DelinquencyTrigger::DelinquencyTrigger(const BloombergVector& Target, const QString& lab)
    :DelinquencyTrigger(lab)
{
    Q_D(DelinquencyTrigger);
    d->m_Target = Target;
}
DelinquencyTrigger::DelinquencyTrigger(const QString& Target, const QString& lab)
    : DelinquencyTrigger(BloombergVector(Target), lab)
{}
DelinquencyTrigger::DelinquencyTrigger(const QString& lab)
    : DelinquencyTrigger()
{
    Q_D(DelinquencyTrigger);
    d->m_TriggerLabel = lab;
}

bool DelinquencyTrigger::Passing(double CurrentSize, const QDate& CurrentPeriod) const
{
    Q_D(const DelinquencyTrigger);
    return d->m_Target.GetValue(CurrentPeriod) > CurrentSize;
}
bool DelinquencyTrigger::Passing(double CurrentSize, int CurrentPeriod) const
{
    Q_D(const DelinquencyTrigger);
    return d->m_Target.GetValue(CurrentPeriod) > CurrentSize;
}

bool DelinquencyTrigger::Failing(double CurrentDelinq, const QDate& CurrentPeriod) const
{
    return !Passing(CurrentDelinq, CurrentPeriod);
}

bool DelinquencyTrigger::Failing(double CurrentDelinq, int CurrentPeriod) const
{
    return !Passing(CurrentDelinq, CurrentPeriod);
}

QString DelinquencyTrigger::ReadyToCalculate() const
{
    Q_D(const DelinquencyTrigger);
    if (d->m_Target.IsEmpty(0.0, 1.0))
		return "Invalid Delinquency Trigger Range"; 
	return QString();
}

const BloombergVector& DelinquencyTrigger::GetTarget() const
{
    Q_D(const DelinquencyTrigger);
    return d->m_Target;
}

void DelinquencyTrigger::SetTarget(const BloombergVector& val)
{
    Q_D( DelinquencyTrigger);
    d->m_Target = val;
}

void DelinquencyTrigger::SetTarget(const QString& val)
{
    Q_D( DelinquencyTrigger);
    d->m_Target = val;
}

void DelinquencyTrigger::SetAnchorDate(const QDate& a)
{
    Q_D( DelinquencyTrigger);
    d->m_Target.SetAnchorDate(a);
}

void DelinquencyTrigger::RemoveAnchorDate()
{
    Q_D( DelinquencyTrigger);
    d->m_Target.RemoveAnchorDate();
}

bool DelinquencyTrigger::HasAnchor() const
{
    Q_D(const DelinquencyTrigger);
    return !d->m_Target.GetAnchorDate().isNull();
}

QString DelinquencyTrigger::ToString() const
{
    Q_D(const DelinquencyTrigger);
    return AbstractTrigger::ToString() + "\nDelinquency Limit: " + d->m_Target.GetVector();
}
