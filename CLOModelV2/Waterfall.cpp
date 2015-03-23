#include <algorithm>
#include <QQueue>
#include <QHash>
#include <QSet>
#include <qmath.h>
#include "Waterfall.h"
#include "QBbgWorker.h"
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
const WatFalPrior* Waterfall::GetStep(int Index)const
{
    if (Index<0 || Index >= m_WaterfallStesps.size()) return nullptr;
    return m_WaterfallStesps.at(Index);
}
WatFalPrior* Waterfall::GetStep(int Index)
{
    if (Index<0 || Index >= m_WaterfallStesps.size()) return nullptr;
    return m_WaterfallStesps[Index];
}
void Waterfall::SetCCCcurve(const QString& a) { m_CCCcurve = a; }

const Tranche* Waterfall::GetTranche(int Index) const
{
    if (Index<0 || Index >= m_Tranches.size()) return nullptr;
    return m_Tranches.at(Index);
}
Tranche* Waterfall::GetTranche(int Index)
{
    if (Index<0 || Index >= m_Tranches.size()) return nullptr;
    return m_Tranches[Index];
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
    std::sort(m_Tranches.begin(), m_Tranches.end(), [](const Tranche* a, const Tranche* b) -> bool { return a->GetProrataGroup(0) < b->GetProrataGroup(0); });
}
QDate Waterfall::GetStructureMaturity()const
{
    if (m_Tranches.isEmpty()) return QDate();
    if (m_Tranches.first()->GetCashFlow().Count() == 0) return QDate();
    return m_Tranches.first()->GetCashFlow().GetDate(m_Tranches.first()->GetCashFlow().Count() - 1);
}
double Waterfall::GetAnnualizedExcess(int index, bool AsShareOfLoans)const
{
    if (index<0 || index >= m_AnnualizedExcess.Count()) return 0.0;
    IntegerVector AdjPaymentFreq(m_PaymentFrequency);
    if (m_PaymentFrequency.GetAnchorDate().isNull()) AdjPaymentFreq.SetAnchorDate(m_MortgagesPayments.GetDate(0));
    if (AsShareOfLoans) {
        if (m_CalculatedMtgPayments.GetAmountOut(m_AnnualizedExcess.GetDate(index))<0.01) return -1.0;
        return qPow((1.0 + (m_AnnualizedExcess.GetFlow(index, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow)) /
            m_CalculatedMtgPayments.GetAmountOut(m_AnnualizedExcess.GetDate(index)))), 12.0 / (AdjPaymentFreq.GetValue(m_AnnualizedExcess.GetDate(index)))) - 1.0;
    }
    double RunningSum = 0.0;
    for (int i = 0; i<m_Tranches.size(); i++) {
        RunningSum += m_Tranches.at(i)->GetCashFlow().GetAmountOutstanding(index);
    }
    if (RunningSum<0.01) return -1.0;
    return qPow(1.0 + (m_AnnualizedExcess.GetFlow(index, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow)) / RunningSum), 12.0 / (AdjPaymentFreq.GetValue(m_AnnualizedExcess.GetDate(index)))) - 1.0;
}
Waterfall::Waterfall()
    :m_SeniorExpenses("0")
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

    m_TotalJuniorFees.SetStock(static_cast<qint32>(TrancheCashFlow::TrancheFlowType::DeferredFlow));
}
Waterfall::Waterfall(const Waterfall& a)
    :m_SeniorExpenses(a.m_SeniorExpenses)
    , m_SeniorFees(a.m_SeniorFees)
    , m_JuniorFees(a.m_JuniorFees)
    , m_MortgagesPayments(a.m_MortgagesPayments)
    , m_PaymentFrequency(a.m_PaymentFrequency)
    , m_ExcessCashFlow(a.m_ExcessCashFlow)
    , m_TotalSeniorExpenses(a.m_TotalSeniorExpenses)
    , m_TotalSeniorFees(a.m_TotalSeniorFees)
    , m_TotalJuniorFees(a.m_TotalJuniorFees)
    , m_ReinvestmentTest(a.m_ReinvestmentTest)
    , m_CCCTestLimit(a.m_CCCTestLimit)
    , m_CCCcurve(a.m_CCCcurve)
    , m_CCChaircut(a.m_CCChaircut)
    , m_PrincipalAvailable(a.m_PrincipalAvailable)
    , m_InterestAvailable(a.m_InterestAvailable)
    , m_JuniorFeesCoupon(a.m_JuniorFeesCoupon)
    , m_AnnualizedExcess(a.m_AnnualizedExcess)
    , m_FirstIPDdate(a.m_FirstIPDdate)
    , m_LastIPDdate(a.m_LastIPDdate)
    , m_CallDate(a.m_CallDate)
    , m_PoolValueAtCall(a.m_PoolValueAtCall)
    , m_UseCall(a.m_UseCall)
    , m_CallMultiple(a.m_CallMultiple)
    , m_CallReserve(a.m_CallReserve)
    , m_CalculatedMtgPayments(a.m_CalculatedMtgPayments)
    , m_EquityIncome(a.m_EquityIncome)
    , m_CumulativeReserves(a.m_CumulativeReserves)
    , m_DealName(a.m_DealName)
    , m_SeniorExpensesFixed(a.m_SeniorExpensesFixed)
    , m_SeniorFeesFixed(a.m_SeniorFeesFixed)
    , m_JuniorFeesFixed(a.m_JuniorFeesFixed)
    , m_GICinterest(a.m_GICinterest)
    , m_StartingDeferredJunFees(a.m_StartingDeferredJunFees)
    , m_GICflows(a.m_GICflows)
    , m_GICBaseRate(a.m_GICBaseRate)
    , m_GICBaseRateValue(a.m_GICBaseRateValue)
    , m_DealDayCountConvention(a.m_DealDayCountConvention)
    , m_CalledPeriod(a.m_CalledPeriod)
    , m_TriggersResults(a.m_TriggersResults)
    , m_IsStressTest(a.m_IsStressTest)
    , m_LegalFinal(a.m_LegalFinal)
{
    m_LoadProtocolVersion = a.m_LoadProtocolVersion;
    for (QList<Tranche*>::const_iterator i = a.m_Tranches.constBegin(); i != a.m_Tranches.constEnd(); i++) {
        m_Tranches.append(new Tranche(**i));
    }
    for (QList<WatFalPrior*>::const_iterator i = a.m_WaterfallStesps.constBegin(); i != a.m_WaterfallStesps.constEnd(); i++) {
        m_WaterfallStesps.append(new WatFalPrior(**i));
    }
    for (QList<ReserveFund*>::const_iterator i = a.m_Reserves.constBegin(); i != a.m_Reserves.constEnd(); i++) {
        m_Reserves.append(new ReserveFund(**i));
    }
    for (auto i = a.m_Triggers.constBegin(); i != a.m_Triggers.constEnd(); ++i) {
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
            Q_ASSERT_X(false, "Waterfall::CopyConstructor", "Unhandled Trigger Type");
            break;
        }
    }

}
Waterfall& Waterfall::operator=(const Waterfall& a)
{
    m_SeniorExpenses = a.m_SeniorExpenses;
    m_SeniorFees = a.m_SeniorFees;
    m_JuniorFees = a.m_JuniorFees;
    m_MortgagesPayments = a.m_MortgagesPayments;
    m_PaymentFrequency = a.m_PaymentFrequency;
    m_ExcessCashFlow = a.m_ExcessCashFlow;
    m_TotalSeniorExpenses = a.m_TotalSeniorExpenses;
    m_TotalSeniorFees = a.m_TotalSeniorFees;
    m_TotalJuniorFees = a.m_TotalJuniorFees;
    m_ReinvestmentTest = a.m_ReinvestmentTest;
    m_CCCTestLimit = a.m_CCCTestLimit;
    m_CCCcurve = a.m_CCCcurve;
    m_CCChaircut = a.m_CCChaircut;
    m_PrincipalAvailable = a.m_PrincipalAvailable;
    m_InterestAvailable = a.m_InterestAvailable;
    m_JuniorFeesCoupon = a.m_JuniorFeesCoupon;
    m_AnnualizedExcess = a.m_AnnualizedExcess;
    m_FirstIPDdate = a.m_FirstIPDdate;
    m_LastIPDdate = a.m_LastIPDdate;
    m_CallDate = a.m_CallDate;
    m_PoolValueAtCall = a.m_PoolValueAtCall;
    m_UseCall = a.m_UseCall;
    m_CallMultiple = a.m_CallMultiple;
    m_CallReserve = a.m_CallReserve;
    m_CalculatedMtgPayments = a.m_CalculatedMtgPayments;
    m_EquityIncome = a.m_EquityIncome;
    m_CumulativeReserves = a.m_CumulativeReserves;
    m_DealName = a.m_DealName;
    m_SeniorExpensesFixed = a.m_SeniorExpensesFixed;
    m_SeniorFeesFixed = a.m_SeniorFeesFixed;
    m_JuniorFeesFixed = a.m_JuniorFeesFixed;
    m_LoadProtocolVersion = a.m_LoadProtocolVersion;
    m_GICinterest = a.m_GICinterest;
    m_StartingDeferredJunFees = a.m_StartingDeferredJunFees;
    m_GICflows = a.m_GICflows;
    m_GICBaseRate = a.m_GICBaseRate;
    m_GICBaseRateValue = a.m_GICBaseRateValue;
    m_DealDayCountConvention = a.m_DealDayCountConvention;
    m_CalledPeriod = a.m_CalledPeriod;
    m_TriggersResults = a.m_TriggersResults;
    m_IsStressTest = a.m_IsStressTest;
    m_LegalFinal = a.m_LegalFinal;
    ResetTranches();
    for (QList<Tranche*>::const_iterator i = a.m_Tranches.constBegin(); i != a.m_Tranches.constEnd(); i++) {
        m_Tranches.append(new Tranche(**i));
    }
    ResetSteps();
    for (QList<WatFalPrior*>::const_iterator i = a.m_WaterfallStesps.constBegin(); i != a.m_WaterfallStesps.constEnd(); i++) {
        m_WaterfallStesps.append(new WatFalPrior(**i));
    }
    RemoveReserve();
    for (QList<ReserveFund*>::const_iterator i = a.m_Reserves.constBegin(); i != a.m_Reserves.constEnd(); i++) {
        m_Reserves.append(new ReserveFund(**i));
    }
    m_Triggers.clear();
    for (auto i = a.m_Triggers.constBegin(); i != a.m_Triggers.constEnd(); ++i) {
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
            Q_ASSERT_X(false, "Waterfall::operator=", "Unhandled Trigger Type");
            break;
        }
    }
    return *this;
}
Waterfall::~Waterfall()
{
    ResetSteps();
    ResetTranches();
    RemoveReserve();
    ResetTriggers();
}
void Waterfall::ResetSteps()
{
    for (QList<WatFalPrior*>::iterator i = m_WaterfallStesps.begin(); i != m_WaterfallStesps.end(); i++) {
        delete (*i);
    }
    m_WaterfallStesps.clear();
}
void Waterfall::ResetTranches()
{
    for (QList<Tranche*>::iterator i = m_Tranches.begin(); i != m_Tranches.end(); i++) {
        delete (*i);
    }
    m_Tranches.clear();
}
int Waterfall::FindMostJuniorLevel(int SeliorityScaleLevel)const
{
    if (m_Tranches.isEmpty()) return 0;
    int Result = m_Tranches.last()->GetProrataGroup(SeliorityScaleLevel);
    for (int i = m_Tranches.size() - 2; i >= 0; i--) {
        if (m_Tranches.at(i)->GetProrataGroup(SeliorityScaleLevel)>Result)Result = m_Tranches.at(i)->GetProrataGroup(SeliorityScaleLevel);
    }
    return Result;
}
void Waterfall::FillAllDates()
{
    bool AnchorDateNull = m_PaymentFrequency.GetAnchorDate().isNull();
    if (AnchorDateNull) m_PaymentFrequency.SetAnchorDate(m_MortgagesPayments.GetDate(0));
    bool ExcessIsEmpty = m_ExcessCashFlow.Count() == 0;
    //All the dates in all the tranches
    for (int i = 0; i<m_Tranches.size() - 1; i++) {
        for (int j = i + 1; j<m_Tranches.size(); j++) {
            for (int h = 0; h<m_Tranches.at(i)->GetCashFlow().Count(); h++) {
                if (m_Tranches.at(j)->GetCashFlow().FindDate(m_Tranches.at(i)->GetCashFlow().GetDate(h)) < 0) {
                    m_Tranches[j]->AddCashFlow(m_Tranches.at(i)->GetCashFlow().GetDate(h), 0.0, TrancheCashFlow::TrancheFlowType::InterestFlow);
                }
            }
        }
    }
    //dates from excess to all the tranches
    if (!ExcessIsEmpty) {
        for (int j = 0; j<m_Tranches.size(); j++) {
            for (int h = 0; h<m_ExcessCashFlow.Count(); h++) {
                if (m_Tranches.at(j)->GetCashFlow().FindDate(m_ExcessCashFlow.GetDate(h)) < 0) {
                    m_Tranches[j]->AddCashFlow(m_ExcessCashFlow.GetDate(h), 0.0, TrancheCashFlow::TrancheFlowType::InterestFlow);
                }
            }
        }
    }
    //dates from annualized excess to all the tranches (equity income dates included here)
    for (int j = 0; j<m_Tranches.size(); j++) {
        for (int h = 0; h<m_AnnualizedExcess.Count(); h++) {
            if (m_Tranches.at(j)->GetCashFlow().FindDate(m_AnnualizedExcess.GetDate(h)) < 0) {
                m_Tranches[j]->AddCashFlow(m_AnnualizedExcess.GetDate(h), 0.0, TrancheCashFlow::TrancheFlowType::InterestFlow);
            }
        }
    }
    //dates from reserve funds to all the tranches
    for (QList<ReserveFund*>::const_iterator i = m_Reserves.constBegin(); i != m_Reserves.constEnd(); i++) {
        if ((*i)->GetReserveFundFlow().Count() == 0) continue;
        for (int j = 0; j < m_Tranches.size(); j++) {
            for (int h = 0; h < (*i)->GetReserveFundFlow().Count(); h++) {
                if (m_Tranches.at(j)->GetCashFlow().FindDate((*i)->GetReserveFundFlow().GetDate(h)) < 0) {
                    m_Tranches[j]->AddCashFlow((*i)->GetReserveFundFlow().GetDate(h), 0.0, TrancheCashFlow::TrancheFlowType::InterestFlow);
                }
            }
        }
    }
    //dates from senior fees to all the tranches
    for (int j = 0; j<m_Tranches.size(); j++) {
        for (int h = 0; h<m_TotalSeniorFees.Count(); h++) {
            if (m_Tranches.at(j)->GetCashFlow().FindDate(m_TotalSeniorFees.GetDate(h)) < 0) {
                m_Tranches[j]->AddCashFlow(m_TotalSeniorFees.GetDate(h), 0.0, TrancheCashFlow::TrancheFlowType::InterestFlow);
            }
        }
    }
    //dates from junior fees to all the tranches
    for (int j = 0; j<m_Tranches.size(); j++) {
        for (int h = 0; h<m_TotalJuniorFees.Count(); h++) {
            if (m_Tranches.at(j)->GetCashFlow().FindDate(m_TotalJuniorFees.GetDate(h)) < 0) {
                m_Tranches[j]->AddCashFlow(m_TotalJuniorFees.GetDate(h), 0.0, TrancheCashFlow::TrancheFlowType::InterestFlow);
            }
        }
    }
    //dates from senior expenses to all the tranches
    for (int j = 0; j<m_Tranches.size(); j++) {
        for (int h = 0; h<m_TotalSeniorExpenses.Count(); h++) {
            if (m_Tranches.at(j)->GetCashFlow().FindDate(m_TotalSeniorExpenses.GetDate(h)) < 0) {
                m_Tranches[j]->AddCashFlow(m_TotalSeniorExpenses.GetDate(h), 0.0, TrancheCashFlow::TrancheFlowType::InterestFlow);
            }
        }
    }
    //dates from tranches to excess, fees and reserve
    if (m_Tranches.size()>0) {
        for (int h = 0; h<m_Tranches.at(0)->GetCashFlow().Count(); h++) {
            if (!ExcessIsEmpty) {
                if (m_ExcessCashFlow.FindDate(m_Tranches.at(0)->GetCashFlow().GetDate(h)) < 0)
                    m_ExcessCashFlow.AddFlow(m_Tranches.at(0)->GetCashFlow().GetDate(h), 0.0, 0);
            }
            if (m_AnnualizedExcess.FindDate(m_Tranches.at(0)->GetCashFlow().GetDate(h)) < 0)
                m_AnnualizedExcess.AddFlow(m_Tranches.at(0)->GetCashFlow().GetDate(h), 0.0, 0);
            if (m_TotalSeniorExpenses.FindDate(m_Tranches.at(0)->GetCashFlow().GetDate(h)) < 0)
                m_TotalSeniorExpenses.AddFlow(m_Tranches.at(0)->GetCashFlow().GetDate(h), 0.0, 0);
            if (m_TotalSeniorFees.FindDate(m_Tranches.at(0)->GetCashFlow().GetDate(h)) < 0)
                m_TotalSeniorFees.AddFlow(m_Tranches.at(0)->GetCashFlow().GetDate(h), 0.0, 0);
            if (m_TotalJuniorFees.FindDate(m_Tranches.at(0)->GetCashFlow().GetDate(h)) < 0)
                m_TotalJuniorFees.AddFlow(m_Tranches.at(0)->GetCashFlow().GetDate(h), 0.0, 0);
            for (QList<ReserveFund*>::iterator ResIter = m_Reserves.begin(); ResIter != m_Reserves.end(); ResIter++) {
                if ((*ResIter)->GetReserveFundFlow().Count() == 0) continue;
                if ((*ResIter)->GetReserveFundFlow().FindDate(m_Tranches.at(0)->GetCashFlow().GetDate(h)) < 0)
                    (*ResIter)->GetReserveFundFlow().AddFlow(m_Tranches.at(0)->GetCashFlow().GetDate(h), 0.0, static_cast<qint32>(ReserveFund::ReserveFlowsType::ReplenishFromInterest));
            }
        }
    }
    QDate RollingNextIPD = m_FirstIPDdate;

    for (int i = 0; i<m_CalculatedMtgPayments.Count(); i++) {
        if (m_CalculatedMtgPayments.GetDate(i).year() > RollingNextIPD.year() || (m_CalculatedMtgPayments.GetDate(i).year() == RollingNextIPD.year() && m_CalculatedMtgPayments.GetDate(i).month() >= RollingNextIPD.month()) || i >= m_CalculatedMtgPayments.Count() - 1) {
            for (int j = 0; j<m_Tranches.size(); j++) {
                m_Tranches[j]->GetCashFlow().ReplaceDate(m_CalculatedMtgPayments.GetDate(i), RollingNextIPD);
            }
            m_ExcessCashFlow.ReplaceDate(m_CalculatedMtgPayments.GetDate(i), RollingNextIPD);
            m_AnnualizedExcess.ReplaceDate(m_CalculatedMtgPayments.GetDate(i), RollingNextIPD);
            m_TotalSeniorExpenses.ReplaceDate(m_CalculatedMtgPayments.GetDate(i), RollingNextIPD);
            m_TotalSeniorFees.ReplaceDate(m_CalculatedMtgPayments.GetDate(i), RollingNextIPD);
            m_TotalJuniorFees.ReplaceDate(m_CalculatedMtgPayments.GetDate(i), RollingNextIPD);
            for (QList<ReserveFund*>::iterator ResIter = m_Reserves.begin(); ResIter != m_Reserves.end(); ResIter++) {
                if ((*ResIter)->GetReserveFundFlow().Count() == 0) continue;
                (*ResIter)->GetReserveFundFlow().ReplaceDate(m_CalculatedMtgPayments.GetDate(i), RollingNextIPD);
            }
            RollingNextIPD = RollingNextIPD.addMonths(m_PaymentFrequency.GetValue(RollingNextIPD));
        }
    }
    if (AnchorDateNull) m_PaymentFrequency.RemoveAnchorDate();
}
int Waterfall::FindTrancheIndex(const QString& Tranchename)const
{
    for (int j = 0; j<m_Tranches.size(); j++) {
        if (m_Tranches.at(j)->GetTrancheName() == Tranchename) return j;
        if (m_Tranches.at(j)->GetISIN() == Tranchename) return j;
    }
#ifndef NO_BLOOMBERG
    QBloombergLib::QBbgWorker ISINparser;
    QBloombergLib::QBbgRequest ISINRequest;
    ISINRequest.AddRequest(Tranchename, "NAME");
    ISINRequest.AddRequest(Tranchename, "ID_ISIN");
    ISINRequest.AddRequest(Tranchename, "ID_CUSIP");
    ISINRequest.AddRequest(Tranchename, "ID_BB_GLOBAL");
    ISINRequest.AddRequest(Tranchename, "ID_BB_UNIQU");
    ISINparser.StartRequestSync(ISINRequest);
    if (!ISINparser.GetRequest().HasErrors()) {
        for (QBloombergLib::QBbgResultsIterator RespIter = ISINparser.GetResultIterator(); RespIter.IsValid(); ++RespIter) {
            if (!RespIter->HasErrors()) {
                for (int i = 0; i < m_Tranches.size(); i++) {
                    if (
                        RespIter->GetString().compare(m_Tranches.at(i)->GetTrancheName().trimmed(), Qt::CaseInsensitive) == 0
                        || RespIter->GetString().compare(m_Tranches.at(i)->GetISIN().trimmed(), Qt::CaseInsensitive) == 0
                        ) return i;
                }
            }
        }
    }
#endif 
    return -1;
}
double Waterfall::GetCreditEnhancement(int TrancheIndex, int TimeIndex)const
{
    if (TrancheIndex<-1 || TrancheIndex >= m_Tranches.size()) return 0.0;
    int TargetSeniority = m_Tranches.at(TrancheIndex)->GetProrataGroup(0);
    double Runningsum = 0.0;
    double ReserveSum;
    for (int i = 0; i<m_Tranches.size(); i++) {
        if (m_Tranches.at(i)->GetProrataGroup(0) <= TargetSeniority) {
            if (TimeIndex >= 0) {
                Runningsum += m_Tranches.at(i)->GetCashFlow().GetAmountOutstanding(TimeIndex);
            }
            else {
                Runningsum += m_Tranches.at(i)->GetOutstandingAmt();
            }
        }
    }
    if (Runningsum == 0.0) return 1.0;
    if (TimeIndex >= 0) {
        ReserveSum = m_CalculatedMtgPayments.GetAmountOut(m_Tranches.first()->GetCashFlow().GetDate(TimeIndex));
        for (QList<ReserveFund*>::const_iterator ResIter = m_Reserves.constBegin(); ResIter != m_Reserves.constEnd(); ResIter++) {
            if ((*ResIter)->GetReserveFundFlow().Count() > 0 && TrancheIndex <= (*ResIter)->GetReserveFundFreed())
                ReserveSum += (*ResIter)->GetReserveFundCurrent((*ResIter)->GetReserveFundFlow().GetDate(TimeIndex));
        }
        if (ReserveSum <= 0.0) return 0.0;
        return 1.0 - (Runningsum / ReserveSum); //(m_CalculatedMtgPayments.GetAmountOut(m_Tranches.first()->GetCashFlow().GetDate(TimeIndex))/Runningsum)-1.0;
    }
    else {
        ReserveSum = m_MortgagesPayments.GetAmountOut(0);
        ReserveSum += m_PrincipalAvailable.Total();
        for (QList<ReserveFund*>::const_iterator ResIter = m_Reserves.constBegin(); ResIter != m_Reserves.constEnd(); ResIter++) {
            if (TrancheIndex <= (*ResIter)->GetReserveFundFreed())
                ReserveSum += (*ResIter)->GetReserveFundCurrent();
        }
        if (ReserveSum <= 0.0) return 0.0;
        return 1.0 - (Runningsum / ReserveSum); //((m_CalculatedMtgPayments.GetAmountOut(0)+m_PrincipalAvailable)/Runningsum)-1.0;
    }
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
    if (CouponTypes.isEmpty()) {
        for (QList<Tranche*>::const_iterator i = m_Tranches.begin(); i != m_Tranches.end(); i++) {
            if ((*i)->GetProrataGroup(SeliorityScaleLevel) == GroupTarget) {
                CouponTypes.append((*i)->GetCouponIndexes());
            }
        }
        CouponTypes = CouponTypes.toSet().toList();
    }
    double RunningSum = 0.0;
    double Result = 0.0;
    double CurrentCoup;
    for (QList<Tranche*>::const_iterator i = m_Tranches.begin(); i != m_Tranches.end(); i++) {
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
    m_ReinvestmentTest.SetupReinvBond(IntrVec, CPRVec, CDRVec, LSVec, WALval, PayFreq, AnnuityVec, ReinvPric, ReinvDel, ReinvSpr, FloatingBase, RecoveryLag, Delinquency, DelinquencyLag);
}
void Waterfall::SetReinvestementPeriod(const QDate& ReinvPer)
{
    m_ReinvestmentTest.SetReinvestementPeriod(ReinvPer);
    QSharedPointer<AbstractTrigger> TempReinvTrigger(new DateTrigger(ReinvPer, DateTrigger::TriggerSide::BeforeIncluding, "Reinvestment Period"));
    SetTrigger(0, TempReinvTrigger);
}
double Waterfall::RedeemNotes(double AvailableFunds, int GroupTarget, int SeliorityScaleLevel, const QDate& TargetDate)
{
    if (AvailableFunds<0.01) return 0.0;
    QMultiMap<quint32, QList<Tranche*>::iterator> groups;
    double TotalPayable = 0.0;
    for (auto i = m_Tranches.begin(); i != m_Tranches.end(); ++i) {
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
    QMap<quint32, double> groupAccrue;
    QHash < quint32, QList<quint32> * > groupRanks;
    auto groupKeys = groups.keys();
    for (auto i = groupKeys.constBegin(); i != groupKeys.constEnd(); ++i) {
        auto currVals = groups.values(*i);
        groupAccrue.insert(*i, 0.0);
        groupRanks.insert(*i, new QList<quint32>());
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
    double RunningSum = 0.0;
    double Result = 0.0;
    if (m_Tranches.isEmpty()) return 0.0;
    if (index<0 || index >= m_Tranches.first()->GetCashFlow().Count()) return 0.0;
    for (QList<Tranche*>::const_iterator i = m_Tranches.begin(); i != m_Tranches.end(); i++) {
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
    if (!ReadyToCalculate().isEmpty()) {
        PrintToTempFile("ReturnFalse.txt", "Not Ready To Calculate");
        return false;
    }
        {//Check if all base rates are valid
            bool KeepSearching = false;
            ConstantBaseRateTable TempTable;
            if (m_GICBaseRateValue.IsEmpty()) {
                if (m_GICBaseRate.IsEmpty()) {
                    m_GICBaseRateValue = "0";
                }
                else {
                    m_GICBaseRateValue = m_GICBaseRate.CompileReferenceRateValue(TempTable);
                    if (m_GICBaseRateValue.IsEmpty()) {
                        PrintToTempFile("ReturnFalse.txt", "Missing Base Rate Value");
                        return false;
                    }
                }
            }
            for (QList<Tranche*>::const_iterator SingleTranche = m_Tranches.constBegin(); SingleTranche != m_Tranches.constEnd(); ++SingleTranche) {
                auto keysList = (*SingleTranche)->GetReferenceRateIndexes();
                for (auto SingleRate = keysList.constBegin(); SingleRate != keysList.constEnd(); ++SingleRate) {
                    if ((*SingleTranche)->GetInterestType(*SingleRate) == Tranche::FloatingInterest && (*SingleTranche)->GetReferenceRateValue(*SingleRate).IsEmpty()) {
                        KeepSearching = true;
                        (*SingleTranche)->CompileReferenceRateValue(TempTable);
                        break;
                    }
                }
            }
            for (QList<Tranche*>::const_iterator SingleTranche = m_Tranches.constBegin(); KeepSearching && SingleTranche != m_Tranches.constEnd(); ++SingleTranche) {
                for (QHash<qint32, BloombergVector*>::const_iterator SingleRate = (*SingleTranche)->GetRefRateValues().constBegin(); SingleRate != (*SingleTranche)->GetRefRateValues().constEnd(); ++SingleRate) {
                    if (SingleRate.value()->IsEmpty()) {
                        PrintToTempFile("ReturnFalse.txt", "Missing Base Rate Value");
                        return false;
                    }
                }
            }
        }
        { //Check steps of the waterfall
            foreach(const WatFalPrior* SingleStep, m_WaterfallStesps)
            {
                //Check if all triggers are valid
                if (SingleStep->HasParameter(WatFalPrior::wstParameters::Trigger)) {
                    if (!ValidTriggerStructure(SingleStep->GetParameter(WatFalPrior::wstParameters::Trigger).toString())) {
                        PrintToTempFile("ReturnFalse.txt", "Invalid Trigger Structure");
                        return false;
                    }
                }
            }
        }
        QDate ActualCallDate; //Date in Loans Flow that is the limit for the call
        m_CalledPeriod = QDate();
        m_TriggersResults.ClearResults();
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
        QDate RollingNextIPD = m_FirstIPDdate;
        QDate RollingLastIPD = m_LastIPDdate;
        double adjSeniorExpenses;//De-annualized Senior Expenses
        double adjSeniorFees;//De-annualized Senior Fees 
        double adjJuniorFees;//De-annualized Junior Fees 
        double OriginalAvailableInterest = m_InterestAvailable; //Stores the interest cash
        PrincipalRecip OriginalAvailablePrincipal = m_PrincipalAvailable; //Stores the Principal cash
        double OriginalStartingDefJunFees = m_StartingDeferredJunFees; //Stores the current deferred junior fees
        bool IsCallPaymentDate = false; //Stores if the current date is the call date
        double ActualCallReserveLevel = 0.0;// Temp Variable used for call calculation
        //QDate CurrentDate;
        bool IsMaturityDate = false;  //Stores if the current date is the maturity date of the notes
        MtgCashFlow OriginalMtgFlows = m_MortgagesPayments; //Stores the loans cash flows before reinvestments
        m_ExcessCashFlow.Clear();
        m_TotalSeniorExpenses.Clear();
        m_TotalSeniorFees.Clear();
        m_TotalJuniorFees.Clear();
        m_AnnualizedExcess.Clear();
        m_EquityIncome.Clear();
        m_ReinvestmentTest.ClearFlows();
        bool NullCCCanchor[] = { //Stores whether the vectors have anchors or not
            m_CCCcurve.GetAnchorDate().isNull()
            , m_PaymentFrequency.GetAnchorDate().isNull()
            , m_SeniorExpenses.GetAnchorDate().isNull()
            , m_SeniorFees.GetAnchorDate().isNull()
            , m_JuniorFees.GetAnchorDate().isNull()
            , m_SeniorExpensesFixed.GetAnchorDate().isNull()
            , m_SeniorFeesFixed.GetAnchorDate().isNull()
            , m_JuniorFeesFixed.GetAnchorDate().isNull()
            , m_GICinterest.GetAnchorDate().isNull()
            , m_GICBaseRateValue.GetAnchorDate().isNull()
        };
        if (NullCCCanchor[0]) m_CCCcurve.SetAnchorDate(m_MortgagesPayments.GetDate(0));
        if (NullCCCanchor[1]) m_PaymentFrequency.SetAnchorDate(m_MortgagesPayments.GetDate(0));
        if (NullCCCanchor[2]) m_SeniorExpenses.SetAnchorDate(m_MortgagesPayments.GetDate(0));
        if (NullCCCanchor[3]) m_SeniorFees.SetAnchorDate(m_MortgagesPayments.GetDate(0));
        if (NullCCCanchor[4]) m_JuniorFees.SetAnchorDate(m_MortgagesPayments.GetDate(0));
        if (NullCCCanchor[5]) m_SeniorExpensesFixed.SetAnchorDate(m_MortgagesPayments.GetDate(0));
        if (NullCCCanchor[6]) m_SeniorFeesFixed.SetAnchorDate(m_MortgagesPayments.GetDate(0));
        if (NullCCCanchor[7]) m_JuniorFeesFixed.SetAnchorDate(m_MortgagesPayments.GetDate(0));
        if (NullCCCanchor[8]) m_GICinterest.SetAnchorDate(m_MortgagesPayments.GetDate(0));
        if (NullCCCanchor[9]) m_GICBaseRateValue.SetAnchorDate(m_MortgagesPayments.GetDate(0));
        m_ReinvestmentTest.SetMissingAnchors(m_MortgagesPayments.GetDate(0));
        foreach(Tranche* SingleTranche, m_Tranches)
        {
            SingleTranche->GetCashFlow().Clear();
        }
        foreach(WatFalPrior* SingleStp, m_WaterfallStesps)
        {
            SingleStp->FillMissingAnchors(m_MortgagesPayments.GetDate(0));
        }
        for (QDate CurrentDate = m_MortgagesPayments.GetDate(0); !IsMaturityDate; CurrentDate = CurrentDate.addMonths(1)) {
            int CurrentPeriodIndex = MonthDiff(CurrentDate, m_MortgagesPayments.GetDate(0));
            if (CurrentDate>m_MortgagesPayments.MaturityDate() && (
                m_LegalFinal.isNull()
                || m_ReinvestmentTest.ReinvestQueueueEmpty()
                )) IsMaturityDate = true;
            if (!m_LegalFinal.isNull() && CurrentDate >= m_LegalFinal)  IsMaturityDate = true;
            if (!IsMaturityDate && m_MortgagesPayments.FindDate(CurrentDate) < 0) m_MortgagesPayments.AddFlow(CurrentDate, 0, 0);
            if (IsMaturityDate) {
                m_PrincipalAvailable.AddPrepay(m_ReinvestmentTest.GetQueuedCash());
                m_ReinvestmentTest.ResetReinvestQueueue();
            }
            else {
                //Process the current reinvestments
                const MtgCashFlow& ReinvFlows = m_ReinvestmentTest.ProcessQueue(CurrentDate, CurrentPeriodIndex, m_LegalFinal);
                if (!ReinvFlows.IsEmpty()) {
                    m_InterestAvailable += ReinvFlows.GetInterest(CurrentDate);
                    m_PrincipalAvailable.AddScheduled(ReinvFlows.GetScheduled(CurrentDate));
                    m_PrincipalAvailable.AddPrepay(ReinvFlows.GetPrepay(CurrentDate));
                    m_ReinvestmentTest.RemoveBondFlow(CurrentDate);
                    m_MortgagesPayments.AddFlow(m_ReinvestmentTest.GetBondCashFlow());
                }
            }
            if (CurrentDate > m_MortgagesPayments.GetDate(0)) {
                CurrentAssetSum += m_MortgagesPayments.GetAmountOut(CurrentDate.addMonths(-1)) + m_ReinvestmentTest.GetQueuedCash();
                ++CurrentAssetCount;
                TotalPayable = AdjustCoupon(m_GICinterest.GetValue(CurrentDate) + m_GICBaseRateValue.GetValue(CurrentDate), CurrentDate.addMonths(-1), CurrentDate, m_DealDayCountConvention.GetValue(CurrentDate));
                m_GICflows.AddFlow(CurrentDate, m_InterestAvailable*TotalPayable, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow));
                m_GICflows.AddFlow(CurrentDate, m_PrincipalAvailable.Total()*TotalPayable, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PrincipalFlow));
                m_GICflows.AddFlow(CurrentDate, m_ReinvestmentTest.GetQueuedCash()*TotalPayable, 2 | static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow));
                foreach(const ReserveFund* SingleRes, m_Reserves)
                {
                    m_GICflows.AddFlow(CurrentDate, SingleRes->GetReserveFundCurrent()*TotalPayable, 1 | static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow));
                }
                m_InterestAvailable += m_GICflows.GetTotalFlow(CurrentDate);
            }
            m_PrincipalAvailable.AddScheduled(m_MortgagesPayments.GetScheduled(CurrentDate));
            m_PrincipalAvailable.AddPrepay(m_MortgagesPayments.GetPrepay(CurrentDate));
            m_InterestAvailable += m_MortgagesPayments.GetInterest(CurrentDate);
            if (m_UseCall && !IsCallPaymentDate) {
                ActualCallReserveLevel = 0.0;
                TotalPayable = 0.0;
                if (m_CallReserve>0 && m_CallMultiple>0) {
                    foreach(Tranche* SingleTranche, m_Tranches)
                    {
                        TotalPayable += SingleTranche->GetCashFlow().GetAmountOutstanding(CurrentDate);
                        if (SingleTranche->GetProrataGroup(0) >= m_CallReserve) ActualCallReserveLevel += SingleTranche->GetCashFlow().GetAmountOutstanding(CurrentDate);
                    }
                    if (ActualCallReserveLevel == 0.0)ActualCallReserveLevel = m_CallReserve;
                    else {
                        for (QList<ReserveFund*>::const_iterator ResIter = m_Reserves.constBegin(); ResIter != m_Reserves.constEnd(); ResIter++)
                            ActualCallReserveLevel += (*ResIter)->GetReserveFundCurrent();
                    }
                    ActualCallReserveLevel *= m_CallMultiple;
                    IsCallPaymentDate = ActualCallReserveLevel >= (TotalPayable - m_PrincipalAvailable.Total());
                }
                IsCallPaymentDate = IsCallPaymentDate || (!m_CallDate.isNull() && CurrentDate >= m_CallDate);
            }
            IsCallPaymentDate = IsCallPaymentDate || CurrentDate >= m_LegalFinal;
            if ((CurrentDate.year()<RollingNextIPD.year() || (CurrentDate.year() == RollingNextIPD.year() && CurrentDate.month()<RollingNextIPD.month())) && !IsMaturityDate) {
                //This is not a Tranche payment date
                foreach(const WatFalPrior* SingleStep, m_WaterfallStesps)
                {
                    if (SingleStep->GetPriorityType() == WatFalPrior::WaterfallStepType::wst_ReinvestPrincipal) {
                        if (
                            (
                            !SingleStep->HasParameter(WatFalPrior::wstParameters::Trigger)
                            || TriggerPassing(SingleStep->GetParameter(WatFalPrior::wstParameters::Trigger).toString(), CurrentPeriodIndex, RollingNextIPD, IsCallPaymentDate || IsMaturityDate)
                            )
                            && !IsCallPaymentDate
                            && !IsMaturityDate
                            ) {
                            //If it should, reinvest
                            if (m_PrincipalAvailable.Total() > 0.0) {
                                double PayablePrincipal;
                                if (SingleStep->GetParameter(WatFalPrior::wstParameters::SourceOfFunding).value<IntegerVector>().GetValue(CurrentDate) == 1) {  //Reinvest Prepay Only
                                    PayablePrincipal = m_PrincipalAvailable.GetPrepay()*SingleStep->GetParameter(WatFalPrior::wstParameters::AdditionalCollateralShare).value<BloombergVector>().GetValue(CurrentDate);
                                    m_PrincipalAvailable.AddPrepay(-PayablePrincipal);
                                }
                                else if (SingleStep->GetParameter(WatFalPrior::wstParameters::SourceOfFunding).value<IntegerVector>().GetValue(CurrentDate) == 2) {  //Reinvest scheduled only
                                    PayablePrincipal = m_PrincipalAvailable.GetScheduled()*SingleStep->GetParameter(WatFalPrior::wstParameters::AdditionalCollateralShare).value<BloombergVector>().GetValue(CurrentDate);
                                    m_PrincipalAvailable.AddScheduled(-PayablePrincipal);
                                }
                                else {
                                    PayablePrincipal = m_PrincipalAvailable.Total()*SingleStep->GetParameter(WatFalPrior::wstParameters::AdditionalCollateralShare).value<BloombergVector>().GetValue(CurrentDate);
                                    m_PrincipalAvailable -= PayablePrincipal;
                                }

                                {
                                    //Fill the reinvestments queue and process the current reinvestments
                                    m_ReinvestmentTest.QueueReinvestments(PayablePrincipal, CurrentDate);
                                    const MtgCashFlow& ReinvFlows = m_ReinvestmentTest.ProcessQueue(CurrentDate, CurrentPeriodIndex, m_LegalFinal);
                                    if (!ReinvFlows.IsEmpty()) {
                                        m_InterestAvailable += ReinvFlows.GetInterest(CurrentDate);
                                        m_PrincipalAvailable.AddScheduled(ReinvFlows.GetScheduled(CurrentDate));
                                        m_PrincipalAvailable.AddPrepay(ReinvFlows.GetPrepay(CurrentDate));
                                        m_ReinvestmentTest.RemoveBondFlow(CurrentDate);
                                        m_MortgagesPayments.AddFlow(m_ReinvestmentTest.GetBondCashFlow());
                                    }
                                }
                            }
                        }
                    }
                }
                continue;
            }
            //This is a Tranche payment date
            AvailablePrincipal = m_PrincipalAvailable;
            AvailableInterest = m_InterestAvailable;
            for (QList<ReserveFund*>::iterator ResIter = m_Reserves.begin(); ResIter != m_Reserves.end(); ResIter++) {
                if ((*ResIter)->GetReserveToInterest()) AvailableInterest += (*ResIter)->GetReserveFundCurrent();
                else AvailablePrincipal += (*ResIter)->GetReserveFundCurrent();
                (*ResIter)->SetReserveFundCurrent(0.0);
            }
            if (IsCallPaymentDate) {
                m_CalledPeriod = RollingNextIPD;
                ActualCallDate = CurrentDate;
                if (m_PoolValueAtCall.IsEmpty())
                    TotalPayable = m_MortgagesPayments.GetWAPrice(CurrentDate);
                else
                    TotalPayable = m_PoolValueAtCall.GetValue(CurrentDate);
                AvailablePrincipal += TotalPayable*m_MortgagesPayments.GetAmountOut(CurrentDate);
            }
            foreach(WatFalPrior* SingleStep, m_WaterfallStesps)
            {//Cycle through the steps of the waterfall
                if (SingleStep->HasParameter(WatFalPrior::wstParameters::Trigger)) {
                    if (!TriggerPassing(SingleStep->GetParameter(WatFalPrior::wstParameters::Trigger).toString(), CurrentPeriodIndex, RollingNextIPD, IsCallPaymentDate || IsMaturityDate))
                        continue;
                }
                switch (SingleStep->GetPriorityType()) {
                    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                case WatFalPrior::WaterfallStepType::wst_SeniorExpenses:
                    adjSeniorExpenses = AdjustCoupon(m_SeniorExpenses.GetValue(CurrentDate), RollingLastIPD, RollingNextIPD, m_DealDayCountConvention.GetValue(CurrentDate));
                    TotalPayable = adjSeniorExpenses*(CurrentAssetSum / static_cast<double>(CurrentAssetCount))
                        - m_TotalSeniorExpenses.GetFlow(CurrentDate, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow))
                        - m_TotalSeniorExpenses.GetFlow(CurrentDate, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PrincipalFlow))
                        ;
                    TotalPayable += m_SeniorExpensesFixed.GetValue(CurrentDate)*static_cast<double>(m_PaymentFrequency.GetValue(CurrentDate)) / 12.0;
                    if (TotalPayable >= 0.01) {
                        if (SingleStep->GetParameter(WatFalPrior::wstParameters::SourceOfFunding).value<IntegerVector>().GetValue(CurrentDate) == 1) {
                            m_TotalSeniorExpenses.AddFlow(CurrentDate, qMin(AvailableInterest, TotalPayable), static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow));
                            AvailableInterest = qMax(0.0, AvailableInterest - TotalPayable);
                        }
                        else if (SingleStep->GetParameter(WatFalPrior::wstParameters::SourceOfFunding).value<IntegerVector>().GetValue(CurrentDate) == 2) {
                            m_TotalSeniorExpenses.AddFlow(CurrentDate, qMin(AvailablePrincipal.Total(), TotalPayable), static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PrincipalFlow));
                            AvailablePrincipal -= TotalPayable;
                            if (AvailablePrincipal < 0.0) AvailablePrincipal.Erase();
                        }
                    }
                    break;
                    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                case WatFalPrior::WaterfallStepType::wst_SeniorFees:
                    adjSeniorFees = AdjustCoupon(m_SeniorFees.GetValue(CurrentDate), RollingLastIPD, RollingNextIPD, m_DealDayCountConvention.GetValue(CurrentDate));
                    TotalPayable = adjSeniorFees*(CurrentAssetSum / static_cast<double>(CurrentAssetCount))
                        - m_TotalSeniorFees.GetFlow(CurrentDate, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow))
                        - m_TotalSeniorFees.GetFlow(CurrentDate, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PrincipalFlow))
                        ;
                    TotalPayable += m_SeniorFeesFixed.GetValue(CurrentDate)*static_cast<double>(m_PaymentFrequency.GetValue(CurrentDate)) / 12.0;;
                    if (TotalPayable >= 0.01) {
                        if (SingleStep->GetParameter(WatFalPrior::wstParameters::SourceOfFunding).value<IntegerVector>().GetValue(CurrentDate) == 1) {
                            m_TotalSeniorFees.AddFlow(CurrentDate, qMin(AvailableInterest, TotalPayable), static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow));
                            AvailableInterest = qMax(0.0, AvailableInterest - TotalPayable);
                        }
                        else if (SingleStep->GetParameter(WatFalPrior::wstParameters::SourceOfFunding).value<IntegerVector>().GetValue(CurrentDate) == 2) {
                            m_TotalSeniorFees.AddFlow(CurrentDate, qMin(AvailablePrincipal.Total(), TotalPayable), static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PrincipalFlow));
                            AvailablePrincipal -= TotalPayable;
                            if (AvailablePrincipal < 0.0) AvailablePrincipal.Erase();
                        }
                    }
                    break;
                    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                case WatFalPrior::WaterfallStepType::wst_juniorFees:
                    if (SingleStep->GetParameter(WatFalPrior::wstParameters::PayAccrue).toInt() & static_cast<quint8>(WatFalPrior::wstAccrueOrPay::Accrue)) {
                        adjJuniorFees = AdjustCoupon(m_JuniorFees.GetValue(CurrentDate), RollingLastIPD, RollingNextIPD, m_DealDayCountConvention.GetValue(CurrentDate));
                        TotalPayable = (adjJuniorFees*(CurrentAssetSum / static_cast<double>(CurrentAssetCount)))
                            - m_TotalJuniorFees.GetFlow(CurrentDate, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::AccruedFlow))
                            + (AdjustCoupon(m_JuniorFeesCoupon, RollingLastIPD, RollingNextIPD, m_DealDayCountConvention.GetValue(CurrentDate))*(m_TotalJuniorFees.GetFlow(CurrentDate, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::DeferredFlow)) + m_StartingDeferredJunFees))
                            ;
                        m_StartingDeferredJunFees = 0.0;
                        TotalPayable += m_JuniorFeesFixed.GetValue(CurrentDate)*static_cast<double>(m_PaymentFrequency.GetValue(CurrentDate)) / 12.0;;
                        TotalPayable = qMax(TotalPayable, 0.0);
                        m_TotalJuniorFees.AddFlow(CurrentDate, TotalPayable, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::AccruedFlow));
                    }
                    if (SingleStep->GetParameter(WatFalPrior::wstParameters::PayAccrue).toInt() & static_cast<quint8>(WatFalPrior::wstAccrueOrPay::Pay)) {
                        TotalPayable =
                            m_TotalJuniorFees.GetFlow(CurrentDate, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::AccruedFlow))
                            + m_TotalJuniorFees.GetFlow(CurrentDate, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::DeferredFlow))
                            - m_TotalJuniorFees.GetTotalFlow(CurrentDate, QList<qint32>() << static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow) << static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PrincipalFlow))
                            ;
                        if (SingleStep->GetParameter(WatFalPrior::wstParameters::SourceOfFunding).value<IntegerVector>().GetValue(CurrentDate) == 1) {
                            Solution = qMin(AvailableInterest, TotalPayable);
                            m_TotalJuniorFees.AddFlow(CurrentDate, Solution, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow));
                            m_AnnualizedExcess.AddFlow(CurrentDate, Solution, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow));
                            AvailableInterest -= Solution;
                        }
                        if (SingleStep->GetParameter(WatFalPrior::wstParameters::SourceOfFunding).value<IntegerVector>().GetValue(CurrentDate) == 2) {
                            Solution = qMin(AvailablePrincipal.Total(), TotalPayable);
                            m_TotalJuniorFees.AddFlow(CurrentDate, Solution, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PrincipalFlow));
                            m_AnnualizedExcess.AddFlow(CurrentDate, Solution, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PrincipalFlow));
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
                    m_TotalJuniorFees.AddFlow(CurrentDate, TotalPayable, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow) + 1);
                    m_AnnualizedExcess.AddFlow(CurrentDate, TotalPayable, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow) + 1);
                    break;
                    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                case WatFalPrior::WaterfallStepType::wst_ReserveReplenish:
                {
                    int RevIdx = SingleStep->GetParameter(WatFalPrior::wstParameters::ReserveIndex).value<IntegerVector>().GetValue(CurrentDate);
                    if (RevIdx< 0 || RevIdx >= m_Reserves.size()) {
                        PrintToTempFile("ReturnFalse.txt", "Reserve Fund Fail");
                        return false;
                    }
                    double FreedAmnt, StacedRsAmt = 0.0;
                    int MostJun = FindMostJuniorLevel(0);

                    if (m_Reserves.at(RevIdx)->GetReserveFundFreed() == 0 || m_Reserves.at(RevIdx)->GetReserveFundFreed()>MostJun)
                        FreedAmnt = 1.0;
                    else {
                        FreedAmnt = 0.0;
                        for (int h = 0; h<m_Tranches.size(); h++) {
                            if (m_Tranches.at(h)->GetProrataGroup(0) <= m_Reserves.at(RevIdx)->GetReserveFundFreed())
                                FreedAmnt += m_Tranches.at(h)->GetCashFlow().GetAmountOutstanding(CurrentDate);
                        }
                    }
                    if (!IsCallPaymentDate && !IsMaturityDate &&  FreedAmnt>0.01) {
                        TestTarget = 0.0;
                        if (qRound(m_Reserves.at(RevIdx)->GetReserveFundTarget().GetValue(CurrentDate))>MostJun) TestTarget = m_Reserves.at(RevIdx)->GetReserveFundTarget().GetValue(CurrentDate);
                        else {
                            foreach(Tranche* SingleTranche, m_Tranches)
                            {
                                if (SingleTranche->GetProrataGroup(0) <= qRound(m_Reserves.at(RevIdx)->GetReserveFundTarget().GetValue(CurrentDate))) TestTarget += SingleTranche->GetCashFlow().GetAmountOutstanding(CurrentDate);
                            }
                        }
                        if (m_CumulativeReserves) {
                            for (int ResIter = 0; ResIter < RevIdx; ResIter++)
                                StacedRsAmt += m_Reserves.at(ResIter)->GetReserveFundCurrent();
                        }
                        else StacedRsAmt = m_Reserves.at(RevIdx)->GetReserveFundCurrent();
                        TotalPayable = qMax(
                            m_Reserves.at(RevIdx)->GetReserveFundFloor().GetValue(CurrentDate),
                            TestTarget*m_Reserves.at(RevIdx)->GetReserveFundMultiple().GetValue(CurrentDate)
                            );
                        if (m_Reserves.at(RevIdx)->GetReserveFundCap().GetValue(CurrentDate) > 0.0)
                            TotalPayable = qMin(TotalPayable, m_Reserves.at(RevIdx)->GetReserveFundCap().GetValue(CurrentDate));
                        TotalPayable -= StacedRsAmt;

                        if (TotalPayable >= 0.01) {
                            if (SingleStep->GetParameter(WatFalPrior::wstParameters::SourceOfFunding).value<IntegerVector>().GetValue(CurrentDate) == 1) {
                                m_Reserves[RevIdx]->GetReserveFundFlow().AddFlow(CurrentDate, qMin(AvailableInterest, TotalPayable), static_cast<qint32>(ReserveFund::ReserveFlowsType::ReplenishFromInterest));
                                m_Reserves[RevIdx]->GetReserveFundFlow().SetFlow(CurrentDate, qMax(0.0, TotalPayable - AvailableInterest), static_cast<qint32>(ReserveFund::ReserveFlowsType::ShortFall));
                                m_Reserves[RevIdx]->SetReserveFundCurrent(m_Reserves.at(RevIdx)->GetReserveFundCurrent() + qMin(AvailableInterest, TotalPayable));
                                AvailableInterest = qMax(0.0, AvailableInterest - TotalPayable);
                            }
                            else if (SingleStep->GetParameter(WatFalPrior::wstParameters::SourceOfFunding).value<IntegerVector>().GetValue(CurrentDate) == 2) {

                                m_Reserves[RevIdx]->GetReserveFundFlow().AddFlow(CurrentDate, qMin(AvailablePrincipal.Total(), TotalPayable), static_cast<qint32>(ReserveFund::ReserveFlowsType::ReplenishFromPrincipal));
                                m_Reserves[RevIdx]->GetReserveFundFlow().SetFlow(CurrentDate, qMax(0.0, TotalPayable - AvailablePrincipal.Total()), static_cast<qint32>(ReserveFund::ReserveFlowsType::ShortFall));
                                m_Reserves[RevIdx]->SetReserveFundCurrent(m_Reserves.at(RevIdx)->GetReserveFundCurrent() + qMin(AvailablePrincipal.Total(), TotalPayable));
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
                    if (SingleStep->GetParameter(WatFalPrior::wstParameters::PayAccrue).toInt() & static_cast<quint8>(WatFalPrior::wstAccrueOrPay::Accrue)) {
                        foreach(Tranche* SingleTranche, m_Tranches)
                        {
                            if (SingleTranche->GetProrataGroup(CurrSenGrpLvl) == CurrSenGrp) {
                                if (!SingleTranche->HasCoupon(CurrCoupIndx)) {
                                    PrintToTempFile("ReturnFalse.txt", SingleTranche->GetTrancheName() + " - Coupon not set in tranche");
                                    return false;
                                }
                                AdjustedCoupon = AdjustCoupon(SingleTranche->GetCoupon(CurrentDate, CurrCoupIndx), RollingLastIPD, RollingNextIPD, SingleTranche->GetDayCount(CurrCoupIndx).GetValue(CurrentDate));
                                Solution = SingleTranche->GetCashFlow().GetDeferred(CurrentDate, CurrCoupIndx);
                                Solution += SingleTranche->GetCashFlow().GetAmountOutstanding(CurrentDate);
                                Solution *= AdjustedCoupon;
                                Solution -= SingleTranche->GetCashFlow().GetAccrued(CurrentDate, CurrCoupIndx);
                                SingleTranche->AddCashFlow(CurrentDate, Solution, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::AccruedFlow) | CurrCoupIndx);
                            }
                        }
                    }
                    if (SingleStep->GetParameter(WatFalPrior::wstParameters::PayAccrue).toInt() & static_cast<quint8>(WatFalPrior::wstAccrueOrPay::Pay)) {
                        ProRataBonds.clear();
                        TotalPayable = 0.0;
                        Solution = 0.0;
                        for (int h = 0; h < m_Tranches.size(); h++) {
                            if (m_Tranches.at(h)->GetProrataGroup(CurrSenGrpLvl) == CurrSenGrp) {
                                TotalPayable += m_Tranches.at(h)->GetCashFlow().GetAccrued(CurrentDate, CurrCoupIndx) - m_Tranches.at(h)->GetCashFlow().GetInterest(CurrentDate, CurrCoupIndx);
                                Solution += m_Tranches.at(h)->GetCashFlow().GetAmountOutstanding(CurrentDate);
                                ProRataBonds.enqueue(h);
                            }
                        }
                        if (TotalPayable >= 0.0) {
                            if (AvailableInterest >= TotalPayable) {
                                //Enough funds to pay all interest
                                foreach(qint32 h, ProRataBonds)
                                {
                                    m_Tranches[h]->AddCashFlow(CurrentDate
                                        , m_Tranches.at(h)->GetCashFlow().GetAccrued(CurrentDate, CurrCoupIndx) - m_Tranches.at(h)->GetCashFlow().GetInterest(CurrentDate, CurrCoupIndx)
                                        , static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow) | CurrCoupIndx
                                        );
                                }
                                ProRataBonds.clear();
                                AvailableInterest -= TotalPayable;
                            }
                            else if (Solution>0.0) {
                                //Pay pro-rata based on outstanding
                                foreach(qint32 h, ProRataBonds)
                                {
                                    m_Tranches[h]->AddCashFlow(CurrentDate
                                        , AvailableInterest * m_Tranches.at(h)->GetCashFlow().GetAmountOutstanding(CurrentDate) / Solution
                                        , static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow) | CurrCoupIndx
                                        );
                                }
                                ProRataBonds.clear();
                                AvailableInterest = 0.0;
                            }
                            else {
                                //Pay pro-rata in equal parts
                                LOGDEBUG("Reached Pay Interest on 0 Outstanding");
                                foreach(qint32 h, ProRataBonds)
                                {
                                    m_Tranches[h]->AddCashFlow(CurrentDate
                                        , AvailableInterest / static_cast<double>(ProRataBonds.size())
                                        , static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow) | CurrCoupIndx
                                        );
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
                    for (int h = 0; h < m_Tranches.size(); h++) {
                        if (m_Tranches.at(h)->GetProrataGroup(CurrSenGrpLvl) == CurrSenGrp) {
                            if (!m_Tranches.at(h)->HasCoupon(CurrCoupIndx)) {
                                PrintToTempFile("ReturnFalse.txt", m_Tranches.at(h)->GetTrancheName() + " - Coupon not set in tranche");
                                return false;
                            }
                            ProRataBonds.enqueue(h);
                            TotalPayable +=
                                m_Tranches.at(h)->GetCashFlow().GetAccrued(CurrentDate, CurrCoupIndx)
                                + m_Tranches.at(h)->GetCashFlow().GetDeferred(CurrentDate, CurrCoupIndx)
                                - m_Tranches.at(h)->GetCashFlow().GetInterest(CurrentDate, CurrCoupIndx);
                            ;
                            Solution += m_Tranches.at(h)->GetCashFlow().GetAmountOutstanding(CurrentDate);
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
                                m_Tranches[h]->AddCashFlow(CurrentDate,
                                    m_Tranches.at(h)->GetCashFlow().GetAccrued(CurrentDate, CurrCoupIndx)
                                    + m_Tranches.at(h)->GetCashFlow().GetDeferred(CurrentDate, CurrCoupIndx)
                                    - m_Tranches.at(h)->GetCashFlow().GetInterest(CurrentDate, CurrCoupIndx)
                                    , static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow) | CurrCoupIndx);
                            }
                        }
                        else if (Solution > 0.0) {
                            //Pro-rata based on outstanding
                            foreach(qint32 h, ProRataBonds)
                            {
                                m_Tranches[h]->AddCashFlow(CurrentDate, FundsToUse*m_Tranches.at(h)->GetCashFlow().GetAmountOutstanding(CurrentDate) / Solution, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow) | CurrCoupIndx);
                            }
                        }
                        else {
                            //Pro-rata equal parts
                            LOGDEBUG("Reached Pay Deferred on 0 Outstanding");
                            foreach(qint32 h, ProRataBonds)
                            {
                                m_Tranches[h]->AddCashFlow(CurrentDate, FundsToUse / static_cast<double>(ProRataBonds.size()), static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow) | CurrCoupIndx);
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
                    for (int h = 0; h < m_Tranches.size(); h++) {
                        if (m_Tranches.at(h)->GetProrataGroup(CurrSenGrpLvl) == CurrSenGrp) {
                            ProRataBonds.enqueue(h);
                            TotalPayable += m_Tranches.at(h)->GetCashFlow().GetAmountOutstanding(CurrentDate);
                        }
                    }
                    if (TotalPayable < 0.01) {
                        TotalPayable = 0.0;
                        UsedOriginals = true;
                        for (auto ProRataIte = ProRataBonds.constBegin(); ProRataIte != ProRataBonds.constEnd(); ++ProRataIte) {
                            TotalPayable += m_Tranches.at(*ProRataIte)->GetOriginalAmount();
                        }
                    }
                    Solution = qMin(AvailableInterest, m_MortgagesPayments.GetPrepayFees(CurrentDate)*SingleStep->GetParameter(WatFalPrior::wstParameters::RedemptionShare).value<BloombergVector>().GetValue(CurrentDate));
                    AvailableInterest -= Solution;
                    int OriginalProRataBondsSize = ProRataBonds.size();
                    while (ProRataBonds.size() > 0) {
                        int ProrataIndex = ProRataBonds.dequeue();
                        if (TotalPayable > 0) {
                            if (UseToRedeem)
                                Solution = RedeemNotes(Solution, CurrSenGrp, CurrSenGrpLvl, CurrentDate);
                            m_Tranches[ProrataIndex]->AddCashFlow(CurrentDate, Solution*(UsedOriginals ? m_Tranches.at(ProrataIndex)->GetOriginalAmount() : m_Tranches.at(ProrataIndex)->GetCashFlow().GetAmountOutstanding(CurrentDate)) / TotalPayable, TrancheCashFlow::TrancheFlowType::InterestFlow);
                        }
                        else {
                            m_Tranches[ProrataIndex]->AddCashFlow(CurrentDate, Solution / OriginalProRataBondsSize, TrancheCashFlow::TrancheFlowType::InterestFlow);
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
                        m_AnnualizedExcess.AddFlow(CurrentDate, AvailableInterest, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow));
                        m_EquityIncome.AddFlow(CurrentDate, AvailableInterest, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow));
                    }
                    if (SolutionDegree >= 2) {
                        m_EquityIncome.AddFlow(CurrentDate, AvailablePrincipal.Total(), static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PrincipalFlow));
                    }
                    if (TestTarget > 0.0) {
                        ProRataBonds.clear();
                        TotalPayable = 0.0;
                        for (int h = 0; h < m_Tranches.size(); h++) {
                            if (m_Tranches.at(h)->GetProrataGroup(CurrSenGrpLvl) == CurrSenGrp) {
                                ProRataBonds.enqueue(h);
                                TotalPayable += m_Tranches.at(h)->GetOriginalAmount();
                            }
                        }
                        {
                            int OriginalProRataBondsSize = ProRataBonds.size();
                            while (ProRataBonds.size()>0) {
                                int ProrataIndex = ProRataBonds.dequeue();
                                if (TotalPayable > 0) {
                                    if (SolutionDegree & 1) m_Tranches[ProrataIndex]->AddCashFlow(CurrentDate, AvailableInterest*TestTarget*m_Tranches.at(ProrataIndex)->GetOriginalAmount() / TotalPayable, TrancheCashFlow::TrancheFlowType::InterestFlow);
                                    if (SolutionDegree >= 2) {
                                        double TmpPrincEx = TestTarget*AvailablePrincipal.Total()*m_Tranches.at(ProrataIndex)->GetOriginalAmount() / TotalPayable;
                                        m_Tranches[ProrataIndex]->AddCashFlow(CurrentDate, TmpPrincEx, TrancheCashFlow::TrancheFlowType::PrincipalFlow);
                                        m_Tranches[ProrataIndex]->AddCashFlow(CurrentDate, -qMin(m_Tranches[ProrataIndex]->GetCashFlow().GetAmountOutstanding(CurrentDate), TmpPrincEx), TrancheCashFlow::TrancheFlowType::AmountOutstandingFlow);
                                    }
                                }
                                else {
                                    if (SolutionDegree & 1) m_Tranches[ProrataIndex]->AddCashFlow(CurrentDate, AvailableInterest*TestTarget / OriginalProRataBondsSize, TrancheCashFlow::TrancheFlowType::InterestFlow);
                                    if (SolutionDegree >= 2) {
                                        m_Tranches[ProrataIndex]->AddCashFlow(CurrentDate, TestTarget*AvailablePrincipal.Total() / OriginalProRataBondsSize, TrancheCashFlow::TrancheFlowType::PrincipalFlow);
                                        m_Tranches[ProrataIndex]->AddCashFlow(CurrentDate, -qMin(m_Tranches[ProrataIndex]->GetCashFlow().GetAmountOutstanding(CurrentDate), TestTarget*AvailablePrincipal.Total() / OriginalProRataBondsSize), TrancheCashFlow::TrancheFlowType::AmountOutstandingFlow);
                                    }
                                }
                            }
                        }
                    }

                    if (SolutionDegree >= 2) m_ExcessCashFlow.AddFlow(CurrentDate, (1.0 - TestTarget)*AvailablePrincipal.Total(), static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PrincipalFlow));
                    if (SolutionDegree & 1) m_ExcessCashFlow.AddFlow(CurrentDate, (1.0 - TestTarget)* AvailableInterest, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow));

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
                    for (int h = 0; h < m_Tranches.size(); h++) {
                        if (m_Tranches.at(h)->GetProrataGroup(CurrSenGrpLvl) <= CurrSenGrp) {
                            TotalPayable += m_Tranches.at(h)->GetCashFlow().GetAmountOutstanding(CurrentDate);
                            if (m_Tranches.at(h)->GetProrataGroup(CurrSenGrpLvl) == CurrSenGrp) {
                                TestTarget += m_Tranches.at(h)->GetCashFlow().GetAmountOutstanding(CurrentDate);
                                ProRataBonds.enqueue(h);
                            }
                        }
                    }
                    Solution = qMin(TotalPayable - m_MortgagesPayments.GetAmountOut(CurrentDate) - AvailablePrincipal.Total(), TestTarget);
                    if (Solution >= 0.01) {
                        foreach(const int& SingleBond, ProRataBonds)
                        {
                            m_Tranches[SingleBond]->SetCashFlow(CurrentDate, qMax(0.0, Solution - AvailableInterest)* m_Tranches.at(SingleBond)->GetCashFlow().GetAmountOutstanding(CurrentDate) / TestTarget, TrancheCashFlow::TrancheFlowType::PDLOutstanding);
                            m_Tranches[SingleBond]->AddCashFlow(CurrentDate, qMin(AvailableInterest, Solution)* m_Tranches.at(SingleBond)->GetCashFlow().GetAmountOutstanding(CurrentDate) / TestTarget, TrancheCashFlow::TrancheFlowType::PDLCured);
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

                    for (int h = 0; h<m_Tranches.size(); h++) {
                        if (m_Tranches.at(h)->GetProrataGroup(CurrSenGrpLvl) <= CurrSenGrp) {
                            TotalPayable += m_Tranches.at(h)->GetCashFlow().GetAmountOutstanding(CurrentDate);
                            if (m_Tranches.at(h)->GetProrataGroup(CurrSenGrpLvl) == CurrSenGrp) ProRataBonds.enqueue(h);
                        }
                    }
                    //CCC test
                    if (m_CCCcurve.GetValue(CurrentDate)>m_CCCTestLimit)
                        Solution = (1.0 - ((m_CCCcurve.GetValue(CurrentDate) - m_CCCTestLimit)*m_CCChaircut))*m_MortgagesPayments.GetOutstandingForOC(CurrentDate);
                    else
                        Solution = m_MortgagesPayments.GetOutstandingForOC(CurrentDate);
                    Solution += AvailablePrincipal.Total() + m_ReinvestmentTest.GetQueuedCash();
                    if (Solution == 0.0) Solution = 1.0;
                    TotalPayable = qMax(TotalPayable, 0.000001);
                    if (SingleStep->GetParameter(WatFalPrior::wstParameters::PayAccrue).toInt() & static_cast<quint8>(WatFalPrior::wstAccrueOrPay::Accrue)) {
                        for (; ProRataBonds.size() > 0; ProRataBonds.dequeue()) {
                            if (m_Tranches[ProRataBonds.head()]->GetCashFlow().GetOCTest(CurrentDate) == 0.0) {
                                m_Tranches[ProRataBonds.head()]->SetCashFlow(CurrentDate, Solution / TotalPayable, TrancheCashFlow::TrancheFlowType::OCFlow);
                            }
                        }
                    }
                    if (SingleStep->GetParameter(WatFalPrior::wstParameters::PayAccrue).toInt() & static_cast<quint8>(WatFalPrior::wstAccrueOrPay::Pay)) {
                        ProRataBonds.clear();
                        TestTarget = -1.0;
                        if (SingleStep->HasParameter(WatFalPrior::wstParameters::TestTargetOverride)) {
                            TestTarget = SingleStep->GetParameter(WatFalPrior::wstParameters::TestTargetOverride).value<BloombergVector>().GetValue(CurrentDate);
                        }
                        for (auto h = m_Tranches.begin(); h != m_Tranches.end(); ++h) {
                            if ((*h)->GetProrataGroup(CurrSenGrpLvl) == CurrSenGrp) {
                                if (TestTarget <= 0.0) TestTarget = (*h)->GetMinOClevel();
                                double CurrTrg = (*h)->GetCashFlow().GetFlow(CurrentDate, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::OCTarget));
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
                                    m_ReinvestmentTest.QueueReinvestments(FundsToCollateral, CurrentDate);
                                    const MtgCashFlow& ReinvFlows = m_ReinvestmentTest.ProcessQueue(CurrentDate, CurrentPeriodIndex, m_LegalFinal);
                                    if (!ReinvFlows.IsEmpty()) {
                                        AvailableInterest += ReinvFlows.GetInterest(CurrentDate);
                                        AvailablePrincipal.AddScheduled(ReinvFlows.GetScheduled(CurrentDate));
                                        AvailablePrincipal.AddPrepay(ReinvFlows.GetPrepay(CurrentDate));
                                        m_ReinvestmentTest.RemoveBondFlow(CurrentDate);
                                        m_MortgagesPayments.AddFlow(m_ReinvestmentTest.GetBondCashFlow());
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
                    adjSeniorExpenses = AdjustCoupon(m_SeniorExpenses.GetValue(CurrentDate), RollingNextIPD, RollingNextIPD.addMonths(m_PaymentFrequency.GetValue(RollingNextIPD)), m_DealDayCountConvention.GetValue(CurrentDate));
                    adjSeniorFees = AdjustCoupon(m_SeniorFees.GetValue(CurrentDate), RollingNextIPD, RollingNextIPD.addMonths(m_PaymentFrequency.GetValue(RollingNextIPD)), m_DealDayCountConvention.GetValue(CurrentDate));
                    Solution =
                        m_InterestAvailable
                        + m_MortgagesPayments.GetAccruedInterest(CurrentDate)
                        - ((adjSeniorFees + adjSeniorExpenses)*(CurrentAssetSum / static_cast<double>(CurrentAssetCount)))
                        - m_SeniorExpensesFixed.GetValue(CurrentDate)*static_cast<double>(m_PaymentFrequency.GetValue(CurrentDate)) / 12.0
                        - m_SeniorFeesFixed.GetValue(CurrentDate)*static_cast<double>(m_PaymentFrequency.GetValue(CurrentDate)) / 12.0
                        ;
                    for (int h = 0; h < m_Tranches.size(); h++) {
                        if (m_Tranches.at(h)->GetProrataGroup(CurrSenGrpLvl) <= CurrSenGrp) {
                            if (!m_Tranches.at(h)->HasCoupon(CurrCoupIndx)) {
                                PrintToTempFile("ReturnFalse.txt", m_Tranches.at(h)->GetTrancheName() + " - Coupon not set in tranche");
                                return false;
                            }
                            AdjustedCoupon = AdjustCoupon((m_Tranches.at(h)->GetCoupon(CurrentDate, CurrCoupIndx)), RollingNextIPD, RollingNextIPD.addMonths(m_PaymentFrequency.GetValue(RollingNextIPD)), m_Tranches.at(h)->GetDayCount(CurrCoupIndx).GetValue(CurrentDate));
                            TotalPayable += AdjustedCoupon*(m_Tranches.at(h)->GetCashFlow().GetAmountOutstanding(CurrentDate) + m_Tranches.at(h)->GetCashFlow().GetFlow(CurrentDate, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::DeferredFlow) | CurrCoupIndx));
                            if (m_Tranches.at(h)->GetProrataGroup(CurrSenGrpLvl) == CurrSenGrp)
                                ProRataBonds.enqueue(h);
                        }
                    }
                    TotalPayable = qMax(TotalPayable, 0.000001);
                    if (SingleStep->GetParameter(WatFalPrior::wstParameters::PayAccrue).toInt() & static_cast<quint8>(WatFalPrior::wstAccrueOrPay::Accrue)) {
                        for (; !ProRataBonds.isEmpty(); ProRataBonds.dequeue()) {
                            if (m_Tranches[ProRataBonds.head()]->GetCashFlow().GetICTarget(CurrentDate) == 0.0) {
                                m_Tranches[ProRataBonds.head()]->SetCashFlow(CurrentDate, qMax(0.0, Solution / TotalPayable), TrancheCashFlow::TrancheFlowType::ICFlow);
                            }
                        }
                    }
                    if (SingleStep->GetParameter(WatFalPrior::wstParameters::PayAccrue).toInt() & static_cast<quint8>(WatFalPrior::wstAccrueOrPay::Pay)) {
                        ProRataBonds.clear();
                        TestTarget = -1.0;
                        if (SingleStep->HasParameter(WatFalPrior::wstParameters::TestTargetOverride)) {
                            TestTarget = SingleStep->GetParameter(WatFalPrior::wstParameters::TestTargetOverride).value<BloombergVector>().GetValue(CurrentDate);
                        }
                        for (auto h = m_Tranches.begin(); h != m_Tranches.end(); ++h) {
                            if ((*h)->GetProrataGroup(CurrSenGrpLvl) == CurrSenGrp) {
                                if (TestTarget <= 0.0) TestTarget = (*h)->GetMinIClevel();
                                double CurrTest = (*h)->GetCashFlow().GetFlow(CurrentDate, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::ICTarget));
                                if (CurrTest <= 0.0 || CurrTest < TestTarget)
                                    (*h)->SetCashFlow(CurrentDate, TestTarget, TrancheCashFlow::TrancheFlowType::ICTarget);
                            }
                        }
                        if (TotalPayable != 0.0 && TestTarget > 0.0) {
                            if (
                                TotalPayable < TestTarget //if it fails,
                                && (// and you have funds available 
                                (AvailablePrincipal.Total()>0.0 && CurrSourceOfFunding == 2)
                                || (AvailableInterest && CurrSourceOfFunding == 1)
                                )
                                ) { //redeem senior notes until cured								
                                SolutionDegree = 1;
                                InterestPayableBefore = (m_InterestAvailable + m_MortgagesPayments.GetAccruedInterest(CurrentDate) - ((adjSeniorExpenses + adjSeniorFees) * (CurrentAssetSum / static_cast<double>(CurrentAssetCount))) - ((m_SeniorExpensesFixed.GetValue(CurrentDate) + m_SeniorFeesFixed.GetValue(CurrentDate))*static_cast<double>(m_PaymentFrequency.GetValue(CurrentDate)) / 12.0)) / TestTarget;
                                TotalPayable = 0.0;
                                bool SolutionFound;
                                do {
                                    SolutionFound = true;
                                    Solution = 0.0;
                                    for (int h = 0; h<m_Tranches.size(); h++) {
                                        if (m_Tranches.at(h)->GetProrataGroup(CurrSenGrpLvl) <= CurrSenGrp && m_Tranches.at(h)->GetProrataGroup(CurrSenGrpLvl) >= SolutionDegree) {
                                            AdjustedCoupon = AdjustCoupon((m_Tranches.at(h)->GetCoupon(CurrentDate, CurrCoupIndx)), RollingNextIPD, RollingNextIPD.addMonths((m_PaymentFrequency.GetValue(RollingNextIPD))), m_Tranches.at(h)->GetDayCount(CurrCoupIndx).GetValue(CurrentDate));
                                            Solution += AdjustedCoupon*(m_Tranches.at(h)->GetCashFlow().GetAmountOutstanding(CurrentDate) + m_Tranches.at(h)->GetCashFlow().GetFlow(CurrentDate, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::DeferredFlow) | CurrCoupIndx));
                                        }
                                    }
                                    if (GroupWACoupon(SolutionDegree, CurrSenGrpLvl, CurrentDate, CurrCoupIndx)>0.0)
                                        TotalPayable = (Solution - InterestPayableBefore) / GroupWACoupon(SolutionDegree, CurrSenGrpLvl, CurrentDate, CurrCoupIndx);
                                    if (TotalPayable > GroupOutstanding(SolutionDegree, CurrSenGrpLvl, CurrentDate) || GroupWACoupon(SolutionDegree, CurrSenGrpLvl, CurrentDate, CurrCoupIndx) <= 0) {
                                        if (++SolutionDegree <= CurrSenGrp) SolutionFound = false;
                                        else TotalPayable = 0;
                                    }
                                    if (SolutionFound) {
                                        for (int h = 1; h < SolutionDegree; h++) TotalPayable += GroupOutstanding(h, CurrSenGrpLvl, CurrentDate);
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
                            m_ReinvestmentTest.QueueReinvestments(PayablePrincipal, CurrentDate);
                            const MtgCashFlow& ReinvFlows = m_ReinvestmentTest.ProcessQueue(CurrentDate, CurrentPeriodIndex, m_LegalFinal);
                            if (!ReinvFlows.IsEmpty()) {
                                AvailableInterest += ReinvFlows.GetInterest(CurrentDate);
                                AvailablePrincipal.AddScheduled(ReinvFlows.GetScheduled(CurrentDate));
                                AvailablePrincipal.AddPrepay(ReinvFlows.GetPrepay(CurrentDate));
                                m_ReinvestmentTest.RemoveBondFlow(CurrentDate);
                                m_MortgagesPayments.AddFlow(m_ReinvestmentTest.GetBondCashFlow());
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
                    m_TotalJuniorFees.GetFlow(CurrentDate, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::DeferredFlow))
                    + m_TotalJuniorFees.GetFlow(CurrentDate, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::AccruedFlow))
                    - m_TotalJuniorFees.GetTotalFlow(CurrentDate, QList<qint32>() << static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow) << static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PrincipalFlow))
                    ;
                if (NewDeferred>0.0) m_TotalJuniorFees.SetFlow(CurrentDate, NewDeferred, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::DeferredFlow));
                foreach(Tranche* SingleTranche, m_Tranches)
                {
                    for (qint32 CoupIdx = 0; CoupIdx<(1 << MaximumInterestsTypes); ++CoupIdx) {
                        if (SingleTranche->HasCoupon(CoupIdx)) {
                            NewDeferred =
                                SingleTranche->GetCashFlow().GetDeferred(CurrentDate, CoupIdx)
                                + SingleTranche->GetCashFlow().GetAccrued(CurrentDate, CoupIdx)
                                - SingleTranche->GetCashFlow().GetInterest(CurrentDate, CoupIdx);
                            if (qAbs(NewDeferred) < 0.01)NewDeferred = 0.0;
                            if (NewDeferred >= 0.0)
                                SingleTranche->SetCashFlow(CurrentDate, NewDeferred, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::DeferredFlow) | CoupIdx);
                        }
                    }
                }
            }
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //If, after applying the waterfall, there are still funds available, apply them to XS
            if (AvailablePrincipal.Total()>0.0) m_ExcessCashFlow.AddFlow(CurrentDate, AvailablePrincipal.Total(), static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PrincipalFlow));
            if (AvailableInterest>0.0) m_ExcessCashFlow.AddFlow(CurrentDate, AvailableInterest, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow));
            AvailablePrincipal.Erase();
            AvailableInterest = 0.0;
            m_PrincipalAvailable.Erase();
            m_InterestAvailable = 0.0;
            CurrentAssetSum = 0.0;
            CurrentAssetCount = 0;
            RollingLastIPD = RollingNextIPD;
            RollingNextIPD = RollingNextIPD.addMonths(m_PaymentFrequency.GetValue(RollingNextIPD));
            if (IsCallPaymentDate) {
                //i++;
                break;
            }
        }//End Cycle in time
        for (int j = 0; j<m_Reserves.size(); j++) {
            m_Reserves[j]->SetReserveFundCurrent(m_Reserves[j]->GetStartingReserve());
            m_Reserves[j]->ResetMissingAnchors();
        }
        m_StartingDeferredJunFees = OriginalStartingDefJunFees;
        m_PrincipalAvailable = OriginalAvailablePrincipal;
        m_InterestAvailable = OriginalAvailableInterest;
        m_CalculatedMtgPayments = m_MortgagesPayments;
        m_MortgagesPayments = OriginalMtgFlows;
        FillAllDates();
        if (NullCCCanchor[0]) m_CCCcurve.RemoveAnchorDate();
        if (NullCCCanchor[1]) m_PaymentFrequency.RemoveAnchorDate();
        if (NullCCCanchor[2]) m_SeniorExpenses.RemoveAnchorDate();
        if (NullCCCanchor[3]) m_SeniorFees.RemoveAnchorDate();
        if (NullCCCanchor[4]) m_JuniorFees.RemoveAnchorDate();
        if (NullCCCanchor[5]) m_SeniorExpensesFixed.RemoveAnchorDate();
        if (NullCCCanchor[6]) m_SeniorFeesFixed.RemoveAnchorDate();
        if (NullCCCanchor[7]) m_JuniorFeesFixed.RemoveAnchorDate();
        if (NullCCCanchor[8]) m_GICinterest.RemoveAnchorDate();
        if (NullCCCanchor[9]) m_GICBaseRateValue.RemoveAnchorDate();
        foreach(WatFalPrior* SingleStp, m_WaterfallStesps)
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
        foreach(ReserveFund* SingleRes, m_Reserves)
        {
            CheckResults -= SingleRes->GetStartingReserve();
            SingleRes->ClearFlows();
        }
        LOGDEBUG(QString("After Reserve Funds:\t%1").arg(CheckResults, 0, 'f'));
        CheckMtgCashFlow.Clear();
        if (ActualCallDate.isNull()) {
            CheckMtgCashFlow.AddFlow(m_CalculatedMtgPayments);
        }
        else {
            for (int MtgFlwIter = 0; MtgFlwIter <= m_CalculatedMtgPayments.Count(); ++MtgFlwIter) {
                CheckMtgCashFlow.AddFlow(m_CalculatedMtgPayments.SingleDate(m_CalculatedMtgPayments.GetDate(MtgFlwIter)));
                if (m_CalculatedMtgPayments.GetDate(MtgFlwIter) >= ActualCallDate) {
                    if (m_PoolValueAtCall.IsEmpty())
                        TotalPayable = m_CalculatedMtgPayments.GetWAPrice(MtgFlwIter);
                    else
                        TotalPayable = m_PoolValueAtCall.GetValue(m_CalculatedMtgPayments.GetDate(MtgFlwIter));
                    CheckMtgCashFlow.AddFlow(ActualCallDate, TotalPayable*m_CalculatedMtgPayments.GetAmountOut(MtgFlwIter), MtgCashFlow::MtgFlowType::PrincipalFlow);
                    break;
                }
            }
        }
        LOGDEBUG(QString("After Call Proceeds:\t%1").arg(CheckResults, 0, 'f'));
        CheckResults -= CheckMtgCashFlow.GetTotalFlow(0);
        LOGDEBUG(QString("After Loans Flows:\t%1").arg(CheckResults, 0, 'f'));
        CheckTranCashFlow.Clear(); CheckTranCashFlow.AddFlow(m_GICflows); CheckResults -= CheckTranCashFlow.GetTotalFlow(0);
        LOGDEBUG(QString("After GIC:\t%1").arg(CheckResults, 0, 'f'));
        CheckResults -= m_InterestAvailable;
        LOGDEBUG(QString("After Interest:\t%1").arg(CheckResults, 0, 'f'));
        CheckResults -= m_PrincipalAvailable.Total();
        LOGDEBUG(QString("After Principal:\t%1").arg(CheckResults, 0, 'f'));
        // Use of funds
        CheckTranCashFlow.Clear(); CheckTranCashFlow.AddFlow(m_ReinvestmentTest.GetReinvested()); CheckResults += CheckTranCashFlow.GetTotalFlow(0);
        LOGDEBUG(QString("After Reinvestments:\t%1").arg(CheckResults, 0, 'f'));
        CheckTranCashFlow.Clear(); CheckTranCashFlow.AddFlow(m_TotalSeniorExpenses); CheckResults += CheckTranCashFlow.GetTotalFlow(0);
        LOGDEBUG(QString("After Senior Expenses:\t%1").arg(CheckResults, 0, 'f'));
        CheckTranCashFlow.Clear(); CheckTranCashFlow.AddFlow(m_TotalSeniorFees); CheckResults += CheckTranCashFlow.GetTotalFlow(0);
        LOGDEBUG(QString("After Senior Fees:\t%1").arg(CheckResults, 0, 'f'));
        CheckTranCashFlow.Clear(); CheckTranCashFlow.AddFlow(m_TotalJuniorFees); CheckResults += CheckTranCashFlow.GetTotalFlow(0);
        LOGDEBUG(QString("After Junior Fees:\t%1").arg(CheckResults, 0, 'f'));
        CheckTranCashFlow.Clear(); CheckTranCashFlow.AddFlow(m_ExcessCashFlow); CheckResults += CheckTranCashFlow.GetTotalFlow(0);
        LOGDEBUG(QString("After Excess Spread:\t%1").arg(CheckResults, 0, 'f'));
        foreach(const Tranche* SingleTranche, m_Tranches)
        {
            CheckTranCashFlow.Clear(); CheckTranCashFlow.AddFlow(SingleTranche->GetCashFlow());	CheckResults += CheckTranCashFlow.GetTotalFlow(0);
            LOGDEBUG(QString("After Tranche %1:\t%2").arg(SingleTranche->GetTrancheName()).arg(CheckResults, 0, 'f'));
        }
        LOGDEBUG(QString("Final Test\t%1").arg(CheckResults, 0, 'f'));
        if (qAbs(CheckResults) >= 1.0) {
            PrintToTempFile("ReturnFalse.txt", QString("Cash Flows don't Match, Diff: %1").arg(CheckResults, 0, 'f'));
#ifndef DebugLogging
            return false;
#endif // !DebugLogging
        }
        return true;
}

QDataStream& operator<<(QDataStream & stream, const Waterfall& flows)
{
    stream
        << flows.m_CalledPeriod
        << flows.m_TriggersResults
        << flows.m_SeniorExpenses
        << flows.m_SeniorFees
        << flows.m_JuniorFees
        << flows.m_SeniorExpensesFixed
        << flows.m_SeniorFeesFixed
        << flows.m_JuniorFeesFixed
        << flows.m_MortgagesPayments
        << flows.m_CalculatedMtgPayments
        << flows.m_PaymentFrequency
        << flows.m_ExcessCashFlow
        << flows.m_TotalSeniorExpenses
        << flows.m_TotalSeniorFees
        << flows.m_TotalJuniorFees
        << flows.m_ReinvestmentTest
        << flows.m_CCCTestLimit
        << flows.m_CCCcurve
        << flows.m_CCChaircut
        << flows.m_PrincipalAvailable
        << flows.m_InterestAvailable
        << flows.m_JuniorFeesCoupon
        << flows.m_AnnualizedExcess
        << flows.m_EquityIncome
        << flows.m_FirstIPDdate
        << flows.m_LastIPDdate
        << flows.m_CallDate
        << flows.m_LegalFinal
        << flows.m_PoolValueAtCall
        << flows.m_UseCall
        << flows.m_IsStressTest
        << flows.m_CallMultiple
        << flows.m_CallReserve
        << flows.m_DealName
        << flows.m_GICinterest
        << flows.m_GICBaseRate
        << flows.m_GICBaseRateValue
        << flows.m_DealDayCountConvention
        << flows.m_StartingDeferredJunFees
        << flows.m_GICflows
        << static_cast<qint32>(flows.m_WaterfallStesps.size());
    ;
    foreach(const WatFalPrior* SingleStep, flows.m_WaterfallStesps)
        stream << (*SingleStep);
    stream << static_cast<qint32>(flows.m_Triggers.size() - 1);
    for (auto i = flows.m_Triggers.constBegin(); i != flows.m_Triggers.constEnd(); ++i) {
        if (i.key() == 0) continue; //Reinvestment period is saved already
        stream << static_cast<quint8>(i.value()->GetTriggerType()) << i.key() << *(i.value());
    }
    stream << static_cast<qint32>(flows.m_Tranches.size());
    foreach(const Tranche* SingleTranche, flows.m_Tranches)
        stream << (*SingleTranche);
    stream << flows.m_CumulativeReserves << static_cast<qint32>(flows.m_Reserves.size());
    for (QList<ReserveFund*>::const_iterator ResIter = flows.m_Reserves.constBegin(); ResIter != flows.m_Reserves.constEnd(); ResIter++)
        stream << (**ResIter);
    return stream;
}
QDataStream& operator>>(QDataStream & stream, Waterfall& flows)
{
    return flows.LoadOldVersion(stream);
}
QDataStream& Waterfall::LoadOldVersion(QDataStream& stream)
{
    double TempDouble;
    QString TempString;
    qint32 TempInt;
    qint16 TemShort;
    Tranche TempTranche;
    ReserveFund TempReserve;
    WatFalPrior TempStep;
    stream >> m_CalledPeriod;
    m_TriggersResults.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_TriggersResults;
    m_SeniorExpenses.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_SeniorExpenses;
    m_SeniorFees.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_SeniorFees;
    m_JuniorFees.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_JuniorFees;
    m_SeniorExpensesFixed.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_SeniorExpensesFixed;
    m_SeniorFeesFixed.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_SeniorFeesFixed;
    m_JuniorFeesFixed.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_JuniorFeesFixed;
    m_MortgagesPayments.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_MortgagesPayments;
    m_CalculatedMtgPayments.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_CalculatedMtgPayments;
    m_PaymentFrequency.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_PaymentFrequency;
    m_ExcessCashFlow.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_ExcessCashFlow;
    m_TotalSeniorExpenses.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_TotalSeniorExpenses;
    m_TotalSeniorFees.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_TotalSeniorFees;
    m_TotalJuniorFees.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_TotalJuniorFees;
    m_ReinvestmentTest.SetLoadProtocolVersion(m_LoadProtocolVersion);  stream >> m_ReinvestmentTest;
    ResetTriggers();
    SetReinvestementPeriod(m_ReinvestmentTest.GetReinvestmentPeriod());
    stream >> m_CCCTestLimit;
    m_CCCcurve.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_CCCcurve;
    stream >> m_CCChaircut;
    m_PrincipalAvailable.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_PrincipalAvailable;
    stream
        >> m_InterestAvailable
        >> m_JuniorFeesCoupon
        ;
    m_AnnualizedExcess.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_AnnualizedExcess;
    m_EquityIncome.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_EquityIncome;
    stream >> m_FirstIPDdate;
    stream >> m_LastIPDdate >> m_CallDate >> m_LegalFinal;
    m_PoolValueAtCall.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_PoolValueAtCall;
    stream >> m_UseCall
        >> m_IsStressTest
        >> m_CallMultiple
        >> m_CallReserve
        >> m_DealName;
    m_GICinterest.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_GICinterest;
    m_GICBaseRate.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_GICBaseRate;
    m_GICBaseRateValue.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_GICBaseRateValue;
    m_DealDayCountConvention.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_DealDayCountConvention;
    stream >> m_StartingDeferredJunFees;
    m_GICflows.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_GICflows;
    stream >> TempInt;

    ResetSteps();
    for (int i = 0; i < TempInt; i++) {
        TempStep.ClearParameters();
        TempStep.SetLoadProtocolVersion(m_LoadProtocolVersion);
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
                Q_ASSERT_X(false, "Waterfall::LoadOldVersion", "Unhadled Trigger Type");
            }
            TempTrig->SetLoadProtocolVersion(m_LoadProtocolVersion);
            stream >> (*TempTrig);
            m_Triggers.insert(TempKey, TempTrig);
        }
    }

    stream >> TempInt;
    ResetTranches();
    for (int i = 0; i < TempInt; i++) {
        TempTranche.SetLoadProtocolVersion(m_LoadProtocolVersion);
        stream >> TempTranche;
        AddTranche(TempTranche);
    }
    stream >> m_CumulativeReserves;
    stream >> TempInt;
    RemoveReserve();
    for (int i = 0; i < TempInt; i++) {
        TempReserve.SetLoadProtocolVersion(m_LoadProtocolVersion);
        stream >> TempReserve;
        m_Reserves.append(new ReserveFund(TempReserve));
    }
    ResetProtocolVersion();
    return stream;
}
const QDate& Waterfall::GetCalledPeriod() const
{
    return m_CalledPeriod;
}
QString Waterfall::ReadyToCalculate()const
{
    QString Result;
    if (m_DealDayCountConvention.IsEmpty()) Result += "Deal Day Count Convention\n";
    if (m_GICinterest.IsEmpty()) Result += "GIC Interest\n";
    if (m_GICBaseRate.IsEmpty()) Result += "GIC Base Rate\n";
    if (m_SeniorExpenses.IsEmpty(0.0)) Result += "Senior Expenses\n";
    if (m_SeniorFees.IsEmpty(0.0)) Result += "Senior Fees\n";
    if (m_JuniorFees.IsEmpty(0.0)) Result += "Junior Fees\n";
    if (m_SeniorExpensesFixed.IsEmpty(0.0)) Result += "Senior Expenses Fixed\n";
    if (m_SeniorFeesFixed.IsEmpty(0.0)) Result += "Senior Fees Fixed\n";
    if (m_JuniorFeesFixed.IsEmpty(0.0)) Result += "Junior Fees Fixed\n";
    if (m_Tranches.isEmpty()) Result += "Tranches\n";
    if (m_WaterfallStesps.isEmpty()) Result += "Waterfall Steps\n";
    if (m_MortgagesPayments.Count() == 0) Result += "Loans Payments\n";
    if (m_PaymentFrequency.IsEmpty(1)) Result += "IDP Frequency\n";
    if (BloombergVector(m_ReinvestmentTest.GetCDRAssumption()).IsEmpty(0.0, 1.0))Result += "CDR\n";
    if (BloombergVector(m_ReinvestmentTest.GetCPRAssumption()).IsEmpty(0.0, 1.0))Result += "CPR\n";
    if (BloombergVector(m_ReinvestmentTest.GetLSAssumption()).IsEmpty(0.0, 1.0))Result += "LS\n";
    if (BloombergVector(m_ReinvestmentTest.GetWALAssumption()).IsEmpty(0.0))Result += "Reinvestment Bond WAL\n";
    if (m_ReinvestmentTest.GetReinvDelay().IsEmpty(0.0))Result += "Reinvestment Delay\n";
    if (BloombergVector(m_ReinvestmentTest.GetReinvPrice()).IsEmpty(0.0))Result += "Reinvestment Price\n";
    if (m_ReinvestmentTest.GetReinvestmentPeriod() < QDate(2000, 1, 1))Result += "Reinvestment Period\n";
    if (m_ReinvestmentTest.GetReinvestmentBond().GetPaymentFreq().IsEmpty(1)) Result += "Reinvestment Bond Payment Frequency\n";
    if (m_ReinvestmentTest.GetReinvestmentBond().GetInterest().isEmpty())Result += "Reinvestment Bond Spread\n";
    if (m_ReinvestmentTest.GetReinvestmentSpreadOverTime().IsEmpty(1)) Result += "Reinvestment Spread Over Time\n";
    if (m_CCCcurve.IsEmpty(0.0, 1.0))Result += "CCC Curve\n";
    if (m_CCChaircut < 0.0 || m_CCChaircut>1.0)Result += "CCC Value\n";
    if (m_PoolValueAtCall.IsEmpty(0.0) && !m_PoolValueAtCall.IsEmpty()) Result += "Pool Value at Call\n";
    if (m_UseCall && m_CallDate < QDate(2000, 1, 1) && (m_CallReserve <= 0.0 || m_CallMultiple <= 0.0))Result += "Specify a call Criteria to use the Call\n";
    if (m_MortgagesPayments.Count() > 0 && m_MortgagesPayments.GetDate(0) < QDate(2000, 1, 1))Result += "Pool Cut Off Date\n";
    if (m_CCCTestLimit < 0.0 || m_CCChaircut>1.0)Result += "CCC Test Limit\n";
    if (m_FirstIPDdate < QDate(2000, 1, 1))Result += "Next IDP\n";
    if (m_LastIPDdate < QDate(2000, 1, 1))Result += "Last IDP\n";
    for (int ResIter = 0; ResIter < m_Reserves.size(); ResIter++) {
        if (m_Reserves.at(ResIter)->GetReserveFundTarget().IsEmpty(0.0)) Result += QString("Reserve %1 Target Level\n").arg(ResIter + 1);
        if (m_Reserves.at(ResIter)->GetReserveFundMultiple().IsEmpty(0.0)) Result += QString("Reserve %1 Multiple\n").arg(ResIter + 1);
        if (m_Reserves.at(ResIter)->GetReserveFundFloor().IsEmpty(0.0))Result += QString("Reserve %1 Floor Level\n").arg(ResIter + 1);
        if (m_Reserves.at(ResIter)->GetReserveFundFreed() < 0) Result += QString("Reserve %1 Freed After Redemption\n").arg(ResIter + 1);
        if (m_Reserves.at(ResIter)->GetReserveFundCurrent() < 0.0) Result += QString("Reserve %1 Current Amount\n").arg(ResIter + 1);
    }
    foreach(const Tranche* SingleTranche, m_Tranches)
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
    foreach(const WatFalPrior* SingleStep, m_WaterfallStesps)
    {
        Result += SingleStep->ReadyToCalculate();
        if (SingleStep->HasParameter(WatFalPrior::wstParameters::Trigger)) {
            if (!ValidTriggerStructure(SingleStep->GetParameter(WatFalPrior::wstParameters::Trigger).toString())) {
                Result += QString("%1 - Trigger Not Valid\n").arg(static_cast<quint32>(SingleStep->GetPriorityType()));
            }
        }
    }
    foreach(const QSharedPointer<AbstractTrigger>& SingleTrig, m_Triggers)
    {
        Result += SingleTrig->ReadyToCalculate();
    }
    if (!Result.isEmpty()) return Result.left(Result.size() - 1);
    return Result;
}
double Waterfall::GetEquityReturn(int index)const
{
    IntegerVector AdjPaymentFreq(m_PaymentFrequency);
    if (m_PaymentFrequency.GetAnchorDate().isNull()) AdjPaymentFreq.SetAnchorDate(m_MortgagesPayments.GetDate(0));
    if (index<0 || index >= m_EquityIncome.Count()) return 0.0;
    int EquityTranche;
    foreach(WatFalPrior* SingleStep, m_WaterfallStesps)
    {
        if (SingleStep->GetPriorityType() == WatFalPrior::WaterfallStepType::wst_Excess) {
            EquityTranche = SingleStep->GetParameter(WatFalPrior::wstParameters::RedemptionGroup).value<IntegerVector>().GetValue(m_EquityIncome.GetDate(index));
            break;
        }
    }
    double denominator = 0.0;
    if (EquityTranche>0) {
        foreach(Tranche* SingleTranche, m_Tranches)
        {
            if (SingleTranche->GetProrataGroup(0) == EquityTranche) denominator += SingleTranche->GetOriginalAmount();
        }
        if (denominator>0) return qPow(1.0 + ((m_EquityIncome.GetFlow(index, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow)) + m_EquityIncome.GetFlow(index, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PrincipalFlow))) / denominator), 12.0 / ((AdjPaymentFreq.GetValue(m_EquityIncome.GetDate(index))))) - 1.0;
        else return 0.0;
    }
    denominator = 0.0;
    foreach(Tranche* SingleTranche, m_Tranches)
    {
        denominator += SingleTranche->GetOriginalAmount();
    }
    denominator = m_CalculatedMtgPayments.GetAmountOut(0) - denominator;
    if (denominator>0) return qPow(1.0 + ((m_EquityIncome.GetFlow(index, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow)) + m_EquityIncome.GetFlow(index, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PrincipalFlow))) / denominator), 12.0 / ((AdjPaymentFreq.GetValue(m_EquityIncome.GetDate(index))))) - 1.0;
    else return 0.0;

}
double Waterfall::GetCumulativeEquityReturn(int index)const
{
    if (index<0 || index >= m_EquityIncome.Count()) return 0.0;
    double numerator = 0.0;
    for (int i = 0; i <= index; i++) numerator += (m_EquityIncome.GetFlow(i, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow)) + m_EquityIncome.GetFlow(i, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PrincipalFlow)));
    int EquityTranche;
    foreach(WatFalPrior* SingleStep, m_WaterfallStesps)
    {
        if (SingleStep->GetPriorityType() == WatFalPrior::WaterfallStepType::wst_Excess) {
            EquityTranche = SingleStep->GetParameter(WatFalPrior::wstParameters::RedemptionGroup).value<IntegerVector>().GetValue(m_EquityIncome.GetDate(index));
            break;
        }
    }
    double denominator = 0.0;
    if (EquityTranche>0) {
        foreach(Tranche* SingleTranche, m_Tranches)
        {
            if (SingleTranche->GetProrataGroup(0) == EquityTranche) denominator += SingleTranche->GetOriginalAmount();
        }
        if (denominator>0) return numerator / denominator;
        else return 0.0;
    }
    denominator = 0.0;
    foreach(Tranche* SingleTranche, m_Tranches)
    {
        denominator += SingleTranche->GetOriginalAmount();
    }
    denominator = m_CalculatedMtgPayments.GetAmountOut(0) - denominator;
    if (denominator>0) return numerator / denominator;
    else return 0.0;
}
double Waterfall::GetCallEquityRatio(int index)const
{
    if (index<0 || index >= m_Tranches.first()->GetCashFlow().Count()) return 0.0;
    QDate TargetDate = m_Tranches.first()->GetCashFlow().GetDate(index);
    int MtgIndex = -1;
    for (int i = 0; i<m_CalculatedMtgPayments.Count(); i++) {
        if (TargetDate.year() == m_CalculatedMtgPayments.GetDate(i).year() && TargetDate.month() == m_CalculatedMtgPayments.GetDate(i).month()) {
            MtgIndex = i;
            break;
        }
    }
    if (MtgIndex<0) return 0.0;
    int EquityTranche;
    foreach(WatFalPrior* SingleStep, m_WaterfallStesps)
    {
        if (SingleStep->GetPriorityType() == WatFalPrior::WaterfallStepType::wst_Excess) {
            EquityTranche = SingleStep->GetParameter(WatFalPrior::wstParameters::RedemptionGroup).value<IntegerVector>().GetValue(m_CalculatedMtgPayments.GetDate(MtgIndex));
            break;
        }
    }
    double denominator = 0.0;
    double numerator;
    if (m_PoolValueAtCall.IsEmpty())
        numerator = m_CalculatedMtgPayments.GetWAPrice(MtgIndex);
    else
        numerator = m_PoolValueAtCall.GetValue(m_CalculatedMtgPayments.GetDate(MtgIndex));
    numerator *= m_CalculatedMtgPayments.GetAmountOut(MtgIndex);
    if (EquityTranche>0) {
        foreach(Tranche* SingleTranche, m_Tranches)
        {
            if (SingleTranche->GetProrataGroup(0) == EquityTranche) denominator += SingleTranche->GetCashFlow().GetAmountOutstanding(index);
            else numerator -= SingleTranche->GetCashFlow().GetAmountOutstanding(index);
        }
        if (denominator>0) return numerator / denominator;
        else return 0.0;
    }
    denominator = 0.0;
    if (m_PoolValueAtCall.IsEmpty())
        numerator = m_CalculatedMtgPayments.GetWAPrice(MtgIndex);
    else
        numerator = m_PoolValueAtCall.GetValue(m_CalculatedMtgPayments.GetDate(MtgIndex));
    numerator *= m_CalculatedMtgPayments.GetAmountOut(MtgIndex);
    foreach(Tranche* SingleTranche, m_Tranches)
    {
        denominator += SingleTranche->GetCashFlow().GetAmountOutstanding(index);
        numerator -= SingleTranche->GetCashFlow().GetAmountOutstanding(index);
    }
    denominator = m_CalculatedMtgPayments.GetAmountOut(index) - denominator;
    if (denominator>0) return numerator / denominator;
    else return 0.0;
}
void Waterfall::SetReserveFund(int RFindex, const QString& RFtarget, const QString& RFmultiple, const QString& RFfloor, const QString& RFcap, double RFcurrent, int RFfreed, bool RFtoInterest)
{
    if (RFindex<0 || RFindex >= m_Reserves.size()) return;
    m_Reserves[RFindex]->SetReserveFund(RFtarget, RFmultiple, RFfloor, RFcap, RFcurrent, RFfreed, RFtoInterest);
}
void Waterfall::SetReserveFund(int RFindex, const ReserveFund& source)
{
    if (RFindex < 0 || RFindex >= m_Reserves.size()) return;
    m_Reserves[RFindex]->operator=(source);
}
void Waterfall::AddReserveFund(const QString& RFtarget, const QString& RFmultiple, const QString& RFfloor, const QString& RFcap, double RFcurrent, int RFfreed, bool RFtoInterest)
{
    m_Reserves.append(new ReserveFund());
    m_Reserves.last()->SetReserveFund(RFtarget, RFmultiple, RFfloor, RFcap, RFcurrent, RFfreed, RFtoInterest);
}
void Waterfall::AddReserveFund(const ReserveFund& source)
{
    m_Reserves.append(new ReserveFund(source));
}
void Waterfall::ResetReserve(int RFindex)
{
    if (RFindex == -1) {
        for (int i = 0; i<m_Reserves.size(); i++)
            m_Reserves[i]->ResetReserve();
        return;
    }
    if (RFindex<0 || RFindex >= m_Reserves.size()) return;
    m_Reserves[RFindex]->ResetReserve();
}
void Waterfall::RemoveReserve(int RFindex)
{
    if (RFindex == -1) {
        for (int i = 0; i < m_Reserves.size(); i++)
            delete m_Reserves[i];
        m_Reserves.clear();
        return;
    }
    if (RFindex < 0 || RFindex >= m_Reserves.size()) return;
    delete m_Reserves[RFindex];
    m_Reserves.removeAt(RFindex);
}
void Waterfall::SetPaymentFrequency(const QString& a)
{
    IntegerVector TempVect(a);
    for (int i = 0; i < TempVect.NumElements(); i++) {
        if (TempVect.GetValue(i) <= 0) return;
    }
    m_PaymentFrequency = TempVect;
}
void Waterfall::CompileReferenceRateValue(ForwardBaseRateTable& Values)
{
    m_GICBaseRateValue = m_GICBaseRate.CompileReferenceRateValue(Values);
    for (auto i = m_Tranches.begin(); i != m_Tranches.end(); ++i)
        (*i)->CompileReferenceRateValue(Values);
    m_ReinvestmentTest.CompileReferenceRateValue(Values);
}
void Waterfall::CompileReferenceRateValue(ConstantBaseRateTable& Values)
{
    m_GICBaseRateValue = m_GICBaseRate.CompileReferenceRateValue(Values);
    for (auto i = m_Tranches.begin(); i != m_Tranches.end(); ++i)
        (*i)->CompileReferenceRateValue(Values);
    m_ReinvestmentTest.CompileReferenceRateValue(Values);
}
#ifndef NO_DATABASE
void Waterfall::GetBaseRatesDatabase(ConstantBaseRateTable& Values, bool DownloadAll)
{
    m_GICBaseRateValue = m_GICBaseRate.GetBaseRatesDatabase(Values, DownloadAll);
    for (auto i = m_Tranches.begin(); i != m_Tranches.end(); ++i)
        (*i)->GetBaseRatesDatabase(Values, DownloadAll);
    m_ReinvestmentTest.GetBaseRatesDatabase(Values, DownloadAll);
}
void Waterfall::GetBaseRatesDatabase(ForwardBaseRateTable& Values, bool DownloadAll)
{
    m_GICBaseRateValue = m_GICBaseRate.GetBaseRatesDatabase(Values, DownloadAll);
    for (auto i = m_Tranches.begin(); i != m_Tranches.end(); ++i)
        (*i)->GetBaseRatesDatabase(Values, DownloadAll);
    m_ReinvestmentTest.GetBaseRatesDatabase(Values, DownloadAll);
}
#endif

void Waterfall::SetTrigger(quint32 key, QSharedPointer<AbstractTrigger> val)
{
    m_Triggers[key] = val;
}

bool Waterfall::ValidTriggerStructure(const QString& TriggerStructure)const
{
    QString AdjStructure = InfixToPostfix(TriggerStructure);
    if (AdjStructure.isEmpty()) {
        LOGDEBUG("Failed in Postfix");
        return false;
    }
    QRegExp SignleTriggers("(\\d+|T|F)", Qt::CaseInsensitive);
    QStringList CapturedTriggers;
    {
        int pos = 0;
        while ((pos = SignleTriggers.indexIn(AdjStructure, pos)) >= 0) {
            CapturedTriggers.append(SignleTriggers.cap(1));
            pos += SignleTriggers.matchedLength();
        }
    }
    if (CapturedTriggers.isEmpty()) return false;
    for (auto i = CapturedTriggers.constBegin(); i != CapturedTriggers.constEnd(); ++i) {
        if (i->compare("F", Qt::CaseInsensitive) == 0 || i->compare("T", Qt::CaseInsensitive) == 0) continue;
        if (!m_Triggers.contains(i->toUInt())) {
            LOGDEBUG("Failed Finding Trigger");
            return false;
        }
    }
    return true;
}

bool Waterfall::TriggerPassing(const QString& TriggerStructure, int PeriodIndex, const QDate& CurrentIPD, bool IsCallDate)
{
    QString AdjStructure = InfixToPostfix(TriggerStructure);
    if (AdjStructure.isEmpty()) return false;
    QRegExp SignleTriggers("(\\d+)");
    QStringList CapturedTriggers;
    {
        int pos = 0;
        while ((pos = SignleTriggers.indexIn(AdjStructure, pos)) >= 0) {
            CapturedTriggers.append(SignleTriggers.cap(1));
            pos += SignleTriggers.matchedLength();
        }
    }
    //if (CapturedTriggers.isEmpty()) return false;
    for (auto i = CapturedTriggers.constBegin(); i != CapturedTriggers.constEnd(); ++i) {
        quint32 CurrentTrigger = i->toUInt();
        if (!m_Triggers.contains(CurrentTrigger)) 	return false;
        TriggersResults::TrigRes SingleRes = m_TriggersResults.GetResult(CurrentTrigger, CurrentIPD);
        if (SingleRes == TriggersResults::TrigRes::trNA) {
            bool TempRes = EvaluateTrigger(CurrentTrigger, PeriodIndex, CurrentIPD, IsCallDate);
            m_TriggersResults.SetResult(CurrentTrigger, CurrentIPD, TempRes);
            SingleRes = (TempRes ? TriggersResults::TrigRes::trTrue : TriggersResults::TrigRes::trFalse);
        }
    }
    QStack<bool> PolishStack;
    QStringList PolishParts = AdjStructure.split(' ');
    SignleTriggers.setPattern("(\\d+)");
    foreach(const QString& SinglePart, PolishParts)
    {
        if (SinglePart.compare("F", Qt::CaseInsensitive) == 0) PolishStack.push(false);
        else if (SinglePart.compare("T", Qt::CaseInsensitive) == 0) PolishStack.push(true);
        else if (SignleTriggers.exactMatch(SinglePart)) {
            PolishStack.push(m_TriggersResults.GetResult(SinglePart.toUInt(), CurrentIPD) == TriggersResults::TrigRes::trTrue);
        }
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

bool Waterfall::EvaluateTrigger(quint32 TrigID, int PeriodIndex, const QDate& CurrentIPD, bool IsCallDate) const
{
    const QSharedPointer<AbstractTrigger> CurrentTrigger = m_Triggers.value(TrigID, QSharedPointer<AbstractTrigger>());
    if (!CurrentTrigger) return false;
    if (PeriodIndex < 0 || PeriodIndex >= m_MortgagesPayments.Count()) return false;
    switch (CurrentTrigger->GetTriggerType()) {
    case AbstractTrigger::TriggerType::DateTrigger:
        return CurrentTrigger.dynamicCast<DateTrigger>()->Passing(CurrentIPD);
    case AbstractTrigger::TriggerType::VectorTrigger:{
        VectorTrigger TempTrig(*CurrentTrigger.dynamicCast<VectorTrigger>());
        if (!TempTrig.HasAnchor())
            TempTrig.SetAnchor(m_MortgagesPayments.GetDate(0));
        return TempTrig.Passing(CurrentIPD);
    }
    case AbstractTrigger::TriggerType::PoolSizeTrigger:{
        PoolSizeTrigger TempTrig(*CurrentTrigger.dynamicCast<PoolSizeTrigger>());
        if (!TempTrig.HasAnchor())
            TempTrig.SetAnchorDate(m_MortgagesPayments.GetDate(0));
        return TempTrig.Passing(m_MortgagesPayments.GetAmountOut(PeriodIndex), m_MortgagesPayments.GetDate(PeriodIndex));
    }
    case AbstractTrigger::TriggerType::TrancheTrigger:{
        TrancheTrigger TempTrig(*CurrentTrigger.dynamicCast<TrancheTrigger>());
        if (!TempTrig.HasAnchor())
            TempTrig.FillMissingAnchorDate(m_MortgagesPayments.GetDate(0));
        return TempTrig.Passing(m_Tranches, CurrentIPD);
    }
    case AbstractTrigger::TriggerType::DelinquencyTrigger:{
        DelinquencyTrigger TempTrig(*CurrentTrigger.dynamicCast<DelinquencyTrigger>());
        if (!TempTrig.HasAnchor())
            TempTrig.SetAnchorDate(m_MortgagesPayments.GetDate(0));
        return TempTrig.Passing(m_MortgagesPayments.GetDelinquentShare(PeriodIndex), m_MortgagesPayments.GetDate(PeriodIndex));
    }
    case AbstractTrigger::TriggerType::DuringStressTestTrigger:{
        return CurrentTrigger.dynamicCast<DuringStressTestTrigger>()->Passing(m_IsStressTest);
    }
    case AbstractTrigger::TriggerType::CumulativeLossTrigger:{
        double TotalLoss = 0.0;
        for (int CumIter = 0; CumIter <= PeriodIndex; ++CumIter) {
            TotalLoss += m_MortgagesPayments.GetLoss(CumIter);
        }
        CumulativeLossTrigger TempTrig(*CurrentTrigger.dynamicCast<CumulativeLossTrigger>());
        if (!TempTrig.HasAnchor())
            TempTrig.SetAnchorDate(m_MortgagesPayments.GetDate(0));
        return TempTrig.Passing(TotalLoss, m_MortgagesPayments.GetDate(PeriodIndex));
    }
    case AbstractTrigger::TriggerType::DeferredInterestTrigger:{
        DeferredInterestTrigger TempTrig(*CurrentTrigger.dynamicCast<DeferredInterestTrigger>());
        if (!TempTrig.HasAnchor())
            TempTrig.FillMissingAnchorDate(m_MortgagesPayments.GetDate(0));
        return TempTrig.Passing(m_Tranches, CurrentIPD);
    }
    case AbstractTrigger::TriggerType::PDLTrigger:{
        PDLTrigger TempTrig(*CurrentTrigger.dynamicCast<PDLTrigger>());
        if (!TempTrig.HasAnchor())
            TempTrig.FillMissingAnchorDate(m_MortgagesPayments.GetDate(0));
        return TempTrig.Passing(m_Tranches, m_MortgagesPayments.GetAmountOut(PeriodIndex) + m_InterestAvailable + m_PrincipalAvailable.Total(), CurrentIPD);
    }
    default:
        Q_ASSERT_X(false, "Waterfall::EvaluateTrigger", "Unhadled Trigger Type");
        return false;
    }
}

void Waterfall::ResetTriggers()
{
    m_Triggers.clear();
    if (!m_ReinvestmentTest.GetReinvestmentPeriod().isNull()) {
        QSharedPointer<AbstractTrigger> TempReinvTrigger(new DateTrigger(m_ReinvestmentTest.GetReinvestmentPeriod(), DateTrigger::TriggerSide::BeforeIncluding, "Reinvestment Period"));
        SetTrigger(0, TempReinvTrigger);
    }
}

GenericCashFlow Waterfall::GetAggregatedReinvestment() const
{
    GenericCashFlow Result;
    Result.AddFlow(m_ReinvestmentTest.GetReinvested().AggregateRange(m_ReinvestmentTest.GetReinvested().GetDate(0), m_FirstIPDdate));
    const TrancheCashFlow& benchFlow = m_Tranches.first()->GetCashFlow();
    for (int i = 1; i < benchFlow.Count(); ++i) {
        Result.AddFlow(m_ReinvestmentTest.GetReinvested().AggregateRange(benchFlow.GetDate(i - 1).addDays(1), benchFlow.GetDate(i)));
    }
    return Result;
}

GenericCashFlow Waterfall::GetAggregatedGIC() const
{
    GenericCashFlow Result;
    Result.AddFlow(m_GICflows.AggregateRange(m_GICflows.GetDate(0), m_FirstIPDdate));
    const TrancheCashFlow& benchFlow = m_Tranches.first()->GetCashFlow();
    for (int i = 1; i < benchFlow.Count(); ++i) {
        Result.AddFlow(m_GICflows.AggregateRange(benchFlow.GetDate(i - 1).addDays(1), benchFlow.GetDate(i)));
    }
    return Result;
}

MtgCashFlow Waterfall::GetAggregatedMtgFlows() const
{
    MtgCashFlow Result;
    Result.AddFlow(m_CalculatedMtgPayments.AggregateRange(m_CalculatedMtgPayments.GetDate(0), m_FirstIPDdate));
    const TrancheCashFlow& benchFlow = m_Tranches.first()->GetCashFlow();
    for (int i = 1; i < benchFlow.Count(); ++i) {
        Result.AddFlow(m_CalculatedMtgPayments.AggregateRange(benchFlow.GetDate(i - 1).addDays(1), benchFlow.GetDate(i)));
    }
    return Result;
}

void Waterfall::SetAssumptions(const AssumptionSet& a)
{
    m_ReinvestmentTest.SetCPR(a.GetCPRscenario());
    m_ReinvestmentTest.SetCDR(a.GetCDRscenario());
    m_ReinvestmentTest.SetLS(a.GetLSscenario());
    m_ReinvestmentTest.SetRecoveryLag(a.GetRecLagScenario());
    m_ReinvestmentTest.SetDelinquency(a.GetDelinqScenario());
    m_ReinvestmentTest.SetDelinquencyLag(a.GetDelinqLagScenario());
}
