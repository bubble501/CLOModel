#ifndef WaterfallViewer_h__
#define WaterfallViewer_h__
#include <QWidget>
#include "WatFalPrior.h"
class QTableWidget;
class QLabel;
class Waterfall;
class IntegerVector;
class BloombergVector;
class WaterfallViewer : public QWidget{
	Q_OBJECT
public:
	WaterfallViewer(QWidget* parent=0);
	void ResetSteps();
	void AddStep(const WatFalPrior& a);
	void AddStep(
		WatFalPrior::WaterfallStepType Step
		, const QString& ArgSeniorityGroup
		, const QString& ArgSeniorityGroupLevel
		, const QString& ArgRedemptionGroup
		, const QString& ArgRedemptionGroupLevel
		, const QString& ArgRedemptionShare
		, const QString& ArgAdditionalCollateralShare
		, const QString& ArgSourceofFunding
		, const QString& ArgCouponIndex
		, const QString& ArgTestTargetOverride
		, const QString& ArgIRRtoEquityTarget
		, const QString& ArgReserveIndex
		, const QString& ArgTrigger
	);
	void SetWaterfall(const Waterfall& a);
private:
	QTableWidget* InterestTable;
	QTableWidget* PrincipalTable;
	QLabel* InterestLabel;
	QLabel* PrincipalLabel;
};
#endif // WaterfallViewer_h__