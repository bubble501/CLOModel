#ifndef DelinquencyTrigger_p_h__
#define DelinquencyTrigger_p_h__
#include "Private/AbstractTrigger_p.h"
#include "DelinquencyTrigger.h"
#include "BloombergVector.h"
class DelinquencyTriggerPrivate : public AbstractTriggerPrivate
{
    DECLARE_PRIVATE_COMMONS(DelinquencyTrigger)
    DECLARE_PRIVATE_COMMONS_COPY(DelinquencyTrigger)
public:
    BloombergVector m_Target;
};
#endif // DelinquencyTrigger_p_h__