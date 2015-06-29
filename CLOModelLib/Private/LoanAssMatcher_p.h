#ifndef LoanAssMatcher_p_h__
#define LoanAssMatcher_p_h__
#include "Private/AbstrAsyncCalculator_p.h"
#include "LoanAssMatcher.h"
#include <QFileInfoList>
#include <QHash>
#include <QSharedPointer>
#include "LoanAssumption.h"
class LoanAssMatcherPrivate : public AbstrAsyncCalculatorPrivate
{
    DECLARE_PRIVATE_COMMONS(LoanAssMatcher)
    DECLARE_PRIVATE_COMMONS_DATASTREAM(LoanAssMatcher)
public:
    QFileInfoList m_FilesInFolder;
    QHash<QString, QSharedPointer<LoanAssumption> > m_AvailableAssumptions;
    QString m_FolderToScan;
};
#endif // LoanAssMatcher_p_h__