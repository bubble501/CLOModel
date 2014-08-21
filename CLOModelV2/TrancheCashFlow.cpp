#include "TrancheCashFlow.h"
#include <QMap>
#include <QDataStream>
TrancheCashFlow::TrancheCashFlow(double ThrancheOutstanding)
	:OutstandingAmt(ThrancheOutstanding)
	, StartingDeferredInterest(0.0)
{
	m_AggregationLevel = Monthly;
	SetStock(OCFlow);
	SetStock(ICFlow);
	SetStock(DeferredFlow);
}

double TrancheCashFlow::GetAmountOutstanding(int index)const{
	if(index<0 || index>=Count()) return 0.0;
	double Result=0.0;
	if(OutstandingAmt>0){
		Result=OutstandingAmt;
		for (int i=0;i<=index;i++) Result-=GetFlow(i,PrincipalFlow);
		if (qAbs(Result)<0.01) return 0.0;
		return Result;
	}
	for (int i = index; i<Count(); i++) Result += GetFlow(i, PrincipalFlow);
	return Result;
}
TrancheCashFlow::TrancheCashFlow(const TrancheCashFlow& a)
	:OutstandingAmt(a.OutstandingAmt)
	,StartingDeferredInterest(a.StartingDeferredInterest)
	,GenericCashFlow(a)
{}

double TrancheCashFlow::GetPreviousDeferred(const QDate& CurrentDate){
	if(m_CashFlows.isEmpty()) return StartingDeferredInterest;
	return GetPreviousFlow(CurrentDate, DeferredFlow);
}
double TrancheCashFlow::GetPreviousOutstanding(const QDate& CurrentDate){
	int Index=FindDate(CurrentDate);
	if(Index>0) return GetAmountOutstanding(Index-1);
	return GetAmountOutstanding(Count()-1);
}
QDataStream& operator<<(QDataStream & stream, const TrancheCashFlow& flows){
	stream
		<< flows.OutstandingAmt
		<< flows.StartingDeferredInterest
		<< static_cast<const GenericCashFlow&>(flows)
	;
	return stream;
}
QDataStream& operator>>(QDataStream & stream, TrancheCashFlow& flows) { return flows.LoadOldVersion(stream); }
QDataStream& TrancheCashFlow::LoadOldVersion(QDataStream& stream) {
	stream
		>> OutstandingAmt
		>> StartingDeferredInterest
		;
	return GenericCashFlow::LoadOldVersion(stream);
}
int TrancheCashFlow::GetLastFlowIndex(bool IncludeDeferred) const {
	for (int i = Count() - 1; i >= 0; i--) {
		if (
			GetFlow(i,InterestFlow) >= 0.01
			|| GetFlow(i,PrincipalFlow) >= 0.01
			|| (IncludeDeferred && GetFlow(i, DeferredFlow) > 0.01)
			) return i;
	}
	return -1;
}

TrancheCashFlow& TrancheCashFlow::operator=(const TrancheCashFlow& a) {
	OutstandingAmt = a.OutstandingAmt; 
	StartingDeferredInterest = a.StartingDeferredInterest; 
	GenericCashFlow::operator=(a);
	return *this;
}
