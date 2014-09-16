#ifndef MtgCashFlow_h__
#define MtgCashFlow_h__
#include <QList>
#include <QDate>
#include <QMetaType>
#include "GenericCashFlow.h"
#include "BloombergVector.h"
#include "IntegerVector.h"
class MtgCashFlow : public GenericCashFlow {
public:
#ifdef _DEBUG
	double GetCoupTimeOut(int index)const { return GetFlow(index, static_cast<qint32>(MtgFlowType::WACouponFlow)); }
	double GetCoupTimeOut(const QDate& index) const { return GetFlow(index, static_cast<qint32>(MtgFlowType::WACouponFlow)); }
#endif
	enum class MtgFlowType : qint32 {
		InterestFlow = 1 << MaximumInterestsTypes,
		PrincipalFlow = 1,
		PrepaymentFlow = 2,
		AmountOutstandingFlow = 3,
		WACouponFlow = 4,
		AccruedInterestFlow = 5,
		LossOnInterestFlow = 6,
		PrincipalDefault = 7,
		PrincipalRecovered = 8,
		InterestRecovered = 9,
		WAPrepayMult = 129,
		WALossMult = 130,
		LossFlow = 10
	};
	virtual MtgCashFlow ApplyScenario(BloombergVector CPRv, BloombergVector CDRv, BloombergVector LSv) const;
	virtual MtgCashFlow ApplyScenario(const QString& CPRv, const QString& CDRv, const QString& LSv) const { return ApplyScenario(BloombergVector(CPRv), BloombergVector(CDRv), BloombergVector(LSv)); }


	double GetInterest(int index) const { return GenericCashFlow::GetFlow(index, static_cast<qint32>(MtgFlowType::InterestFlow)); }
	double GetScheduled(int index) const { return GenericCashFlow::GetFlow(index, static_cast<qint32>(MtgFlowType::PrincipalFlow)); }
	double GetPrepay(int index) const { return GenericCashFlow::GetFlow(index, static_cast<qint32>(MtgFlowType::PrepaymentFlow)); }
	double GetPrincipal(int index) const { return GetScheduled(index) + GetPrepay(index); }
	double GetLoss(int index) const { return GenericCashFlow::GetFlow(index, static_cast<qint32>(MtgFlowType::LossFlow)); }
	double GetAmountOut(int index) const { return GenericCashFlow::GetFlow(index, static_cast<qint32>(MtgFlowType::AmountOutstandingFlow)); }
	double GetAccruedInterest(int index) const { return GenericCashFlow::GetFlow(index, static_cast<qint32>(MtgFlowType::AccruedInterestFlow)); }
	double GetLossOnInterest(int index) const { return GenericCashFlow::GetFlow(index, static_cast<qint32>(MtgFlowType::LossOnInterestFlow)); }
	double GetWAcoupon(int index) const;
	double GetWAprepayMult(int index) const;
	double GetWAlossMult(int index) const;
	double GetDefaults(int index) const { return GenericCashFlow::GetFlow(index, static_cast<qint32>(MtgFlowType::PrincipalDefault)); }
	double GetRecoveries(int index) const { return GenericCashFlow::GetFlow(index, static_cast<qint32>(MtgFlowType::PrincipalRecovered)); }
	double GetInterestRecoveries(int index) const { return GenericCashFlow::GetFlow(index, static_cast<qint32>(MtgFlowType::InterestRecovered)); }
	double GetInterest(const QDate& index) const { return GenericCashFlow::GetFlow(index, static_cast<qint32>(MtgFlowType::InterestFlow)); }
	double GetScheduled(const QDate& index) const { return GenericCashFlow::GetFlow(index, static_cast<qint32>(MtgFlowType::PrincipalFlow)); }
	double GetPrepay(const QDate& index) const { return GenericCashFlow::GetFlow(index, static_cast<qint32>(MtgFlowType::PrepaymentFlow)); }
	double GetPrincipal(const QDate& index) const { return GetScheduled(index) + GetPrepay(index); }
	double GetLoss(const QDate& index) const { return GenericCashFlow::GetFlow(index, static_cast<qint32>(MtgFlowType::LossFlow)); }
	double GetAmountOut(const QDate& index) const { return GenericCashFlow::GetFlow(index, static_cast<qint32>(MtgFlowType::AmountOutstandingFlow)); }
	double GetAccruedInterest(const QDate& index) const { return GenericCashFlow::GetFlow(index, static_cast<qint32>(MtgFlowType::AccruedInterestFlow)); }
	double GetLossOnInterest(const QDate& index) const { return GenericCashFlow::GetFlow(index, static_cast<qint32>(MtgFlowType::LossOnInterestFlow)); }
	double GetWAcoupon(const QDate& index) const;
	double GetWAprepayMult(const QDate& index) const;
	double GetWAlossMult(const QDate& index) const;
	double GetDefaults(const QDate& index) const { return GenericCashFlow::GetFlow(index, static_cast<qint32>(MtgFlowType::PrincipalDefault)); }
	double GetRecoveries(const QDate& index) const { return GenericCashFlow::GetFlow(index, static_cast<qint32>(MtgFlowType::PrincipalRecovered)); }
	double GetInterestRecoveries(const QDate& index) const { return GenericCashFlow::GetFlow(index, static_cast<qint32>(MtgFlowType::InterestRecovered)); }
	virtual double GetTotalFlow(int index)const override;
	virtual double GetTotalFlow(const QDate& a)const override;



	virtual void AddFlow(const QDate& Dte, double Amt, MtgFlowType FlowTpe) { GenericCashFlow::AddFlow(Dte, Amt, static_cast<qint32>(FlowTpe)); }
	virtual double GetFlow(const QDate& Dte, MtgFlowType FlowTpe)const {return GenericCashFlow::GetFlow(Dte, static_cast<qint32>(FlowTpe)); }
	virtual double GetPreviousFlow(const QDate& Dte, MtgFlowType FlowTpe)const { return GenericCashFlow::GetPreviousFlow(Dte, static_cast<qint32>(FlowTpe)); }
	virtual double GetFlow(int index, MtgFlowType FlowTpe) const { return GenericCashFlow::GetFlow(index, static_cast<qint32>(FlowTpe)); }
	virtual double GetPreviousFlow(int index, MtgFlowType FlowTpe) const { return GenericCashFlow::GetPreviousFlow(index, static_cast<qint32>(FlowTpe)); }
	using GenericCashFlow::AddFlow;
	using GenericCashFlow::GetFlow; 
	using GenericCashFlow::GetPreviousFlow;
	MtgCashFlow(const MtgCashFlow& a);
	MtgCashFlow();
	//friend QDataStream& operator<<(QDataStream & stream, const MtgCashFlow& flows);
	//friend QDataStream& operator>>(QDataStream & stream, MtgCashFlow& flows);
};
Q_DECLARE_METATYPE(MtgCashFlow)
//QDataStream& operator<<(QDataStream & stream, const MtgCashFlow& flows);
//QDataStream& operator>>(QDataStream & stream, MtgCashFlow& flows);
#endif // MtgCashFlow_h__