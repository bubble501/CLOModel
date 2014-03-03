#ifndef TrancheCashFlow_h__
#define TrancheCashFlow_h__
#include <QDate>
class TrancheCashFlow{
private:
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
	int Count() const;
	QDate GetDate(int index) const;
	int FindDate(const QDate& a)const;
	double GetInterest(int index)const;
	double GetPrincipal(int index)const;
	double GetOCTest(int index)const;
	double GetICTest(int index)const;
	double GetDeferred(int index)const;
	double GetTotalFlow(int index)const;
	double GetAmountOutstanding(int index = 0)const;
	double GetInterest(const QDate& a)const;
	double GetPrincipal(const QDate& a)const;
	double GetOCTest(const QDate& a)const;
	double GetICTest(const QDate& a)const;
	double GetDeferred(const QDate& a)const;
	double GetTotalFlow(const QDate& a)const;
	double GetAmountOutstanding(const QDate& a)const;
	void SetInitialOutstanding(double a);
	void AddFlow(QDate Dte,double Amt, ThrancheFlowType FlwTpe);
	void AddFlow(const TrancheCashFlow& a);
	TrancheCashFlow& operator+=(const TrancheCashFlow& a);
	void ResetFlows();
	TrancheCashFlow& operator=(const TrancheCashFlow& a);
	void ReplaceDate(const QDate& OriginalDate, const QDate& NewDate);
	double GetPreviousDeferred(const QDate& CurrentDate);
	double GetPreviousOutstanding(const QDate& CurrentDate);
};
#endif // TrancheCashFlow_h__