#include "CommonFunctions.h"
#include "MtgCashFlow.h"
#include <QtGlobal>
#include<QMap>
#include <QDataStream>
#include <qmath.h>
double MtgCashFlow::GetWAcoupon(int index) const {
	if (GenericCashFlow::GetFlow(index, AmountOutstandingFlow) == 0) return 0.0;
	return GenericCashFlow::GetFlow(index, WACouponFlow) / GenericCashFlow::GetFlow(index, AmountOutstandingFlow);
}
double MtgCashFlow::GetWAcoupon(const QDate& index) const {
	if (GenericCashFlow::GetFlow(index, AmountOutstandingFlow) == 0) return 0.0;
	return GenericCashFlow::GetFlow(index, WACouponFlow) / GenericCashFlow::GetFlow(index, AmountOutstandingFlow);
}
double MtgCashFlow::GetWAprepayMult(int index) const {
	if (GenericCashFlow::GetFlow(index, AmountOutstandingFlow) == 0) return 0.0;
	return GenericCashFlow::GetFlow(index, WAPrepayMult) / GenericCashFlow::GetFlow(index, AmountOutstandingFlow);
}
double MtgCashFlow::GetWAprepayMult(const QDate& index) const {
	if (GenericCashFlow::GetFlow(index, AmountOutstandingFlow) == 0) return 0.0;
	return GenericCashFlow::GetFlow(index, WAPrepayMult) / GenericCashFlow::GetFlow(index, AmountOutstandingFlow);
}
double MtgCashFlow::GetWAlossMult(int index) const {
	if (GenericCashFlow::GetFlow(index, AmountOutstandingFlow) == 0) return 0.0;
	return GenericCashFlow::GetFlow(index, WALossMult) / GenericCashFlow::GetFlow(index, AmountOutstandingFlow);
}
double MtgCashFlow::GetWAlossMult(const QDate& index) const {
	if (GenericCashFlow::GetFlow(index, AmountOutstandingFlow) == 0) return 0.0;
	return GenericCashFlow::GetFlow(index, WALossMult) / GenericCashFlow::GetFlow(index, AmountOutstandingFlow);
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
	for (i = 0; i <= LossFlow; i++)
		Result.AddFlow(GetDate(0), GetFlow(GetDate(0), i), static_cast<MtgFlowType>(i));
	double ShareOfPrinc, ShareOfIntr, ShareOfAccrIntr, ShareOfLoss, ShareOfLossOnInterest, SumDeltaOut, TempF, ApplicablePrincipal, ApplicableMultiplier;
	for (i = 1; i < Count(); i++) {

		ShareOfPrinc = GetFlow(i, PrincipalFlow) / GetPreviousFlow(i, AmountOutstandingFlow); 
		ShareOfIntr = GetFlow(i, InterestFlow) / GetPreviousFlow(i, AmountOutstandingFlow);
		ShareOfAccrIntr = GetFlow(i, AccruedInterestFlow) / GetPreviousFlow(i, AmountOutstandingFlow);
		if (GetPreviousFlow(i, AmountOutstandingFlow) - GetFlow(i, PrincipalFlow) <= 0.0) {
			ShareOfLoss = 0.0;
			ShareOfLossOnInterest = 0.0;
		}
		else {
			ShareOfLoss = GetFlow(i, LossFlow) / (GetPreviousFlow(i, AmountOutstandingFlow) - GetFlow(i, PrincipalFlow));
			ShareOfLossOnInterest = GetFlow(i, LossOnInterestFlow) / (GetPreviousFlow(i, AmountOutstandingFlow) - GetFlow(i, PrincipalFlow));
		}

		ApplicablePrincipal = Result.GetPreviousFlow(i, AmountOutstandingFlow) + Result.GetFlow(i, PrincipalRecovered);

		Result.AddFlow(GetDate(i), ApplicablePrincipal*ShareOfIntr, InterestFlow);
		Result.AddFlow(GetDate(i), ApplicablePrincipal*ShareOfAccrIntr, AccruedInterestFlow);

		SumDeltaOut = ApplicablePrincipal*ShareOfPrinc;
		Result.AddFlow(GetDate(i), SumDeltaOut, PrincipalFlow);

		Result.AddFlow(GetDate(i), (ApplicablePrincipal - SumDeltaOut)*ShareOfLoss, LossFlow);
		Result.AddFlow(GetDate(i), (ApplicablePrincipal - SumDeltaOut)*ShareOfLossOnInterest, LossOnInterestFlow);
		SumDeltaOut += (ApplicablePrincipal - SumDeltaOut)*ShareOfLoss;


		if (GetFlow(i, AmountOutstandingFlow) == 0.0)
			ApplicableMultiplier = 1.0;
		else
			ApplicableMultiplier = GetFlow(i, WAPrepayMult) / GetFlow(i, AmountOutstandingFlow);
		ApplicableMultiplier *= CPRv.GetSMM(GetDate(i), 1);
		Result.AddFlow(GetDate(i), (ApplicablePrincipal - SumDeltaOut)*ApplicableMultiplier, PrepaymentFlow);
		SumDeltaOut += (ApplicablePrincipal - SumDeltaOut)*ApplicableMultiplier;
		Result.AddFlow(GetDate(i), Result.GetAccruedInterest(GetDate(i))*ApplicableMultiplier, InterestFlow);
		Result.AddFlow(GetDate(i), -Result.GetAccruedInterest(GetDate(i))*ApplicableMultiplier, AccruedInterestFlow);

		if (GetFlow(i, AmountOutstandingFlow) == 0.0)
			ApplicableMultiplier = LSv.GetValue(GetDate(i));
		else
			ApplicableMultiplier = LSv.GetValue(GetDate(i))* GetFlow(i, WALossMult) / GetFlow(i, AmountOutstandingFlow);
		TempF = (ApplicablePrincipal - SumDeltaOut)*CDRv.GetSMM(GetDate(i), 1);
		Result.AddFlow(GetDate(i), TempF, PrincipalDefault);
		Result.AddFlow(GetDate(i), qMin(TempF, TempF*ApplicableMultiplier), LossFlow);
		Result.AddFlow(GetDate(i), qMin(TempF, TempF*(1.0 - ApplicableMultiplier)), PrincipalRecovered);
		SumDeltaOut += TempF;
		SumDeltaOut -= qMin(TempF, TempF*(1.0 - ApplicableMultiplier));

		TempF = Result.GetAccruedInterest(GetDate(i))*CDRv.GetSMM(GetDate(i), 1);
		if (TempF >= 0.01) {
			Result.AddFlow(GetDate(i), qMin(TempF, -TempF*ApplicableMultiplier), AccruedInterestFlow);
			Result.AddFlow(GetDate(i), qMin(TempF, TempF*(1.0 - (ApplicableMultiplier))), InterestRecovered);
			Result.AddFlow(GetDate(i), qMin(TempF, TempF*(ApplicableMultiplier)), LossOnInterestFlow);
		}
		Result.AddFlow(GetDate(i), ApplicablePrincipal - SumDeltaOut, AmountOutstandingFlow);
		if (GetFlow(i, AmountOutstandingFlow) != 0.0) {
			Result.AddFlow(GetDate(i), GetFlow(i, WACouponFlow) *(ApplicablePrincipal - SumDeltaOut) / GetFlow(i, AmountOutstandingFlow), WACouponFlow);
			Result.AddFlow(GetDate(i), GetFlow(i, WAPrepayMult) *(ApplicablePrincipal - SumDeltaOut) / GetFlow(i, AmountOutstandingFlow), WAPrepayMult);
			Result.AddFlow(GetDate(i), GetFlow(i, WALossMult) *(ApplicablePrincipal - SumDeltaOut) / GetFlow(i, AmountOutstandingFlow), WALossMult);
		}
	}
	return Result;
}

MtgCashFlow::MtgCashFlow() {
	Aggregate(Monthly); 
	SetStock(AmountOutstandingFlow);
	SetStock(WACouponFlow);
	SetStock(AccruedInterestFlow);
	SetStock(WAPrepayMult);
	SetStock(WALossMult);
}

MtgCashFlow::MtgCashFlow(const MtgCashFlow& a) {
	Aggregate(Monthly); 
	SetStock(AmountOutstandingFlow);
	SetStock(WACouponFlow);
	SetStock(AccruedInterestFlow);
	SetStock(WAPrepayMult);
	SetStock(WALossMult);
	AddFlow(a);
}
