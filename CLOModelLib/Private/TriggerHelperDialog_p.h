#ifndef TriggerHelperDialog_p_h__
#define TriggerHelperDialog_p_h__

#include "TriggerHelperDialog.h"
#include "CommonFunctions.h"
#include <QStringList>
class QComboBox;
class QStackedWidget;
class QPushButton;
class QDateEdit;
class QLineEdit;
class QDoubleSpinBox;
class TriggerHelperDialogPrivate
{
    DECLARE_PRIVATE_COMMONS(TriggerHelperDialog)
protected:
    TriggerHelperDialog* q_ptr;
public:
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
#endif // TriggerHelperDialog_p_h__