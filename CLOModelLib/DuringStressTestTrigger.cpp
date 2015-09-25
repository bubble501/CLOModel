#include "DuringStressTestTrigger.h"
#include "Private/AbstractTrigger_p.h"
#include "Private/AbstractTriggerSettingWidget.h"
DuringStressTestTrigger::~DuringStressTestTrigger()
{

}
DuringStressTestTrigger::DuringStressTestTrigger(AbstractTriggerPrivate* d)
    :AbstractTrigger(d, TriggerType::DuringStressTestTrigger)
{}

DuringStressTestTrigger::DuringStressTestTrigger(const QString& lab) 
    : DuringStressTestTrigger(new AbstractTriggerPrivate(this))
{
   d_func()->m_TriggerLabel=lab;
}
// If you change this change #PublicCopies
DuringStressTestTrigger::DuringStressTestTrigger(const DuringStressTestTrigger& other) 
    : DuringStressTestTrigger(new AbstractTriggerPrivate(this,*other.d_func()),other)
{}

DuringStressTestTrigger::DuringStressTestTrigger(AbstractTriggerPrivate* d, const DuringStressTestTrigger& other)
    : AbstractTrigger(d, other)
{}

bool DuringStressTestTrigger::Passing(bool IsStressTest) const
{
    return IsStressTest;
}

bool DuringStressTestTrigger::Failing(bool IsStressTest) const
{
    return !Passing(IsStressTest);
}

QString DuringStressTestTrigger::ReadyToCalculate() const
{
    return QString();
}

AbstractTriggerSettingWidget* DuringStressTestTrigger::createSettingsWidget(QWidget* parent /*= nullptr*/)
{
    return new AbstractTriggerSettingWidget(parent);
}

QDataStream& DuringStressTestTrigger::WriteToStream(QDataStream& stream) const
{
    return stream;
}
