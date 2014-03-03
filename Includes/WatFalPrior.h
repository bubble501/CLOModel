#ifndef WatFalPrior_h__
#define WatFalPrior_h__
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
	int GetGroupTarget()const;
	int GetRedemptionGroup()const;
	double GetRedemptionShare() const;
	WaterfallStepType GetPriorityType() const;
	void SetGroupTarget(int a);
	void SetRedemptionGroup(int a);
	void SetRedemptionShare(double a);
	void SetPriorityType(WaterfallStepType a);
	WatFalPrior();
	WatFalPrior(const WatFalPrior& a);
	WatFalPrior& operator=(const WatFalPrior& a);
};
#endif // WatFalPrior_h__