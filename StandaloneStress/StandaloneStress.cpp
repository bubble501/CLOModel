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
#include <CommonFunctions.h>
#include <BloombergVector.h>
#include <MtgCalculator.h>
#include <QGroupBox>
#include <IntegerVector.h>
#include <Mortgage.h>
StandaloneStress::StandaloneStress(QWidget *parent)
	: QWidget(parent)
{
	const QString ParamNames[NumStressDimentsions] = { tr("CPR"), tr("CDR"), tr("LS"), tr("Recovery Lag"), tr("Delinquency"), tr("Delinquency Lag") };

	setWindowTitle(tr("Standalone Stress Test"));
	setWindowIcon(QIcon(":/Icons/Logo.png"));
    if (QMetaType::type("Waterfall") == QMetaType::UnknownType) {
        qRegisterMetaType<Waterfall>("Waterfall");
        qRegisterMetaTypeStreamOperators<Waterfall>("Waterfall");
    }
    if (QMetaType::type("MtgCashFlow") == QMetaType::UnknownType) {
        qRegisterMetaType<MtgCashFlow>("MtgCashFlow");
        qRegisterMetaTypeStreamOperators<MtgCashFlow>("MtgCashFlow");
    }
	Stresser=new StressTest(this);
	

	QVBoxLayout* MainLay=new QVBoxLayout(this);
	StressToCallBox=new QCheckBox(this);
	StressToCallBox->setText(tr("Stress To Call"));
	StressToCallBox->setChecked(false);
	FastStressBox = new QCheckBox(this);
	FastStressBox->setText(tr("Fast Stress"));
	FastStressBox->setChecked(true);
	
	
	QHBoxLayout* TopLay=new QHBoxLayout;
	QSpacerItem* TopSpace=new QSpacerItem(20,20,QSizePolicy::Expanding);
	TopLay->addWidget(FastStressBox);
	TopLay->addWidget(StressToCallBox);
	TopLay->addItem(TopSpace);
	QLabel* PathLabel = new QLabel(this);
	PathLabel->setText(tr("Model File:"));
	TopLay->addWidget(PathLabel);
	InPathEdit = new QLineEdit(this);
	InPathEdit->setText("");
	InPathEdit->setToolTip(tr("Path to model clom file"));
	TopLay->addWidget(InPathEdit);
	QPushButton* BrowseButton = new QPushButton(this);
	BrowseButton->setText("...");
	connect(BrowseButton, SIGNAL(clicked()), this, SLOT(BrowseModel()));
	TopLay->addWidget(BrowseButton);

	PathLabel = new QLabel(this);
	PathLabel->setText(tr("Output Folder:"));
	TopLay->addWidget(PathLabel);
	OutPathEdit = new QLineEdit(this);
	OutPathEdit->setText("");
	OutPathEdit->setToolTip(tr("Folder where results will be saved"));
	TopLay->addWidget(OutPathEdit);
	BrowseButton = new QPushButton(this);
	BrowseButton->setText("...");
	connect(BrowseButton, SIGNAL(clicked()), this, SLOT(BrowseFolder()));
	TopLay->addWidget(BrowseButton);
	MainLay->addLayout(TopLay);

	
	QGridLayout *TableLay=new QGridLayout;
	QStringList TmpStrList;
	for(int i=0;i<NumStressDimentsions;i++){
		QGroupBox* TempGroup = new QGroupBox(this);
		QVBoxLayout* GroupLay = new QVBoxLayout(TempGroup);
		TempGroup->setTitle(ParamNames[i]);
		VariablesList[i]=new QTableWidget(this);
		VariablesList[i]->setColumnCount(1);
		VariablesList[i]->setRowCount(1);
		VariablesList[i]->setItem(0,0,new QTableWidgetItem("0"));
		VariablesList[i]->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		VariablesList[i]->verticalHeader()->hide();
		VariablesList[i]->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
		VariablesList[i]->setSelectionMode(QAbstractItemView::NoSelection);
		PasteButton[i]= new QPushButton(this);
		PasteButton[i]->setText(tr("Paste from Clipboard"));
		connect(PasteButton[i], SIGNAL(clicked()), this, SLOT(PasteClipboard()));
		QHBoxLayout *SpinnsLay = new QHBoxLayout;
		VariablesCountlabel[i] = new QLabel(this);
		SpinnsLay->addWidget(VariablesCountlabel[i]);
		VariablesCount[i] = new QSpinBox(this);
		VariablesCount[i]->setMinimum(0);
		VariablesCount[i]->setMaximum(99);
		VariablesCount[i]->setValue(1);
		connect(VariablesCount[i], SIGNAL(valueChanged(int)), this, SLOT(RowsChanged(int)));
		SpinnsLay->addWidget(VariablesCount[i]);
		GroupLay->addLayout(SpinnsLay);
		GroupLay->addWidget(VariablesList[i]);
		GroupLay->addWidget(PasteButton[i]);
		TableLay->addWidget(TempGroup, i / 3, i % 3);
		VariablesCountlabel[i]->setText(tr("%1 Scenarios").arg(ParamNames[i]));
		TmpStrList.clear();
		TmpStrList.append(ParamNames[i]);
		VariablesList[i]->setHorizontalHeaderLabels(TmpStrList);
	}
	MainLay->addLayout(TableLay);

	StartButton=new QPushButton(this);
	StartButton->setText(tr("Start Stress Test"));
	QHBoxLayout *BottomLay=new QHBoxLayout;
	QSpacerItem* BottomSpace=new QSpacerItem(20,20,QSizePolicy::Expanding);
	BottomLay->addItem(BottomSpace);
	BottomLay->addWidget(StartButton);
	MainLay->addLayout(BottomLay);

	connect(InPathEdit,SIGNAL(editingFinished()),this,SLOT(CheckAllValid()),Qt::QueuedConnection);
	connect(OutPathEdit, SIGNAL(editingFinished()), this, SLOT(CheckAllValid()), Qt::QueuedConnection);
	connect(StartButton,SIGNAL(clicked()),this,SLOT(Start()));
	for(int i=0;i<2;i++){
		connect(VariablesList[i],SIGNAL(itemChanged(QTableWidgetItem *)),this,SLOT(CheckAllValid()),Qt::QueuedConnection);
		connect(VariablesCount[i],SIGNAL(valueChanged(int)),this,SLOT(RowsChanged()),Qt::QueuedConnection);
	}
	connect(Stresser, SIGNAL(AllFinished()), this, SLOT(Finished()));
	connect(Stresser, SIGNAL(FinishedWithErrors()), this, SLOT(FinishedEroors()));
	connect(Stresser, SIGNAL(ErrorsOccured()), Stresser, SLOT(StopCalculation()));
	connect(Stresser, SIGNAL(ErrorsOccured()), this, SLOT(FinishedEroors()), Qt::QueuedConnection);
	CheckAllValid();
}

void StandaloneStress::CheckAllValid(){
	StartButton->setEnabled(true);
	InPathEdit->setStyleSheet("");
	OutPathEdit->setStyleSheet("");
	InPathEdit->setToolTip(tr("Path to model clom file"));
	QDir OutFolder(OutPathEdit->text());
	if (!OutFolder.exists() || OutPathEdit->text().isEmpty()) {
		StartButton->setEnabled(false);
		OutPathEdit->setStyleSheet("background-color: red;");
	}
	QFile ModelFile(InPathEdit->text());
	if (ModelFile.exists()) {
		qint32 VersionCheck;
		bool VersionGood = false;
		if(ModelFile.open(QIODevice::ReadOnly)) {
			QDataStream VersionIn(&ModelFile);
			VersionIn >> VersionCheck;
			VersionGood = VersionCheck >= MinimumSupportedVersion && VersionCheck <= ModelVersionNumber;
			ModelFile.close();
		}
		if (!VersionGood) {
			StartButton->setEnabled(false);
			InPathEdit->setStyleSheet("background-color: red;");
			InPathEdit->setToolTip(tr("Model Version is invalid"));
		}
	}
	else{
		StartButton->setEnabled(false);
		InPathEdit->setStyleSheet("background-color: red;");
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


void StandaloneStress::RowsChanged(int NewRows) {
	int j = -1;
	for (int i = 0; i < NumStressDimentsions && j < 0; ++i) {
		if (VariablesCount[i] == sender()) j = i;
	}
	if (j < 0) return;
	int OldRowCount = VariablesList[j]->rowCount();
	if (OldRowCount > NewRows) {
		VariablesList[j]->setRowCount(NewRows);
	}
	if (OldRowCount < NewRows) {
		VariablesList[j]->setRowCount(NewRows);
		for (; OldRowCount < NewRows; ++OldRowCount)
			VariablesList[j]->setItem(OldRowCount, 0, new QTableWidgetItem("0"));
	}
	if (NewRows < 1) VariablesCount[j]->setValue(1);
}

void StandaloneStress::Start() {
	QFile ModelFile(InPathEdit->text());
	qint32 VersionCheck;
	Waterfall TempWaterfall;
	MtgCalculator TmpMtg;
	if (ModelFile.open(QIODevice::ReadOnly)) {
		QDataStream ModelIn(&ModelFile);
		ModelIn >> VersionCheck;
		if (VersionCheck >= MinimumSupportedVersion && VersionCheck <= ModelVersionNumber) {
			{QDate Junk; ModelIn >> Junk; } //LiborUpdateDate
			{bool Junk; ModelIn >> Junk; } //m_UseForwardCurve
			{bool Junk; ModelIn >> Junk; } //m_BaseCaseToCall
			TempWaterfall.SetLoadProtocolVersion(VersionCheck); ModelIn >> TempWaterfall; //Structure
			{Waterfall Junk; Junk.SetLoadProtocolVersion(VersionCheck); ModelIn >> Junk; } //CallStructure
			TmpMtg.SetLoadProtocolVersion(VersionCheck); ModelIn >> TmpMtg;
		}
		ModelFile.close();
	}
	const auto SourceLoansKeys = TmpMtg.GetResultKeys();
    for (auto i = SourceLoansKeys.constBegin(); i != SourceLoansKeys.constEnd(); ++i) {
        Stresser->AddLoan(TmpMtg.getLoan(*i));
	}
	for (int j = 0; j < VariablesList[0]->rowCount(); j++) Stresser->AddCPRscenarios(VariablesList[0]->item(j, 0)->text());
	for (int j = 0; j < VariablesList[1]->rowCount(); j++) Stresser->AddCDRscenarios(VariablesList[1]->item(j, 0)->text());
	for (int j = 0; j < VariablesList[2]->rowCount(); j++) Stresser->AddLSscenarios(VariablesList[2]->item(j, 0)->text());
	for (int j = 0; j < VariablesList[3]->rowCount(); j++) Stresser->AddRecLagScenarios(VariablesList[3]->item(j, 0)->text());
	for (int j = 0; j < VariablesList[4]->rowCount(); j++) Stresser->AddDelinqScenarios(VariablesList[4]->item(j, 0)->text());
	for (int j = 0; j < VariablesList[5]->rowCount(); j++) Stresser->AddDelinqLagScenarios(VariablesList[5]->item(j, 0)->text());

	Stresser->SetStartDate(TmpMtg.GetStartDate());
	TempWaterfall.SetUseCall(StressToCallBox->isChecked());
	Stresser->SetStructure(TempWaterfall);
	Stresser->SetUseFastVersion(FastStressBox->isChecked());
	hide();
	Stresser->RunStressTest();
}
void StandaloneStress::Finished(){
	QDir dir(OutPathEdit->text());
	Stresser->SaveResults(dir.absolutePath());
	if(QMessageBox::information(this,tr("Finished"),tr("Stress Test Finished Successfully"))==QMessageBox::Ok) qApp->quit();
}
void StandaloneStress::FinishedEroors() {
	if (QMessageBox::critical(this, tr("Finished"), tr("Errors Occurred during calculation of stress test\nNo results were saved")) == QMessageBox::Ok) qApp->quit();
}


void StandaloneStress::PasteClipboard(){
	int j = -1;
	for (int i = 0; i < NumStressDimentsions && j<0; ++i) {
		if (PasteButton[i] == sender()) j = i;
	}
	if (j < 0) return;

	const QMimeData *mimeData = QApplication::clipboard()->mimeData();
	if (mimeData->hasHtml()) {
		QString Test=mimeData->html();
		const QRegularExpression TableCell("<td.*>(.+?)</td>");
        Q_ASSERT(TableCell.isValid());
		QStringList SingleValues;
        for (auto i = TableCell.globalMatch(Test); i.hasNext();){
            SingleValues.append(i.next().captured(1));
        }
		if(SingleValues.size()>=1){
			VariablesList[j]->setRowCount(SingleValues.size());
			for(int i=0;i<SingleValues.size();i++){
				if(VariablesList[j]->item(i,0)) VariablesList[j]->item(i,0)->setText(SingleValues.at(i));
				else VariablesList[j]->setItem(i,0,new QTableWidgetItem(SingleValues.at(i)));
			}
		}
		else{
			VariablesList[j]->setRowCount(1);
			VariablesList[j]->item(0,0)->setText("0");
		}
		VariablesCount[j]->setValue(qMax(1, SingleValues.size()));
	}
	else{
		VariablesList[j]->setRowCount(1);
		VariablesList[j]->item(0,0)->setText("0");
		VariablesCount[j]->setValue(1);
	}
}

void StandaloneStress::BrowseModel(){
	QDir dir(InPathEdit->text());
	QString StdPath;
	if (dir.exists() && !InPathEdit->text().isEmpty()) StdPath = dir.absolutePath();
	else {
        StdPath = GetFromConfig("Folders", "UnifiedResultsFolder");
	}
	QString SelectedDir = QFileDialog::getOpenFileName(this, tr("Select Model File"), StdPath, tr("CLO Model Files (*.clom)"));
	if(!SelectedDir.isEmpty()) InPathEdit->setText(SelectedDir);
	CheckAllValid();
}
void StandaloneStress::BrowseFolder() {
	QDir dir(OutPathEdit->text());
	QString StdPath;
	if (dir.exists() && !OutPathEdit->text().isEmpty()) StdPath = dir.absolutePath();
	else {
		StdPath = QString();
	}
	QString SelectedDir = QFileDialog::getExistingDirectory(this, tr("Select Output Folder"), StdPath);
	if (!SelectedDir.isEmpty()) OutPathEdit->setText(SelectedDir);
	CheckAllValid();
}