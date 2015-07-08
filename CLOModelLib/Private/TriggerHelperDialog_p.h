#ifndef TriggerHelperDialog_p_h__
#define TriggerHelperDialog_p_h__

#include "TriggerHelperDialog.h"
#include "CommonFunctions.h"
class QStackedWidget;
class QDialogButtonBox;
class QLineEdit;
class QComboBox;
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
    QDialogButtonBox* DialogButtons;
    QLineEdit* TriggerLabelEdit;
};
#endif // TriggerHelperDialog_p_h__