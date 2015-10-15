#include "OnMaturityTrigger.h"
#include "Private/AbstractTrigger_p.h"
#include "Private/AbstractTriggerSettingWidget.h"
OnMaturityTrigger::~OnMaturityTrigger()
{

}
OnMaturityTrigger::OnMaturityTrigger(AbstractTriggerPrivate* d)
    :AbstractTrigger(d, TriggerType::OnMaturityTrigger)
{}

OnMaturityTrigger::OnMaturityTrigger(const QString& lab)
    : OnMaturityTrigger(new AbstractTriggerPrivate(this))
{
    d_func()->m_TriggerLabel = lab;
}
// If you change this change #PublicCopies
OnMaturityTrigger::OnMaturityTrigger(const OnMaturityTrigger& other)
    : OnMaturityTrigger(new AbstractTriggerPrivate(this, *other.d_func()), other)
{}

OnMaturityTrigger::OnMaturityTrigger(AbstractTriggerPrivate* d, const OnMaturityTrigger& other)
    : AbstractTrigger(d, other)
{}

bool OnMaturityTrigger::Passing(bool IsStressTest) const
{
    return IsStressTest;
}

bool OnMaturityTrigger::Failing(bool IsStressTest) const
{
    return !Passing(IsStressTest);
}

QString OnMaturityTrigger::ReadyToCalculate() const
{
    return QString();
}

AbstractTriggerSettingWidget* OnMaturityTrigger::createSettingsWidget(QWidget* parent /*= nullptr*/)
{
    return new AbstractTriggerSettingWidget(parent);
}

QDataStream& OnMaturityTrigger::WriteToStream(QDataStream& stream) const
{
    return stream;
}
