#include "BackwardCompatibilityInterface_p.h"
#include "AbstractTrigger.h"
class AbstractTriggerPrivate : public BackwardInterfacePrivate
{
    DECLARE_PRIVATE_COMMONS(AbstractTrigger)
    DECLARE_PRIVATE_COMMONS_DATASTREAM(AbstractTrigger)
public:
    AbstractTrigger::TriggerType m_TriggerType;
    QString m_TriggerLabel;

};