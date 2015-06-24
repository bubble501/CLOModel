
#include "Waterfall.h"
#include "Private/BackwardCompatibilityInterface_p.h"
#include <QList>
#include "Tranche.h"
#include "WatFalPrior.h"
#include "MtgCashFlow.h"
#include "ReinvestmentTest.h"
#include "BloombergVector.h"

#include "IntegerVector.h"
#include "ReserveFund.h"
#include "DayCountVect.h"
#include <QSharedPointer>
#include "AbstractTrigger.h"
#include "PrincipalProceeds.h"
#include "TriggersResults.h"
class WaterfallPrivate : public BackwardInterfacePrivate
{
    DECLARE_PRIVATE_COMMONS(Waterfall)
    DECLARE_PRIVATE_COMMONS_DATASTREAM(Waterfall)
public:
    QList<ReserveFund*> m_Reserves;
    bool m_CumulativeReserves;
    QDate m_LegalFinal;
    BloombergVector m_SeniorExpenses;
    BloombergVector m_SeniorFees;
    BloombergVector m_JuniorFees;
    BloombergVector m_SeniorExpensesFixed;
    BloombergVector m_SeniorFeesFixed;
    BloombergVector m_JuniorFeesFixed;
    QList<Tranche*> m_Tranches;
    QList<WatFalPrior*>	m_WaterfallStesps;
    MtgCashFlow m_MortgagesPayments;
    MtgCashFlow m_CalculatedMtgPayments;
    IntegerVector m_PaymentFrequency;
    GenericCashFlow m_ExcessCashFlow;
    GenericCashFlow m_TotalSeniorExpenses;
    GenericCashFlow m_TotalSeniorFees;
    GenericCashFlow m_TotalJuniorFees;
    ReinvestmentTest m_ReinvestmentTest;
    double m_CCCTestLimit;
    BloombergVector m_CCCcurve;
    double m_CCChaircut;
    PrincipalRecip m_PrincipalAvailable;
    double m_InterestAvailable;
    double m_JuniorFeesCoupon;
    GenericCashFlow m_AnnualizedExcess;
    GenericCashFlow m_EquityIncome;
    QDate m_FirstIPDdate;
    QDate m_LastIPDdate;
    QDate m_CallDate;
    BloombergVector m_PoolValueAtCall;
    bool m_UseCall;
    bool m_IsStressTest;
    double m_CallMultiple;
    double m_CallReserve;
    QString m_DealName;
    BloombergVector m_GICinterest;
    BaseRateVector m_GICBaseRate;
    BloombergVector m_GICBaseRateValue;
    double m_StartingDeferredJunFees;
    GenericCashFlow m_GICflows;
    DayCountVector m_DealDayCountConvention;
    QDate m_CalledPeriod;
    TriggersResults m_TriggersResults;
    QHash <quint32, QSharedPointer<AbstractTrigger> > m_Triggers;
    template<class T> double GroupOutstanding(int GroupTarget, int SeliorityScaleLevel, const T& PeriodInedex)const
    {
        static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "GroupOutstanding can be used only with int or QDate");
        double Result = 0.0;
        for (QList<Tranche*>::const_iterator i = m_Tranches.constBegin(); i != m_Tranches.constEnd(); i++) {
            if ((*i)->GetProrataGroup(SeliorityScaleLevel) == GroupTarget) Result += (*i)->GetCashFlow().GetAmountOutstanding(PeriodInedex);
        }
        return Result;
    }
};