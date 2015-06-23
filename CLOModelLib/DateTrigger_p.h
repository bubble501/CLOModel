#include "Private/AbstractTrigger_p.h"
#include "DateTrigger.h"
#include <QDate>
class DateTriggerPrivate : public AbstractTriggerPrivate
{
    DECLARE_PRIVATE_COMMONS(DateTrigger)
public:
    QDate m_LimitDate;
    DateTrigger::TriggerSide m_Side;
};