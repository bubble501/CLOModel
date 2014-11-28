#ifndef WATERFALLSTEPHELPERDIALOG_H
#define WATERFALLSTEPHELPERDIALOG_H

#include <QDialog>
#include <QMap>
class QComboBox;
class QStackedWidget;

class WaterfallStepHelperDialog : public QDialog
{
	Q_OBJECT

public:
	WaterfallStepHelperDialog(QWidget *parent=0);
	QString GetParameters() const;
private slots:
	void CheckOkEnabled(int index);
	void ClearParameters() { ResultingParameters.clear(); }
	void SetParameter(int PrmIndex, QString value) { ResultingParameters[PrmIndex] = value; }
	void SetSeniorityGroup(QString value);
	void SetSeniorityGroupLevel(QString value);
	void SetRedemptionGroup(QString value);
	void SetRedemptionGroupLevel(QString value);
	void SetRedemptionShare(QString value);
	void SetAdditionalCollateralShare(QString value);
	void SetSourceOfFunding(QString value);
	void SetCouponIndex(QString value);
	void SetTestTargetOverride(QString value);
	void SetIRRtoEquityTarget(QString value);
	void SetReserveIndex(QString value);
	void SetTrigger(QString value);
	void SetDefaults1();
	void SetDefaults100();
	void SetDefaults0();
	void SetbasedOnWaterfall(int index);
	void ResetSoFCombo(int index);
	void SoFComboChanged(int index);
signals:
	void SetTo1(QString);
	void SetTo100(QString);
	void SetTo0(QString);
	void SetSoFCombo(int);
private:
	QWidget* CreateInterestWidget();
	QWidget* CreatePrincipalWidget();
	QWidget* CreateICWidget();
	QWidget* CreateOCWidget();
	QWidget* CreateDeferredInterestWidget();
	QWidget* CreateExcessWidget();
	QWidget* CreateReinvestPrincipalWidget();
	QWidget* CreateReserveReplenishWidget();
	QWidget* CreateTurboWidget();
	QWidget* CreateCurePDLWidget();
	QWidget* CreateFeesFromXSWidget();
	QWidget* CreateAllocPrepayWidget();
	bool FirstCombodeleted;
	QMap<qint32,QString> ResultingParameters;
	QPushButton* AcceptButton;
	QComboBox* StepSelectorCombo;
	QStackedWidget* StepBuilderBase;
	bool m_InterestWF;
};

#endif // WATERFALLSTEPHELPERDIALOG_H
