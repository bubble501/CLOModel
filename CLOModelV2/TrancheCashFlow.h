#ifndef TrancheCashFlow_h__
#define TrancheCashFlow_h__
#include "GenericCashFlow.h"
#include <QList>
#include <QDate>
class TrancheCashFlow : public GenericCashFlow {
private:
	double OutstandingAmt;
	double StartingDeferredInterest;
protected:
	virtual QDataStream& LoadOldVersion(QDataStream& stream);
public:
	enum TrancheFlowType{
		InterestFlow=0,
		PrincipalFlow=1,
		OCFlow=11,
		ICFlow=12,
		DeferredFlow=13
	};
	TrancheCashFlow(double ThrancheOutstanding=0.0);
	TrancheCashFlow(const TrancheCashFlow& a);
	double GetInterest(int index)const { return GetFlow(index, InterestFlow); }
	double GetPrincipal(int index)const { return GetFlow(index, PrincipalFlow); }
	double GetOCTest(int index)const { return GetFlow(index, OCFlow); }
	double GetICTest(int index)const { return GetFlow(index, ICFlow); }
	double GetDeferred(int index)const { return GetFlow(index, DeferredFlow); }
	double GetTotalFlow(int index)const{return GetInterest(index)+GetPrincipal(index);}
	double GetAmountOutstanding(int index = 0)const;
	double GetInterest(const QDate& a)const { return GetFlow(a, InterestFlow); }
	double GetPrincipal(const QDate& a)const { return GetFlow(a, PrincipalFlow); }
	double GetOCTest(const QDate& a)const { return GetFlow(a, OCFlow); }
	double GetICTest(const QDate& a)const { return GetFlow(a, ICFlow); }
	double GetDeferred(const QDate& a)const { return GetFlow(a, DeferredFlow); }
	double GetTotalFlow(const QDate& a)const{return GetInterest(a)+GetPrincipal(a);}
	double GetAmountOutstanding(const QDate& a)const{return GetAmountOutstanding(FindDate(a));} //TODO
	double GetInitialOutstanding()const{return OutstandingAmt;}
	void SetInitialOutstanding(double a){OutstandingAmt=a;}
	virtual void AddFlow(const QDate& Dte, double Amt, TrancheFlowType FlwTpe) { GenericCashFlow::AddFlow(Dte, Amt, static_cast<qint32>(FlwTpe)); }
	using GenericCashFlow::AddFlow;
	virtual TrancheCashFlow& operator=(const TrancheCashFlow& a);
	double GetPreviousDeferred(const QDate& CurrentDate);
	double GetPreviousOutstanding(const QDate& CurrentDate);
	QDate GetLastFlowDate(bool IncludeDeferred = false) const { return GetDate(GetLastFlowIndex(IncludeDeferred)); }
	int GetLastFlowIndex(bool IncludeDeferred = false) const;
	const double& GetStartingDeferredInterest() const { return StartingDeferredInterest; }
	void SetStartingDeferredInterest(const double& val) { StartingDeferredInterest = val; }
	friend QDataStream& operator<<(QDataStream & stream, const TrancheCashFlow& flows);
	friend QDataStream& operator>>(QDataStream & stream, TrancheCashFlow& flows);
};
QDataStream& operator<<(QDataStream & stream, const TrancheCashFlow& flows);
QDataStream& operator>>(QDataStream & stream, TrancheCashFlow& flows);
#endif // TrancheCashFlow_h__