#ifndef MtgCashFlow_h__
#define MtgCashFlow_h__
#include <QList>
#include <QDate>
#include <QMetaType>
#include "BackwardCompatibilityInterface.h"
#include "BloombergVector.h"
#include "IntegerVector.h"
class MtgCashFlow : public BackwardInterface{
private:
	QList<QDate> m_FlowDates;
	QList<double> m_Interest;
	QList<double> m_Scheduled;
	QList<double> m_Prepay;
	QList<double> m_Loss;
	QList<double> m_AmountOut;
	QList<double> m_CoupTimesOut;
	QList<double> m_AccruedInterest;
	QList<double> m_LossOnInterest;
	QList<double> m_Defaulted;
	QList<double> m_PrincipalRecovered;
	QList<double> m_InterestRecovered;
	void SortByDate();
	double GetApplicablePrincipal(int index);
	double GetApplicablePrincipal(const QDate& index);
protected:
	virtual QDataStream& LoadOldVersion(QDataStream& stream);
public:
#ifdef _DEBUG
	bool operator==(const MtgCashFlow& a) const;
	double GetCoupTimeOut(int index)const;
	double GetCoupTimeOut(const QDate& index) const { return GetCoupTimeOut(FindDate(index)); }
#endif
	enum MtgFlowType{
		InterestFlow,
		PrincipalFlow,
		PrepaymentFlow,
		AmountOutstandingFlow,
		WACouponFlow,
		AccruedInterestFlow,
		LossOnInterestFlow,
		PrincipalDefault,
		PrincipalRecovered,
		InterestRecovered,
		LossFlow
	};
	MtgCashFlow ApplyScenario(BloombergVector CPRv, BloombergVector CDRv, BloombergVector LSv/*, BloombergVector Delinq = BloombergVector("0"), IntegerVector DelinqLag = IntegerVector("0")*/) const;
	MtgCashFlow ApplyScenario(const QString& CPRv, const QString& CDRv, const QString& LSv/*, const QString& Delinq = "0", const QString& DelinqLag = "0"*/) const { return ApplyScenario(BloombergVector(CPRv), BloombergVector(CDRv), BloombergVector(LSv)/*, BloombergVector(Delinq), IntegerVector(DelinqLag)*/); }
	int GetPaymentFrequency() const;
	QDate GetDate(int index) const;
	double GetInterest(int index) const;
	double GetScheduled(int index) const;
	double GetPrepay(int index) const;
	double GetPrincipal(int index) const{return GetScheduled(index)+GetPrepay(index);}
	double GetLoss(int index) const;
	double GetAmountOut(int index) const;
	double GetAccruedInterest(int index) const;
	double GetLossOnInterest(int index) const;
	double GetWAcoupon(int index) const;
	double GetDefaults(int index) const;
	double GetRecoveries(int index) const;
	double GetInterestRecoveries(int index) const;
	double GetInterest(const QDate& index) const{return GetInterest(FindDate(index));}
	double GetScheduled(const QDate& index) const{return GetScheduled(FindDate(index));}
	double GetPrepay(const QDate& index) const{return GetPrepay(FindDate(index));}
	double GetPrincipal(const QDate& index) const{return GetPrincipal(FindDate(index));}
	double GetLoss(const QDate& index) const{return GetLoss(FindDate(index));}
	double GetAmountOut(const QDate& index) const{return GetAmountOut(FindDate(index));}
	double GetAccruedInterest(const QDate& index) const{return GetAccruedInterest(FindDate(index));}
	double GetLossOnInterest(const QDate& index) const{return GetLossOnInterest(FindDate(index));}
	double GetWAcoupon(const QDate& index) const;
	double GetDefaults(const QDate& index) const { return GetDefaults(FindDate(index)); }
	double GetRecoveries(const QDate& index) const { return GetRecoveries(FindDate(index)); }
	double GetInterestRecoveries(const QDate& index) const { return GetInterestRecoveries(FindDate(index)); }
	QDate MaturityDate() const { if (m_FlowDates.isEmpty()) return QDate(); return m_FlowDates.last(); }
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