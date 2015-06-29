#ifndef TriggerStructHelperWidget_p_h__
#define TriggerStructHelperWidget_p_h__

#include "TriggerStructHelperWidget.h"
#include "AbstractTrigger.h"

class QStandardItemModel;
class QLineEdit;
class QTableView;
class QPushButton;
class QModelIndex;
class TriggerStructHelperWidgetPrivate
{
    DECLARE_PRIVATE_COMMONS(TriggerStructHelperWidget)
protected:
    TriggerStructHelperWidget* q_ptr;
public:
    QLineEdit* EncriptedTriggers;
    QLineEdit* DecriptedTriggers;
    QStandardItemModel* TriggersModel;
    QTableView *TriggerTable;
    QPushButton* AndButton;
    QPushButton* OrButton;
    QPushButton* NandButton;
    QPushButton* NorButton;
    QPushButton* OpenParButton;
    QPushButton* CloseParButton;
    QPushButton* DelButton;
    QPushButton* ClearButton;
    QPushButton* NegateButton;
    void EnableOperators(bool a = true);
};
#endif // TriggerStructHelperWidget_p_h__
