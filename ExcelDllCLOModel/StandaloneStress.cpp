#include "StandaloneStress.h"
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
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QMessageBox>
#include <QFileDialog>
#include <QCheckBox>
#include "CommonFunctions.h"
#include "BloombergVector.h"
StandaloneStress::StandaloneStress(QWidget *parent)
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
	StressToCallBox=new QCheckBox(this);
	StressToCallBox->setText("Stress To Call");
	StressToCallBox->setChecked(false);
	
	QHBoxLayout* TopLay=new QHBoxLayout;
	QSpacerItem* TopSpace=new QSpacerItem(20,20,QSizePolicy::Expanding);
	TopLay->addWidget(StressTypeCombo);
	TopLay->addItem(TopSpace);
	TopLay->addWidget(StressToCallBox);
	TopLay->addWidget(ConstantLabel);
	TopLay->addWidget(ConstantEdit);
	MainLay->addLayout(TopLay);
	QHBoxLayout* PathLay=new QHBoxLayout;
	PathEdit=new QLineEdit(this);
	PathEdit->setText("C:\\Temp");
	PathEdit->setToolTip("Folder Containing the .Loans.clp and .BaseCase.clo files for the relevant model<br/>Results will be put in this folder");
	PathLay->addWidget(PathEdit);
	QPushButton* BrowseButton=new QPushButton(this);
	BrowseButton->setText("Browse...");
	connect(BrowseButton,SIGNAL(clicked()),this,SLOT(BrowseFolder()));
	PathLay->addWidget(BrowseButton);
	MainLay->addLayout(PathLay);
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

	QGridLayout *TableLay=new QGridLayout;
	for(int i=0;i<2;i++){
		VariablesList[i]=new QTableWidget(this);
		VariablesList[i]->setColumnCount(1);
		VariablesList[i]->setRowCount(1);
		VariablesList[i]->setItem(0,0,new QTableWidgetItem("0"));
		VariablesList[i]->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		VariablesList[i]->verticalHeader()->hide();
		VariablesList[i]->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
		VariablesList[i]->setSelectionMode(QAbstractItemView::NoSelection);
		TableLay->addWidget(VariablesList[i],0,i);
		QPushButton *TempPasteButton=new QPushButton(this);
		TempPasteButton->setText("Paste from Clipboard");
		if(i==0) connect(TempPasteButton,SIGNAL(clicked()),this,SLOT(PasteFirst()));
		else connect(TempPasteButton,SIGNAL(clicked()),this,SLOT(PasteSecond()));
		TableLay->addWidget(TempPasteButton,1,i);
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
	connect(ConstantEdit,SIGNAL(editingFinished()),this,SLOT(CheckAllValid()),Qt::QueuedConnection);
	connect(PathEdit,SIGNAL(editingFinished()),this,SLOT(CheckAllValid()),Qt::QueuedConnection);
	connect(StartButton,SIGNAL(clicked()),this,SLOT(Start()));
	for(int i=0;i<2;i++){
		connect(VariablesList[i],SIGNAL(itemChanged(QTableWidgetItem *)),this,SLOT(CheckAllValid()),Qt::QueuedConnection);
		connect(VariablesCount[i],SIGNAL(valueChanged(int)),this,SLOT(RowsChanged()),Qt::QueuedConnection);
	}
	CheckAllValid();
}

void StandaloneStress::CheckAllValid(){
	StartButton->setEnabled(true);
	ConstantEdit->setStyleSheet("");
	PathEdit->setStyleSheet("");
	QDir dir(PathEdit->text());
	if(!dir.exists() || !QFile::exists(dir.absolutePath()+"/.Loans.clp") || !QFile::exists(dir.absolutePath()+"/.BaseCase.clo")){
		StartButton->setEnabled(false);
		PathEdit->setStyleSheet("background-color: red;");
	}
	if(BloombergVector(ConstantEdit->text()).IsEmpty()){
		StartButton->setEnabled(false);
		ConstantEdit->setStyleSheet("background-color: red;");
	}
	for(int i=0;i<2;i++){
		for(int j=0;j<VariablesList[i]->rowCount();j++){
			if(VariablesList[i]->item(j,0)){
				if(BloombergVector(VariablesList[i]->item(j,0)->text()).IsEmpty()){
					StartButton->setEnabled(false);
					VariablesList[i]->item(j,0)->setBackgroundColor(Qt::red);
				}
				else VariablesList[i]->item(j,0)->setBackgroundColor(Qt::white);
			}
		}
	}
}
void StandaloneStress::ChangeType(int tpInd){
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
void StandaloneStress::RowsChanged(){
	int OldNum;
	
	for(int i=0;i<2;i++){
		int Debug=VariablesCount[i]->value();
		OldNum=VariablesList[i]->rowCount();
		if(OldNum!=VariablesCount[i]->value()) VariablesList[i]->setRowCount(VariablesCount[i]->value());
		for(;OldNum<VariablesList[i]->rowCount();OldNum++){
			VariablesList[i]->setItem(OldNum,0,new QTableWidgetItem("0"));
		}
	}
}
void StandaloneStress::Start(){
	Waterfall TempWaterfall;
	//QString Tempstring;
	BloombergVector TempVector;
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
	out >> TempVector;
	out >> TempVector;
	out >> TempVector;
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
	TempWaterfall.SetUseCall(StressToCallBox->isChecked());
	Stresser->SetStructure(TempWaterfall);
	hide();
	Stresser->RunStressTest();
}
void StandaloneStress::Finished(){
	QDir dir(PathEdit->text());
	Stresser->SaveResults(dir.absolutePath());
	if(QMessageBox::information(this,"Finished","Stress Test Finished Successfuly")==QMessageBox::Ok) close();
}
void StandaloneStress::PasteFirst(){
	const QMimeData *mimeData = QApplication::clipboard()->mimeData();
	if (mimeData->hasHtml()) {
		QString Test=mimeData->html();
		QRegExp TableCell("<td.*>(.+)</td>");
		TableCell.setMinimal(true);
		int CurrIndex=-1;
		QStringList SingleValues;
		while(true){
			CurrIndex=TableCell.indexIn(Test,CurrIndex+1);
			if(CurrIndex<0) break;
			SingleValues.append(TableCell.capturedTexts().last());
		}
		VariablesCount[0]->setValue(qMax(1,SingleValues.size()));
		if(SingleValues.size()>=1){
			VariablesList[0]->setRowCount(SingleValues.size());
			for(int i=0;i<SingleValues.size();i++){
				if(VariablesList[0]->item(i,0)) VariablesList[0]->item(0,0)->setText(SingleValues.at(i));
				else VariablesList[0]->setItem(i,0,new QTableWidgetItem(SingleValues.at(i)));
			}
		}
		else{
			VariablesList[0]->setRowCount(1);
			VariablesList[0]->item(0,0)->setText("0");
		}
	}
	else{
		VariablesList[0]->setRowCount(1);
		VariablesList[0]->item(0,0)->setText("0");
	}
}
void StandaloneStress::PasteSecond(){
	const QMimeData *mimeData = QApplication::clipboard()->mimeData();
	if (mimeData->hasHtml()) {
		QString Test=mimeData->html();
		QRegExp TableCell("<td.*>(.+)</td>");
		TableCell.setMinimal(true);
		int CurrIndex=-1;
		QStringList SingleValues;
		while(true){
			CurrIndex=TableCell.indexIn(Test,CurrIndex+1);
			if(CurrIndex<0) break;
			SingleValues.append(TableCell.capturedTexts().last());
		}
		VariablesCount[1]->setValue(qMax(1,SingleValues.size()));
		if(SingleValues.size()>=1){
			VariablesList[1]->setRowCount(SingleValues.size());
			for(int i=0;i<SingleValues.size();i++){
				if(VariablesList[1]->item(i,0)) VariablesList[1]->item(0,0)->setText(SingleValues.at(i));
				else VariablesList[1]->setItem(i,0,new QTableWidgetItem(SingleValues.at(i)));
			}
		}
		else{
			VariablesList[1]->setRowCount(1);
			VariablesList[1]->item(0,0)->setText("0");
		}
	}
	else{
		VariablesList[1]->setRowCount(1);
		VariablesList[1]->item(0,0)->setText("0");
	}
}
void StandaloneStress::BrowseFolder(){
	QDir dir(PathEdit->text());
	QString StdPath;
	if (dir.exists()) StdPath=dir.absolutePath();
	else StdPath=QString();
	QString SelectedDir = QFileDialog::getExistingDirectory(this, tr("Select Working Directory"),StdPath);
	if(!SelectedDir.isEmpty()) PathEdit->setText(SelectedDir);
	CheckAllValid();
}