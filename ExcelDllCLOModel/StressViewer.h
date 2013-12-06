#include <QWidget>
#include "Waterfall.h"
#include "StressTest.h"
class QTableWidget;
class QLabel;
class QComboBox;
class QDoubleSpinBox;
class StressViewer : public QWidget{
	Q_OBJECT
public:
	StressViewer(QWidget* parent=0);
	bool LoadStress(const QString& filename);
	QString GetFirstName() const{return StressTarget.GetResults().value(StressTarget.GetXSpann().first()).value(StressTarget.GetYSpann().first()).GetTranche(0)->GetTrancheName();}
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
	QLabel* PriceLabel;
	QDoubleSpinBox* PriceSpin;
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
	void PriceChanged(double a);
signals:
	void StressLevelChanged(Waterfall);
	void Closing();
};