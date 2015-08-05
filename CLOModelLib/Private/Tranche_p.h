#include "Tranche.h"
#include "Seniority.h"
#include "IntegerVector.h"
#include "BaseRateTable.h"
#include "Private/BackwardCompatibilityInterface_p.h"
#include "Ratings.h"
#include <QSet>
class TranchePrivate : public BackwardInterfacePrivate
{
    DECLARE_PRIVATE_COMMONS(Tranche)
    DECLARE_PRIVATE_COMMONS_COPY(Tranche)
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
    QSet<QString> ISINcode;
    QString TrancheName;
    Seniority ProrataGroup;
    TrancheCashFlow CashFlow;
    double ExchangeRate;
    double MinIClevel;
    double MinOClevel;
    double OriginalAmt;
    double OutstandingAmt;
    double Price;
    Ratings m_rating;
    mutable ConstantBaseRateTable m_CnstRateCache;
    mutable ForwardBaseRateTable m_FrwRateCache;
    mutable QHash<qint32, BloombergVector*> ReferenceRateValue;
    mutable bool m_UseForwardCurve;
    QString downloadISIN() const;
    template <class T> double getTotalActualCoupon(const T& index) const
    {
        Q_Q(const Tranche);
        static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "getTotalActualCoupon can be used only with int or QDate");
        double result = 0.0;
        for (qint32 i = 0; i < (1 << MaximumInterestsTypes); ++i)
            result += q->getActualCoupon(index, i);
        return result;
    }
};

