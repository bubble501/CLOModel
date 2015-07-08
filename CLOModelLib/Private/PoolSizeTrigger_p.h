#ifndef PoolSizeTrigger_p_h__
#define PoolSizeTrigger_p_h__

#include "Private/AbstractTrigger_p.h"
#include "PoolSizeTrigger.h"
#include "BloombergVector.h"
class PoolSizeTriggerPrivate : public AbstractTriggerPrivate
{
    DECLARE_PRIVATE_COMMONS(PoolSizeTrigger)
    DECLARE_PRIVATE_COMMONS_COPY(PoolSizeTrigger)
public:
    BloombergVector m_TargetSize;
    PoolSizeTrigger::TriggerSide m_Side;
};
#endif // PoolSizeTrigger_p_h__