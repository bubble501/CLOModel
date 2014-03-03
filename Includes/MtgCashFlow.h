#ifndef MtgCashFlow_h__
#define MtgCashFlow_h__
#include <QDate>
class  MtgCashFlow{
public:
	enum MtgFlowType{
		InterestFlow,
		PrincipalFlow,
		PrepaymentFlow,
		AmountOutstandingFlow,
		WACouponFlow,
		AccruedInterestFlow,
		LossOnInterestFlow,
		LossFlow
	};
	int GetPaymentFrequency() const;
	QDate GetDate(int index) const;
	double GetInterest(int index) const;
	double GetScheduled(int index) const;
	double GetPrepay(int index) const;
	double GetPrincipal(int index) const;
	double GetLoss(int index) const;
	double GetAmountOut(int index) const;
	double GetAccruedInterest(int index) const;
	double GetLossOnInterest(int index) const;
	double GetWAcoupon(int index) const;
	double GetInterest(const QDate& index) const;
	double GetScheduled(const QDate& index) const;
	double GetPrepay(const QDate& index) const;
	double GetPrincipal(const QDate& index) const;
	double GetLoss(const QDate& index) const;
	double GetAmountOut(const QDate& index) const;
	double GetAccruedInterest(const QDate& index) const;
	double GetLossOnInterest(const QDate& index) const;
	double GetWAcoupon(const QDate& index) const;
	int Count() const;
	int FindDate(const QDate& a) const;
	void AddFlow(const QDate& Dte, double Amt, MtgFlowType FlowTpe);
	void AddFlow(const MtgCashFlow& a);
	void RemoveAllFlows();
	const MtgCashFlow& operator=(const MtgCashFlow& a);
	const MtgCashFlow& operator+=(const MtgCashFlow& a);
	MtgCashFlow(const MtgCashFlow& a);
	MtgCashFlow();
};
#endif // MtgCashFlow_h__