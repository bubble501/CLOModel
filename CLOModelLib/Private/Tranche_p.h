#include "Tranche.h"
#include "Seniority.h"
#include "IntegerVector.h"
#include "BaseRateTable.h"
#include "Private/BackwardCompatibilityInterface_p.h"
class TranchePrivate : public BackwardInterfacePrivate
{
    DECLARE_PRIVATE_COMMONS(Tranche)
    DECLARE_PRIVATE_COMMONS_DATASTREAM(Tranche)
public:
    IntegerVector PaymentFrequency;
    QDate LastPaymentDate;
    QDate SettlementDate;
    QHash<qint32, BaseRateVector*> ReferenceRate;
    QHash<qint32, BloombergVector*> Coupon;
    QHash<qint32, DayCountVector*> m_DayCount;
    QString BloombergExtension;
    QString Currency;
    QString ISINcode;
    QString TrancheName;
    Seniority ProrataGroup;
    TrancheCashFlow CashFlow;
    double ExchangeRate;
    double MinIClevel;
    double MinOClevel;
    double OriginalAmt;
    double OutstandingAmt;
    double Price;
    mutable ConstantBaseRateTable m_CnstRateCache;
    mutable ForwardBaseRateTable m_FrwRateCache;
    mutable QHash<qint32, BloombergVector*> ReferenceRateValue;
    mutable bool m_UseForwardCurve;
};