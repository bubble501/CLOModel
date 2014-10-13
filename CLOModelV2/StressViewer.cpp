#include "StressViewer.h"
#include <QCloseEvent>
#include <QIcon>
#include <QLabel>
#include <QComboBox>
#include <QHBoxLayout>
#include <QDoubleSpinBox>
#include <QTableView>
#include <QHeaderView>
#include <QGroupBox>
#include <QTableWidget>
#include <QTableWidgetItem>
void StressViewer::closeEvent(QCloseEvent *event){
	//emit Closing();
	event->accept();
}

StressViewer::StressViewer(QWidget* parent/*=0*/)
	:QWidget(parent)
{
	ParamNames[0] = tr("CPR");
	ParamNames[1] = tr("CDR");
	ParamNames[2] = tr("LS");
	ParamNames[3] = tr("Recovery Lag");
	ParamNames[4] = tr("Delinquency");
	ParamNames[5] = tr("Delinquency Lag");
	setWindowIcon(QIcon(":/Icons/Logo.png"));
	setWindowTitle("CLO Stress Test Viewer");
	setMinimumSize(1024, 768);
	RegisterAsMetaType<Waterfall>();
	StressTarget = new StressTest(this);

	QVBoxLayout* MainLay = new QVBoxLayout(this);
	QGroupBox* ConstGroup = new QGroupBox(this);
	ConstGroup->setTitle(tr("Deal Parameters"));
	QSpacerItem* HSpace = new QSpacerItem(20, 20, QSizePolicy::Expanding,QSizePolicy::Preferred);
	QHBoxLayout* TopLay = new QHBoxLayout(ConstGroup);
	QLabel* TempLabel = new QLabel(this);
	TempLabel->setText(tr("Tranche:"));
	TopLay->addWidget(TempLabel);
	TrancheCombo = new QComboBox(this);
	TopLay->addWidget(TrancheCombo);
	TopLay->addItem(HSpace);
	TempLabel = new QLabel(this);
	TempLabel->setText(tr("Price:"));
	TopLay->addWidget(TempLabel);
	TempLabel->hide();
	PriceSpin = new QDoubleSpinBox(this);
	PriceSpin->setValue(100.0);
	TopLay->addWidget(PriceSpin);
	PriceSpin->hide();
	HSpace = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Preferred);
	TopLay->addItem(HSpace);
	TempLabel = new QLabel(this);
	TempLabel->setText(tr("Display:"));
	TopLay->addWidget(TempLabel);
	DisplayValuesCombo = new QComboBox(this);
	DisplayValuesCombo->addItem(tr("Loss Rate"), 0);
	DisplayValuesCombo->addItem(tr("DM"), 1);
	DisplayValuesCombo->addItem(tr("WAL"), 2);
	TopLay->addWidget(DisplayValuesCombo);
	MainLay->addWidget(ConstGroup);

	ConstGroup = new QGroupBox(this);
	ConstGroup->setTitle(tr("Other Variables"));
	TopLay = new QHBoxLayout(ConstGroup);
	int ParamIter = 0;
	for (int j = 0; j < NumStressDimentsions; ++j) {
		if (j == 1 || j == 2)continue;
		/*if (j > 0) {
			HSpace = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Preferred);
			TopLay->addItem(HSpace);
		}*/
		ConstParLabel[ParamIter] = new QLabel(this);
		ConstParLabel[ParamIter]->setText(ParamNames[j]);
		ConstParLabel[ParamIter]->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		ConstParCombo[ParamIter] = new QComboBox(this);
		TopLay->addWidget(ConstParLabel[ParamIter]);
		TopLay->addWidget(ConstParCombo[ParamIter]);
		ParamIter++;
	}
	MainLay->addWidget(ConstGroup);

	
	

	for (int j = 0; j < 2;++j){
		VariableParCombo[j] = new QComboBox(this);
		VariableParCombo[j]->setEditable(false);
		for (int i = 0; i < NumStressDimentsions; ++i) {
			if (j==0 && i==2) continue;
			if (j == 1 && i == 1) continue;
			VariableParCombo[j]->addItem(ParamNames[i], i);
		}
		VariableParCombo[j]->setCurrentIndex(1);
	}
	ConstGroup = new QGroupBox(this);
	ConstGroup->setTitle(tr("Stress Test"));
	QGridLayout* CentreLay = new QGridLayout(ConstGroup);
	QVBoxLayout* TableLeftLay = new QVBoxLayout;
	HSpace = new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding);
	TableLeftLay->addItem(HSpace);
	TableLeftLay->addWidget(VariableParCombo[0]);
	HSpace = new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding);
	TableLeftLay->addItem(HSpace);
	CentreLay->addLayout(TableLeftLay, 1, 0);
	StressTable = new QTableWidget(this);
	StressTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	StressTable->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	StressTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	StressTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	StressTable->setSelectionMode(QAbstractItemView::SingleSelection);
	CentreLay->addWidget(StressTable, 1, 1);
	QHBoxLayout* TableToplay = new QHBoxLayout;
	HSpace = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Preferred);
	TableToplay->addItem(HSpace);
	TableToplay->addWidget(VariableParCombo[1]);
	HSpace = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Preferred);
	TableToplay->addItem(HSpace);
	CentreLay->addLayout(TableToplay, 0, 1);
	MainLay->addWidget(ConstGroup);

	connect(TrancheCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(TrancheTargetChanged(int)));
	connect(VariableParCombo[0], SIGNAL(currentIndexChanged(int)), this, SLOT(RowColComboChanged()));
	connect(VariableParCombo[1], SIGNAL(currentIndexChanged(int)), this, SLOT(RowColComboChanged()));

}

QString StressViewer::GetFirstName() const {
	return (*(StressTarget->GetResults().begin()))->GetDealName();
}

bool StressViewer::LoadStress(const QString& filename) {
	StressTarget->ResetLoans();
	StressTarget->ResetResults();
	StressTarget->ResetScenarios();
	if (!StressTarget->LoadResultsFromFile(filename)) return false;
	AvailableAssumptions[AssCPR] = StressTarget->GetCPRscenarios().toList();
	AvailableAssumptions[AssCDR] = StressTarget->GetCDRscenarios().toList();
	AvailableAssumptions[AssLS] = StressTarget->GetLSscenarios().toList();
	AvailableAssumptions[AssRecLag] = StressTarget->GetRecLagScenarios().toList();
	AvailableAssumptions[AssDelinq] = StressTarget->GetDelinqScenarios().toList();
	AvailableAssumptions[AssDelinqLag] = StressTarget->GetDelinqLagScenarios().toList();
	TrancheCombo->clear();
	for (int i = 0; i < StressTarget->GetStructure().GetTranchesCount(); ++i) {
		TrancheCombo->addItem(StressTarget->GetStructure().GetTranche(i)->GetTrancheName(), i);
	}
	StressTable->setRowCount(AvailableAssumptions[VariableParCombo[0]->currentData().toInt()].size());
	StressTable->setColumnCount(AvailableAssumptions[VariableParCombo[1]->currentData().toInt()].size());
	StressTable->setVerticalHeaderLabels(AvailableAssumptions[VariableParCombo[0]->currentData().toInt()]);
	StressTable->setHorizontalHeaderLabels(AvailableAssumptions[VariableParCombo[1]->currentData().toInt()]);
	int ParamIter = 0;
	for (int j = 0; j < NumStressDimentsions; ++j) {
		if (VariableParCombo[0]->currentData().toInt() == j || VariableParCombo[1]->currentData().toInt() == j) continue;
		ConstParCombo[ParamIter]->clear();
		ConstParLabel[ParamIter]->setText(ParamNames[j]);
		for (auto i = AvailableAssumptions[j].constBegin(); i != AvailableAssumptions[j].constEnd();++i)
			ConstParCombo[ParamIter]->addItem(*i);
		ParamIter++;
	}
	for (int i = 0; i < StressTable->rowCount(); ++i) {
		for (int j = 0; j < StressTable->columnCount(); ++j) {
			QSharedPointer<Waterfall> CurrentRes = StressTarget->GetResults().value(AssembleAssumption(i, j));
			if (!CurrentRes) {
				StressTable->setItem(i, j, new QTableWidgetItem("N/A"));
				continue;
			}
			const Tranche* CurrTran = CurrentRes->GetTranche(TrancheCombo->currentText());
			if (!CurrTran) {
				StressTable->setItem(i, j, new QTableWidgetItem("N/A"));
				continue;
			}
			switch (DisplayValuesCombo->currentData().toInt()) {
			case 0:
				StressTable->setItem(i, j, new QTableWidgetItem(Commarize(
					CurrTran->GetLossRate()
					*100.0, 2) + '%'));
				break;
			case 1:
				StressTable->setItem(i, j, new QTableWidgetItem(Commarize(
					CurrTran->GetDiscountMargin(PriceSpin->value())
					, 0)));
				break;
			case 2:
				StressTable->setItem(i, j, new QTableWidgetItem(Commarize(
					CurrTran->GetWALife(CurrTran->GetSettlementDate())
					, 0)));
				break;
			default:
				StressTable->setItem(i, j, new QTableWidgetItem("N/A"));
			}
		}
	}
	return true;
}

void StressViewer::RowColComboChanged() {
	
	int ParamIter=0;
	StressTable->setRowCount(AvailableAssumptions[VariableParCombo[0]->currentData().toInt()].size());
	StressTable->setColumnCount(AvailableAssumptions[VariableParCombo[1]->currentData().toInt()].size());
	StressTable->setVerticalHeaderLabels(AvailableAssumptions[VariableParCombo[0]->currentData().toInt()]);
	StressTable->setHorizontalHeaderLabels(AvailableAssumptions[VariableParCombo[1]->currentData().toInt()]);
	for (int j = 0; j < NumStressDimentsions; ++j) {
		if (VariableParCombo[0]->currentData().toInt() == j || VariableParCombo[1]->currentData().toInt() == j) continue;
		ConstParLabel[ParamIter]->setText(ParamNames[j]);
		ConstParCombo[ParamIter]->clear();
		foreach(const QString& a, AvailableAssumptions[j])
			ConstParCombo[ParamIter]->addItem(a);
		ParamIter++;
	}
}

AssumptionSet StressViewer::AssembleAssumption(int r, int c) const {
	AssumptionSet Result;
	int ConstIndex = 0;

	if (VariableParCombo[0]->currentData().toInt() == AssCPR) Result.SetCPRscenario(AvailableAssumptions[AssCPR].at(r));
	else if (VariableParCombo[1]->currentData().toInt() == AssCPR) Result.SetCPRscenario(AvailableAssumptions[AssCPR].at(c));
	else Result.SetCPRscenario(ConstParCombo[ConstIndex++]->currentText());
	if (VariableParCombo[0]->currentData().toInt() == AssCDR) Result.SetCDRscenario(AvailableAssumptions[AssCDR].at(r));
	else if (VariableParCombo[1]->currentData().toInt() == AssCDR) Result.SetCDRscenario(AvailableAssumptions[AssCDR].at(c));
	else Result.SetCDRscenario(ConstParCombo[ConstIndex++]->currentText());
	if (VariableParCombo[0]->currentData().toInt() == AssLS) Result.SetLSscenario(AvailableAssumptions[AssLS].at(r));
	else if (VariableParCombo[1]->currentData().toInt() == AssLS) Result.SetLSscenario(AvailableAssumptions[AssLS].at(c));
	else Result.SetLSscenario(ConstParCombo[ConstIndex++]->currentText());
	if (VariableParCombo[0]->currentData().toInt() == AssRecLag) Result.SetRecLagScenario(AvailableAssumptions[AssRecLag].at(r));
	else if (VariableParCombo[1]->currentData().toInt() == AssRecLag) Result.SetRecLagScenario(AvailableAssumptions[AssRecLag].at(c));
	else Result.SetRecLagScenario(ConstParCombo[ConstIndex++]->currentText());
	if (VariableParCombo[0]->currentData().toInt() == AssDelinq) Result.SetDelinqScenario(AvailableAssumptions[AssDelinq].at(r));
	else if (VariableParCombo[1]->currentData().toInt() == AssDelinq) Result.SetDelinqScenario(AvailableAssumptions[AssDelinq].at(c));
	else Result.SetDelinqScenario(ConstParCombo[ConstIndex++]->currentText());
	if (VariableParCombo[0]->currentData().toInt() == AssDelinqLag) Result.SetDelinqLagScenario(AvailableAssumptions[AssDelinqLag].at(r));
	else if (VariableParCombo[1]->currentData().toInt() == AssDelinqLag) Result.SetDelinqLagScenario(AvailableAssumptions[AssDelinqLag].at(c));
	else Result.SetDelinqLagScenario(ConstParCombo[ConstIndex++]->currentText());

	return Result;
}
