#ifndef TrancheTrigger_p_h__
#define TrancheTrigger_p_h__

#include "Private/AbstractTrigger_p.h"
#include "TrancheTrigger.h"
#include "IntegerVector.h"
#include "BloombergVector.h"
class TrancheTriggerPrivate : public AbstractTriggerPrivate
{
    DECLARE_PRIVATE_COMMONS(TrancheTrigger)
    DECLARE_PRIVATE_COMMONS_COPY(TrancheTrigger)
public:
    IntegerVector m_TargetSeniorityLevel;
    IntegerVector m_TargetSeniority;
    TrancheTrigger::TriggerSizeSide m_SizeSide;
    TrancheTrigger::TriggerSenioritySide m_SenioritySide;
    BloombergVector m_TargetSize;
};
#endif // TrancheTrigger_p_h__