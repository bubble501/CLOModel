#ifndef VectorTrigger_p_h__
#define VectorTrigger_p_h__

#include "Private/AbstractTrigger_p.h"
#include "VectorTrigger.h"
#include "BoolVector.h"
class VectorTriggerPrivate : public AbstractTriggerPrivate
{
    DECLARE_PRIVATE_COMMONS(VectorTrigger)
    DECLARE_PRIVATE_COMMONS_COPY(VectorTrigger)
public:
    BoolVector m_TrigVector;
};
#endif // VectorTrigger_p_h__