#ifndef TrancheCashFlow_h__
#define TrancheCashFlow_h__

#include <QList>
#include <QDate>
class TrancheCashFlow{
private:
	double OutstandingAmt;
	QList<QDate> FlowDates;
	QList<double> Interest;
	QList<double> Principal;
	QList<double> OCTest;
	QList<double> ICTest;
	QList<double> Deferred;
	void SortByDate();
public:
	enum ThrancheFlowType{
		InterestFlow,
		PrincipalFlow,
		OCFlow,
		ICFlow,
		DeferredFlow
	};
	TrancheCashFlow(double ThrancheOutstanding=0.0);
	TrancheCashFlow(const TrancheCashFlow& a);
	int Count() const {return FlowDates.count();}
	QDate GetDate(int index) const;
	int FindDate(const QDate& a)const;
	double GetInterest(int index)const;
	double GetPrincipal(int index)const;
	double GetOCTest(int index)const;
	double GetICTest(int index)const;
	double GetDeferred(int index)const;
	double GetTotalFlow(int index)const{return GetInterest(index)+GetPrincipal(index);}
	double GetAmountOutstanding(int index = 0)const;
	double GetInterest(const QDate& a)const{return GetInterest(FindDate(a));}
	double GetPrincipal(const QDate& a)const{return GetPrincipal(FindDate(a));}
	double GetOCTest(const QDate& a)const{return GetOCTest(FindDate(a));}
	double GetICTest(const QDate& a)const{return GetICTest(FindDate(a));}
	double GetDeferred(const QDate& a)const{return GetDeferred(FindDate(a));}
	double GetTotalFlow(const QDate& a)const{return GetInterest(a)+GetPrincipal(a);}
	double GetAmountOutstanding(const QDate& a)const{return GetAmountOutstanding(FindDate(a));}
	double GetInitialOutstanding()const{return OutstandingAmt;}
	void SetInitialOutstanding(double a){OutstandingAmt=a;}
	void AddFlow(const QDate& Dte,double Amt, ThrancheFlowType FlwTpe);
	void AddFlow(const TrancheCashFlow& a);
	TrancheCashFlow& operator+=(const TrancheCashFlow& a){AddFlow(a); return *this;}
	void ResetFlows();
	TrancheCashFlow& operator=(const TrancheCashFlow& a);
	void ReplaceDate(const QDate& OriginalDate, const QDate& NewDate);
	double GetPreviousDeferred(const QDate& CurrentDate);
	double GetPreviousOutstanding(const QDate& CurrentDate);
	friend QDataStream& operator<<(QDataStream & stream, const TrancheCashFlow& flows);
	friend QDataStream& operator>>(QDataStream & stream, TrancheCashFlow& flows);
};
QDataStream& operator<<(QDataStream & stream, const TrancheCashFlow& flows);
QDataStream& operator>>(QDataStream & stream, TrancheCashFlow& flows);
#endif // TrancheCashFlow_h__