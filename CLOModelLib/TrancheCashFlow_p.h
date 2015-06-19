#include "TrancheCashFlow.h"
#include "GenericCashFlow_p.h"
#include <QMap>
#include <QSet>
class TrancheCashFlowPrivate : public GenericCashFlowPrivate
{
    DECLARE_PRIVATE_COMMONS(TrancheCashFlow)
    DECLARE_PRIVATE_COMMONS_DATASTREAM(TrancheCashFlow)
public:
    double OutstandingAmt;
    QHash<qint32, double> StartingDeferredInterest;
};