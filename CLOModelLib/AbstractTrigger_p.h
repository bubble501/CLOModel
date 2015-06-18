#include "BackwardCompatibilityInterface_p.h"
#include "AbstractTrigger.h"
class AbstractTriggerPrivate : public BackwardInterfacePrivate
{
    Q_DECLARE_PUBLIC(AbstractTrigger)
private:
    AbstractTriggerPrivate(const AbstractTriggerPrivate& other);
public:
    AbstractTriggerPrivate(AbstractTrigger* q);
    AbstractTriggerPrivate(AbstractTrigger* q, const AbstractTriggerPrivate& other);
    virtual AbstractTriggerPrivate& operator=(const AbstractTriggerPrivate& other);
protected:
    AbstractTrigger::TriggerType m_TriggerType;
    QString m_TriggerLabel;
    friend QDataStream& operator<<(QDataStream & stream, const AbstractTrigger& flows);
    friend QDataStream& operator>>(QDataStream & stream, AbstractTrigger& flows);
};