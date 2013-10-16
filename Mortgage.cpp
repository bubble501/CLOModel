#include "Mortgage.h"
#include "CommonFunctions.h"
#include <qmath.h>
Mortgage::Mortgage()
	:m_LossMultiplier(100)
	,m_PrepayMultiplier(100)
	,m_FloatingRateBase(0.0)
	,m_PaymentFreq(1)
	,m_AnnuityVect("N")
	,m_InterestVect("0")
	,m_Size(0.0)
{}
void Mortgage::SetAnnuity(const QString& a){
	if(ValidAnnuityVector(a)) m_AnnuityVect=a;
	else m_AnnuityVect="N";
}
void Mortgage::SetInterest(const QString& a){
	if(ValidBloombergVector(a)) m_InterestVect=a;
	else m_InterestVect="0";
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
 void Mortgage::CalculateCashFlows(const QString& CPRVec,const QString& CDRVec,const QString& LossVec,const QDate& StartDate){
	if (StartDate >= m_MaturityDate) return m_CashFlows.AddFlow (StartDate, m_Size, MtgCashFlow::PrincipalFlow);
	int NumPayments=qAbs(MonthDiff(m_MaturityDate,StartDate));
	if(
		!ValidBloombergVector(CPRVec)
		|| !ValidBloombergVector(CDRVec)
		|| !ValidBloombergVector(LossVec)
		|| NumPayments<1
	) return;
	QDate AdjStartDate;
	AdjStartDate.setDate(StartDate.year(),StartDate.month(),15);
	m_CashFlows.RemoveAllFlows();
	QList<double> CPRVector=UnpackVect(CPRVec, m_PaymentFreq);
	QList<double> CDRVector=UnpackVect(CDRVec, m_PaymentFreq);
	QList<double> LossVector=UnpackVect(LossVec, m_PaymentFreq,false);
	QList<double> InterestVector=UnpackVect(m_InterestVect);
	QList<QString> AnnuityVector=UnpackAnnuityVect(m_AnnuityVect);
	double CurrentCPR;
	double CurrentCDR;
	double CurrentLS;
	double CurrentInterest;
	QString CurrentAnnuity;
	QDate CurrentMonth=AdjStartDate.addMonths(1);
	double CurrentAmtOut=m_Size;
	double TempFlow;
	QDate NextPaymentDate=AdjStartDate.addMonths(m_PaymentFreq);
	if(NextPaymentDate > m_MaturityDate) NextPaymentDate.setDate(m_MaturityDate.year(),m_MaturityDate.month(),15);
	for (int j=0;j<NumPayments;j++){
		CurrentCPR=CPRVector.at(qMin(j,CPRVector.size()-1));
		CurrentCDR=CDRVector.at(qMin(j,CDRVector.size()-1));
		CurrentLS=LossVector.at(qMin(j,LossVector.size()-1));
		CurrentInterest=m_FloatingRateBase+InterestVector.at(qMin(j,InterestVector.size()-1));
		CurrentAnnuity=AnnuityVector.at(qMin(j,AnnuityVector.size()-1));
		TempFlow=CurrentInterest*CurrentAmtOut;
		m_CashFlows.AddFlow( CurrentMonth, TempFlow + m_CashFlows.GetAccruedInterest(m_CashFlows.Count()-1), MtgCashFlow::AccruedInterestFlow);
		if (CurrentMonth >= NextPaymentDate || j == NumPayments){
			m_CashFlows.AddFlow (CurrentMonth, m_CashFlows.GetAccruedInterest(CurrentMonth), MtgCashFlow::InterestFlow);
			m_CashFlows.AddFlow (CurrentMonth, -m_CashFlows.GetAccruedInterest(CurrentMonth), MtgCashFlow::AccruedInterestFlow);
			if (CurrentAnnuity == "Y"){
				TempFlow=qAbs(pmt(CurrentInterest, RoundUp(static_cast<double>(NumPayments) / static_cast<double>(m_PaymentFreq)) - static_cast<double>(j) + 1.0, CurrentAmtOut)) - TempFlow;
			}
			else if(j == NumPayments-1){TempFlow = CurrentAmtOut;}
			else{TempFlow = 0.0;}
			m_CashFlows.AddFlow (CurrentMonth, TempFlow, MtgCashFlow::PrincipalFlow);
			CurrentAmtOut = CurrentAmtOut - TempFlow;
			TempFlow = CurrentAmtOut * CurrentCPR * (static_cast<double>(m_PrepayMultiplier) / 100.0);
			m_CashFlows.AddFlow( CurrentMonth, TempFlow, MtgCashFlow::PrepaymentFlow);
			CurrentAmtOut = CurrentAmtOut - TempFlow;
			NextPaymentDate=NextPaymentDate.addMonths(m_PaymentFreq);
			if(NextPaymentDate > m_MaturityDate) NextPaymentDate.setDate(m_MaturityDate.year(),m_MaturityDate.month(),15);
		} else{
			TempFlow = CurrentAmtOut * CurrentCPR * (static_cast<double>(m_PrepayMultiplier) / 100.0);
			m_CashFlows.AddFlow( CurrentMonth, TempFlow / CurrentAmtOut * m_CashFlows.GetAccruedInterest(CurrentMonth),  MtgCashFlow::InterestFlow);
			m_CashFlows.AddFlow( CurrentMonth, -(TempFlow / CurrentAmtOut * m_CashFlows.GetAccruedInterest(CurrentMonth)),  MtgCashFlow::AccruedInterestFlow);
			m_CashFlows.AddFlow( CurrentMonth, TempFlow,  MtgCashFlow::PrepaymentFlow);
			CurrentAmtOut = CurrentAmtOut - TempFlow;
		}
		TempFlow = CurrentAmtOut * CurrentCDR * CurrentLS * (static_cast<double>(m_LossMultiplier) / 100.0);
		if (CurrentAmtOut > 0){
			m_CashFlows.AddFlow( CurrentMonth, CurrentCDR * CurrentLS * (static_cast<double>(m_LossMultiplier) / 100.0) * m_CashFlows.GetAccruedInterest(CurrentMonth), MtgCashFlow::LossOnInterestFlow);
			m_CashFlows.AddFlow( CurrentMonth, -(CurrentCDR * CurrentLS * (static_cast<double>(m_LossMultiplier) / 100.0) * m_CashFlows.GetAccruedInterest(CurrentMonth)), MtgCashFlow::AccruedInterestFlow);
		}
		m_CashFlows.AddFlow (CurrentMonth, TempFlow,  MtgCashFlow::LossFlow);
		CurrentAmtOut = CurrentAmtOut - TempFlow;
		m_CashFlows.AddFlow( CurrentMonth, CurrentAmtOut, MtgCashFlow::AmountOutstandingFlow);
		CurrentMonth = CurrentMonth.addMonths(1);
		if (CurrentAmtOut < 0.01) break;
	}
	m_CashFlows.AddFlow( StartDate, m_Size, MtgCashFlow::AmountOutstandingFlow);
 }
 double Mortgage::pmt(double Interest, int Periods, double PresentValue) const{
	 if(Periods<=0) return 0;
	 if(Interest=0){
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
