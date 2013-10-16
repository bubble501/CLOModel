#ifndef MtgCashFlow_h__
#define MtgCashFlow_h__
#include <QList>
#include <QDate>
#include <QMetaType>
class MtgCashFlow{
private:
	QList<QDate> m_FlowDates;
	QList<double> m_Interest;
	QList<double> m_Scheduled;
	QList<double> m_Prepay;
	QList<double> m_Loss;
	QList<double> m_AmountOut;
	QList<double> m_AccruedInterest;
	QList<double> m_LossOnInterest;
	void SortByDate();
public:
	enum MtgFlowType{
		InterestFlow,
		PrincipalFlow,
		PrepaymentFlow,
		AmountOutstandingFlow,
		AccruedInterestFlow,
		LossOnInterestFlow,
		LossFlow
	};
	int GetPaymentFrequency() const;
	QDate GetDate(const int index) const;
	double GetInterest(const int index) const;
	double GetScheduled(const int index) const;
	double GetPrepay(const int index) const;
	double GetPrincipal(const int index) const{return GetScheduled(index)+GetPrepay(index);}
	double GetLoss(const int index) const;
	double GetAmountOut(const int index) const;
	double GetAccruedInterest(const int index) const;
	double GetLossOnInterest(const int index) const;
	double GetInterest(const QDate& index) const{return GetInterest(FindDate(index));}
	double GetScheduled(const QDate& index) const{return GetScheduled(FindDate(index));}
	double GetPrepay(const QDate& index) const{return GetPrepay(FindDate(index));}
	double GetPrincipal(const QDate& index) const{return GetPrincipal(FindDate(index));}
	double GetLoss(const QDate& index) const{return GetLoss(FindDate(index));}
	double GetAmountOut(const QDate& index) const{return GetAmountOut(FindDate(index));}
	double GetAccruedInterest(const QDate& index) const{return GetAccruedInterest(FindDate(index));}
	double GetLossOnInterest(const QDate& index) const{return GetLossOnInterest(FindDate(index));}
	int Count() const;
	int FindDate(const QDate& a) const;
	void AddFlow(const QDate& Dte, double Amt, MtgFlowType FlowTpe);
	void AddFlow(const MtgCashFlow& a);
	void RemoveAllFlows();
	const MtgCashFlow& operator=(const MtgCashFlow& a);
	const MtgCashFlow& operator+=(const MtgCashFlow& a){AddFlow(a); return *this;}
	MtgCashFlow(const MtgCashFlow& a){AddFlow(a);}
	MtgCashFlow(){}
	friend QDataStream& operator<<(QDataStream & stream, const MtgCashFlow& flows);
	friend QDataStream& operator>>(QDataStream & stream, MtgCashFlow& flows);
};
Q_DECLARE_METATYPE(MtgCashFlow)
QDataStream& operator<<(QDataStream & stream, const MtgCashFlow& flows);
QDataStream& operator>>(QDataStream & stream, MtgCashFlow& flows);
#endif // MtgCashFlow_h__