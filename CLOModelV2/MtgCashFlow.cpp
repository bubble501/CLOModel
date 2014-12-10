#include "CommonFunctions.h"
#include "MtgCashFlow.h"
#include <QtGlobal>
#include<QMap>
#include <QDataStream>
#include <qmath.h>

MtgCashFlow MtgCashFlow::ApplyScenario(BloombergVector CPRv, BloombergVector CDRv, BloombergVector LSv) const {
	MtgCashFlow Result;
	Result.SetAdjustHolidays(m_AdjustHolidays);
	Result.Aggregate(m_AggregationLevel);
	Result.m_Stocks = m_Stocks;
	Result.m_CashFlowLabels = m_CashFlowLabels;
	if (Count() < 1 || CPRv.IsEmpty() || CDRv.IsEmpty() || LSv.IsEmpty()) return Result;
	if (CPRv.GetAnchorDate().isNull()) CPRv.SetAnchorDate(GetDate(0));
	if (CDRv.GetAnchorDate().isNull()) CDRv.SetAnchorDate(GetDate(0));
	if (LSv.GetAnchorDate().isNull()) LSv.SetAnchorDate(GetDate(0));
	Result.AddFlow(SingleDate(GetDate(0)));
	double ShareOfPrinc, ShareOfIntr, ShareOfAccrIntr, ShareOfLoss, ShareOfLossOnInterest, SumDeltaOut, TempF, ApplicablePrincipal, ApplicableMultiplier;
	bool HasOCoutstanding = HasFlowType(static_cast<qint32>(MtgFlowType::OutstandingForOC));
	if (Count() <= 1) return Result;
	for (QMap<QDate, QHash<qint32, double>* >::const_iterator i = (m_CashFlows.constBegin() + 1); i != m_CashFlows.constEnd(); ++i) {
		ShareOfPrinc = GetFlow(i.key(), MtgFlowType::PrincipalFlow) / GetFlow((i-1).key(), MtgFlowType::AmountOutstandingFlow);
		ShareOfIntr = GetFlow(i.key(), MtgFlowType::InterestFlow) / GetFlow((i - 1).key(), MtgFlowType::AmountOutstandingFlow);
		ShareOfAccrIntr = GetFlow(i.key(), MtgFlowType::AccruedInterestFlow) / GetFlow((i - 1).key(), MtgFlowType::AmountOutstandingFlow);
		if (GetFlow((i - 1).key(), MtgFlowType::AmountOutstandingFlow) - GetFlow(i.key(), MtgFlowType::PrincipalFlow) <= 0.0) {
			ShareOfLoss = 0.0;
			ShareOfLossOnInterest = 0.0;
		}
		else {
			ShareOfLoss = GetFlow(i.key(), MtgFlowType::LossFlow) / (GetFlow((i - 1).key(), MtgFlowType::AmountOutstandingFlow) - GetFlow(i.key(), MtgFlowType::PrincipalFlow));
			ShareOfLossOnInterest = GetFlow(i.key(), MtgFlowType::LossOnInterestFlow) / (GetFlow((i - 1).key(), MtgFlowType::AmountOutstandingFlow) - GetFlow(i.key(), MtgFlowType::PrincipalFlow));
		}

		ApplicablePrincipal = Result.GetFlow((i - 1).key(), MtgFlowType::AmountOutstandingFlow) + Result.GetFlow(i.key(), MtgFlowType::PrincipalRecovered);

		Result.SetFlow(i.key(), ApplicablePrincipal*ShareOfIntr, MtgFlowType::InterestFlow);
		Result.SetFlow(i.key(), ApplicablePrincipal*ShareOfAccrIntr, MtgFlowType::AccruedInterestFlow);

		SumDeltaOut = ApplicablePrincipal*ShareOfPrinc;
		Result.SetFlow(i.key(), SumDeltaOut, MtgFlowType::PrincipalFlow);

		Result.SetFlow(i.key(), (ApplicablePrincipal - SumDeltaOut)*ShareOfLoss, MtgFlowType::LossFlow);
		Result.SetFlow(i.key(), (ApplicablePrincipal - SumDeltaOut)*ShareOfLossOnInterest, MtgFlowType::LossOnInterestFlow);
		SumDeltaOut += (ApplicablePrincipal - SumDeltaOut)*ShareOfLoss;


		if (GetFlow(i.key(), MtgFlowType::AmountOutstandingFlow) == 0.0)
			ApplicableMultiplier = 1.0;
		else
			ApplicableMultiplier = GetFlow(i.key(), MtgFlowType::WAPrepayMult) / GetFlow(i.key(), MtgFlowType::AmountOutstandingFlow);
		ApplicableMultiplier *= CPRv.GetSMM(i.key(), 1);
		{
			double PrepayedFlow = (ApplicablePrincipal - SumDeltaOut)*ApplicableMultiplier;
			Result.SetFlow(i.key(), PrepayedFlow, MtgFlowType::PrepaymentFlow);
			double ApplicFee = GetFlow(i.key(), MtgFlowType::WAPrepayFees);
			if (GetFlow(i.key(), MtgFlowType::AmountOutstandingFlow) == 0.0) ApplicFee = 0.0;
			else ApplicFee /= GetFlow(i.key(), MtgFlowType::AmountOutstandingFlow);
			Result.SetFlow(i.key(), PrepayedFlow*ApplicFee, MtgFlowType::PrepaymentFees);
			Result.AddFlow(i.key(), PrepayedFlow*ApplicFee, MtgFlowType::InterestFlow);
		}
		SumDeltaOut += (ApplicablePrincipal - SumDeltaOut)*ApplicableMultiplier;
		Result.AddFlow(i.key(), Result.GetAccruedInterest(i.key())*ApplicableMultiplier, MtgFlowType::InterestFlow);
		Result.AddFlow(i.key(), -Result.GetAccruedInterest(i.key())*ApplicableMultiplier, MtgFlowType::AccruedInterestFlow);

		if (GetFlow(i.key(), MtgFlowType::AmountOutstandingFlow) == 0.0)
			ApplicableMultiplier = LSv.GetValue(i.key());
		else
			ApplicableMultiplier = LSv.GetValue(i.key())* GetFlow(i.key(), MtgFlowType::WALossMult) / GetFlow(i.key(), MtgFlowType::AmountOutstandingFlow);
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
		if (GetFlow(i.key(), MtgFlowType::AmountOutstandingFlow) != 0.0) {
			if (HasOCoutstanding) Result.SetFlow(i.key(), GetFlow(i.key(), MtgFlowType::OutstandingForOC) *(ApplicablePrincipal - SumDeltaOut) / GetFlow(i.key(), MtgFlowType::AmountOutstandingFlow), MtgFlowType::OutstandingForOC);
			Result.SetFlow(i.key(), GetFlow(i.key(), MtgFlowType::WACouponFlow) *(ApplicablePrincipal - SumDeltaOut) / GetFlow(i.key(), MtgFlowType::AmountOutstandingFlow), MtgFlowType::WACouponFlow);
			Result.SetFlow(i.key(), GetFlow(i.key(), MtgFlowType::WAPrepayMult) *(ApplicablePrincipal - SumDeltaOut) / GetFlow(i.key(), MtgFlowType::AmountOutstandingFlow), MtgFlowType::WAPrepayMult);
			Result.SetFlow(i.key(), GetFlow(i.key(), MtgFlowType::WALossMult) *(ApplicablePrincipal - SumDeltaOut) / GetFlow(i.key(), MtgFlowType::AmountOutstandingFlow), MtgFlowType::WALossMult);
			Result.SetFlow(i.key(), GetFlow(i.key(), MtgFlowType::WAPrice) *(ApplicablePrincipal - SumDeltaOut) / GetFlow(i.key(), MtgFlowType::AmountOutstandingFlow), MtgFlowType::WAPrice);
			Result.SetFlow(i.key(), GetFlow(i.key(), MtgFlowType::WAPrepayFees) *(ApplicablePrincipal - SumDeltaOut) / GetFlow(i.key(), MtgFlowType::AmountOutstandingFlow), MtgFlowType::WAPrepayFees);
			Result.SetFlow(i.key(), GetFlow(i.key(), MtgFlowType::WALlevel) *(ApplicablePrincipal - SumDeltaOut) / GetFlow(i.key(), MtgFlowType::AmountOutstandingFlow), MtgFlowType::WALlevel);
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

MtgCashFlow::MtgCashFlow() {
	SetLabel(static_cast<qint32>(MtgFlowType::InterestFlow), "Interest");
	SetLabel(static_cast<qint32>(MtgFlowType::PrincipalFlow), "Scheduled");
	SetLabel(static_cast<qint32>(MtgFlowType::PrepaymentFlow), "Prepayment");
	SetLabel(static_cast<qint32>(MtgFlowType::AmountOutstandingFlow), "Amount Outstanding");
	SetLabel(static_cast<qint32>(MtgFlowType::WACouponFlow), "WA Coupon");
	SetLabel(static_cast<qint32>(MtgFlowType::AccruedInterestFlow), "Accrued Interest");
	SetLabel(static_cast<qint32>(MtgFlowType::LossOnInterestFlow), "Loss on Interest");
	SetLabel(static_cast<qint32>(MtgFlowType::PrincipalDefault), "Principal Default");
	SetLabel(static_cast<qint32>(MtgFlowType::PrincipalRecovered), "Principal Recovered");
	SetLabel(static_cast<qint32>(MtgFlowType::InterestRecovered), "Interest Recovered");
	SetLabel(static_cast<qint32>(MtgFlowType::LossFlow), "Loss");
	SetLabel(static_cast<qint32>(MtgFlowType::WAPrepayMult), "WA Prepay Multiplier");
	SetLabel(static_cast<qint32>(MtgFlowType::WALossMult), "WA Loss Multiplier");
	SetLabel(static_cast<qint32>(MtgFlowType::WAPrice), "WA Price");
	SetLabel(static_cast<qint32>(MtgFlowType::WALlevel), "WAL");
	SetLabel(static_cast<qint32>(MtgFlowType::DelinquentOutstanding), "Delinquent");
	SetLabel(static_cast<qint32>(MtgFlowType::WAPrepayFees), "WA Prepayment Fees");
	SetLabel(static_cast<qint32>(MtgFlowType::PrepaymentFees), "Prepayment Fees");
	SetLabel(static_cast<qint32>(MtgFlowType::OutstandingForOC), "Outstanding for OC Test");
	Aggregate(Monthly); 
	SetStock(static_cast<qint32>(MtgFlowType::AmountOutstandingFlow));
	SetStock(static_cast<qint32>(MtgFlowType::WACouponFlow));
	SetStock(static_cast<qint32>(MtgFlowType::AccruedInterestFlow));
	SetStock(static_cast<qint32>(MtgFlowType::WAPrepayMult));
	SetStock(static_cast<qint32>(MtgFlowType::WALossMult));
	SetStock(static_cast<qint32>(MtgFlowType::WAPrice));
	SetStock(static_cast<qint32>(MtgFlowType::WALlevel));
	SetStock(static_cast<qint32>(MtgFlowType::DelinquentOutstanding));
	SetStock(static_cast<qint32>(MtgFlowType::WAPrepayFees));
	SetStock(static_cast<qint32>(MtgFlowType::OutstandingForOC));
}

MtgCashFlow::MtgCashFlow(const MtgCashFlow& a) 
	:GenericCashFlow(a)
{
}
double MtgCashFlow::GetTotalFlow(int index) const {
	return GenericCashFlow::GetTotalFlow(index, QList<qint32>() 
		<< static_cast<qint32>(MtgFlowType::InterestFlow)
		<< static_cast<qint32>(MtgFlowType::PrincipalFlow)
		<< static_cast<qint32>(MtgFlowType::PrepaymentFlow)
	);
}
double MtgCashFlow::GetTotalFlow(const QDate& a) const {
	return GenericCashFlow::GetTotalFlow(a, QList<qint32>()
		<< static_cast<qint32>(MtgFlowType::InterestFlow)
		<< static_cast<qint32>(MtgFlowType::PrincipalFlow)
		<< static_cast<qint32>(MtgFlowType::PrepaymentFlow)
	);
}

MtgCashFlow MtgCashFlow::ScaledCashFlows(double OriginalRefSize, double ResultSize) const {
	MtgCashFlow Result(*this);
	Result.SetFlow(GenericCashFlow::ScaledCashFlows(OriginalRefSize, ResultSize, QList<qint32>(), QList<qint32>()
		<< static_cast<qint32>(MtgFlowType::WACouponFlow)
		<< static_cast<qint32>(MtgFlowType::WAPrepayMult)
		<< static_cast<qint32>(MtgFlowType::WALossMult)
		<< static_cast<qint32>(MtgFlowType::WAPrice)
		<< static_cast<qint32>(MtgFlowType::WALlevel)
		<< static_cast<qint32>(MtgFlowType::WAPrepayFees)
		));
	double CurrentOut;
	for (auto i = Result.m_CashFlows.begin(); i != Result.m_CashFlows.end(); ++i) {
		CurrentOut = GetAmountOut(i.key());
		if (CurrentOut != 0) {
			i.value()->operator[](static_cast<qint32>(MtgFlowType::WACouponFlow)) *= i.value()->value(static_cast<qint32>(MtgFlowType::AmountOutstandingFlow), 0.0) / CurrentOut;
			i.value()->operator[](static_cast<qint32>(MtgFlowType::WAPrepayMult)) *= i.value()->value(static_cast<qint32>(MtgFlowType::AmountOutstandingFlow), 0.0) / CurrentOut;
			i.value()->operator[](static_cast<qint32>(MtgFlowType::WALossMult)) *= i.value()->value(static_cast<qint32>(MtgFlowType::AmountOutstandingFlow), 0.0) / CurrentOut;
			i.value()->operator[](static_cast<qint32>(MtgFlowType::WAPrice)) *= i.value()->value(static_cast<qint32>(MtgFlowType::AmountOutstandingFlow), 0.0) / CurrentOut;
			i.value()->operator[](static_cast<qint32>(MtgFlowType::WALlevel)) *= i.value()->value(static_cast<qint32>(MtgFlowType::AmountOutstandingFlow), 0.0) / CurrentOut;
			i.value()->operator[](static_cast<qint32>(MtgFlowType::WAPrepayFees)) *= i.value()->value(static_cast<qint32>(MtgFlowType::AmountOutstandingFlow), 0.0) / CurrentOut;
		}
			
	}
	return Result;
}
