#ifndef LoanAssMatcherThread_p_h__
#define LoanAssMatcherThread_p_h__

#include "Private/AbstrAsyncThread_p.h"
#include "LoanAssMatcherResult.h"
#include "LoanAssumption.h"
#include <QHash>
#include <memory>
#include "LoanAssMatcherThread.h"
class LoanAssMatcherThreadPrivate : public AbstrAsyncThreadPrivate
{
    DECLARE_PRIVATE_COMMONS(LoanAssMatcherThread)
public:
    QString m_ModelToScan;
    LoanAssMatcherResult Result;
    const QHash<QString, std::shared_ptr<LoanAssumption> >* m_AvailableAssumptions;
};
#endif // LoanAssMatcherThread_p_h__