#include <QWidget>
#include "Waterfall.h"
#include "StressTest.h"
class QTableWidget;
class QLabel;
class QComboBox;
class StressViewer : public QWidget{
	Q_OBJECT
public:
	StressViewer(QWidget* parent=0);
	void LoadStress(const QString& filename);
private:
	QTableWidget* Table;
	QLabel* TypeLabel;
	QComboBox* TypeCombo;
	QLabel* TrancheLabel;
	QLabel* XDimLabel;
	QLabel* YDimLabel;
	QLabel* ConstParLabel;
	QComboBox* TrancheCombo;
	StressTest StressTarget;

	int TrancheTarg;
	int TypeTarg;
protected:
	void resizeEvent(QResizeEvent *event);
	void closeEvent(QCloseEvent *event);
private slots:
	void TableTargetChanged(int a){TypeTarg=a; UpdateTable();}
	void TrancheTargetChanged(int a){TrancheTarg=a; UpdateTable();}
	void AdjustTableSize();
	void UpdateTable();
	void CellSelected(int r,int c);
signals:
	void StressLevelChanged(Waterfall);
	void Closing();
};