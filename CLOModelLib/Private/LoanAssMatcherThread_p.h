#ifndef LoanAssMatcherThread_p_h__
#define LoanAssMatcherThread_p_h__

#include "ApplyFlowThread.h"
#include "Private/AbstrAsyncThread_p.h"
#include "LoanAssMatcherResult.h"
#include "LoanAssumption.h"
#include <QHash>
#include <QSharedPointer>
#include "LoanAssMatcherThread.h"
class LoanAssMatcherThreadPrivate : public AbstrAsyncThreadPrivate
{
    DECLARE_PRIVATE_COMMONS(LoanAssMatcherThread)
public:
    QString m_ModelToScan;
    LoanAssMatcherResult Result;
    const QHash<QString, QSharedPointer<LoanAssumption> >* m_AvailableAssumptions;
};
#endif // LoanAssMatcherThread_p_h__