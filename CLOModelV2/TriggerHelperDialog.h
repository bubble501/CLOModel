#ifndef TRIGGERHELPERDIALOG_H
#define TRIGGERHELPERDIALOG_H

#include <QDialog>
#include <QStringList>
class QComboBox;
class QStackedWidget;
class QPushButton;
class QDateEdit;
class QLineEdit;
class QDoubleSpinBox;
class TriggerHelperDialog : public QDialog
{
	Q_OBJECT

public:
	TriggerHelperDialog(QDialog *parent = 0);
	QString GetParameters() const;
	void SetCurrentPars(const QString& pars);
private slots:
	void ClearParameters();
	void CheckOkEnabled(int index);
	void SetParameter(int parIdx, const QString& parVal);
signals:
	void ImportParam(int parIdx, const QString& parVal);
private:
	bool Cleared;
	bool FirstCombodeleted;
	QComboBox* TriggerTypeCombo;
	QStackedWidget* TriggerBuilderBase;
	QPushButton* AcceptButton;
	QLineEdit* TriggerLabelEdit;
	QStringList ParamColumns;
	QWidget* CreateDateTriggerWidget();
	QWidget* CreateVectorTriggerWidget();
	QWidget* CreatePoolSizeTriggerWidget();
	QWidget* CreateTrancheTriggerWidget();
	QWidget* CreateDelinquencyTriggerWidget();
	QWidget* CreateCumulativeLossTriggerWidget();
	QWidget* CreateDeferredInterestTriggerWidget();
    QWidget* CreatePDLWidget();

	QDateEdit* DateTrigger_dateEdit;
	QComboBox* DateTrigger_SideCombo;
	QLineEdit* VectorTrigger_VecEditor;
	QComboBox* PoolTrigger_SideCombo;
	QLineEdit* PoolTrigger_VecEditor;
	QLineEdit* DelinqTrigger_VecEditor;
	QLineEdit* TrancheTrigger_SenEditor;
	QLineEdit* TrancheTrigger_SenLvlEditor;
	QLineEdit* TrancheTrigger_SizeEditor;
	QLineEdit* DeferredTrigger_SenEditor;
	QLineEdit* DeferredTrigger_SenLvlEditor;
	QLineEdit* DeferredTrigger_SizeEditor;
	QLineEdit* DeferredTrigger_CouponEditor;
	QDoubleSpinBox* TrancheTrigger_SizeMultEditor;
	QComboBox* TrancheTrigger_SizeSideCombo;
	QComboBox* TrancheTrigger_SeniorSideCombo;
	QDoubleSpinBox* DeferredTrigger_SizeMultEditor;
	QComboBox* DeferredTrigger_SizeSideCombo;
	QComboBox* DeferredTrigger_SeniorSideCombo;
	QComboBox* DeferredTrigger_CouponSideCombo;
	QComboBox* CumLoss_SideCombo;
	QLineEdit* CumLoss_VecEditor;


};

#endif // TRIGGERHELPERDIALOG_H
