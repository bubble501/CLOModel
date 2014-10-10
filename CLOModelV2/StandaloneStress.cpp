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
#include "MtgCalculator.h"
#include <QSettings>
StandaloneStress::StandaloneStress(QWidget *parent)
	: QWidget(parent)
{
	setWindowTitle("Standalone Stress Test");
	setWindowIcon(QIcon(":/Icons/Logo.png"));
	RegisterAsMetaType<Waterfall>();
	RegisterAsMetaType<MtgCashFlow>();
	Stresser=new StressTest(this);
	connect(Stresser,SIGNAL(AllFinished()),this,SLOT(Finished()));

	QVBoxLayout* MainLay=new QVBoxLayout(this);
	StressToCallBox=new QCheckBox(this);
	StressToCallBox->setText("Stress To Call");
	StressToCallBox->setChecked(false);
	FastStressBox = new QCheckBox(this);
	FastStressBox->setText("Fast Stress");
	FastStressBox->setChecked(true);
	
	
	QHBoxLayout* TopLay=new QHBoxLayout;
	QSpacerItem* TopSpace=new QSpacerItem(20,20,QSizePolicy::Expanding);
	TopLay->addWidget(FastStressBox);
	TopLay->addWidget(StressToCallBox);
	TopLay->addItem(TopSpace);
	QLabel* PathLabel = new QLabel(this);
	PathLabel->setText("Model File:");
	TopLay->addWidget(PathLabel);
	PathEdit = new QLineEdit(this);
	PathEdit->setText("");
	PathEdit->setToolTip("Path to model clom file");
	TopLay->addWidget(PathEdit);
	QPushButton* BrowseButton = new QPushButton(this);
	BrowseButton->setText("...");
	connect(BrowseButton, SIGNAL(clicked()), this, SLOT(BrowseFolder()));
	TopLay->addWidget(BrowseButton);
	MainLay->addLayout(TopLay);

	
	QGridLayout *TableLay=new QGridLayout;
	for(int i=0;i<NumStressDimentsions;i++){
		VariablesList[i]=new QTableWidget(this);
		VariablesList[i]->setColumnCount(1);
		VariablesList[i]->setRowCount(1);
		VariablesList[i]->setItem(0,0,new QTableWidgetItem("0"));
		VariablesList[i]->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		VariablesList[i]->verticalHeader()->hide();
		VariablesList[i]->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
		VariablesList[i]->setSelectionMode(QAbstractItemView::NoSelection);
		TableLay->addWidget(VariablesList[i], 1 + (3 * (i / (NumStressDimentsions / 2))), i % (NumStressDimentsions / 2));
		PasteButton[i]= new QPushButton(this);
		PasteButton[i]->setText("Paste from Clipboard");
		TableLay->addWidget(PasteButton[i], 2 + (3 * (i / (NumStressDimentsions / 2))), i % (NumStressDimentsions / 2));
		QHBoxLayout *SpinnsLay = new QHBoxLayout;
		VariablesCountlabel[i] = new QLabel(this);
		SpinnsLay->addWidget(VariablesCountlabel[i]);
		VariablesCount[i] = new QSpinBox(this);
		VariablesCount[i]->setMinimum(1);
		VariablesCount[i]->setMaximum(99);
		VariablesCount[i]->setValue(1);
		SpinnsLay->addWidget(VariablesCount[i]);
		TableLay->addLayout(SpinnsLay, 3 * (i / (NumStressDimentsions / 2)), i % (NumStressDimentsions / 2));
	}
	VariablesCountlabel[0]->setText("CPR Scenarios");
	VariablesCountlabel[1]->setText("CDR Scenarios");
	VariablesCountlabel[2]->setText("LS Scenarios");
	VariablesCountlabel[3]->setText("Recovery Lag Scenarios");
	VariablesCountlabel[4]->setText("Delinquency Scenarios");
	VariablesCountlabel[5]->setText("Delinquency Lag Scenarios");
	{
		QStringList TmpStrList;
		TmpStrList << "CPR";
		VariablesList[0]->setHorizontalHeaderLabels(TmpStrList);
		TmpStrList<< "CDR";
		VariablesList[1]->setHorizontalHeaderLabels(TmpStrList);
		TmpStrList.clear();
		TmpStrList<< "LS";
		VariablesList[2]->setHorizontalHeaderLabels(TmpStrList);
		TmpStrList.clear();
		TmpStrList << "Recovery Lag";
		VariablesList[3]->setHorizontalHeaderLabels(TmpStrList);
		TmpStrList.clear();
		TmpStrList << "Delinquency";
		VariablesList[4]->setHorizontalHeaderLabels(TmpStrList);
		TmpStrList.clear();
		TmpStrList << "Delinquency Lag";
		VariablesList[5]->setHorizontalHeaderLabels(TmpStrList);
	}
	MainLay->addLayout(TableLay);

	StartButton=new QPushButton(this);
	StartButton->setText("Start Stress Test");
	QHBoxLayout *BottomLay=new QHBoxLayout;
	QSpacerItem* BottomSpace=new QSpacerItem(20,20,QSizePolicy::Expanding);
	BottomLay->addItem(BottomSpace);
	BottomLay->addWidget(StartButton);
	MainLay->addLayout(BottomLay);

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
	PathEdit->setStyleSheet("");
	QDir dir(PathEdit->text());
	if(!dir.exists() || !QFile::exists(dir.absolutePath()+"/.Loans.clp") || !QFile::exists(dir.absolutePath()+"/.BaseCase.clo")){
		StartButton->setEnabled(false);
		PathEdit->setStyleSheet("background-color: red;");
	}
	for(int i=0;i<NumStressDimentsions;i++){
		for(int j=0;j<VariablesList[i]->rowCount();j++){
			if(VariablesList[i]->item(j,0)){
				if (i<3 || i==4) {
					if (BloombergVector(VariablesList[i]->item(j, 0)->text()).IsEmpty(0.0, 1.0)) {
						StartButton->setEnabled(false);
						VariablesList[i]->item(j, 0)->setBackgroundColor(Qt::red);
					}
					else VariablesList[i]->item(j, 0)->setBackgroundColor(Qt::white);
				}
				else {
					if (IntegerVector(VariablesList[i]->item(j, 0)->text()).IsEmpty(0)) {
						StartButton->setEnabled(false);
						VariablesList[i]->item(j, 0)->setBackgroundColor(Qt::red);
					}
					else VariablesList[i]->item(j, 0)->setBackgroundColor(Qt::white);
				}
				
			}
		}
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
	QDate StartDate;
	QDir dir(PathEdit->text());
	{
		QFile file(dir.absolutePath()+"/.Loans.clp");
		file.open(QIODevice::ReadOnly);
		qint32 VersionChecker;
		QDataStream out(&file);
		out.setVersion(QDataStream::Qt_5_3);
		out >> VersionChecker;
		if (VersionChecker<MinimumSupportedVersion || VersionChecker>ModelVersionNumber) 
			return;
		{
			MtgCalculator TmpMtg;
			TmpMtg.SetLoadProtocolVersion(VersionChecker);
			out >> TmpMtg;
			StartDate = TmpMtg.GetStartDate();
			for (auto i = TmpMtg.GetLoans().constBegin(); i != TmpMtg.GetLoans().constEnd(); ++i) {
				Stresser->AddLoan(*(i.value()));
			}
		}
		file.close();
	}
	{
		QFile file(dir.absolutePath() + "/.BaseCase.clo");
		file.open(QIODevice::ReadOnly);
		qint32 VersionChecker;
		QDataStream out(&file);
		out.setVersion(QDataStream::Qt_5_3);
		out >> VersionChecker;
		if (VersionChecker != qint32(ModelVersionNumber)) return;
		{bool Junk; out >> Junk; }
		out >> TempWaterfall;
		file.close();
	}




	Stresser->SetStartDate(StartDate);
	TempWaterfall.SetUseCall(StressToCallBox->isChecked());
	Stresser->SetStructure(TempWaterfall);
	Stresser->SetUseFastVersion(FastStressBox->isChecked());
	hide();
#ifdef _DEBUG
	//Stresser->UseMultithread(false);
#endif // _DEBUG
	Stresser->RunStressTest();
}
void StandaloneStress::Finished(){
	QDir dir(PathEdit->text());
	Stresser->SaveResults(dir.absolutePath());
	if(QMessageBox::information(this,"Finished","Stress Test Finished Successfully")==QMessageBox::Ok) close();
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
	if (dir.exists() && !PathEdit->text().isEmpty()) StdPath = dir.absolutePath();
	else {
		QSettings ConfigIni(":/Configs/GlobalConfigs.ini", QSettings::IniFormat);
		ConfigIni.beginGroup("Folders");
		StdPath=ConfigIni.value("UnifiedResultsFolder", "\\\\synserver2\\Company Share\\24AM\\Monitoring\\Model Results").toString();
	}
	QString SelectedDir = QFileDialog::getOpenFileName(this, tr("Select Model File"), StdPath, tr("CLO Model Files (*.clom)"));
	if(!SelectedDir.isEmpty()) PathEdit->setText(SelectedDir);
	CheckAllValid();
}