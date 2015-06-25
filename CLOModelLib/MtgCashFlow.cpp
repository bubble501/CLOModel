#include "CommonFunctions.h"
#include "MtgCashFlow.h"
#include <QtGlobal>
#include<QMap>
#include <QDataStream>
#include <qmath.h>
#include "Private/GenericCashFlow_p.h"
MtgCashFlow MtgCashFlow::ApplyScenario(BloombergVector CPRv, BloombergVector CDRv, BloombergVector LSv) const {
    Q_D(const GenericCashFlow);
	MtgCashFlow Result;
	Result.SetAdjustHolidays(d->m_AdjustHolidays);
    Result.Aggregate(d->m_AggregationLevel);
    Result.d_func()->m_Stocks = d->m_Stocks;
    Result.d_func()->m_CashFlowLabels = d->m_CashFlowLabels;
	if (Count() < 1 || CPRv.IsEmpty(0.0, 1.0) || CDRv.IsEmpty(0.0, 1.0) || LSv.IsEmpty(0.0, 1.0)) return Result;
	if (CPRv.GetAnchorDate().isNull()) CPRv.SetAnchorDate(GetDate(0));
	if (CDRv.GetAnchorDate().isNull()) CDRv.SetAnchorDate(GetDate(0));
	if (LSv.GetAnchorDate().isNull()) LSv.SetAnchorDate(GetDate(0));
	Result.AddFlow(SingleDate(GetDate(0)));
	double CurrentOut,ShareOfPrinc, ShareOfIntr, ShareOfAccrIntr, ShareOfLoss, ShareOfLossOnInterest, SumDeltaOut, TempF, ApplicablePrincipal, ApplicableMultiplier;
	bool HasOCoutstanding = HasFlowType(MtgFlowType::OutstandingForOC);
	if (Count() <= 1) return Result;
    for (auto i = (d->m_CashFlows.constBegin() + 1); i != d->m_CashFlows.constEnd(); ++i) {
		CurrentOut = GetFlow((i - 1).key(), MtgFlowType::AmountOutstandingFlow);
		if (CurrentOut == 0.0) {
			ShareOfPrinc = ShareOfIntr = ShareOfAccrIntr = 0.0;
		}
		else {
			ShareOfPrinc = GetFlow(i.key(), MtgFlowType::PrincipalFlow) / CurrentOut; 
			ShareOfIntr = GetFlow(i.key(), MtgFlowType::InterestFlow) / CurrentOut;
			ShareOfAccrIntr = GetFlow(i.key(), MtgFlowType::AccruedInterestFlow) / CurrentOut; 
		}
		Q_ASSERT_X(ShareOfPrinc >= 0.0 && ShareOfPrinc <= 1.0, "ApplyScenario", "ShareOfPrinc is not in [0,1]");
		Q_ASSERT_X(ShareOfIntr >= 0.0 && ShareOfIntr <= 1.0, "ApplyScenario", "ShareOfIntr is not in [0,1]");
		Q_ASSERT_X(ShareOfAccrIntr >= 0.0 && ShareOfAccrIntr <= 1.0, "ApplyScenario", "ShareOfAccrIntr is not in [0,1]");
		if (CurrentOut - GetFlow(i.key(), MtgFlowType::PrincipalFlow) <= 0.0) {
			ShareOfLoss = 0.0;
			ShareOfLossOnInterest = 0.0;
		}
		else {
			ShareOfLoss = GetFlow(i.key(), MtgFlowType::LossFlow) / (CurrentOut - GetFlow(i.key(), MtgFlowType::PrincipalFlow));
			ShareOfLossOnInterest = GetFlow(i.key(), MtgFlowType::LossOnInterestFlow) / (CurrentOut - GetFlow(i.key(), MtgFlowType::PrincipalFlow));
		}
		Q_ASSERT_X(ShareOfLoss >= 0.0 && ShareOfLoss <= 1.0, "ApplyScenario", "ShareOfLoss is not in [0,1]");
		Q_ASSERT_X(ShareOfLossOnInterest >= 0.0 && ShareOfLossOnInterest <= 1.0, "ApplyScenario", "ShareOfLossOnInterest is not in [0,1]");
		ApplicablePrincipal = Result.GetFlow(i.key(), MtgFlowType::AmountOutstandingFlow) + Result.GetFlow(i.key(), MtgFlowType::PrincipalRecovered);

		Result.SetFlow(i.key(), ApplicablePrincipal*ShareOfIntr, MtgFlowType::InterestFlow);
		Result.SetFlow(i.key(), ApplicablePrincipal*ShareOfAccrIntr, MtgFlowType::AccruedInterestFlow);

		SumDeltaOut = ApplicablePrincipal*ShareOfPrinc;
		Result.SetFlow(i.key(), SumDeltaOut, MtgFlowType::PrincipalFlow);

		Result.SetFlow(i.key(), (ApplicablePrincipal - SumDeltaOut)*ShareOfLoss, MtgFlowType::LossFlow);
		Result.SetFlow(i.key(), (ApplicablePrincipal - SumDeltaOut)*ShareOfLossOnInterest, MtgFlowType::LossOnInterestFlow);
		SumDeltaOut += (ApplicablePrincipal - SumDeltaOut)*ShareOfLoss;

		CurrentOut = GetFlow(i.key(), MtgFlowType::AmountOutstandingFlow);
		if (CurrentOut == 0.0)
			ApplicableMultiplier = 1.0;
		else
			ApplicableMultiplier = GetFlow(i.key(), MtgFlowType::WAPrepayMult) / CurrentOut;
		ApplicableMultiplier *= CPRv.GetSMM(i.key(), 1);
		{
			double PrepayedFlow = (ApplicablePrincipal - SumDeltaOut)*ApplicableMultiplier;
			Result.SetFlow(i.key(), PrepayedFlow, MtgFlowType::PrepaymentFlow);
			double ApplicFee = GetFlow(i.key(), MtgFlowType::WAPrepayFees);
			if (CurrentOut == 0.0) ApplicFee = 0.0;
			else ApplicFee /= CurrentOut;
			Result.SetFlow(i.key(), PrepayedFlow*ApplicFee, MtgFlowType::PrepaymentFees);
			Result.AddFlow(i.key(), PrepayedFlow*ApplicFee, MtgFlowType::InterestFlow);
		}
		SumDeltaOut += (ApplicablePrincipal - SumDeltaOut)*ApplicableMultiplier;
		Result.AddFlow(i.key(), Result.GetAccruedInterest(i.key())*ApplicableMultiplier, MtgFlowType::InterestFlow);
		Result.AddFlow(i.key(), -Result.GetAccruedInterest(i.key())*ApplicableMultiplier, MtgFlowType::AccruedInterestFlow);

		if (CurrentOut == 0.0)
			ApplicableMultiplier = LSv.GetValue(i.key());
		else
			ApplicableMultiplier = LSv.GetValue(i.key())* GetFlow(i.key(), MtgFlowType::WALossMult) / CurrentOut;
		TempF = (ApplicablePrincipal - SumDeltaOut)*CDRv.GetSMM(i.key(), 1);
		Result.SetFlow(i.key(), TempF, MtgFlowType::PrincipalDefault);
		Result.AddFlow(i.key(), qMin(TempF, TempF*ApplicableMultiplier), MtgFlowType::LossFlow);
		Result.SetFlow(i.key(), qMin(TempF, TempF*(1.0 - ApplicableMultiplier)), MtgFlowType::PrincipalRecovered);
		SumDeltaOut += TempF;
		SumDeltaOut -= qMin(TempF, TempF*(1.0 - ApplicableMultiplier));

		TempF = Result.GetAccruedInterest(i.key())*CDRv.GetSMM(i.key(), 1);
		if (TempF >= 0.01) {
			Result.AddFlow(i.key(), qMin(TempF, -TempF*ApplicableMultiplier), MtgFlowType::AccruedInterestFlow);
			Result.SetFlow(i.key(), qMin(TempF, TempF*(1.0 - (ApplicableMultiplier))), MtgFlowType::InterestRecovered);
			Result.AddFlow(i.key(), qMin(TempF, TempF*(ApplicableMultiplier)), MtgFlowType::LossOnInterestFlow);
		}
		Result.SetFlow(i.key(), ApplicablePrincipal - SumDeltaOut, MtgFlowType::AmountOutstandingFlow);
		if (CurrentOut != 0.0) {
			if (HasOCoutstanding) Result.SetFlow(i.key(), GetFlow(i.key(), MtgFlowType::OutstandingForOC) *(ApplicablePrincipal - SumDeltaOut) / CurrentOut, MtgFlowType::OutstandingForOC);
			Result.SetFlow(i.key(), GetFlow(i.key(), MtgFlowType::WACouponFlow) *(ApplicablePrincipal - SumDeltaOut) / CurrentOut, MtgFlowType::WACouponFlow);
			Result.SetFlow(i.key(), GetFlow(i.key(), MtgFlowType::WAPrepayMult) *(ApplicablePrincipal - SumDeltaOut) / CurrentOut, MtgFlowType::WAPrepayMult);
			Result.SetFlow(i.key(), GetFlow(i.key(), MtgFlowType::WALossMult) *(ApplicablePrincipal - SumDeltaOut) / CurrentOut, MtgFlowType::WALossMult);
			Result.SetFlow(i.key(), GetFlow(i.key(), MtgFlowType::WAPrice) *(ApplicablePrincipal - SumDeltaOut) / CurrentOut, MtgFlowType::WAPrice);
			Result.SetFlow(i.key(), GetFlow(i.key(), MtgFlowType::WAPrepayFees) *(ApplicablePrincipal - SumDeltaOut) / CurrentOut, MtgFlowType::WAPrepayFees);
			Result.SetFlow(i.key(), GetFlow(i.key(), MtgFlowType::WALlevel) *(ApplicablePrincipal - SumDeltaOut) / CurrentOut, MtgFlowType::WALlevel);
		}
		else {
			if (HasOCoutstanding) Result.SetFlow(i.key(), 0.0, MtgFlowType::OutstandingForOC);
			Result.SetFlow(i.key(), 0.0, MtgFlowType::WACouponFlow);
			Result.SetFlow(i.key(), 0.0, MtgFlowType::WAPrepayMult);
			Result.SetFlow(i.key(), 0.0, MtgFlowType::WALossMult);
			Result.SetFlow(i.key(), 0.0, MtgFlowType::WAPrice);
			Result.SetFlow(i.key(), 0.0, MtgFlowType::WAPrepayFees);
			Result.SetFlow(i.key(), 0.0, MtgFlowType::WALlevel);
		}
	}
	return Result;
}

MtgCashFlow MtgCashFlow::ApplyScenario(const QString& CPRv, const QString& CDRv, const QString& LSv) const
{

    return ApplyScenario(BloombergVector(CPRv), BloombergVector(CDRv), BloombergVector(LSv));
}

MtgCashFlow::MtgCashFlow()
{
    SetLabel(MtgFlowType::InterestFlow, "Interest");
    SetLabel(MtgFlowType::PrincipalFlow, "Scheduled");
    SetLabel(MtgFlowType::PrepaymentFlow, "Prepayment");
    SetLabel(MtgFlowType::AmountOutstandingFlow, "Amount Outstanding");
    SetLabel(MtgFlowType::WACouponFlow, "WA Coupon");
    SetLabel(MtgFlowType::AccruedInterestFlow, "Accrued Interest");
    SetLabel(MtgFlowType::LossOnInterestFlow, "Loss on Interest");
    SetLabel(MtgFlowType::PrincipalDefault, "Principal Default");
    SetLabel(MtgFlowType::PrincipalRecovered, "Principal Recovered");
    SetLabel(MtgFlowType::InterestRecovered, "Interest Recovered");
    SetLabel(MtgFlowType::LossFlow, "Loss");
    SetLabel(MtgFlowType::WAPrepayMult, "WA Prepay Multiplier");
    SetLabel(MtgFlowType::WALossMult, "WA Loss Multiplier");
    SetLabel(MtgFlowType::WAPrice, "WA Price");
    SetLabel(MtgFlowType::WALlevel, "WAL");
    SetLabel(MtgFlowType::DelinquentOutstanding, "Delinquent");
    SetLabel(MtgFlowType::WAPrepayFees, "WA Prepayment Fees");
    SetLabel(MtgFlowType::PrepaymentFees, "Prepayment Fees");
    SetLabel(MtgFlowType::OutstandingForOC, "Outstanding for OC Test");
    Aggregate(Monthly);
    SetStock(MtgFlowType::AmountOutstandingFlow);
    SetStock(MtgFlowType::WACouponFlow);
    SetStock(MtgFlowType::AccruedInterestFlow);
    SetStock(MtgFlowType::WAPrepayMult);
    SetStock(MtgFlowType::WALossMult);
    SetStock(MtgFlowType::WAPrice);
    SetStock(MtgFlowType::WALlevel);
    SetStock(MtgFlowType::DelinquentOutstanding);
    SetStock(MtgFlowType::WAPrepayFees);
    SetStock(MtgFlowType::OutstandingForOC);
}

MtgCashFlow::MtgCashFlow(const MtgCashFlow& a) 
	:GenericCashFlow(a)
{}
double MtgCashFlow::GetTotalFlow(int index) const
{
    return GenericCashFlow::GetTotalFlow(index, QList<qint32>()
        << MtgFlowType::InterestFlow
        << MtgFlowType::PrincipalFlow
        << MtgFlowType::PrepaymentFlow
        );
}
double MtgCashFlow::GetTotalFlow(const QDate& a) const
{
    return GenericCashFlow::GetTotalFlow(a, QList<qint32>()
        << MtgFlowType::InterestFlow
        << MtgFlowType::PrincipalFlow
        << MtgFlowType::PrepaymentFlow
        );
}

MtgCashFlow MtgCashFlow::ScaledCashFlows(double OriginalRefSize, double ResultSize) const
{
    MtgCashFlow Result(*this);
    Result.SetFlow(GenericCashFlow::ScaledCashFlows(OriginalRefSize, ResultSize, QList<qint32>(), QList<qint32>()
        << MtgFlowType::WACouponFlow
        << MtgFlowType::WAPrepayMult
        << MtgFlowType::WALossMult
        << MtgFlowType::WAPrice
        << MtgFlowType::WALlevel
        << MtgFlowType::WAPrepayFees
        ));
    double CurrentOut;
    for (auto i = Result.d_func()->m_CashFlows.begin(); i != Result.d_func()->m_CashFlows.end(); ++i) {
        CurrentOut = GetAmountOut(i.key());
        if (CurrentOut != 0) {
            i.value()->operator[](MtgFlowType::WACouponFlow) *= i.value()->value(MtgFlowType::AmountOutstandingFlow, 0.0) / CurrentOut;
            i.value()->operator[](MtgFlowType::WAPrepayMult) *= i.value()->value(MtgFlowType::AmountOutstandingFlow, 0.0) / CurrentOut;
            i.value()->operator[](MtgFlowType::WALossMult) *= i.value()->value(MtgFlowType::AmountOutstandingFlow, 0.0) / CurrentOut;
            i.value()->operator[](MtgFlowType::WAPrice) *= i.value()->value(MtgFlowType::AmountOutstandingFlow, 0.0) / CurrentOut;
            i.value()->operator[](MtgFlowType::WALlevel) *= i.value()->value(MtgFlowType::AmountOutstandingFlow, 0.0) / CurrentOut;
            i.value()->operator[](MtgFlowType::WAPrepayFees) *= i.value()->value(MtgFlowType::AmountOutstandingFlow, 0.0) / CurrentOut;
        }

    }
    return Result;
}