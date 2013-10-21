#ifndef TestTable_h__
#define TestTable_h__

#include <QWidget>
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
class SummaryView : public QWidget{
	Q_OBJECT
private:
	QTableWidget* MtgTable;
	QList<TrancheViewer*> TrancheTables;
	QTabWidget* MainWidget;
	Waterfall Structure;
	QStackedWidget* TranchesArea;
	QListWidget* TranchesList;
	QTableWidget* ExpensesTable;
	QTableWidget* ReinvestmentsTable;
	QTableWidget* StructureTable;
	QList<QLineEdit*> PricesLabel;
	void DisplayStructure();
	void ResetTranches();
	void ResetPricesLabel();
public:
	void SetStructure(const Waterfall& a);
	SummaryView(QWidget* parent=0);
protected:
	void resizeEvent(QResizeEvent *event);
private slots:
	void AdjustTableSizes();
	void PriceChanged();
};
#endif // TestTable_h__