#include <algorithm>
#include <QQueue>
#include <QHash>
#include <QSet>
#include <qmath.h>
#include "Waterfall.h"
#include "Private/Waterfall_p.h"
#include "DateTrigger.h"
#include "VectorTrigger.h"
#include "PoolSizeTrigger.h"
#include "DeferredInterestTrigger.h"
#include "TrancheTrigger.h"
#include "CumulativeLossTrigger.h"
#include <QStack>
#include "AssumptionSet.h"
#include "DelinquencyTrigger.h"
#include "DuringStressTestTrigger.h"
#include "PDLtrigger.h"
#include "Private/InternalItems.h"
#include "Mortgage.h"
#include "Seniority.h"
#include "BaseRateTable.h"
#include <QRegularExpression>
#ifndef NO_BLOOMBERG
#include <QBbgManager.h>
#include <QBbgRequestGroup.h>
#include <QbbgReferenceDataRequest.h>
#include <QBbgReferenceDataResponse.h>
#endif // !NO_BLOOMBERG

DEFINE_PUBLIC_COMMONS(Waterfall)
DEFINE_PUBLIC_COMMONS_COPY(Waterfall)
WaterfallPrivate::WaterfallPrivate(Waterfall *q,const WaterfallPrivate& other)
	:BackwardInterfacePrivate(q,other)
    , m_SeniorExpenses(other.m_SeniorExpenses)
    , m_SeniorFees(other.m_SeniorFees)
    , m_JuniorFees(other.m_JuniorFees)
    , m_MortgagesPayments(other.m_MortgagesPayments)
    , m_PaymentFrequency(other.m_PaymentFrequency)
    , m_ExcessCashFlow(other.m_ExcessCashFlow)
    , m_TotalSeniorExpenses(other.m_TotalSeniorExpenses)
    , m_TotalSeniorFees(other.m_TotalSeniorFees)
    , m_TotalJuniorFees(other.m_TotalJuniorFees)
    , m_ReinvestmentTest(other.m_ReinvestmentTest)
    , m_CCCTestLimit(other.m_CCCTestLimit)
    , m_CCCcurve(other.m_CCCcurve)
    , m_CCChaircut(other.m_CCChaircut)
    , m_PrincipalAvailable(other.m_PrincipalAvailable)
    , m_InterestAvailable(other.m_InterestAvailable)
    , m_JuniorFeesCoupon(other.m_JuniorFeesCoupon)
    , m_AnnualizedExcess(other.m_AnnualizedExcess)
    , m_FirstIPDdate(other.m_FirstIPDdate)
    , m_LastIPDdate(other.m_LastIPDdate)
    , m_CallDate(other.m_CallDate)
    , m_PoolValueAtCall(other.m_PoolValueAtCall)
    , m_UseCall(other.m_UseCall)
    , m_CallMultiple(other.m_CallMultiple)
    , m_CallReserve(other.m_CallReserve)
    , m_CalculatedMtgPayments(other.m_CalculatedMtgPayments)
    , m_EquityIncome(other.m_EquityIncome)
    , m_CumulativeReserves(other.m_CumulativeReserves)
    , m_DealName(other.m_DealName)
    , m_SeniorExpensesFixed(other.m_SeniorExpensesFixed)
    , m_SeniorFeesFixed(other.m_SeniorFeesFixed)
    , m_JuniorFeesFixed(other.m_JuniorFeesFixed)
    , m_GICinterest(other.m_GICinterest)
    , m_StartingDeferredJunFees(other.m_StartingDeferredJunFees)
    , m_GICflows(other.m_GICflows)
    , m_GICBaseRate(other.m_GICBaseRate)
    , m_GICBaseRateValue(other.m_GICBaseRateValue)
    , m_DealDayCountConvention(other.m_DealDayCountConvention)
    , m_CalledPeriod(other.m_CalledPeriod)
    , m_TriggersResults(other.m_TriggersResults)
    , m_IsStressTest(other.m_IsStressTest)
    , m_LegalFinal(other.m_LegalFinal)
{
    for (auto i = other.m_Tranches.constBegin(); i != other.m_Tranches.constEnd(); i++) {
        m_Tranches.append(new Tranche(**i));
    }
    for (auto i = other.m_WaterfallStesps.constBegin(); i != other.m_WaterfallStesps.constEnd(); i++) {
        m_WaterfallStesps.append(new WatFalPrior(**i));
    }
    for (auto i = other.m_Reserves.constBegin(); i != other.m_Reserves.constEnd(); i++) {
        m_Reserves.append(new ReserveFund(**i));
    }
    for (auto i = other.m_Triggers.constBegin(); i != other.m_Triggers.constEnd(); ++i) {
        switch (i.value()->GetTriggerType()) {
        case AbstractTrigger::TriggerType::DateTrigger:
            m_Triggers.insert(i.key(), QSharedPointer<AbstractTrigger>(new DateTrigger(*(i.value().dynamicCast<DateTrigger>()))));
            break;
        case AbstractTrigger::TriggerType::VectorTrigger:
            m_Triggers.insert(i.key(), QSharedPointer<AbstractTrigger>(new VectorTrigger(*(i.value().dynamicCast<VectorTrigger>()))));
            break;
        case AbstractTrigger::TriggerType::PoolSizeTrigger:
            m_Triggers.insert(i.key(), QSharedPointer<AbstractTrigger>(new PoolSizeTrigger(*(i.value().dynamicCast<PoolSizeTrigger>()))));
            break;
        case AbstractTrigger::TriggerType::TrancheTrigger:
            m_Triggers.insert(i.key(), QSharedPointer<AbstractTrigger>(new TrancheTrigger(*(i.value().dynamicCast<TrancheTrigger>()))));
            break;
        case AbstractTrigger::TriggerType::DelinquencyTrigger:
            m_Triggers.insert(i.key(), QSharedPointer<AbstractTrigger>(new DelinquencyTrigger(*(i.value().dynamicCast<DelinquencyTrigger>()))));
            break;
        case AbstractTrigger::TriggerType::DuringStressTestTrigger:
            m_Triggers.insert(i.key(), QSharedPointer<AbstractTrigger>(new DuringStressTestTrigger(*(i.value().dynamicCast<DuringStressTestTrigger>()))));
            break;
        case AbstractTrigger::TriggerType::CumulativeLossTrigger:
            m_Triggers.insert(i.key(), QSharedPointer<AbstractTrigger>(new CumulativeLossTrigger(*(i.value().dynamicCast<CumulativeLossTrigger>()))));
            break;
        case AbstractTrigger::TriggerType::DeferredInterestTrigger:
            m_Triggers.insert(i.key(), QSharedPointer<AbstractTrigger>(new DeferredInterestTrigger(*(i.value().dynamicCast<DeferredInterestTrigger>()))));
            break;
        case AbstractTrigger::TriggerType::PDLTrigger:
            m_Triggers.insert(i.key(), QSharedPointer<AbstractTrigger>(new PDLTrigger(*(i.value().dynamicCast<PDLTrigger>()))));
            break;
        default:
            Q_UNREACHABLE(); //Unhandled Trigger Type
            break;
        }
    }
}
WaterfallPrivate::WaterfallPrivate(Waterfall *q)
	:BackwardInterfacePrivate(q)
    , m_SeniorExpenses("0")
    , m_SeniorFees("0")
    , m_JuniorFees("0")
    , m_SeniorExpensesFixed("0")
    , m_SeniorFeesFixed("0")
    , m_JuniorFeesFixed("0")
    , m_PaymentFrequency("1")
    , m_CCCTestLimit(1.0)
    , m_CCCcurve("0")
    , m_CCChaircut(0.0)
    , m_InterestAvailable(0.0)
    , m_JuniorFeesCoupon(0.0)
    , m_PoolValueAtCall("100")
    , m_UseCall(false)
    , m_CallMultiple(0.0)
    , m_CallReserve(0.0)
    , m_CumulativeReserves(false)
    , m_StartingDeferredJunFees(0.0)
    , m_GICBaseRate("ZERO")
    , m_DealDayCountConvention(DayCountConvention::ACT360)
    , m_IsStressTest(false)
{
    m_GICflows.Aggregate(GenericCashFlow::Monthly);
    m_ExcessCashFlow.Aggregate(GenericCashFlow::Monthly);
    m_TotalSeniorExpenses.Aggregate(GenericCashFlow::Monthly);
    m_TotalSeniorFees.Aggregate(GenericCashFlow::Monthly);
    m_TotalJuniorFees.Aggregate(GenericCashFlow::Monthly);
    m_AnnualizedExcess.Aggregate(GenericCashFlow::Monthly);
    m_EquityIncome.Aggregate(GenericCashFlow::Monthly);

    m_SeniorExpensesFixed.SetDivisor(1.0);
    m_SeniorFeesFixed.SetDivisor(1.0);
    m_JuniorFeesFixed.SetDivisor(1.0);

    m_TotalJuniorFees.SetStock(TrancheCashFlow::TrancheFlowType::DeferredFlow);
}
Waterfall::Waterfall(WaterfallPrivate *d, const Waterfall& other)
	:BackwardInterface(d,other)
{}
Waterfall& Waterfall::operator=(const Waterfall& other){
	Q_D(Waterfall);
	BackwardInterface::operator=(other);
    d->m_SeniorExpenses = other.d_func()->m_SeniorExpenses;
    d->m_SeniorFees = other.d_func()->m_SeniorFees;
    d->m_JuniorFees = other.d_func()->m_JuniorFees;
    d->m_MortgagesPayments = other.d_func()->m_MortgagesPayments;
    d->m_PaymentFrequency = other.d_func()->m_PaymentFrequency;
    d->m_ExcessCashFlow = other.d_func()->m_ExcessCashFlow;
    d->m_TotalSeniorExpenses = other.d_func()->m_TotalSeniorExpenses;
    d->m_TotalSeniorFees = other.d_func()->m_TotalSeniorFees;
    d->m_TotalJuniorFees = other.d_func()->m_TotalJuniorFees;
    d->m_ReinvestmentTest = other.d_func()->m_ReinvestmentTest;
    d->m_CCCTestLimit = other.d_func()->m_CCCTestLimit;
    d->m_CCCcurve = other.d_func()->m_CCCcurve;
    d->m_CCChaircut = other.d_func()->m_CCChaircut;
    d->m_PrincipalAvailable = other.d_func()->m_PrincipalAvailable;
    d->m_InterestAvailable = other.d_func()->m_InterestAvailable;
    d->m_JuniorFeesCoupon = other.d_func()->m_JuniorFeesCoupon;
    d->m_AnnualizedExcess = other.d_func()->m_AnnualizedExcess;
    d->m_FirstIPDdate = other.d_func()->m_FirstIPDdate;
    d->m_LastIPDdate = other.d_func()->m_LastIPDdate;
    d->m_CallDate = other.d_func()->m_CallDate;
    d->m_PoolValueAtCall = other.d_func()->m_PoolValueAtCall;
    d->m_UseCall = other.d_func()->m_UseCall;
    d->m_CallMultiple = other.d_func()->m_CallMultiple;
    d->m_CallReserve = other.d_func()->m_CallReserve;
    d->m_CalculatedMtgPayments = other.d_func()->m_CalculatedMtgPayments;
    d->m_EquityIncome = other.d_func()->m_EquityIncome;
    d->m_CumulativeReserves = other.d_func()->m_CumulativeReserves;
    d->m_DealName = other.d_func()->m_DealName;
    d->m_SeniorExpensesFixed = other.d_func()->m_SeniorExpensesFixed;
    d->m_SeniorFeesFixed = other.d_func()->m_SeniorFeesFixed;
    d->m_JuniorFeesFixed = other.d_func()->m_JuniorFeesFixed;
    d->m_GICinterest = other.d_func()->m_GICinterest;
    d->m_StartingDeferredJunFees = other.d_func()->m_StartingDeferredJunFees;
    d->m_GICflows = other.d_func()->m_GICflows;
    d->m_GICBaseRate = other.d_func()->m_GICBaseRate;
    d->m_GICBaseRateValue = other.d_func()->m_GICBaseRateValue;
    d->m_DealDayCountConvention = other.d_func()->m_DealDayCountConvention;
    d->m_CalledPeriod = other.d_func()->m_CalledPeriod;
    d->m_TriggersResults = other.d_func()->m_TriggersResults;
    d->m_IsStressTest = other.d_func()->m_IsStressTest;
    d->m_LegalFinal = other.d_func()->m_LegalFinal;
    ResetTranches();
    for (auto i = other.d_func()->m_Tranches.constBegin(); i != other.d_func()->m_Tranches.constEnd(); i++) {
        d->m_Tranches.append(new Tranche(**i));
    }
    ResetSteps();
    for (auto i = other.d_func()->m_WaterfallStesps.constBegin(); i != other.d_func()->m_WaterfallStesps.constEnd(); i++) {
        d->m_WaterfallStesps.append(new WatFalPrior(**i));
    }
    RemoveReserve();
    for (auto i = other.d_func()->m_Reserves.constBegin(); i != other.d_func()->m_Reserves.constEnd(); i++) {
        d->m_Reserves.append(new ReserveFund(**i));
    }
    d->m_Triggers.clear();
    for (auto i = other.d_func()->m_Triggers.constBegin(); i != other.d_func()->m_Triggers.constEnd(); ++i) {
        switch (i.value()->GetTriggerType()) {
        case AbstractTrigger::TriggerType::DateTrigger:
            d->m_Triggers.insert(i.key(), QSharedPointer<AbstractTrigger>(new DateTrigger(*(i.value().dynamicCast<DateTrigger>()))));
            break;
        case AbstractTrigger::TriggerType::VectorTrigger:
            d->m_Triggers.insert(i.key(), QSharedPointer<AbstractTrigger>(new VectorTrigger(*(i.value().dynamicCast<VectorTrigger>()))));
            break;
        case AbstractTrigger::TriggerType::PoolSizeTrigger:
            d->m_Triggers.insert(i.key(), QSharedPointer<AbstractTrigger>(new PoolSizeTrigger(*(i.value().dynamicCast<PoolSizeTrigger>()))));
            break;
        case AbstractTrigger::TriggerType::TrancheTrigger:
            d->m_Triggers.insert(i.key(), QSharedPointer<AbstractTrigger>(new TrancheTrigger(*(i.value().dynamicCast<TrancheTrigger>()))));
            break;
        case AbstractTrigger::TriggerType::DelinquencyTrigger:
            d->m_Triggers.insert(i.key(), QSharedPointer<AbstractTrigger>(new DelinquencyTrigger(*(i.value().dynamicCast<DelinquencyTrigger>()))));
            break;
        case AbstractTrigger::TriggerType::DuringStressTestTrigger:
            d->m_Triggers.insert(i.key(), QSharedPointer<AbstractTrigger>(new DuringStressTestTrigger(*(i.value().dynamicCast<DuringStressTestTrigger>()))));
            break;
        case AbstractTrigger::TriggerType::CumulativeLossTrigger:
            d->m_Triggers.insert(i.key(), QSharedPointer<AbstractTrigger>(new CumulativeLossTrigger(*(i.value().dynamicCast<CumulativeLossTrigger>()))));
            break;
        case AbstractTrigger::TriggerType::DeferredInterestTrigger:
            d->m_Triggers.insert(i.key(), QSharedPointer<AbstractTrigger>(new DeferredInterestTrigger(*(i.value().dynamicCast<DeferredInterestTrigger>()))));
            break;
        case AbstractTrigger::TriggerType::PDLTrigger:
            d->m_Triggers.insert(i.key(), QSharedPointer<AbstractTrigger>(new PDLTrigger(*(i.value().dynamicCast<PDLTrigger>()))));
            break;
        default:
            Q_UNREACHABLE(); //Unhandled Trigger Type
            break;
        }
    }
	return *this;
}
Waterfall::Waterfall(WaterfallPrivate *d)
	:BackwardInterface(d)
{}


const WatFalPrior* Waterfall::GetStep(int Index)const
{
    Q_D(const Waterfall);
    if (Index<0 || Index >= d->m_WaterfallStesps.size()) 
        return nullptr;
    return d->m_WaterfallStesps.at(Index);
}
WatFalPrior* Waterfall::GetStep(int Index)
{
    Q_D( Waterfall);
    if (Index<0 || Index >= d->m_WaterfallStesps.size()) return nullptr;
    return d->m_WaterfallStesps[Index];
}
void Waterfall::SetCCCcurve(const QString& a)
{
    Q_D( Waterfall);
    d->m_CCCcurve = a; 
}

void Waterfall::SetFirstIPDdate(const QDate& a)
{
    Q_D( Waterfall);
    d->m_FirstIPDdate = a;
}

void Waterfall::SetLastIPDdate(const QDate& a)
{
    Q_D( Waterfall);
    d->m_LastIPDdate = a;
}

void Waterfall::SetCallDate(const QDate& a)
{
    Q_D( Waterfall);
    d->m_CallDate = a;
}

const Tranche* Waterfall::GetTranche(int Index) const
{
    Q_D(const Waterfall);
    if (Index<0 || Index >= d->m_Tranches.size()) return nullptr;
    return d->m_Tranches.at(Index);
}
Tranche* Waterfall::GetTranche(int Index)
{
    Q_D( Waterfall);
    if (Index<0 || Index >= d->m_Tranches.size()) return nullptr;
    return d->m_Tranches[Index];
}
const Tranche* Waterfall::GetTranche(const QString& TrancheName) const
{
    return GetTranche(FindTrancheIndex(TrancheName));
}
Tranche* Waterfall::GetTranche(const QString& TrancheName)
{
    return GetTranche(FindTrancheIndex(TrancheName));
}
void Waterfall::SortByProRataGroup()
{
    Q_D( Waterfall);
    std::sort(d->m_Tranches.begin(), d->m_Tranches.end(), [](const Tranche* a, const Tranche* b) -> bool { return a->GetProrataGroup(0) < b->GetProrataGroup(0); });
}
QDate Waterfall::GetStructureMaturity()const
{
    Q_D(const Waterfall);
    if (d->m_Tranches.isEmpty()) 
        return QDate();
    if (d->m_Tranches.first()->GetCashFlow().Count() == 0) 
        return QDate();
    return d->m_Tranches.first()->GetCashFlow().GetDate(d->m_Tranches.first()->GetCashFlow().Count() - 1);
}

const GenericCashFlow& Waterfall::GetRawAnnualizedExcess() const
{
    Q_D(const Waterfall);
    return d->m_AnnualizedExcess;
}

GenericCashFlow& Waterfall::GetRawAnnualizedExcess()
{
    Q_D( Waterfall);
    return d->m_AnnualizedExcess;
}

double Waterfall::GetAnnualizedExcess(int index, bool AsShareOfLoans)const
{
    Q_D(const Waterfall);
    if (index<0 || index >= d->m_AnnualizedExcess.Count()) 
        return 0.0;
    if (AsShareOfLoans) {
        double loanSize = d->m_CalculatedMtgPayments.GetAmountOut(d->m_AnnualizedExcess.GetDate(index));
        loanSize += index == 0 ? d->m_CalculatedMtgPayments.GetAmountOut(0) : d->m_CalculatedMtgPayments.GetAmountOut(d->m_AnnualizedExcess.GetDate(index - 1));
        loanSize *= 0.5;
        if (d->m_CalculatedMtgPayments.GetAmountOut(d->m_AnnualizedExcess.GetDate(index))<0.01) 
            return -1.0;
        return 1.0 / deannualiseCoupon(loanSize / d->m_AnnualizedExcess.GetTotalFlow(index), index == 0 ? d->m_LastIPDdate : d->m_AnnualizedExcess.GetDate(index - 1), d->m_AnnualizedExcess.GetDate(index), DayCountConvention::N30360);
    }
    double RunningSum = 0.0;
    double RunningPrev = 0.0;
    for (auto i = d->m_Tranches.constBegin(); i != d->m_Tranches.constEnd(); ++i) {
        RunningSum += (*i)->GetCashFlow().GetAmountOutstanding(index);
        RunningPrev += index == 0 ? (*i)->GetOutstandingAmt(): (*i)->GetCashFlow().GetAmountOutstanding(index - 1);
    }
    if (RunningSum<0.01)
        return -1.0;
    return 1.0 / deannualiseCoupon(RunningSum / d->m_AnnualizedExcess.GetTotalFlow(index), index == 0 ? d->m_LastIPDdate : d->m_AnnualizedExcess.GetDate(index - 1), d->m_AnnualizedExcess.GetDate(index), DayCountConvention::N30360);

}

Waterfall::~Waterfall()
{
    ResetSteps();
    ResetTranches();
    RemoveReserve();
    ResetTriggers();
}

const QDate& Waterfall::GetLegalFinal() const
{
    Q_D(const Waterfall);
    return d->m_LegalFinal;
}

void Waterfall::SetLegalFinal(const QDate& val)
{
    Q_D( Waterfall);
    d->m_LegalFinal = val;
}

const QHash<quint32, QSharedPointer<AbstractTrigger> >& Waterfall::GetTriggers() const
{
    Q_D(const Waterfall);
    return d->m_Triggers;
}

QString Waterfall::GetGICinterest() const
{
    Q_D(const Waterfall);
    return d->m_GICinterest.GetVector();
}

QString Waterfall::GetSeniorExpenses() const
{
    Q_D(const Waterfall);
    return d->m_SeniorExpenses.GetVector();
}

QString Waterfall::GetSeniorFees() const
{
    Q_D(const Waterfall);
    return d->m_SeniorFees.GetVector();
}

QString Waterfall::GetJuniorFees() const
{
    Q_D(const Waterfall);
    return d->m_JuniorFees.GetVector();
}

QString Waterfall::GetSeniorExpensesFixed() const
{
    Q_D(const Waterfall);
    return d->m_SeniorExpensesFixed.GetVector();
}

QString Waterfall::GetSeniorFeesFixed() const
{
    Q_D(const Waterfall);
    return d->m_SeniorFeesFixed.GetVector();
}

QString Waterfall::GetJuniorFeesFixed() const
{
    Q_D(const Waterfall);
    return d->m_JuniorFeesFixed.GetVector();
}

QString Waterfall::GetPaymentFrequency() const
{
    Q_D(const Waterfall);
    return d->m_PaymentFrequency.GetVector();
}

double Waterfall::GetCCCTestLimit() const
{
    Q_D(const Waterfall);
    return d->m_CCCTestLimit;
}

double Waterfall::GetCCChaircut() const
{
    Q_D(const Waterfall);
    return d->m_CCChaircut;
}

double Waterfall::GetPrincipalAvailable() const
{
    Q_D(const Waterfall);
    return d->m_PrincipalAvailable.Total();
}

double Waterfall::GetScheduledPrincipalAvailable() const
{
    Q_D(const Waterfall);
    return d->m_PrincipalAvailable.GetScheduled();
}

double Waterfall::GetPrepayPrincipalAvailable() const
{
    Q_D(const Waterfall);
    return d->m_PrincipalAvailable.GetPrepay();
}

double Waterfall::GetInterestAvailable() const
{
    Q_D(const Waterfall);
    return d->m_InterestAvailable;
}

double Waterfall::GetJuniorFeesCoupon() const
{
    Q_D(const Waterfall);
    return d->m_JuniorFeesCoupon;
}

QString Waterfall::GetPoolValueAtCall() const
{
    Q_D(const Waterfall);
    return d->m_PoolValueAtCall.GetVector();
}

bool Waterfall::GetUseCall() const
{
    Q_D(const Waterfall);
    return d->m_UseCall;
}

double Waterfall::GetCallMultiple() const
{
    Q_D(const Waterfall);
    return d->m_CallMultiple;
}

double Waterfall::GetCallReserve() const
{
    Q_D(const Waterfall);
    return d->m_CallReserve;
}

const MtgCashFlow& Waterfall::GetMortgagesPayments() const
{
    Q_D(const Waterfall);
    return d->m_MortgagesPayments;
}

MtgCashFlow& Waterfall::GetMortgagesPayments()
{
    Q_D( Waterfall);
    return d->m_MortgagesPayments;
}

const GenericCashFlow& Waterfall::GetExcessCashFlow() const
{
    Q_D(const Waterfall);
    return d->m_ExcessCashFlow;
}

GenericCashFlow& Waterfall::GetExcessCashFlow()
{
    Q_D( Waterfall);
    return d->m_ExcessCashFlow;
}

const GenericCashFlow& Waterfall::GetTotalSeniorExpenses() const
{
    Q_D(const Waterfall);
    return d->m_TotalSeniorExpenses;
}

GenericCashFlow& Waterfall::GetTotalSeniorExpenses()
{
    Q_D( Waterfall);
    return d->m_TotalSeniorExpenses;
}

const GenericCashFlow& Waterfall::GetTotalSeniorFees() const
{
    Q_D(const Waterfall);
    return d->m_TotalSeniorFees;
}

GenericCashFlow& Waterfall::GetTotalSeniorFees()
{
    Q_D( Waterfall);
    return d->m_TotalSeniorFees;
}

const GenericCashFlow& Waterfall::GetTotalJuniorFees() const
{
    Q_D(const Waterfall);
    return d->m_TotalJuniorFees;
}

GenericCashFlow& Waterfall::GetTotalJuniorFees()
{
    Q_D( Waterfall);
    return d->m_TotalJuniorFees;
}

const ReinvestmentTest& Waterfall::GetReinvestmentTest() const
{
    Q_D(const Waterfall);
    return d->m_ReinvestmentTest;
}

ReinvestmentTest& Waterfall::GetReinvestmentTest()
{
    Q_D( Waterfall);
    return d->m_ReinvestmentTest;
}

QString Waterfall::GetCCCcurve() const
{
    Q_D(const Waterfall);
    return d->m_CCCcurve.GetVector();
}

const GenericCashFlow& Waterfall::GetGICflows() const
{
    Q_D(const Waterfall);
    return d->m_GICflows;
}

void Waterfall::ResetSteps()
{
    Q_D( Waterfall);
    for (auto i = d->m_WaterfallStesps.begin(); i != d->m_WaterfallStesps.end(); i++)
        delete (*i);
    d->m_WaterfallStesps.clear();
}

void Waterfall::AddTranche(const Tranche& a)
{
    Q_D( Waterfall);
    d->m_Tranches.append(new Tranche(a));
}

void Waterfall::ResetTranches()
{
    Q_D( Waterfall);
    for (auto i = d->m_Tranches.begin(); i != d->m_Tranches.end(); i++)
        delete (*i);
    d->m_Tranches.clear();
}
int Waterfall::FindMostJuniorLevel(int SeliorityScaleLevel)const
{
    Q_D(const Waterfall);
    if (d->m_Tranches.isEmpty())
        return 0;
    int Result = d->m_Tranches.last()->GetProrataGroup(SeliorityScaleLevel);
    for (int i = d->m_Tranches.size() - 2; i >= 0; i--) {
        if (d->m_Tranches.at(i)->GetProrataGroup(SeliorityScaleLevel)>Result)
            Result = d->m_Tranches.at(i)->GetProrataGroup(SeliorityScaleLevel);
    }
    return Result;
}
void Waterfall::FillAllDates()
{
    Q_D( Waterfall);
    bool AnchorDateNull = d->m_PaymentFrequency.GetAnchorDate().isNull();
    if (AnchorDateNull) 
        d->m_PaymentFrequency.SetAnchorDate(d->m_MortgagesPayments.GetDate(0));
    bool ExcessIsEmpty = d->m_ExcessCashFlow.Count() == 0;
    //All the dates in all the tranches
    for (int i = 0; i<d->m_Tranches.size() - 1; i++) {
        for (int j = i + 1; j<d->m_Tranches.size(); j++) {
            for (int h = 0; h<d->m_Tranches.at(i)->GetCashFlow().Count(); h++) {
                if (d->m_Tranches.at(j)->GetCashFlow().FindDate(d->m_Tranches.at(i)->GetCashFlow().GetDate(h)) < 0) {
                    d->m_Tranches[j]->AddCashFlow(d->m_Tranches.at(i)->GetCashFlow().GetDate(h), 0.0, TrancheCashFlow::TrancheFlowType::InterestFlow);
                }
            }
        }
    }
    //dates from excess to all the tranches
    if (!ExcessIsEmpty) {
        for (int j = 0; j<d->m_Tranches.size(); j++) {
            for (int h = 0; h<d->m_ExcessCashFlow.Count(); h++) {
                if (d->m_Tranches.at(j)->GetCashFlow().FindDate(d->m_ExcessCashFlow.GetDate(h)) < 0) {
                    d->m_Tranches[j]->AddCashFlow(d->m_ExcessCashFlow.GetDate(h), 0.0, TrancheCashFlow::TrancheFlowType::InterestFlow);
                }
            }
        }
    }
    //dates from annualized excess to all the tranches (equity income dates included here)
    for (int j = 0; j<d->m_Tranches.size(); j++) {
        for (int h = 0; h<d->m_AnnualizedExcess.Count(); h++) {
            if (d->m_Tranches.at(j)->GetCashFlow().FindDate(d->m_AnnualizedExcess.GetDate(h)) < 0) {
                d->m_Tranches[j]->AddCashFlow(d->m_AnnualizedExcess.GetDate(h), 0.0, TrancheCashFlow::TrancheFlowType::InterestFlow);
            }
        }
    }
    //dates from reserve funds to all the tranches
    for (QList<ReserveFund*>::const_iterator i = d->m_Reserves.constBegin(); i != d->m_Reserves.constEnd(); i++) {
        if ((*i)->GetReserveFundFlow().Count() == 0) continue;
        for (int j = 0; j < d->m_Tranches.size(); j++) {
            for (int h = 0; h < (*i)->GetReserveFundFlow().Count(); h++) {
                if (d->m_Tranches.at(j)->GetCashFlow().FindDate((*i)->GetReserveFundFlow().GetDate(h)) < 0) {
                    d->m_Tranches[j]->AddCashFlow((*i)->GetReserveFundFlow().GetDate(h), 0.0, TrancheCashFlow::TrancheFlowType::InterestFlow);
                }
            }
        }
    }
    //dates from senior fees to all the tranches
    for (int j = 0; j<d->m_Tranches.size(); j++) {
        for (int h = 0; h<d->m_TotalSeniorFees.Count(); h++) {
            if (d->m_Tranches.at(j)->GetCashFlow().FindDate(d->m_TotalSeniorFees.GetDate(h)) < 0) {
                d->m_Tranches[j]->AddCashFlow(d->m_TotalSeniorFees.GetDate(h), 0.0, TrancheCashFlow::TrancheFlowType::InterestFlow);
            }
        }
    }
    //dates from junior fees to all the tranches
    for (int j = 0; j<d->m_Tranches.size(); j++) {
        for (int h = 0; h<d->m_TotalJuniorFees.Count(); h++) {
            if (d->m_Tranches.at(j)->GetCashFlow().FindDate(d->m_TotalJuniorFees.GetDate(h)) < 0) {
                d->m_Tranches[j]->AddCashFlow(d->m_TotalJuniorFees.GetDate(h), 0.0, TrancheCashFlow::TrancheFlowType::InterestFlow);
            }
        }
    }
    //dates from senior expenses to all the tranches
    for (int j = 0; j<d->m_Tranches.size(); j++) {
        for (int h = 0; h<d->m_TotalSeniorExpenses.Count(); h++) {
            if (d->m_Tranches.at(j)->GetCashFlow().FindDate(d->m_TotalSeniorExpenses.GetDate(h)) < 0) {
                d->m_Tranches[j]->AddCashFlow(d->m_TotalSeniorExpenses.GetDate(h), 0.0, TrancheCashFlow::TrancheFlowType::InterestFlow);
            }
        }
    }
    //dates from tranches to excess, fees and reserve
    if (d->m_Tranches.size()>0) {
        for (int h = 0; h<d->m_Tranches.at(0)->GetCashFlow().Count(); h++) {
            if (!ExcessIsEmpty) {
                if (d->m_ExcessCashFlow.FindDate(d->m_Tranches.at(0)->GetCashFlow().GetDate(h)) < 0)
                    d->m_ExcessCashFlow.AddFlow(d->m_Tranches.at(0)->GetCashFlow().GetDate(h), 0.0, 0);
            }
            if (d->m_AnnualizedExcess.FindDate(d->m_Tranches.at(0)->GetCashFlow().GetDate(h)) < 0)
                d->m_AnnualizedExcess.AddFlow(d->m_Tranches.at(0)->GetCashFlow().GetDate(h), 0.0, 0);
            if (d->m_TotalSeniorExpenses.FindDate(d->m_Tranches.at(0)->GetCashFlow().GetDate(h)) < 0)
                d->m_TotalSeniorExpenses.AddFlow(d->m_Tranches.at(0)->GetCashFlow().GetDate(h), 0.0, 0);
            if (d->m_TotalSeniorFees.FindDate(d->m_Tranches.at(0)->GetCashFlow().GetDate(h)) < 0)
                d->m_TotalSeniorFees.AddFlow(d->m_Tranches.at(0)->GetCashFlow().GetDate(h), 0.0, 0);
            if (d->m_TotalJuniorFees.FindDate(d->m_Tranches.at(0)->GetCashFlow().GetDate(h)) < 0)
                d->m_TotalJuniorFees.AddFlow(d->m_Tranches.at(0)->GetCashFlow().GetDate(h), 0.0, 0);
            for (auto ResIter = d->m_Reserves.begin(); ResIter != d->m_Reserves.end(); ResIter++) {
                if ((*ResIter)->GetReserveFundFlow().Count() == 0) continue;
                if ((*ResIter)->GetReserveFundFlow().FindDate(d->m_Tranches.at(0)->GetCashFlow().GetDate(h)) < 0)
                    (*ResIter)->GetReserveFundFlow().AddFlow(d->m_Tranches.at(0)->GetCashFlow().GetDate(h), 0.0, ReserveFund::ReserveFlowsType::ReplenishFromInterest);
            }
        }
    }
    QDate RollingNextIPD = d->m_FirstIPDdate;

    for (int i = 0; i<d->m_CalculatedMtgPayments.Count(); i++) {
        if (d->m_CalculatedMtgPayments.GetDate(i).year() > RollingNextIPD.year() || (d->m_CalculatedMtgPayments.GetDate(i).year() == RollingNextIPD.year() && d->m_CalculatedMtgPayments.GetDate(i).month() >= RollingNextIPD.month()) || i >= d->m_CalculatedMtgPayments.Count() - 1) {
            for (int j = 0; j<d->m_Tranches.size(); j++) {
                d->m_Tranches[j]->GetCashFlow().ReplaceDate(d->m_CalculatedMtgPayments.GetDate(i), RollingNextIPD);
            }
            d->m_ExcessCashFlow.ReplaceDate(d->m_CalculatedMtgPayments.GetDate(i), RollingNextIPD);
            d->m_AnnualizedExcess.ReplaceDate(d->m_CalculatedMtgPayments.GetDate(i), RollingNextIPD);
            d->m_TotalSeniorExpenses.ReplaceDate(d->m_CalculatedMtgPayments.GetDate(i), RollingNextIPD);
            d->m_TotalSeniorFees.ReplaceDate(d->m_CalculatedMtgPayments.GetDate(i), RollingNextIPD);
            d->m_TotalJuniorFees.ReplaceDate(d->m_CalculatedMtgPayments.GetDate(i), RollingNextIPD);
            for (auto ResIter = d->m_Reserves.begin(); ResIter != d->m_Reserves.end(); ResIter++) {
                if ((*ResIter)->GetReserveFundFlow().Count() == 0) continue;
                (*ResIter)->GetReserveFundFlow().ReplaceDate(d->m_CalculatedMtgPayments.GetDate(i), RollingNextIPD);
            }
            RollingNextIPD = RollingNextIPD.addMonths(d->m_PaymentFrequency.GetValue(RollingNextIPD));
        }
    }
    if (AnchorDateNull) d->m_PaymentFrequency.RemoveAnchorDate();
}
int Waterfall::FindTrancheIndex(const QString& Tranchename)const
{
    Q_D(const Waterfall);
    for (int j = 0; j<d->m_Tranches.size(); j++) {
        if (d->m_Tranches.at(j)->GetTrancheName() == Tranchename) 
            return j;
        for (auto isinIter = d->m_Tranches.at(j)->GetISIN().constBegin(); isinIter != d->m_Tranches.at(j)->GetISIN().constEnd(); ++isinIter) {
            if (isinIter->trimmed().compare(Tranchename.trimmed(), Qt::CaseInsensitive)==0)
                return j;
        }
    }
#ifndef NO_BLOOMBERG
    QBbgLib::QBbgManager ISINparser;
    QBbgLib::QBbgRequestGroup ISINRequest;
    QBbgLib::QBbgReferenceDataRequest SingleRq;
    const QString FieldsToSearch[] = { "NAME",
        "ID_ISIN",
        "ID_CUSIP",
        "ID_BB_GLOBAL",
        "ID_BB_UNIQUE",
        "ID_BB",
        "ID_SEDOL1",
        "ID_SEDOL2",
        "ID_SEDOL3",
        "ID_SEDOL4",
        "ID_SEDOL5" };
    SingleRq.setSecurity(QBbgLib::QBbgSecurity(Tranchename, QBbgLib::QBbgSecurity::Mtge));
    for (auto i = std::begin(FieldsToSearch); i != std::end(FieldsToSearch); ++i) {
        SingleRq.setField(*i);
        ISINRequest.addRequest(SingleRq);
    }
    SingleRq.setSecurity(QBbgLib::QBbgSecurity(Tranchename, QBbgLib::QBbgSecurity::Corp));
    for (auto i = std::begin(FieldsToSearch); i != std::end(FieldsToSearch); ++i) {
        SingleRq.setField(*i);
        ISINRequest.addRequest(SingleRq);
    }
    const auto& bbgResult = ISINparser.processRequest(ISINRequest);
    const QBbgLib::QBbgReferenceDataResponse* CurrentResponse;
    for (auto RespIter = bbgResult.constBegin(); RespIter != bbgResult.constEnd(); ++RespIter) {
        if (RespIter.value()->hasErrors())
            continue;
        CurrentResponse = dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(RespIter.value());
        if (!CurrentResponse)
            continue;
        for (int i = 0; i < d->m_Tranches.size(); i++) {
            for (auto isinIter = d->m_Tranches.at(i)->GetISIN().constBegin(); isinIter != d->m_Tranches.at(i)->GetISIN().constEnd(); ++isinIter) {
                if (CurrentResponse->value().toString().compare(isinIter->trimmed(), Qt::CaseInsensitive) == 0)
                    return i;
            }
            if (CurrentResponse->value().toString().compare(d->m_Tranches.at(i)->GetTrancheName().trimmed(), Qt::CaseInsensitive) == 0)
                return i;
        }
    }
#endif 
    return -1;
}
double Waterfall::GetCreditEnhancement(int TrancheIndex, int TimeIndex)const
{
    Q_D(const Waterfall);
    if (TrancheIndex<-1 || TrancheIndex >= d->m_Tranches.size()) return 0.0;
    int TargetSeniority = d->m_Tranches.at(TrancheIndex)->GetProrataGroup(0);
    double Runningsum = 0.0;
    double ReserveSum;
    for (int i = 0; i<d->m_Tranches.size(); i++) {
        if (d->m_Tranches.at(i)->GetProrataGroup(0) <= TargetSeniority) {
            if (TimeIndex >= 0) {
                Runningsum += d->m_Tranches.at(i)->GetCashFlow().GetAmountOutstanding(TimeIndex);
            }
            else {
                Runningsum += d->m_Tranches.at(i)->GetOutstandingAmt();
            }
        }
    }
    if (Runningsum == 0.0) return 1.0;
    if (TimeIndex >= 0) {
        ReserveSum = d->m_CalculatedMtgPayments.GetAmountOut(d->m_Tranches.first()->GetCashFlow().GetDate(TimeIndex));
        for (QList<ReserveFund*>::const_iterator ResIter = d->m_Reserves.constBegin(); ResIter != d->m_Reserves.constEnd(); ResIter++) {
            if ((*ResIter)->GetReserveFundFlow().Count() > 0 && TrancheIndex <= (*ResIter)->GetReserveFundFreed())
                ReserveSum += (*ResIter)->GetReserveFundCurrent((*ResIter)->GetReserveFundFlow().GetDate(TimeIndex));
        }
        if (ReserveSum <= 0.0) return 0.0;
        return 1.0 - (Runningsum / ReserveSum); //(m_CalculatedMtgPayments.GetAmountOut(m_Tranches.first()->GetCashFlow().GetDate(TimeIndex))/Runningsum)-1.0;
    }
    else {
        ReserveSum = d->m_MortgagesPayments.GetAmountOut(0);
        ReserveSum += d->m_PrincipalAvailable.Total();
        for (QList<ReserveFund*>::const_iterator ResIter = d->m_Reserves.constBegin(); ResIter != d->m_Reserves.constEnd(); ResIter++) {
            if (TrancheIndex <= (*ResIter)->GetReserveFundFreed())
                ReserveSum += (*ResIter)->GetReserveFundCurrent();
        }
        if (ReserveSum <= 0.0) return 0.0;
        return 1.0 - (Runningsum / ReserveSum); //((m_CalculatedMtgPayments.GetAmountOut(0)+m_PrincipalAvailable)/Runningsum)-1.0;
    }
}


int Waterfall::GetTranchesCount() const
{
    Q_D(const Waterfall);
    return d->m_Tranches.size();
}

int Waterfall::GetStepsCount() const
{
    Q_D(const Waterfall);
    return d->m_WaterfallStesps.size();
}

const MtgCashFlow& Waterfall::GetCalculatedMtgPayments() const
{
    Q_D(const Waterfall);
    return d->m_CalculatedMtgPayments;
}

MtgCashFlow& Waterfall::GetCalculatedMtgPayments()
{
    Q_D( Waterfall);
    return d->m_CalculatedMtgPayments;
}

const ReserveFund* const Waterfall::GetReserveFund(int RFindex) const
{
    Q_D(const Waterfall);
    if (RFindex < 0 || RFindex >= d->m_Reserves.size()) return nullptr;
    return d->m_Reserves.at(RFindex);
}

bool Waterfall::GetCumulativeReserves() const
{
    Q_D(const Waterfall);
    return d->m_CumulativeReserves;
}

int Waterfall::GetNumReserves() const
{
    Q_D(const Waterfall);
    return d->m_Reserves.size();
}

const QString& Waterfall::GetDealName() const
{
    Q_D(const Waterfall);
    return d->m_DealName;
}

const double& Waterfall::GetStartingDeferredJunFees() const
{
    Q_D(const Waterfall);
    return d->m_StartingDeferredJunFees;
}

QString Waterfall::GetGICBaseRate() const
{
    Q_D(const Waterfall);
    return d->m_GICBaseRate.GetVector();
}

const DayCountVector& Waterfall::GetDealDayCountConvention() const
{
    Q_D(const Waterfall);
    return d->m_DealDayCountConvention;
}

double Waterfall::GroupWACoupon(int GroupTarget, int SeliorityScaleLevel, const QDate& Period, qint32 CouponType/*=0*/) const
{
    /*double RunningSum = 0.0;
    double Result = 0.0;
    for (QList<Tranche*>::const_iterator i = m_Tranches.begin(); i != m_Tranches.end(); i++) {
    if ((*i)->GetProrataGroup(SeliorityScaleLevel) == GroupTarget) {
    Result += (*i)->GetCoupon(Period, CouponType)*(*i)->GetCashFlow().GetAmountOutstanding(Period);
    RunningSum += (*i)->GetCashFlow().GetAmountOutstanding(Period);
    }
    }
    if (RunningSum > 0) return Result / RunningSum;
    else return 0.0;*/
    QList<qint32> CouponTypes;
    CouponTypes << CouponType;
    return GroupWACoupon(GroupTarget, SeliorityScaleLevel, Period, CouponTypes);
}

double Waterfall::GroupWACoupon(int GroupTarget, int SeliorityScaleLevel, const QDate& Period, QList<qint32> CouponTypes) const
{
    Q_D(const Waterfall);
    if (CouponTypes.isEmpty()) {
        for (auto i = d->m_Tranches.constBegin(); i != d->m_Tranches.constEnd(); i++) {
            if ((*i)->GetProrataGroup(SeliorityScaleLevel) == GroupTarget) {
                CouponTypes.append((*i)->GetCouponIndexes());
            }
        }
        CouponTypes = CouponTypes.toSet().toList();
    }
    double RunningSum = 0.0;
    double Result = 0.0;
    double CurrentCoup;
    for (auto i = d->m_Tranches.constBegin(); i != d->m_Tranches.constEnd(); i++) {
        if ((*i)->GetProrataGroup(SeliorityScaleLevel) == GroupTarget) {
            CurrentCoup = 0.0;
            foreach(qint32 CouponType, CouponTypes)
            {
                CurrentCoup += (*i)->GetCoupon(Period, CouponType);
            }
            Result += CurrentCoup*(*i)->GetCashFlow().GetAmountOutstanding(Period);
            RunningSum += (*i)->GetCashFlow().GetAmountOutstanding(Period);
        }
    }
    if (qAbs(RunningSum) >= 0.01) return Result / RunningSum;
    else return 0.0;
}

void Waterfall::SetupReinvBond(
    const QString& IntrVec
    , const QString& CPRVec
    , const QString& CDRVec
    , const QString& LSVec
    , const QString& WALval
    , const QString& PayFreq
    , const QString& AnnuityVec
    , const QString& ReinvPric
    , const QString& ReinvDel
    , const QString& ReinvSpr
    , const QString& FloatingBase
    , const QString& RecoveryLag
    , const QString& Delinquency
    , const QString& DelinquencyLag
    )
{
    Q_D( Waterfall);
    d->m_ReinvestmentTest.SetupReinvBond(IntrVec, CPRVec, CDRVec, LSVec, WALval, PayFreq, AnnuityVec, ReinvPric, ReinvDel, ReinvSpr, FloatingBase, RecoveryLag, Delinquency, DelinquencyLag);
}

void Waterfall::AddStep(const WatFalPrior& a)
{
    Q_D( Waterfall);
    d->m_WaterfallStesps.append(new WatFalPrior(a));
}

void Waterfall::SetReinvestementPeriod(const QDate& ReinvPer)
{
    Q_D( Waterfall);
    d->m_ReinvestmentTest.SetReinvestementPeriod(ReinvPer);
    QSharedPointer<AbstractTrigger> TempReinvTrigger(new DateTrigger(ReinvPer, DateTrigger::TriggerSide::BeforeIncluding, "Reinvestment Period"));
    SetTrigger(0, TempReinvTrigger);
}
double Waterfall::RedeemNotes(double AvailableFunds, int GroupTarget, int SeliorityScaleLevel, const QDate& TargetDate)
{
    Q_D( Waterfall);
    if (AvailableFunds<0.01) return 0.0;
    QMultiMap<quint32, QList<Tranche*>::iterator> groups;
    double TotalPayable = 0.0;
    for (auto i = d->m_Tranches.begin(); i != d->m_Tranches.end(); ++i) {
        if ((*i)->GetProrataGroup(SeliorityScaleLevel) == GroupTarget) {
            groups.insert((*i)->GetProrataGroup().GetGroup(SeliorityScaleLevel), i);
            TotalPayable += (*i)->GetCashFlow().GetAmountOutstanding(TargetDate);
        }
    }
    if (AvailableFunds >= TotalPayable) {
        for (auto i = groups.begin(); i != groups.end(); ++i) {
            (*i.value())->AddCashFlow(TargetDate, (*i.value())->GetCashFlow().GetAmountOutstanding(TargetDate), TrancheCashFlow::TrancheFlowType::PrincipalFlow);
            (*i.value())->SetCashFlow(TargetDate, 0.0, TrancheCashFlow::TrancheFlowType::AmountOutstandingFlow);
        }
        return ((AvailableFunds - TotalPayable) >= 0.01 ? (AvailableFunds - TotalPayable) : 0.0);
    }
    QMap<qint32, double> groupAccrue;
    QHash < qint32, QList<qint32> * > groupRanks;
    auto groupKeys = groups.keys();
    for (auto i = groupKeys.constBegin(); i != groupKeys.constEnd(); ++i) {
        auto currVals = groups.values(*i);
        groupAccrue.insert(*i, 0.0);
        groupRanks.insert(*i, new QList<qint32>());
        for (auto j = currVals.constBegin(); j != currVals.constEnd(); ++j) {
            groupAccrue[*i] += (**j)->GetCashFlow().GetAmountOutstanding(TargetDate);
            if (!groupRanks.value(*i)->contains((**j)->GetProrataGroup().GetRank(SeliorityScaleLevel)))
                groupRanks.value(*i)->append((**j)->GetProrataGroup().GetRank(SeliorityScaleLevel));
        }
        std::sort(groupRanks[*i]->begin(), groupRanks[*i]->end());
    }
    for (auto i = groupAccrue.begin(); i != groupAccrue.end(); ++i) {
        i.value() *= AvailableFunds / TotalPayable;
        auto currentGroup = groups.values(i.key());
        for (auto j = groupRanks[i.key()]->begin(); j != groupRanks[i.key()]->end() && i.value()>=0.01; ++j) {
            double sumRank = 0.0;
            for (QList<Tranche*>::iterator &singleTranche : currentGroup) {
                if ((*singleTranche)->GetProrataGroup().GetRank(SeliorityScaleLevel) != *j) continue;
                sumRank += (*singleTranche)->GetCashFlow().GetAmountOutstanding(TargetDate);
            }
            if (i.value() >= sumRank) {
                for (QList<Tranche*>::iterator &singleTranche : currentGroup) {
                    if ((*singleTranche)->GetProrataGroup().GetRank(SeliorityScaleLevel) != *j) continue;
                    (*singleTranche)->AddCashFlow(TargetDate, (*singleTranche)->GetCashFlow().GetAmountOutstanding(TargetDate), TrancheCashFlow::TrancheFlowType::PrincipalFlow);
                    (*singleTranche)->SetCashFlow(TargetDate, 0.0, TrancheCashFlow::TrancheFlowType::AmountOutstandingFlow);
                }
                i.value() -= sumRank;
            }
            else {
                for (QList<Tranche*>::iterator &singleTranche : currentGroup) {
                    if ((*singleTranche)->GetProrataGroup().GetRank(SeliorityScaleLevel) != *j) continue;
                    double Paymade = i.value()*(*singleTranche)->GetCashFlow().GetAmountOutstanding(TargetDate) / sumRank;
                    (*singleTranche)->AddCashFlow(TargetDate, Paymade, TrancheCashFlow::TrancheFlowType::PrincipalFlow);
                    (*singleTranche)->AddCashFlow(TargetDate, -Paymade, TrancheCashFlow::TrancheFlowType::AmountOutstandingFlow);
                }
                i.value() = 0.0;
            }
           
        }
        Q_ASSERT(i.value() > -0.01);
    }
    for (auto i = groupRanks.begin(); i != groupRanks.end(); ++i)
        delete i.value();
    return 0.0;
}
double Waterfall::RedeemSequential(double AvailableFunds, const QDate& TargetDate, int SeliorityScaleLevel, int MaxGroup)
{
    if (AvailableFunds<0.01) return 0.0;
    if (MaxGroup <= 0) MaxGroup = FindMostJuniorLevel(SeliorityScaleLevel);
    for (int CurrentSeniority = 1; CurrentSeniority <= MaxGroup && AvailableFunds >= 0.01; CurrentSeniority++) {
        AvailableFunds = RedeemNotes(AvailableFunds, CurrentSeniority, SeliorityScaleLevel, TargetDate);
    }
    return (AvailableFunds<0.01 ? 0.0 : AvailableFunds);
}
/*
double Waterfall::RedeemProRata(double AvailableFunds, const QDate& TargetDate, QList<int> Groups, int SeliorityScaleLevel) {
int MostJunior = FindMostJuniorLevel(SeliorityScaleLevel);
if(Groups.isEmpty()){
for(int i=1;i<=MostJunior;i++) Groups.append(i);
}
if(Groups.size()==1){
for(int i=1;i<=MostJunior && i<=Groups.first();i++) Groups.append(i);
}
double TotalPayable=0.0;
QHash<int,double> GroupSum;
for(int i=0;i<m_Tranches.size();i++){
if (Groups.contains(m_Tranches.at(i)->GetProrataGroup(SeliorityScaleLevel))) {
TotalPayable += m_Tranches.at(i)->GetCashFlow().GetAmountOutstanding(TargetDate);
if (GroupSum.contains(m_Tranches.at(i)->GetProrataGroup(SeliorityScaleLevel)))
GroupSum[m_Tranches.at(i)->GetProrataGroup(SeliorityScaleLevel)] += m_Tranches.at(i)->GetCashFlow().GetAmountOutstanding(TargetDate);
else
GroupSum.insert(m_Tranches.at(i)->GetProrataGroup(SeliorityScaleLevel), m_Tranches.at(i)->GetCashFlow().GetAmountOutstanding(TargetDate));
}
}
if (TotalPayable < 0.01) return AvailableFunds;
double RemainingFunds=0.0;
foreach(int CurrentSeniority, Groups){
RemainingFunds+=RedeemNotes(
AvailableFunds*GroupSum.value(CurrentSeniority)/TotalPayable
, CurrentSeniority, SeliorityScaleLevel, TargetDate);
}
if (RemainingFunds > 0.01) {
if (qAbs(RemainingFunds - AvailableFunds) < 0.01) return RemainingFunds;
return RedeemProRata(RemainingFunds, TargetDate, Groups, SeliorityScaleLevel);
}
return 0.0;
}*/
double Waterfall::GetWACostOfCapital(int index)const
{
    Q_D(const Waterfall);
    double RunningSum = 0.0;
    double Result = 0.0;
    if (d->m_Tranches.isEmpty()) 
        return 0.0;
    if (index<0 || index >= d->m_Tranches.first()->GetCashFlow().Count()) return 0.0;
    for (auto i = d->m_Tranches.constBegin(); i != d->m_Tranches.constEnd(); i++) {
        if ((*i)->GetTotalCoupon((*i)->GetCashFlow().GetDate(index))>0) {
            Result += ((*i)->GetTotalCoupon((*i)->GetCashFlow().GetDate(index))) * ((*i)->GetCashFlow().GetAmountOutstanding(index));
            RunningSum += (*i)->GetCashFlow().GetAmountOutstanding(index);
        }
    }
    if (qAbs(RunningSum)<0.01) return 0.0;
    return Result / RunningSum;
}

bool Waterfall::CalculateTranchesCashFlows()
{
    Q_D( Waterfall);
    if (!ReadyToCalculate().isEmpty()) {
        PrintToTempFile("ReturnFalse.txt", "Not Ready To Calculate");
        return false;
    }
        {//Check if all base rates are valid
            bool KeepSearching = false;
            ConstantBaseRateTable TempTable;
            if (d->m_GICBaseRateValue.IsEmpty()) {
                if (d->m_GICBaseRate.IsEmpty()) {
                    d->m_GICBaseRateValue = "0";
                }
                else {
                    d->m_GICBaseRateValue = d->m_GICBaseRate.CompileReferenceRateValue(TempTable);
                    if (d->m_GICBaseRateValue.IsEmpty()) {
                        PrintToTempFile("ReturnFalse.txt", "Missing Base Rate Value");
                        return false;
                    }
                }
            }
            for (auto SingleTranche = d->m_Tranches.constBegin(); SingleTranche != d->m_Tranches.constEnd(); ++SingleTranche) {
                auto keysList = (*SingleTranche)->GetReferenceRateIndexes();
                for (auto SingleRate = keysList.constBegin(); SingleRate != keysList.constEnd(); ++SingleRate) {
                    if (!(*SingleTranche)->GetCouponVector(*SingleRate).IsEmpty() && (*SingleTranche)->GetReferenceRateValue(*SingleRate).IsEmpty()) {
                        KeepSearching = true;
                        (*SingleTranche)->CompileReferenceRateValue(TempTable);
                        break;
                    }
                }
            }
            for (auto SingleTranche = d->m_Tranches.constBegin(); KeepSearching && SingleTranche != d->m_Tranches.constEnd(); ++SingleTranche) {
                for (auto SingleRate = (*SingleTranche)->GetRefRateValues().constBegin(); SingleRate != (*SingleTranche)->GetRefRateValues().constEnd(); ++SingleRate) {
                    if (SingleRate.value()->IsEmpty()) {
                        PrintToTempFile("ReturnFalse.txt", "Missing Base Rate Value");
                        return false;
                    }
                }
            }
        }
        { //Check steps of the waterfall
            for (auto SingleStep = d->m_WaterfallStesps.constBegin(); SingleStep != d->m_WaterfallStesps.constEnd(); ++SingleStep){
                //Check if all triggers are valid
                if ((*SingleStep)->HasParameter(WatFalPrior::wstParameters::Trigger)) {
                    if (!ValidTriggerStructure((*SingleStep)->GetParameter(WatFalPrior::wstParameters::Trigger).toString())) {
                        PrintToTempFile("ReturnFalse.txt", "Invalid Trigger Structure");
                        return false;
                    }
                }
            }
        }
        QDate ActualCallDate; //Date in Loans Flow that is the limit for the call
        d->m_CalledPeriod = QDate();
        d->m_TriggersResults.ClearResults();
        PrincipalRecip AvailablePrincipal; //Principal Available during an IPD
        double AvailableInterest; //Interest Available during an IPD
        double TotalPayable; // Temp Variable
        QQueue<int> ProRataBonds; //Queue filled with all the bonds that should be considered pro rata
        double TestTarget;// Temp Variable
        double Solution;// Temp Variable
        int SolutionDegree;// Temp Variable
        double CurrentAssetSum = 0.0; //Sum of the AUM between 2 IPDs, used when calculating fees based on avg AUM
        int CurrentAssetCount = 0; //Count of months between 2 IPDs, used when calculating fees based on avg AUM
        double AdjustedCoupon; //De-annualized interest rate 
        double InterestPayableBefore;// Temp Variable
        QDate RollingNextIPD = d->m_FirstIPDdate;
        QDate RollingLastIPD = d->m_LastIPDdate;
        double adjSeniorExpenses;//De-annualized Senior Expenses
        double adjSeniorFees;//De-annualized Senior Fees 
        double adjJuniorFees;//De-annualized Junior Fees 
        double OriginalAvailableInterest = d->m_InterestAvailable; //Stores the interest cash
        PrincipalRecip OriginalAvailablePrincipal = d->m_PrincipalAvailable; //Stores the Principal cash
        double OriginalStartingDefJunFees = d->m_StartingDeferredJunFees; //Stores the current deferred junior fees
        bool IsCallPaymentDate = false; //Stores if the current date is the call date
        double ActualCallReserveLevel = 0.0;// Temp Variable used for call calculation
        //QDate CurrentDate;
        bool IsMaturityDate = false;  //Stores if the current date is the maturity date of the notes
        MtgCashFlow OriginalMtgFlows = d->m_MortgagesPayments; //Stores the loans cash flows before reinvestments
        d->m_ExcessCashFlow.Clear();
        d->m_TotalSeniorExpenses.Clear();
        d->m_TotalSeniorFees.Clear();
        d->m_TotalJuniorFees.Clear();
        d->m_AnnualizedExcess.Clear();
        d->m_EquityIncome.Clear();
        d->m_ReinvestmentTest.ClearFlows();
        bool NullCCCanchor[] = { //Stores whether the vectors have anchors or not
            d->m_CCCcurve.GetAnchorDate().isNull()
            , d->m_PaymentFrequency.GetAnchorDate().isNull()
            , d->m_SeniorExpenses.GetAnchorDate().isNull()
            , d->m_SeniorFees.GetAnchorDate().isNull()
            , d->m_JuniorFees.GetAnchorDate().isNull()
            , d->m_SeniorExpensesFixed.GetAnchorDate().isNull()
            , d->m_SeniorFeesFixed.GetAnchorDate().isNull()
            , d->m_JuniorFeesFixed.GetAnchorDate().isNull()
            , d->m_GICinterest.GetAnchorDate().isNull()
            , d->m_GICBaseRateValue.GetAnchorDate().isNull()
        };
        if (NullCCCanchor[0]) d->m_CCCcurve.SetAnchorDate(d->m_MortgagesPayments.GetDate(0));
        if (NullCCCanchor[1]) d->m_PaymentFrequency.SetAnchorDate(d->m_MortgagesPayments.GetDate(0));
        if (NullCCCanchor[2]) d->m_SeniorExpenses.SetAnchorDate(d->m_MortgagesPayments.GetDate(0));
        if (NullCCCanchor[3]) d->m_SeniorFees.SetAnchorDate(d->m_MortgagesPayments.GetDate(0));
        if (NullCCCanchor[4]) d->m_JuniorFees.SetAnchorDate(d->m_MortgagesPayments.GetDate(0));
        if (NullCCCanchor[5]) d->m_SeniorExpensesFixed.SetAnchorDate(d->m_MortgagesPayments.GetDate(0));
        if (NullCCCanchor[6]) d->m_SeniorFeesFixed.SetAnchorDate(d->m_MortgagesPayments.GetDate(0));
        if (NullCCCanchor[7]) d->m_JuniorFeesFixed.SetAnchorDate(d->m_MortgagesPayments.GetDate(0));
        if (NullCCCanchor[8]) d->m_GICinterest.SetAnchorDate(d->m_MortgagesPayments.GetDate(0));
        if (NullCCCanchor[9]) d->m_GICBaseRateValue.SetAnchorDate(d->m_MortgagesPayments.GetDate(0));
        d->m_ReinvestmentTest.SetMissingAnchors(d->m_MortgagesPayments.GetDate(0));
        for (auto SingleTranche = d->m_Tranches.begin(); SingleTranche != d->m_Tranches.end(); ++SingleTranche){
            (*SingleTranche)->GetCashFlow().Clear();
        }
        {
            const QDate newAncor = d->m_MortgagesPayments.GetDate(0);
            for (auto SingleStp = d->m_WaterfallStesps.begin(); SingleStp != d->m_WaterfallStesps.end(); ++SingleStp) {
                (*SingleStp)->FillMissingAnchors(newAncor);
            }
        }
        for (QDate CurrentDate = d->m_MortgagesPayments.GetDate(0); !IsMaturityDate; CurrentDate = CurrentDate.addMonths(1)) {
            int CurrentPeriodIndex = MonthDiff(CurrentDate, d->m_MortgagesPayments.GetDate(0));
            if (CurrentDate>d->m_MortgagesPayments.MaturityDate() && (
                d->m_LegalFinal.isNull()
                || d->m_ReinvestmentTest.ReinvestQueueueEmpty()
                )) IsMaturityDate = true;
            if (!d->m_LegalFinal.isNull() && CurrentDate >= d->m_LegalFinal)  IsMaturityDate = true;
            if (!IsMaturityDate && d->m_MortgagesPayments.FindDate(CurrentDate) < 0) d->m_MortgagesPayments.AddFlow(CurrentDate, 0, 0);
            if (IsMaturityDate) {
                d->m_PrincipalAvailable.AddPrepay(d->m_ReinvestmentTest.GetQueuedCash(CurrentDate));
                d->m_ReinvestmentTest.ResetReinvestQueueue();
            }
            else {
                //Process the current reinvestments
                const MtgCashFlow& ReinvFlows = d->m_ReinvestmentTest.ProcessQueue(CurrentDate, CurrentPeriodIndex, d->m_LegalFinal);
                if (!ReinvFlows.IsEmpty()) {
                    d->m_InterestAvailable += ReinvFlows.GetInterest(CurrentDate);
                    d->m_PrincipalAvailable.AddScheduled(ReinvFlows.GetScheduled(CurrentDate));
                    d->m_PrincipalAvailable.AddPrepay(ReinvFlows.GetPrepay(CurrentDate));
                    d->m_ReinvestmentTest.RemoveBondFlow(CurrentDate);
                    d->m_MortgagesPayments.AddFlow(d->m_ReinvestmentTest.GetBondCashFlow());
                }
            }
            if (CurrentDate > d->m_MortgagesPayments.GetDate(0)) {
                CurrentAssetSum += d->m_MortgagesPayments.GetAmountOut(CurrentDate.addMonths(-1)) + d->m_ReinvestmentTest.GetQueuedCash(CurrentDate);
                ++CurrentAssetCount;
                TotalPayable = deannualiseCoupon(d->m_GICinterest.GetValue(CurrentDate) + d->m_GICBaseRateValue.GetValue(CurrentDate), CurrentDate.addMonths(-1), CurrentDate, d->m_DealDayCountConvention.GetValue(CurrentDate));
                d->m_GICflows.AddFlow(CurrentDate, d->m_InterestAvailable*TotalPayable, TrancheCashFlow::TrancheFlowType::InterestFlow);
                d->m_GICflows.AddFlow(CurrentDate, d->m_PrincipalAvailable.Total()*TotalPayable, TrancheCashFlow::TrancheFlowType::PrincipalFlow);
                d->m_GICflows.AddFlow(CurrentDate, d->m_ReinvestmentTest.GetQueuedCash(CurrentDate)*TotalPayable, 2 | TrancheCashFlow::TrancheFlowType::InterestFlow);
                for (auto SingleRes = d->m_Reserves.constBegin(); SingleRes != d->m_Reserves.constEnd(); ++SingleRes)
                {
                    d->m_GICflows.AddFlow(CurrentDate, (*SingleRes)->GetReserveFundCurrent()*TotalPayable, 1 | TrancheCashFlow::TrancheFlowType::InterestFlow);
                }
                d->m_InterestAvailable += d->m_GICflows.GetTotalFlow(CurrentDate);
            }
            d->m_PrincipalAvailable.AddScheduled(d->m_MortgagesPayments.GetScheduled(CurrentDate));
            d->m_PrincipalAvailable.AddPrepay(d->m_MortgagesPayments.GetPrepay(CurrentDate));
            d->m_InterestAvailable += d->m_MortgagesPayments.GetInterest(CurrentDate);
            if (d->m_UseCall && !IsCallPaymentDate) {
                ActualCallReserveLevel = 0.0;
                TotalPayable = 0.0;
                if (d->m_CallReserve>0 && d->m_CallMultiple>0) {
                    for (auto SingleTranche = d->m_Tranches.constBegin(); SingleTranche != d->m_Tranches.constEnd(); ++SingleTranche){
                        TotalPayable += (*SingleTranche)->GetCashFlow().GetAmountOutstanding(CurrentDate);
                        if ((*SingleTranche)->GetProrataGroup(0) >= d->m_CallReserve) 
                            ActualCallReserveLevel += (*SingleTranche)->GetCashFlow().GetAmountOutstanding(CurrentDate);
                    }
                    if (ActualCallReserveLevel == 0.0) {
                        ActualCallReserveLevel = d->m_CallReserve;
                    }
                    else {
                        for (auto ResIter = d->m_Reserves.constBegin(); ResIter != d->m_Reserves.constEnd(); ++ResIter)
                            ActualCallReserveLevel += (*ResIter)->GetReserveFundCurrent();
                    }
                    ActualCallReserveLevel *= d->m_CallMultiple;
                    IsCallPaymentDate = ActualCallReserveLevel >= (TotalPayable - d->m_PrincipalAvailable.Total());
                }
                IsCallPaymentDate = IsCallPaymentDate || (!d->m_CallDate.isNull() && CurrentDate >= d->m_CallDate);
            }
            IsCallPaymentDate = IsCallPaymentDate || CurrentDate >= d->m_LegalFinal;
            if ((CurrentDate.year()<RollingNextIPD.year() || (CurrentDate.year() == RollingNextIPD.year() && CurrentDate.month()<RollingNextIPD.month())) && !IsMaturityDate) {
                //This is not a Tranche payment date
                for (auto SingleStep = d->m_WaterfallStesps.constBegin(); SingleStep != d->m_WaterfallStesps.constEnd(); ++SingleStep){
                    if ((*SingleStep)->GetPriorityType() == WatFalPrior::WaterfallStepType::wst_ReinvestPrincipal) {
                        if (
                            (
                            !(*SingleStep)->HasParameter(WatFalPrior::wstParameters::Trigger)
                            || TriggerPassing((*SingleStep)->GetParameter(WatFalPrior::wstParameters::Trigger).toString(), CurrentPeriodIndex, RollingNextIPD, IsCallPaymentDate || IsMaturityDate)
                            )
                            && !IsCallPaymentDate
                            && !IsMaturityDate
                            ) {
                            //If it should, reinvest
                            if (d->m_PrincipalAvailable.Total() > 0.0) {
                                double PayablePrincipal;
                                if ((*SingleStep)->GetParameter(WatFalPrior::wstParameters::SourceOfFunding).value<IntegerVector>().GetValue(CurrentDate) == 1) {  //Reinvest Prepay Only
                                    PayablePrincipal = d->m_PrincipalAvailable.GetPrepay()*(*SingleStep)->GetParameter(WatFalPrior::wstParameters::AdditionalCollateralShare).value<BloombergVector>().GetValue(CurrentDate);
                                    d->m_PrincipalAvailable.AddPrepay(-PayablePrincipal);
                                }
                                else if ((*SingleStep)->GetParameter(WatFalPrior::wstParameters::SourceOfFunding).value<IntegerVector>().GetValue(CurrentDate) == 2) {  //Reinvest scheduled only
                                    PayablePrincipal = d->m_PrincipalAvailable.GetScheduled()*(*SingleStep)->GetParameter(WatFalPrior::wstParameters::AdditionalCollateralShare).value<BloombergVector>().GetValue(CurrentDate);
                                    d->m_PrincipalAvailable.AddScheduled(-PayablePrincipal);
                                }
                                else {
                                    PayablePrincipal = d->m_PrincipalAvailable.Total()*(*SingleStep)->GetParameter(WatFalPrior::wstParameters::AdditionalCollateralShare).value<BloombergVector>().GetValue(CurrentDate);
                                    d->m_PrincipalAvailable -= PayablePrincipal;
                                }

                                {
                                    //Fill the reinvestments queue and process the current reinvestments
                                    d->m_ReinvestmentTest.QueueReinvestments(PayablePrincipal, CurrentDate);
                                    const MtgCashFlow& ReinvFlows = d->m_ReinvestmentTest.ProcessQueue(CurrentDate, CurrentPeriodIndex, d->m_LegalFinal);
                                    if (!ReinvFlows.IsEmpty()) {
                                        d->m_InterestAvailable += ReinvFlows.GetInterest(CurrentDate);
                                        d->m_PrincipalAvailable.AddScheduled(ReinvFlows.GetScheduled(CurrentDate));
                                        d->m_PrincipalAvailable.AddPrepay(ReinvFlows.GetPrepay(CurrentDate));
                                        d->m_ReinvestmentTest.RemoveBondFlow(CurrentDate);
                                        d->m_MortgagesPayments.AddFlow(d->m_ReinvestmentTest.GetBondCashFlow());
                                    }
                                }
                            }
                        }
                    }
                }
                continue;
            }
            //This is a Tranche payment date
            AvailablePrincipal = d->m_PrincipalAvailable;
            AvailableInterest = d->m_InterestAvailable;
            for (auto ResIter = d->m_Reserves.begin(); ResIter != d->m_Reserves.end(); ResIter++) {
                if ((*ResIter)->GetReserveToInterest()) AvailableInterest += (*ResIter)->GetReserveFundCurrent();
                else AvailablePrincipal += (*ResIter)->GetReserveFundCurrent();
                (*ResIter)->SetReserveFundCurrent(0.0);
            }
            if (IsCallPaymentDate) {
                d->m_CalledPeriod = RollingNextIPD;
                ActualCallDate = CurrentDate;
                if (d->m_PoolValueAtCall.IsEmpty())
                    TotalPayable = d->m_MortgagesPayments.GetWAPrice(CurrentDate);
                else
                    TotalPayable = d->m_PoolValueAtCall.GetValue(CurrentDate);
                AvailablePrincipal += TotalPayable*d->m_MortgagesPayments.GetAmountOut(CurrentDate);
                LOGDEBUG(QString("Cash in queue at call:\t%1").arg(d->m_ReinvestmentTest.GetQueuedCash(CurrentDate), 0, 'f'));
                AvailablePrincipal += d->m_ReinvestmentTest.GetQueuedCash(CurrentDate);
                d->m_ReinvestmentTest.ResetReinvestQueueue();
            }
            foreach(WatFalPrior* SingleStep, d->m_WaterfallStesps)
            {//Cycle through the steps of the waterfall
                if (SingleStep->HasParameter(WatFalPrior::wstParameters::Trigger)) {
                    if (!TriggerPassing(SingleStep->GetParameter(WatFalPrior::wstParameters::Trigger).toString(), CurrentPeriodIndex, RollingNextIPD, IsCallPaymentDate || IsMaturityDate))
                        continue;
                }
                switch (SingleStep->GetPriorityType()) {
                    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                case WatFalPrior::WaterfallStepType::wst_SeniorExpenses:
                    adjSeniorExpenses = deannualiseCoupon(d->m_SeniorExpenses.GetValue(CurrentDate), RollingLastIPD, RollingNextIPD, d->m_DealDayCountConvention.GetValue(CurrentDate));
                    TotalPayable = adjSeniorExpenses*(CurrentAssetSum / static_cast<double>(CurrentAssetCount))
                        - d->m_TotalSeniorExpenses.GetFlow(CurrentDate, TrancheCashFlow::TrancheFlowType::InterestFlow)
                        - d->m_TotalSeniorExpenses.GetFlow(CurrentDate, TrancheCashFlow::TrancheFlowType::PrincipalFlow)
                        ;
                    TotalPayable += d->m_SeniorExpensesFixed.GetValue(CurrentDate)*static_cast<double>(d->m_PaymentFrequency.GetValue(CurrentDate)) / 12.0;
                    if (TotalPayable >= 0.01) {
                        if (SingleStep->GetParameter(WatFalPrior::wstParameters::SourceOfFunding).value<IntegerVector>().GetValue(CurrentDate) == 1) {
                            d->m_TotalSeniorExpenses.AddFlow(CurrentDate, qMin(AvailableInterest, TotalPayable), TrancheCashFlow::TrancheFlowType::InterestFlow);
                            AvailableInterest = qMax(0.0, AvailableInterest - TotalPayable);
                        }
                        else if (SingleStep->GetParameter(WatFalPrior::wstParameters::SourceOfFunding).value<IntegerVector>().GetValue(CurrentDate) == 2) {
                            d->m_TotalSeniorExpenses.AddFlow(CurrentDate, qMin(AvailablePrincipal.Total(), TotalPayable), TrancheCashFlow::TrancheFlowType::PrincipalFlow);
                            AvailablePrincipal -= TotalPayable;
                            if (AvailablePrincipal < 0.0) AvailablePrincipal.Erase();
                        }
                    }
                    break;
                    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                case WatFalPrior::WaterfallStepType::wst_SeniorFees:
                    adjSeniorFees = deannualiseCoupon(d->m_SeniorFees.GetValue(CurrentDate), RollingLastIPD, RollingNextIPD, d->m_DealDayCountConvention.GetValue(CurrentDate));
                    TotalPayable = adjSeniorFees*(CurrentAssetSum / static_cast<double>(CurrentAssetCount))
                        - d->m_TotalSeniorFees.GetFlow(CurrentDate, TrancheCashFlow::TrancheFlowType::InterestFlow)
                        - d->m_TotalSeniorFees.GetFlow(CurrentDate, TrancheCashFlow::TrancheFlowType::PrincipalFlow)
                        ;
                    TotalPayable += d->m_SeniorFeesFixed.GetValue(CurrentDate)*static_cast<double>(d->m_PaymentFrequency.GetValue(CurrentDate)) / 12.0;;
                    if (TotalPayable >= 0.01) {
                        if (SingleStep->GetParameter(WatFalPrior::wstParameters::SourceOfFunding).value<IntegerVector>().GetValue(CurrentDate) == 1) {
                            d->m_TotalSeniorFees.AddFlow(CurrentDate, qMin(AvailableInterest, TotalPayable), TrancheCashFlow::TrancheFlowType::InterestFlow);
                            AvailableInterest = qMax(0.0, AvailableInterest - TotalPayable);
                        }
                        else if (SingleStep->GetParameter(WatFalPrior::wstParameters::SourceOfFunding).value<IntegerVector>().GetValue(CurrentDate) == 2) {
                            d->m_TotalSeniorFees.AddFlow(CurrentDate, qMin(AvailablePrincipal.Total(), TotalPayable), TrancheCashFlow::TrancheFlowType::PrincipalFlow);
                            AvailablePrincipal -= TotalPayable;
                            if (AvailablePrincipal < 0.0) AvailablePrincipal.Erase();
                        }
                    }
                    break;
                    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                case WatFalPrior::WaterfallStepType::wst_juniorFees:
                    if (SingleStep->GetParameter(WatFalPrior::wstParameters::PayAccrue).toInt() & WatFalPrior::wstAccrueOrPay::Accrue) {
                        adjJuniorFees = deannualiseCoupon(d->m_JuniorFees.GetValue(CurrentDate), RollingLastIPD, RollingNextIPD, d->m_DealDayCountConvention.GetValue(CurrentDate));
                        TotalPayable = (adjJuniorFees*(CurrentAssetSum / static_cast<double>(CurrentAssetCount)))
                            - d->m_TotalJuniorFees.GetFlow(CurrentDate, TrancheCashFlow::TrancheFlowType::AccruedFlow)
                            + (deannualiseCoupon(d->m_JuniorFeesCoupon, RollingLastIPD, RollingNextIPD, d->m_DealDayCountConvention.GetValue(CurrentDate))*(d->m_TotalJuniorFees.GetFlow(CurrentDate, TrancheCashFlow::TrancheFlowType::DeferredFlow) + d->m_StartingDeferredJunFees))
                            ;
                        d->m_StartingDeferredJunFees = 0.0;
                        TotalPayable += d->m_JuniorFeesFixed.GetValue(CurrentDate)*static_cast<double>(d->m_PaymentFrequency.GetValue(CurrentDate)) / 12.0;;
                        TotalPayable = qMax(TotalPayable, 0.0);
                        d->m_TotalJuniorFees.AddFlow(CurrentDate, TotalPayable, TrancheCashFlow::TrancheFlowType::AccruedFlow);
                    }
                    if (SingleStep->GetParameter(WatFalPrior::wstParameters::PayAccrue).toInt() & WatFalPrior::wstAccrueOrPay::Pay) {
                        TotalPayable =
                            d->m_TotalJuniorFees.GetFlow(CurrentDate, TrancheCashFlow::TrancheFlowType::AccruedFlow)
                            + d->m_TotalJuniorFees.GetFlow(CurrentDate, TrancheCashFlow::TrancheFlowType::DeferredFlow)
                            - d->m_TotalJuniorFees.GetTotalFlow(CurrentDate, QList<qint32>() << TrancheCashFlow::TrancheFlowType::InterestFlow << TrancheCashFlow::TrancheFlowType::PrincipalFlow)
                            ;
                        if (SingleStep->GetParameter(WatFalPrior::wstParameters::SourceOfFunding).value<IntegerVector>().GetValue(CurrentDate) == 1) {
                            Solution = qMin(AvailableInterest, TotalPayable);
                            d->m_TotalJuniorFees.AddFlow(CurrentDate, Solution, TrancheCashFlow::TrancheFlowType::InterestFlow);
                            d->m_AnnualizedExcess.AddFlow(CurrentDate, Solution, TrancheCashFlow::TrancheFlowType::InterestFlow);
                            AvailableInterest -= Solution;
                        }
                        if (SingleStep->GetParameter(WatFalPrior::wstParameters::SourceOfFunding).value<IntegerVector>().GetValue(CurrentDate) == 2) {
                            Solution = qMin(AvailablePrincipal.Total(), TotalPayable);
                            d->m_TotalJuniorFees.AddFlow(CurrentDate, Solution, TrancheCashFlow::TrancheFlowType::PrincipalFlow);
                            d->m_AnnualizedExcess.AddFlow(CurrentDate, Solution, TrancheCashFlow::TrancheFlowType::PrincipalFlow);
                            AvailablePrincipal -= Solution;
                        }
                    }
                    break;

                    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                case WatFalPrior::WaterfallStepType::wst_FeesFromExcess:
                    TotalPayable = 0.0;
                    if (SingleStep->GetParameter(WatFalPrior::wstParameters::SourceOfFunding).value<IntegerVector>().GetValue(CurrentDate) & 1) {
                        double RedSh = SingleStep->GetParameter(WatFalPrior::wstParameters::RedemptionShare).value<BloombergVector>().GetValue(CurrentDate);
                        TotalPayable += AvailableInterest*RedSh;
                        AvailableInterest *= (1.0 - RedSh);
                    }
                    if (SingleStep->GetParameter(WatFalPrior::wstParameters::SourceOfFunding).value<IntegerVector>().GetValue(CurrentDate) >= 2) {
                        double RedSh = SingleStep->GetParameter(WatFalPrior::wstParameters::RedemptionShare).value<BloombergVector>().GetValue(CurrentDate);
                        TotalPayable += AvailablePrincipal.Total()*RedSh;
                        AvailablePrincipal -= AvailablePrincipal.Total()*RedSh;
                    }
                    d->m_TotalJuniorFees.AddFlow(CurrentDate, TotalPayable, TrancheCashFlow::TrancheFlowType::InterestFlow + 1);
                    d->m_AnnualizedExcess.AddFlow(CurrentDate, TotalPayable, TrancheCashFlow::TrancheFlowType::InterestFlow + 1);
                    break;
                    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                case WatFalPrior::WaterfallStepType::wst_ReserveReplenish:
                {
                    int RevIdx = SingleStep->GetParameter(WatFalPrior::wstParameters::ReserveIndex).value<IntegerVector>().GetValue(CurrentDate);
                    if (RevIdx< 0 || RevIdx >= d->m_Reserves.size()) {
                        PrintToTempFile("ReturnFalse.txt", "Reserve Fund Fail");
                        return false;
                    }
                    double FreedAmnt, StacedRsAmt = 0.0;
                    int MostJun = FindMostJuniorLevel(0);

                    if (d->m_Reserves.at(RevIdx)->GetReserveFundFreed() == 0 || d->m_Reserves.at(RevIdx)->GetReserveFundFreed()>MostJun)
                        FreedAmnt = 1.0;
                    else {
                        FreedAmnt = 0.0;
                        for (int h = 0; h<d->m_Tranches.size(); h++) {
                            if (d->m_Tranches.at(h)->GetProrataGroup(0) <= d->m_Reserves.at(RevIdx)->GetReserveFundFreed())
                                FreedAmnt += d->m_Tranches.at(h)->GetCashFlow().GetAmountOutstanding(CurrentDate);
                        }
                    }
                    if (!IsCallPaymentDate && !IsMaturityDate &&  FreedAmnt>0.01) {
                        TestTarget = 0.0;
                        if (qRound(d->m_Reserves.at(RevIdx)->GetReserveFundTarget().GetValue(CurrentDate))>MostJun) TestTarget = d->m_Reserves.at(RevIdx)->GetReserveFundTarget().GetValue(CurrentDate);
                        else {
                            foreach(Tranche* SingleTranche, d->m_Tranches)
                            {
                                if (SingleTranche->GetProrataGroup(0) <= qRound(d->m_Reserves.at(RevIdx)->GetReserveFundTarget().GetValue(CurrentDate))) TestTarget += SingleTranche->GetCashFlow().GetAmountOutstanding(CurrentDate);
                            }
                        }
                        if (d->m_CumulativeReserves) {
                            for (int ResIter = 0; ResIter < RevIdx; ResIter++)
                                StacedRsAmt += d->m_Reserves.at(ResIter)->GetReserveFundCurrent();
                        }
                        else StacedRsAmt = d->m_Reserves.at(RevIdx)->GetReserveFundCurrent();
                        TotalPayable = qMax(
                            d->m_Reserves.at(RevIdx)->GetReserveFundFloor().GetValue(CurrentDate),
                            TestTarget*d->m_Reserves.at(RevIdx)->GetReserveFundMultiple().GetValue(CurrentDate)
                            );
                        if (d->m_Reserves.at(RevIdx)->GetReserveFundCap().GetValue(CurrentDate) > 0.0)
                            TotalPayable = qMin(TotalPayable, d->m_Reserves.at(RevIdx)->GetReserveFundCap().GetValue(CurrentDate));
                        TotalPayable -= StacedRsAmt;

                        if (TotalPayable >= 0.01) {
                            if (SingleStep->GetParameter(WatFalPrior::wstParameters::SourceOfFunding).value<IntegerVector>().GetValue(CurrentDate) == 1) {
                                d->m_Reserves[RevIdx]->GetReserveFundFlow().AddFlow(CurrentDate, qMin(AvailableInterest, TotalPayable), ReserveFund::ReserveFlowsType::ReplenishFromInterest);
                                d->m_Reserves[RevIdx]->GetReserveFundFlow().SetFlow(CurrentDate, qMax(0.0, TotalPayable - AvailableInterest), ReserveFund::ReserveFlowsType::ShortFall);
                                d->m_Reserves[RevIdx]->SetReserveFundCurrent(d->m_Reserves.at(RevIdx)->GetReserveFundCurrent() + qMin(AvailableInterest, TotalPayable));
                                AvailableInterest = qMax(0.0, AvailableInterest - TotalPayable);
                            }
                            else if (SingleStep->GetParameter(WatFalPrior::wstParameters::SourceOfFunding).value<IntegerVector>().GetValue(CurrentDate) == 2) {

                                d->m_Reserves[RevIdx]->GetReserveFundFlow().AddFlow(CurrentDate, qMin(AvailablePrincipal.Total(), TotalPayable), ReserveFund::ReserveFlowsType::ReplenishFromPrincipal);
                                d->m_Reserves[RevIdx]->GetReserveFundFlow().SetFlow(CurrentDate, qMax(0.0, TotalPayable - AvailablePrincipal.Total()), ReserveFund::ReserveFlowsType::ShortFall);
                                d->m_Reserves[RevIdx]->SetReserveFundCurrent(d->m_Reserves.at(RevIdx)->GetReserveFundCurrent() + qMin(AvailablePrincipal.Total(), TotalPayable));
                                AvailablePrincipal -= TotalPayable;
                                if (AvailablePrincipal < 0.0) AvailablePrincipal.Erase();
                            }
                        }
                    }
                }
                break;
                ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                case WatFalPrior::WaterfallStepType::wst_Interest:{
                    int CurrSenGrpLvl = SingleStep->GetParameter(WatFalPrior::wstParameters::SeniorityGroupLevel).value<IntegerVector>().GetValue(CurrentDate);
                    int CurrSenGrp = SingleStep->GetParameter(WatFalPrior::wstParameters::SeniorityGroup).value<IntegerVector>().GetValue(CurrentDate);
                    int CurrCoupIndx = SingleStep->GetParameter(WatFalPrior::wstParameters::CouponIndex).value<IntegerVector>().GetValue(CurrentDate);
                    if (CurrSenGrp > FindMostJuniorLevel(CurrSenGrpLvl)) {
                        PrintToTempFile("ReturnFalse.txt", "Invalid seniority in Interest");
                        return false;
                    }
                    if (SingleStep->GetParameter(WatFalPrior::wstParameters::PayAccrue).toInt() & WatFalPrior::wstAccrueOrPay::Accrue) {
                        foreach(Tranche* SingleTranche, d->m_Tranches)
                        {
                            if (SingleTranche->GetProrataGroup(CurrSenGrpLvl) == CurrSenGrp) {
                                if (!SingleTranche->HasCoupon(CurrCoupIndx)) {
                                    PrintToTempFile("ReturnFalse.txt", SingleTranche->GetTrancheName() + " - Coupon not set in tranche");
                                    return false;
                                }
                                AdjustedCoupon = deannualiseCoupon(SingleTranche->GetCoupon(CurrentDate, CurrCoupIndx), RollingLastIPD, RollingNextIPD, SingleTranche->GetDayCount(CurrCoupIndx).GetValue(CurrentDate));
                                Solution = SingleTranche->GetCashFlow().GetDeferred(CurrentDate, CurrCoupIndx);
                                Solution += SingleTranche->GetCashFlow().GetAmountOutstanding(CurrentDate);
                                Solution *= AdjustedCoupon;
                                Solution -= SingleTranche->GetCashFlow().GetAccrued(CurrentDate, CurrCoupIndx);
                                SingleTranche->AddCashFlow(CurrentDate, Solution, TrancheCashFlow::TrancheFlowType::AccruedFlow | CurrCoupIndx);
                            }
                        }
                    }
                    if (SingleStep->GetParameter(WatFalPrior::wstParameters::PayAccrue).toInt() & WatFalPrior::wstAccrueOrPay::Pay) {
                        ProRataBonds.clear();
                        TotalPayable = 0.0;
                        Solution = 0.0;
                        for (int h = 0; h < d->m_Tranches.size(); h++) {
                            if (d->m_Tranches.at(h)->GetProrataGroup(CurrSenGrpLvl) == CurrSenGrp) {
                                TotalPayable += d->m_Tranches.at(h)->GetCashFlow().GetAccrued(CurrentDate, CurrCoupIndx) - d->m_Tranches.at(h)->GetCashFlow().GetInterest(CurrentDate, CurrCoupIndx);
                                Solution += d->m_Tranches.at(h)->GetCashFlow().GetAmountOutstanding(CurrentDate);
                                ProRataBonds.enqueue(h);
                            }
                        }
                        if (TotalPayable >= 0.0) {
                            if (AvailableInterest >= TotalPayable) {
                                //Enough funds to pay all interest
                                foreach(qint32 h, ProRataBonds)
                                {
                                    d->m_Tranches[h]->AddCashFlow(CurrentDate
                                        , d->m_Tranches.at(h)->GetCashFlow().GetAccrued(CurrentDate, CurrCoupIndx) - d->m_Tranches.at(h)->GetCashFlow().GetInterest(CurrentDate, CurrCoupIndx)
                                        , TrancheCashFlow::TrancheFlowType::InterestFlow | CurrCoupIndx
                                        );
                                }
                                ProRataBonds.clear();
                                AvailableInterest -= TotalPayable;
                            }
                            else if (Solution>0.0) {
                                //Pay pro-rata based on outstanding
                                foreach(qint32 h, ProRataBonds)
                                {
                                    d->m_Tranches[h]->AddCashFlow(CurrentDate
                                        , AvailableInterest * d->m_Tranches.at(h)->GetCashFlow().GetAmountOutstanding(CurrentDate) / Solution
                                        , TrancheCashFlow::TrancheFlowType::InterestFlow | CurrCoupIndx
                                        );
                                }
                                ProRataBonds.clear();
                                AvailableInterest = 0.0;
                            }
                            else {
                                // Pro rata based on original outstanding
                                Solution = 0.0;
                                foreach(qint32 h, ProRataBonds)
                                {
                                    Solution += d->m_Tranches.at(h)->GetOriginalAmount();
                                }
                                if (Solution > 0.0) {
                                    foreach(qint32 h, ProRataBonds)
                                    {
                                        d->m_Tranches[h]->AddCashFlow(CurrentDate
                                            , AvailableInterest * d->m_Tranches.at(h)->GetOriginalAmount() / Solution
                                            , TrancheCashFlow::TrancheFlowType::InterestFlow | CurrCoupIndx
                                            );
                                    }
                                }
                                else {
                                    //Pay pro-rata in equal parts
                                    LOGDEBUG("Reached Pay Interest on 0 Outstanding");
                                    foreach(qint32 h, ProRataBonds)
                                    {
                                        d->m_Tranches[h]->AddCashFlow(CurrentDate
                                            , AvailableInterest / static_cast<double>(ProRataBonds.size())
                                            , TrancheCashFlow::TrancheFlowType::InterestFlow | CurrCoupIndx
                                            );
                                    }
                                }
                                ProRataBonds.clear();
                                AvailableInterest = 0.0;
                            }
                        }
                    }
                }
                break;
 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                case WatFalPrior::WaterfallStepType::wst_DeferredInterest:{
                    ProRataBonds.clear();
                    TotalPayable = Solution = 0.0;
                    int CurrSenGrpLvl = SingleStep->GetParameter(WatFalPrior::wstParameters::SeniorityGroupLevel).value<IntegerVector>().GetValue(CurrentDate);
                    int CurrSenGrp = SingleStep->GetParameter(WatFalPrior::wstParameters::SeniorityGroup).value<IntegerVector>().GetValue(CurrentDate);
                    int CurrCoupIndx = SingleStep->GetParameter(WatFalPrior::wstParameters::CouponIndex).value<IntegerVector>().GetValue(CurrentDate);
                    if (CurrSenGrp > FindMostJuniorLevel(CurrSenGrpLvl)) {
                        PrintToTempFile("ReturnFalse.txt", "Invalid seniority in Deferred Interest");
                        return false;
                    }
                    for (int h = 0; h < d->m_Tranches.size(); h++) {
                        if (d->m_Tranches.at(h)->GetProrataGroup(CurrSenGrpLvl) == CurrSenGrp) {
                            if (!d->m_Tranches.at(h)->HasCoupon(CurrCoupIndx)) {
                                PrintToTempFile("ReturnFalse.txt", d->m_Tranches.at(h)->GetTrancheName() + " - Coupon not set in tranche");
                                return false;
                            }
                            ProRataBonds.enqueue(h);
                            TotalPayable +=
                                d->m_Tranches.at(h)->GetCashFlow().GetAccrued(CurrentDate, CurrCoupIndx)
                                + d->m_Tranches.at(h)->GetCashFlow().GetDeferred(CurrentDate, CurrCoupIndx)
                                - d->m_Tranches.at(h)->GetCashFlow().GetInterest(CurrentDate, CurrCoupIndx);
                            ;
                            Solution += d->m_Tranches.at(h)->GetCashFlow().GetAmountOutstanding(CurrentDate);
                        }
                    }
                    if (TotalPayable > 0.0) {
                        double FundsToUse;
                        if (SingleStep->GetParameter(WatFalPrior::wstParameters::SourceOfFunding).value<IntegerVector>().GetValue(CurrentDate) == 1) {
                            FundsToUse = qMin(AvailableInterest, TotalPayable);
                            AvailableInterest -= FundsToUse;
                        }
                        else {
                            FundsToUse = qMin(AvailablePrincipal.Total(), TotalPayable);
                            AvailablePrincipal -= TotalPayable;
                            if (AvailablePrincipal < 0.0) AvailablePrincipal.Erase();
                        }
                        if (FundsToUse >= TotalPayable) {
                            //Enough funds to pay all deferred
                            foreach(qint32 h, ProRataBonds)
                            {
                                d->m_Tranches[h]->AddCashFlow(CurrentDate,
                                    d->m_Tranches.at(h)->GetCashFlow().GetAccrued(CurrentDate, CurrCoupIndx)
                                    + d->m_Tranches.at(h)->GetCashFlow().GetDeferred(CurrentDate, CurrCoupIndx)
                                    - d->m_Tranches.at(h)->GetCashFlow().GetInterest(CurrentDate, CurrCoupIndx)
                                    , TrancheCashFlow::TrancheFlowType::InterestFlow | CurrCoupIndx);
                            }
                        }
                        else if (Solution > 0.0) {
                            //Pro-rata based on outstanding
                            foreach(qint32 h, ProRataBonds)
                            {
                                d->m_Tranches[h]->AddCashFlow(CurrentDate, FundsToUse*d->m_Tranches.at(h)->GetCashFlow().GetAmountOutstanding(CurrentDate) / Solution, TrancheCashFlow::TrancheFlowType::InterestFlow | CurrCoupIndx);
                            }
                        }
                        else {
                            //Pro-Rata based on original outstanding
                            Solution = 0.0;
                            foreach(qint32 h, ProRataBonds)
                            {
                                Solution += d->m_Tranches.at(h)->GetOriginalAmount();
                            }
                            if (Solution > 0.0) {
                                foreach(qint32 h, ProRataBonds)
                                {
                                    d->m_Tranches[h]->AddCashFlow(CurrentDate, FundsToUse*d->m_Tranches.at(h)->GetOriginalAmount() / Solution, TrancheCashFlow::TrancheFlowType::InterestFlow | CurrCoupIndx);
                                }
                            }
                            else {
                                //Pro-rata equal parts
                                LOGDEBUG("Reached Pay Deferred on 0 Outstanding");
                                foreach(qint32 h, ProRataBonds)
                                {
                                    d->m_Tranches[h]->AddCashFlow(CurrentDate, FundsToUse / static_cast<double>(ProRataBonds.size()), TrancheCashFlow::TrancheFlowType::InterestFlow | CurrCoupIndx);
                                }
                            }
                        }
                        ProRataBonds.clear();
                    }
                }
                break;
                ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                case  WatFalPrior::WaterfallStepType::wst_Principal:{
                    int CurrSenGrpLvl = SingleStep->GetParameter(WatFalPrior::wstParameters::SeniorityGroupLevel).value<IntegerVector>().GetValue(CurrentDate);
                    int CurrSenGrp = SingleStep->GetParameter(WatFalPrior::wstParameters::SeniorityGroup).value<IntegerVector>().GetValue(CurrentDate);
                    if (CurrSenGrp > FindMostJuniorLevel(CurrSenGrpLvl)) {
                        PrintToTempFile("ReturnFalse.txt", "Invalid seniority in Principal");
                        return false;
                    }
                    TotalPayable = RedeemNotes(AvailablePrincipal.Total(), CurrSenGrp, CurrSenGrpLvl, CurrentDate);
                    AvailablePrincipal -= AvailablePrincipal.Total() - TotalPayable;
                }
                break;
                ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                case WatFalPrior::WaterfallStepType::wst_Turbo:{
                    int CurrSenGrpLvl = SingleStep->GetParameter(WatFalPrior::wstParameters::RedemptionGroupLevel).value<IntegerVector>().GetValue(CurrentDate);
                    int CurrSenGrp = SingleStep->GetParameter(WatFalPrior::wstParameters::RedemptionGroup).value<IntegerVector>().GetValue(CurrentDate);
                    if (CurrSenGrp > FindMostJuniorLevel(CurrSenGrpLvl)) {
                        PrintToTempFile("ReturnFalse.txt", "Invalid seniority in Turbo");
                        return false;
                    }
                    TotalPayable = RedeemNotes(
                        AvailableInterest*SingleStep->GetParameter(WatFalPrior::wstParameters::RedemptionShare).value<BloombergVector>().GetValue(CurrentDate)
                        , CurrSenGrp
                        , CurrSenGrpLvl
                        , CurrentDate
                        );
                    AvailableInterest += TotalPayable - (AvailableInterest*SingleStep->GetParameter(WatFalPrior::wstParameters::RedemptionShare).value<BloombergVector>().GetValue(CurrentDate));
                }
                                                               break;
                ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                case WatFalPrior::WaterfallStepType::wst_AllocPrepayFees:{
                    int CurrSenGrpLvl = SingleStep->GetParameter(WatFalPrior::wstParameters::RedemptionGroupLevel).value<IntegerVector>().GetValue(CurrentDate);
                    int CurrSenGrp = SingleStep->GetParameter(WatFalPrior::wstParameters::RedemptionGroup).value<IntegerVector>().GetValue(CurrentDate);
                    bool UseToRedeem = SingleStep->GetParameter(WatFalPrior::wstParameters::SourceOfFunding).value<IntegerVector>().GetValue(CurrentDate) == 2;
                    if (CurrSenGrp > FindMostJuniorLevel(CurrSenGrpLvl)) {
                        PrintToTempFile("ReturnFalse.txt", "Invalid seniority in Turbo");
                        return false;
                    }
                    ProRataBonds.clear();
                    bool UsedOriginals = false;
                    TotalPayable = 0.0;
                    for (int h = 0; h < d->m_Tranches.size(); h++) {
                        if (d->m_Tranches.at(h)->GetProrataGroup(CurrSenGrpLvl) == CurrSenGrp) {
                            ProRataBonds.enqueue(h);
                            TotalPayable += d->m_Tranches.at(h)->GetCashFlow().GetAmountOutstanding(CurrentDate);
                        }
                    }
                    if (TotalPayable < 0.01) {
                        TotalPayable = 0.0;
                        UsedOriginals = true;
                        for (auto ProRataIte = ProRataBonds.constBegin(); ProRataIte != ProRataBonds.constEnd(); ++ProRataIte) {
                            TotalPayable += d->m_Tranches.at(*ProRataIte)->GetOriginalAmount();
                        }
                    }
                    Solution = qMin(AvailableInterest, d->m_MortgagesPayments.GetPrepayFees(CurrentDate)*SingleStep->GetParameter(WatFalPrior::wstParameters::RedemptionShare).value<BloombergVector>().GetValue(CurrentDate));
                    AvailableInterest -= Solution;
                    int OriginalProRataBondsSize = ProRataBonds.size();
                    while (ProRataBonds.size() > 0) {
                        int ProrataIndex = ProRataBonds.dequeue();
                        if (TotalPayable > 0) {
                            if (UseToRedeem)
                                Solution = RedeemNotes(Solution, CurrSenGrp, CurrSenGrpLvl, CurrentDate);
                            d->m_Tranches[ProrataIndex]->AddCashFlow(CurrentDate, Solution*(UsedOriginals ? d->m_Tranches.at(ProrataIndex)->GetOriginalAmount() : d->m_Tranches.at(ProrataIndex)->GetCashFlow().GetAmountOutstanding(CurrentDate)) / TotalPayable, TrancheCashFlow::TrancheFlowType::InterestFlow);
                        }
                        else {
                            d->m_Tranches[ProrataIndex]->AddCashFlow(CurrentDate, Solution / OriginalProRataBondsSize, TrancheCashFlow::TrancheFlowType::InterestFlow);
                        }
                    }

                }
                break;
                ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                case WatFalPrior::WaterfallStepType::wst_Excess:{
                    SolutionDegree = SingleStep->GetParameter(WatFalPrior::wstParameters::SourceOfFunding).value<IntegerVector>().GetValue(CurrentDate);
                    TestTarget = SingleStep->GetParameter(WatFalPrior::wstParameters::RedemptionShare).value<BloombergVector>().GetValue(CurrentDate);
                    int CurrSenGrpLvl = SingleStep->GetParameter(WatFalPrior::wstParameters::RedemptionGroupLevel).value<IntegerVector>().GetValue(CurrentDate);
                    int CurrSenGrp = SingleStep->GetParameter(WatFalPrior::wstParameters::RedemptionGroup).value<IntegerVector>().GetValue(CurrentDate);
                    if (CurrSenGrp > FindMostJuniorLevel(CurrSenGrpLvl)) {
                        PrintToTempFile("ReturnFalse.txt", "Invalid seniority in Excess spread");
                        return false;
                    }
                    if (SolutionDegree & 1) {
                        d->m_AnnualizedExcess.AddFlow(CurrentDate, AvailableInterest, TrancheCashFlow::TrancheFlowType::InterestFlow);
                        d->m_EquityIncome.AddFlow(CurrentDate, AvailableInterest, TrancheCashFlow::TrancheFlowType::InterestFlow);
                    }
                    if (SolutionDegree >= 2) {
                        d->m_EquityIncome.AddFlow(CurrentDate, AvailablePrincipal.Total(), TrancheCashFlow::TrancheFlowType::PrincipalFlow);
                    }
                    if (TestTarget > 0.0) {
                        ProRataBonds.clear();
                        TotalPayable = 0.0;
                        for (int h = 0; h < d->m_Tranches.size(); h++) {
                            if (d->m_Tranches.at(h)->GetProrataGroup(CurrSenGrpLvl) == CurrSenGrp) {
                                ProRataBonds.enqueue(h);
                                TotalPayable += d->m_Tranches.at(h)->GetOriginalAmount();
                            }
                        }
                        {
                            int OriginalProRataBondsSize = ProRataBonds.size();
                            while (ProRataBonds.size()>0) {
                                int ProrataIndex = ProRataBonds.dequeue();
                                if (TotalPayable > 0) {
                                    if (SolutionDegree & 1) 
                                        d->m_Tranches[ProrataIndex]->AddCashFlow(CurrentDate, AvailableInterest*TestTarget*d->m_Tranches.at(ProrataIndex)->GetOriginalAmount() / TotalPayable, TrancheCashFlow::TrancheFlowType::InterestFlow);
                                    if (SolutionDegree >= 2) {
                                        double TmpPrincEx = TestTarget*AvailablePrincipal.Total()*d->m_Tranches.at(ProrataIndex)->GetOriginalAmount() / TotalPayable;
                                        d->m_Tranches[ProrataIndex]->AddCashFlow(CurrentDate, TmpPrincEx, TrancheCashFlow::TrancheFlowType::PrincipalFlow);
                                        d->m_Tranches[ProrataIndex]->AddCashFlow(CurrentDate, -qMin(d->m_Tranches[ProrataIndex]->GetCashFlow().GetAmountOutstanding(CurrentDate), TmpPrincEx), TrancheCashFlow::TrancheFlowType::AmountOutstandingFlow);
                                    }
                                }
                                else {
                                    if (SolutionDegree & 1) 
                                        d->m_Tranches[ProrataIndex]->AddCashFlow(CurrentDate, AvailableInterest*TestTarget / OriginalProRataBondsSize, TrancheCashFlow::TrancheFlowType::InterestFlow);
                                    if (SolutionDegree >= 2) {
                                        d->m_Tranches[ProrataIndex]->AddCashFlow(CurrentDate, TestTarget*AvailablePrincipal.Total() / OriginalProRataBondsSize, TrancheCashFlow::TrancheFlowType::PrincipalFlow);
                                        d->m_Tranches[ProrataIndex]->AddCashFlow(CurrentDate, -qMin(d->m_Tranches[ProrataIndex]->GetCashFlow().GetAmountOutstanding(CurrentDate), TestTarget*AvailablePrincipal.Total() / OriginalProRataBondsSize), TrancheCashFlow::TrancheFlowType::AmountOutstandingFlow);
                                    }
                                }
                            }
                        }
                    }

                    if (SolutionDegree >= 2) d->m_ExcessCashFlow.AddFlow(CurrentDate, (1.0 - TestTarget)*AvailablePrincipal.Total(), TrancheCashFlow::TrancheFlowType::PrincipalFlow);
                    if (SolutionDegree & 1) d->m_ExcessCashFlow.AddFlow(CurrentDate, (1.0 - TestTarget)* AvailableInterest, TrancheCashFlow::TrancheFlowType::InterestFlow);

                    if (SolutionDegree & 1) AvailableInterest = 0.0;
                    if (SolutionDegree >= 2) AvailablePrincipal.Erase();
                }
                break;
                ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                case WatFalPrior::WaterfallStepType::wst_PDL:{
                    int CurrSenGrpLvl = SingleStep->GetParameter(WatFalPrior::wstParameters::SeniorityGroupLevel).value<IntegerVector>().GetValue(CurrentDate);
                    int CurrSenGrp = SingleStep->GetParameter(WatFalPrior::wstParameters::SeniorityGroup).value<IntegerVector>().GetValue(CurrentDate);
                    int CurrRedGrpLvl = SingleStep->GetParameter(WatFalPrior::wstParameters::RedemptionGroupLevel).value<IntegerVector>().GetValue(CurrentDate);
                    int CurrRedGrp = SingleStep->GetParameter(WatFalPrior::wstParameters::RedemptionGroup).value<IntegerVector>().GetValue(CurrentDate);
                    if (
                        CurrSenGrp > FindMostJuniorLevel(CurrSenGrpLvl)
                        || (
                        !SingleStep->GetParameter(WatFalPrior::wstParameters::RedemptionGroup).value<IntegerVector>().IsEmpty()
                        && CurrRedGrp > FindMostJuniorLevel(CurrRedGrpLvl)
                        )
                        ) {
                        PrintToTempFile("ReturnFalse.txt", "Invalid seniority in PDL cure");
                        return false;
                    }
                    ProRataBonds.clear();
                    TotalPayable = TestTarget = 0.0;
                    for (int h = 0; h < d->m_Tranches.size(); h++) {
                        if (d->m_Tranches.at(h)->GetProrataGroup(CurrSenGrpLvl) <= CurrSenGrp) {
                            TotalPayable += d->m_Tranches.at(h)->GetCashFlow().GetAmountOutstanding(CurrentDate);
                            if (d->m_Tranches.at(h)->GetProrataGroup(CurrSenGrpLvl) == CurrSenGrp) {
                                TestTarget += d->m_Tranches.at(h)->GetCashFlow().GetAmountOutstanding(CurrentDate);
                                ProRataBonds.enqueue(h);
                            }
                        }
                    }
                    Solution = qMin(TotalPayable - d->m_MortgagesPayments.GetAmountOut(CurrentDate) - AvailablePrincipal.Total(), TestTarget);
                    if (Solution >= 0.01) {
                        foreach(const int& SingleBond, ProRataBonds)
                        {
                            d->m_Tranches[SingleBond]->SetCashFlow(CurrentDate, qMax(0.0, Solution - AvailableInterest)* d->m_Tranches.at(SingleBond)->GetCashFlow().GetAmountOutstanding(CurrentDate) / TestTarget, TrancheCashFlow::TrancheFlowType::PDLOutstanding);
                            d->m_Tranches[SingleBond]->AddCashFlow(CurrentDate, qMin(AvailableInterest, Solution)* d->m_Tranches.at(SingleBond)->GetCashFlow().GetAmountOutstanding(CurrentDate) / TestTarget, TrancheCashFlow::TrancheFlowType::PDLCured);
                        }
                        TotalPayable = qMin(AvailableInterest, Solution);
                        if (!SingleStep->GetParameter(WatFalPrior::wstParameters::RedemptionGroup).value<IntegerVector>().IsEmpty() && SingleStep->GetParameter(WatFalPrior::wstParameters::RedemptionShare).value<BloombergVector>().GetValue(CurrentDate) > 0.0) {
                            TotalPayable =
                                (TotalPayable*(1.0 - SingleStep->GetParameter(WatFalPrior::wstParameters::RedemptionShare).value<BloombergVector>().GetValue(CurrentDate)))
                                + RedeemNotes(
                                SingleStep->GetParameter(WatFalPrior::wstParameters::RedemptionShare).value<BloombergVector>().GetValue(CurrentDate)*TotalPayable
                                , CurrRedGrp
                                , CurrRedGrpLvl
                                , CurrentDate
                                );
                        }
                        TotalPayable = RedeemSequential(TotalPayable, CurrentDate, CurrRedGrpLvl);
                        AvailableInterest += TotalPayable - qMin(AvailableInterest, Solution);
                    }
                }
                break;
                ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                case WatFalPrior::WaterfallStepType::wst_ReinvestmentTest:
                case WatFalPrior::WaterfallStepType::wst_OCTest:{
                    int CurrSenGrpLvl = SingleStep->GetParameter(WatFalPrior::wstParameters::SeniorityGroupLevel).value<IntegerVector>().GetValue(CurrentDate);
                    int CurrSenGrp = SingleStep->GetParameter(WatFalPrior::wstParameters::SeniorityGroup).value<IntegerVector>().GetValue(CurrentDate);
                    int CurrRedGrpLvl = SingleStep->GetParameter(WatFalPrior::wstParameters::RedemptionGroupLevel).value<IntegerVector>().GetValue(CurrentDate);
                    int CurrRedGrp = SingleStep->GetParameter(WatFalPrior::wstParameters::RedemptionGroup).value<IntegerVector>().GetValue(CurrentDate);
                    if (
                        CurrSenGrp > FindMostJuniorLevel(CurrSenGrpLvl)
                        || (
                        !SingleStep->GetParameter(WatFalPrior::wstParameters::RedemptionGroup).value<IntegerVector>().IsEmpty()
                        && CurrRedGrp > FindMostJuniorLevel(CurrRedGrpLvl)
                        )
                        ) {
                        PrintToTempFile("ReturnFalse.txt", "Invalid seniority in OC Test");
                        return false;
                    }
                    ProRataBonds.clear();
                    TotalPayable = Solution = 0.0;

                    for (int h = 0; h<d->m_Tranches.size(); h++) {
                        if (d->m_Tranches.at(h)->GetProrataGroup(CurrSenGrpLvl) <= CurrSenGrp) {
                            TotalPayable += d->m_Tranches.at(h)->GetCashFlow().GetAmountOutstanding(CurrentDate);
                            if (d->m_Tranches.at(h)->GetProrataGroup(CurrSenGrpLvl) == CurrSenGrp)
                                ProRataBonds.enqueue(h);
                        }
                    }
                    //CCC test
                    if (d->m_CCCcurve.GetValue(CurrentDate)>d->m_CCCTestLimit)
                        Solution = (1.0 - ((d->m_CCCcurve.GetValue(CurrentDate) - d->m_CCCTestLimit)*d->m_CCChaircut))*d->m_MortgagesPayments.GetOutstandingForOC(CurrentDate);
                    else
                        Solution = d->m_MortgagesPayments.GetOutstandingForOC(CurrentDate);
                    Solution += AvailablePrincipal.Total() + d->m_ReinvestmentTest.GetQueuedCash(CurrentDate);
                    if (Solution == 0.0) Solution = 1.0;
                    TotalPayable = qMax(TotalPayable, 0.000001);
                    if (SingleStep->GetParameter(WatFalPrior::wstParameters::PayAccrue).toInt() & WatFalPrior::wstAccrueOrPay::Accrue) {
                        for (; ProRataBonds.size() > 0; ProRataBonds.dequeue()) {
                            if (d->m_Tranches[ProRataBonds.head()]->GetCashFlow().GetOCTest(CurrentDate) == 0.0) {
                                d->m_Tranches[ProRataBonds.head()]->SetCashFlow(CurrentDate, Solution / TotalPayable, TrancheCashFlow::TrancheFlowType::OCFlow);
                            }
                        }
                    }
                    if (SingleStep->GetParameter(WatFalPrior::wstParameters::PayAccrue).toInt() & WatFalPrior::wstAccrueOrPay::Pay) {
                        ProRataBonds.clear();
                        TestTarget = -1.0;
                        if (SingleStep->HasParameter(WatFalPrior::wstParameters::TestTargetOverride)) {
                            TestTarget = SingleStep->GetParameter(WatFalPrior::wstParameters::TestTargetOverride).value<BloombergVector>().GetValue(CurrentDate);
                        }
                        for (auto h = d->m_Tranches.begin(); h != d->m_Tranches.end(); ++h) {
                            if ((*h)->GetProrataGroup(CurrSenGrpLvl) == CurrSenGrp) {
                                if (TestTarget <= 0.0) TestTarget = (*h)->GetMinOClevel();
                                double CurrTrg = (*h)->GetCashFlow().GetFlow(CurrentDate, TrancheCashFlow::TrancheFlowType::OCTarget);
                                if (CurrTrg <= 0.0 || CurrTrg > TestTarget)
                                    (*h)->SetCashFlow(CurrentDate, TestTarget, TrancheCashFlow::TrancheFlowType::OCTarget);
                            }
                        }
                        if (TestTarget > 0.0) {
                            //if it fails redeem notes until cured
                            if (Solution / TotalPayable < TestTarget) {
                                double CurrAddColShare = SingleStep->GetParameter(WatFalPrior::wstParameters::AdditionalCollateralShare).value<BloombergVector>().GetValue(CurrentDate);
                                double CurrentRedemptionShare = SingleStep->GetParameter(WatFalPrior::wstParameters::RedemptionShare).value<BloombergVector>().GetValue(CurrentDate);
                                double NeedToCure;
                                if (IsCallPaymentDate || IsMaturityDate) CurrAddColShare = 0.0;
                                if (CurrAddColShare + (CurrentRedemptionShare*TestTarget) == 0.0) {
                                    PrintToTempFile("ReturnFalse.txt", "OC cure denominator is 0");
                                    return false;
                                }
                                NeedToCure = ((TotalPayable*TestTarget) - Solution) / (CurrAddColShare + (CurrentRedemptionShare*TestTarget));
                                Q_ASSERT_X(NeedToCure >= 0.0, "OC Test", "Negative NeedToCure");
                                double FundsToRedemption, FundsToCollateral;
                                if (SingleStep->GetParameter(WatFalPrior::wstParameters::SourceOfFunding).value<IntegerVector>().GetValue(CurrentDate) == 1) {
                                    NeedToCure = qMin(NeedToCure, AvailableInterest);
                                    FundsToRedemption = NeedToCure*CurrentRedemptionShare;
                                    FundsToCollateral = NeedToCure*CurrAddColShare;
                                    Q_ASSERT_X(FundsToRedemption + FundsToCollateral <= AvailableInterest, "OC Test", "Paying Too Much");
                                    AvailableInterest -= FundsToRedemption + FundsToCollateral;
                                }
                                else {
                                    NeedToCure = qMin(NeedToCure, AvailablePrincipal.Total());
                                    FundsToRedemption = NeedToCure*CurrentRedemptionShare;
                                    FundsToCollateral = NeedToCure*CurrAddColShare;
                                    Q_ASSERT_X(FundsToRedemption + FundsToCollateral <= AvailablePrincipal.Total(), "OC Test", "Paying Too Much");
                                    AvailablePrincipal -= FundsToRedemption + FundsToCollateral;
                                }
                                //reinvest
                                if (FundsToCollateral >= 0.01) {
                                    //Fill the reinvestments queue and process the current reinvestments
                                    d->m_ReinvestmentTest.QueueReinvestments(FundsToCollateral, CurrentDate);
                                    const MtgCashFlow& ReinvFlows = d->m_ReinvestmentTest.ProcessQueue(CurrentDate, CurrentPeriodIndex, d->m_LegalFinal);
                                    if (!ReinvFlows.IsEmpty()) {
                                        AvailableInterest += ReinvFlows.GetInterest(CurrentDate);
                                        AvailablePrincipal.AddScheduled(ReinvFlows.GetScheduled(CurrentDate));
                                        AvailablePrincipal.AddPrepay(ReinvFlows.GetPrepay(CurrentDate));
                                        d->m_ReinvestmentTest.RemoveBondFlow(CurrentDate);
                                        d->m_MortgagesPayments.AddFlow(d->m_ReinvestmentTest.GetBondCashFlow());
                                    }
                                }
                                //Redeem
                                if (FundsToRedemption >= 0.01) {
                                    if (!SingleStep->GetParameter(WatFalPrior::wstParameters::RedemptionGroup).value<IntegerVector>().IsEmpty()) {
                                        FundsToRedemption = RedeemNotes(FundsToRedemption, CurrRedGrp, CurrRedGrpLvl, CurrentDate);
                                    }
                                    if (FundsToRedemption >= 0.01) FundsToRedemption = RedeemSequential(FundsToRedemption, CurrentDate, CurrRedGrpLvl);
                                    if (SingleStep->GetParameter(WatFalPrior::wstParameters::SourceOfFunding).value<IntegerVector>().GetValue(CurrentDate) == 1)
                                        AvailableInterest += FundsToRedemption;
                                    else
                                        AvailablePrincipal += FundsToRedemption;
                                }
                            }
                        }
                    }
                }
                break;
                ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                case WatFalPrior::WaterfallStepType::wst_ICTest:{
                    int CurrSenGrpLvl = SingleStep->GetParameter(WatFalPrior::wstParameters::SeniorityGroupLevel).value<IntegerVector>().GetValue(CurrentDate);
                    int CurrSenGrp = SingleStep->GetParameter(WatFalPrior::wstParameters::SeniorityGroup).value<IntegerVector>().GetValue(CurrentDate);
                    int CurrCoupIndx = SingleStep->GetParameter(WatFalPrior::wstParameters::CouponIndex).value<IntegerVector>().GetValue(CurrentDate);
                    int CurrSourceOfFunding = SingleStep->GetParameter(WatFalPrior::wstParameters::SourceOfFunding).value<IntegerVector>().GetValue(CurrentDate);
                    if (CurrSenGrp > FindMostJuniorLevel(CurrSenGrpLvl)) {
                        PrintToTempFile("ReturnFalse.txt", "Invalid seniority in IC Test");
                        return false;
                    }
                    ProRataBonds.clear();
                    TotalPayable = Solution = 0.0;
                    adjSeniorExpenses = deannualiseCoupon(d->m_SeniorExpenses.GetValue(CurrentDate), RollingLastIPD, RollingNextIPD, d->m_DealDayCountConvention.GetValue(CurrentDate));
                    adjSeniorFees = deannualiseCoupon(d->m_SeniorFees.GetValue(CurrentDate), RollingLastIPD, RollingNextIPD, d->m_DealDayCountConvention.GetValue(CurrentDate));
                    Solution =
                        d->m_InterestAvailable
                        + d->m_MortgagesPayments.GetAccruedInterest(CurrentDate)
                        - ((adjSeniorFees + adjSeniorExpenses)*(CurrentAssetSum / static_cast<double>(CurrentAssetCount)))
                        - d->m_SeniorExpensesFixed.GetValue(CurrentDate)*static_cast<double>(d->m_PaymentFrequency.GetValue(CurrentDate)) / 12.0
                        - d->m_SeniorFeesFixed.GetValue(CurrentDate)*static_cast<double>(d->m_PaymentFrequency.GetValue(CurrentDate)) / 12.0
                        ;
                    for (int h = 0; h < d->m_Tranches.size(); h++) {
                        if (d->m_Tranches.at(h)->GetProrataGroup(CurrSenGrpLvl) <= CurrSenGrp) {
                            if (!d->m_Tranches.at(h)->HasCoupon(CurrCoupIndx)) {
                                PrintToTempFile("ReturnFalse.txt", d->m_Tranches.at(h)->GetTrancheName() + " - Coupon not set in tranche");
                                return false;
                            }
                            AdjustedCoupon = deannualiseCoupon((d->m_Tranches.at(h)->GetCoupon(CurrentDate, CurrCoupIndx)), RollingLastIPD, RollingNextIPD, d->m_Tranches.at(h)->GetDayCount(CurrCoupIndx).GetValue(CurrentDate));
                            TotalPayable += AdjustedCoupon*(d->m_Tranches.at(h)->GetCashFlow().GetAmountOutstanding(CurrentDate) + d->m_Tranches.at(h)->GetCashFlow().GetFlow(CurrentDate, TrancheCashFlow::TrancheFlowType::DeferredFlow | CurrCoupIndx));
                            if (d->m_Tranches.at(h)->GetProrataGroup(CurrSenGrpLvl) == CurrSenGrp)
                                ProRataBonds.enqueue(h);
                        }
                    }
                    TotalPayable = qMax(TotalPayable, 0.000001);
                    if (SingleStep->GetParameter(WatFalPrior::wstParameters::PayAccrue).toInt() & WatFalPrior::wstAccrueOrPay::Accrue) {
                        for (; !ProRataBonds.isEmpty(); ProRataBonds.dequeue()) {
                            if (d->m_Tranches[ProRataBonds.head()]->GetCashFlow().GetICTarget(CurrentDate) == 0.0) {
                                d->m_Tranches[ProRataBonds.head()]->SetCashFlow(CurrentDate, qMax(0.0, Solution / TotalPayable), TrancheCashFlow::TrancheFlowType::ICFlow);
                            }
                        }
                    }
                    if (SingleStep->GetParameter(WatFalPrior::wstParameters::PayAccrue).toInt() & WatFalPrior::wstAccrueOrPay::Pay) {
                        ProRataBonds.clear();
                        TestTarget = -1.0;
                        if (SingleStep->HasParameter(WatFalPrior::wstParameters::TestTargetOverride)) {
                            TestTarget = SingleStep->GetParameter(WatFalPrior::wstParameters::TestTargetOverride).value<BloombergVector>().GetValue(CurrentDate);
                        }
                        for (auto h = d->m_Tranches.begin(); h != d->m_Tranches.end(); ++h) {
                            if ((*h)->GetProrataGroup(CurrSenGrpLvl) == CurrSenGrp) {
                                if (TestTarget <= 0.0) TestTarget = (*h)->GetMinIClevel();
                                double CurrTest = (*h)->GetCashFlow().GetFlow(CurrentDate, TrancheCashFlow::TrancheFlowType::ICTarget);
                                if (CurrTest <= 0.0 || CurrTest < TestTarget)
                                    (*h)->SetCashFlow(CurrentDate, TestTarget, TrancheCashFlow::TrancheFlowType::ICTarget);
                            }
                        }
                        if (TotalPayable != 0.0 && TestTarget > 0.0) {
                            if (
                                Solution / TotalPayable < TestTarget //if it fails,
                                && (// and you have funds available 
                                (AvailablePrincipal.Total()>0.0 && CurrSourceOfFunding == 2)
                                || (AvailableInterest && CurrSourceOfFunding == 1)
                                )
                                ) { //redeem senior notes until cured								
                                SolutionDegree = 1;
                                InterestPayableBefore = (d->m_InterestAvailable + d->m_MortgagesPayments.GetAccruedInterest(CurrentDate) - ((adjSeniorExpenses + adjSeniorFees) * (CurrentAssetSum / static_cast<double>(CurrentAssetCount))) - ((d->m_SeniorExpensesFixed.GetValue(CurrentDate) + d->m_SeniorFeesFixed.GetValue(CurrentDate))*static_cast<double>(d->m_PaymentFrequency.GetValue(CurrentDate)) / 12.0)) / TestTarget;
                                TotalPayable = 0.0;
                                bool SolutionFound;
                                do {
                                    SolutionFound = true;
                                    Solution = 0.0;
                                    for (int h = 0; h<d->m_Tranches.size(); h++) {
                                        if (d->m_Tranches.at(h)->GetProrataGroup(CurrSenGrpLvl) <= CurrSenGrp && d->m_Tranches.at(h)->GetProrataGroup(CurrSenGrpLvl) >= SolutionDegree) {
                                            AdjustedCoupon = deannualiseCoupon((d->m_Tranches.at(h)->GetCoupon(CurrentDate, CurrCoupIndx)), RollingLastIPD, RollingNextIPD, d->m_Tranches.at(h)->GetDayCount(CurrCoupIndx).GetValue(CurrentDate));
                                            Solution += AdjustedCoupon*(d->m_Tranches.at(h)->GetCashFlow().GetAmountOutstanding(CurrentDate) + d->m_Tranches.at(h)->GetCashFlow().GetFlow(CurrentDate, TrancheCashFlow::TrancheFlowType::DeferredFlow | CurrCoupIndx));
                                        }
                                    }
                                    if (GroupWACoupon(SolutionDegree, CurrSenGrpLvl, CurrentDate, CurrCoupIndx)>0.0)
                                        TotalPayable = (Solution - InterestPayableBefore) / GroupWACoupon(SolutionDegree, CurrSenGrpLvl, CurrentDate, CurrCoupIndx);
                                    if (TotalPayable > d->GroupOutstanding(SolutionDegree, CurrSenGrpLvl, CurrentDate) || GroupWACoupon(SolutionDegree, CurrSenGrpLvl, CurrentDate, CurrCoupIndx) <= 0) {
                                        if (++SolutionDegree <= CurrSenGrp) SolutionFound = false;
                                        else TotalPayable = 0;
                                    }
                                    if (SolutionFound) {
                                        for (int h = 1; h < SolutionDegree; h++) TotalPayable += d->GroupOutstanding(h, CurrSenGrpLvl, CurrentDate);
                                        if (CurrSourceOfFunding == 2)
                                            AvailablePrincipal += RedeemSequential(qMin(TotalPayable, AvailablePrincipal.Total()), CurrentDate, CurrSenGrpLvl) - qMin(TotalPayable, AvailablePrincipal.Total());
                                        else
                                            AvailableInterest += RedeemSequential(qMin(TotalPayable, AvailableInterest), CurrentDate, CurrSenGrpLvl) - qMin(TotalPayable, AvailableInterest);
                                    }
                                } while (!SolutionFound);
                            }
                        }
                    }
                }
                break;
                ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                case WatFalPrior::WaterfallStepType::wst_ReinvestPrincipal:
                    if (!IsCallPaymentDate && !IsMaturityDate) {
                        if (AvailablePrincipal.Total()>0.0) {
                            double PayablePrincipal;
                            if (SingleStep->GetParameter(WatFalPrior::wstParameters::SourceOfFunding).value<IntegerVector>().GetValue(CurrentDate) == 1) {  //Reinvest Prepay Only
                                PayablePrincipal = AvailablePrincipal.GetPrepay()*SingleStep->GetParameter(WatFalPrior::wstParameters::AdditionalCollateralShare).value<BloombergVector>().GetValue(CurrentDate);
                                AvailablePrincipal.AddPrepay(-PayablePrincipal);
                            }
                            else if (SingleStep->GetParameter(WatFalPrior::wstParameters::SourceOfFunding).value<IntegerVector>().GetValue(CurrentDate) == 2) {  //Reinvest scheduled only
                                PayablePrincipal = AvailablePrincipal.GetScheduled()*SingleStep->GetParameter(WatFalPrior::wstParameters::AdditionalCollateralShare).value<BloombergVector>().GetValue(CurrentDate);
                                AvailablePrincipal.AddScheduled(-PayablePrincipal);
                            }
                            else {
                                PayablePrincipal = AvailablePrincipal.Total()*SingleStep->GetParameter(WatFalPrior::wstParameters::AdditionalCollateralShare).value<BloombergVector>().GetValue(CurrentDate);
                                AvailablePrincipal -= PayablePrincipal;
                            }
                            //Fill the reinvestments queue and process the current reinvestments
                            d->m_ReinvestmentTest.QueueReinvestments(PayablePrincipal, CurrentDate);
                            const MtgCashFlow& ReinvFlows = d->m_ReinvestmentTest.ProcessQueue(CurrentDate, CurrentPeriodIndex, d->m_LegalFinal);
                            if (!ReinvFlows.IsEmpty()) {
                                AvailableInterest += ReinvFlows.GetInterest(CurrentDate);
                                AvailablePrincipal.AddScheduled(ReinvFlows.GetScheduled(CurrentDate));
                                AvailablePrincipal.AddPrepay(ReinvFlows.GetPrepay(CurrentDate));
                                d->m_ReinvestmentTest.RemoveBondFlow(CurrentDate);
                                d->m_MortgagesPayments.AddFlow(d->m_ReinvestmentTest.GetBondCashFlow());
                            }
                        }
                    }
                    break;
                    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                default:
                    PrintToTempFile("ReturnFalse.txt", "Reached Default");
                    return false;
                }//End Switch
            }//End Cycle through the steps of the waterfall

            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //Accrued But Unpaid Becomes Deferred			
            {
                double NewDeferred =
                    d->m_TotalJuniorFees.GetFlow(CurrentDate, TrancheCashFlow::TrancheFlowType::DeferredFlow)
                    + d->m_TotalJuniorFees.GetFlow(CurrentDate, TrancheCashFlow::TrancheFlowType::AccruedFlow)
                    - d->m_TotalJuniorFees.GetTotalFlow(CurrentDate, QList<qint32>() << TrancheCashFlow::TrancheFlowType::InterestFlow << TrancheCashFlow::TrancheFlowType::PrincipalFlow)
                    ;
                if (NewDeferred>0.0) d->m_TotalJuniorFees.SetFlow(CurrentDate, NewDeferred, TrancheCashFlow::TrancheFlowType::DeferredFlow);
                foreach(Tranche* SingleTranche, d->m_Tranches)
                {
                    for (qint32 CoupIdx = 0; CoupIdx<(1 << MaximumInterestsTypes); ++CoupIdx) {
                        if (SingleTranche->HasCoupon(CoupIdx)) {
                            NewDeferred =
                                SingleTranche->GetCashFlow().GetDeferred(CurrentDate, CoupIdx)
                                + SingleTranche->GetCashFlow().GetAccrued(CurrentDate, CoupIdx)
                                - SingleTranche->GetCashFlow().GetInterest(CurrentDate, CoupIdx);
                            if (qAbs(NewDeferred) < 0.01)NewDeferred = 0.0;
                            if (NewDeferred >= 0.0)
                                SingleTranche->SetCashFlow(CurrentDate, NewDeferred, TrancheCashFlow::TrancheFlowType::DeferredFlow | CoupIdx);
                        }
                    }
                }
            }
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //If, after applying the waterfall, there are still funds available, apply them to XS
            if (AvailablePrincipal.Total()>0.0) 
                d->m_ExcessCashFlow.AddFlow(CurrentDate, AvailablePrincipal.Total(), TrancheCashFlow::TrancheFlowType::PrincipalFlow);
            if (AvailableInterest>0.0) 
                d->m_ExcessCashFlow.AddFlow(CurrentDate, AvailableInterest, TrancheCashFlow::TrancheFlowType::InterestFlow);
            AvailablePrincipal.Erase();
            AvailableInterest = 0.0;
            d->m_PrincipalAvailable.Erase();
            d->m_InterestAvailable = 0.0;
            CurrentAssetSum = 0.0;
            CurrentAssetCount = 0;
            RollingLastIPD = RollingNextIPD;
            RollingNextIPD = RollingNextIPD.addMonths(d->m_PaymentFrequency.GetValue(RollingNextIPD));
            if (IsCallPaymentDate) {
                break;
            }
        }//End Cycle in time
        for (int j = 0; j<d->m_Reserves.size(); j++) {
            d->m_Reserves[j]->SetReserveFundCurrent(d->m_Reserves[j]->GetStartingReserve());
            d->m_Reserves[j]->ResetMissingAnchors();
        }
        d->m_StartingDeferredJunFees = OriginalStartingDefJunFees;
        d->m_PrincipalAvailable = OriginalAvailablePrincipal;
        d->m_InterestAvailable = OriginalAvailableInterest;
        d->m_CalculatedMtgPayments = d->m_MortgagesPayments;
        d->m_MortgagesPayments = OriginalMtgFlows;
        FillAllDates();
        if (NullCCCanchor[0]) d->m_CCCcurve.RemoveAnchorDate();
        if (NullCCCanchor[1]) d->m_PaymentFrequency.RemoveAnchorDate();
        if (NullCCCanchor[2]) d->m_SeniorExpenses.RemoveAnchorDate();
        if (NullCCCanchor[3]) d->m_SeniorFees.RemoveAnchorDate();
        if (NullCCCanchor[4]) d->m_JuniorFees.RemoveAnchorDate();
        if (NullCCCanchor[5]) d->m_SeniorExpensesFixed.RemoveAnchorDate();
        if (NullCCCanchor[6]) d->m_SeniorFeesFixed.RemoveAnchorDate();
        if (NullCCCanchor[7]) d->m_JuniorFeesFixed.RemoveAnchorDate();
        if (NullCCCanchor[8]) d->m_GICinterest.RemoveAnchorDate();
        if (NullCCCanchor[9]) d->m_GICBaseRateValue.RemoveAnchorDate();
        foreach(WatFalPrior* SingleStp, d->m_WaterfallStesps)
        {
            SingleStp->ResetMissinAnchors();
        }
        //Check that there is no losses of cash flows
        TrancheCashFlow CheckTranCashFlow;
        MtgCashFlow CheckMtgCashFlow;
        double CheckResults = 0.0;
        LOGDEBUG("Logging Cash Flows Tests:");
        CheckTranCashFlow.Aggregate(GenericCashFlow::TotalAggragate);
        CheckMtgCashFlow.Aggregate(GenericCashFlow::TotalAggragate);
        // Sources of funding
        foreach(ReserveFund* SingleRes, d->m_Reserves)
        {
            CheckResults -= SingleRes->GetStartingReserve();
        }
        LOGDEBUG(QString("After Reserve Funds:\t%1").arg(CheckResults, 0, 'f'));
        CheckMtgCashFlow.Clear();
        if (ActualCallDate.isNull()) {
            CheckMtgCashFlow.AddFlow(d->m_CalculatedMtgPayments);
        }
        else {
            for (int MtgFlwIter = 0; MtgFlwIter <= d->m_CalculatedMtgPayments.Count(); ++MtgFlwIter) {
                CheckMtgCashFlow.AddFlow(d->m_CalculatedMtgPayments.SingleDate(d->m_CalculatedMtgPayments.GetDate(MtgFlwIter)));
                if (d->m_CalculatedMtgPayments.GetDate(MtgFlwIter) >= ActualCallDate) {
                    if (d->m_PoolValueAtCall.IsEmpty())
                        TotalPayable = d->m_CalculatedMtgPayments.GetWAPrice(MtgFlwIter);
                    else
                        TotalPayable = d->m_PoolValueAtCall.GetValue(d->m_CalculatedMtgPayments.GetDate(MtgFlwIter));
                    CheckMtgCashFlow.AddFlow(ActualCallDate, TotalPayable*d->m_CalculatedMtgPayments.GetAmountOut(MtgFlwIter), MtgCashFlow::MtgFlowType::PrincipalFlow);
                    break;
                }
            }
        }
        CheckResults -= CheckMtgCashFlow.GetTotalFlow(0);
        LOGDEBUG(QString("After Loans Flows:\t%1").arg(CheckResults, 0, 'f'));
        CheckTranCashFlow.Clear(); CheckTranCashFlow.AddFlow(d->m_GICflows); CheckResults -= CheckTranCashFlow.GetTotalFlow(0);
        LOGDEBUG(QString("After GIC:\t%1").arg(CheckResults, 0, 'f'));
        CheckResults -= d->m_InterestAvailable;
        LOGDEBUG(QString("After Interest:\t%1").arg(CheckResults, 0, 'f'));
        CheckResults -= d->m_PrincipalAvailable.Total();
        LOGDEBUG(QString("After Principal:\t%1").arg(CheckResults, 0, 'f'));
        // Use of funds
        CheckTranCashFlow.Clear(); CheckTranCashFlow.AddFlow(d->m_ReinvestmentTest.GetReinvested()); CheckResults += CheckTranCashFlow.GetTotalFlow(0);
        LOGDEBUG(QString("After Reinvestments:\t%1").arg(CheckResults, 0, 'f'));
        CheckTranCashFlow.Clear(); CheckTranCashFlow.AddFlow(d->m_TotalSeniorExpenses); CheckResults += CheckTranCashFlow.GetTotalFlow(0);
        LOGDEBUG(QString("After Senior Expenses:\t%1").arg(CheckResults, 0, 'f'));
        CheckTranCashFlow.Clear(); CheckTranCashFlow.AddFlow(d->m_TotalSeniorFees); CheckResults += CheckTranCashFlow.GetTotalFlow(0);
        LOGDEBUG(QString("After Senior Fees:\t%1").arg(CheckResults, 0, 'f'));
        CheckTranCashFlow.Clear(); CheckTranCashFlow.AddFlow(d->m_TotalJuniorFees); CheckResults += CheckTranCashFlow.GetTotalFlow(0);
        LOGDEBUG(QString("After Junior Fees:\t%1").arg(CheckResults, 0, 'f'));
        CheckTranCashFlow.Clear(); CheckTranCashFlow.AddFlow(d->m_ExcessCashFlow); CheckResults += CheckTranCashFlow.GetTotalFlow(0);
        LOGDEBUG(QString("After Excess Spread:\t%1").arg(CheckResults, 0, 'f'));
        foreach(const Tranche* SingleTranche, d->m_Tranches)
        {
            CheckTranCashFlow.Clear(); CheckTranCashFlow.AddFlow(SingleTranche->GetCashFlow());	CheckResults += CheckTranCashFlow.GetTotalFlow(0);
            LOGDEBUG(QString("After Tranche %1:\t%2").arg(SingleTranche->GetTrancheName()).arg(CheckResults, 0, 'f'));
        }
        LOGDEBUG(QString("Final Test\t%1").arg(CheckResults, 0, 'f'));
        if (qAbs(CheckResults) >= 1.0) {
            PrintToTempFile("ReturnFalse.txt", QString("Cash Flows don't Match, Diff: %1; Call %2abled").arg(CheckResults, 0, 'f').arg(d->m_UseCall ? "En" : "Dis"));
#ifndef DebugLogging
            return false;
#endif // !DebugLogging
        }
        return true;
}

QDataStream& operator<<(QDataStream & stream, const Waterfall& flows)
{
    stream
        << flows.d_func()->m_CalledPeriod
        << flows.d_func()->m_TriggersResults
        << flows.d_func()->m_SeniorExpenses
        << flows.d_func()->m_SeniorFees
        << flows.d_func()->m_JuniorFees
        << flows.d_func()->m_SeniorExpensesFixed
        << flows.d_func()->m_SeniorFeesFixed
        << flows.d_func()->m_JuniorFeesFixed
        << flows.d_func()->m_MortgagesPayments
        << flows.d_func()->m_CalculatedMtgPayments
        << flows.d_func()->m_PaymentFrequency
        << flows.d_func()->m_ExcessCashFlow
        << flows.d_func()->m_TotalSeniorExpenses
        << flows.d_func()->m_TotalSeniorFees
        << flows.d_func()->m_TotalJuniorFees
        << flows.d_func()->m_ReinvestmentTest
        << flows.d_func()->m_CCCTestLimit
        << flows.d_func()->m_CCCcurve
        << flows.d_func()->m_CCChaircut
        << flows.d_func()->m_PrincipalAvailable
        << flows.d_func()->m_InterestAvailable
        << flows.d_func()->m_JuniorFeesCoupon
        << flows.d_func()->m_AnnualizedExcess
        << flows.d_func()->m_EquityIncome
        << flows.d_func()->m_FirstIPDdate
        << flows.d_func()->m_LastIPDdate
        << flows.d_func()->m_CallDate
        << flows.d_func()->m_LegalFinal
        << flows.d_func()->m_PoolValueAtCall
        << flows.d_func()->m_UseCall
        << flows.d_func()->m_IsStressTest
        << flows.d_func()->m_CallMultiple
        << flows.d_func()->m_CallReserve
        << flows.d_func()->m_DealName
        << flows.d_func()->m_GICinterest
        << flows.d_func()->m_GICBaseRate
        << flows.d_func()->m_GICBaseRateValue
        << flows.d_func()->m_DealDayCountConvention
        << flows.d_func()->m_StartingDeferredJunFees
        << flows.d_func()->m_GICflows
        << static_cast<qint32>(flows.d_func()->m_WaterfallStesps.size());
    ;
    foreach(const WatFalPrior* SingleStep, flows.d_func()->m_WaterfallStesps)
        stream << (*SingleStep);
    stream << static_cast<qint32>(flows.d_func()->m_Triggers.size() - 1);
    for (auto i = flows.d_func()->m_Triggers.constBegin(); i != flows.d_func()->m_Triggers.constEnd(); ++i) {
        if (i.key() == 0) continue; //Reinvestment period is saved already
        stream << static_cast<quint8>(i.value()->GetTriggerType()) << i.key() << *(i.value());
    }
    stream << static_cast<qint32>(flows.d_func()->m_Tranches.size());
    foreach(const Tranche* SingleTranche, flows.d_func()->m_Tranches)
        stream << (*SingleTranche);
    stream << flows.d_func()->m_CumulativeReserves << static_cast<qint32>(flows.d_func()->m_Reserves.size());
    for (QList<ReserveFund*>::const_iterator ResIter = flows.d_func()->m_Reserves.constBegin(); ResIter != flows.d_func()->m_Reserves.constEnd(); ResIter++)
        stream << (**ResIter);
    return stream;
}
QDataStream& operator>>(QDataStream & stream, Waterfall& flows)
{
    return flows.LoadOldVersion(stream);
}
QDataStream& Waterfall::LoadOldVersion(QDataStream& stream)
{
    Q_D(Waterfall);
    QString TempString;
    qint32 TempInt;
    Tranche TempTranche;
    ReserveFund TempReserve;
    WatFalPrior TempStep;
    stream >> d->m_CalledPeriod;
    d->m_TriggersResults.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->m_TriggersResults;
    d->m_SeniorExpenses.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->m_SeniorExpenses;
    d->m_SeniorFees.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->m_SeniorFees;
    d->m_JuniorFees.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->m_JuniorFees;
    d->m_SeniorExpensesFixed.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->m_SeniorExpensesFixed;
    d->m_SeniorFeesFixed.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->m_SeniorFeesFixed;
    d->m_JuniorFeesFixed.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->m_JuniorFeesFixed;
    d->m_MortgagesPayments.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->m_MortgagesPayments;
    d->m_CalculatedMtgPayments.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->m_CalculatedMtgPayments;
    d->m_PaymentFrequency.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->m_PaymentFrequency;
    d->m_ExcessCashFlow.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->m_ExcessCashFlow;
    d->m_TotalSeniorExpenses.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->m_TotalSeniorExpenses;
    d->m_TotalSeniorFees.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->m_TotalSeniorFees;
    d->m_TotalJuniorFees.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->m_TotalJuniorFees;
    d->m_ReinvestmentTest.SetLoadProtocolVersion(loadProtocolVersion());  stream >> d->m_ReinvestmentTest;
    ResetTriggers();
    SetReinvestementPeriod(d->m_ReinvestmentTest.GetReinvestmentPeriod());
    stream >> d->m_CCCTestLimit;
    d->m_CCCcurve.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->m_CCCcurve;
    stream >> d->m_CCChaircut;
    d->m_PrincipalAvailable.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->m_PrincipalAvailable;
    stream
        >> d->m_InterestAvailable
        >> d->m_JuniorFeesCoupon
        ;
    d->m_AnnualizedExcess.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->m_AnnualizedExcess;
    d->m_EquityIncome.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->m_EquityIncome;
    stream >> d->m_FirstIPDdate;
    stream >> d->m_LastIPDdate >> d->m_CallDate >> d->m_LegalFinal;
    d->m_PoolValueAtCall.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->m_PoolValueAtCall;
    stream >> d->m_UseCall
        >> d->m_IsStressTest
        >> d->m_CallMultiple
        >> d->m_CallReserve
        >> d->m_DealName;
    d->m_GICinterest.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->m_GICinterest;
    d->m_GICBaseRate.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->m_GICBaseRate;
    d->m_GICBaseRateValue.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->m_GICBaseRateValue;
    d->m_DealDayCountConvention.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->m_DealDayCountConvention;
    stream >> d->m_StartingDeferredJunFees;
    d->m_GICflows.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->m_GICflows;
    stream >> TempInt;

    ResetSteps();
    for (int i = 0; i < TempInt; i++) {
        TempStep.ClearParameters();
        TempStep.SetLoadProtocolVersion(loadProtocolVersion());
        stream >> TempStep;
        AddStep(TempStep);
    }
    //Triggers
    {
        quint8 TempChar;
        quint32 TempKey;
        QSharedPointer<AbstractTrigger> TempTrig;
        stream >> TempInt;
        for (int i = 0; i < TempInt; i++) {
            stream >> TempChar >> TempKey;
            switch (static_cast<AbstractTrigger::TriggerType>(TempChar)) {
            case AbstractTrigger::TriggerType::DateTrigger:
                TempTrig.reset(new DateTrigger());
                break;
            case AbstractTrigger::TriggerType::VectorTrigger:
                TempTrig.reset(new VectorTrigger());
                break;
            case AbstractTrigger::TriggerType::PoolSizeTrigger:
                TempTrig.reset(new PoolSizeTrigger());
                break;
            case AbstractTrigger::TriggerType::TrancheTrigger:
                TempTrig.reset(new TrancheTrigger());
                break;
            case AbstractTrigger::TriggerType::DelinquencyTrigger:
                TempTrig.reset(new DelinquencyTrigger());
                break;
            case AbstractTrigger::TriggerType::DuringStressTestTrigger:
                TempTrig.reset(new DuringStressTestTrigger());
                break;
            case AbstractTrigger::TriggerType::CumulativeLossTrigger:
                TempTrig.reset(new CumulativeLossTrigger());
                break;
            case AbstractTrigger::TriggerType::DeferredInterestTrigger:
                TempTrig.reset(new DeferredInterestTrigger());
                break;
            case AbstractTrigger::TriggerType::PDLTrigger:
                TempTrig.reset(new PDLTrigger());
                break;
            default:
                Q_UNREACHABLE(); // Unhadled Trigger Type"
            }
            TempTrig->SetLoadProtocolVersion(loadProtocolVersion());
            stream >> (*TempTrig);
            d->m_Triggers.insert(TempKey, TempTrig);
        }
    }

    stream >> TempInt;
    ResetTranches();
    for (int i = 0; i < TempInt; i++) {
        TempTranche.SetLoadProtocolVersion(loadProtocolVersion());
        stream >> TempTranche;
        AddTranche(TempTranche);
    }
    stream >> d->m_CumulativeReserves;
    stream >> TempInt;
    RemoveReserve();
    for (int i = 0; i < TempInt; i++) {
        TempReserve.SetLoadProtocolVersion(loadProtocolVersion());
        stream >> TempReserve;
        d->m_Reserves.append(new ReserveFund(TempReserve));
    }
    ResetProtocolVersion();
    return stream;
}
const QDate& Waterfall::GetCalledPeriod() const
{
    Q_D(const Waterfall);
    return d->m_CalledPeriod;
}
QString Waterfall::ReadyToCalculate()const
{
    Q_D(const Waterfall);
    QString Result;
    if (d->m_DealDayCountConvention.IsEmpty()) Result += "Deal Day Count Convention\n";
    if (d->m_GICinterest.IsEmpty()) Result += "GIC Interest\n";
    if (d->m_GICBaseRate.IsEmpty()) Result += "GIC Base Rate\n";
    if (d->m_SeniorExpenses.IsEmpty(0.0)) Result += "Senior Expenses\n";
    if (d->m_SeniorFees.IsEmpty(0.0)) Result += "Senior Fees\n";
    if (d->m_JuniorFees.IsEmpty(0.0)) Result += "Junior Fees\n";
    if (d->m_SeniorExpensesFixed.IsEmpty(0.0)) Result += "Senior Expenses Fixed\n";
    if (d->m_SeniorFeesFixed.IsEmpty(0.0)) Result += "Senior Fees Fixed\n";
    if (d->m_JuniorFeesFixed.IsEmpty(0.0)) Result += "Junior Fees Fixed\n";
    if (d->m_Tranches.isEmpty()) Result += "Tranches\n";
    if (d->m_WaterfallStesps.isEmpty()) Result += "Waterfall Steps\n";
    if (d->m_MortgagesPayments.Count() == 0) Result += "Loans Payments\n";
    if (d->m_PaymentFrequency.IsEmpty(1)) Result += "IDP Frequency\n";
    if (BloombergVector(d->m_ReinvestmentTest.GetCDRAssumption()).IsEmpty(0.0, 1.0))Result += "CDR\n";
    if (BloombergVector(d->m_ReinvestmentTest.GetCPRAssumption()).IsEmpty(0.0, 1.0))Result += "CPR\n";
    if (BloombergVector(d->m_ReinvestmentTest.GetLSAssumption()).IsEmpty(0.0, 1.0))Result += "LS\n";
    if (BloombergVector(d->m_ReinvestmentTest.GetWALAssumption()).IsEmpty(0.0))Result += "Reinvestment Bond WAL\n";
    if (d->m_ReinvestmentTest.GetReinvDelay().IsEmpty(0.0))Result += "Reinvestment Delay\n";
    if (BloombergVector(d->m_ReinvestmentTest.GetReinvPrice()).IsEmpty(0.0))Result += "Reinvestment Price\n";
    if (d->m_ReinvestmentTest.GetReinvestmentPeriod() < QDate(2000, 1, 1))Result += "Reinvestment Period\n";
    if (d->m_ReinvestmentTest.GetReinvestmentBond().GetPaymentFreq().IsEmpty(1)) Result += "Reinvestment Bond Payment Frequency\n";
    if (d->m_ReinvestmentTest.GetReinvestmentBond().GetInterest().isEmpty())Result += "Reinvestment Bond Spread\n";
    if (d->m_ReinvestmentTest.GetReinvestmentSpreadOverTime().IsEmpty(1)) Result += "Reinvestment Spread Over Time\n";
    if (d->m_CCCcurve.IsEmpty(0.0, 1.0))Result += "CCC Curve\n";
    if (d->m_CCChaircut < 0.0 || d->m_CCChaircut>1.0)Result += "CCC Value\n";
    if (d->m_PoolValueAtCall.IsEmpty(0.0) && !d->m_PoolValueAtCall.IsEmpty()) Result += "Pool Value at Call\n";
    if (d->m_UseCall && d->m_CallDate < QDate(2000, 1, 1) && (d->m_CallReserve <= 0.0 || d->m_CallMultiple <= 0.0))Result += "Specify a call Criteria to use the Call\n";
    if (d->m_MortgagesPayments.Count() > 0 && d->m_MortgagesPayments.GetDate(0) < QDate(2000, 1, 1))Result += "Pool Cut Off Date\n";
    if (d->m_CCCTestLimit < 0.0 || d->m_CCChaircut>1.0)Result += "CCC Test Limit\n";
    if (d->m_FirstIPDdate < QDate(2000, 1, 1))Result += "Next IDP\n";
    if (d->m_LastIPDdate < QDate(2000, 1, 1))Result += "Last IDP\n";
    for (int ResIter = 0; ResIter < d->m_Reserves.size(); ResIter++) {
        if (d->m_Reserves.at(ResIter)->GetReserveFundTarget().IsEmpty(0.0)) Result += QString("Reserve %1 Target Level\n").arg(ResIter + 1);
        if (d->m_Reserves.at(ResIter)->GetReserveFundMultiple().IsEmpty(0.0)) Result += QString("Reserve %1 Multiple\n").arg(ResIter + 1);
        if (d->m_Reserves.at(ResIter)->GetReserveFundFloor().IsEmpty(0.0))Result += QString("Reserve %1 Floor Level\n").arg(ResIter + 1);
        if (d->m_Reserves.at(ResIter)->GetReserveFundFreed() < 0) Result += QString("Reserve %1 Freed After Redemption\n").arg(ResIter + 1);
        if (d->m_Reserves.at(ResIter)->GetReserveFundCurrent() < 0.0) Result += QString("Reserve %1 Current Amount\n").arg(ResIter + 1);
    }
    foreach(const Tranche* SingleTranche, d->m_Tranches)
    {
        const auto tempCoupons = SingleTranche->GetDayCountsIndexes();
        for (auto singlCoup = tempCoupons.constBegin(); singlCoup != tempCoupons.constEnd(); ++singlCoup) {
            if (SingleTranche->GetDayCount(*singlCoup).IsEmpty()) Result += "Tranche Day Count Convention\n";
        }
        if (IntegerVector(SingleTranche->GetPaymentFrequency()).IsEmpty(1)) Result += "Tranche payment Frequency\n";
        if (SingleTranche->GetOriginalAmount() < 0.0) Result += "Tranche Original Amount\n";
        if (SingleTranche->GetOutstandingAmt() < 0.0) Result += "Tranche Amount Outstanding\n";
        if (SingleTranche->GetPrice() < 0.0) Result += "Tranche Price\n";
        if (!SingleTranche->GetProrataGroup().isValid()) Result += "Tranche Seniority Structure\n";
    }
    foreach(const WatFalPrior* SingleStep, d->m_WaterfallStesps)
    {
        Result += SingleStep->ReadyToCalculate();
        if (SingleStep->HasParameter(WatFalPrior::wstParameters::Trigger)) {
            if (!ValidTriggerStructure(SingleStep->GetParameter(WatFalPrior::wstParameters::Trigger).toString())) {
                Result += QString("%1 - Trigger Not Valid\n").arg(static_cast<quint32>(SingleStep->GetPriorityType()));
            }
        }
    }
    foreach(const QSharedPointer<AbstractTrigger>& SingleTrig, d->m_Triggers)
    {
        Result += SingleTrig->ReadyToCalculate();
    }
    if (!Result.isEmpty()) return Result.left(Result.size() - 1);
    return Result;
}
double Waterfall::GetEquityReturn(int index)const
{
    Q_D(const Waterfall);
    IntegerVector AdjPaymentFreq(d->m_PaymentFrequency);
    if (d->m_PaymentFrequency.GetAnchorDate().isNull()) AdjPaymentFreq.SetAnchorDate(d->m_MortgagesPayments.GetDate(0));
    if (index<0 || index >= d->m_EquityIncome.Count()) return 0.0;
    int EquityTranche=0.0;
    foreach(WatFalPrior* SingleStep, d->m_WaterfallStesps)
    {
        if (SingleStep->GetPriorityType() == WatFalPrior::WaterfallStepType::wst_Excess) {
            EquityTranche = SingleStep->GetParameter(WatFalPrior::wstParameters::RedemptionGroup).value<IntegerVector>().GetValue(d->m_EquityIncome.GetDate(index));
            break;
        }
    }
    double denominator = 0.0;
    if (EquityTranche>0) {
        foreach(Tranche* SingleTranche, d->m_Tranches)
        {
            if (SingleTranche->GetProrataGroup(0) == EquityTranche) denominator += SingleTranche->GetOriginalAmount();
        }
        if (denominator>0) return qPow(1.0 + ((d->m_EquityIncome.GetFlow(index, TrancheCashFlow::TrancheFlowType::InterestFlow) + d->m_EquityIncome.GetFlow(index, TrancheCashFlow::TrancheFlowType::PrincipalFlow)) / denominator), 12.0 / ((AdjPaymentFreq.GetValue(d->m_EquityIncome.GetDate(index))))) - 1.0;
        else return 0.0;
    }
    denominator = 0.0;
    foreach(Tranche* SingleTranche, d->m_Tranches)
    {
        denominator += SingleTranche->GetOriginalAmount();
    }
    denominator = d->m_CalculatedMtgPayments.GetAmountOut(0) - denominator;
    if (denominator>0) return qPow(1.0 + ((d->m_EquityIncome.GetFlow(index, TrancheCashFlow::TrancheFlowType::InterestFlow) + d->m_EquityIncome.GetFlow(index, TrancheCashFlow::TrancheFlowType::PrincipalFlow)) / denominator), 12.0 / ((AdjPaymentFreq.GetValue(d->m_EquityIncome.GetDate(index))))) - 1.0;
    else return 0.0;

}
double Waterfall::GetCumulativeEquityReturn(int index)const
{
    Q_D(const Waterfall);
    if (index<0 || index >= d->m_EquityIncome.Count()) return 0.0;
    double numerator = 0.0;
    for (int i = 0; i <= index; i++) numerator += (d->m_EquityIncome.GetFlow(i, TrancheCashFlow::TrancheFlowType::InterestFlow) + d->m_EquityIncome.GetFlow(i, TrancheCashFlow::TrancheFlowType::PrincipalFlow));
    int EquityTranche=0.0;
    foreach(WatFalPrior* SingleStep, d->m_WaterfallStesps)
    {
        if (SingleStep->GetPriorityType() == WatFalPrior::WaterfallStepType::wst_Excess) {
            EquityTranche = SingleStep->GetParameter(WatFalPrior::wstParameters::RedemptionGroup).value<IntegerVector>().GetValue(d->m_EquityIncome.GetDate(index));
            break;
        }
    }
    double denominator = 0.0;
    if (EquityTranche>0) {
        foreach(Tranche* SingleTranche, d->m_Tranches)
        {
            if (SingleTranche->GetProrataGroup(0) == EquityTranche) denominator += SingleTranche->GetOriginalAmount();
        }
        if (denominator>0) return numerator / denominator;
        else return 0.0;
    }
    denominator = 0.0;
    foreach(Tranche* SingleTranche, d->m_Tranches)
    {
        denominator += SingleTranche->GetOriginalAmount();
    }
    denominator = d->m_CalculatedMtgPayments.GetAmountOut(0) - denominator;
    if (denominator>0) return numerator / denominator;
    else return 0.0;
}
double Waterfall::GetCallEquityRatio(int index)const
{
    Q_D(const Waterfall);
    if (index<0 || index >= d->m_Tranches.first()->GetCashFlow().Count()) return 0.0;
    QDate TargetDate = d->m_Tranches.first()->GetCashFlow().GetDate(index);
    int MtgIndex = -1;
    for (int i = 0; i<d->m_CalculatedMtgPayments.Count(); i++) {
        if (TargetDate.year() == d->m_CalculatedMtgPayments.GetDate(i).year() && TargetDate.month() == d->m_CalculatedMtgPayments.GetDate(i).month()) {
            MtgIndex = i;
            break;
        }
    }
    if (MtgIndex<0) return 0.0;
    int EquityTranche=0.0;
    foreach(WatFalPrior* SingleStep, d->m_WaterfallStesps)
    {
        if (SingleStep->GetPriorityType() == WatFalPrior::WaterfallStepType::wst_Excess) {
            EquityTranche = SingleStep->GetParameter(WatFalPrior::wstParameters::RedemptionGroup).value<IntegerVector>().GetValue(d->m_CalculatedMtgPayments.GetDate(MtgIndex));
            break;
        }
    }
    double denominator = 0.0;
    double numerator;
    if (d->m_PoolValueAtCall.IsEmpty())
        numerator = d->m_CalculatedMtgPayments.GetWAPrice(MtgIndex);
    else
        numerator = d->m_PoolValueAtCall.GetValue(d->m_CalculatedMtgPayments.GetDate(MtgIndex));
    numerator *= d->m_CalculatedMtgPayments.GetAmountOut(MtgIndex);
    if (EquityTranche>0) {
        foreach(Tranche* SingleTranche, d->m_Tranches)
        {
            if (SingleTranche->GetProrataGroup(0) == EquityTranche) denominator += SingleTranche->GetCashFlow().GetAmountOutstanding(index);
            else numerator -= SingleTranche->GetCashFlow().GetAmountOutstanding(index);
        }
        if (denominator>0) return numerator / denominator;
        else return 0.0;
    }
    denominator = 0.0;
    if (d->m_PoolValueAtCall.IsEmpty())
        numerator = d->m_CalculatedMtgPayments.GetWAPrice(MtgIndex);
    else
        numerator = d->m_PoolValueAtCall.GetValue(d->m_CalculatedMtgPayments.GetDate(MtgIndex));
    numerator *= d->m_CalculatedMtgPayments.GetAmountOut(MtgIndex);
    foreach(Tranche* SingleTranche, d->m_Tranches)
    {
        denominator += SingleTranche->GetCashFlow().GetAmountOutstanding(index);
        numerator -= SingleTranche->GetCashFlow().GetAmountOutstanding(index);
    }
    denominator = d->m_CalculatedMtgPayments.GetAmountOut(index) - denominator;
    if (denominator>0) return numerator / denominator;
    else return 0.0;
}

const QDate& Waterfall::GetFirstIPDdate() const
{
    Q_D(const Waterfall);
    return d->m_FirstIPDdate;
}

const GenericCashFlow& Waterfall::GetReinvested() const
{
    Q_D(const Waterfall);
    return d->m_ReinvestmentTest.GetReinvested();
}

const QDate& Waterfall::GetLastIPDdate() const
{
    Q_D(const Waterfall);
    return d->m_LastIPDdate;
}

const QDate& Waterfall::GetCallDate() const
{
    Q_D(const Waterfall);
    return d->m_CallDate;
}

void Waterfall::SetReserveFund(int RFindex, const QString& RFtarget, const QString& RFmultiple, const QString& RFfloor, const QString& RFcap, double RFcurrent, int RFfreed, bool RFtoInterest)
{
    Q_D( Waterfall);
    if (RFindex<0 || RFindex >= d->m_Reserves.size()) return;
    d->m_Reserves[RFindex]->SetReserveFund(RFtarget, RFmultiple, RFfloor, RFcap, RFcurrent, RFfreed, RFtoInterest);
}
void Waterfall::SetReserveFund(int RFindex, const ReserveFund& source)
{
    Q_D( Waterfall);
    if (RFindex < 0 || RFindex >= d->m_Reserves.size()) return;
    d->m_Reserves[RFindex]->operator=(source);
}
void Waterfall::AddReserveFund(const QString& RFtarget, const QString& RFmultiple, const QString& RFfloor, const QString& RFcap, double RFcurrent, int RFfreed, bool RFtoInterest)
{
    Q_D( Waterfall);
    d->m_Reserves.append(new ReserveFund());
    d->m_Reserves.last()->SetReserveFund(RFtarget, RFmultiple, RFfloor, RFcap, RFcurrent, RFfreed, RFtoInterest);
}
void Waterfall::AddReserveFund(const ReserveFund& source)
{
    Q_D( Waterfall);
    d->m_Reserves.append(new ReserveFund(source));
}
void Waterfall::ResetReserve(int RFindex)
{
    Q_D( Waterfall);
    if (RFindex == -1) {
        for (int i = 0; i<d->m_Reserves.size(); i++)
            d->m_Reserves[i]->ResetReserve();
        return;
    }
    if (RFindex<0 || RFindex >= d->m_Reserves.size()) return;
    d->m_Reserves[RFindex]->ResetReserve();
}
void Waterfall::RemoveReserve(int RFindex)
{
    Q_D( Waterfall);
    if (RFindex == -1) {
        for (int i = 0; i < d->m_Reserves.size(); i++)
            delete d->m_Reserves[i];
        d->m_Reserves.clear();
        return;
    }
    if (RFindex < 0 || RFindex >= d->m_Reserves.size()) return;
    delete d->m_Reserves[RFindex];
    d->m_Reserves.removeAt(RFindex);
}
void Waterfall::SetPaymentFrequency(const QString& a)
{
    Q_D( Waterfall);
    IntegerVector TempVect(a);
    for (int i = 0; i < TempVect.NumElements(); i++) {
        if (TempVect.GetValue(i) <= 0) return;
    }
    d->m_PaymentFrequency = TempVect;
}

void Waterfall::SetCCCTestLimit(double a)
{
    Q_D( Waterfall);
    if (a >= 0.0 && a <= 1.0) d->m_CCCTestLimit = a;
}

void Waterfall::SetCCChaircut(double a)
{
    Q_D( Waterfall);
    if (a >= 0.0 && a <= 1.0) 
        d->m_CCChaircut = a;
}

void Waterfall::SetSchedPrincAvailable(double a)
{
    Q_D( Waterfall);
    d->m_PrincipalAvailable.SetScheduled(a);
}

void Waterfall::SetPrepPrincAvailable(double a)
{
    Q_D( Waterfall);
    d->m_PrincipalAvailable.SetPrepay(a);
}

void Waterfall::SetInterestAvailable(double a)
{
    Q_D( Waterfall);
    d->m_InterestAvailable = a;
}

void Waterfall::SetJuniorFeesCoupon(double a)
{
    Q_D( Waterfall);
    if (a >= 0.0) 
        d->m_JuniorFeesCoupon = a;
}

void Waterfall::SetPoolValueAtCall(const QString& a)
{
    Q_D( Waterfall);
    d->m_PoolValueAtCall = a;
}

void Waterfall::SetUseCall(bool a)
{
    Q_D(Waterfall);
    d->m_UseCall = a;
}

void Waterfall::SetCallMultiple(double a)
{
    Q_D(Waterfall);
    if (a >= 0.0)
        d->m_CallMultiple = a;
}

void Waterfall::SetCallReserve(double a)
{
    Q_D(Waterfall);
    if (a >= 0.0) 
        d->m_CallReserve = a;
}

void Waterfall::AddMortgagesFlows(const MtgCashFlow& a)
{
    Q_D(Waterfall);
    d->m_MortgagesPayments += a;
}

void Waterfall::ResetMtgFlows()
{
    Q_D(Waterfall);
    d->m_MortgagesPayments.Clear();
}

void Waterfall::CompileReferenceRateValue(ForwardBaseRateTable& Values)
{
    Q_D(Waterfall);
    d->m_GICBaseRateValue = d->m_GICBaseRate.CompileReferenceRateValue(Values);
    for (auto i = d->m_Tranches.begin(); i != d->m_Tranches.end(); ++i)
        (*i)->CompileReferenceRateValue(Values);
    d->m_ReinvestmentTest.CompileReferenceRateValue(Values);
}
void Waterfall::CompileReferenceRateValue(ConstantBaseRateTable& Values)
{
    Q_D(Waterfall);
    d->m_GICBaseRateValue = d->m_GICBaseRate.CompileReferenceRateValue(Values);
    for (auto i = d->m_Tranches.begin(); i != d->m_Tranches.end(); ++i)
        (*i)->CompileReferenceRateValue(Values);
    d->m_ReinvestmentTest.CompileReferenceRateValue(Values);
}

void Waterfall::GetBaseRatesDatabase(ConstantBaseRateTable& Values, bool DownloadAll)
{
    Q_D(Waterfall);
    d->m_GICBaseRateValue = d->m_GICBaseRate.GetBaseRatesDatabase(Values, DownloadAll);
    for (auto i = d->m_Tranches.begin(); i != d->m_Tranches.end(); ++i)
        (*i)->GetBaseRatesDatabase(Values, DownloadAll);
    d->m_ReinvestmentTest.GetBaseRatesDatabase(Values, DownloadAll);
}
void Waterfall::GetBaseRatesDatabase(ForwardBaseRateTable& Values, bool DownloadAll)
{
    Q_D(Waterfall);
    d->m_GICBaseRateValue = d->m_GICBaseRate.GetBaseRatesDatabase(Values, DownloadAll);
    for (auto i = d->m_Tranches.begin(); i != d->m_Tranches.end(); ++i)
        (*i)->GetBaseRatesDatabase(Values, DownloadAll);
    d->m_ReinvestmentTest.GetBaseRatesDatabase(Values, DownloadAll);
}


void Waterfall::SetTrigger(quint32 key, QSharedPointer<AbstractTrigger> val)
{
    Q_D(Waterfall);
    d->m_Triggers[key] = val;
}

bool Waterfall::ValidTriggerStructure(const QString& TriggerStructure)const
{
    Q_D(const Waterfall);
    QString AdjStructure = InfixToPostfix(TriggerStructure);
    if (AdjStructure.isEmpty()) {
        LOGDEBUG("Failed in Postfix");
        return false;
    }
    const QRegularExpression SignleTriggers("(\\d+|T|F)", QRegularExpression::CaseInsensitiveOption);
    Q_ASSERT(SignleTriggers.isValid());
    QStringList CapturedTriggers;
    for (auto i = SignleTriggers.globalMatch(AdjStructure); i.hasNext();){
        CapturedTriggers.append(i.next().captured(1));
    }
    if (CapturedTriggers.isEmpty()) return false;
    for (auto i = CapturedTriggers.constBegin(); i != CapturedTriggers.constEnd(); ++i) {
        if (i->compare("F", Qt::CaseInsensitive) == 0 || i->compare("T", Qt::CaseInsensitive) == 0) continue;
        if (!d->m_Triggers.contains(i->toUInt())) {
            LOGDEBUG("Failed Finding Trigger");
            return false;
        }
    }
    return true;
}

bool Waterfall::TriggerPassing(const QString& TriggerStructure, int PeriodIndex, const QDate& CurrentIPD, bool IsCallDate)
{
    Q_D( Waterfall);
    QString AdjStructure = InfixToPostfix(TriggerStructure);
    if (AdjStructure.isEmpty()) return false;
    const QRegularExpression SignleTriggers("(\\d+)");
    QStringList CapturedTriggers;
    for (auto i = SignleTriggers.globalMatch(AdjStructure); i.hasNext();) {
        CapturedTriggers.append(i.next().captured(1));
    }
    //if (CapturedTriggers.isEmpty()) return false;
    for (auto i = CapturedTriggers.constBegin(); i != CapturedTriggers.constEnd(); ++i) {
        quint32 CurrentTrigger = i->toUInt();
        if (!d->m_Triggers.contains(CurrentTrigger)) 	
            return false;
        TriggersResults::TrigRes SingleRes = d->m_TriggersResults.GetResult(CurrentTrigger, CurrentIPD);
        if (SingleRes == TriggersResults::TrigRes::trNA) {
            bool TempRes = EvaluateTrigger(CurrentTrigger, PeriodIndex, CurrentIPD, IsCallDate);
            d->m_TriggersResults.SetResult(CurrentTrigger, CurrentIPD, TempRes);
            SingleRes = (TempRes ? TriggersResults::TrigRes::trTrue : TriggersResults::TrigRes::trFalse);
        }
    }
    QStack<bool> PolishStack;
    QStringList PolishParts = AdjStructure.split(' ');
    foreach(const QString& SinglePart, PolishParts)
    {
        if (SinglePart.compare("F", Qt::CaseInsensitive) == 0) 
            PolishStack.push(false);
        else if (SinglePart.compare("T", Qt::CaseInsensitive) == 0)
            PolishStack.push(true);
        else if (SignleTriggers.match(SinglePart).hasMatch())
            PolishStack.push(d->m_TriggersResults.GetResult(SinglePart.toUInt(), CurrentIPD) == TriggersResults::TrigRes::trTrue);
        else {
            bool Pusher;
            switch (SinglePart.at(0).toLatin1()) {
            case '+':
                Pusher = PolishStack.pop();
                Pusher = PolishStack.pop() || Pusher;
                PolishStack.push(Pusher);
                break;
            case '*':
                Pusher = PolishStack.pop();
                Pusher = PolishStack.pop() && Pusher;
                PolishStack.push(Pusher);
                break;
            case '-':
                Pusher = PolishStack.pop();
                Pusher = PolishStack.pop() || Pusher;
                PolishStack.push(!Pusher);
                break;
            case '/':
                Pusher = PolishStack.pop();
                Pusher = PolishStack.pop() && Pusher;
                PolishStack.push(!Pusher);
                break;
            case '!': PolishStack.push(!PolishStack.pop()); break;
            default: return false;
            }
        }
    }
    Q_ASSERT_X(PolishStack.size() == 1, "TriggerPassing", "Polish Notation stack has more than one element when finished");
    return PolishStack.pop();
}

bool Waterfall::EvaluateTrigger(quint32 TrigID, int PeriodIndex, const QDate& CurrentIPD, bool /*IsCallDate*/) const
{
    Q_D(const Waterfall);
    const QSharedPointer<AbstractTrigger> CurrentTrigger = d->m_Triggers.value(TrigID, QSharedPointer<AbstractTrigger>());
    if (!CurrentTrigger) return false;
    if (PeriodIndex < 0 || PeriodIndex >= d->m_MortgagesPayments.Count()) return false;
    switch (CurrentTrigger->GetTriggerType()) {
    case AbstractTrigger::TriggerType::DateTrigger:
        return CurrentTrigger.dynamicCast<DateTrigger>()->Passing(CurrentIPD);
    case AbstractTrigger::TriggerType::VectorTrigger:{
        VectorTrigger TempTrig(*CurrentTrigger.dynamicCast<VectorTrigger>());
        if (!TempTrig.HasAnchor())
            TempTrig.SetAnchor(d->m_MortgagesPayments.GetDate(0));
        return TempTrig.Passing(CurrentIPD);
    }
    case AbstractTrigger::TriggerType::PoolSizeTrigger:{
        PoolSizeTrigger TempTrig(*CurrentTrigger.dynamicCast<PoolSizeTrigger>());
        if (!TempTrig.HasAnchor())
            TempTrig.SetAnchorDate(d->m_MortgagesPayments.GetDate(0));
        return TempTrig.Passing(d->m_MortgagesPayments.GetAmountOut(PeriodIndex), d->m_MortgagesPayments.GetDate(PeriodIndex));
    }
    case AbstractTrigger::TriggerType::TrancheTrigger:{
        TrancheTrigger TempTrig(*CurrentTrigger.dynamicCast<TrancheTrigger>());
        if (!TempTrig.HasAnchor())
            TempTrig.FillMissingAnchorDate(d->m_MortgagesPayments.GetDate(0));
        return TempTrig.Passing(d->m_Tranches, CurrentIPD);
    }
    case AbstractTrigger::TriggerType::DelinquencyTrigger:{
        DelinquencyTrigger TempTrig(*CurrentTrigger.dynamicCast<DelinquencyTrigger>());
        if (!TempTrig.HasAnchor())
            TempTrig.SetAnchorDate(d->m_MortgagesPayments.GetDate(0));
        return TempTrig.Passing(d->m_MortgagesPayments.GetDelinquentShare(PeriodIndex), d->m_MortgagesPayments.GetDate(PeriodIndex));
    }
    case AbstractTrigger::TriggerType::DuringStressTestTrigger:{
        return CurrentTrigger.dynamicCast<DuringStressTestTrigger>()->Passing(d->m_IsStressTest);
    }
    case AbstractTrigger::TriggerType::CumulativeLossTrigger:{
        double TotalLoss = 0.0;
        for (int CumIter = 0; CumIter <= PeriodIndex; ++CumIter) {
            TotalLoss += d->m_MortgagesPayments.GetLoss(CumIter);
        }
        CumulativeLossTrigger TempTrig(*CurrentTrigger.dynamicCast<CumulativeLossTrigger>());
        if (!TempTrig.HasAnchor())
            TempTrig.SetAnchorDate(d->m_MortgagesPayments.GetDate(0));
        return TempTrig.Passing(TotalLoss, d->m_MortgagesPayments.GetDate(PeriodIndex));
    }
    case AbstractTrigger::TriggerType::DeferredInterestTrigger:{
        DeferredInterestTrigger TempTrig(*CurrentTrigger.dynamicCast<DeferredInterestTrigger>());
        if (!TempTrig.HasAnchor())
            TempTrig.FillMissingAnchorDate(d->m_MortgagesPayments.GetDate(0));
        return TempTrig.Passing(d->m_Tranches, CurrentIPD);
    }
    case AbstractTrigger::TriggerType::PDLTrigger:{
        PDLTrigger TempTrig(*CurrentTrigger.dynamicCast<PDLTrigger>());
        if (!TempTrig.HasAnchor())
            TempTrig.FillMissingAnchorDate(d->m_MortgagesPayments.GetDate(0));
        return TempTrig.Passing(d->m_Tranches, d->m_MortgagesPayments.GetAmountOut(PeriodIndex) + d->m_InterestAvailable + d->m_PrincipalAvailable.Total(), CurrentIPD);
    }
    default:
        Q_ASSERT_X(false, "Waterfall::EvaluateTrigger", "Unhadled Trigger Type");
        return false;
    }
}

void Waterfall::ResetTriggers()
{
    Q_D( Waterfall);
    d->m_Triggers.clear();
    if (!d->m_ReinvestmentTest.GetReinvestmentPeriod().isNull()) {
        QSharedPointer<AbstractTrigger> TempReinvTrigger(new DateTrigger(d->m_ReinvestmentTest.GetReinvestmentPeriod(), DateTrigger::TriggerSide::BeforeIncluding, "Reinvestment Period"));
        SetTrigger(0, TempReinvTrigger);
    }
}

void Waterfall::RemoveTrigger(quint32 key)
{
    Q_D( Waterfall);
    d->m_Triggers.remove(key);
}

void Waterfall::SetDealDayCountConvention(const QString& val)
{
    Q_D( Waterfall);
    d->m_DealDayCountConvention = val;
}

void Waterfall::SetGICBaseRate(const QString& a)
{
    Q_D( Waterfall);
    d->m_GICBaseRate = a;
}

void Waterfall::SetStartingDeferredJunFees(const double& val)
{
    Q_D( Waterfall);
    d->m_StartingDeferredJunFees = val;
}

void Waterfall::SetDealName(const QString& a)
{
    Q_D( Waterfall);
    d->m_DealName = a;
}

void Waterfall::SetCumulativeReserves(bool a)
{
    Q_D( Waterfall);
    d->m_CumulativeReserves = a;
}

void Waterfall::SetGICinterest(const QString& a)
{
    Q_D( Waterfall);
    d->m_GICinterest = a;
}

void Waterfall::SetSeniorExpenses(const QString& a)
{
    Q_D( Waterfall);
    d->m_SeniorExpenses = a;
}

void Waterfall::SetSeniorFees(const QString& a)
{
    Q_D( Waterfall);
    d->m_SeniorFees = a;
}

void Waterfall::SetJuniorFees(const QString& a)
{
    Q_D( Waterfall);
    d->m_JuniorFees = a;
}

void Waterfall::SetSeniorExpensesFixed(const QString& a)
{
    Q_D(Waterfall);
    d->m_SeniorExpensesFixed = a;
}

void Waterfall::SetSeniorFeesFixed(const QString& a)
{
    Q_D(Waterfall);
    d->m_SeniorFeesFixed = a;
}

void Waterfall::SetJuniorFeesFixed(const QString& a)
{
    Q_D(Waterfall);
    d->m_JuniorFeesFixed = a;
}

const QSharedPointer<AbstractTrigger> Waterfall::GetTrigger(quint32 key) const
{
    Q_D(const Waterfall);
    return d->m_Triggers.value(key, QSharedPointer<AbstractTrigger>());
}

QSharedPointer<AbstractTrigger> Waterfall::GetTrigger(quint32 key)
{
    Q_D(Waterfall);
    return d->m_Triggers.value(key, QSharedPointer<AbstractTrigger>());
}

GenericCashFlow Waterfall::GetAggregatedReinvestment() const
{
    Q_D(const Waterfall);
    GenericCashFlow Result;
    Result.AddFlow(d->m_ReinvestmentTest.GetReinvested().AggregateRange(d->m_ReinvestmentTest.GetReinvested().GetDate(0), d->m_FirstIPDdate));
    const TrancheCashFlow& benchFlow = d->m_Tranches.first()->GetCashFlow();
    for (int i = 1; i < benchFlow.Count(); ++i) {
        Result.AddFlow(d->m_ReinvestmentTest.GetReinvested().AggregateRange(benchFlow.GetDate(i - 1).addDays(1), benchFlow.GetDate(i)));
    }
    return Result;
}

GenericCashFlow Waterfall::GetAggregatedGIC() const
{
    Q_D(const Waterfall);
    GenericCashFlow Result;
    Result.AddFlow(d->m_GICflows.AggregateRange(d->m_GICflows.GetDate(0), d->m_FirstIPDdate));
    const TrancheCashFlow& benchFlow = d->m_Tranches.first()->GetCashFlow();
    for (int i = 1; i < benchFlow.Count(); ++i) {
        Result.AddFlow(d->m_GICflows.AggregateRange(benchFlow.GetDate(i - 1).addDays(1), benchFlow.GetDate(i)));
    }
    return Result;
}

MtgCashFlow Waterfall::GetAggregatedMtgFlows() const
{
    Q_D(const Waterfall);
    MtgCashFlow Result;
    Result.AddFlow(d->m_CalculatedMtgPayments.AggregateRange(d->m_CalculatedMtgPayments.GetDate(0), d->m_FirstIPDdate));
    const TrancheCashFlow& benchFlow = d->m_Tranches.first()->GetCashFlow();
    for (int i = 1; i < benchFlow.Count(); ++i) {
        Result.AddFlow(d->m_CalculatedMtgPayments.AggregateRange(benchFlow.GetDate(i - 1).addDays(1), benchFlow.GetDate(i)));
    }
    return Result;
}

const bool& Waterfall::GetIsStressTest() const
{
    Q_D(const Waterfall);
    return d->m_IsStressTest;
}

void Waterfall::SetIsStressTest(const bool& val)
{
    Q_D( Waterfall);
    d->m_IsStressTest = val;
}

void Waterfall::SetAssumptions(const AssumptionSet& a)
{
    Q_D( Waterfall);
    d->m_ReinvestmentTest.SetCPR(a.GetCPRscenario());
    d->m_ReinvestmentTest.SetCDR(a.GetCDRscenario());
    d->m_ReinvestmentTest.SetLS(a.GetLSscenario());
    d->m_ReinvestmentTest.SetRecoveryLag(a.GetRecLagScenario());
    d->m_ReinvestmentTest.SetDelinquency(a.GetDelinqScenario());
    d->m_ReinvestmentTest.SetDelinquencyLag(a.GetDelinqLagScenario());
}

const TriggersResults& Waterfall::GetTriggersResults() const
{
    Q_D(const Waterfall);
    return d->m_TriggersResults;
}
