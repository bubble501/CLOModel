#include "QuickObject.h"
#include <QTableWidget>
#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QComboBox>
#include <QSpinBox>
#include <QFile>
#include <QPushButton>
#include <QDir>
#include <QHeaderView>
#include "CommonFunctions.h"
QuickObject::QuickObject(QWidget *parent)
	: QWidget(parent)
{
	setWindowTitle("Standalone Stress Test");
	setWindowIcon(QIcon(":/Icons/Logo.png"));
	if(!QMetaType::isRegistered(qMetaTypeId<Waterfall>()))
		qRegisterMetaType<Waterfall>("Waterfall");
	if(!QMetaType::isRegistered(qMetaTypeId<MtgCashFlow>()))
		qRegisterMetaType<MtgCashFlow>("MtgCashFlow");
	Stresser=new StressTest(this);
	connect(Stresser,SIGNAL(AllFinished()),this,SLOT(Finished()));

	QVBoxLayout* MainLay=new QVBoxLayout(this);
	ConstantLabel=new QLabel(this);
	ConstantLabel->setText("CPR");
	ConstantLabel->setAlignment(Qt::AlignRight);
	ConstantEdit=new QLineEdit(this);
	ConstantEdit->setText("0");
	StressTypeCombo=new QComboBox(this);
	StressTypeCombo->addItem("CDR-LS");
	StressTypeCombo->addItem("CPR-LS");
	StressTypeCombo->addItem("CPR-CDR");
	
	QHBoxLayout* TopLay=new QHBoxLayout;
	QSpacerItem* TopSpace=new QSpacerItem(20,20,QSizePolicy::Expanding);
	TopLay->addWidget(StressTypeCombo);
	TopLay->addItem(TopSpace);
	TopLay->addWidget(ConstantLabel);
	TopLay->addWidget(ConstantEdit);
	MainLay->addLayout(TopLay);
	PathEdit=new QLineEdit(this);
	PathEdit->setText("C:\\Temp");
	PathEdit->setToolTip("Folder Containing the .Loans.clp and .BaseCase.clo files for the relevant model<br/>Results will be put in this folder");
	MainLay->addWidget(PathEdit);
	QHBoxLayout *SpinnsLay=new QHBoxLayout;
	QSpacerItem* SpinnsSpace=new QSpacerItem(20,20,QSizePolicy::Expanding);
	for(int i=0;i<2;i++){
		VariablesCountlabel[i]=new QLabel(this);
		SpinnsLay->addWidget(VariablesCountlabel[i]);
		VariablesCount[i]=new QSpinBox(this);
		VariablesCount[i]->setMinimum(1);
		VariablesCount[i]->setMaximum(99);
		VariablesCount[i]->setValue(1);
		SpinnsLay->addWidget(VariablesCount[i]);
	}
	VariablesCountlabel[0]->setText("CDR Scenarios");
	VariablesCountlabel[1]->setText("LS Scenarios");
	SpinnsLay->addItem(SpinnsSpace);
	MainLay->addLayout(SpinnsLay);

	QHBoxLayout *TableLay=new QHBoxLayout;
	for(int i=0;i<2;i++){
		VariablesList[i]=new QTableWidget(this);
		VariablesList[i]->setColumnCount(1);
		VariablesList[i]->setRowCount(1);
		VariablesList[i]->setItem(0,0,new QTableWidgetItem("0"));
		VariablesList[i]->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		VariablesList[i]->verticalHeader()->hide();
		VariablesList[i]->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
		VariablesList[i]->setSelectionMode(QAbstractItemView::NoSelection);
		TableLay->addWidget(VariablesList[i]);
	}
	{
		QStringList TmpStrList;
		TmpStrList<< "CDR";
		VariablesList[0]->setHorizontalHeaderLabels(TmpStrList);
		TmpStrList.clear();
		TmpStrList<< "LS";
		VariablesList[1]->setHorizontalHeaderLabels(TmpStrList);
	}
	MainLay->addLayout(TableLay);

	StartButton=new QPushButton(this);
	StartButton->setText("Start Stress Test");
	QHBoxLayout *BottomLay=new QHBoxLayout;
	QSpacerItem* BottomSpace=new QSpacerItem(20,20,QSizePolicy::Expanding);
	BottomLay->addItem(BottomSpace);
	BottomLay->addWidget(StartButton);
	MainLay->addLayout(BottomLay);

	connect(StressTypeCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(ChangeType(int)));
	connect(ConstantEdit,SIGNAL(editingFinished()),this,SLOT(CheckAllValid()));
	connect(PathEdit,SIGNAL(editingFinished()),this,SLOT(CheckAllValid()));
	connect(StartButton,SIGNAL(clicked()),this,SLOT(Start()));
	for(int i=0;i<2;i++){
		connect(VariablesList[i],SIGNAL(itemChanged(QTableWidgetItem *)),this,SLOT(CheckAllValid()));
		connect(VariablesCount[i],SIGNAL(valueChanged(int)),this,SLOT(RowsChanged()));
	}
	CheckAllValid();
}

void QuickObject::CheckAllValid(){
	StartButton->setEnabled(true);
	ConstantEdit->setStyleSheet("");
	PathEdit->setStyleSheet("");
	QDir dir(PathEdit->text());
	if(!dir.exists()){
		StartButton->setEnabled(false);
		PathEdit->setStyleSheet("background-color: red;");
	}
	if(!ValidBloombergVector(ConstantEdit->text())){
		StartButton->setEnabled(false);
		ConstantEdit->setStyleSheet("background-color: red;");
	}
	for(int i=0;i<2;i++){
		for(int j=0;j<VariablesList[i]->rowCount();j++){
			if(!ValidBloombergVector(VariablesList[i]->item(j,0)->text())){
				StartButton->setEnabled(false);
				VariablesList[i]->item(j,0)->setBackgroundColor(Qt::red);
			}
			else VariablesList[i]->item(j,0)->setBackgroundColor(Qt::white);
		}
	}
}
void QuickObject::ChangeType(int tpInd){
	switch(tpInd){
	case 1:
		{
			QStringList TmpStrList;
			TmpStrList<< "CPR";
			VariablesList[0]->setHorizontalHeaderLabels(TmpStrList);
			TmpStrList.clear();
			TmpStrList<< "LS";
			VariablesList[1]->setHorizontalHeaderLabels(TmpStrList);
		}
		VariablesCountlabel[0]->setText("CPR Scenarios");
		VariablesCountlabel[1]->setText("LS Scenarios");
		ConstantLabel->setText("CDR");
		break;
	case 2:
		{
			QStringList TmpStrList;
			TmpStrList<< "CPR";
			VariablesList[0]->setHorizontalHeaderLabels(TmpStrList);
			TmpStrList.clear();
			TmpStrList<< "CDR";
			VariablesList[1]->setHorizontalHeaderLabels(TmpStrList);
		}
		VariablesCountlabel[0]->setText("CPR Scenarios");
		VariablesCountlabel[1]->setText("CDR Scenarios");
		ConstantLabel->setText("LS");
		break;
	case 0:
	default:
		{
			QStringList TmpStrList;
			TmpStrList<< "CDR";
			VariablesList[0]->setHorizontalHeaderLabels(TmpStrList);
			TmpStrList.clear();
			TmpStrList<< "LS";
			VariablesList[1]->setHorizontalHeaderLabels(TmpStrList);
		}
		VariablesCountlabel[0]->setText("CDR Scenarios");
		VariablesCountlabel[1]->setText("LS Scenarios");
		ConstantLabel->setText("CPR");
	}
}
void QuickObject::RowsChanged(){
	int OldNum;
	for(int i=0;i<2;i++){
		OldNum=VariablesList[i]->rowCount();
		if(OldNum!=VariablesCount[i]->value()) VariablesList[i]->setRowCount(VariablesCount[i]->value());
		for(;OldNum<VariablesList[i]->rowCount();OldNum++){
			VariablesList[i]->setItem(OldNum,0,new QTableWidgetItem("0"));
		}
	}
}
void QuickObject::Start(){
	Waterfall TempWaterfall;
	QString Tempstring;
	int countloans;
	QDate StartDate;
	Mortgage TmpMtg;
	QDir dir(PathEdit->text());
	{QFile file(dir.absolutePath()+"/.Loans.clp");
	file.open(QIODevice::ReadOnly);
	qint32 VersionChecker;
	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_4_8);
	out >> VersionChecker;
	if(VersionChecker!=qint32(ModelVersionNumber)) return;
	out >> countloans;
	out >> Tempstring;
	out >> Tempstring;
	out >> Tempstring;
	out >> StartDate;
	for(int i=0;i<countloans;i++){
		out >> TmpMtg;
		Stresser->AddLoan(TmpMtg);
	}
	file.close();}
	{QFile file(dir.absolutePath()+"/.BaseCase.clo");
	file.open(QIODevice::ReadOnly);
	qint32 VersionChecker;
	QDataStream out(&file);
	out.setVersion(QDataStream::Qt_4_8);
	out >> VersionChecker;
	if(VersionChecker!=qint32(ModelVersionNumber)) return;
	out >> TempWaterfall;
	file.close();}



	Stresser->SetConstantPar(ConstantEdit->text());
	QStringList TempStrLst; 
	for(int i=0;i<VariablesList[0]->rowCount();i++) TempStrLst.append(VariablesList[0]->item(i,0)->text());
	Stresser->SetXSpann(TempStrLst);
	TempStrLst.clear();
	for(int i=0;i<VariablesList[1]->rowCount();i++) TempStrLst.append(VariablesList[1]->item(i,0)->text());
	Stresser->SetYSpann(TempStrLst);
	switch(StressTypeCombo->currentIndex()){
	case 1:
		Stresser->SetXVariability(StressTest::ChangingCPR);
		Stresser->SetYVariability(StressTest::ChangingLS);
		break;
	case 2:
		Stresser->SetXVariability(StressTest::ChangingCPR);
		Stresser->SetYVariability(StressTest::ChangingCDR);
		break;
	case 0:
	default:
		Stresser->SetXVariability(StressTest::ChangingCDR);
		Stresser->SetYVariability(StressTest::ChangingLS);
	}
	Stresser->SetStartDate(StartDate);
	Stresser->SetStructure(TempWaterfall);
	hide();
	Stresser->RunStressTest();
}
void QuickObject::Finished(){
	QDir dir(PathEdit->text());
	Stresser->SaveResults(dir.absolutePath());
	close();
}