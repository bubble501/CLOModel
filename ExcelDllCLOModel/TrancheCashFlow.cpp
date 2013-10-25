#include "TrancheCashFlow.h"
#include <QMap>
#include <QDataStream>
TrancheCashFlow::TrancheCashFlow(double ThrancheOutstanding)
	:OutstandingAmt(ThrancheOutstanding)
{}
QDate TrancheCashFlow::GetDate(int index)const{
	if(index<0 || index>FlowDates.size()-1) return QDate();
	return FlowDates.at(index);
}
double TrancheCashFlow::GetInterest(int index)const{
	if(index<0 || index>Interest.size()-1) return 0.0;
	return Interest.at(index);
}
double TrancheCashFlow::GetPrincipal(int index)const{
	if(index<0 || index>Principal.size()-1) return 0.0;
	return Principal.at(index);
}
double TrancheCashFlow::GetICTest(int index)const{
	if(index<0 || index>ICTest.size()-1) return 0.0;
	return ICTest.at(index);
}
double TrancheCashFlow::GetOCTest(int index)const{
	if(index<0 || index>OCTest.size()-1) return 0.0;
	return OCTest.at(index);
}
double TrancheCashFlow::GetDeferred(int index)const{
	if(index<0 || index>Deferred.size()-1) return 0.0;
	return Deferred.at(index);
}
int TrancheCashFlow::FindDate(const QDate& a)const{
	return FlowDates.indexOf(a);
}
void TrancheCashFlow::SortByDate(){
	int Orderer;
	QMap<int,QDate> DateSorter;
	QMap<int,double> InerestSorter;
	QMap<int,double> PrincipalSorter;
	QMap<int,double> ICSorter;
	QMap<int,double> OCSorter;
	QMap<int,double> DeferredSorter;
	for (int i=0;i<FlowDates.size();i++){
		Orderer=0;
		for (int j=0;j<FlowDates.size();j++){
			if(j==i) continue;
			if(FlowDates.at(j)>FlowDates.at(i)) Orderer++;
		}
		DateSorter.insert(FlowDates.size()-Orderer,FlowDates.at(i));
		InerestSorter.insert(FlowDates.size()-Orderer,Interest.at(i));
		OCSorter.insert(FlowDates.size()-Orderer,OCTest.at(i));
		ICSorter.insert(FlowDates.size()-Orderer,ICTest.at(i));
		PrincipalSorter.insert(FlowDates.size()-Orderer,Principal.at(i));
		DeferredSorter.insert(FlowDates.size()-Orderer,Deferred.at(i));
	}
	FlowDates=DateSorter.values();
	Interest=InerestSorter.values();
	Principal=PrincipalSorter.values();
	OCTest=OCSorter.values();
	ICTest=ICSorter.values();
	Deferred=DeferredSorter.values();
}
double TrancheCashFlow::GetAmountOutstanding(int index)const{
	if(index<0 || index>=FlowDates.size()) return 0.0;
	double Result=0.0;
	if(OutstandingAmt>0){
		Result=OutstandingAmt;
		for (int i=0;i<=index;i++) Result-=Principal.at(i);
		return Result;
	}
	for (int i=index;i<FlowDates.size();i++) Result+=Principal.at(i);
	return Result;
}
void TrancheCashFlow::AddFlow(QDate Dte,double Amt, ThrancheFlowType FlwTpe){
	int DateIndex = FlowDates.indexOf(Dte);
	bool NeedSorting = false;
	if(DateIndex<0){
		FlowDates.append(Dte);
		Interest.append(0.0);
		Principal.append(0.0);
		OCTest.append(0.0);
		ICTest.append(0.0);
		Deferred.append(0.0);
		DateIndex=FlowDates.size()-1;
		NeedSorting=true;
	}
	switch(FlwTpe){
	case InterestFlow:
		Interest[DateIndex]+=Amt; break;
	case PrincipalFlow:
		Principal[DateIndex]+=Amt; break;
	case ICFlow:
		ICTest[DateIndex]+=Amt; break;
	case OCFlow:
		OCTest[DateIndex]+=Amt; break;
	case DeferredFlow:
		Deferred[DateIndex]+=Amt; break;
	}
	if(NeedSorting) SortByDate();
}
void TrancheCashFlow::AddFlow(const TrancheCashFlow& a){
	bool NeedSorting = false;
	int DateIndex;
	for(int i=0;i<a.Count();i++){
		DateIndex = FlowDates.indexOf(a.GetDate(i));
		if(DateIndex<0){
			FlowDates.append(a.GetDate(i));
			Interest.append(0.0);
			Principal.append(0.0);
			ICTest.append(0.0);
			OCTest.append(0.0);
			Deferred.append(0.0);
			DateIndex=FlowDates.size()-1;
			NeedSorting=true;
		}
		Interest[DateIndex]+=a.GetInterest(i);
		Principal[DateIndex]+=a.GetPrincipal(i);
		ICTest[DateIndex]+=a.GetICTest(i);
		OCTest[DateIndex]+=a.GetOCTest(i);
		Deferred[DateIndex]+=a.GetDeferred(i);
	}
	if(NeedSorting) SortByDate();
}
void TrancheCashFlow::ResetFlows(){
	FlowDates.clear();
	Interest.clear();
	Principal.clear();
	ICTest.clear();
	OCTest.clear();
	Deferred.clear();
}
TrancheCashFlow& TrancheCashFlow::operator=(const TrancheCashFlow& a){
	ResetFlows();
	AddFlow(a);
	OutstandingAmt=a.OutstandingAmt;
	return *this;
}
TrancheCashFlow::TrancheCashFlow(const TrancheCashFlow& a)
	:OutstandingAmt(a.OutstandingAmt)
{
	ResetFlows();
	AddFlow(a);
}
void TrancheCashFlow::ReplaceDate(const QDate& OriginalDate, const QDate& NewDate){
	int Index=FindDate(OriginalDate);
	if(Index>=0) FlowDates[Index]=NewDate;
}
double TrancheCashFlow::GetPreviousDeferred(const QDate& CurrentDate){
	if(Deferred.isEmpty()) return 0.0;
	int Index=FindDate(CurrentDate);
	if(Index>0) return Deferred.at(Index-1);
	return Deferred.last();
}
double TrancheCashFlow::GetPreviousOutstanding(const QDate& CurrentDate){
	int Index=FindDate(CurrentDate);
	if(Index>0) return GetAmountOutstanding(Index-1);
	return GetAmountOutstanding(FlowDates.size()-1);
}
QDataStream& operator<<(QDataStream & stream, const TrancheCashFlow& flows){
	stream
		<< flows.OutstandingAmt
		<< flows.FlowDates
		<< flows.Interest
		<< flows.Principal
		<< flows.OCTest
		<< flows.ICTest
		<< flows.Deferred
	;
	return stream;
}
QDataStream& operator>>(QDataStream & stream, TrancheCashFlow& flows){
	stream
		>> flows.OutstandingAmt
		>> flows.FlowDates
		>> flows.Interest
		>> flows.Principal
		>> flows.OCTest
		>> flows.ICTest
		>> flows.Deferred
		;
	return stream;
}