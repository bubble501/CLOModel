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
	emit Closing();
	event->accept();
}

StressViewer::StressViewer(QWidget* parent/*=0*/)
	:QWidget(parent)
#ifdef USE_SOFT_COLOUR
	, GreenFill(196,215,155)
	, YellowFill(255,255,204)
	, RedFill(192, 80, 70)
	, GrayFill(191, 191,191)
#else
	, GreenFill(Qt::green)
	, YellowFill(Qt::yellow)
	, RedFill(Qt::red)
	, GrayFill(Qt::gray)
#endif
	, PriceToBeSet(false)
{
	GradientBase.setEasingCurve(QEasingCurve::Linear);
	GradientBase.setDuration(100.0);
	GradientBase.setKeyValueAt(0.0, QColor(YellowFill));
	GradientBase.setKeyValueAt(1.0, QColor(GreenFill));
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
	PriceLabel = new QLabel(this);
	PriceLabel->setText(tr("Price:"));
	TopLay->addWidget(PriceLabel);
	PriceLabel->hide();
	PriceSpin = new QDoubleSpinBox(this);
	PriceSpin->setMinimum(0.01);
	PriceSpin->setMaximum(300.0);
	PriceSpin->setDecimals(2);
	PriceSpin->setSingleStep(0.1);
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
		ConstParCombo[ParamIter]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
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
	StressTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	CentreLay->addWidget(StressTable, 1, 1);
	QHBoxLayout* TableToplay = new QHBoxLayout;
	HSpace = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Preferred);
	TableToplay->addItem(HSpace);
	TableToplay->addWidget(VariableParCombo[1]);
	HSpace = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Preferred);
	TableToplay->addItem(HSpace);
	CentreLay->addLayout(TableToplay, 0, 1);
	MainLay->addWidget(ConstGroup);

	connect(TrancheCombo, SIGNAL(currentTextChanged(QString)), this, SLOT(SetPriceChange()));
	connect(TrancheCombo, SIGNAL(currentTextChanged(QString)), this, SLOT(UpdateTable()), Qt::QueuedConnection);
	connect(DisplayValuesCombo, SIGNAL(currentTextChanged(QString)), this, SLOT(UpdateTable()), Qt::QueuedConnection);
	connect(PriceSpin, SIGNAL(valueChanged(double)), this, SLOT(UpdateTable()), Qt::QueuedConnection);
	connect(VariableParCombo[0], SIGNAL(currentTextChanged(QString)), this, SLOT(UpdateCombos()));
	connect(VariableParCombo[0], SIGNAL(currentTextChanged(QString)), this, SLOT(UpdateTable()), Qt::QueuedConnection);
	connect(VariableParCombo[1], SIGNAL(currentTextChanged(QString)), this, SLOT(UpdateCombos()));
	connect(VariableParCombo[1], SIGNAL(currentTextChanged(QString)), this, SLOT(UpdateTable()), Qt::QueuedConnection);
	connect(StressTable, &QTableWidget::cellDoubleClicked, this, &StressViewer::SetStressLevel, Qt::QueuedConnection);
	for (int j = 0; j < NumStressDimentsions - 2; ++j) connect(ConstParCombo[j], SIGNAL(currentTextChanged(QString)), this, SLOT(UpdateTable()), Qt::QueuedConnection);

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
	for (int i = 0; i < NumStressDimentsions; ++i) std::sort(AvailableAssumptions[i].begin(), AvailableAssumptions[i].end(), [](const QString& a, const QString& b) {return BloombergVector(a).GetValue(0) < BloombergVector(b).GetValue(0); });
	TrancheCombo->clear();
	for (int i = 0; i < StressTarget->GetStructure().GetTranchesCount(); ++i) {
		TrancheCombo->addItem(StressTarget->GetStructure().GetTranche(i)->GetTrancheName(), i);
	}
	UpdateCombos();
	UpdateTable();
	return true;
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

void StressViewer::UpdateCombos() {
	StressTable->setRowCount(AvailableAssumptions[VariableParCombo[0]->currentData().toInt()].size());
	StressTable->setColumnCount(AvailableAssumptions[VariableParCombo[1]->currentData().toInt()].size());
	StressTable->setVerticalHeaderLabels(AvailableAssumptions[VariableParCombo[0]->currentData().toInt()]);
	StressTable->setHorizontalHeaderLabels(AvailableAssumptions[VariableParCombo[1]->currentData().toInt()]);
	int ParamIter = 0;
	for (int j = 0; j < NumStressDimentsions; ++j) {
		if (VariableParCombo[0]->currentData().toInt() != j) {
			if (VariableParCombo[1]->findData(j) < 0) {
				VariableParCombo[1]->addItem(ParamNames[j], j);
			}
		}
		else VariableParCombo[1]->removeItem(VariableParCombo[1]->findData(j));
		if (VariableParCombo[1]->currentData().toInt() != j) {
			if (VariableParCombo[0]->findData(j) < 0) {
				VariableParCombo[0]->addItem(ParamNames[j], j);
			}
		}
		else VariableParCombo[0]->removeItem(VariableParCombo[0]->findData(j));
		if (VariableParCombo[0]->currentData().toInt() == j || VariableParCombo[1]->currentData().toInt() == j) continue;
		ConstParCombo[ParamIter]->clear();
		ConstParLabel[ParamIter]->setText(ParamNames[j]);
		for (auto i = AvailableAssumptions[j].constBegin(); i != AvailableAssumptions[j].constEnd(); ++i)
			ConstParCombo[ParamIter]->addItem(*i);
		ParamIter++;
	}
	for (int j = 0; j < 2; ++j) VariableParCombo[j]->model()->sort(0);
}

void StressViewer::UpdateTable() {
	QTableWidgetItem* CurrentItem=nullptr;
	double CurrentData;
	double MaxDisc, MinDisc;
	bool Minset = false;
	for (int i = 0; i < StressTable->rowCount(); ++i) {
		for (int j = 0; j < StressTable->columnCount(); ++j) {
			CurrentItem = nullptr;
			const QSharedPointer<Waterfall> CurrentRes = StressTarget->GetResults().value(AssembleAssumption(i, j));
			if (!CurrentRes) {
				CurrentItem=new QTableWidgetItem("N/A");
				CurrentItem->setData(Qt::UserRole, "N/A");
				CurrentItem->setBackgroundColor(GrayFill);
			}
			else {
				const Tranche* const CurrTran = CurrentRes->GetTranche(TrancheCombo->currentText());
				if (!CurrTran) {
					CurrentItem = new QTableWidgetItem("N/A");
					CurrentItem->setData(Qt::UserRole, "N/A");
					CurrentItem->setBackgroundColor(GrayFill);
				}
				else {
					switch (DisplayValuesCombo->currentData().toInt()) {
					case 0:
						CurrentData = CurrTran->GetLossRate();
						CurrentItem = new QTableWidgetItem(Commarize(CurrentData*100.0, 2) + '%');
						CurrentItem->setData(Qt::UserRole, CurrentData);
						if (CurrentData >= 1.0) CurrentItem->setBackgroundColor(RedFill);
						else if (CurrentData>0.0) CurrentItem->setBackgroundColor(YellowFill);
						else CurrentItem->setBackgroundColor(GreenFill);
						break;
					case 1:
						if (PriceToBeSet) {PriceSpin->setValue(CurrTran->GetPrice()); PriceToBeSet = false;}
						CurrentData = CurrTran->GetDiscountMargin(PriceSpin->value());
						if ((i == 0 && j == 0) || CurrentData>MaxDisc) MaxDisc = CurrentData;
						if ((!Minset || CurrentData<MinDisc) && CurrentData >= 1.0) { MinDisc = CurrentData; Minset = true; }
						CurrentItem = new QTableWidgetItem(Commarize(CurrentData, 0));
						CurrentItem->setData(Qt::UserRole, CurrentData);
						break;
					case 2:
						CurrentData = CurrTran->GetWALife(CurrTran->GetSettlementDate());
						if ((i == 0 && j == 0) || CurrentData > MaxDisc) MaxDisc = CurrentData;
						if ((!Minset || CurrentData < MinDisc) && CurrentData>0) { MinDisc = CurrentData; Minset = true; }
						CurrentItem = new QTableWidgetItem(Commarize(CurrentData, 2));
						CurrentItem->setData(Qt::UserRole, CurrentData);
						break;
					default:
						CurrentItem = new QTableWidgetItem("N/A");
						CurrentItem->setData(Qt::UserRole, "N/A");
						CurrentItem->setBackgroundColor(GrayFill);
					}
				}
			}
			StressTable->setItem(i, j, CurrentItem);
		}
	}
	for (int i = 0; i < StressTable->rowCount(); i++) {
		for (int j = 0; j < StressTable->columnCount(); j++) {
			CurrentItem = StressTable->item(i, j);
			if (CurrentItem->data(Qt::UserRole).toString() == "N/A") continue;
			switch (DisplayValuesCombo->currentData().toInt()) {
			case 1:
				CurrentData = CurrentItem->data(Qt::UserRole).toDouble();
				if (CurrentData < 1.0) CurrentItem->setBackgroundColor(RedFill);
				else {
					if (MaxDisc - MinDisc < 1.0) GradientBase.setCurrentTime(100.0);
					else GradientBase.setCurrentTime(100.0*(CurrentData - MinDisc) / (MaxDisc - MinDisc));
					CurrentItem->setBackgroundColor(GradientBase.currentValue().value<QColor>());
				}
				break;

			case 2:
				CurrentData = CurrentItem->data(Qt::UserRole).toDouble();
				if (CurrentData <= 0.0) CurrentItem->setBackgroundColor(RedFill);
				else {
					if (MaxDisc - MinDisc < 1.0) GradientBase.setCurrentTime(100.0);
					else GradientBase.setCurrentTime(100.0 - (100.0*(CurrentData - MinDisc) / (MaxDisc - MinDisc)));
					CurrentItem->setBackgroundColor(GradientBase.currentValue().value<QColor>());
				}
				break;
			default:
				break;
			}
		}
	}
	if (DisplayValuesCombo->currentData().toInt() == 1) {
		PriceSpin->show();
		PriceLabel->show();
	}
	else {
		PriceSpin->hide();
		PriceLabel->hide();
	}
}
void StressViewer::SetStressLevel(int r, int c) {
	QSharedPointer<Waterfall> TempWat = StressTarget->GetResults().value(AssembleAssumption(r, c), QSharedPointer<Waterfall>(nullptr));
	if (TempWat) emit StressLevelChanged(*TempWat);
}
