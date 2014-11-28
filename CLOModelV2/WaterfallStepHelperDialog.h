#ifndef WATERFALLSTEPHELPERDIALOG_H
#define WATERFALLSTEPHELPERDIALOG_H

#include <QDialog>
#include <QStringList>
class QComboBox;
class QStackedWidget;
#define NumwstParameters 12
class WaterfallStepHelperDialog : public QDialog
{
	Q_OBJECT

public:
	WaterfallStepHelperDialog(QWidget *parent=0);
	QStringList GetParameters() const;
private slots:
	void CheckOkEnabled(int index);
	void ClearParameters() { for (int i = 0; i < NumwstParameters; ++i) ResultingParameters[i].clear(); }
	void SetParameter(int PrmIndex, QString value) { if (PrmIndex < 0 || PrmIndex >= NumwstParameters) return;  ResultingParameters[PrmIndex] = value; }
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
signals:
	void SetDefaults(QString);
private:
	QWidget* CreateInterestWidget();
	QWidget* CreatePrincipalWidget();
	bool FirstCombodeleted;
	QString ResultingParameters[NumwstParameters];
	QPushButton* AcceptButton;
	QComboBox* StepSelectorCombo;
	QStackedWidget* StepBuilderBase;
	bool m_InterestWF;
};

#endif // WATERFALLSTEPHELPERDIALOG_H
