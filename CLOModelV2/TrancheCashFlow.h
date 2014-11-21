#ifndef TrancheCashFlow_h__
#define TrancheCashFlow_h__
#include "GenericCashFlow.h"
#include <QList>
#include <QDate>
#define MaximumInterestsTypes 8
class TrancheCashFlow : public GenericCashFlow {
private:
	double OutstandingAmt;
	double StartingDeferredInterest;
protected:
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
	
public:
	enum class TrancheFlowType : qint32{
		InterestFlow = 1 << MaximumInterestsTypes,
		PrincipalFlow=1,
		OCFlow=11,
		ICFlow=12,
		OCTarget=211,
		ICTarget=212,
		DeferredFlow = 1 << (MaximumInterestsTypes+1),
		PDLOutstanding=201,
		PDLCured=202
	};
	TrancheCashFlow(double ThrancheOutstanding=0.0);
	TrancheCashFlow(const TrancheCashFlow& a);
	double GetInterest(int index)const; /*{ return GetFlow(index, static_cast<qint32>(TrancheFlowType::InterestFlow)); }*/
	double GetPrincipal(int index)const { return GetFlow(index, static_cast<qint32>(TrancheFlowType::PrincipalFlow)); }
	double GetOCTest(int index)const { return GetFlow(index, static_cast<qint32>(TrancheFlowType::OCFlow)); }
	double GetICTest(int index)const { return GetFlow(index, static_cast<qint32>(TrancheFlowType::ICFlow)); }
	double GetPDLOutstanding(int index)const { return GetFlow(index, static_cast<qint32>(TrancheFlowType::PDLOutstanding)); }
	double GetPDLCured(int index)const { return GetFlow(index, static_cast<qint32>(TrancheFlowType::PDLCured)); }
	double GetDeferred(int index)const; /*{ return GetFlow(index, static_cast<qint32>(TrancheFlowType::DeferredFlow)); }*/
	virtual double GetTotalFlow(int index)const override;
	virtual double GetTotalFlow(const QDate& a)const override;
	double GetAmountOutstanding(int index = 0)const;
	double GetInterest(const QDate& a)const; /*{ return GetFlow(a, static_cast<qint32>(TrancheFlowType::InterestFlow)); }*/
	double GetPrincipal(const QDate& a)const { return GetFlow(a, static_cast<qint32>(TrancheFlowType::PrincipalFlow)); }
	double GetOCTest(const QDate& a)const { return GetFlow(a, static_cast<qint32>(TrancheFlowType::OCFlow)); }
	double GetICTest(const QDate& a)const { return GetFlow(a, static_cast<qint32>(TrancheFlowType::ICFlow)); }
	double GetPDLOutstanding(const QDate& a)const { return GetFlow(a, static_cast<qint32>(TrancheFlowType::PDLOutstanding)); }
	double GetPDLCured(const QDate& a)const { return GetFlow(a, static_cast<qint32>(TrancheFlowType::PDLCured)); }
	double GetDeferred(const QDate& a)const; /*{ return GetFlow(a, static_cast<qint32>(TrancheFlowType::DeferredFlow)); }*/
	double GetAmountOutstanding(const QDate& a)const{return GetAmountOutstanding(FindDate(a));} //TODO
	double GetInitialOutstanding()const{return OutstandingAmt;}
	void SetInitialOutstanding(double a){OutstandingAmt=a;}
	virtual void AddFlow(const QDate& Dte, double Amt, TrancheFlowType FlwTpe) { GenericCashFlow::AddFlow(Dte, Amt, static_cast<qint32>(FlwTpe)); }
	using GenericCashFlow::AddFlow;
	virtual TrancheCashFlow& operator=(const TrancheCashFlow& a);
	double GetPreviousDeferred(const QDate& CurrentDate);
	double GetPreviousOutstanding(const QDate& CurrentDate);
	QDate GetLastFlowDate(bool IncludeDeferred = false) const;
	int GetLastFlowIndex(bool IncludeDeferred = false) const;
	const double& GetStartingDeferredInterest() const { return StartingDeferredInterest; }
	void SetStartingDeferredInterest(const double& val) { StartingDeferredInterest = val; }
#ifndef NO_DATABASE
	virtual bool GetCashFlowsDatabase(const QString& TrancheID);
#endif
	virtual TrancheCashFlow ScaledCashFlows(double NewSize, double OldSize)const;
	virtual TrancheCashFlow ScaledCashFlows(double NewSize) const { return ScaledCashFlows(NewSize, OutstandingAmt); }
	friend QDataStream& operator<<(QDataStream & stream, const TrancheCashFlow& flows);
	friend QDataStream& operator>>(QDataStream & stream, TrancheCashFlow& flows);
};
QDataStream& operator<<(QDataStream & stream, const TrancheCashFlow& flows);
QDataStream& operator>>(QDataStream & stream, TrancheCashFlow& flows);
#endif // TrancheCashFlow_h__