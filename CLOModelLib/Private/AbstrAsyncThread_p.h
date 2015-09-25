#ifndef AbstrAsyncThread_p_h__
#define AbstrAsyncThread_p_h__
#include "AbstrAsyncThread.h"
class AbstrAsyncThreadPrivate
{
    DECLARE_PRIVATE_COMMONS(AbstrAsyncThread)
protected:
    AbstrAsyncThread* q_ptr;
public:
    int Identifier;
    std::shared_ptr<void> m_Result;
};
#endif // AbstrAsyncThread_p_h__