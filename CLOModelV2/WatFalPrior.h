#ifndef WatFalPrior_h__
#define WatFalPrior_h__
#include <QDataStream>
#include <QVariant>
#include <QHash>
#include "BackwardCompatibilityInterface.h"
class WatFalPrior : public BackwardInterface {
public:
	//! Enum defining the different types of waterfall steps
	enum class WaterfallStepType: qint16{
		wst_SeniorExpenses = 0, /*!< Senior Expenses*/
		wst_SeniorFees = 1, /*!< Senior Fees*/
		wst_Interest = 2, /*!< Interest Payment*/
		wst_Principal = 3,  /*!< Notes Redemption*/
		wst_OCTest = 4,  /*!< Over Collateralization test. Considered part of the interest waterfall*/
		//wst_OCTestPrinc = 5, /*!< Over Collateralization test. Considered part of the principal waterfall*/ Obsolete
		wst_ICTest = 6, /*!< Interest Coverage test. Considered part of the interest waterfall*/
		//wst_ICTestPrinc = 7, /*!< Interest Coverage test. Considered part of the principal waterfall*/ Obsolete
		wst_DeferredInterest = 8, /*!< Payment of deferred interest. Considered part of the interest waterfall*/
		//wst_DeferredPrinc = 9, /*!< Payment of deferred interest. Considered part of the principal waterfall*/ Obsolete
		wst_juniorFees = 10, /*!< Junior Fees*/
		//wst_ReinvestmentTest=11, /*!< Reinvestment Test*/ Obsolete
		wst_Excess = 12, /*!< Excess Spread*/
		wst_ReinvestPrincipal = 13, /*!< Reinvest all available principal during the reinvestment period*/
		wst_ReserveReplenish = 14, /*!< Replenish the reserve fund*/
		//wst_RedeemProRata = 15, /*!< Use principal to redeem bonds pro-rata*/ Obsolete
		wst_Turbo = 16,  /*!< Use interest to redeem a class of notes*/
		wst_PDL = 17,  /*!< Cure Outstanding PDL*/
		wst_FeesFromExcess = 18 /*!< Junior Fees as percentage of excess spread*/
	};
	enum class wstParameters : qint32 {
		SeniorityGroup
		, SeniorityGroupLevel
		, RedemptionGroup
		, RedemptionGroupLevel
		, RedemptionShare
		, AdditionalCollateralShare
		, SourceOfFunding
		, CouponIndex
		, TestTargetOverride
		, IRRtoEquityTarget
		, ReserveIndex
		, Trigger
	};
	QVariant GetParameter(qint32 ParameterType) const { return Parameters.value(ParameterType, QVariant()); }
	bool HasParameter(qint32 ParameterType) const { return Parameters.contains(ParameterType); }
	void SetParameter(qint32 ParameterType, QVariant val) { Parameters[ParameterType] = val; }
	QVariant GetParameter(wstParameters ParameterType) const { return GetParameter(static_cast<qint32>(ParameterType)); }
	bool HasParameter(wstParameters ParameterType)const { return HasParameter(static_cast<qint32>(ParameterType)); }
	void SetParameter(wstParameters ParameterType, QVariant val) { SetParameter(static_cast<qint32>(ParameterType), val); }
	void RemoveParameter(qint32 ParameterType) { Parameters.remove(ParameterType); }
	void RemoveParameter(wstParameters ParameterType) { RemoveParameter(static_cast<qint32>(ParameterType)); }
	void ClearParameters() { Parameters.clear(); }
	void SetParameters(const QHash<qint32, QVariant>& a) { Parameters = a; }
	WaterfallStepType GetPriorityType() const{return PriorityType;}
	void SetPriorityType(WaterfallStepType a){PriorityType=a;}
	QString ReadyToCalculate() const;
	WatFalPrior();
	WatFalPrior(const WatFalPrior& a);
	WatFalPrior& operator=(const WatFalPrior& a);
private:
	QHash<qint32, QVariant> Parameters;
	WaterfallStepType PriorityType;
protected:
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
	friend QDataStream& operator<<(QDataStream & stream, const WatFalPrior& flows);
	friend QDataStream& operator>>(QDataStream & stream, WatFalPrior& flows);
};
QDataStream& operator<<(QDataStream & stream, const WatFalPrior& flows);
QDataStream& operator>>(QDataStream & stream, WatFalPrior& flows);
#endif // WatFalPrior_h__
