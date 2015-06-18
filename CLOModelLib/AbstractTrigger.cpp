#include "AbstractTrigger.h"
#include "AbstractTrigger_p.h"
#include <QDataStream>
QDataStream& operator>>(QDataStream & stream, AbstractTrigger& flows) {
	return flows.LoadOldVersion(stream);
}
QDataStream& operator<<(QDataStream & stream, const AbstractTrigger& flows) {
	flows.WriteToStream(stream);
    stream << static_cast<quint8>(flows.d_func()->m_TriggerType) << flows.d_func()->m_TriggerLabel;
	return stream;
}
QDataStream& AbstractTrigger::LoadOldVersion(QDataStream& stream) {
    Q_D(AbstractTrigger);
	quint8 TempChar;
	stream >> TempChar >> d->m_TriggerLabel;
    d->m_TriggerType = static_cast<AbstractTrigger::TriggerType>(TempChar);
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
	case TriggerType::DuringStressTestTrigger: return "During Stress Test Trigger";
	case TriggerType::CumulativeLossTrigger: return "Cumulative Loss Trigger";
	case TriggerType::DeferredInterestTrigger: return "Deferred Interest Trigger";
    case TriggerType::PDLTrigger: return "PDL Trigger";
	default: return QString();
	}
}

AbstractTrigger::AbstractTrigger(const AbstractTrigger& a) 
    : BackwardInterface(new AbstractTriggerPrivate(this,*a.d_func()))
{}

AbstractTrigger::AbstractTrigger(TriggerType TTP, const QString& lab /*= QString()*/) 
    : BackwardInterface(new AbstractTriggerPrivate(this))
{
    Q_D(AbstractTrigger);
    d->m_TriggerType = TTP;
    d->m_TriggerLabel = lab;
}

AbstractTrigger::TriggerType AbstractTrigger::GetTriggerType() const
{
    Q_D(const AbstractTrigger);
    return d->m_TriggerType;
}

const QString& AbstractTrigger::GetTriggerLabel() const
{
    Q_D(const AbstractTrigger);
    return d->m_TriggerLabel;
}

void AbstractTrigger::SetTriggerLabel(const QString& lab)
{
    Q_D(AbstractTrigger);
    d->m_TriggerLabel = lab;
}

QString AbstractTrigger::ToString() const
{
    Q_D(const AbstractTrigger);
    return "Label: " + d->m_TriggerLabel + "\nTrigger type: " + TriggerTypeToString(d->m_TriggerType);
}

AbstractTriggerPrivate::AbstractTriggerPrivate(AbstractTrigger* q, const AbstractTriggerPrivate& other)
    :BackwardInterfacePrivate(q, other)
    , m_TriggerType(other.m_TriggerType)
    , m_TriggerLabel(other.m_TriggerLabel)
{}

AbstractTriggerPrivate::AbstractTriggerPrivate(AbstractTrigger* q)
    : BackwardInterfacePrivate(q)
{}

AbstractTriggerPrivate& AbstractTriggerPrivate::operator=(const AbstractTriggerPrivate& other)
{
    BackwardInterfacePrivate::operator=(other);
    m_TriggerType = other.m_TriggerType;
    m_TriggerLabel = other.m_TriggerLabel;
    return *this;
}