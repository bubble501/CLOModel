#include "CommonFunctions.h"
#include "MtgCashFlow.h"
#include <QtGlobal>
#include<QMap>
#include <QDataStream>
#include <qmath.h>
double MtgCashFlow::GetWAcoupon(int index) const {
	if (GetFlow(index,MtgFlowType::AmountOutstandingFlow) == 0.0) return 0.0;
	return GetFlow(index, MtgFlowType::WACouponFlow) / GetFlow(index, MtgFlowType::AmountOutstandingFlow);
}
double MtgCashFlow::GetWAcoupon(const QDate& index) const {
	if (GetFlow(index, MtgFlowType::AmountOutstandingFlow) == 0.0) return 0.0;
	return GetFlow(index, MtgFlowType::WACouponFlow) / GetFlow(index, MtgFlowType::AmountOutstandingFlow);
}
double MtgCashFlow::GetWAprepayMult(int index) const {
	if (GetFlow(index, MtgFlowType::AmountOutstandingFlow) == 0.0) return 0.0;
	return GetFlow(index, MtgFlowType::WAPrepayMult) / GetFlow(index, MtgFlowType::AmountOutstandingFlow);
}
double MtgCashFlow::GetWAprepayMult(const QDate& index) const {
	if (GetFlow(index, MtgFlowType::AmountOutstandingFlow) == 0.0) return 0.0;
	return GetFlow(index, MtgFlowType::WAPrepayMult) / GetFlow(index, MtgFlowType::AmountOutstandingFlow);
}
double MtgCashFlow::GetWAlossMult(int index) const {
	if (GetFlow(index, MtgFlowType::AmountOutstandingFlow) == 0.0) return 0.0;
	return GetFlow(index, MtgFlowType::WALossMult) / GetFlow(index, MtgFlowType::AmountOutstandingFlow);
}
double MtgCashFlow::GetWAlossMult(const QDate& index) const {
	if (GetFlow(index, MtgFlowType::AmountOutstandingFlow) == 0.0) return 0.0;
	return GetFlow(index, MtgFlowType::WALossMult) / GetFlow(index, MtgFlowType::AmountOutstandingFlow);
}
double MtgCashFlow::GetWAPrice(int index) const {
	if (GetFlow(index, MtgFlowType::AmountOutstandingFlow) == 0.0) return 0.0;
	return GetFlow(index, MtgFlowType::WAPrice) / GetFlow(index, MtgFlowType::AmountOutstandingFlow);
}
double MtgCashFlow::GetWAPrice(const QDate& index) const {
	if (GetFlow(index, MtgFlowType::AmountOutstandingFlow) == 0.0) return 0.0;
	return GetFlow(index, MtgFlowType::WAPrice) / GetFlow(index, MtgFlowType::AmountOutstandingFlow);
}
/*
QDataStream& operator<<(QDataStream & stream, const MtgCashFlow& flows) {
	stream << static_cast<const GenericCashFlow&>(flows);
	return stream;
}
QDataStream& operator>>(QDataStream & stream, MtgCashFlow& flows) {
	return flows.LoadOldVersion(stream);
}*/
MtgCashFlow MtgCashFlow::ApplyScenario(BloombergVector CPRv, BloombergVector CDRv, BloombergVector LSv) const {
	MtgCashFlow Result;
	qint32 i;
	if (Count() < 1 || CPRv.IsEmpty() || CDRv.IsEmpty() || LSv.IsEmpty()) return Result;
	if (CPRv.GetAnchorDate().isNull()) CPRv.SetAnchorDate(GetDate(0));
	if (CDRv.GetAnchorDate().isNull()) CDRv.SetAnchorDate(GetDate(0));
	if (LSv.GetAnchorDate().isNull()) LSv.SetAnchorDate(GetDate(0));

	Result.AddFlow(SingleDate(GetDate(0)));
	double ShareOfPrinc, ShareOfIntr, ShareOfAccrIntr, ShareOfLoss, ShareOfLossOnInterest, SumDeltaOut, TempF, ApplicablePrincipal, ApplicableMultiplier;
	for (i = 1; i < Count(); i++) {

		ShareOfPrinc = GetFlow(i, MtgFlowType::PrincipalFlow) / GetPreviousFlow(i, MtgFlowType::AmountOutstandingFlow);
		ShareOfIntr = GetFlow(i, MtgFlowType::InterestFlow) / GetPreviousFlow(i, MtgFlowType::AmountOutstandingFlow);
		ShareOfAccrIntr = GetFlow(i, MtgFlowType::AccruedInterestFlow) / GetPreviousFlow(i, MtgFlowType::AmountOutstandingFlow);
		if (GetPreviousFlow(i, MtgFlowType::AmountOutstandingFlow) - GetFlow(i, MtgFlowType::PrincipalFlow) <= 0.0) {
			ShareOfLoss = 0.0;
			ShareOfLossOnInterest = 0.0;
		}
		else {
			ShareOfLoss = GetFlow(i, MtgFlowType::LossFlow) / (GetPreviousFlow(i, MtgFlowType::AmountOutstandingFlow) - GetFlow(i, MtgFlowType::PrincipalFlow));
			ShareOfLossOnInterest = GetFlow(i, MtgFlowType::LossOnInterestFlow) / (GetPreviousFlow(i, MtgFlowType::AmountOutstandingFlow) - GetFlow(i, MtgFlowType::PrincipalFlow));
		}

		ApplicablePrincipal = Result.GetPreviousFlow(i, MtgFlowType::AmountOutstandingFlow) + Result.GetFlow(i, MtgFlowType::PrincipalRecovered);

		Result.AddFlow(GetDate(i), ApplicablePrincipal*ShareOfIntr, MtgFlowType::InterestFlow);
		Result.AddFlow(GetDate(i), ApplicablePrincipal*ShareOfAccrIntr, MtgFlowType::AccruedInterestFlow);

		SumDeltaOut = ApplicablePrincipal*ShareOfPrinc;
		Result.AddFlow(GetDate(i), SumDeltaOut, MtgFlowType::PrincipalFlow);

		Result.AddFlow(GetDate(i), (ApplicablePrincipal - SumDeltaOut)*ShareOfLoss, MtgFlowType::LossFlow);
		Result.AddFlow(GetDate(i), (ApplicablePrincipal - SumDeltaOut)*ShareOfLossOnInterest, MtgFlowType::LossOnInterestFlow);
		SumDeltaOut += (ApplicablePrincipal - SumDeltaOut)*ShareOfLoss;


		if (GetFlow(i, MtgFlowType::AmountOutstandingFlow) == 0.0)
			ApplicableMultiplier = 1.0;
		else
			ApplicableMultiplier = GetFlow(i, MtgFlowType::WAPrepayMult) / GetFlow(i, MtgFlowType::AmountOutstandingFlow);
		ApplicableMultiplier *= CPRv.GetSMM(GetDate(i), 1);
		Result.AddFlow(GetDate(i), (ApplicablePrincipal - SumDeltaOut)*ApplicableMultiplier, MtgFlowType::PrepaymentFlow);
		SumDeltaOut += (ApplicablePrincipal - SumDeltaOut)*ApplicableMultiplier;
		Result.AddFlow(GetDate(i), Result.GetAccruedInterest(GetDate(i))*ApplicableMultiplier, MtgFlowType::InterestFlow);
		Result.AddFlow(GetDate(i), -Result.GetAccruedInterest(GetDate(i))*ApplicableMultiplier, MtgFlowType::AccruedInterestFlow);

		if (GetFlow(i, MtgFlowType::AmountOutstandingFlow) == 0.0)
			ApplicableMultiplier = LSv.GetValue(GetDate(i));
		else
			ApplicableMultiplier = LSv.GetValue(GetDate(i))* GetFlow(i, MtgFlowType::WALossMult) / GetFlow(i, MtgFlowType::AmountOutstandingFlow);
		TempF = (ApplicablePrincipal - SumDeltaOut)*CDRv.GetSMM(GetDate(i), 1);
		Result.AddFlow(GetDate(i), TempF, MtgFlowType::PrincipalDefault);
		Result.AddFlow(GetDate(i), qMin(TempF, TempF*ApplicableMultiplier), MtgFlowType::LossFlow);
		Result.AddFlow(GetDate(i), qMin(TempF, TempF*(1.0 - ApplicableMultiplier)), MtgFlowType::PrincipalRecovered);
		SumDeltaOut += TempF;
		SumDeltaOut -= qMin(TempF, TempF*(1.0 - ApplicableMultiplier));

		TempF = Result.GetAccruedInterest(GetDate(i))*CDRv.GetSMM(GetDate(i), 1);
		if (TempF >= 0.01) {
			Result.AddFlow(GetDate(i), qMin(TempF, -TempF*ApplicableMultiplier), MtgFlowType::AccruedInterestFlow);
			Result.AddFlow(GetDate(i), qMin(TempF, TempF*(1.0 - (ApplicableMultiplier))), MtgFlowType::InterestRecovered);
			Result.AddFlow(GetDate(i), qMin(TempF, TempF*(ApplicableMultiplier)), MtgFlowType::LossOnInterestFlow);
		}
		Result.AddFlow(GetDate(i), ApplicablePrincipal - SumDeltaOut, MtgFlowType::AmountOutstandingFlow);
		if (GetFlow(i, MtgFlowType::AmountOutstandingFlow) != 0.0) {
			Result.AddFlow(GetDate(i), GetFlow(i, MtgFlowType::WACouponFlow) *(ApplicablePrincipal - SumDeltaOut) / GetFlow(i, MtgFlowType::AmountOutstandingFlow), MtgFlowType::WACouponFlow);
			Result.AddFlow(GetDate(i), GetFlow(i, MtgFlowType::WAPrepayMult) *(ApplicablePrincipal - SumDeltaOut) / GetFlow(i, MtgFlowType::AmountOutstandingFlow), MtgFlowType::WAPrepayMult);
			Result.AddFlow(GetDate(i), GetFlow(i, MtgFlowType::WALossMult) *(ApplicablePrincipal - SumDeltaOut) / GetFlow(i, MtgFlowType::AmountOutstandingFlow), MtgFlowType::WALossMult);
			Result.AddFlow(GetDate(i), GetFlow(i, MtgFlowType::WAPrice) *(ApplicablePrincipal - SumDeltaOut) / GetFlow(i, MtgFlowType::AmountOutstandingFlow), MtgFlowType::WAPrice);
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
	Aggregate(Monthly); 
}

MtgCashFlow::MtgCashFlow(const MtgCashFlow& a) 
	:GenericCashFlow(a)
{
	/*Aggregate(Monthly); 
	AddFlow(a);*/
}
double MtgCashFlow::GetTotalFlow(int index) const {
	QList<qint32> FlowsType;
	FlowsType.append(static_cast<qint32>(MtgFlowType::InterestFlow));
	FlowsType.append(static_cast<qint32>(MtgFlowType::PrincipalFlow));
	FlowsType.append(static_cast<qint32>(MtgFlowType::PrepaymentFlow));
	return GenericCashFlow::GetTotalFlow(index, FlowsType);
}
double MtgCashFlow::GetTotalFlow(const QDate& a) const {
	QList<qint32> FlowsType;
	
	return GenericCashFlow::GetTotalFlow(a, FlowsType);
}

void MtgCashFlow::FillWAL() {
	RemoveFlow(static_cast<qint32>(MtgFlowType::WALlevel));
	for (auto i = m_CashFlows.constBegin(); i != m_CashFlows.constEnd(); ++i) {
		AddFlow(i.key(), CalculateWAL(i.key()), MtgFlowType::WALlevel);
	}
}
double MtgCashFlow::CalculateWAL(const QDate& StartDate) const {
	double RunningSum = 0.0, Result = 0.0, CurrentPrinc;
	for (auto i = m_CashFlows.constFind(StartDate); i != m_CashFlows.constEnd(); ++i) {
		CurrentPrinc = i.value()->value(static_cast<qint32>(MtgFlowType::PrincipalFlow),0);
		if (CurrentPrinc > 0) {
			RunningSum += CurrentPrinc;
			Result += CurrentPrinc*static_cast<double>(StartDate.daysTo(i.key())) / 365.25;
		}
	}
	if (RunningSum <= 0) return 0.0;
	return Result / RunningSum;
}
