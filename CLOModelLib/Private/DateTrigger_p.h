#ifndef DateTrigger_p_h__
#define DateTrigger_p_h__

#include "Private/AbstractTrigger_p.h"
#include "DateTrigger.h"
#include <QDate>
class DateTriggerPrivate : public AbstractTriggerPrivate
{
    DECLARE_PRIVATE_COMMONS(DateTrigger)
    DECLARE_PRIVATE_COMMONS_COPY(DateTrigger)
public:
    QDate m_LimitDate;
    DateTrigger::TriggerSide m_Side;
};
#endif // DateTrigger_p_h__