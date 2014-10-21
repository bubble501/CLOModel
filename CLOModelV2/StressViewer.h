#include <QWidget>
#include "Waterfall.h"
#include "StressTest.h"
#include <QPropertyAnimation>
#include <QSet>
#include <QString>
class QTableWidget;
class QLabel;
class QComboBox;
class QDoubleSpinBox;
class QTableModel;
class StressViewer : public QWidget{
	Q_OBJECT
public:
	StressViewer(QWidget* parent=0);
	bool LoadStress(const QString& filename);
	QString GetFirstName() const;
private:
	AssumptionSet AssembleAssumption(int r, int c) const;
	QString ParamNames[NumStressDimentsions];
	QComboBox* DisplayValuesCombo;
	QDoubleSpinBox* PriceSpin;
	QLabel* PriceLabel;
	QList<QString> AvailableAssumptions[NumStressDimentsions];
	QTableWidget* StressTable;
	StressTest* StressTarget;
	QComboBox* TrancheCombo;
	QComboBox* ConstParCombo[NumStressDimentsions-2];
	QLabel* ConstParLabel[NumStressDimentsions - 2];
	QComboBox* VariableParCombo[2];
	QLabel* VariableParLabel[2];
	QPropertyAnimation GradientBase;
	bool PriceToBeSet;
	enum {
		AssCPR = 0
		, AssCDR = 1
		, AssLS = 2
		, AssRecLag = 3
		, AssDelinq = 4
		, AssDelinqLag = 5
	};
protected:
	const QColor GreenFill;
	const QColor YellowFill;
	const QColor RedFill;
	const QColor GrayFill;
	void closeEvent(QCloseEvent *event);
	
private slots:
	void UpdateCombos();
	void UpdateTable();
	void SetPriceChange() { PriceToBeSet = true; }
signals:
	//void StressLevelChanged(Waterfall);
	void Closing();
};