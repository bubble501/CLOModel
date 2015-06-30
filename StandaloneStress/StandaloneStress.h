#ifndef QUICKOBJECT_H
#define QUICKOBJECT_H

#include <QWidget>
#include <CommonFunctions.h>
#include <Waterfall.h>
#include <StressTest.h>
class QLineEdit;
class QTableWidget;
class QLabel;
class QComboBox;
class QSpinBox;
class QPushButton;
class QCheckBox;
class StandaloneStress : public QWidget
{
	Q_OBJECT

public:
	StandaloneStress(QWidget *parent=0);
	
private slots:
	void Start();
	void Finished();
	void FinishedEroors();
	void CheckAllValid();
	void RowsChanged(int NewRows);
	void PasteClipboard();
	void BrowseModel();
	void BrowseFolder();
private:
	QPushButton *PasteButton[NumStressDimentsions];
	StressTest *Stresser;
	QTableWidget* VariablesList[NumStressDimentsions];

	QLineEdit* OutPathEdit;
	QLineEdit* InPathEdit;
	QLabel* VariablesCountlabel[NumStressDimentsions];
	QSpinBox* VariablesCount[NumStressDimentsions];
	QPushButton* StartButton;
	QCheckBox* StressToCallBox;
	QCheckBox* FastStressBox;
};

#endif // QUICKOBJECT_H
