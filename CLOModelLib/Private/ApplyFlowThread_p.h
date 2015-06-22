#ifndef ApplyFlowThread_p_h__
#define ApplyFlowThread_p_h__

#include "ApplyFlowThread.h"
class ApplyFlowThreadPrivate : public AbstrAsyncThreadPrivate
{
    DECLARE_PRIVATE_COMMONS(ApplyFlowThread)
public:
    const MtgCashFlow* BaseFlow;
    AssumptionSet AssumptionsToApply;
};
#endif // ApplyFlowThread_p_h__