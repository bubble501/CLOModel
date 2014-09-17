#include <QtAlgorithms>
#include <QQueue>
#include <QHash>
#include <QSet>
#include <qmath.h>
#include "Waterfall.h"
#include "QBbgWorker.h"

const WatFalPrior* Waterfall::GetStep(int Index)const {
	if(Index<0 || Index>=m_WaterfallStesps.size()) return NULL;
	return m_WaterfallStesps.at(Index);
}
WatFalPrior* Waterfall::GetStep(int Index){
	if(Index<0 || Index>=m_WaterfallStesps.size()) return NULL;
	return m_WaterfallStesps[Index];
}
void Waterfall::SetCCCcurve(const QString& a){m_CCCcurve=a;}

const Tranche* Waterfall::GetTranche(int Index) const{
	if(Index<0 || Index>=m_Tranches.size()) return NULL;
	return m_Tranches.at(Index);
}
Tranche* Waterfall::GetTranche(int Index){
	if(Index<0 || Index>=m_Tranches.size()) return NULL;
	return m_Tranches[Index];
}
const Tranche* Waterfall::GetTranche(const QString& TrancheName) const{
	return GetTranche(FindTrancheIndex(TrancheName));
}
Tranche* Waterfall::GetTranche(const QString& TrancheName){
	return GetTranche(FindTrancheIndex(TrancheName));
}
void Waterfall::SortByProRataGroup(){
	qSort(m_Tranches.begin(),m_Tranches.end(),LessThanPoint<Tranche>);
}
QDate Waterfall::GetStructureMaturity()const{
	if(m_Tranches.isEmpty()) return QDate();
	if(m_Tranches.first()->GetCashFlow().Count()==0) return QDate();
	return m_Tranches.first()->GetCashFlow().GetDate(m_Tranches.first()->GetCashFlow().Count()-1);
}
double Waterfall::GetAnnualizedExcess(int index, bool AsShareOfLoans)const{
	if(index<0 || index>=m_AnnualizedExcess.Count()) return 0.0;
	IntegerVector AdjPaymentFreq(m_PaymentFrequency);
	if(m_PaymentFrequency.GetAnchorDate().isNull()) AdjPaymentFreq.SetAnchorDate(m_MortgagesPayments.GetDate(0));
	if(AsShareOfLoans){
		if(m_CalculatedMtgPayments.GetAmountOut(m_AnnualizedExcess.GetDate(index))<0.01) return -1.0;
		return qPow((1.0 + (m_AnnualizedExcess.GetFlow(index, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow)) /
			m_CalculatedMtgPayments.GetAmountOut(m_AnnualizedExcess.GetDate(index)))),12.0/(AdjPaymentFreq.GetValue(m_AnnualizedExcess.GetDate(index))))-1.0;
	}
	double RunningSum=0.0;
	for (int i=0;i<m_Tranches.size();i++){
		RunningSum+=m_Tranches.at(i)->GetCashFlow().GetAmountOutstanding(index);
	}
	if(RunningSum<0.01) return -1.0;
	return qPow(1.0+(m_AnnualizedExcess.GetFlow(index,static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow))/RunningSum),12.0/(AdjPaymentFreq.GetValue(m_AnnualizedExcess.GetDate(index))))-1.0;
}
Waterfall::Waterfall()
	:m_SeniorExpenses("0")
	, m_SeniorFees("0")
	, m_JuniorFees("0")
	, m_SeniorExpensesFixed("0")
	, m_SeniorFeesFixed("0")
	, m_JuniorFeesFixed("0")
	,m_PaymentFrequency("1")
	,m_CCCTestLimit(1.0)
	,m_CCCcurve("0")
	,m_CCChaircut(0.0)
	,m_UseTurbo(true)
	,m_InterestAvailable(0.0)
	,m_JuniorFeesCoupon(0.0)
	,m_PoolValueAtCall("100")
	,m_UseCall(false)
	,m_CallMultiple(0.0)
	,m_CallReserve(0.0)
	,m_CumulativeReserves(false)
	, m_StartingDeferredJunFees(0.0)
	, m_GICBaseRate("ZERO")
{
	m_GICflows.Aggregate(GenericCashFlow::Monthly);
	m_ExcessCashFlow.Aggregate(GenericCashFlow::Monthly);
	m_TotalSeniorExpenses.Aggregate(GenericCashFlow::Monthly);
	m_TotalSeniorFees.Aggregate(GenericCashFlow::Monthly);
	m_TotalJuniorFees.Aggregate(GenericCashFlow::Monthly);
	m_AnnualizedExcess.Aggregate(GenericCashFlow::Monthly);
	m_EquityIncome.Aggregate(GenericCashFlow::Monthly);
	m_Reinvested.Aggregate(GenericCashFlow::Monthly);

	m_SeniorExpensesFixed.SetDivisor(1.0);
	m_SeniorFeesFixed.SetDivisor(1.0);
	m_JuniorFeesFixed.SetDivisor(1.0);
}
Waterfall::Waterfall(const Waterfall& a)
	:m_SeniorExpenses(a.m_SeniorExpenses)
	,m_SeniorFees(a.m_SeniorFees)
	,m_JuniorFees(a.m_JuniorFees)
	,m_MortgagesPayments(a.m_MortgagesPayments)
	,m_PaymentFrequency(a.m_PaymentFrequency)
	,m_ExcessCashFlow(a.m_ExcessCashFlow)
	,m_TotalSeniorExpenses(a.m_TotalSeniorExpenses)
	,m_TotalSeniorFees(a.m_TotalSeniorFees)
	,m_TotalJuniorFees(a.m_TotalJuniorFees)
	,m_ReinvestmentTest(a.m_ReinvestmentTest)
	,m_CCCTestLimit(a.m_CCCTestLimit)
	,m_CCCcurve(a.m_CCCcurve)
	,m_CCChaircut(a.m_CCChaircut)
	,m_UseTurbo(a.m_UseTurbo)
	,m_PrincipalAvailable(a.m_PrincipalAvailable)
	,m_InterestAvailable(a.m_InterestAvailable)
	,m_JuniorFeesCoupon(a.m_JuniorFeesCoupon)
	,m_AnnualizedExcess(a.m_AnnualizedExcess)
	,m_FirstIPDdate(a.m_FirstIPDdate)
	,m_Reinvested(a.m_Reinvested)
	,m_LastIPDdate(a.m_LastIPDdate)
	,m_CallDate(a.m_CallDate)
	,m_PoolValueAtCall(a.m_PoolValueAtCall)
	,m_UseCall(a.m_UseCall)
	,m_CallMultiple(a.m_CallMultiple)
	,m_CallReserve(a.m_CallReserve)
	,m_CalculatedMtgPayments(a.m_CalculatedMtgPayments)
	,m_EquityIncome(a.m_EquityIncome)
	,m_CumulativeReserves(a.m_CumulativeReserves)
	,m_DealName(a.m_DealName)
	, m_SeniorExpensesFixed(a.m_SeniorExpensesFixed)
	, m_SeniorFeesFixed(a.m_SeniorFeesFixed)
	, m_JuniorFeesFixed(a.m_JuniorFeesFixed)
	, m_GICinterest(a.m_GICinterest)
	, m_StartingDeferredJunFees(a.m_StartingDeferredJunFees)
	, m_GICflows(a.m_GICflows)
	, m_GICBaseRate(a.m_GICBaseRate)
	, m_GICBaseRateValue(a.m_GICBaseRateValue)
{
	m_LoadProtocolVersion = a.m_LoadProtocolVersion;
	for(QList<Tranche*>::const_iterator i=a.m_Tranches.constBegin();i!=a.m_Tranches.constEnd();i++){
		m_Tranches.append(new Tranche(**i));
	}
	for(QList<WatFalPrior*>::const_iterator i=a.m_WaterfallStesps.constBegin();i!=a.m_WaterfallStesps.constEnd();i++){
		m_WaterfallStesps.append(new WatFalPrior(**i));
	}
	for (QList<ReserveFund*>::const_iterator i = a.m_Reserves.constBegin(); i != a.m_Reserves.constEnd(); i++) {
		m_Reserves.append(new ReserveFund(**i));
	}
}
Waterfall& Waterfall::operator=(const Waterfall& a){
	m_SeniorExpenses=a.m_SeniorExpenses;
	m_SeniorFees=a.m_SeniorFees;
	m_JuniorFees=a.m_JuniorFees;
	m_MortgagesPayments=a.m_MortgagesPayments;
	m_PaymentFrequency=a.m_PaymentFrequency;
	m_ExcessCashFlow=a.m_ExcessCashFlow;
	m_TotalSeniorExpenses=a.m_TotalSeniorExpenses;
	m_TotalSeniorFees=a.m_TotalSeniorFees;
	m_TotalJuniorFees=a.m_TotalJuniorFees;
	m_ReinvestmentTest=a.m_ReinvestmentTest;
	m_CCCTestLimit=a.m_CCCTestLimit;
	m_CCCcurve=a.m_CCCcurve;
	m_CCChaircut=a.m_CCChaircut;
	m_UseTurbo=a.m_UseTurbo;
	m_PrincipalAvailable=a.m_PrincipalAvailable;
	m_InterestAvailable=a.m_InterestAvailable;
	m_JuniorFeesCoupon=a.m_JuniorFeesCoupon;
	m_AnnualizedExcess=a.m_AnnualizedExcess;
	m_FirstIPDdate=a.m_FirstIPDdate;
	m_Reinvested=a.m_Reinvested;
	m_LastIPDdate=a.m_LastIPDdate;
	m_CallDate=a.m_CallDate;
	m_PoolValueAtCall=a.m_PoolValueAtCall;
	m_UseCall=a.m_UseCall;
	m_CallMultiple=a.m_CallMultiple;
	m_CallReserve=a.m_CallReserve;
	m_CalculatedMtgPayments=a.m_CalculatedMtgPayments;
	m_EquityIncome=a.m_EquityIncome;
	m_CumulativeReserves=a.m_CumulativeReserves;
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
	ResetTranches();
	for(QList<Tranche*>::const_iterator i=a.m_Tranches.constBegin();i!=a.m_Tranches.constEnd();i++){
		m_Tranches.append(new Tranche(**i));
	}
	ResetSteps();
	for(QList<WatFalPrior*>::const_iterator i=a.m_WaterfallStesps.constBegin();i!=a.m_WaterfallStesps.constEnd();i++){
		m_WaterfallStesps.append(new WatFalPrior(**i));
	}
	RemoveReserve();
	for (QList<ReserveFund*>::const_iterator i = a.m_Reserves.constBegin(); i != a.m_Reserves.constEnd(); i++) {
		m_Reserves.append(new ReserveFund(**i));
	}
	return *this;
}
Waterfall::~Waterfall(){
	ResetSteps();
	ResetTranches();
	RemoveReserve();
}
void Waterfall::ResetSteps(){
	for(QList<WatFalPrior*>::iterator i=m_WaterfallStesps.begin();i!=m_WaterfallStesps.end();i++){
		delete (*i);
	}
	m_WaterfallStesps.clear();
}
void Waterfall::ResetTranches(){
	for(QList<Tranche*>::iterator i=m_Tranches.begin();i!=m_Tranches.end();i++){
		delete (*i);
	}
	m_Tranches.clear();
}
int Waterfall::FindMostJuniorLevel()const{
	if(m_Tranches.isEmpty()) return 0;
	int Result=m_Tranches.last()->GetProrataGroup();
	for(int i=m_Tranches.size()-2;i>=0;i--){
		if(m_Tranches.at(i)->GetProrataGroup()>Result)Result=m_Tranches.at(i)->GetProrataGroup();
	}
	return Result;
}
void Waterfall::FillAllDates(){
	bool AnchorDateNull=m_PaymentFrequency.GetAnchorDate().isNull();
	if (AnchorDateNull) m_PaymentFrequency.SetAnchorDate(m_MortgagesPayments.GetDate(0));
	bool ExcessIsEmpty=m_ExcessCashFlow.Count()==0;
	//All the dates in all the tranches
	for(int i=0;i<m_Tranches.size()-1;i++){
		for(int j=i+1;j<m_Tranches.size();j++){
			for(int h=0;h<m_Tranches.at(i)->GetCashFlow().Count();h++){
				if(m_Tranches.at(j)->GetCashFlow().FindDate(m_Tranches.at(i)->GetCashFlow().GetDate(h)) < 0){
					m_Tranches[j]->AddCashFlow(m_Tranches.at(i)->GetCashFlow().GetDate(h), 0.0, TrancheCashFlow::TrancheFlowType::InterestFlow);
				}
			}
		}
	}
	//dates from excess to all the tranches
	if(!ExcessIsEmpty){
		for(int j=0;j<m_Tranches.size();j++){
			for(int h=0;h<m_ExcessCashFlow.Count();h++){
				if(m_Tranches.at(j)->GetCashFlow().FindDate(m_ExcessCashFlow.GetDate(h)) < 0){
					m_Tranches[j]->AddCashFlow(m_ExcessCashFlow.GetDate(h), 0.0, TrancheCashFlow::TrancheFlowType::InterestFlow);
				}
			}
		}
	}
	//dates from annualized excess to all the tranches (equity income dates included here)
	for(int j=0;j<m_Tranches.size();j++){
		for(int h=0;h<m_AnnualizedExcess.Count();h++){
			if(m_Tranches.at(j)->GetCashFlow().FindDate(m_AnnualizedExcess.GetDate(h)) < 0){
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
	for(int j=0;j<m_Tranches.size();j++){
		for(int h=0;h<m_TotalSeniorFees.Count();h++){
			if(m_Tranches.at(j)->GetCashFlow().FindDate(m_TotalSeniorFees.GetDate(h)) < 0){
				m_Tranches[j]->AddCashFlow(m_TotalSeniorFees.GetDate(h), 0.0, TrancheCashFlow::TrancheFlowType::InterestFlow);
			}
		}
	}
	//dates from junior fees to all the tranches
	for(int j=0;j<m_Tranches.size();j++){
		for(int h=0;h<m_TotalJuniorFees.Count();h++){
			if(m_Tranches.at(j)->GetCashFlow().FindDate(m_TotalJuniorFees.GetDate(h)) < 0){
				m_Tranches[j]->AddCashFlow(m_TotalJuniorFees.GetDate(h), 0.0, TrancheCashFlow::TrancheFlowType::InterestFlow);
			}
		}
	}
	//dates from senior expenses to all the tranches
	for(int j=0;j<m_Tranches.size();j++){
		for(int h=0;h<m_TotalSeniorExpenses.Count();h++){
			if(m_Tranches.at(j)->GetCashFlow().FindDate(m_TotalSeniorExpenses.GetDate(h)) < 0){
				m_Tranches[j]->AddCashFlow(m_TotalSeniorExpenses.GetDate(h), 0.0, TrancheCashFlow::TrancheFlowType::InterestFlow);
			}
		}
	}
	//dates from tranches to excess, fees and reserve
	if(m_Tranches.size()>0){
		for(int h=0;h<m_Tranches.at(0)->GetCashFlow().Count();h++){
			if(!ExcessIsEmpty){
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
	QDate RollingNextIPD=m_FirstIPDdate;
	
	for (int i=0;i<m_CalculatedMtgPayments.Count();i++){
		if(m_CalculatedMtgPayments.GetDate(i).year() > RollingNextIPD.year() || (m_CalculatedMtgPayments.GetDate(i).year() == RollingNextIPD.year() && m_CalculatedMtgPayments.GetDate(i).month() >= RollingNextIPD.month()) || i >= m_CalculatedMtgPayments.Count()-1){
			for(int j=0;j<m_Tranches.size();j++){
				m_Tranches[j]->GetCashFlow().ReplaceDate(m_CalculatedMtgPayments.GetDate(i),RollingNextIPD);
			}
			m_ExcessCashFlow.ReplaceDate(m_CalculatedMtgPayments.GetDate(i),RollingNextIPD);
			m_AnnualizedExcess.ReplaceDate(m_CalculatedMtgPayments.GetDate(i),RollingNextIPD);
			m_TotalSeniorExpenses.ReplaceDate(m_CalculatedMtgPayments.GetDate(i),RollingNextIPD);
			m_TotalSeniorFees.ReplaceDate(m_CalculatedMtgPayments.GetDate(i),RollingNextIPD);
			m_TotalJuniorFees.ReplaceDate(m_CalculatedMtgPayments.GetDate(i),RollingNextIPD);
			for (QList<ReserveFund*>::iterator ResIter = m_Reserves.begin(); ResIter != m_Reserves.end(); ResIter++) {
				if ((*ResIter)->GetReserveFundFlow().Count() == 0) continue;
				(*ResIter)->GetReserveFundFlow().ReplaceDate(m_CalculatedMtgPayments.GetDate(i), RollingNextIPD);
			}
			RollingNextIPD=RollingNextIPD.addMonths(m_PaymentFrequency.GetValue(RollingNextIPD));
		}
	}
	if (AnchorDateNull) m_PaymentFrequency.RemoveAnchorDate();
}
int Waterfall::FindTrancheIndex(const QString& Tranchename)const{
	for(int j=0;j<m_Tranches.size();j++){
		if(m_Tranches.at(j)->GetTrancheName()==Tranchename) return j;
		if(m_Tranches.at(j)->GetISIN()==Tranchename) return j;
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
double Waterfall::GetCreditEnhancement(int TrancheIndex,int TimeIndex)const{
	if(TrancheIndex<-1 || TrancheIndex>=m_Tranches.size()) return 0.0;
	int TargetSeniority = m_Tranches.at(TrancheIndex)->GetProrataGroup();
	double Runningsum=0.0;
	double ReserveSum;
	for(int i=0;i<m_Tranches.size();i++){
		if(m_Tranches.at(i)->GetProrataGroup()<=TargetSeniority){
			if(TimeIndex>=0){
				Runningsum+=m_Tranches.at(i)->GetCashFlow().GetAmountOutstanding(TimeIndex);
			}
			else{
				Runningsum+=m_Tranches.at(i)->GetOutstandingAmt();
			}
		}
	}
	if(Runningsum==0.0) return 1.0;
	if(TimeIndex>=0){
		ReserveSum=m_CalculatedMtgPayments.GetAmountOut(m_Tranches.first()->GetCashFlow().GetDate(TimeIndex));
		for (QList<ReserveFund*>::const_iterator ResIter = m_Reserves.constBegin(); ResIter != m_Reserves.constEnd(); ResIter++) {
			if ((*ResIter)->GetReserveFundFlow().Count() > 0 && TrancheIndex <= (*ResIter)->GetReserveFundFreed())
				ReserveSum += (*ResIter)->GetReserveFundCurrent((*ResIter)->GetReserveFundFlow().GetDate(TimeIndex));
		}
		if(ReserveSum<=0.0) return 0.0;
		return 1.0-(Runningsum/ReserveSum); //(m_CalculatedMtgPayments.GetAmountOut(m_Tranches.first()->GetCashFlow().GetDate(TimeIndex))/Runningsum)-1.0;
	}else{
		ReserveSum=m_CalculatedMtgPayments.GetAmountOut(0);
		ReserveSum += m_PrincipalAvailable.Total();
		for (QList<ReserveFund*>::const_iterator ResIter = m_Reserves.constBegin(); ResIter != m_Reserves.constEnd(); ResIter++) {
			if (TrancheIndex<=(*ResIter)->GetReserveFundFreed())
				ReserveSum += (*ResIter)->GetReserveFundCurrent();
		}
		if(ReserveSum<=0.0) return 0.0;
		return 1.0-(Runningsum/ReserveSum); //((m_CalculatedMtgPayments.GetAmountOut(0)+m_PrincipalAvailable)/Runningsum)-1.0;
	}
}
double Waterfall::GroupOutstanding(int GroupTarget)const{
	double Result=0.0;
	for(QList<Tranche*>::const_iterator i=m_Tranches.begin();i!=m_Tranches.end();i++){
		if((*i)->GetProrataGroup()==GroupTarget) Result+=(*i)->GetCashFlow().GetAmountOutstanding(0);
	}
	return Result;
}


double Waterfall::GroupWACoupon(int GroupTarget, const QDate& Period, qint32 CouponType/*=0*/) const {
	double RunningSum = 0.0;
	double Result = 0.0;
	for (QList<Tranche*>::const_iterator i = m_Tranches.begin(); i != m_Tranches.end(); i++) {
		if ((*i)->GetProrataGroup() == GroupTarget) {
			Result += (*i)->GetCoupon(Period, CouponType)*(*i)->GetCashFlow().GetAmountOutstanding(Period);
			RunningSum += (*i)->GetCashFlow().GetAmountOutstanding(Period);
		}
	}
	if (RunningSum > 0) return Result / RunningSum;
	else return 0.0;
}

double Waterfall::GroupWACoupon(int GroupTarget, const QDate& Period, QList<qint32> CouponTypes) const {
	if (CouponTypes.isEmpty()) {
		for (QList<Tranche*>::const_iterator i = m_Tranches.begin(); i != m_Tranches.end(); i++) {
			if ((*i)->GetProrataGroup() == GroupTarget) {
				CouponTypes.append((*i)->GetCouponIndexes());
			}
		}
		CouponTypes = CouponTypes.toSet().toList();
	}
	double RunningSum = 0.0;
	double Result = 0.0;
	double CurrentCoup;
	for (QList<Tranche*>::const_iterator i = m_Tranches.begin(); i != m_Tranches.end(); i++) {
		if ((*i)->GetProrataGroup() == GroupTarget) {
			CurrentCoup = 0.0;
			foreach(qint32 CouponType, CouponTypes){
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
	, const QString& FloatingBase 
	, const QString& RecoveryLag 
	, const QString& Delinquency 
	, const QString& DelinquencyLag
	) {
	m_ReinvestmentTest.SetupReinvBond(IntrVec, CPRVec, CDRVec, LSVec, WALval, PayFreq, AnnuityVec, ReinvPric, ReinvDel, FloatingBase, RecoveryLag, Delinquency, DelinquencyLag);
}
void Waterfall::SetupReinvestmentTest(const QDate& ReinvPer,double TstLvl, double IIshare,double IRshare,double OIshare,double ORshare){
	m_ReinvestmentTest.SetupTest(ReinvPer,TstLvl,IIshare,IRshare,OIshare,ORshare);
}
double Waterfall::RedeemNotes(double AvailableFunds, int GroupTarget, const QDate& TargetDate) {
	if(AvailableFunds<0.01) return 0.0;
	/*QDate TargetDate(m_MortgagesPayments.GetDate(PeriodIndex));*/
	QQueue<int> ProRataBonds;
	double TotalPayable=0.0;
	for(int i=0;i<m_Tranches.size();i++){
		if(m_Tranches.at(i)->GetProrataGroup()==GroupTarget){
			TotalPayable+=m_Tranches.at(i)->GetCurrentOutstanding();
			ProRataBonds.enqueue(i);
		}
	}
	if(TotalPayable<=0.0) return AvailableFunds;
	if(AvailableFunds>=TotalPayable){
		while(ProRataBonds.size()>0){
			int ProRataIndex=ProRataBonds.dequeue();
			m_Tranches[ProRataIndex]->AddCashFlow(
				TargetDate,
				m_Tranches.at(ProRataIndex)->GetCurrentOutstanding(),
				TrancheCashFlow::TrancheFlowType::PrincipalFlow
			);
		}
		return AvailableFunds-TotalPayable;
	}
	while(ProRataBonds.size()>0){
		int ProRataIndex=ProRataBonds.dequeue();
		m_Tranches[ProRataIndex]->AddCashFlow(
			TargetDate,
			AvailableFunds * (m_Tranches.at(ProRataIndex)->GetCurrentOutstanding() /TotalPayable),
			TrancheCashFlow::TrancheFlowType::PrincipalFlow
		);
	}
	return 0.0;
}
double Waterfall::RedeemSequential(double AvailableFunds, const QDate& TargetDate, int MaxGroup) {
	if(AvailableFunds<0.01) return 0.0;
	if(MaxGroup<=0) MaxGroup=FindMostJuniorLevel();
	for( int CurrentSeniority=1;CurrentSeniority<=MaxGroup && AvailableFunds>=0.01;CurrentSeniority++){
		AvailableFunds = RedeemNotes(AvailableFunds, CurrentSeniority, TargetDate);
	}
	return AvailableFunds;
}
double Waterfall::RedeemProRata(double AvailableFunds, const QDate& TargetDate, QList<int> Groups) {
	int MostJunior=FindMostJuniorLevel();
	if(Groups.isEmpty()){
		for(int i=1;i<=MostJunior;i++) Groups.append(i);
	}
	if(Groups.size()==1){
		for(int i=1;i<=MostJunior && i<=Groups.first();i++) Groups.append(i);
	}
	double TotalPayable=0.0;
	QHash<int,double> GroupSum;
	for(int i=0;i<m_Tranches.size();i++){
		if(Groups.contains(m_Tranches.at(i)->GetProrataGroup())){
			TotalPayable += m_Tranches.at(i)->GetCashFlow().GetAmountOutstanding(TargetDate);
			if (GroupSum.contains(m_Tranches.at(i)->GetProrataGroup()))
				GroupSum[m_Tranches.at(i)->GetProrataGroup()] += m_Tranches.at(i)->GetCashFlow().GetAmountOutstanding(TargetDate);
			else
				GroupSum.insert(m_Tranches.at(i)->GetProrataGroup(), m_Tranches.at(i)->GetCashFlow().GetAmountOutstanding(TargetDate));
		}
	}
	if (TotalPayable < 0.01) return AvailableFunds;
	double RemainingFunds=0.0;
	foreach(int CurrentSeniority, Groups){
		RemainingFunds+=RedeemNotes(
			AvailableFunds*GroupSum.value(CurrentSeniority)/TotalPayable
			, CurrentSeniority, TargetDate);
	}
	if (RemainingFunds > 0.01) {
		if (qAbs(RemainingFunds - AvailableFunds) < 0.01) return RemainingFunds;
		return RedeemProRata(RemainingFunds, TargetDate, Groups);
	}
	return 0.0;
}
double Waterfall::GetWACostOfCapital(int index)const{
	double RunningSum=0.0;
	double Result=0.0;
	if(m_Tranches.isEmpty()) return 0.0;
	if(index<0 || index>=m_Tranches.first()->GetCashFlow().Count()) return 0.0;
	for (QList<Tranche*>::const_iterator i=m_Tranches.begin();i!=m_Tranches.end();i++){
		if((*i)->GetTotalCoupon((*i)->GetCashFlow().GetDate(index))>0){
			Result += ((*i)->GetTotalCoupon((*i)->GetCashFlow().GetDate(index))) * ((*i)->GetCashFlow().GetAmountOutstanding(index));
			RunningSum+=(*i)->GetCashFlow().GetAmountOutstanding(index);
		}
	}
	if(qAbs(RunningSum)<0.01) return 0.0;
	return Result/RunningSum;
}

bool Waterfall::CalculateTranchesCashFlows(){
		if (!ReadyToCalculate().isEmpty()) {
			PrintToTempFile("ReturnFalse.txt", "Not Ready To Calculate");
			return false;
		}
		{//Check if all base rates are valid
			bool KeepSearching = false;
			ConstantBaseRateTable TempTable;
			if (m_GICBaseRateValue.IsEmpty()) {
				m_GICBaseRateValue = m_GICBaseRate.CompileReferenceRateValue(TempTable);
				if (m_GICBaseRateValue.IsEmpty()) {
					PrintToTempFile("ReturnFalse.txt", "Missing Base Rate Value");
					return false;
				}
			}
			for (QList<Tranche*>::const_iterator SingleTranche = m_Tranches.constBegin(); SingleTranche != m_Tranches.constEnd(); ++SingleTranche) {
				for (QHash<qint32, BloombergVector*>::const_iterator SingleRate = (*SingleTranche)->GetRefRateValues().constBegin();SingleRate != (*SingleTranche)->GetRefRateValues().constEnd(); ++SingleRate) {
					if (SingleRate.value()->IsEmpty()) {
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
		double CheckResults = 0.0;
		PrincipalRecip AvailablePrincipal;
		double AvailableInterest;
		double TotalPayable;
		QQueue<int> ProRataBonds;
		double TestTarget;
		//int CurrentSeniority;
		double Solution;
		int SolutionDegree;
		//double CurrentWArate;
		double AdjustedCoupon;
		double InterestPayableBefore;
		QDate RollingNextIPD=m_FirstIPDdate;
		QDate RollingLastIPD=m_LastIPDdate;
		double adjSeniorExpenses;
		double adjSeniorFees;
		double adjJuniorFees;
		double OriginalAvailableInterest=m_InterestAvailable;
		PrincipalRecip OriginalAvailablePrincipal=m_PrincipalAvailable;
		double OriginalStartingDefJunFees = m_StartingDeferredJunFees;
		double* OriginalReserveLevel;
		bool IsCallPaymentDate=false;
		double ActualCallReserveLevel=0.0;
		QDate CurrentDate;
		int PeriodsCounter=0;
		MtgCashFlow OriginalMtgFlows=m_MortgagesPayments;
		double* OriginalStartingDeferred = new double[m_Tranches.size()];
		for (int i = 0; i < m_Tranches.size(); ++i) {
			OriginalStartingDeferred[i] = m_Tranches.at(i)->GetStartingDeferredInterest();
		}
		m_ExcessCashFlow.Clear();
		m_TotalSeniorExpenses.Clear();
		m_TotalSeniorFees.Clear();
		m_TotalJuniorFees.Clear();
		m_AnnualizedExcess.Clear();
		m_EquityIncome.Clear();
		m_Reinvested.Clear();
		bool NullCCCanchor[]={
			m_CCCcurve.GetAnchorDate().isNull()
			,m_PaymentFrequency.GetAnchorDate().isNull()
			, m_SeniorExpenses.GetAnchorDate().isNull()
			, m_SeniorFees.GetAnchorDate().isNull()
			, m_JuniorFees.GetAnchorDate().isNull()
			, m_SeniorExpensesFixed.GetAnchorDate().isNull()
			, m_SeniorFeesFixed.GetAnchorDate().isNull()
			, m_JuniorFeesFixed.GetAnchorDate().isNull()
			, m_GICinterest.GetAnchorDate().isNull()
			, m_GICBaseRateValue.GetAnchorDate().isNull()
		};
		if(NullCCCanchor[0]) m_CCCcurve.SetAnchorDate(m_MortgagesPayments.GetDate(0));
		if(NullCCCanchor[1]) m_PaymentFrequency.SetAnchorDate(m_MortgagesPayments.GetDate(0));
		if (NullCCCanchor[2]) m_SeniorExpenses.SetAnchorDate(m_MortgagesPayments.GetDate(0));
		if (NullCCCanchor[3]) m_SeniorFees.SetAnchorDate(m_MortgagesPayments.GetDate(0));
		if (NullCCCanchor[4]) m_JuniorFees.SetAnchorDate(m_MortgagesPayments.GetDate(0));
		if (NullCCCanchor[5]) m_SeniorExpensesFixed.SetAnchorDate(m_MortgagesPayments.GetDate(0));
		if (NullCCCanchor[6]) m_SeniorFeesFixed.SetAnchorDate(m_MortgagesPayments.GetDate(0));
		if (NullCCCanchor[7]) m_JuniorFeesFixed.SetAnchorDate(m_MortgagesPayments.GetDate(0));
		if (NullCCCanchor[8]) m_GICinterest.SetAnchorDate(m_MortgagesPayments.GetDate(0));
		if (NullCCCanchor[9]) m_GICBaseRateValue.SetAnchorDate(m_MortgagesPayments.GetDate(0));
		foreach(ReserveFund* SingleRes, m_Reserves) {
			CheckResults -= SingleRes->GetStartingReserve();
			SingleRes->ClearFlows();
		}
		foreach(Tranche* SingleTranche, m_Tranches) {
			SingleTranche->GetCashFlow().Clear();
		}
		for(int i=0;i<m_MortgagesPayments.Count();i++){
			CurrentDate=m_MortgagesPayments.GetDate(i);
			CheckResults -= m_MortgagesPayments.GetScheduled(i) + m_MortgagesPayments.GetPrepay(i) + m_MortgagesPayments.GetInterest(i);
			if (i > 0) {
				TotalPayable = AdjustCoupon(m_GICinterest.GetValue(CurrentDate) + m_GICBaseRateValue.GetValue(CurrentDate), m_MortgagesPayments.GetDate(i - 1), m_MortgagesPayments.GetDate(i), DayCountConvention::ACT360);
				m_GICflows.AddFlow(CurrentDate, m_InterestAvailable*TotalPayable, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow));
				m_GICflows.AddFlow(CurrentDate, m_PrincipalAvailable.GetScheduled()*TotalPayable, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PrincipalFlow));
				m_GICflows.AddFlow(CurrentDate, m_PrincipalAvailable.GetPrepay()*TotalPayable, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PrincipalFlow));
				m_InterestAvailable +=
					m_GICflows.GetFlow(CurrentDate, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow))
					+ m_GICflows.GetFlow(CurrentDate, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PrincipalFlow))
				;
			}
			m_PrincipalAvailable.AddScheduled(m_MortgagesPayments.GetScheduled(i));
			m_PrincipalAvailable.AddPrepay(m_MortgagesPayments.GetPrepay(i));
			m_InterestAvailable+=m_MortgagesPayments.GetInterest(i);
			if(m_UseCall && !IsCallPaymentDate){
				ActualCallReserveLevel=0.0;
				TotalPayable=0.0;
				if(m_CallReserve>0 && m_CallMultiple>0){
					foreach(Tranche* SingleTranche, m_Tranches){
						TotalPayable+=SingleTranche->GetCurrentOutstanding();
						if(SingleTranche->GetProrataGroup()>=m_CallReserve) ActualCallReserveLevel+=SingleTranche->GetCurrentOutstanding();
					}
					if(ActualCallReserveLevel==0.0)ActualCallReserveLevel=m_CallReserve;
					else {
						for (QList<ReserveFund*>::const_iterator ResIter = m_Reserves.constBegin(); ResIter != m_Reserves.constEnd(); ResIter++)
							ActualCallReserveLevel += (*ResIter)->GetReserveFundCurrent();
					}
					ActualCallReserveLevel*=m_CallMultiple;
					IsCallPaymentDate=ActualCallReserveLevel>=TotalPayable-m_PrincipalAvailable.Total();
				}
				IsCallPaymentDate= IsCallPaymentDate || (!m_CallDate.isNull() && CurrentDate>=m_CallDate);
			}
			if((CurrentDate.year()<RollingNextIPD.year() || (CurrentDate.year()==RollingNextIPD.year() && CurrentDate.month()<RollingNextIPD.month())) && i<m_MortgagesPayments.Count()-1){
				//This is not a Tranche payment date
				int ReinvestRightAway=-1;
				foreach(WatFalPrior* SingleStep,m_WaterfallStesps){
					if(SingleStep->GetPriorityType()==WatFalPrior::wst_ReinvestPrincipal){
						ReinvestRightAway = SingleStep->GetRedemptionGroup();
						break;
					}
				}
				if(CurrentDate<=m_ReinvestmentTest.GetReinvestmentPeriod() && ReinvestRightAway>=0 && !IsCallPaymentDate){
					//during reinvestment period
					//reinvest
					if(m_PrincipalAvailable.Total()>0.0){
						double PayablePrincipal; 
						if (ReinvestRightAway == 1){  //Reinvest Prepay Only
							PayablePrincipal = m_PrincipalAvailable.GetPrepay();
							m_PrincipalAvailable.SetPrepay(0.0);
						}
						else if (ReinvestRightAway == 2){  //Reinvest scheduled only
							PayablePrincipal = m_PrincipalAvailable.GetScheduled();
							m_PrincipalAvailable.SetScheduled(0.0);
						}
						else {
							PayablePrincipal = m_PrincipalAvailable.Total(); //Reinvest all principal
							m_PrincipalAvailable.Erase();
						}
						m_ReinvestmentTest.CalculateBondCashFlows(PayablePrincipal, CurrentDate, i);
						m_MortgagesPayments.AddFlow(m_ReinvestmentTest.GetBondCashFlow());
						m_Reinvested.AddFlow(CurrentDate, PayablePrincipal, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PrincipalFlow));
						m_InterestAvailable+=m_ReinvestmentTest.GetBondCashFlow().GetInterest(CurrentDate);
						m_PrincipalAvailable.AddScheduled(m_ReinvestmentTest.GetBondCashFlow().GetScheduled(CurrentDate));
						m_PrincipalAvailable.AddPrepay(m_ReinvestmentTest.GetBondCashFlow().GetPrepay(CurrentDate));
						CheckResults -= m_ReinvestmentTest.GetBondCashFlow().GetScheduled(CurrentDate) + m_ReinvestmentTest.GetBondCashFlow().GetPrepay(CurrentDate) + m_ReinvestmentTest.GetBondCashFlow().GetInterest(CurrentDate);
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
				if (m_PoolValueAtCall.IsEmpty())
					TotalPayable = m_MortgagesPayments.GetWAPrice(i);
				else
					TotalPayable = m_PoolValueAtCall.GetValue(CurrentDate);
				AvailablePrincipal += TotalPayable*m_MortgagesPayments.GetAmountOut(i);
				CheckResults -= TotalPayable*m_MortgagesPayments.GetAmountOut(i);
			}
			foreach(WatFalPrior* SingleStep,m_WaterfallStesps){//Cycle through the steps of the waterfall
				switch(SingleStep->GetPriorityType()){
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				case WatFalPrior::wst_SeniorExpenses:
					adjSeniorExpenses = AdjustCoupon(m_SeniorExpenses.GetValue(CurrentDate),RollingLastIPD,RollingNextIPD,DayCountConvention::ACT360);
					TotalPayable=adjSeniorExpenses*m_MortgagesPayments.GetAmountOut(i)
						- m_TotalSeniorExpenses.GetFlow(CurrentDate, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow))
						- m_TotalSeniorExpenses.GetFlow(CurrentDate, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PrincipalFlow))
					;
					TotalPayable += m_SeniorExpensesFixed.GetValue(CurrentDate);
					if(TotalPayable>=0.01){
						if(SingleStep->GetRedemptionGroup()==1){
							m_TotalSeniorExpenses.AddFlow(CurrentDate, qMin(AvailableInterest, TotalPayable), static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow));
							AvailableInterest=qMax(0.0,AvailableInterest-TotalPayable);
						}
						else if(SingleStep->GetRedemptionGroup()==2){
							m_TotalSeniorExpenses.AddFlow(CurrentDate, qMin(AvailablePrincipal.Total(), TotalPayable), static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PrincipalFlow));
							AvailablePrincipal-=TotalPayable;
							if (AvailablePrincipal < 0.0) AvailablePrincipal.Erase();
						}
					}
				break;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				case WatFalPrior::wst_SeniorFees:
					adjSeniorFees = AdjustCoupon(m_SeniorFees.GetValue(CurrentDate), RollingLastIPD, RollingNextIPD, DayCountConvention::ACT360);
					TotalPayable=adjSeniorFees*m_MortgagesPayments.GetAmountOut(i)
						- m_TotalSeniorFees.GetFlow(CurrentDate, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow))
						- m_TotalSeniorFees.GetFlow(CurrentDate, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PrincipalFlow))
						;
					TotalPayable += m_SeniorFeesFixed.GetValue(CurrentDate);
					if(TotalPayable>=0.01){
						if(SingleStep->GetRedemptionGroup()==1){
							m_TotalSeniorFees.AddFlow(CurrentDate, qMin(AvailableInterest, TotalPayable), static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow));
							AvailableInterest=qMax(0.0,AvailableInterest-TotalPayable);
						}
						else if(SingleStep->GetRedemptionGroup()==2){
							m_TotalSeniorFees.AddFlow(CurrentDate, qMin(AvailablePrincipal.Total(), TotalPayable), static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PrincipalFlow));
							AvailablePrincipal -= TotalPayable;
							if (AvailablePrincipal < 0.0) AvailablePrincipal.Erase();
						}
					}
				break;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				case WatFalPrior::wst_juniorFees:
					adjJuniorFees = AdjustCoupon(m_JuniorFees.GetValue(CurrentDate), RollingLastIPD, RollingNextIPD, DayCountConvention::ACT360);
					TotalPayable= (adjJuniorFees*m_MortgagesPayments.GetAmountOut(i))
						- m_TotalJuniorFees.GetFlow(CurrentDate, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow))
						- m_TotalJuniorFees.GetFlow(CurrentDate, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PrincipalFlow))
						+ ((1.0 + AdjustCoupon(m_JuniorFeesCoupon,RollingLastIPD,RollingNextIPD, DayCountConvention::ACT360))*m_TotalJuniorFees.GetPreviousFlow(CurrentDate, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::DeferredFlow)))
						+ ((1.0 + AdjustCoupon(m_JuniorFeesCoupon, RollingLastIPD, RollingNextIPD, DayCountConvention::ACT360))*m_StartingDeferredJunFees)
					;
					m_StartingDeferredJunFees = 0.0;
					TotalPayable += m_JuniorFeesFixed.GetValue(CurrentDate);
					TotalPayable=qMax(TotalPayable,0.0);
					if(SingleStep->GetRedemptionGroup()==1){
						m_TotalJuniorFees.AddFlow(CurrentDate, qMin(AvailableInterest, TotalPayable), static_cast<double>(TrancheCashFlow::TrancheFlowType::InterestFlow));
						m_AnnualizedExcess.AddFlow(CurrentDate, qMin(AvailableInterest, TotalPayable), static_cast<double>(TrancheCashFlow::TrancheFlowType::InterestFlow));
						if(AvailableInterest<TotalPayable){
							m_TotalJuniorFees.SetFlow(CurrentDate, TotalPayable - AvailableInterest, static_cast<double>(TrancheCashFlow::TrancheFlowType::DeferredFlow));
						}
						AvailableInterest -= qMin(AvailableInterest, TotalPayable);
					}
					else if(SingleStep->GetRedemptionGroup()==2){
						m_TotalJuniorFees.AddFlow(CurrentDate, qMin(AvailablePrincipal.Total(), TotalPayable), static_cast<double>(TrancheCashFlow::TrancheFlowType::PrincipalFlow));
						m_AnnualizedExcess.AddFlow(CurrentDate, qMin(AvailablePrincipal.Total(), TotalPayable), static_cast<double>(TrancheCashFlow::TrancheFlowType::PrincipalFlow));
						if(AvailablePrincipal.Total()<TotalPayable){
							m_TotalJuniorFees.SetFlow(CurrentDate, TotalPayable - AvailablePrincipal.Total(), static_cast<double>(TrancheCashFlow::TrancheFlowType::DeferredFlow));
						}
						AvailablePrincipal -= qMin(AvailablePrincipal.Total(),TotalPayable);
					}
				break;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				case WatFalPrior::wst_ReserveReplenish:
					{
						if (SingleStep->GetGroupTarget() < 1 || SingleStep->GetGroupTarget() > m_Reserves.size()) {
							PrintToTempFile("ReturnFalse.txt", "Reserve Fund Fail");
							return false;
						}
						double FreedAmnt, StacedRsAmt=0.0;
						int MostJun=FindMostJuniorLevel();
						
						if (m_Reserves.at(SingleStep->GetGroupTarget() - 1)->GetReserveFundFreed() == 0 || m_Reserves.at(SingleStep->GetGroupTarget() - 1)->GetReserveFundFreed()>MostJun)
							FreedAmnt=1.0;
						else{
							FreedAmnt=0.0;
							for(int h=0;h<m_Tranches.size();h++){
								if (m_Tranches.at(h)->GetProrataGroup() <= m_Reserves.at(SingleStep->GetGroupTarget() - 1)->GetReserveFundFreed())
									FreedAmnt+=m_Tranches.at(h)->GetCurrentOutstanding();
							}
						}
						if(i<m_MortgagesPayments.Count()-1 && !IsCallPaymentDate && FreedAmnt>0.01){
							TestTarget=0.0;
							if (qRound(m_Reserves.at(SingleStep->GetGroupTarget() - 1)->GetReserveFundTarget().GetValue(CurrentDate))>MostJun) TestTarget = m_Reserves.at(SingleStep->GetGroupTarget() - 1)->GetReserveFundTarget().GetValue(CurrentDate);
							else{
								foreach(Tranche* SingleTranche, m_Tranches){
									if (SingleTranche->GetProrataGroup() <= qRound(m_Reserves.at(SingleStep->GetGroupTarget() - 1)->GetReserveFundTarget().GetValue(CurrentDate))) TestTarget += SingleTranche->GetCurrentOutstanding();
								}
							}
							if(m_CumulativeReserves){
								for (int ResIter = 0; ResIter < SingleStep->GetGroupTarget() - 1; ResIter++)
									StacedRsAmt += m_Reserves.at(ResIter)->GetReserveFundCurrent();
							}
							else StacedRsAmt = m_Reserves.at(SingleStep->GetGroupTarget() - 1)->GetReserveFundCurrent();
							TotalPayable = qMax(
								m_Reserves.at(SingleStep->GetGroupTarget() - 1)->GetReserveFundFloor().GetValue(CurrentDate),
								TestTarget*m_Reserves.at(SingleStep->GetGroupTarget() - 1)->GetReserveFundMultiple().GetValue(CurrentDate)
								);
							if (m_Reserves.at(SingleStep->GetGroupTarget() - 1)->GetReserveFundCap().GetValue(CurrentDate) > 0.0) 
								TotalPayable = qMin(TotalPayable, m_Reserves.at(SingleStep->GetGroupTarget() - 1)->GetReserveFundCap().GetValue(CurrentDate));
							TotalPayable -= StacedRsAmt;

							if(TotalPayable>=0.01){
								if(SingleStep->GetRedemptionGroup()==1){
									m_Reserves[SingleStep->GetGroupTarget() - 1]->GetReserveFundFlow().AddFlow(CurrentDate, qMin(AvailableInterest, TotalPayable), static_cast<qint32>(ReserveFund::ReserveFlowsType::ReplenishFromInterest));
									m_Reserves[SingleStep->GetGroupTarget() - 1]->GetReserveFundFlow().SetFlow(CurrentDate, qMax(0.0, TotalPayable - AvailableInterest), static_cast<qint32>(ReserveFund::ReserveFlowsType::ShortFall));
									m_Reserves[SingleStep->GetGroupTarget() - 1]->SetReserveFundCurrent(m_Reserves.at(SingleStep->GetGroupTarget() - 1)->GetReserveFundCurrent() + qMin(AvailableInterest, TotalPayable));
									AvailableInterest=qMax(0.0,AvailableInterest-TotalPayable);
								}
								else if(SingleStep->GetRedemptionGroup()==2){

									m_Reserves[SingleStep->GetGroupTarget() - 1]->GetReserveFundFlow().AddFlow(CurrentDate, qMin(AvailablePrincipal.Total(), TotalPayable), static_cast<qint32>(ReserveFund::ReserveFlowsType::ReplenishFromPrincipal));
									m_Reserves[SingleStep->GetGroupTarget() - 1]->GetReserveFundFlow().SetFlow(CurrentDate, qMax(0.0, TotalPayable - AvailablePrincipal.Total()), static_cast<qint32>(ReserveFund::ReserveFlowsType::ShortFall));
									m_Reserves[SingleStep->GetGroupTarget() - 1]->SetReserveFundCurrent(m_Reserves.at(SingleStep->GetGroupTarget() - 1)->GetReserveFundCurrent() + qMin(AvailablePrincipal.Total(), TotalPayable));
									AvailablePrincipal -= TotalPayable;
									if (AvailablePrincipal < 0.0) AvailablePrincipal.Erase();
								}
							}
						}
					}
					break;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				case WatFalPrior::wst_Interest:
					ProRataBonds.clear();
					TotalPayable=0.0;
					for(int h=0;h<m_Tranches.size();h++){
						if(m_Tranches.at(h)->GetProrataGroup()==SingleStep->GetGroupTarget()){
							ProRataBonds.enqueue(h);
							AdjustedCoupon = AdjustCoupon(m_Tranches.at(h)->GetCoupon(CurrentDate, qMin(SingleStep->GetRedemptionGroup() - 1, 0)), RollingLastIPD, RollingNextIPD, m_Tranches.at(h)->GetDayCount());
							if (m_Tranches.at(h)->GetStartingDeferredInterest()>=0.01) {
								Solution = m_Tranches.at(h)->GetStartingDeferredInterest();
							}
							else
								Solution = m_Tranches.at(h)->GetCashFlow().GetPreviousFlow(CurrentDate, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::DeferredFlow) | (qMin(SingleStep->GetRedemptionGroup() - 1,0)));
							TotalPayable+=AdjustedCoupon*(Solution+m_Tranches.at(h)->GetCurrentOutstanding());
							m_Tranches[h]->AddCashFlow(CurrentDate, Solution, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::DeferredFlow) | (qMin(SingleStep->GetRedemptionGroup() - 1,0)));
						}
					}
					while (ProRataBonds.size()>0) {
						AdjustedCoupon = AdjustCoupon(m_Tranches.at(ProRataBonds.head())->GetCoupon(CurrentDate, qMin(SingleStep->GetRedemptionGroup() - 1, 0)), RollingLastIPD, RollingNextIPD, m_Tranches.at(ProRataBonds.head())->GetDayCount());
						if (m_Tranches.at(ProRataBonds.head())->GetStartingDeferredInterest() >= 0.01) {
							Solution = AdjustedCoupon* m_Tranches.at(ProRataBonds.head())->GetStartingDeferredInterest();
							m_Tranches[ProRataBonds.head()]->SetStartingDeferredInterest(0.0);
						}
						else
							Solution = AdjustedCoupon* m_Tranches.at(ProRataBonds.head())->GetCashFlow().GetPreviousFlow(CurrentDate, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::DeferredFlow) | (qMin(SingleStep->GetRedemptionGroup() - 1, 0)));
						Solution += AdjustedCoupon*(m_Tranches.at(ProRataBonds.head())->GetCurrentOutstanding());
						if (AvailableInterest >= TotalPayable) {
							m_Tranches[ProRataBonds.dequeue()]->AddCashFlow(CurrentDate, Solution, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow) | (qMin(SingleStep->GetRedemptionGroup() - 1, 0)));
						}
						else {
							int ProrataIndex = ProRataBonds.dequeue();
							m_Tranches[ProrataIndex]->AddCashFlow(CurrentDate, AvailableInterest*Solution / TotalPayable, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow) | (qMin(SingleStep->GetRedemptionGroup() - 1, 0)));
							m_Tranches[ProrataIndex]->AddCashFlow(CurrentDate, Solution - (AvailableInterest*Solution / TotalPayable), static_cast<qint32>(TrancheCashFlow::TrancheFlowType::DeferredFlow) | (qMin(SingleStep->GetRedemptionGroup() - 1, 0)));
						}
					}
					AvailableInterest = qMax(AvailableInterest - TotalPayable, 0.0);
				break;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				case WatFalPrior::wst_DeferredInterest:
				case WatFalPrior::wst_DeferredPrinc:
					ProRataBonds.clear();
					TotalPayable=0.0;
					for(int h=0;h<m_Tranches.size();h++){
						if(m_Tranches.at(h)->GetProrataGroup()==SingleStep->GetGroupTarget()){
							ProRataBonds.enqueue(h);
							TotalPayable += m_Tranches.at(h)->GetCashFlow().GetFlow(CurrentDate, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::DeferredFlow) | (qMin(SingleStep->GetRedemptionGroup() - 1, 0)));
						}
					}
					if(TotalPayable>0.0){
						if(SingleStep->GetPriorityType()==WatFalPrior::wst_DeferredInterest){
							Solution=AvailableInterest;
							AvailableInterest=qMax(AvailableInterest-TotalPayable,0.0);
						}
						else{
							Solution=AvailablePrincipal.Total();
							AvailablePrincipal -= TotalPayable;
							if (AvailablePrincipal < 0.0) AvailablePrincipal.Erase();
						}
						while(ProRataBonds.size()>0){
							if(Solution>=TotalPayable){
								int ProrataIndex=ProRataBonds.dequeue();
								m_Tranches[ProrataIndex]->AddCashFlow(CurrentDate, m_Tranches.at(ProrataIndex)->GetCashFlow().GetFlow(CurrentDate, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::DeferredFlow) | (qMin(SingleStep->GetRedemptionGroup() - 1, 0))), static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow) | (qMin(SingleStep->GetRedemptionGroup() - 1, 0)));
								m_Tranches[ProrataIndex]->AddCashFlow(CurrentDate, -m_Tranches.at(ProrataIndex)->GetCashFlow().GetFlow(CurrentDate, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::DeferredFlow) | (qMin(SingleStep->GetRedemptionGroup() - 1, 0))), static_cast<qint32>(TrancheCashFlow::TrancheFlowType::DeferredFlow) | (qMin(SingleStep->GetRedemptionGroup() - 1, 0)));
							}
							else{
								int ProrataIndex=ProRataBonds.dequeue();
								m_Tranches[ProrataIndex]->AddCashFlow(CurrentDate, (Solution / TotalPayable)*m_Tranches.at(ProrataIndex)->GetCashFlow().GetFlow(CurrentDate, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::DeferredFlow) | (qMin(SingleStep->GetRedemptionGroup() - 1, 0))), static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow) | (qMin(SingleStep->GetRedemptionGroup() - 1, 0)));
								m_Tranches[ProrataIndex]->AddCashFlow(CurrentDate, -(Solution / TotalPayable)*m_Tranches.at(ProrataIndex)->GetCashFlow().GetFlow(CurrentDate, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::DeferredFlow) | (qMin(SingleStep->GetRedemptionGroup() - 1, 0))), static_cast<qint32>(TrancheCashFlow::TrancheFlowType::DeferredFlow) | (qMin(SingleStep->GetRedemptionGroup() - 1, 0)));
							}
						}
					}
				break;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				case  WatFalPrior::wst_Principal:
					TotalPayable = RedeemNotes(AvailablePrincipal.Total(), SingleStep->GetGroupTarget(), CurrentDate);
					AvailablePrincipal -= AvailablePrincipal.Total() - TotalPayable;
				break;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				case  WatFalPrior::wst_RedeemProRata:
					ProRataBonds.clear();
					for (int SigleGroup = qMin(SingleStep->GetGroupTarget(), SingleStep->GetRedemptionGroup()); SigleGroup <= qMax(SingleStep->GetGroupTarget(), SingleStep->GetRedemptionGroup()); ++SigleGroup)
						ProRataBonds.append(SigleGroup);
					TotalPayable = RedeemProRata(AvailablePrincipal.Total(), CurrentDate, ProRataBonds);
					AvailablePrincipal -= AvailablePrincipal.Total() - TotalPayable;
				break;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				case WatFalPrior::wst_Turbo:
					TotalPayable = RedeemNotes(AvailableInterest*SingleStep->GetRedemptionShare(), SingleStep->GetRedemptionGroup(), CurrentDate);
					AvailableInterest += TotalPayable-(AvailableInterest*SingleStep->GetRedemptionShare());
				break;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				case WatFalPrior::wst_Excess:
					m_AnnualizedExcess.AddFlow(CurrentDate, AvailableInterest, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow));
					m_EquityIncome.AddFlow(CurrentDate, AvailablePrincipal.Total(), static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PrincipalFlow));
					m_EquityIncome.AddFlow(CurrentDate, AvailableInterest, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow));
					if(SingleStep->GetRedemptionGroup()>0){
						ProRataBonds.clear();
						TotalPayable=0.0;
						for(int h=0;h<m_Tranches.size();h++){
							if(m_Tranches.at(h)->GetProrataGroup()==SingleStep->GetRedemptionGroup()){
								ProRataBonds.enqueue(h);
								TotalPayable+=m_Tranches.at(h)->GetOriginalAmount();
							}
						}
						{
							int OriginalProRataBondsSize=ProRataBonds.size();
							while(ProRataBonds.size()>0){
								int ProrataIndex=ProRataBonds.dequeue();
								if(TotalPayable>0){
									m_Tranches[ProrataIndex]->AddCashFlow(CurrentDate, AvailableInterest*m_Tranches.at(ProrataIndex)->GetOriginalAmount() / TotalPayable, TrancheCashFlow::TrancheFlowType::InterestFlow);
									m_Tranches[ProrataIndex]->AddCashFlow(CurrentDate, AvailablePrincipal.Total()*m_Tranches.at(ProrataIndex)->GetOriginalAmount() / TotalPayable, TrancheCashFlow::TrancheFlowType::PrincipalFlow);
								} else {
									m_Tranches[ProrataIndex]->AddCashFlow(CurrentDate, AvailableInterest / OriginalProRataBondsSize, TrancheCashFlow::TrancheFlowType::InterestFlow);
									m_Tranches[ProrataIndex]->AddCashFlow(CurrentDate, AvailablePrincipal.Total() / OriginalProRataBondsSize, TrancheCashFlow::TrancheFlowType::PrincipalFlow);
								}
							}
						}
					}
					else{
						m_ExcessCashFlow.AddFlow(CurrentDate, AvailablePrincipal.Total(), static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PrincipalFlow));
						m_ExcessCashFlow.AddFlow(CurrentDate, AvailableInterest, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow));
					}
					AvailableInterest=0.0;
					AvailablePrincipal.Erase();
				break;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				case WatFalPrior::wst_OCTest:
				case WatFalPrior::wst_OCTestPrinc:
					ProRataBonds.clear();
					TotalPayable=0.0;
					for(int h=0;h<m_Tranches.size();h++){
						if(m_Tranches.at(h)->GetProrataGroup()<=SingleStep->GetGroupTarget()){
							TotalPayable+=m_Tranches.at(h)->GetCurrentOutstanding();
							if(m_Tranches.at(h)->GetProrataGroup()==SingleStep->GetGroupTarget()) ProRataBonds.enqueue(h);
						}
					}
					TotalPayable=qMax(TotalPayable,0.000001);
					//CCC test
					if(m_CCCcurve.GetValue(CurrentDate)>m_CCCTestLimit)
						Solution=(1.0-((m_CCCcurve.GetValue(CurrentDate)-m_CCCTestLimit)*m_CCChaircut))*m_MortgagesPayments.GetAmountOut(i);
					else
						Solution=m_MortgagesPayments.GetAmountOut(i);
					Solution+=AvailablePrincipal.Total();
					if(Solution==0.0) Solution=1.0;
					while(ProRataBonds.size()>0){
						if(m_Tranches.at(ProRataBonds.head())->GetCashFlow().GetOCTest(CurrentDate)<=0.0)
							m_Tranches[ProRataBonds.head()]->AddCashFlow(CurrentDate, Solution / TotalPayable, TrancheCashFlow::TrancheFlowType::OCFlow);
						TestTarget=m_Tranches.at(ProRataBonds.dequeue())->GetMinOClevel();
					}
					//if it fails redeem notes until cured
					if(Solution/TotalPayable<TestTarget){
						//Calculate the amount needed to cure the test
						//If the need is greater than the available funds adjust it down
						if(SingleStep->GetPriorityType()== WatFalPrior::wst_OCTestPrinc){
							TotalPayable=qMin(TotalPayable-(Solution/TestTarget),AvailablePrincipal.Total());
							AvailablePrincipal-=TotalPayable;
						}
						else{
							TotalPayable=qMin(TotalPayable-(Solution/TestTarget),AvailableInterest);
							AvailableInterest-=TotalPayable;
						}
						//If turbo is to be used
						if(SingleStep->GetRedemptionGroup()>0 && SingleStep->GetRedemptionShare()>0.0 && m_UseTurbo){
							TotalPayable=
								(TotalPayable*(1.0-SingleStep->GetRedemptionShare()))
								+RedeemNotes(SingleStep->GetRedemptionShare()*TotalPayable,SingleStep->GetRedemptionGroup(),CurrentDate);
						}
						TotalPayable=RedeemSequential(TotalPayable,CurrentDate);
						if(TotalPayable>0.0){
							if(SingleStep->GetPriorityType()== WatFalPrior::wst_OCTestPrinc)
								AvailablePrincipal+=TotalPayable;
							else
								AvailableInterest+=TotalPayable;
						}
					}
				break;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				case WatFalPrior::wst_ICTest:
				case WatFalPrior::wst_ICTestPrinc:
					adjSeniorExpenses = AdjustCoupon(m_SeniorExpenses.GetValue(CurrentDate),RollingNextIPD,RollingNextIPD.addMonths(m_PaymentFrequency.GetValue(RollingNextIPD)),DayCountConvention::ACT360);
					adjSeniorFees = AdjustCoupon(m_SeniorFees.GetValue(CurrentDate), RollingNextIPD, RollingNextIPD.addMonths(m_PaymentFrequency.GetValue(RollingNextIPD)), DayCountConvention::ACT360);
					ProRataBonds.clear();
					TotalPayable = 0.0;
					Solution = m_InterestAvailable + m_MortgagesPayments.GetAccruedInterest(i) - ((adjSeniorFees + adjSeniorExpenses)*m_MortgagesPayments.GetAmountOut(i)) - m_SeniorExpensesFixed.GetValue(CurrentDate) - m_SeniorFeesFixed.GetValue(CurrentDate);
					for (int h = 0; h < m_Tranches.size(); h++) {
						if (m_Tranches.at(h)->GetProrataGroup() <= SingleStep->GetGroupTarget()) {
							AdjustedCoupon = AdjustCoupon((m_Tranches.at(h)->GetCoupon(CurrentDate, qMin(SingleStep->GetRedemptionGroup() - 1, 0))), RollingNextIPD, RollingNextIPD.addMonths(m_PaymentFrequency.GetValue(RollingNextIPD)), m_Tranches.at(h)->GetDayCount());
							TotalPayable += AdjustedCoupon*(m_Tranches.at(h)->GetCurrentOutstanding() + m_Tranches.at(h)->GetCashFlow().GetFlow(CurrentDate, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::DeferredFlow) | (qMin(SingleStep->GetRedemptionGroup() - 1, 0))));
							if (m_Tranches.at(h)->GetProrataGroup() == SingleStep->GetGroupTarget()) ProRataBonds.enqueue(h);
						}
					}
					TotalPayable = qMax(TotalPayable, 0.000001);
					while (ProRataBonds.size()>0) {
						if (m_Tranches.at(ProRataBonds.head())->GetCashFlow().GetICTest(CurrentDate) <= 0.0)
							m_Tranches[ProRataBonds.head()]->AddCashFlow(CurrentDate, Solution / TotalPayable, TrancheCashFlow::TrancheFlowType::ICFlow);
						TestTarget = m_Tranches.at(ProRataBonds.dequeue())->GetMinIClevel();
					}
					//if it fails redeem senior notes until cured
					if (Solution / TotalPayable < TestTarget && ((AvailablePrincipal.Total()>0.0 && SingleStep->GetPriorityType() == WatFalPrior::wst_ICTestPrinc) || (AvailableInterest > 0 && SingleStep->GetPriorityType() == WatFalPrior::wst_ICTest))) {
						SolutionDegree = 1;
						InterestPayableBefore = (m_InterestAvailable + m_MortgagesPayments.GetAccruedInterest(i) - ((adjSeniorExpenses + adjSeniorFees) * m_MortgagesPayments.GetAmountOut(i)) - m_SeniorExpensesFixed.GetValue(CurrentDate) - m_SeniorFeesFixed.GetValue(CurrentDate)) / TestTarget;
						TotalPayable = 0;
						bool SolutionFound;
						do {
							SolutionFound = true;
							Solution = 0.0;
							for (int h = 0; h<m_Tranches.size(); h++) {
								if (m_Tranches.at(h)->GetProrataGroup() <= SingleStep->GetGroupTarget() && m_Tranches.at(h)->GetProrataGroup() >= SolutionDegree) {
									AdjustedCoupon = AdjustCoupon((m_Tranches.at(h)->GetCoupon(CurrentDate, qMin(SingleStep->GetRedemptionGroup() - 1, 0))), RollingNextIPD, RollingNextIPD.addMonths((m_PaymentFrequency.GetValue(RollingNextIPD))), m_Tranches.at(h)->GetDayCount());
									Solution += AdjustedCoupon*(m_Tranches.at(h)->GetCurrentOutstanding() + m_Tranches.at(h)->GetCashFlow().GetFlow(CurrentDate, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::DeferredFlow) | (qMin(SingleStep->GetRedemptionGroup() - 1, 0))));
								}
							}
							if (GroupWACoupon(SolutionDegree, CurrentDate, qMin(SingleStep->GetRedemptionGroup() - 1,0))>0.0)
								TotalPayable = (Solution - InterestPayableBefore) / GroupWACoupon(SolutionDegree, CurrentDate, qMin(SingleStep->GetRedemptionGroup() - 1,0)); 
							if (TotalPayable > GroupOutstanding(SolutionDegree) || GroupWACoupon(SolutionDegree, CurrentDate, qMin(SingleStep->GetRedemptionGroup() - 1,0)) <= 0) {
								if (++SolutionDegree <= SingleStep->GetGroupTarget()) SolutionFound = false;
								else TotalPayable = 0;
							}
							if (SolutionFound) {
								for (int h = 1; h < SolutionDegree; h++) TotalPayable += GroupOutstanding(h);
								if (SingleStep->GetPriorityType() == WatFalPrior::wst_ICTestPrinc)
									AvailablePrincipal += RedeemSequential(qMin(TotalPayable, AvailablePrincipal.Total()), CurrentDate) - qMin(TotalPayable, AvailablePrincipal.Total());
								else
									AvailableInterest += RedeemSequential(qMin(TotalPayable, AvailableInterest), CurrentDate) - qMin(TotalPayable, AvailableInterest);
							}
						} while (!SolutionFound);
					}
					break;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				case WatFalPrior::wst_ReinvestmentTest:
					if(m_UseTurbo && !IsCallPaymentDate){
						//Just like an OC
						ProRataBonds.clear();
						TotalPayable=0.0;
						for(int h=0;h<m_Tranches.size();h++){
							if(m_Tranches.at(h)->GetProrataGroup()<=SingleStep->GetGroupTarget()){
								TotalPayable+=m_Tranches.at(h)->GetCurrentOutstanding();
								if(m_Tranches.at(h)->GetProrataGroup()==SingleStep->GetGroupTarget()) ProRataBonds.enqueue(h);
							}
						}
						TotalPayable=qMax(TotalPayable,0.000001);
						//CCC test
						if(m_CCCcurve.GetValue(CurrentDate)>m_CCCTestLimit)
							Solution=(1.0-((m_CCCcurve.GetValue(CurrentDate)-m_CCCTestLimit)*m_CCChaircut))*m_MortgagesPayments.GetAmountOut(i);
						else
							Solution=m_MortgagesPayments.GetAmountOut(i);
						Solution += AvailablePrincipal.Total();
						if(Solution==0.0) Solution=1.0;

						if(Solution/TotalPayable<m_ReinvestmentTest.GetTestLevel() && AvailableInterest>0.0){
							//if it fails and you can do something about it
							if(CurrentDate<m_ReinvestmentTest.GetReinvestmentPeriod()){
								//during reinvestment period
								Solution=qMin(TotalPayable-(Solution/m_ReinvestmentTest.GetTestLevel()),AvailableInterest);
								TotalPayable=Solution*m_ReinvestmentTest.GetShare(ReinvestmentTest::InReinvShare);
								Solution*=m_ReinvestmentTest.GetShare(ReinvestmentTest::InRedempShare);
							}
							else{
								//after reinvestment period
								Solution=qMin(TotalPayable-(Solution/m_ReinvestmentTest.GetTestLevel()),AvailableInterest);
								TotalPayable=Solution*m_ReinvestmentTest.GetShare(ReinvestmentTest::OutReinvShare);
								Solution*=m_ReinvestmentTest.GetShare(ReinvestmentTest::OutRedempShare);
							}
							AvailableInterest-=TotalPayable+Solution;
							//reinvest
							if(TotalPayable>0.0){
								m_ReinvestmentTest.CalculateBondCashFlows(TotalPayable,CurrentDate,i);
								m_MortgagesPayments.AddFlow(m_ReinvestmentTest.GetBondCashFlow());
								m_Reinvested.AddFlow(CurrentDate, TotalPayable, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow));
								AvailableInterest+=m_ReinvestmentTest.GetBondCashFlow().GetInterest(CurrentDate);
								AvailablePrincipal.AddScheduled(m_ReinvestmentTest.GetBondCashFlow().GetScheduled(CurrentDate));
								AvailablePrincipal.AddPrepay(m_ReinvestmentTest.GetBondCashFlow().GetPrepay(CurrentDate));
								CheckResults -= m_ReinvestmentTest.GetBondCashFlow().GetScheduled(CurrentDate) + m_ReinvestmentTest.GetBondCashFlow().GetPrepay(CurrentDate) + m_ReinvestmentTest.GetBondCashFlow().GetInterest(CurrentDate);
							}
							//Redeem
							if(SingleStep->GetRedemptionGroup()>0) Solution=RedeemNotes(Solution,SingleStep->GetRedemptionGroup(),CurrentDate);
							else Solution=RedeemSequential(Solution,CurrentDate);
							AvailableInterest+=Solution;
						}
					}
				break;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				case WatFalPrior::wst_ReinvestPrincipal:
					if(CurrentDate<=m_ReinvestmentTest.GetReinvestmentPeriod() && !IsCallPaymentDate){
						if(AvailablePrincipal.Total()>0.0){
							double PayablePrincipal;
							if (SingleStep->GetRedemptionGroup() == 1) {  //Reinvest Prepay Only
								PayablePrincipal = AvailablePrincipal.GetPrepay();
								AvailablePrincipal.SetPrepay(0.0);
							}
							else if (SingleStep->GetRedemptionGroup() == 2) {  //Reinvest scheduled only
								PayablePrincipal = AvailablePrincipal.GetScheduled();
								AvailablePrincipal.SetScheduled(0.0);
							}
							else {
								PayablePrincipal = AvailablePrincipal.Total();
								AvailablePrincipal.Erase();
							}
							m_ReinvestmentTest.CalculateBondCashFlows(PayablePrincipal, CurrentDate, i);
							m_MortgagesPayments.AddFlow(m_ReinvestmentTest.GetBondCashFlow());
							m_Reinvested.AddFlow(CurrentDate, PayablePrincipal, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PrincipalFlow));
							AvailableInterest+=m_ReinvestmentTest.GetBondCashFlow().GetInterest(CurrentDate);
							AvailablePrincipal.AddScheduled(m_ReinvestmentTest.GetBondCashFlow().GetScheduled(CurrentDate));
							AvailablePrincipal.AddPrepay(m_ReinvestmentTest.GetBondCashFlow().GetPrepay(CurrentDate));
							CheckResults -= m_ReinvestmentTest.GetBondCashFlow().GetScheduled(CurrentDate) + m_ReinvestmentTest.GetBondCashFlow().GetPrepay(CurrentDate) + m_ReinvestmentTest.GetBondCashFlow().GetInterest(CurrentDate);
						}
					}	
				break;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				default:
					PrintToTempFile("ReturnFalse.txt", "Reached Default");
					return false;
				}//End Switch
			}//End Cicle through the steps of the waterfall
			m_PrincipalAvailable.Erase();
			m_InterestAvailable=0.0;
			RollingLastIPD=RollingNextIPD;
			RollingNextIPD=RollingNextIPD.addMonths(m_PaymentFrequency.GetValue(RollingNextIPD));
			if(IsCallPaymentDate){
				i++;
				break;
			}
		}//End Cicle in time
		for(int j=0;j<m_Reserves.size();j++){
			m_Reserves[j]->SetReserveFundCurrent(m_Reserves[j]->GetStartingReserve());
			m_Reserves[j]->ResetMissingAnchors();
		}
		m_StartingDeferredJunFees = OriginalStartingDefJunFees;
		m_PrincipalAvailable=OriginalAvailablePrincipal;
		m_InterestAvailable=OriginalAvailableInterest;
		m_CalculatedMtgPayments=m_MortgagesPayments;
		m_MortgagesPayments=OriginalMtgFlows;
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
		for (int i = 0; i < m_Tranches.size(); ++i) {
			m_Tranches[i]->SetStartingDeferredInterest(OriginalStartingDeferred[i]);
		}
		delete [] OriginalStartingDeferred;
		//Check that there is no losses of cash flows
		TrancheCashFlow CheckTranCashFlow;
		MtgCashFlow CheckMtgCashFlow;
		CheckTranCashFlow.Aggregate(GenericCashFlow::TotalAggragate);
		CheckMtgCashFlow.Aggregate(GenericCashFlow::TotalAggragate);
		
		CheckTranCashFlow.Clear(); CheckTranCashFlow.AddFlow(m_Reinvested); CheckResults += CheckTranCashFlow.GetTotalFlow(0);
		CheckTranCashFlow.Clear(); CheckTranCashFlow.AddFlow(m_TotalSeniorExpenses); CheckResults += CheckTranCashFlow.GetTotalFlow(0);
		CheckTranCashFlow.Clear(); CheckTranCashFlow.AddFlow(m_TotalSeniorFees); CheckResults += CheckTranCashFlow.GetTotalFlow(0);
		CheckTranCashFlow.Clear(); CheckTranCashFlow.AddFlow(m_TotalJuniorFees); CheckResults += CheckTranCashFlow.GetTotalFlow(0);
		CheckTranCashFlow.Clear(); CheckTranCashFlow.AddFlow(m_ExcessCashFlow); CheckResults += CheckTranCashFlow.GetTotalFlow(0);
		/*foreach(const ReserveFund* SingleReserve, m_Reserves) {
			CheckTranCashFlow.Clear(); CheckTranCashFlow.AddFlow(SingleReserve->GetReserveFundFlow()); CheckResults += CheckTranCashFlow.GetFlow(0, static_cast<qint32>(ReserveFund::ReserveFlowsType::ReplenishFromInterest)) + CheckTranCashFlow.GetFlow(0, static_cast<qint32>(ReserveFund::ReserveFlowsType::ReplenishFromPrincipal));
		}*/
		foreach(const Tranche* SingleTranche, m_Tranches) {
			CheckTranCashFlow.Clear(); CheckTranCashFlow.AddFlow(SingleTranche->GetCashFlow());	CheckResults += CheckTranCashFlow.GetTotalFlow(0);
		}
		CheckTranCashFlow.Clear(); CheckTranCashFlow.AddFlow(m_GICflows); CheckResults -= CheckTranCashFlow.GetTotalFlow(0);
		
		CheckResults -= m_InterestAvailable;
		CheckResults -= m_PrincipalAvailable.Total();
		if (qAbs(CheckResults) >= 0.01) {
			PrintToTempFile("ReturnFalse.txt", "Cash Flows Dont Match");
			return false;
		}
		return true;
}

QDataStream& operator<<(QDataStream & stream, const Waterfall& flows){
	stream
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
		<< flows.m_UseTurbo
		<< flows.m_PrincipalAvailable.GetScheduled()
		<< flows.m_PrincipalAvailable.GetPrepay()
		<< flows.m_InterestAvailable
		<< flows.m_JuniorFeesCoupon
		<< flows.m_AnnualizedExcess
		<< flows.m_EquityIncome
		<< flows.m_FirstIPDdate
		<< flows.m_Reinvested
		<< flows.m_LastIPDdate
		<< flows.m_CallDate
		<< flows.m_PoolValueAtCall
		<< flows.m_UseCall
		<< flows.m_CallMultiple
		<< flows.m_CallReserve
		<< flows.m_DealName
		<< flows.m_GICinterest
		<< flows.m_GICBaseRate
		<< flows.m_GICBaseRateValue
		<< flows.m_StartingDeferredJunFees
		<< flows.m_GICflows
		<< flows.m_WaterfallStesps.size();
	;
	foreach(const WatFalPrior* SingleStep, flows.m_WaterfallStesps)
		stream << (*SingleStep);
	stream << flows.m_Tranches.size();
	foreach(const Tranche* SingleTranche,flows.m_Tranches)
		stream << (*SingleTranche);	
	stream << flows.m_CumulativeReserves << flows.m_Reserves.size();
	for (QList<ReserveFund*>::const_iterator ResIter = flows.m_Reserves.constBegin(); ResIter != flows.m_Reserves.constEnd(); ResIter++)
		stream << (**ResIter);
	return stream;
}
QDataStream& operator>>(QDataStream & stream, Waterfall& flows){
	return flows.LoadOldVersion(stream);
}
QDataStream& Waterfall::LoadOldVersion(QDataStream& stream){
	double TempDouble;
	QString TempString;
	qint32 TempInt;
	Tranche TempTranche;
	ReserveFund TempReserve;
	WatFalPrior TempStep;
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
	m_ReinvestmentTest.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_ReinvestmentTest;
	stream >> m_CCCTestLimit;
	m_CCCcurve.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_CCCcurve;
	stream >> m_CCChaircut
		>> m_UseTurbo
		>> TempDouble;
	m_PrincipalAvailable.SetScheduled(TempDouble);
	stream >> TempDouble
		>> m_InterestAvailable
		>> m_JuniorFeesCoupon
	;
	m_PrincipalAvailable.SetPrepay(TempDouble);
	m_AnnualizedExcess.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_AnnualizedExcess;
	m_EquityIncome.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_EquityIncome;
	stream >> m_FirstIPDdate;
	m_Reinvested.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_Reinvested;
	stream >> m_LastIPDdate
		>> m_CallDate;
	m_PoolValueAtCall.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_PoolValueAtCall;
	stream >> m_UseCall
		>> m_CallMultiple
		>> m_CallReserve
		>> m_DealName;
	m_GICinterest.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_GICinterest;
	m_GICBaseRate.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_GICBaseRate;
	m_GICBaseRateValue.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_GICBaseRateValue;
	stream >> m_StartingDeferredJunFees;
	m_GICflows.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_GICflows;
	stream >> TempInt;
	;
	ResetSteps();
	for (int i = 0; i < TempInt; i++) {
		TempStep.SetLoadProtocolVersion(m_LoadProtocolVersion);
		stream >> TempStep;
		AddStep(TempStep);
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
QDate Waterfall::GetCalledPeriod() const{
	QDate RollingNextIPD;
	double ActualCallReserveLevel;
	double TotalPayable;
	bool IsCallPaymentDate=false;
	for(int PeriodsCounter=0;!IsCallPaymentDate;PeriodsCounter++){
		RollingNextIPD=m_Tranches.first()->GetCashFlow().GetDate(PeriodsCounter);
		ActualCallReserveLevel=0.0;
		TotalPayable=0.0;
		if(m_CallReserve>0 && m_CallMultiple>0){
			foreach(Tranche* SingleTranche, m_Tranches){
				TotalPayable+=SingleTranche->GetCashFlow().GetAmountOutstanding(RollingNextIPD);
				if(SingleTranche->GetProrataGroup()>=m_CallReserve) ActualCallReserveLevel+=SingleTranche->GetCashFlow().GetAmountOutstanding(RollingNextIPD);
			}
			if(ActualCallReserveLevel==0.0)ActualCallReserveLevel=m_CallReserve;
			else {
				for (QList<ReserveFund*>::const_iterator ResIter = m_Reserves.constBegin(); ResIter != m_Reserves.constEnd(); ResIter++)
					ActualCallReserveLevel += (*ResIter)->GetReserveFundCurrent(RollingNextIPD);
			}
			ActualCallReserveLevel*=m_CallMultiple;
			IsCallPaymentDate=ActualCallReserveLevel>=TotalPayable;
		}
		IsCallPaymentDate= IsCallPaymentDate || (!m_CallDate.isNull() && RollingNextIPD>=m_CallDate) || RollingNextIPD>=m_MortgagesPayments.GetDate(m_MortgagesPayments.Count()-1);
	}
	return RollingNextIPD;
}
QString Waterfall::ReadyToCalculate()const{
	QString Result;
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
	if (BloombergVector(m_ReinvestmentTest.GetCDRAssumption()).IsEmpty(0.0,1.0))Result += "CDR\n";
	if (BloombergVector(m_ReinvestmentTest.GetCPRAssumption()).IsEmpty(0.0, 1.0))Result += "CPR\n";
	if (BloombergVector(m_ReinvestmentTest.GetLSAssumption()).IsEmpty(0.0, 1.0))Result += "LS\n";
	if (BloombergVector(m_ReinvestmentTest.GetWALAssumption()).IsEmpty(0.0))Result += "Reinvestment Bond WAL\n";
	if (BloombergVector(m_ReinvestmentTest.GetReinvDelay()).IsEmpty(0.0))Result += "Reinvestment Delay\n";
	if (BloombergVector(m_ReinvestmentTest.GetReinvPrice()).IsEmpty(0.0))Result += "Reinvestment Price\n";
	if (m_ReinvestmentTest.GetReinvestmentPeriod() < QDate(2000, 1, 1))Result += "Reinvestment Period\n";
	if (m_ReinvestmentTest.GetTestLevel() < 0.0)Result += "Reinvestment Test Limit\n";
	if (m_ReinvestmentTest.GetReinvestmentBond().GetPaymentFreq().IsEmpty(1)) Result += "Reinvestment Bond Payment Frequency\n";
	if (m_ReinvestmentTest.GetReinvestmentBond().GetInterest().isEmpty())Result += "Reinvestment Bond Spread\n";
	if (m_CCCcurve.IsEmpty(0.0,1.0))Result += "CCC Curve\n";
	if (m_CCChaircut < 0.0 || m_CCChaircut>1.0)Result += "CCC Value\n";
	if (m_PoolValueAtCall.IsEmpty(0.0) && !m_PoolValueAtCall.IsEmpty()) Result += "Pool Value at Call\n";
	if (m_UseCall && m_CallDate < QDate(2000, 1, 1) && (m_CallReserve <= 0.0 || m_CallMultiple <= 0.0))Result += "Specify a call Criteria to use the Call\n";
	if (m_MortgagesPayments.Count() > 0 && m_MortgagesPayments.GetDate(0) < QDate(2000, 1, 1))Result += "Pool Cut Off Date\n";
	if (m_CCCTestLimit < 0.0 || m_CCChaircut>1.0)Result += "CCC Test Limit\n";
	if (
		m_ReinvestmentTest.GetShare(ReinvestmentTest::InRedempShare) < 0.0
		|| m_ReinvestmentTest.GetShare(ReinvestmentTest::OutRedempShare) < 0.0
		|| m_ReinvestmentTest.GetShare(ReinvestmentTest::InReinvShare) < 0.0
		|| m_ReinvestmentTest.GetShare(ReinvestmentTest::OutReinvShare) < 0.0
		|| m_ReinvestmentTest.GetShare(ReinvestmentTest::InRedempShare) + m_ReinvestmentTest.GetShare(ReinvestmentTest::InReinvShare) > 1.0
		|| m_ReinvestmentTest.GetShare(ReinvestmentTest::OutRedempShare) + m_ReinvestmentTest.GetShare(ReinvestmentTest::OutReinvShare) > 1.0
		)Result += QString("Reinvestment Test Shares InRedempShare: %1 | OutRedempShare: %2 | InReinvShare: %3 | OutReinvShare: %4 | %5 | %6\n")
		.arg(m_ReinvestmentTest.GetShare(ReinvestmentTest::InRedempShare))
		.arg(m_ReinvestmentTest.GetShare(ReinvestmentTest::OutRedempShare))
		.arg(m_ReinvestmentTest.GetShare(ReinvestmentTest::InReinvShare))
		.arg(m_ReinvestmentTest.GetShare(ReinvestmentTest::OutReinvShare))
		.arg(m_ReinvestmentTest.GetShare(ReinvestmentTest::InRedempShare) + m_ReinvestmentTest.GetShare(ReinvestmentTest::InReinvShare))
		.arg(m_ReinvestmentTest.GetShare(ReinvestmentTest::OutRedempShare) + m_ReinvestmentTest.GetShare(ReinvestmentTest::OutReinvShare))
		;
	if (m_FirstIPDdate < QDate(2000, 1, 1))Result += "Next IDP\n";
	if (m_LastIPDdate < QDate(2000, 1, 1))Result += "Last IDP\n";
	for (int ResIter = 0; ResIter < m_Reserves.size(); ResIter++) {
		if (m_Reserves.at(ResIter)->GetReserveFundTarget().IsEmpty(0.0)) Result += QString("Reserve %1 Target Level\n").arg(ResIter + 1);
		if (m_Reserves.at(ResIter)->GetReserveFundMultiple().IsEmpty(0.0)) Result += QString("Reserve %1 Multiple\n").arg(ResIter + 1);
		if (m_Reserves.at(ResIter)->GetReserveFundFloor().IsEmpty(0.0))Result += QString("Reserve %1 Floor Level\n").arg(ResIter + 1);
		if (m_Reserves.at(ResIter)->GetReserveFundFreed() < 0) Result += QString("Reserve %1 Freed After Redemption\n").arg(ResIter + 1);
		if (m_Reserves.at(ResIter)->GetReserveFundCurrent() < 0.0) Result += QString("Reserve %1 Current Amount\n").arg(ResIter + 1);
	}
	if (!Result.isEmpty()) return Result.left(Result.size() - 1);
	return Result;
}
double Waterfall::GetEquityReturn(int index)const{
	IntegerVector AdjPaymentFreq(m_PaymentFrequency);
	if(m_PaymentFrequency.GetAnchorDate().isNull()) AdjPaymentFreq.SetAnchorDate(m_MortgagesPayments.GetDate(0));
	if(index<0 || index>=m_EquityIncome.Count()) return 0.0;
	int EquityTranche;
	foreach(WatFalPrior* SingleStep,m_WaterfallStesps){
		if(SingleStep->GetPriorityType()==WatFalPrior::wst_Excess){
			EquityTranche=SingleStep->GetRedemptionGroup();
			break;
		}
	}
	double denominator=0.0;
	if(EquityTranche>0){
		foreach(Tranche* SingleTranche, m_Tranches){
			if(SingleTranche->GetProrataGroup()==EquityTranche) denominator+=SingleTranche->GetOriginalAmount();
		}
		if (denominator>0) return qPow(1.0 + ((m_EquityIncome.GetFlow(index, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow)) + m_EquityIncome.GetFlow(index, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PrincipalFlow))) / denominator), 12.0 / ((AdjPaymentFreq.GetValue(m_EquityIncome.GetDate(index))))) - 1.0;
		else return 0.0;
	}
	denominator=0.0;
	foreach(Tranche* SingleTranche, m_Tranches){
		denominator+=SingleTranche->GetOriginalAmount();
	}
	denominator=m_CalculatedMtgPayments.GetAmountOut(0)-denominator;
	if (denominator>0) return qPow(1.0 + ((m_EquityIncome.GetFlow(index, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow)) + m_EquityIncome.GetFlow(index, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PrincipalFlow))) / denominator), 12.0 / ((AdjPaymentFreq.GetValue(m_EquityIncome.GetDate(index))))) - 1.0;
	else return 0.0;

}
double Waterfall::GetCumulativeEquityReturn(int index)const{
	if(index<0 || index>=m_EquityIncome.Count()) return 0.0;
	double numerator=0.0;
	for (int i = 0; i <= index; i++) numerator += (m_EquityIncome.GetFlow(i, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow)) + m_EquityIncome.GetFlow(i, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PrincipalFlow)));
	int EquityTranche;
	foreach(WatFalPrior* SingleStep,m_WaterfallStesps){
		if(SingleStep->GetPriorityType()==WatFalPrior::wst_Excess){
			EquityTranche=SingleStep->GetRedemptionGroup();
			break;
		}
	}
	double denominator=0.0;
	if(EquityTranche>0){
		foreach(Tranche* SingleTranche, m_Tranches){
			if(SingleTranche->GetProrataGroup()==EquityTranche) denominator+=SingleTranche->GetOriginalAmount();
		}
		if(denominator>0) return numerator/denominator;
		else return 0.0;
	}
	denominator=0.0;
	foreach(Tranche* SingleTranche, m_Tranches){
		denominator+=SingleTranche->GetOriginalAmount();
	}
	denominator=m_CalculatedMtgPayments.GetAmountOut(0)-denominator;
	if(denominator>0) return numerator/denominator;
	else return 0.0;
}
double Waterfall::GetCallEquityRatio(int index)const{
	if(index<0 || index>=m_Tranches.first()->GetCashFlow().Count()) return 0.0;
	QDate TargetDate=m_Tranches.first()->GetCashFlow().GetDate(index);
	int MtgIndex=-1;
	for(int i=0;i<m_CalculatedMtgPayments.Count();i++){
		if(TargetDate.year()==m_CalculatedMtgPayments.GetDate(i).year() &&  TargetDate.month()==m_CalculatedMtgPayments.GetDate(i).month()){
			MtgIndex=i;
			break;
		}
	}
	if(MtgIndex<0) return 0.0;
	int EquityTranche;
	foreach(WatFalPrior* SingleStep,m_WaterfallStesps){
		if(SingleStep->GetPriorityType()==WatFalPrior::wst_Excess){
			EquityTranche=SingleStep->GetRedemptionGroup();
			break;
		}
	}
	double denominator=0.0;
	double numerator;
	if (m_PoolValueAtCall.IsEmpty())
		numerator = m_CalculatedMtgPayments.GetWAPrice(MtgIndex);
	else
		numerator = m_PoolValueAtCall.GetValue(m_CalculatedMtgPayments.GetDate(MtgIndex));
	numerator*=m_CalculatedMtgPayments.GetAmountOut(MtgIndex);
	if(EquityTranche>0){
		foreach(Tranche* SingleTranche, m_Tranches){
			if(SingleTranche->GetProrataGroup()==EquityTranche) denominator+=SingleTranche->GetCashFlow().GetAmountOutstanding(index);
			else numerator-=SingleTranche->GetCashFlow().GetAmountOutstanding(index);
		}
		if(denominator>0) return numerator/denominator;
		else return 0.0;
	}
	denominator=0.0;
	if (m_PoolValueAtCall.IsEmpty())
		numerator = m_CalculatedMtgPayments.GetWAPrice(MtgIndex);
	else
		numerator = m_PoolValueAtCall.GetValue(m_CalculatedMtgPayments.GetDate(MtgIndex));
	numerator *= m_CalculatedMtgPayments.GetAmountOut(MtgIndex);
	foreach(Tranche* SingleTranche, m_Tranches){
		denominator+=SingleTranche->GetCashFlow().GetAmountOutstanding(index);
		numerator-=SingleTranche->GetCashFlow().GetAmountOutstanding(index);
	}
	denominator=m_CalculatedMtgPayments.GetAmountOut(index)-denominator;
	if(denominator>0) return numerator/denominator;
	else return 0.0;
}
void Waterfall::SetReserveFund(int RFindex, const QString& RFtarget, const QString& RFmultiple, const QString& RFfloor, const QString& RFcap, double RFcurrent, int RFfreed, bool RFtoInterest) {
	if (RFindex<0 || RFindex >= m_Reserves.size()) return;
	m_Reserves[RFindex]->SetReserveFund(RFtarget, RFmultiple, RFfloor, RFcap, RFcurrent, RFfreed, RFtoInterest);
}
void Waterfall::SetReserveFund(int RFindex, const ReserveFund& source) {
	if (RFindex < 0 || RFindex >= m_Reserves.size()) return;
	m_Reserves[RFindex]->operator=(source);
}
void Waterfall::AddReserveFund(const QString& RFtarget, const QString& RFmultiple, const QString& RFfloor, const QString& RFcap, double RFcurrent, int RFfreed, bool RFtoInterest) {
	m_Reserves.append(new ReserveFund());
	m_Reserves.last()->SetReserveFund(RFtarget, RFmultiple, RFfloor, RFcap, RFcurrent,  RFfreed, RFtoInterest);
}
void Waterfall::AddReserveFund(const ReserveFund& source) {
	m_Reserves.append(new ReserveFund(source));
}
void Waterfall::ResetReserve(int RFindex){
	if(RFindex==-1){
		for (int i = 0; i<m_Reserves.size(); i++)
			m_Reserves[i]->ResetReserve();
		return;
	}
	if (RFindex<0 || RFindex >= m_Reserves.size()) return;
	m_Reserves[RFindex]->ResetReserve();
}
void Waterfall::RemoveReserve(int RFindex) {
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
void Waterfall::SetPaymentFrequency(const QString& a){
	IntegerVector TempVect(a);
	for (int i = 0; i < TempVect.NumElements(); i++) {
		if (TempVect.GetValue(i) <= 0) return;
	}
	m_PaymentFrequency = TempVect;
}
void Waterfall::CompileReferenceRateValue(ForwardBaseRateTable& Values) {
	m_GICBaseRateValue = m_GICBaseRate.CompileReferenceRateValue(Values);
	for (auto i = m_Tranches.begin(); i != m_Tranches.end(); ++i)
		(*i)->CompileReferenceRateValue(Values);
}
void Waterfall::CompileReferenceRateValue(ConstantBaseRateTable& Values) {
	m_GICBaseRateValue = m_GICBaseRate.CompileReferenceRateValue(Values);
	for (auto i = m_Tranches.begin(); i != m_Tranches.end(); ++i)
		(*i)->CompileReferenceRateValue(Values);
}
#ifndef NO_DATABASE
void Waterfall::GetBaseRatesDatabase(ConstantBaseRateTable& Values, bool DownloadAll) {
	m_GICBaseRateValue = m_GICBaseRate.GetBaseRatesDatabase(Values, DownloadAll);
	for (auto i = m_Tranches.begin(); i != m_Tranches.end(); ++i)
		(*i)->GetBaseRatesDatabase(Values, DownloadAll);
}
void Waterfall::GetBaseRatesDatabase(ForwardBaseRateTable& Values, bool DownloadAll) {
	m_GICBaseRateValue = m_GICBaseRate.GetBaseRatesDatabase(Values, DownloadAll);
	for (auto i = m_Tranches.begin(); i != m_Tranches.end(); ++i)
		(*i)->GetBaseRatesDatabase(Values, DownloadAll);
}
#endif