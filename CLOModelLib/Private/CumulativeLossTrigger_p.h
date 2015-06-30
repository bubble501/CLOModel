#ifndef CumulativeLossTrigger_p_h__
#define CumulativeLossTrigger_p_h__

#include "Private/AbstractTrigger_p.h"
#include "CumulativeLossTrigger.h"
#include "BloombergVector.h"
class CumulativeLossTriggerPrivate : public AbstractTriggerPrivate
{
    DECLARE_PRIVATE_COMMONS(CumulativeLossTrigger)
    DECLARE_PRIVATE_COMMONS_COPY(CumulativeLossTrigger)
public:
    BloombergVector m_TargetSize;
    CumulativeLossTrigger::TriggerSide m_Side;
};
#endif // CumulativeLossTrigger_p_h__