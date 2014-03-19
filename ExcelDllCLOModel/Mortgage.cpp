#include "Mortgage.h"
#include "CommonFunctions.h"
#include <qmath.h>
Mortgage::Mortgage()
	:m_LossMultiplier("100")
	,m_PrepayMultiplier("100")
	,m_FloatingRateBase(0.0)
	,m_PaymentFreq(1)
	,m_AnnuityVect("N")
	,m_InterestVect("0")
	,m_Size(0.0)
{}
void Mortgage::SetAnnuity(const QString& a){
	m_AnnuityVect=a;
}
void Mortgage::SetInterest(const QString& a){
	m_InterestVect=a;
}
 Mortgage::Mortgage(const Mortgage& a)
	 :m_LossMultiplier(a.m_LossMultiplier)
	 ,m_PrepayMultiplier(a.m_PrepayMultiplier)
	 ,m_FloatingRateBase(a.m_FloatingRateBase)
	 ,m_PaymentFreq(a.m_PaymentFreq)
	 ,m_AnnuityVect(a.m_AnnuityVect)
	 ,m_InterestVect(a.m_InterestVect)
	 ,m_Size(a.m_Size)
	 ,m_MaturityDate(a.m_MaturityDate)
	 ,m_CashFlows(a.m_CashFlows)
 {}
 const Mortgage& Mortgage::operator=(const Mortgage& a){
	 m_LossMultiplier=a.m_LossMultiplier;
	 m_PrepayMultiplier=a.m_PrepayMultiplier;
	 m_FloatingRateBase=a.m_FloatingRateBase;
	 m_PaymentFreq=a.m_PaymentFreq;
	 m_AnnuityVect=a.m_AnnuityVect;
	 m_InterestVect=a.m_InterestVect;
	 m_Size=a.m_Size;
	 m_MaturityDate=a.m_MaturityDate;
	 m_CashFlows=a.m_CashFlows;
	 return *this;
 }
 void Mortgage::CalculateCashFlows(const QString& CPRVecs,const QString& CDRVecs,const QString& LossVecs,const QDate& StartDate){
	 QDate AdjStartDate(StartDate.year(),StartDate.month(),15);
	 BloombergVector CPRVec(CPRVecs,AdjStartDate);
	 BloombergVector CDRVec(CDRVecs,AdjStartDate);
	 BloombergVector LossVec(LossVecs,AdjStartDate);
	 CalculateCashFlows(CPRVec,CDRVec,LossVec,StartDate);
 }
void Mortgage::CalculateCashFlows(const BloombergVector& CPRVecs,const BloombergVector& CDRVecs,const BloombergVector& LossVecs,const QDate& StartDate){
	QDate AdjStartDate(StartDate.year(),StartDate.month(),15);
	BloombergVector CPRVec(CPRVecs);
	BloombergVector CDRVec(CDRVecs);
	BloombergVector LossVec(LossVecs);
	bool NullAnchorDates[]={m_InterestVect.GetAnchorDate().isNull(),m_LossMultiplier.GetAnchorDate().isNull(),m_PrepayMultiplier.GetAnchorDate().isNull()};
	if(CPRVec.GetAnchorDate().isNull()) CPRVec.SetAnchorDate(AdjStartDate);
	if(CDRVec.GetAnchorDate().isNull()) CDRVec.SetAnchorDate(AdjStartDate);
	if(LossVec.GetAnchorDate().isNull()) LossVec.SetAnchorDate(AdjStartDate);
	if(NullAnchorDates[0]) m_InterestVect.SetAnchorDate(AdjStartDate);
	if(NullAnchorDates[1]) m_LossMultiplier.SetAnchorDate(AdjStartDate);
	if(NullAnchorDates[2]) m_PrepayMultiplier.SetAnchorDate(AdjStartDate);
	if (StartDate >= m_MaturityDate) return m_CashFlows.AddFlow (StartDate, m_Size, MtgCashFlow::PrincipalFlow);
	int NumPayments=qAbs(MonthDiff(m_MaturityDate,StartDate));
	if(
		CPRVec.IsEmpty()
		|| CDRVec.IsEmpty()
		|| LossVec.IsEmpty()
		|| NumPayments<1
	) return;
	
	m_CashFlows.RemoveAllFlows();
	double CurrentCPR;
	double CurrentCDR;
	double CurrentLS;
	double CurrentInterest;
	QString CurrentAnnuity;
	QDate CurrentMonth=AdjStartDate.addMonths(1);
	double CurrentAmtOut=m_Size;
	double TempFlow;
	QDate NextPaymentDate=AdjStartDate.addMonths(m_PaymentFreq);
	if(NextPaymentDate > m_MaturityDate)
		NextPaymentDate.setDate(m_MaturityDate.year(),m_MaturityDate.month(),15);
	for (int j=0;j<NumPayments;j++){
		CurrentCPR=CPRVec.GetValue(CurrentMonth,1);
		CurrentCDR=CDRVec.GetValue(CurrentMonth,1);
		CurrentLS=LossVec.GetValue(CurrentMonth);
		CurrentInterest=qPow(1.0+m_FloatingRateBase,1.0/12.0)-1.0+m_InterestVect.GetValue(CurrentMonth,1);
		CurrentAnnuity=m_AnnuityVect.GetValue(CurrentMonth);
		TempFlow=(CurrentInterest*CurrentAmtOut)+((1.0+CurrentInterest)*m_CashFlows.GetAccruedInterest(m_CashFlows.Count()-1));
		m_CashFlows.AddFlow( CurrentMonth, TempFlow, MtgCashFlow::AccruedInterestFlow);
		if (CurrentMonth >= NextPaymentDate || j == NumPayments){
			m_CashFlows.AddFlow (CurrentMonth, m_CashFlows.GetAccruedInterest(CurrentMonth), MtgCashFlow::InterestFlow);
			m_CashFlows.AddFlow (CurrentMonth, -m_CashFlows.GetAccruedInterest(CurrentMonth), MtgCashFlow::AccruedInterestFlow);
			if(j == NumPayments-1){TempFlow = CurrentAmtOut;}
			else if (CurrentAnnuity == "Y"){
				int CountPeriods= 0;//CurrentMonth == QDate(m_MaturityDate.year(),m_MaturityDate.month(),15) ? 0:-1;
				QDate RollingdateCounter=CurrentMonth;
				while(RollingdateCounter<=m_MaturityDate){
					CountPeriods++;
					RollingdateCounter=RollingdateCounter.addMonths(m_PaymentFreq);
				}CountPeriods++;
				double InterestApplicable=qPow(1.0+m_FloatingRateBase,static_cast<double>(m_PaymentFreq)/12.0)-1.0+m_InterestVect.GetValue(CurrentMonth,m_PaymentFreq);
				TempFlow=qAbs(pmt(InterestApplicable, CountPeriods, CurrentAmtOut)) - TempFlow;
			}
			else{TempFlow = 0.0;}
			m_CashFlows.AddFlow (CurrentMonth, TempFlow, MtgCashFlow::PrincipalFlow);
			CurrentAmtOut = CurrentAmtOut - TempFlow;
			TempFlow = qMin(CurrentAmtOut,CurrentAmtOut * CurrentCPR * m_PrepayMultiplier.GetValue(CurrentMonth));
			m_CashFlows.AddFlow( CurrentMonth, TempFlow, MtgCashFlow::PrepaymentFlow);
			CurrentAmtOut = CurrentAmtOut - TempFlow;
			NextPaymentDate=NextPaymentDate.addMonths(m_PaymentFreq);
			if(NextPaymentDate > m_MaturityDate) NextPaymentDate.setDate(m_MaturityDate.year(),m_MaturityDate.month(),15);
		} else{
			TempFlow = qMin(CurrentAmtOut,CurrentAmtOut * CurrentCPR * m_PrepayMultiplier.GetValue(CurrentMonth));
			m_CashFlows.AddFlow( CurrentMonth, TempFlow / CurrentAmtOut * m_CashFlows.GetAccruedInterest(CurrentMonth),  MtgCashFlow::InterestFlow);
			m_CashFlows.AddFlow( CurrentMonth, -(TempFlow / CurrentAmtOut * m_CashFlows.GetAccruedInterest(CurrentMonth)),  MtgCashFlow::AccruedInterestFlow);
			m_CashFlows.AddFlow( CurrentMonth, TempFlow,  MtgCashFlow::PrepaymentFlow);
			CurrentAmtOut = CurrentAmtOut - TempFlow;
		}
		TempFlow = m_CashFlows.GetAccruedInterest(CurrentMonth) * CurrentCDR * CurrentLS * m_LossMultiplier.GetValue(CurrentMonth);
		if (CurrentAmtOut > 0){
			m_CashFlows.AddFlow( CurrentMonth, qMin(TempFlow,m_CashFlows.GetAccruedInterest(CurrentMonth)), MtgCashFlow::LossOnInterestFlow);
			m_CashFlows.AddFlow( CurrentMonth, -qMin(TempFlow,m_CashFlows.GetAccruedInterest(CurrentMonth)), MtgCashFlow::AccruedInterestFlow);
		}
		TempFlow = CurrentAmtOut * CurrentCDR * CurrentLS * m_LossMultiplier.GetValue(CurrentMonth);
		m_CashFlows.AddFlow (CurrentMonth, qMin(TempFlow,CurrentAmtOut),  MtgCashFlow::LossFlow);
		CurrentAmtOut = qMax(0.0,CurrentAmtOut - TempFlow);
		m_CashFlows.AddFlow( CurrentMonth, CurrentAmtOut, MtgCashFlow::AmountOutstandingFlow);
		m_CashFlows.AddFlow( CurrentMonth, CurrentAmtOut*(qPow(1.0+CurrentInterest,12.0)-1.0), MtgCashFlow::WACouponFlow);
		CurrentMonth = CurrentMonth.addMonths(1);
		if (CurrentAmtOut < 0.01) break;
	}
	m_CashFlows.AddFlow( StartDate, m_Size, MtgCashFlow::AmountOutstandingFlow);
	m_CashFlows.AddFlow( StartDate, m_Size*m_InterestVect.GetValue(0), MtgCashFlow::WACouponFlow);
	if(NullAnchorDates[0]) m_InterestVect.RemoveAnchorDate();
	if(NullAnchorDates[1]) m_LossMultiplier.RemoveAnchorDate();
	if(NullAnchorDates[2]) m_PrepayMultiplier.RemoveAnchorDate();
 }
 double Mortgage::pmt(double Interest, int Periods, double PresentValue){
	 if(Periods<=0) return 0.0;
	 if(Interest==0.0){
		 return PresentValue/static_cast<double>(Periods);
	 }
	 return (PresentValue*qPow(1.0+Interest,Periods))/((qPow(1.0+Interest,Periods)-1.0)/Interest);
 }
 QDataStream& operator<<(QDataStream & stream, const Mortgage& flows){
	 stream 
		 << flows.m_MaturityDate
		 << flows.m_AnnuityVect
		 << flows.m_Size
		 << flows.m_LossMultiplier
		 << flows.m_PrepayMultiplier
		 << flows.m_InterestVect
		 << flows.m_CashFlows
		 << flows.m_FloatingRateBase
		 << flows.m_PaymentFreq
	;
	 return stream;
 }
 QDataStream& operator>>(QDataStream & stream, Mortgage& flows){
	 stream 
		 >> flows.m_MaturityDate
		 >> flows.m_AnnuityVect
		 >> flows.m_Size
		 >> flows.m_LossMultiplier
		 >> flows.m_PrepayMultiplier
		 >> flows.m_InterestVect
		 >> flows.m_CashFlows
		 >> flows.m_FloatingRateBase
		 >> flows.m_PaymentFreq
		 ;
	 return stream;
 }
 QString Mortgage::ReadyToCalculate()const{
	QString Result;
	if(m_MaturityDate<QDate(2000,1,1)) Result+="Loan Maturity Date\n";
	if(m_AnnuityVect.IsEmpty())Result+="Loan Annuity Vector\n";
	if(m_Size<0.0)Result+="Loan Size\n";
	if(m_LossMultiplier.IsEmpty()) Result+="Loss Multiplier\n";
	if(m_PrepayMultiplier.IsEmpty()) Result+="Prepay Multiplier\n";
	if(m_InterestVect.IsEmpty())Result+="Loan Coupon\n";
	if(m_FloatingRateBase<0.0)Result+="Loan Base Rate\n";
	if(m_PaymentFreq<1)Result+="Loan Payment Frequency\n";
	if(!Result.isEmpty()) return Result.left(Result.size()-1);
	return Result;
 }