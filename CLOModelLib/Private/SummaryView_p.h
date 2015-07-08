#ifndef SummaryView_p_h__
#define SummaryView_p_h__


#include "SummaryView.h"
#include <QList>
#include "Waterfall.h"
#include "StressTest.h"
class TrancheViewer;
class QTableWidget;
class Mortgage;
class QTabWidget;
class QStackedWidget;
class QListWidget;
class QLineEdit;
class ChartsWidget;
class WaterfallViewer;
class SummaryViewPrivate
{
    DECLARE_PRIVATE_COMMONS(SummaryView)
protected:
    SummaryView* q_ptr;
public:
    QTableWidget* MtgTable;
    QList<TrancheViewer*> TrancheTables;
    QList<TrancheViewer*> CallTrancheTables;
    QTabWidget* MainWidget;
    Waterfall Structure;
    Waterfall CallStructure;
    QStackedWidget* CallTranchesArea;
    QListWidget* CallTranchesList;
    QStackedWidget* TranchesArea;
    QListWidget* TranchesList;
    QTableWidget* ExpensesTable;
    QTableWidget* ReinvestmentsTable;
    QTableWidget* GICTable;
    QTableWidget* StructureTable;
    QList<QLineEdit*> PricesLabel;
    ChartsWidget* ChartPlotter;
    WaterfallViewer* WatStructView;
    void ResetCharts();
    void DisplayStructure();
    void ResetTranches();
    void ResetPricesLabel();
};
#endif // SummaryView_p_h__