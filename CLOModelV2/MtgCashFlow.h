#ifndef MtgCashFlow_h__
#define MtgCashFlow_h__
#include <QList>
#include <QDate>
#include <QMetaType>
#include "GenericCashFlow.h"
#include "BloombergVector.h"
#include "IntegerVector.h"
class MtgCashFlow : public GenericCashFlow{
public:
#ifdef _DEBUG
	double GetCoupTimeOut(int index)const { return GetFlow(index, WACouponFlow); }
	double GetCoupTimeOut(const QDate& index) const { return GetFlow(index, WACouponFlow); }
#endif
	enum MtgFlowType{
		InterestFlow=0,
		PrincipalFlow=1,
		PrepaymentFlow=2,
		AmountOutstandingFlow=3,
		WACouponFlow=4,
		AccruedInterestFlow=5,
		LossOnInterestFlow=6,
		PrincipalDefault=7,
		PrincipalRecovered=8,
		InterestRecovered=9,
		WAPrepayMult=129,
		WALossMult = 130,
		LossFlow=10 //Keep this last
	};
	virtual MtgCashFlow ApplyScenario(BloombergVector CPRv, BloombergVector CDRv, BloombergVector LSv) const;
	virtual MtgCashFlow ApplyScenario(const QString& CPRv, const QString& CDRv, const QString& LSv) const { return ApplyScenario(BloombergVector(CPRv), BloombergVector(CDRv), BloombergVector(LSv)); }


	double GetInterest(int index) const { return GenericCashFlow::GetFlow(index, InterestFlow); }
	double GetScheduled(int index) const { return GenericCashFlow::GetFlow(index, PrincipalFlow); }
	double GetPrepay(int index) const { return GenericCashFlow::GetFlow(index, PrepaymentFlow); }
	double GetPrincipal(int index) const{return GetScheduled(index)+GetPrepay(index);}
	double GetLoss(int index) const { return GenericCashFlow::GetFlow(index, LossFlow); }
	double GetAmountOut(int index) const { return GenericCashFlow::GetFlow(index, AmountOutstandingFlow); }
	double GetAccruedInterest(int index) const { return GenericCashFlow::GetFlow(index, AccruedInterestFlow); }
	double GetLossOnInterest(int index) const { return GenericCashFlow::GetFlow(index, LossOnInterestFlow); }
	double GetWAcoupon(int index) const;
	double GetWAprepayMult(int index) const;
	double GetWAlossMult(int index) const;
	double GetDefaults(int index) const { return GenericCashFlow::GetFlow(index, PrincipalDefault); }
	double GetRecoveries(int index) const { return GenericCashFlow::GetFlow(index, PrincipalRecovered); }
	double GetInterestRecoveries(int index) const { return GenericCashFlow::GetFlow(index, InterestRecovered); }
	double GetInterest(const QDate& index) const { return GenericCashFlow::GetFlow(index, InterestFlow); }
	double GetScheduled(const QDate& index) const { return GenericCashFlow::GetFlow(index, PrincipalFlow); }
	double GetPrepay(const QDate& index) const { return GenericCashFlow::GetFlow(index, PrepaymentFlow); }
	double GetPrincipal(const QDate& index) const { return GetScheduled(index) + GetPrepay(index); }
	double GetLoss(const QDate& index) const { return GenericCashFlow::GetFlow(index, LossFlow); }
	double GetAmountOut(const QDate& index) const { return GenericCashFlow::GetFlow(index, AmountOutstandingFlow); }
	double GetAccruedInterest(const QDate& index) const { return GenericCashFlow::GetFlow(index, AccruedInterestFlow); }
	double GetLossOnInterest(const QDate& index) const { return GenericCashFlow::GetFlow(index, LossOnInterestFlow); }
	double GetWAcoupon(const QDate& index) const;
	double GetWAprepayMult(const QDate& index) const;
	double GetWAlossMult(const QDate& index) const;
	double GetDefaults(const QDate& index) const { return GenericCashFlow::GetFlow(index, PrincipalDefault); }
	double GetRecoveries(const QDate& index) const { return GenericCashFlow::GetFlow(index, PrincipalRecovered); }
	double GetInterestRecoveries(const QDate& index) const { return GenericCashFlow::GetFlow(index, InterestRecovered); }
	



	virtual void AddFlow(const QDate& Dte, double Amt, MtgFlowType FlowTpe) { GenericCashFlow::AddFlow(Dte, Amt, static_cast<qint32>(FlowTpe)); }
	using GenericCashFlow::AddFlow;
	MtgCashFlow(const MtgCashFlow& a);
	MtgCashFlow();
	//friend QDataStream& operator<<(QDataStream & stream, const MtgCashFlow& flows);
	//friend QDataStream& operator>>(QDataStream & stream, MtgCashFlow& flows);
};
Q_DECLARE_METATYPE(MtgCashFlow)
//QDataStream& operator<<(QDataStream & stream, const MtgCashFlow& flows);
//QDataStream& operator>>(QDataStream & stream, MtgCashFlow& flows);
#endif // MtgCashFlow_h__