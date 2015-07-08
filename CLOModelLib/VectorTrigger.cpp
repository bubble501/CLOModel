#include "VectorTrigger.h"
#include "Private/VectorTrigger_p.h"
#include "Private/VectorTriggerSettingWidget.h"
#include <QDataStream>
#include <QDate>
DEFINE_PUBLIC_COMMONS(VectorTrigger)
DEFINE_PUBLIC_COMMONS_COPY(VectorTrigger)
VectorTriggerPrivate::VectorTriggerPrivate(VectorTrigger *q,const VectorTriggerPrivate& other)
	:AbstractTriggerPrivate(q,other)
    , m_TrigVector(other.m_TrigVector)
{}
VectorTriggerPrivate::VectorTriggerPrivate(VectorTrigger *q)
	:AbstractTriggerPrivate(q)
{}
VectorTrigger::VectorTrigger(VectorTriggerPrivate *d, const VectorTrigger& other)
	:AbstractTrigger(d,other)
{}
VectorTrigger& VectorTrigger::operator=(const VectorTrigger& other){
	Q_D(VectorTrigger);
	AbstractTrigger::operator=(other);
    d->m_TrigVector = other.d_func()->m_TrigVector;
	return *this;
}
VectorTrigger::VectorTrigger(VectorTriggerPrivate *d)
    :AbstractTrigger(d,TriggerType::VectorTrigger)
{}

VectorTrigger::VectorTrigger(const QString& Vec, const QString& lab)
    :VectorTrigger(BoolVector(Vec), lab)
{}
VectorTrigger::VectorTrigger(const BoolVector& Vec, const QString& lab)
    : VectorTrigger(lab)
{
    Q_D(VectorTrigger);
    d->m_TrigVector = Vec;
}

VectorTrigger::VectorTrigger(const QString& lab)
    : VectorTrigger()
{
    Q_D(VectorTrigger);
    d->m_TriggerLabel = lab;
}

bool VectorTrigger::Passing(const QDate& CurrentDate) const
{
    Q_D(const VectorTrigger);
	if (d->m_TrigVector.IsEmpty())
        return false;
    return d->m_TrigVector.GetValue(CurrentDate);
}
bool VectorTrigger::Passing(int VectorIndex) const
{
    Q_D(const VectorTrigger);
    if (d->m_TrigVector.IsEmpty())
        return false;
    return d->m_TrigVector.GetValue(VectorIndex);
}

bool VectorTrigger::Failing(int VectorIndex) const
{
    return !Passing(VectorIndex);
}

bool VectorTrigger::Failing(const QDate& CurrentDate) const
{
    return !Passing(CurrentDate);
}

QDataStream& VectorTrigger::WriteToStream(QDataStream& stream) const
{
    Q_D(const VectorTrigger);
    stream << d->m_TrigVector;
	return stream;
}
QDataStream& VectorTrigger::LoadOldVersion(QDataStream& stream)
{
    Q_D( VectorTrigger);
    d->m_TrigVector.SetLoadProtocolVersion(loadProtocolVersion()); 
    stream >> d->m_TrigVector;
	return AbstractTrigger::LoadOldVersion(stream);
}



const BoolVector& VectorTrigger::GetTrigVector() const
{
    Q_D(const VectorTrigger);
    return d->m_TrigVector;
}

void VectorTrigger::SetTrigVector(const QString& val)
{
    Q_D( VectorTrigger);
    d->m_TrigVector = val;
}

void VectorTrigger::SetTrigVector(const BoolVector& val)
{
    Q_D( VectorTrigger);
    d->m_TrigVector = val;
}

bool VectorTrigger::IsEmpty() const
{
    Q_D(const VectorTrigger);
    return d->m_TrigVector.IsEmpty();
}

bool VectorTrigger::HasAnchor() const
{
    Q_D(const VectorTrigger);
    return !d->m_TrigVector.GetAnchorDate().isNull();
}

const QDate& VectorTrigger::GetAnchor() const
{
    Q_D(const VectorTrigger);
    return d->m_TrigVector.GetAnchorDate();
}

void VectorTrigger::SetAnchor(const QDate& a)
{
    Q_D( VectorTrigger);
    d->m_TrigVector.SetAnchorDate(a);
}

QString VectorTrigger::ReadyToCalculate() const
{
    Q_D(const VectorTrigger);
    if (d->m_TrigVector.IsEmpty()) 
        return "Trigger " + d->m_TriggerLabel + " is invalid\n";
	return QString();
}

QString VectorTrigger::ToString() const
{
    Q_D(const VectorTrigger);
    return AbstractTrigger::ToString() + "\nVector: " + d->m_TrigVector.GetVector();
}

VectorTriggerSettingWidget* VectorTrigger::createSettingsWidget(QWidget* parent /*= nullptr*/)
{
    return new VectorTriggerSettingWidget(parent);
}
