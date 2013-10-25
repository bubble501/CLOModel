#ifndef WatFalPrior_h__
#define WatFalPrior_h__
#include <QDataStream>
class WatFalPrior{
public:
	enum WaterfallStepType{
		wst_SeniorExpenses,
		wst_SeniorFees,
		wst_Interest,
		wst_Principal,
		wst_OCTest,
		wst_OCTestPrinc,
		wst_ICTest,
		wst_ICTestPrinc,
		wst_DeferredInterest,
		wst_DeferredPrinc,
		wst_juniorFees,
		wst_ReinvestmentTest,
		wst_Excess,
		wst_ReinvestPrincipal,
	};
	int GetGroupTarget()const{return GroupTarget;}
	int GetRedemptionGroup()const{return RedemptionGroup;}
	double GetRedemptionShare() const{return RedemptionShare;}
	WaterfallStepType GetPriorityType() const{return PriorityType;}
	void SetGroupTarget(int a){if(a>0) GroupTarget=a;}
	void SetRedemptionGroup(int a){if(a>0) RedemptionGroup=a;}
	void SetRedemptionShare(double a){if(a>=0.0 && a<=1.0) RedemptionShare=a;}
	void SetPriorityType(WaterfallStepType a){PriorityType=a;}
	WatFalPrior();
	WatFalPrior(const WatFalPrior& a);
	WatFalPrior& operator=(const WatFalPrior& a);
private:
	int GroupTarget;
	int RedemptionGroup;
	double RedemptionShare;
	WaterfallStepType PriorityType;
	friend QDataStream& operator<<(QDataStream & stream, const WatFalPrior& flows);
	friend QDataStream& operator>>(QDataStream & stream, WatFalPrior& flows);
};
QDataStream& operator<<(QDataStream & stream, const WatFalPrior& flows);
QDataStream& operator>>(QDataStream & stream, WatFalPrior& flows);
#endif // WatFalPrior_h__