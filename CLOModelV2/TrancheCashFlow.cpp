#include "TrancheCashFlow.h"
#include <QMap>
#include <QDataStream>
TrancheCashFlow::TrancheCashFlow(double ThrancheOutstanding)
	:OutstandingAmt(ThrancheOutstanding)
	, StartingDeferredInterest(0.0)
{
	m_AggregationLevel = Monthly;
	SetStock(static_cast<qint32>(TrancheFlowType::OCFlow));
	SetStock(static_cast<qint32>(TrancheFlowType::ICFlow));
	SetStock(static_cast<qint32>(TrancheFlowType::DeferredFlow));
}

double TrancheCashFlow::GetAmountOutstanding(int index)const{
	if(index<0 || index>=Count()) return 0.0;
	double Result=0.0;
	if(OutstandingAmt>0){
		Result=OutstandingAmt;
		for (int i = 0; i <= index; i++) Result -= GetFlow(i, static_cast<qint32>(TrancheFlowType::PrincipalFlow));
		if (qAbs(Result)<0.01) return 0.0;
		return Result;
	}
	for (int i = index; i<Count(); i++) Result += GetFlow(i, static_cast<qint32>(TrancheFlowType::PrincipalFlow));
	return Result;
}
TrancheCashFlow::TrancheCashFlow(const TrancheCashFlow& a)
	:OutstandingAmt(a.OutstandingAmt)
	,StartingDeferredInterest(a.StartingDeferredInterest)
	,GenericCashFlow(a)
{}

double TrancheCashFlow::GetPreviousDeferred(const QDate& CurrentDate){
	if(m_CashFlows.isEmpty()) return StartingDeferredInterest;
	return GetPreviousFlow(CurrentDate, static_cast<qint32>(TrancheFlowType::DeferredFlow));
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

QDate TrancheCashFlow::GetLastFlowDate(bool IncludeDeferred ) const {
	for (QMap<QDate, QHash<qint32, double>* >::const_iterator i = m_CashFlows.constEnd() - 1; true; --i) {
		for (QHash<qint32, double>::const_iterator j = i.value()->constBegin(); j != i.value()->constEnd(); ++j) {
			if ((
				(j.key() & static_cast<qint32>(TrancheFlowType::InterestFlow)) 
				|| (j.key() == static_cast<qint32>(TrancheFlowType::PrincipalFlow))
				|| ((j.key() & static_cast<qint32>(TrancheFlowType::DeferredFlow)) && IncludeDeferred)
			) && j.value() >= 0.01) return i.key();

		}
		if (i == m_CashFlows.constBegin()) break;
	}
	return QDate();
}

int TrancheCashFlow::GetLastFlowIndex(bool IncludeDeferred) const {
	QDate Target = GetLastFlowDate(IncludeDeferred);
	if (!Target.isValid()) return -1;
	int Counter = 0;
	for (QMap<QDate, QHash<qint32, double>* >::const_iterator i = m_CashFlows.constBegin(); i != m_CashFlows.constEnd(); ++i) {
		if (i.key() == Target) return Counter;
		++Counter;
	}
}

TrancheCashFlow& TrancheCashFlow::operator=(const TrancheCashFlow& a) {
	OutstandingAmt = a.OutstandingAmt; 
	StartingDeferredInterest = a.StartingDeferredInterest; 
	GenericCashFlow::operator=(a);
	return *this;
}

double TrancheCashFlow::GetTotalFlow(int index) const {
	double Result = GetPrincipal(index);
	for (qint32 i = static_cast<qint32>(TrancheFlowType::InterestFlow); i < (static_cast<qint32>(TrancheFlowType::InterestFlow) << 1); ++i)
		Result += GetFlow(index, i);
	return Result;
}
