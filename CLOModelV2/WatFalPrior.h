#ifndef WatFalPrior_h__
#define WatFalPrior_h__
#include <QDataStream>
#include "BackwardCompatibilityInterface.h"
class WatFalPrior : public BackwardInterface {
public:
	//! Enum defining the different types of waterfall steps
	enum WaterfallStepType{
		wst_SeniorExpenses, /*!< Senior Expenses*/
		wst_SeniorFees, /*!< Senior Fees*/
		wst_Interest, /*!< Interest Payment*/
		wst_Principal,  /*!< Notes Redemption*/
		wst_OCTest,  /*!< Over Collateralization test. Considered part of the interest waterfall*/
		wst_OCTestPrinc, /*!< Over Collateralization test. Considered part of the principal waterfall*/
		wst_ICTest, /*!< Interest Coverage test. Considered part of the interest waterfall*/
		wst_ICTestPrinc, /*!< Interest Coverage test. Considered part of the principal waterfall*/
		wst_DeferredInterest, /*!< Payment of deferred interest. Considered part of the interest waterfall*/
		wst_DeferredPrinc, /*!< Payment of deferred interest. Considered part of the principal waterfall*/
		wst_juniorFees, /*!< Junior Fees*/
		wst_ReinvestmentTest, /*!< Reinvestment Test*/
		wst_Excess, /*!< Excess Spread*/
		wst_ReinvestPrincipal, /*!< Reinvest all available princiapl during the reinvestment period*/
		wst_ReserveReplenish /*!< Replenish the reserve fund*/
	};
	int GetGroupTarget()const{return GroupTarget;}
	int GetRedemptionGroup()const{return RedemptionGroup;}
	double GetRedemptionShare() const{return RedemptionShare;}
	WaterfallStepType GetPriorityType() const{return PriorityType;}
	void SetGroupTarget(int a){if(a>0) GroupTarget=a;}
	void SetRedemptionGroup(int a){if(a>=0) RedemptionGroup=a;}
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
protected:
	virtual QDataStream& LoadOldVersion(QDataStream& stream);
	friend QDataStream& operator<<(QDataStream & stream, const WatFalPrior& flows);
	friend QDataStream& operator>>(QDataStream & stream, WatFalPrior& flows);
};
QDataStream& operator<<(QDataStream & stream, const WatFalPrior& flows);
QDataStream& operator>>(QDataStream & stream, WatFalPrior& flows);
#endif // WatFalPrior_h__
