#include "CommonFunctions.h"
#include "MtgCashFlow.h"
#include <QtGlobal>
#include<QMap>
int MtgCashFlow::GetPaymentFrequency() const{
	if (m_FlowDates.size()>1)
		return qAbs(MonthDiff(m_FlowDates.last(),*(m_FlowDates.end()-2)));
	else
		return 0;
}
int MtgCashFlow::Count() const{
	return m_FlowDates.size();
}
QDate MtgCashFlow::GetDate(int index)const{
	if(index<0 || index>m_FlowDates.size()-1) return QDate();
	return m_FlowDates.at(index);
}
double MtgCashFlow::GetInterest(int index) const{
	if(index<0 || index>m_Interest.size()-1) return 0.0;
	return m_Interest.at(index);
}
double MtgCashFlow::GetScheduled(int index) const{
	if(index<0 || index>m_Scheduled.size()-1) return 0.0;
	return m_Scheduled.at(index);
}
double MtgCashFlow::GetPrepay(int index) const{
	if(index<0 || index>m_Prepay.size()-1) return 0.0;
	return m_Prepay.at(index);
}
double MtgCashFlow::GetLoss(int index) const{
	if(index<0 || index>m_Loss.size()-1) return 0.0;
	return m_Loss.at(index);
}
double MtgCashFlow::GetAccruedInterest(int index) const{
	if(index<0 || index>m_AccruedInterest.size()-1) return 0.0;
	return m_AccruedInterest.at(index);
}
double MtgCashFlow::GetLossOnInterest(int index) const{
	if(index<0 || index>m_LossOnInterest.size()-1) return 0.0;
	return m_LossOnInterest.at(index);
}
double MtgCashFlow::GetAmountOut(int index) const{
	if(index<0 || index>m_AmountOut.size()-1) return 0.0;
	return m_AmountOut.at(index);
}
double MtgCashFlow::GetWAcoupon(int index) const{
	if(index<0 || index>m_CoupTimesOut.size()-1 || GetAmountOut(index)==0.0) return 0.0;
	return m_CoupTimesOut.at(index)/GetAmountOut(index);
}
double MtgCashFlow::GetWAcoupon(const QDate& index) const{
	int NewIndex=-1;
	for(int i=0;i<m_FlowDates.size();i++){
		if(m_FlowDates.at(i).year()==index.year() && m_FlowDates.at(i).month()==index.month()){
			NewIndex=i;
			break;
		}
	}
	return GetWAcoupon(NewIndex);
}
int  MtgCashFlow::FindDate(const QDate& a) const{
	return m_FlowDates.indexOf(a);
}
void MtgCashFlow::AddFlow(const QDate& Dte, double Amt, MtgFlowType FlowTpe){
	int DateIndex = m_FlowDates.indexOf(Dte);
	bool NeedSorting = false;
	if(DateIndex<0){
		m_FlowDates.append(Dte);
		m_Interest.append(0.0);
		m_Scheduled.append(0.0);
		m_Prepay.append(0.0);
		m_Loss.append(0.0);
		m_AmountOut.append(0.0);
		m_AccruedInterest.append(0.0);
		m_LossOnInterest.append(0.0);
		m_CoupTimesOut.append(0.0);
		DateIndex=m_FlowDates.size()-1;
		NeedSorting=true;
	}
	switch(FlowTpe){
	case InterestFlow:
		m_Interest[DateIndex]+=Amt; break;
	case PrincipalFlow:
		m_Scheduled[DateIndex]+=Amt; break;
	case PrepaymentFlow:
		m_Prepay[DateIndex]+=Amt; break;
	case LossFlow:
		m_Loss[DateIndex]+=Amt; break;
	case AmountOutstandingFlow:
		m_AmountOut[DateIndex]+=Amt; break;
	case AccruedInterestFlow:
		m_AccruedInterest[DateIndex]+=Amt; break;
	case LossOnInterestFlow:
		m_LossOnInterest[DateIndex]+=Amt; break;
	case WACouponFlow:
		m_CoupTimesOut[DateIndex]+=Amt; break;
	}
	if(NeedSorting) SortByDate();
}
void MtgCashFlow::AddFlow(const MtgCashFlow& a){
	bool NeedSorting = false;
	int DateIndex;
	for(int i=0;i<a.Count();i++){
		DateIndex = m_FlowDates.indexOf(a.GetDate(i));
		if(DateIndex<0){
			m_FlowDates.append(a.GetDate(i));
			m_Interest.append(0.0);
			m_Scheduled.append(0.0);
			m_Prepay.append(0.0);
			m_Loss.append(0.0);
			m_AmountOut.append(0.0);
			m_AccruedInterest.append(0.0);
			m_LossOnInterest.append(0.0);
			m_CoupTimesOut.append(0.0);
			DateIndex=m_FlowDates.size()-1;
			NeedSorting=true;
		}
		m_Interest[DateIndex]+=a.GetInterest(i);
		m_Scheduled[DateIndex]+=a.GetScheduled(i);
		m_Prepay[DateIndex]+=a.GetPrepay(i);
		m_Loss[DateIndex]+=a.GetLoss(i);
		m_AmountOut[DateIndex]+=a.GetAmountOut(i);
		m_CoupTimesOut[DateIndex]+=a.m_CoupTimesOut[i];
		m_AccruedInterest[DateIndex]+=a.GetAccruedInterest(i);
		m_LossOnInterest[DateIndex]+=a.GetLossOnInterest(i);
	}
	if(NeedSorting) SortByDate();
}
void MtgCashFlow::SortByDate(){
	int Orderer;
	QMap<int,QDate> DateSorter;
	QMap<int,double> InerestSorter;
	QMap<int,double> ScheduledSorter;
	QMap<int,double> PrepaySorter;
	QMap<int,double> LossSorter;
	QMap<int,double> AmountOutSorter;
	QMap<int,double> AccruedInterestSorter;
	QMap<int,double> LossOnInterestSorter;
	QMap<int,double> CoupTimesOutSorter;
	for (int i=0;i<m_FlowDates.size();i++){
		Orderer=0;
		for (int j=0;j<m_FlowDates.size();j++){
			if(j==i) continue;
			if(m_FlowDates.at(j)>m_FlowDates.at(i)) Orderer++;
		}
		DateSorter.insert(m_FlowDates.size()-Orderer,m_FlowDates.at(i));
		InerestSorter.insert(m_FlowDates.size()-Orderer,m_Interest.at(i));
		ScheduledSorter.insert(m_FlowDates.size()-Orderer,m_Scheduled.at(i));
		PrepaySorter.insert(m_FlowDates.size()-Orderer,m_Prepay.at(i));
		LossSorter.insert(m_FlowDates.size()-Orderer,m_Loss.at(i));
		AmountOutSorter.insert(m_FlowDates.size()-Orderer,m_AmountOut.at(i));
		AccruedInterestSorter.insert(m_FlowDates.size()-Orderer,m_AccruedInterest.at(i));
		LossOnInterestSorter.insert(m_FlowDates.size()-Orderer,m_LossOnInterest.at(i));
		CoupTimesOutSorter.insert(m_FlowDates.size()-Orderer,m_CoupTimesOut.at(i));
	}
	m_FlowDates=DateSorter.values();
	m_Interest=InerestSorter.values();
	m_Scheduled=ScheduledSorter.values();
	m_Prepay=PrepaySorter.values();
	m_Loss=LossSorter.values();
	m_AmountOut=AmountOutSorter.values();
	m_AccruedInterest=AccruedInterestSorter.values();
	m_LossOnInterest=LossOnInterestSorter.values();
	m_CoupTimesOut=CoupTimesOutSorter.values();
}
void MtgCashFlow::RemoveAllFlows(){
	m_FlowDates.clear();
	m_Interest.clear();
	m_Scheduled.clear();
	m_Prepay.clear();
	m_Loss.clear();
	m_AmountOut.clear();
	m_AccruedInterest.clear();
	m_LossOnInterest.clear();
	m_CoupTimesOut.clear();
}
const MtgCashFlow& MtgCashFlow::operator=(const MtgCashFlow& a){
	RemoveAllFlows();
	AddFlow(a);
	return *this;
}
QDataStream& operator<<(QDataStream & stream, const MtgCashFlow& flows){
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
	;
	return stream;
}
QDataStream& operator>>(QDataStream & stream, MtgCashFlow& flows){
	stream 
		>> flows.m_FlowDates
		>> flows.m_Interest
		>> flows.m_Scheduled
		>> flows.m_Prepay
		>> flows.m_Loss
		>> flows.m_AmountOut
		>> flows.m_CoupTimesOut
		>> flows.m_AccruedInterest
		>> flows.m_LossOnInterest
		;
	return stream;
}