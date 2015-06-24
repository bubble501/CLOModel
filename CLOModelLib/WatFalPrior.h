#ifndef WatFalPrior_h__
#define WatFalPrior_h__
#include <QDataStream>
#include <QString>
#include <QVariant>
#include "BackwardCompatibilityInterface.h"
class IntegerVector;
class BloombergVector;
class WatFalPriorPrivate;
class WatFalPrior : public BackwardInterface {
    DECLARE_PUBLIC_COMMONS(WatFalPrior)
    DECLARE_PUBLIC_COMMONS_COPY(WatFalPrior)
public:
	//! Enum defining the different types of waterfall steps
	enum  WaterfallStepType: qint16{
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
		wst_ReinvestmentTest=11, /*!< Reinvestment Test. It's the same effect as wst_OCTest*/
		wst_Excess = 12, /*!< Excess Spread*/
		wst_ReinvestPrincipal = 13, /*!< Reinvest all available principal during the reinvestment period*/
		wst_ReserveReplenish = 14, /*!< Replenish the reserve fund*/
		//wst_RedeemProRata = 15, /*!< Use principal to redeem bonds pro-rata*/ Obsolete
		wst_Turbo = 16,  /*!< Use interest to redeem a class of notes*/
		wst_PDL = 17,  /*!< Cure Outstanding PDL*/
		wst_FeesFromExcess = 18, /*!< Junior Fees as percentage of excess spread*/
		wst_AllocPrepayFees=19  /*!< Allocate proceeds from prepayment fees*/
	};
	enum  wstParameters : qint32 {
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
		, PayAccrue
		, Trigger
	};
	enum  wstAccrueOrPay :quint8 {
		Invalid = 0
		, Accrue = 1
		, Pay = 2
		, AccrueAndPay = Accrue | Pay
	};
	void FillMissingAnchors(const QDate& a);
	void ResetMissinAnchors();
	QVariant GetParameter(wstParameters ParameterType) const;
	bool HasParameter(wstParameters ParameterType)const;
	void SetParameter(wstParameters ParameterType, const QString& val);
	void RemoveParameter(wstParameters ParameterType);
	void ClearParameters();
	WaterfallStepType GetPriorityType() const;
	void SetPriorityType(WaterfallStepType a);
	QString ReadyToCalculate() const;
	virtual ~WatFalPrior();
	QString ToString() const;
	QString CodeForDialog() const;
protected:
    virtual void RemoveParameter(qint32 ParameterType);
    virtual QVariant GetParameter(qint32 ParameterType) const;
    virtual bool HasParameter(qint32 ParameterType) const;
    virtual void SetParameter(qint32 ParameterType, const QString& val);
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
	friend QDataStream& operator<<(QDataStream & stream, const WatFalPrior& flows);
	friend QDataStream& operator>>(QDataStream & stream, WatFalPrior& flows);
};
QDataStream& operator<<(QDataStream & stream, const WatFalPrior& flows);
QDataStream& operator>>(QDataStream & stream, WatFalPrior& flows);
#endif // WatFalPrior_h__
