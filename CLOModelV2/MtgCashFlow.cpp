#include "CommonFunctions.h"
#include "MtgCashFlow.h"
#include <QtGlobal>
#include<QMap>
#include <QDataStream>
#include <qmath.h>

int MtgCashFlow::GetPaymentFrequency() const {
	if (m_FlowDates.size()>1)
		return qAbs(MonthDiff(m_FlowDates.last(), *(m_FlowDates.end() - 2)));
	else
		return 0;
}
int MtgCashFlow::Count() const {
	return m_FlowDates.size();
}
QDate MtgCashFlow::GetDate(int index)const {
	if (index<0 || index>m_FlowDates.size() - 1) return QDate();
	return m_FlowDates.at(index);
}
double MtgCashFlow::GetInterest(int index) const {
	if (index<0 || index>m_Interest.size() - 1) return 0.0;
	return m_Interest.at(index);
}
double MtgCashFlow::GetScheduled(int index) const {
	if (index<0 || index>m_Scheduled.size() - 1) return 0.0;
	return m_Scheduled.at(index);
}
double MtgCashFlow::GetPrepay(int index) const {
	if (index<0 || index>m_Prepay.size() - 1) return 0.0;
	return m_Prepay.at(index);
}
double MtgCashFlow::GetLoss(int index) const {
	if (index<0 || index>m_Loss.size() - 1) return 0.0;
	return m_Loss.at(index);
}
double MtgCashFlow::GetAccruedInterest(int index) const {
	if (index<0 || index>m_AccruedInterest.size() - 1) return 0.0;
	return m_AccruedInterest.at(index);
}
double MtgCashFlow::GetLossOnInterest(int index) const {
	if (index<0 || index>m_LossOnInterest.size() - 1) return 0.0;
	return m_LossOnInterest.at(index);
}
double MtgCashFlow::GetAmountOut(int index) const {
	if (index<0 || index>m_AmountOut.size() - 1) return 0.0;
	return m_AmountOut.at(index);
}
double MtgCashFlow::GetWAcoupon(int index) const {
	if (index<0 || index>m_CoupTimesOut.size() - 1 || GetAmountOut(index) == 0.0) return 0.0;
	return m_CoupTimesOut.at(index) / GetAmountOut(index);
}
double MtgCashFlow::GetApplicablePrincipal(int index) {
	if (index<1) return 0.0;
	return GetAmountOut(index - 1) + GetRecoveries(index);
}
double MtgCashFlow::GetApplicablePrincipal(const QDate& index) {
	return GetAmountOut(index.addMonths(-1)) + GetRecoveries(index);
	//return GetAmountOut(index.addMonths(-1)) + GetRecoveries(index.addMonths(-1));
}
double MtgCashFlow::GetWAcoupon(const QDate& index) const {
	int NewIndex = -1;
	for (int i = 0; i<m_FlowDates.size(); i++) {
		if (m_FlowDates.at(i).year() == index.year() && m_FlowDates.at(i).month() == index.month()) {
			NewIndex = i;
			break;
		}
	}
	return GetWAcoupon(NewIndex);
}
double MtgCashFlow::GetRecoveries(int index) const {
	if (index<0 || index>m_PrincipalRecovered.size() - 1) return 0.0;
	return m_PrincipalRecovered.at(index);
}
double MtgCashFlow::GetInterestRecoveries(int index) const {
	if (index<0 || index>m_InterestRecovered.size() - 1) return 0.0;
	return m_InterestRecovered.at(index);
}
double MtgCashFlow::GetDefaults(int index) const {
	if (index<0 || index>m_Defaulted.size() - 1) return 0.0;
	return m_Defaulted.at(index);
}
int  MtgCashFlow::FindDate(const QDate& a) const {
	return m_FlowDates.indexOf(a);
}
void MtgCashFlow::AddFlow(const QDate& Dte, double Amt, MtgFlowType FlowTpe) {
	if (qAbs(Amt)<0.01) Amt = 0.0;
	int DateIndex = m_FlowDates.indexOf(Dte);
	bool NeedSorting = false;
	if (DateIndex<0) {
		m_FlowDates.append(Dte);
		m_Interest.append(0.0);
		m_Scheduled.append(0.0);
		m_Prepay.append(0.0);
		m_Loss.append(0.0);
		m_AmountOut.append(0.0);
		m_AccruedInterest.append(0.0);
		m_LossOnInterest.append(0.0);
		m_CoupTimesOut.append(0.0);
		m_Defaulted.append(0.0);
		m_PrincipalRecovered.append(0.0);
		m_InterestRecovered.append(0.0);
		DateIndex = m_FlowDates.size() - 1;
		NeedSorting = true;
	}
	switch (FlowTpe) {
	case InterestFlow:
		m_Interest[DateIndex] += Amt; break;
	case PrincipalFlow:
		m_Scheduled[DateIndex] += Amt; break;
	case PrepaymentFlow:
		m_Prepay[DateIndex] += Amt; break;
	case LossFlow:
		m_Loss[DateIndex] += Amt; break;
	case AmountOutstandingFlow:
		m_AmountOut[DateIndex] += Amt; break;
	case AccruedInterestFlow:
		m_AccruedInterest[DateIndex] += Amt; break;
	case LossOnInterestFlow:
		m_LossOnInterest[DateIndex] += Amt; break;
	case WACouponFlow:
		m_CoupTimesOut[DateIndex] += Amt; break;
	case PrincipalDefault:
		m_Defaulted[DateIndex] += Amt; break;
	case PrincipalRecovered:
		m_PrincipalRecovered[DateIndex] += Amt; break;
	case InterestRecovered:
		m_InterestRecovered[DateIndex] += Amt; break;
	}
	if (NeedSorting) SortByDate();
}
void MtgCashFlow::AddFlow(const MtgCashFlow& a) {
	bool NeedSorting = false;
	int DateIndex;
	for (int i = 0; i<a.Count(); i++) {
		DateIndex = m_FlowDates.indexOf(a.GetDate(i));
		if (DateIndex<0) {
			m_FlowDates.append(a.GetDate(i));
			m_Interest.append(0.0);
			m_Scheduled.append(0.0);
			m_Prepay.append(0.0);
			m_Loss.append(0.0);
			m_AmountOut.append(0.0);
			m_AccruedInterest.append(0.0);
			m_LossOnInterest.append(0.0);
			m_CoupTimesOut.append(0.0);
			m_Defaulted.append(0.0);
			m_PrincipalRecovered.append(0.0);
			m_InterestRecovered.append(0.0);
			DateIndex = m_FlowDates.size() - 1;
			NeedSorting = true;
		}
		m_Interest[DateIndex] += a.m_Interest.at(i);
		m_Scheduled[DateIndex] += a.m_Scheduled.at(i);
		m_Prepay[DateIndex] += a.m_Prepay.at(i);
		m_Loss[DateIndex] += a.m_Loss.at(i);
		m_AmountOut[DateIndex] += a.m_AmountOut.at(i);
		m_CoupTimesOut[DateIndex] += a.m_CoupTimesOut.at(i);
		m_AccruedInterest[DateIndex] += a.m_AccruedInterest.at(i);
		m_LossOnInterest[DateIndex] += a.m_LossOnInterest.at(i);
		m_Defaulted[DateIndex] += a.m_Defaulted.at(i);
		m_PrincipalRecovered[DateIndex] += a.m_PrincipalRecovered.at(i);
		m_InterestRecovered[DateIndex] += a.m_InterestRecovered.at(i);
	}
	if (NeedSorting) SortByDate();
}
void MtgCashFlow::SortByDate() {
	int Orderer;
	QMap<int, QDate> DateSorter;
	QMap<int, double> InerestSorter;
	QMap<int, double> ScheduledSorter;
	QMap<int, double> PrepaySorter;
	QMap<int, double> LossSorter;
	QMap<int, double> AmountOutSorter;
	QMap<int, double> AccruedInterestSorter;
	QMap<int, double> LossOnInterestSorter;
	QMap<int, double> CoupTimesOutSorter;
	QMap<int, double> DefaultedPrincipalSorter;
	QMap<int, double> PrincipalRecoveriesSorter;
	QMap<int, double> InterestRecoveriesSorter;
	for (int i = 0; i<m_FlowDates.size(); i++) {
		Orderer = 0;
		for (int j = 0; j<m_FlowDates.size(); j++) {
			if (j == i) continue;
			if (m_FlowDates.at(j)>m_FlowDates.at(i)) Orderer++;
		}
		DateSorter.insert(m_FlowDates.size() - Orderer, m_FlowDates.at(i));
		InerestSorter.insert(m_FlowDates.size() - Orderer, m_Interest.at(i));
		ScheduledSorter.insert(m_FlowDates.size() - Orderer, m_Scheduled.at(i));
		PrepaySorter.insert(m_FlowDates.size() - Orderer, m_Prepay.at(i));
		LossSorter.insert(m_FlowDates.size() - Orderer, m_Loss.at(i));
		AmountOutSorter.insert(m_FlowDates.size() - Orderer, m_AmountOut.at(i));
		AccruedInterestSorter.insert(m_FlowDates.size() - Orderer, m_AccruedInterest.at(i));
		LossOnInterestSorter.insert(m_FlowDates.size() - Orderer, m_LossOnInterest.at(i));
		CoupTimesOutSorter.insert(m_FlowDates.size() - Orderer, m_CoupTimesOut.at(i));
		DefaultedPrincipalSorter.insert(m_FlowDates.size() - Orderer, m_Defaulted.at(i));
		PrincipalRecoveriesSorter.insert(m_FlowDates.size() - Orderer, m_PrincipalRecovered.at(i));
		InterestRecoveriesSorter.insert(m_FlowDates.size() - Orderer, m_InterestRecovered.at(i));
	}
	m_FlowDates = DateSorter.values();
	m_Interest = InerestSorter.values();
	m_Scheduled = ScheduledSorter.values();
	m_Prepay = PrepaySorter.values();
	m_Loss = LossSorter.values();
	m_AmountOut = AmountOutSorter.values();
	m_AccruedInterest = AccruedInterestSorter.values();
	m_LossOnInterest = LossOnInterestSorter.values();
	m_CoupTimesOut = CoupTimesOutSorter.values();
	m_Defaulted = DefaultedPrincipalSorter.values();
	m_PrincipalRecovered = PrincipalRecoveriesSorter.values();
	m_InterestRecovered = InterestRecoveriesSorter.values();
}
void MtgCashFlow::RemoveAllFlows() {
	m_FlowDates.clear();
	m_Interest.clear();
	m_Scheduled.clear();
	m_Prepay.clear();
	m_Loss.clear();
	m_AmountOut.clear();
	m_AccruedInterest.clear();
	m_LossOnInterest.clear();
	m_CoupTimesOut.clear();
	m_Defaulted.clear();
	m_PrincipalRecovered.clear();
	m_InterestRecovered.clear();
}
const MtgCashFlow& MtgCashFlow::operator=(const MtgCashFlow& a) {
	RemoveAllFlows();
	AddFlow(a);
	return *this;
}
QDataStream& operator<<(QDataStream & stream, const MtgCashFlow& flows) {
	stream
		<< flows.m_FlowDates
		<< flows.m_Interest
		<< flows.m_Scheduled
		<< flows.m_Prepay
		<< flows.m_Loss
		<< flows.m_AmountOut
		<< flows.m_CoupTimesOut
		<< flows.m_AccruedInterest
		<< flows.m_LossOnInterest
		<< flows.m_Defaulted
		<< flows.m_PrincipalRecovered
		<< flows.m_InterestRecovered
		;
	return stream;
}
QDataStream& MtgCashFlow::LoadOldVersion(QDataStream& stream) {
	stream
		>> m_FlowDates
		>> m_Interest
		>> m_Scheduled
		>> m_Prepay
		>> m_Loss
		>> m_AmountOut
		>> m_CoupTimesOut
		>> m_AccruedInterest
		>> m_LossOnInterest
		>> m_Defaulted
		>> m_PrincipalRecovered
		>> m_InterestRecovered
		;
	ResetProtocolVersion();
	return stream;
}
QDataStream& operator>>(QDataStream & stream, MtgCashFlow& flows) {
	return flows.LoadOldVersion(stream);
}
MtgCashFlow MtgCashFlow::ApplyScenario(BloombergVector CPRv, BloombergVector CDRv, BloombergVector LSv) const {
	MtgCashFlow Result;
	MtgCashFlow DelinquenciesFlows;
	MtgCashFlow InterestRecoveriesFlows;
	if (Count() < 1 || CPRv.IsEmpty() || CDRv.IsEmpty() || LSv.IsEmpty()) return Result;
	if (CPRv.GetAnchorDate().isNull()) CPRv.SetAnchorDate(m_FlowDates.first());
	if (CDRv.GetAnchorDate().isNull()) CDRv.SetAnchorDate(m_FlowDates.first());
	if (LSv.GetAnchorDate().isNull()) LSv.SetAnchorDate(m_FlowDates.first());
	Result.m_FlowDates.append(m_FlowDates.first());
	Result.m_Interest.append(m_Interest.first());
	Result.m_Prepay.append(m_Prepay.first());
	Result.m_Loss.append(m_Loss.first());
	Result.m_AmountOut.append(m_AmountOut.first());
	Result.m_CoupTimesOut.append(m_CoupTimesOut.first());
	Result.m_AccruedInterest.append(m_AccruedInterest.first());
	Result.m_LossOnInterest.append(m_LossOnInterest.first());
	Result.m_Scheduled.append(m_Scheduled.first());
	Result.m_Defaulted.append(m_Scheduled.first());
	Result.m_InterestRecovered.append(m_Scheduled.first());
	Result.m_PrincipalRecovered.append(m_Scheduled.first());
	double ShareOfPrinc, ShareOfIntr, ShareOfAccrIntr, ShareOfLoss, ShareOfLossOnInterest, SumDeltaOut, TempF, ApplicablePrincipal;
	int i;
	for (i = 1; i < Count(); i++) {

		ShareOfPrinc = GetPrincipal(i) / m_AmountOut.at(i - 1);
		ShareOfIntr = m_Interest.at(i) / (m_AmountOut.at(i - 1));
		ShareOfAccrIntr = m_AccruedInterest.at(i) / m_AmountOut.at(i - 1);
		if ((m_AmountOut.at(i - 1) - GetPrincipal(i)) <= 0.0) ShareOfLoss = 0.0;
		else ShareOfLoss = m_Loss.at(i) / (m_AmountOut.at(i - 1) - GetPrincipal(i));
		if ((m_AmountOut.at(i - 1) - GetPrincipal(i)) <= 0.0) ShareOfLossOnInterest = 0.0;
		else ShareOfLossOnInterest = m_LossOnInterest.at(i) / (m_AmountOut.at(i - 1) - GetPrincipal(i));

		ApplicablePrincipal = Result.GetApplicablePrincipal(m_FlowDates.at(i)); //Result.GetAmountOut(m_FlowDates.at(i - 1))

		Result.AddFlow(m_FlowDates.at(i),
			(ApplicablePrincipal*ShareOfIntr/**(1.0 - Delinq.GetValue(m_FlowDates.at(i)))*/)
			, InterestFlow);
		Result.AddFlow(m_FlowDates.at(i), ApplicablePrincipal*ShareOfAccrIntr, AccruedInterestFlow);

		/*Result.AddFlow(m_FlowDates.at(i),
		Result.GetInterestRecoveries(m_FlowDates.at(i)) *
		(Result.GetInterest(m_FlowDates.at(i)) / (Result.GetInterest(m_FlowDates.at(i)) + Result.GetAccruedInterest(m_FlowDates.at(i))))
		, InterestFlow);
		Result.AddFlow(m_FlowDates.at(i),
		Result.GetInterestRecoveries(m_FlowDates.at(i)) *
		(Result.GetAccruedInterest(m_FlowDates.at(i)) / (Result.GetInterest(m_FlowDates.at(i)) + Result.GetAccruedInterest(m_FlowDates.at(i))))
		, AccruedInterestFlow);*/

		/*if (Delinq.GetValue(m_FlowDates.at(i)) > 0.0) {
		if (DelinqLag.GetValue(m_FlowDates.at(i)) == 0) {
		Result.AddFlow(m_FlowDates.at(i), ApplicablePrincipal*ShareOfIntr*Delinq.GetValue(m_FlowDates.at(i)), LossOnInterestFlow);
		}
		else {
		MtgCashFlow CurrentDelinq;
		double RollShareOfAccrIntr;
		CurrentDelinq.AddFlow(m_FlowDates.at(i), ApplicablePrincipal*ShareOfIntr*Delinq.GetValue(m_FlowDates.at(i)), AccruedInterestFlow);
		for (int DelinqIter = 0; DelinqIter < DelinqLag.GetValue(m_FlowDates.at(i)); DelinqIter++) {

		if (
		(i + DelinqIter + 1 >= m_AccruedInterest.size())
		|| (i + DelinqIter + 1 >= m_Interest.size())
		|| (i + DelinqIter >= m_AmountOut.size())
		)
		RollShareOfAccrIntr = ((*(m_AccruedInterest.end() - 2)) + (*(m_Interest.end() - 2))) / *(m_AmountOut.end() - 2);
		else
		RollShareOfAccrIntr = (m_AccruedInterest.at(i + DelinqIter + 1) + m_Interest.at(i + DelinqIter + 1)) / m_AmountOut.at(i + DelinqIter);
		CurrentDelinq.AddFlow(
		m_FlowDates.at(i).addMonths(DelinqIter + 1),
		(1.0 + RollShareOfAccrIntr)*CurrentDelinq.GetAccruedInterest(m_FlowDates.at(i).addMonths(DelinqIter)),
		AccruedInterestFlow
		);
		}
		CurrentDelinq.AddFlow(
		m_FlowDates.at(i).addMonths(DelinqLag.GetValue(m_FlowDates.at(i))),
		CurrentDelinq.GetAccruedInterest(m_FlowDates.at(i).addMonths(DelinqLag.GetValue(m_FlowDates.at(i))))
		, InterestFlow
		);
		CurrentDelinq.AddFlow(
		m_FlowDates.at(i).addMonths(DelinqLag.GetValue(m_FlowDates.at(i))),
		-CurrentDelinq.GetAccruedInterest(m_FlowDates.at(i).addMonths(DelinqLag.GetValue(m_FlowDates.at(i))))
		, AccruedInterestFlow
		);
		DelinquenciesFlows.AddFlow(CurrentDelinq);
		}
		}*/







		SumDeltaOut = ApplicablePrincipal*ShareOfPrinc;
		Result.AddFlow(m_FlowDates.at(i), SumDeltaOut, PrincipalFlow);

		Result.AddFlow(m_FlowDates.at(i), (ApplicablePrincipal - SumDeltaOut)*ShareOfLoss, LossFlow);
		Result.AddFlow(m_FlowDates.at(i), (ApplicablePrincipal - SumDeltaOut)*ShareOfLossOnInterest, LossOnInterestFlow);
		SumDeltaOut += (ApplicablePrincipal - SumDeltaOut)*ShareOfLoss;




		Result.AddFlow(m_FlowDates.at(i), (ApplicablePrincipal - SumDeltaOut)*CPRv.GetSMM(m_FlowDates.at(i), 1), PrepaymentFlow);
		SumDeltaOut += (ApplicablePrincipal - SumDeltaOut)*CPRv.GetSMM(m_FlowDates.at(i), 1);
		Result.AddFlow(m_FlowDates.at(i), Result.GetAccruedInterest(m_FlowDates.at(i))*CPRv.GetSMM(m_FlowDates.at(i), 1), InterestFlow);
		Result.AddFlow(m_FlowDates.at(i), -Result.GetAccruedInterest(m_FlowDates.at(i))*CPRv.GetSMM(m_FlowDates.at(i), 1), AccruedInterestFlow);



		TempF = (ApplicablePrincipal - SumDeltaOut)*CDRv.GetSMM(m_FlowDates.at(i), 1);
		Result.AddFlow(m_FlowDates.at(i), TempF, PrincipalDefault);
		Result.AddFlow(m_FlowDates.at(i)/*.addMonths(RecovLag.GetValue(m_FlowDates.at(i)))*/, qMin(TempF, TempF*LSv.GetValue(m_FlowDates.at(i))), LossFlow);
		Result.AddFlow(m_FlowDates.at(i)/*.addMonths(RecovLag.GetValue(m_FlowDates.at(i)))*/, qMin(TempF, TempF*(1.0 - LSv.GetValue(m_FlowDates.at(i)))), PrincipalRecovered);
		SumDeltaOut += TempF;
		/*if (RecovLag.GetValue(m_FlowDates.at(i)) == 0)*/ SumDeltaOut -= qMin(TempF, TempF*(1.0 - LSv.GetValue(m_FlowDates.at(i))));

		TempF = Result.GetAccruedInterest(m_FlowDates.at(i))*CDRv.GetSMM(m_FlowDates.at(i), 1);
		if (TempF >= 0.01) {
			/*if (RecovLag.GetValue(m_FlowDates.at(i)) == 0)*/ Result.AddFlow(m_FlowDates.at(i), qMin(TempF, -TempF*LSv.GetValue(m_FlowDates.at(i))), AccruedInterestFlow);
			/*else {
			Result.AddFlow(m_FlowDates.at(i), -TempF, AccruedInterestFlow);

			}*/
			Result.AddFlow(m_FlowDates.at(i)/*.addMonths(RecovLag.GetValue(m_FlowDates.at(i)))*/, qMin(TempF, TempF*(1.0 - (LSv.GetValue(m_FlowDates.at(i))))), InterestRecovered);
			Result.AddFlow(m_FlowDates.at(i)/*.addMonths(RecovLag.GetValue(m_FlowDates.at(i)))*/, qMin(TempF, TempF*(LSv.GetValue(m_FlowDates.at(i)))), LossOnInterestFlow);
		}
		Result.AddFlow(m_FlowDates.at(i), ApplicablePrincipal - SumDeltaOut, AmountOutstandingFlow);
		if (m_AmountOut.at(i) == 0.0)
			Result.AddFlow(m_FlowDates.at(i), 0.0, WACouponFlow);
		else
			Result.AddFlow(m_FlowDates.at(i), m_CoupTimesOut.at(i) *(ApplicablePrincipal - SumDeltaOut) / m_AmountOut.at(i), WACouponFlow);
	}
	/*for (; i < Result.Count(); i++) {
	Result.AddFlow(Result.GetDate(i), Result.GetRecoveries(i), PrincipalFlow);
	Result.AddFlow(Result.GetDate(i), Result.GetInterestRecoveries(i), InterestFlow);
	}*/
	Result.AddFlow(DelinquenciesFlows);
	return Result;
}

#ifdef _DEBUG
bool MtgCashFlow::operator==(const MtgCashFlow& a) const {
	int i, Maxiter = qMax(Count(), a.Count());
	for (i = 0; i < Maxiter; i++) {
		if (qAbs(GetInterest(i) - a.GetInterest(i)) >= 0.01) return false;
		if (qAbs(GetScheduled(i) - a.GetScheduled(i)) >= 0.01) return false;
		if (qAbs(GetAccruedInterest(i) - a.GetAccruedInterest(i)) >= 0.01) return false;
		if (qAbs(GetPrepay(i) - a.GetPrepay(i)) >= 0.01) return false;
		if (qAbs(GetAmountOut(i) - a.GetAmountOut(i)) >= 0.01) return false;
		if (qAbs(GetLoss(i) - a.GetLoss(i)) >= 0.01) return false;
		if (qAbs(GetLossOnInterest(i) - a.GetLossOnInterest(i)) >= 0.01) return false;
		if (qAbs(GetWAcoupon(i) - a.GetWAcoupon(i)) >= 0.01) return false;
		if (qAbs(GetRecoveries(i) - a.GetRecoveries(i)) >= 0.01) return false;
		if (qAbs(GetDefaults(i) - a.GetDefaults(i)) >= 0.01) return false;
		if (qAbs(GetInterestRecoveries(i) - a.GetInterestRecoveries(i)) >= 0.01) return false;
	}
	return true;
}
double MtgCashFlow::GetCoupTimeOut(int index) const {
	if (index<0 || index>m_CoupTimesOut.size() - 1) return 0.0;
	return m_CoupTimesOut.at(index);
}
#endif // _DEBUG
