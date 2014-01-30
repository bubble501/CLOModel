#ifndef QUICKOBJECT_H
#define QUICKOBJECT_H

#include <QWidget>

#include "CommonFunctions.h"
#include "Waterfall.h"
#include "ExcelOutput.h"
#include "StressTest.h"
class QLineEdit;
class QTableWidget;
class QLabel;
class QComboBox;
class QSpinBox;
class QPushButton;
class QuickObject : public QWidget
{
	Q_OBJECT

public:
	QuickObject(QWidget *parent=0);
	
private slots:
	void Start();
	void Finished();
	void ChangeType(int tpInd);
	void CheckAllValid();
	void RowsChanged();
private:
	QLabel* ConstantLabel;
	StressTest *Stresser;
	QTableWidget* VariablesList[2];
	QLineEdit* ConstantEdit;
	QComboBox* StressTypeCombo;
	QLineEdit* PathEdit;
	QLabel* VariablesCountlabel[2];
	QSpinBox* VariablesCount[2];
	QPushButton* StartButton;
};

#endif // QUICKOBJECT_H
