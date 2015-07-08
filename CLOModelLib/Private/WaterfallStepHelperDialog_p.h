#include "WaterfallStepHelperDialog.h"
#include <QMap>

class QComboBox;
class QStackedWidget;
class TriggerStructHelperWidget;
class QSortFilterProxyModel;
class QStandardItemModel;
class WaterfallStepHelperDialogPrivate
{
    DECLARE_PRIVATE_COMMONS(WaterfallStepHelperDialog)
protected:
    WaterfallStepHelperDialog* q_ptr;
public:
    enum
    {
        AllWaterfalls = 0
        , InterestWFonly = 1
        , PrincipalWFonly = 2
    };
    enum { NumberOfStepTypes = 16 };
    QStandardItemModel* StepSelectorModel;
    QSortFilterProxyModel* StepsFilter;
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
    QWidget* CreateJuniorFeesWidget();
    bool FirstCombodeleted;
    bool Cleared;
    QMap<qint32, QString> ResultingParameters;
    QPushButton* AcceptButton;
    QComboBox* StepSelectorCombo;
    QStackedWidget* StepBuilderBase;
    bool m_InterestWF;
    TriggerStructHelperWidget* TriggerBuilder;
};