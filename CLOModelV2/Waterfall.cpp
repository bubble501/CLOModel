#include <QtAlgorithms>
#include <QQueue>
#include <QHash>
#include <qmath.h>
#include "Waterfall.h"
#include "QBbgWorker.h"
//#include "QSingleBbgRequest.h"
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
		return qPow((1.0+(m_AnnualizedExcess.GetInterest(index)/
			m_CalculatedMtgPayments.GetAmountOut(m_AnnualizedExcess.GetDate(index)))),12.0/(AdjPaymentFreq.GetValue(m_AnnualizedExcess.GetDate(index))))-1.0;
	}
	double RunningSum=0.0;
	for (int i=0;i<m_Tranches.size();i++){
		RunningSum+=m_Tranches.at(i)->GetCashFlow().GetAmountOutstanding(index);
	}
	if(RunningSum<0.01) return -1.0;
	return qPow(1.0+(m_AnnualizedExcess.GetInterest(index)/RunningSum),12.0/(AdjPaymentFreq.GetValue(m_AnnualizedExcess.GetDate(index))))-1.0;
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
	,m_PrincipalAvailable(0.0)
	,m_InterestAvailable(0.0)
	,m_JuniorFeesCoupon(0.0)
	,m_PoolValueAtCall(100.0)
	,m_UseCall(false)
	,m_CallMultiple(0.0)
	,m_CallReserve(0.0)
	,m_CumulativeReserves(false)
{

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
					m_Tranches[j]->AddCashFlow(m_Tranches.at(i)->GetCashFlow().GetDate(h),0.0,TrancheCashFlow::InterestFlow);
				}
			}
		}
	}
	//dates from excess to all the tranches
	if(!ExcessIsEmpty){
		for(int j=0;j<m_Tranches.size();j++){
			for(int h=0;h<m_ExcessCashFlow.Count();h++){
				if(m_Tranches.at(j)->GetCashFlow().FindDate(m_ExcessCashFlow.GetDate(h)) < 0){
					m_Tranches[j]->AddCashFlow(m_ExcessCashFlow.GetDate(h),0.0,TrancheCashFlow::InterestFlow);
				}
			}
		}
	}
	//dates from annualized excess to all the tranches (equity income dates included here)
	for(int j=0;j<m_Tranches.size();j++){
		for(int h=0;h<m_AnnualizedExcess.Count();h++){
			if(m_Tranches.at(j)->GetCashFlow().FindDate(m_AnnualizedExcess.GetDate(h)) < 0){
				m_Tranches[j]->AddCashFlow(m_AnnualizedExcess.GetDate(h),0.0,TrancheCashFlow::InterestFlow);
			}
		}
	}
	//dates from reserve funds to all the tranches
	for (QList<ReserveFund*>::const_iterator i = m_Reserves.constBegin(); i != m_Reserves.constEnd(); i++) {
		if ((*i)->GetReserveFundFlow().Count() == 0) continue;
		for (int j = 0; j < m_Tranches.size(); j++) {
			for (int h = 0; h < (*i)->GetReserveFundFlow().Count(); h++) {
				if (m_Tranches.at(j)->GetCashFlow().FindDate((*i)->GetReserveFundFlow().GetDate(h)) < 0) {
					m_Tranches[j]->AddCashFlow((*i)->GetReserveFundFlow().GetDate(h), 0.0, TrancheCashFlow::InterestFlow);
				}
			}
		}
	}
	//dates from senior fees to all the tranches
	for(int j=0;j<m_Tranches.size();j++){
		for(int h=0;h<m_TotalSeniorFees.Count();h++){
			if(m_Tranches.at(j)->GetCashFlow().FindDate(m_TotalSeniorFees.GetDate(h)) < 0){
				m_Tranches[j]->AddCashFlow(m_TotalSeniorFees.GetDate(h),0.0,TrancheCashFlow::InterestFlow);
			}
		}
	}
	//dates from junior fees to all the tranches
	for(int j=0;j<m_Tranches.size();j++){
		for(int h=0;h<m_TotalJuniorFees.Count();h++){
			if(m_Tranches.at(j)->GetCashFlow().FindDate(m_TotalJuniorFees.GetDate(h)) < 0){
				m_Tranches[j]->AddCashFlow(m_TotalJuniorFees.GetDate(h),0.0,TrancheCashFlow::InterestFlow);
			}
		}
	}
	//dates from senior expenses to all the tranches
	for(int j=0;j<m_Tranches.size();j++){
		for(int h=0;h<m_TotalSeniorExpenses.Count();h++){
			if(m_Tranches.at(j)->GetCashFlow().FindDate(m_TotalSeniorExpenses.GetDate(h)) < 0){
				m_Tranches[j]->AddCashFlow(m_TotalSeniorExpenses.GetDate(h),0.0,TrancheCashFlow::InterestFlow);
			}
		}
	}
	//dates from tranches to excess, fees and reserve
	for(int i=0;i<m_Tranches.size();i++){
		for(int h=0;h<m_Tranches.at(i)->GetCashFlow().Count();h++){
			if(!ExcessIsEmpty){
				if (m_ExcessCashFlow.FindDate(m_Tranches.at(i)->GetCashFlow().GetDate(h)) < 0)
					m_ExcessCashFlow.AddFlow(m_Tranches.at(i)->GetCashFlow().GetDate(h),0.0,TrancheCashFlow::InterestFlow);
			}
			if (m_AnnualizedExcess.FindDate(m_Tranches.at(i)->GetCashFlow().GetDate(h)) < 0)
				m_AnnualizedExcess.AddFlow(m_Tranches.at(i)->GetCashFlow().GetDate(h),0.0,TrancheCashFlow::InterestFlow);
			if (m_TotalSeniorExpenses.FindDate(m_Tranches.at(i)->GetCashFlow().GetDate(h)) < 0)
				m_TotalSeniorExpenses.AddFlow(m_Tranches.at(i)->GetCashFlow().GetDate(h),0.0,TrancheCashFlow::InterestFlow);
			if (m_TotalSeniorFees.FindDate(m_Tranches.at(i)->GetCashFlow().GetDate(h)) < 0)
				m_TotalSeniorFees.AddFlow(m_Tranches.at(i)->GetCashFlow().GetDate(h),0.0,TrancheCashFlow::InterestFlow);
			if (m_TotalJuniorFees.FindDate(m_Tranches.at(i)->GetCashFlow().GetDate(h)) < 0)
				m_TotalJuniorFees.AddFlow(m_Tranches.at(i)->GetCashFlow().GetDate(h),0.0,TrancheCashFlow::InterestFlow);
			for (QList<ReserveFund*>::iterator ResIter = m_Reserves.begin(); ResIter != m_Reserves.end(); ResIter++) {
				if ((*ResIter)->GetReserveFundFlow().Count() == 0) continue;
				if ((*ResIter)->GetReserveFundFlow().FindDate(m_Tranches.at(i)->GetCashFlow().GetDate(h)) < 0)
					(*ResIter)->GetReserveFundFlow().AddFlow(m_Tranches.at(i)->GetCashFlow().GetDate(h), 0.0, TrancheCashFlow::InterestFlow);
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
				ReserveSum += (*ResIter)->GetReserveFundFlow().GetTotalFlow((*ResIter)->GetReserveFundFlow().GetDate(TimeIndex));
		}
		if(ReserveSum<=0.0) return 0.0;
		return 1.0-(Runningsum/ReserveSum); //(m_CalculatedMtgPayments.GetAmountOut(m_Tranches.first()->GetCashFlow().GetDate(TimeIndex))/Runningsum)-1.0;
	}else{
		ReserveSum=m_CalculatedMtgPayments.GetAmountOut(0);
		ReserveSum += m_PrincipalAvailable;
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
double Waterfall::GroupWACoupon(int GroupTarget)const{
	double RunningSum=0.0;
	double Result=0.0;
	for(QList<Tranche*>::const_iterator i=m_Tranches.begin();i!=m_Tranches.end();i++){
		if((*i)->GetProrataGroup()==GroupTarget){
			Result+=(*i)->GetCoupon()*(*i)->GetCashFlow().GetAmountOutstanding(0);
			RunningSum+=(*i)->GetCashFlow().GetAmountOutstanding(0);
		}
	}
	if(RunningSum>0) return Result / RunningSum;
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
double Waterfall::RedeemNotes(double AvailableFunds,int GroupTarget, int PeriodIndex){
	if(AvailableFunds<0.01) return 0.0;
	QDate TargetDate(m_MortgagesPayments.GetDate(PeriodIndex));
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
				TrancheCashFlow::PrincipalFlow
			);
		}
		return AvailableFunds-TotalPayable;
	}
	while(ProRataBonds.size()>0){
		int ProRataIndex=ProRataBonds.dequeue();
		m_Tranches[ProRataIndex]->AddCashFlow(
			TargetDate,
			AvailableFunds * (m_Tranches.at(ProRataIndex)->GetCurrentOutstanding() /TotalPayable),
			TrancheCashFlow::PrincipalFlow
		);
	}
	return 0.0;
}
double Waterfall::RedeemSequential(double AvailableFunds, int PeriodIndex,int MaxGroup){
	if(AvailableFunds<0.01) return 0.0;
	if(MaxGroup<=0) MaxGroup=FindMostJuniorLevel();
	for( int CurrentSeniority=1;CurrentSeniority<=MaxGroup && AvailableFunds>=0.01;CurrentSeniority++){
		AvailableFunds=RedeemNotes(AvailableFunds,CurrentSeniority,PeriodIndex);
	}
	return AvailableFunds;
}
double Waterfall::RedeemProRata(double AvailableFunds, int PeriodIndex,QList<int> Groups){
	int MostJunior=FindMostJuniorLevel();
	QDate TargetDate(m_MortgagesPayments.GetDate(PeriodIndex));
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
			TotalPayable+=m_Tranches.at(i)->GetCurrentOutstanding();
			GroupSum[m_Tranches.at(i)->GetProrataGroup()]+=m_Tranches.at(i)->GetCurrentOutstanding();
		}
	}
	double RemainingFunds=0.0;
	foreach(int CurrentSeniority, Groups){
		RemainingFunds+=RedeemNotes(
			AvailableFunds*GroupSum.value(CurrentSeniority)/TotalPayable
			,CurrentSeniority,PeriodIndex);
	}
	if(RemainingFunds>0) return RedeemProRata(AvailableFunds,PeriodIndex,Groups);
	else return 0.0;
}
double Waterfall::GetWACostOfCapital(int index)const{
	double RunningSum=0.0;
	double Result=0.0;
	if(m_Tranches.isEmpty()) return 0.0;
	if(index<0 || index>=m_Tranches.first()->GetCashFlow().Count()) return 0.0;
	for (QList<Tranche*>::const_iterator i=m_Tranches.begin();i!=m_Tranches.end();i++){
		if((*i)->GetCoupon((*i)->GetCashFlow().GetDate(index))>0){
			Result+=((*i)->GetCoupon((*i)->GetCashFlow().GetDate(index))) * ((*i)->GetCashFlow().GetAmountOutstanding(index));
			RunningSum+=(*i)->GetCashFlow().GetAmountOutstanding(index);
		}
	}
	if(RunningSum<=0.0)return 0.0;
	return Result/RunningSum;
}

bool Waterfall::CalculateTranchesCashFlows(){
	if(!ReadyToCalculate().isEmpty())return false; 
		double AvailablePrincipal;
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
		double OriginalAvailablePrincipal=m_PrincipalAvailable;
		double cumulativePrepay=0.0;
		double* OriginalReserveLevel;
		bool IsCallPaymentDate=false;
		double ActualCallReserveLevel=0.0;
		QDate CurrentDate;
		int PeriodsCounter=0;
		MtgCashFlow OriginalMtgFlows=m_MortgagesPayments;
		m_ExcessCashFlow.ResetFlows();
		m_TotalSeniorExpenses.ResetFlows();
		m_TotalSeniorFees.ResetFlows();
		m_TotalJuniorFees.ResetFlows();
		m_AnnualizedExcess.ResetFlows();
		m_EquityIncome.ResetFlows();
		m_Reinvested.ResetFlows();
		bool NullCCCanchor[]={
			m_CCCcurve.GetAnchorDate().isNull()
			,m_PaymentFrequency.GetAnchorDate().isNull()
			, m_SeniorExpenses.GetAnchorDate().isNull()
			, m_SeniorFees.GetAnchorDate().isNull()
			, m_JuniorFees.GetAnchorDate().isNull()
			, m_SeniorExpensesFixed.GetAnchorDate().isNull()
			, m_SeniorFeesFixed.GetAnchorDate().isNull()
			, m_JuniorFeesFixed.GetAnchorDate().isNull()
		};
		if(NullCCCanchor[0]) m_CCCcurve.SetAnchorDate(m_MortgagesPayments.GetDate(0));
		if(NullCCCanchor[1]) m_PaymentFrequency.SetAnchorDate(m_MortgagesPayments.GetDate(0));
		if (NullCCCanchor[2]) m_SeniorExpenses.SetAnchorDate(m_MortgagesPayments.GetDate(0));
		if (NullCCCanchor[3]) m_SeniorFees.SetAnchorDate(m_MortgagesPayments.GetDate(0));
		if (NullCCCanchor[4]) m_JuniorFees.SetAnchorDate(m_MortgagesPayments.GetDate(0));
		if (NullCCCanchor[5]) m_SeniorExpensesFixed.SetAnchorDate(m_MortgagesPayments.GetDate(0));
		if (NullCCCanchor[6]) m_SeniorFeesFixed.SetAnchorDate(m_MortgagesPayments.GetDate(0));
		if (NullCCCanchor[7]) m_JuniorFeesFixed.SetAnchorDate(m_MortgagesPayments.GetDate(0));
		foreach(Tranche* SingleTranche, m_Tranches) {
			SingleTranche->GetCashFlow().ResetFlows();
			/*if (SingleTranche->GetStartingDeferredInterest() > 0)
				SingleTranche->AddCashFlow(m_LastIPDdate, SingleTranche->GetStartingDeferredInterest(), TrancheCashFlow::DeferredFlow);*/
		}
		for(int i=0;i<m_MortgagesPayments.Count();i++){
			CurrentDate=m_MortgagesPayments.GetDate(i);
			m_PrincipalAvailable+=m_MortgagesPayments.GetPrincipal(i);
			m_InterestAvailable+=m_MortgagesPayments.GetInterest(i);
			cumulativePrepay += m_MortgagesPayments.GetPrepay(i);
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
					IsCallPaymentDate=ActualCallReserveLevel>=TotalPayable-m_PrincipalAvailable;
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
					if(m_PrincipalAvailable>0.0){
						double PayablePrincipal = m_PrincipalAvailable; //Reinvest all principal
						if (ReinvestRightAway == 1)  //Reinvest Prepay Only
						{
							PayablePrincipal = qMin(m_PrincipalAvailable, m_MortgagesPayments.GetPrepay(i));
						}
						else if (ReinvestRightAway == 2) { //Reinvest scheduled only
							PayablePrincipal = qMax(0.0, PayablePrincipal - cumulativePrepay);
						}
						m_PrincipalAvailable -= PayablePrincipal;
						m_ReinvestmentTest.CalculateBondCashFlows(PayablePrincipal, CurrentDate, i);
						m_MortgagesPayments.AddFlow(m_ReinvestmentTest.GetBondCashFlow());
						m_Reinvested.AddFlow(CurrentDate, PayablePrincipal, TrancheCashFlow::PrincipalFlow);
						m_InterestAvailable+=m_ReinvestmentTest.GetBondCashFlow().GetInterest(CurrentDate);
						m_PrincipalAvailable+=m_ReinvestmentTest.GetBondCashFlow().GetPrincipal(CurrentDate);
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
			if(IsCallPaymentDate) AvailablePrincipal+=m_PoolValueAtCall*m_MortgagesPayments.GetAmountOut(i)/100.0;
			foreach(WatFalPrior* SingleStep,m_WaterfallStesps){//Cycle through the steps of the waterfall
				switch(SingleStep->GetPriorityType()){
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				case WatFalPrior::wst_SeniorExpenses:
					adjSeniorExpenses = m_SeniorExpenses.GetValue(CurrentDate)*static_cast<double>(RollingLastIPD.daysTo(RollingNextIPD)) / 360.0;
					TotalPayable=adjSeniorExpenses*m_MortgagesPayments.GetAmountOut(i)
						- m_TotalSeniorExpenses.GetInterest(CurrentDate)
						- m_TotalSeniorExpenses.GetPrincipal(CurrentDate)
					;
					TotalPayable += m_SeniorExpensesFixed.GetValue(CurrentDate);
					if(TotalPayable>=0.01){
						if(SingleStep->GetRedemptionGroup()==1){
							m_TotalSeniorExpenses.AddFlow(CurrentDate,qMin(AvailableInterest,TotalPayable),TrancheCashFlow::InterestFlow);
							AvailableInterest=qMax(0.0,AvailableInterest-TotalPayable);
						}
						else if(SingleStep->GetRedemptionGroup()==2){
							m_TotalSeniorExpenses.AddFlow(CurrentDate,qMin(AvailablePrincipal,TotalPayable),TrancheCashFlow::PrincipalFlow);
							AvailablePrincipal=qMax(0.0,AvailablePrincipal-TotalPayable);
						}
					}
				break;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				case WatFalPrior::wst_SeniorFees:
					adjSeniorFees = m_SeniorFees.GetValue(CurrentDate)*static_cast<double>(RollingLastIPD.daysTo(RollingNextIPD)) / 360.0;
					TotalPayable=adjSeniorFees*m_MortgagesPayments.GetAmountOut(i)
						- m_TotalSeniorFees.GetInterest(CurrentDate)
						- m_TotalSeniorFees.GetPrincipal(CurrentDate)
						;
					TotalPayable += m_SeniorFeesFixed.GetValue(CurrentDate);
					if(TotalPayable>=0.01){
						if(SingleStep->GetRedemptionGroup()==1){
							m_TotalSeniorFees.AddFlow(CurrentDate,qMin(AvailableInterest,TotalPayable),TrancheCashFlow::InterestFlow);
							AvailableInterest=qMax(0.0,AvailableInterest-TotalPayable);
						}
						else if(SingleStep->GetRedemptionGroup()==2){
							m_TotalSeniorFees.AddFlow(CurrentDate,qMin(AvailablePrincipal,TotalPayable),TrancheCashFlow::PrincipalFlow);
							AvailablePrincipal=qMax(0.0,AvailablePrincipal-TotalPayable);
						}
					}
				break;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				case WatFalPrior::wst_juniorFees:
					adjJuniorFees = m_JuniorFees.GetValue(CurrentDate)*static_cast<double>(RollingLastIPD.daysTo(RollingNextIPD)) / 360.0;
					TotalPayable= adjJuniorFees*m_MortgagesPayments.GetAmountOut(i)
						-m_TotalJuniorFees.GetInterest(CurrentDate)
						-m_TotalJuniorFees.GetPrincipal(CurrentDate)
						+((1.0+(m_JuniorFeesCoupon*static_cast<double>(RollingLastIPD.daysTo(RollingNextIPD))/360.0))*m_TotalJuniorFees.GetPreviousDeferred(CurrentDate))
					;
					TotalPayable += m_JuniorFeesFixed.GetValue(CurrentDate);
					TotalPayable=qMax(TotalPayable,0.0);
					if(SingleStep->GetRedemptionGroup()==1){
						m_TotalJuniorFees.AddFlow(CurrentDate,qMin(AvailableInterest,TotalPayable),TrancheCashFlow::InterestFlow);
						m_AnnualizedExcess.AddFlow(CurrentDate,qMin(AvailableInterest,TotalPayable),TrancheCashFlow::InterestFlow);
						if(AvailableInterest<TotalPayable){
							m_TotalJuniorFees.AddFlow(CurrentDate,TotalPayable-AvailableInterest-m_TotalJuniorFees.GetDeferred(CurrentDate),TrancheCashFlow::DeferredFlow);
						}
						AvailableInterest=qMax(0.0,AvailableInterest-TotalPayable);
					}
					else if(SingleStep->GetRedemptionGroup()==2){
						m_TotalJuniorFees.AddFlow(CurrentDate,qMin(AvailablePrincipal,TotalPayable),TrancheCashFlow::PrincipalFlow);
						m_AnnualizedExcess.AddFlow(CurrentDate,qMin(AvailablePrincipal,TotalPayable),TrancheCashFlow::PrincipalFlow);
						if(AvailablePrincipal<TotalPayable){
							m_TotalJuniorFees.AddFlow(CurrentDate,TotalPayable-AvailablePrincipal-m_TotalJuniorFees.GetDeferred(CurrentDate),TrancheCashFlow::DeferredFlow);
						}
						AvailablePrincipal=qMax(0.0,AvailablePrincipal-TotalPayable);
					}
				break;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				case WatFalPrior::wst_ReserveReplenish:
					{
						if(SingleStep->GetGroupTarget()<1 || SingleStep->GetGroupTarget()>=m_Reserves.size()) return false;
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
									m_Reserves[SingleStep->GetGroupTarget() - 1]->GetReserveFundFlow().AddFlow(CurrentDate,qMin(AvailableInterest,TotalPayable),TrancheCashFlow::InterestFlow);
									m_Reserves[SingleStep->GetGroupTarget() - 1]->GetReserveFundFlow().AddFlow(CurrentDate, qMax(0.0, TotalPayable - AvailableInterest) - m_Reserves.at(SingleStep->GetGroupTarget() - 1)->GetReserveFundFlow().GetDeferred(CurrentDate), TrancheCashFlow::DeferredFlow);
									m_Reserves[SingleStep->GetGroupTarget() - 1]->SetReserveFundCurrent(m_Reserves.at(SingleStep->GetGroupTarget() - 1)->GetReserveFundCurrent() + qMin(AvailableInterest, TotalPayable));
									AvailableInterest=qMax(0.0,AvailableInterest-TotalPayable);
								}
								else if(SingleStep->GetRedemptionGroup()==2){

									m_Reserves[SingleStep->GetGroupTarget() - 1]->GetReserveFundFlow().AddFlow(CurrentDate, qMin(AvailablePrincipal, TotalPayable), TrancheCashFlow::PrincipalFlow);
									m_Reserves[SingleStep->GetGroupTarget() - 1]->GetReserveFundFlow().AddFlow(CurrentDate, qMax(0.0, TotalPayable - AvailablePrincipal) - m_Reserves.at(SingleStep->GetGroupTarget() - 1)->GetReserveFundFlow().GetDeferred(CurrentDate), TrancheCashFlow::DeferredFlow);
									m_Reserves[SingleStep->GetGroupTarget() - 1]->SetReserveFundCurrent(m_Reserves.at(SingleStep->GetGroupTarget() - 1)->GetReserveFundCurrent() + qMin(AvailablePrincipal, TotalPayable));
									AvailablePrincipal=qMax(0.0,AvailablePrincipal-TotalPayable);
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
							AdjustedCoupon=(m_Tranches.at(h)->GetCoupon(CurrentDate))*static_cast<double>(RollingLastIPD.daysTo(RollingNextIPD))/360.0;
							Solution=m_Tranches.at(h)->GetCashFlow().GetPreviousDeferred(CurrentDate);
							TotalPayable+=AdjustedCoupon*(Solution+m_Tranches.at(h)->GetCurrentOutstanding());
							m_Tranches[h]->AddCashFlow(CurrentDate,Solution,TrancheCashFlow::DeferredFlow);
						}
					}
					while(ProRataBonds.size()>0){
						AdjustedCoupon=(m_Tranches.at(ProRataBonds.head())->GetCoupon(CurrentDate))*static_cast<double>(RollingLastIPD.daysTo(RollingNextIPD))/360.0;
						Solution=AdjustedCoupon*(m_Tranches.at(ProRataBonds.head())->GetCashFlow().GetPreviousDeferred(CurrentDate)+m_Tranches.at(ProRataBonds.head())->GetCurrentOutstanding());
						if(AvailableInterest>=TotalPayable){
							m_Tranches[ProRataBonds.dequeue()]->AddCashFlow(CurrentDate,Solution,TrancheCashFlow::InterestFlow);
						}
						else{
							int ProrataIndex=ProRataBonds.dequeue();
							m_Tranches[ProrataIndex]->AddCashFlow(CurrentDate,AvailableInterest*Solution/TotalPayable,TrancheCashFlow::InterestFlow);
							m_Tranches[ProrataIndex]->AddCashFlow(CurrentDate,Solution-(AvailableInterest*Solution/TotalPayable),TrancheCashFlow::DeferredFlow);
						}
					}
					AvailableInterest=qMax(AvailableInterest-TotalPayable,0.0);
				break;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				case WatFalPrior::wst_DeferredInterest:
				case WatFalPrior::wst_DeferredPrinc:
					ProRataBonds.clear();
					TotalPayable=0.0;
					for(int h=0;h<m_Tranches.size();h++){
						if(m_Tranches.at(h)->GetProrataGroup()==SingleStep->GetGroupTarget()){
							ProRataBonds.enqueue(h);
							TotalPayable+=m_Tranches.at(h)->GetCashFlow().GetDeferred(CurrentDate);
						}
					}
					if(TotalPayable>0.0){
						if(SingleStep->GetPriorityType()==WatFalPrior::wst_DeferredInterest){
							Solution=AvailableInterest;
							AvailableInterest=qMax(AvailableInterest-TotalPayable,0.0);
						}
						else{
							Solution=AvailablePrincipal;
							AvailablePrincipal=qMax(AvailablePrincipal-TotalPayable,0.0);
						}
						while(ProRataBonds.size()>0){
							if(Solution>=TotalPayable){
								int ProrataIndex=ProRataBonds.dequeue();
								m_Tranches[ProrataIndex]->AddCashFlow(CurrentDate,m_Tranches.at(ProrataIndex)->GetCashFlow().GetDeferred(CurrentDate),TrancheCashFlow::InterestFlow);
								m_Tranches[ProrataIndex]->AddCashFlow(CurrentDate,-m_Tranches.at(ProrataIndex)->GetCashFlow().GetDeferred(CurrentDate),TrancheCashFlow::DeferredFlow);
							}
							else{
								int ProrataIndex=ProRataBonds.dequeue();
								m_Tranches[ProrataIndex]->AddCashFlow(CurrentDate,(Solution/TotalPayable)*m_Tranches.at(ProrataIndex)->GetCashFlow().GetDeferred(CurrentDate),TrancheCashFlow::InterestFlow);
								m_Tranches[ProrataIndex]->AddCashFlow(CurrentDate,-(Solution/TotalPayable)*m_Tranches.at(ProrataIndex)->GetCashFlow().GetDeferred(CurrentDate),TrancheCashFlow::DeferredFlow);
							}
						}
					}
				break;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				case  WatFalPrior::wst_Principal:
					AvailablePrincipal=RedeemNotes(AvailablePrincipal,SingleStep->GetGroupTarget(),i);
				break;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				case WatFalPrior::wst_Excess:
					m_AnnualizedExcess.AddFlow(CurrentDate,AvailableInterest,TrancheCashFlow::InterestFlow);
					m_EquityIncome.AddFlow(CurrentDate,AvailablePrincipal,TrancheCashFlow::PrincipalFlow);
					m_EquityIncome.AddFlow(CurrentDate,AvailableInterest,TrancheCashFlow::InterestFlow);
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
									m_Tranches[ProrataIndex]->AddCashFlow(CurrentDate,AvailableInterest*m_Tranches.at(ProrataIndex)->GetOriginalAmount()/TotalPayable,TrancheCashFlow::InterestFlow);
									m_Tranches[ProrataIndex]->AddCashFlow(CurrentDate,AvailablePrincipal*m_Tranches.at(ProrataIndex)->GetOriginalAmount()/TotalPayable,TrancheCashFlow::PrincipalFlow);
								} else {
									m_Tranches[ProrataIndex]->AddCashFlow(CurrentDate,AvailableInterest/OriginalProRataBondsSize,TrancheCashFlow::InterestFlow);
									m_Tranches[ProrataIndex]->AddCashFlow(CurrentDate,AvailablePrincipal/OriginalProRataBondsSize,TrancheCashFlow::PrincipalFlow);
								}
							}
						}
					}
					else{
						m_ExcessCashFlow.AddFlow(CurrentDate,AvailablePrincipal,TrancheCashFlow::PrincipalFlow);
						m_ExcessCashFlow.AddFlow(CurrentDate,AvailableInterest,TrancheCashFlow::InterestFlow);
					}
					AvailableInterest=0.0;
					AvailablePrincipal=0.0;
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
					Solution+=AvailablePrincipal;
					if(Solution==0.0) Solution=1.0;
					while(ProRataBonds.size()>0){
						if(m_Tranches.at(ProRataBonds.head())->GetCashFlow().GetOCTest(CurrentDate)<=0.0)
							m_Tranches[ProRataBonds.head()]->AddCashFlow(CurrentDate,Solution/TotalPayable,TrancheCashFlow::OCFlow);
						TestTarget=m_Tranches.at(ProRataBonds.dequeue())->GetMinOClevel();
					}
					//if it fails redeem notes until cured
					if(Solution/TotalPayable<TestTarget){
						//Calculate the amount needed to cure the test
						//If the need is greater than the available funds adjust it down
						if(SingleStep->GetPriorityType()== WatFalPrior::wst_OCTestPrinc){
							TotalPayable=qMin(TotalPayable-(Solution/TestTarget),AvailablePrincipal);
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
								+RedeemNotes(SingleStep->GetRedemptionShare()*TotalPayable,SingleStep->GetRedemptionGroup(),i);
						}
						TotalPayable=RedeemSequential(TotalPayable,i);
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
					adjSeniorExpenses = m_SeniorExpenses.GetValue(CurrentDate)*static_cast<double>(RollingNextIPD.daysTo(RollingNextIPD.addMonths(m_PaymentFrequency.GetValue(RollingNextIPD)))) / 360.0;
					adjSeniorFees = m_SeniorFees.GetValue(CurrentDate)*static_cast<double>(RollingNextIPD.daysTo(RollingNextIPD.addMonths(m_PaymentFrequency.GetValue(RollingNextIPD)))) / 360.0;
					ProRataBonds.clear();
					TotalPayable=0.0;
					Solution=m_InterestAvailable+m_MortgagesPayments.GetAccruedInterest(i)-((adjSeniorFees+adjSeniorExpenses)*m_MortgagesPayments.GetAmountOut(i));
					for(int h=0;h<m_Tranches.size();h++){
						if(m_Tranches.at(h)->GetProrataGroup()<=SingleStep->GetGroupTarget()){
							AdjustedCoupon=(m_Tranches.at(h)->GetCoupon(CurrentDate))*static_cast<double>(RollingNextIPD.daysTo(RollingNextIPD.addMonths(m_PaymentFrequency.GetValue(RollingNextIPD))))/360.0;
							TotalPayable+=AdjustedCoupon*(m_Tranches.at(h)->GetCurrentOutstanding()+m_Tranches.at(h)->GetCashFlow().GetDeferred(CurrentDate));
							if(m_Tranches.at(h)->GetProrataGroup()==SingleStep->GetGroupTarget()) ProRataBonds.enqueue(h);
						}
					}
					TotalPayable=qMax(TotalPayable,0.000001);
					while(ProRataBonds.size()>0){
						if(m_Tranches.at(ProRataBonds.head())->GetCashFlow().GetICTest(CurrentDate)<=0.0)
							m_Tranches[ProRataBonds.head()]->AddCashFlow(CurrentDate,Solution/TotalPayable,TrancheCashFlow::ICFlow);
						TestTarget=m_Tranches.at(ProRataBonds.dequeue())->GetMinIClevel();
					}
					//if it fails redeem senior notes until cured
					if(Solution/TotalPayable<TestTarget && ((AvailablePrincipal>0 && SingleStep->GetPriorityType()==WatFalPrior::wst_ICTestPrinc)||(AvailableInterest>0 && SingleStep->GetPriorityType()==WatFalPrior::wst_ICTest))){
						SolutionDegree=1;
						InterestPayableBefore=(m_InterestAvailable + m_MortgagesPayments.GetAccruedInterest(i) - ((adjSeniorExpenses + adjSeniorFees) * m_MortgagesPayments.GetAmountOut(i)))/TestTarget;
						TotalPayable=0;
						bool SolutionFound;
						do{
							SolutionFound=true;
							Solution=0.0;
							for(int h=0;h<m_Tranches.size();h++){
								if(m_Tranches.at(h)->GetProrataGroup()<=SingleStep->GetGroupTarget() && m_Tranches.at(h)->GetProrataGroup()>=SolutionDegree){
									AdjustedCoupon=(m_Tranches.at(h)->GetCoupon(CurrentDate))*static_cast<double>(RollingNextIPD.daysTo(RollingNextIPD.addMonths((m_PaymentFrequency.GetValue(RollingNextIPD)))))/360.0;
									Solution+=AdjustedCoupon*(m_Tranches.at(h)->GetCurrentOutstanding()+m_Tranches.at(h)->GetCashFlow().GetDeferred(CurrentDate));
								}
							}
							if(GroupWACoupon(SolutionDegree)>0.0)
								TotalPayable=(Solution-InterestPayableBefore)/GroupWACoupon(SolutionDegree); //Solution-InterestPayableBefore changed SolutionDegree to Solution
							if(TotalPayable>GroupOutstanding(SolutionDegree) || GroupWACoupon(SolutionDegree)<=0){
								if(++SolutionDegree<=SingleStep->GetGroupTarget()) SolutionFound=false;
								else TotalPayable=0;
							}
							if(SolutionFound){
								for(int h=1;h<SolutionDegree;h++) TotalPayable+=GroupOutstanding(h);
								if(SingleStep->GetPriorityType()==WatFalPrior::wst_ICTestPrinc)
									AvailablePrincipal+=RedeemSequential(qMin(TotalPayable,AvailablePrincipal),i)-qMin(TotalPayable,AvailablePrincipal);
								else
									AvailableInterest+=RedeemSequential(qMin(TotalPayable,AvailableInterest),i)-qMin(TotalPayable,AvailableInterest);
							}
						}while(!SolutionFound);
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
						Solution += AvailablePrincipal;
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
								m_Reinvested.AddFlow(CurrentDate,TotalPayable,TrancheCashFlow::InterestFlow);
								AvailableInterest+=m_ReinvestmentTest.GetBondCashFlow().GetInterest(CurrentDate);
								AvailablePrincipal+=m_ReinvestmentTest.GetBondCashFlow().GetPrincipal(CurrentDate);
							}
							//Redeem
							if(SingleStep->GetRedemptionGroup()>0) Solution=RedeemNotes(Solution,SingleStep->GetRedemptionGroup(),i);
							else Solution=RedeemSequential(Solution,i);
							AvailableInterest+=Solution;
						}
					}
				break;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				case WatFalPrior::wst_ReinvestPrincipal:
					if(CurrentDate<=m_ReinvestmentTest.GetReinvestmentPeriod() && !IsCallPaymentDate){
						if(AvailablePrincipal>0.0){
							double PayablePrincipal = AvailablePrincipal; //Reinvest all principal
							if (SingleStep->GetRedemptionGroup() == 1)  //Reinvest Prepay Only
								PayablePrincipal = qMin(AvailablePrincipal, m_MortgagesPayments.GetPrepay(i));
							else if (SingleStep->GetRedemptionGroup() == 2)  //Reinvest scheduled only
								PayablePrincipal = qMax(0.0, AvailablePrincipal - cumulativePrepay);
							AvailablePrincipal -= PayablePrincipal;
							m_ReinvestmentTest.CalculateBondCashFlows(PayablePrincipal, CurrentDate, i);
							m_MortgagesPayments.AddFlow(m_ReinvestmentTest.GetBondCashFlow());
							m_Reinvested.AddFlow(CurrentDate, PayablePrincipal, TrancheCashFlow::PrincipalFlow);
							AvailableInterest+=m_ReinvestmentTest.GetBondCashFlow().GetInterest(CurrentDate);
							AvailablePrincipal += m_ReinvestmentTest.GetBondCashFlow().GetPrincipal(CurrentDate);
						}
					}	
				break;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				default:
					return false;
				}//End Switch
			}//End Cicle through the steps of the waterfall
			m_PrincipalAvailable=0.0;
			m_InterestAvailable=0.0;
			RollingLastIPD=RollingNextIPD;
			RollingNextIPD=RollingNextIPD.addMonths(m_PaymentFrequency.GetValue(RollingNextIPD));
			cumulativePrepay = 0.0;
			if(IsCallPaymentDate){
				i++;
				break;
			}
		}//End Cicle in time
		for(int j=0;j<m_Reserves.size();j++){
			m_Reserves[j]->SetReserveFundCurrent(m_Reserves[j]->GetStartingReserve());
			m_Reserves[j]->ResetMissingAnchors();
		}
		m_PrincipalAvailable=OriginalAvailablePrincipal;
		m_InterestAvailable=OriginalAvailableInterest;
		m_CalculatedMtgPayments=m_MortgagesPayments;
		m_MortgagesPayments=OriginalMtgFlows;
		FillAllDates();
		if(NullCCCanchor[0]) m_CCCcurve.RemoveAnchorDate();
		if(NullCCCanchor[1]) m_PaymentFrequency.RemoveAnchorDate();
		if (NullCCCanchor[2]) m_SeniorExpenses.RemoveAnchorDate();
		if (NullCCCanchor[3]) m_SeniorFees.RemoveAnchorDate();
		if (NullCCCanchor[4]) m_JuniorFees.RemoveAnchorDate();
		if (NullCCCanchor[5]) m_SeniorExpensesFixed.RemoveAnchorDate();
		if (NullCCCanchor[6]) m_SeniorFeesFixed.RemoveAnchorDate();
		if (NullCCCanchor[7]) m_JuniorFeesFixed.RemoveAnchorDate();
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
		<< flows.m_PrincipalAvailable
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
		>> m_PrincipalAvailable
		>> m_InterestAvailable
		>> m_JuniorFeesCoupon
	;
	m_AnnualizedExcess.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_AnnualizedExcess;
	m_EquityIncome.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_EquityIncome;
	stream >> m_FirstIPDdate;
	m_Reinvested.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_Reinvested;
	stream >> m_LastIPDdate
		>> m_CallDate
		>> m_PoolValueAtCall
		>> m_UseCall
		>> m_CallMultiple
		>> m_CallReserve
		>> m_DealName
		>> TempInt
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
					ActualCallReserveLevel += (*ResIter)->GetReserveFundFlow().GetTotalFlow(RollingNextIPD);
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
	if (m_SeniorExpenses.GetVector().isEmpty()) Result += "Senior Expenses\n";
	if (m_SeniorFees.GetVector().isEmpty()) Result += "Senior Fees\n";
	if (m_JuniorFees.GetVector().isEmpty()) Result += "Junior Fees\n";
	if (m_SeniorExpensesFixed.GetVector().isEmpty()) Result += "Senior Expenses Fixed\n";
	if (m_SeniorFeesFixed.GetVector().isEmpty()) Result += "Senior Fees Fixed\n";
	if (m_JuniorFeesFixed.GetVector().isEmpty()) Result += "Junior Fees Fixed\n";
	if(m_Tranches.isEmpty()) Result+="Tranches\n";
	if(m_WaterfallStesps.isEmpty()) Result+="Waterfall Steps\n";
	if(m_MortgagesPayments.Count()==0) Result+="Loans Payments\n";
	if(m_PaymentFrequency.GetVector().isEmpty()) Result+="IDP Frequency\n";
	if(m_ReinvestmentTest.GetCDRAssumption().isEmpty())Result+="CDR\n";
	if(m_ReinvestmentTest.GetCPRAssumption().isEmpty())Result+="CPR\n";
	if(m_ReinvestmentTest.GetLSAssumption().isEmpty())Result+="LS\n";
	if(m_ReinvestmentTest.GetWALAssumption().isEmpty())Result+="Reinvestment Bond WAL\n";
	if (m_ReinvestmentTest.GetReinvDelay().isEmpty())Result += "Reinvestment Delay\n";
	if (m_ReinvestmentTest.GetReinvPrice().isEmpty())Result += "Reinvestment Price\n";
	if(m_ReinvestmentTest.GetReinvestmentPeriod()<QDate(2000,1,1))Result+="Reinvestment Period\n";
	if(m_ReinvestmentTest.GetTestLevel()<0.0)Result+="Reinvestment Test Limit\n";
	if(m_ReinvestmentTest.GetReinvestmentBond().GetPaymentFreq().IsEmpty(1)) Result+="Reinvestment Bond Payment Frequency\n";
	if(m_ReinvestmentTest.GetReinvestmentBond().GetInterest().isEmpty())Result+="Reinvestment Bond Spread\n";
	if(m_CCCcurve.IsEmpty())Result+="CCC Curve\n";
	if(m_CCChaircut<0.0)Result+="CCC Value\n";
	if(m_PoolValueAtCall<0.0)Result+="Pool Value at Call\n";
	if(m_UseCall && m_CallDate<QDate(2000,1,1) && (m_CallReserve<=0.0 || m_CallMultiple<=0.0))Result+="Specify a call Criteria to use the Call\n";
	if(m_MortgagesPayments.Count()>0 && m_MortgagesPayments.GetDate(0)<QDate(2000,1,1))Result+="Pool Cut Off Date\n";
	if(m_CCCTestLimit<0.0)Result+="CCC Test Limit\n";
	if(
		m_ReinvestmentTest.GetShare(ReinvestmentTest::InRedempShare)<0.0
		|| m_ReinvestmentTest.GetShare(ReinvestmentTest::OutRedempShare)<0.0
		|| m_ReinvestmentTest.GetShare(ReinvestmentTest::InReinvShare)<0.0
		|| m_ReinvestmentTest.GetShare(ReinvestmentTest::OutReinvShare)<0.0
		|| m_ReinvestmentTest.GetShare(ReinvestmentTest::InRedempShare)+m_ReinvestmentTest.GetShare(ReinvestmentTest::InReinvShare)>1.0
		|| m_ReinvestmentTest.GetShare(ReinvestmentTest::OutRedempShare)+m_ReinvestmentTest.GetShare(ReinvestmentTest::OutReinvShare)>1.0
	)Result+=QString("Reinvestment Test Shares InRedempShare: %1 | OutRedempShare: %2 | InReinvShare: %3 | OutReinvShare: %4 | %5 | %6\n")
		.arg(m_ReinvestmentTest.GetShare(ReinvestmentTest::InRedempShare))
		.arg(m_ReinvestmentTest.GetShare(ReinvestmentTest::OutRedempShare))
		.arg(m_ReinvestmentTest.GetShare(ReinvestmentTest::InReinvShare))
		.arg(m_ReinvestmentTest.GetShare(ReinvestmentTest::OutReinvShare))
		.arg(m_ReinvestmentTest.GetShare(ReinvestmentTest::InRedempShare)+m_ReinvestmentTest.GetShare(ReinvestmentTest::InReinvShare))
		.arg(m_ReinvestmentTest.GetShare(ReinvestmentTest::OutRedempShare)+m_ReinvestmentTest.GetShare(ReinvestmentTest::OutReinvShare))
	;
	if(m_FirstIPDdate<QDate(2000,1,1))Result+="Next IDP\n";
	if(m_LastIPDdate<QDate(2000,1,1))Result+="Last IDP\n";
	for (int ResIter = 0; ResIter<m_Reserves.size(); ResIter++) {
		if (m_Reserves.at(ResIter)->GetReserveFundTargetString().isEmpty()) Result+=QString("Reserve %1 Target Level\n").arg(ResIter+1);
		if (m_Reserves.at(ResIter)->GetReserveFundMultipleString().isEmpty()) Result += QString("Reserve %1 Multiple\n").arg(ResIter + 1);
		if (m_Reserves.at(ResIter)->GetReserveFundFloorString().isEmpty())Result += QString("Reserve %1 Floor Level\n").arg(ResIter + 1);
		if (m_Reserves.at(ResIter)->GetReserveFundFreed()<0) Result += QString("Reserve %1 Freed After Redemption\n").arg(ResIter + 1);
		if (m_Reserves.at(ResIter)->GetReserveFundCurrent()<0.0) Result += QString("Reserve %1 Current Amount\n").arg(ResIter + 1);
	}
	if(!Result.isEmpty()) return Result.left(Result.size()-1);
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
		if(denominator>0) return qPow(1.0+(m_EquityIncome.GetTotalFlow(index)/denominator),12.0/((AdjPaymentFreq.GetValue(m_EquityIncome.GetDate(index)))))-1.0;
		else return 0.0;
	}
	denominator=0.0;
	foreach(Tranche* SingleTranche, m_Tranches){
		denominator+=SingleTranche->GetOriginalAmount();
	}
	denominator=m_CalculatedMtgPayments.GetAmountOut(0)-denominator;
	if(denominator>0) return qPow(1.0+(m_EquityIncome.GetTotalFlow(index)/denominator),12.0/((AdjPaymentFreq.GetValue(m_EquityIncome.GetDate(index)))))-1.0;
	else return 0.0;

}
double Waterfall::GetCumulativeEquityReturn(int index)const{
	if(index<0 || index>=m_EquityIncome.Count()) return 0.0;
	double numerator=0.0;
	for(int i=0;i<=index;i++) numerator+=m_EquityIncome.GetTotalFlow(i);
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
	double numerator=(m_PoolValueAtCall*m_CalculatedMtgPayments.GetAmountOut(MtgIndex)/100.0);
	if(EquityTranche>0){
		foreach(Tranche* SingleTranche, m_Tranches){
			if(SingleTranche->GetProrataGroup()==EquityTranche) denominator+=SingleTranche->GetCashFlow().GetAmountOutstanding(index);
			else numerator-=SingleTranche->GetCashFlow().GetAmountOutstanding(index);
		}
		if(denominator>0) return numerator/denominator;
		else return 0.0;
	}
	denominator=0.0;
	numerator=(m_PoolValueAtCall*m_CalculatedMtgPayments.GetAmountOut(MtgIndex)/100.0);
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