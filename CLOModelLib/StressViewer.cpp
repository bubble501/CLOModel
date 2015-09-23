#include "StressViewer.h"
#include "Private/StressViewer_p.h"
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
#include "Private/InternalItems.h"
#include "Tranche.h"
#include "AssumptionSet.h"
void StressViewer::closeEvent(QCloseEvent *event){
	emit Closing();
	event->accept();
}
StressViewerPrivate::StressViewerPrivate(StressViewer *q)
    :q_ptr(q)
#ifdef USE_SOFT_COLOUR
    , GreenFill(196, 215, 155)
    , YellowFill(255, 255, 204)
    , RedFill(192, 80, 70)
    , GrayFill(191, 191, 191)
#else
    , GreenFill(Qt::green)
    , YellowFill(Qt::yellow)
    , RedFill(Qt::red)
    , GrayFill(Qt::gray)
#endif
    , PriceToBeSet(false)
{}
StressViewer::StressViewer( QWidget* parent)
    :StressViewer(new StressViewerPrivate(this), parent)
{}
StressViewer::StressViewer(StressViewerPrivate* d, QWidget* parent)
	:QWidget(parent)
    , d_ptr(d)
{
	d->GradientBase.setEasingCurve(QEasingCurve::Linear);
    d->GradientBase.setDuration(100.0);
    d->GradientBase.setKeyValueAt(0.0, QColor(d->YellowFill));
    d->GradientBase.setKeyValueAt(1.0, QColor(d->GreenFill));
    d->ParamNames[0] = tr("CPR");
    d->ParamNames[1] = tr("CDR");
    d->ParamNames[2] = tr("LS");
    d->ParamNames[3] = tr("Recovery Lag");
    d->ParamNames[4] = tr("Delinquency");
    d->ParamNames[5] = tr("Delinquency Lag");
	setWindowIcon(QIcon(":/Icons/Logo.png"));
	setWindowTitle("CLO Stress Test Viewer");
	setMinimumSize(1024, 768);
    RegisterAsMetaType(Waterfall);
    d->StressTarget = new StressTest(this);

	QVBoxLayout* MainLay = new QVBoxLayout(this);
	QGroupBox* ConstGroup = new QGroupBox(this);
	ConstGroup->setTitle(tr("Deal Parameters"));
	QSpacerItem* HSpace = new QSpacerItem(20, 20, QSizePolicy::Expanding,QSizePolicy::Preferred);
	QHBoxLayout* TopLay = new QHBoxLayout(ConstGroup);
	QLabel* TempLabel = new QLabel(this);
	TempLabel->setText(tr("Tranche:"));
	TopLay->addWidget(TempLabel);
    d->TrancheCombo = new QComboBox(this);
    TopLay->addWidget(d->TrancheCombo);
	TopLay->addItem(HSpace);
    d->PriceLabel = new QLabel(this);
    d->PriceLabel->setText(tr("Price:"));
    TopLay->addWidget(d->PriceLabel);
    d->PriceLabel->hide();
    d->PriceSpin = new QDoubleSpinBox(this);
    d->PriceSpin->setMinimum(0.01);
    d->PriceSpin->setMaximum(300.0);
    d->PriceSpin->setDecimals(2);
    d->PriceSpin->setSingleStep(0.1);
    d->PriceSpin->setValue(100.0);
    TopLay->addWidget(d->PriceSpin);
    d->PriceSpin->hide();
	HSpace = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Preferred);
	TopLay->addItem(HSpace);
	TempLabel = new QLabel(this);
	TempLabel->setText(tr("Display:"));
	TopLay->addWidget(TempLabel);
    d->DisplayValuesCombo = new QComboBox(this);
    d->DisplayValuesCombo->addItem(tr("Loss Rate"), 0);
    d->DisplayValuesCombo->addItem(tr("DM"), 1);
    d->DisplayValuesCombo->addItem(tr("WAL"), 2);
    TopLay->addWidget(d->DisplayValuesCombo);
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
        d->ConstParLabel[ParamIter] = new QLabel(this);
        d->ConstParLabel[ParamIter]->setText(d->ParamNames[j]);
        d->ConstParLabel[ParamIter]->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        d->ConstParCombo[ParamIter] = new QComboBox(this);
        d->ConstParCombo[ParamIter]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        TopLay->addWidget(d->ConstParLabel[ParamIter]);
        TopLay->addWidget(d->ConstParCombo[ParamIter]);
		ParamIter++;
	}
	MainLay->addWidget(ConstGroup);

	
	

	for (int j = 0; j < 2;++j){
        d->VariableParCombo[j] = new QComboBox(this);
        d->VariableParCombo[j]->setEditable(false);
		for (int i = 0; i < NumStressDimentsions; ++i) {
			if (j==0 && i==2) continue;
			if (j == 1 && i == 1) continue;
            d->VariableParCombo[j]->addItem(d->ParamNames[i], i);
		}
        d->VariableParCombo[j]->setCurrentIndex(1);
	}
	ConstGroup = new QGroupBox(this);
	ConstGroup->setTitle(tr("Stress Test"));
	QGridLayout* CentreLay = new QGridLayout(ConstGroup);
	QVBoxLayout* TableLeftLay = new QVBoxLayout;
	HSpace = new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding);
	TableLeftLay->addItem(HSpace);
    TableLeftLay->addWidget(d->VariableParCombo[0]);
	HSpace = new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding);
	TableLeftLay->addItem(HSpace);
	CentreLay->addLayout(TableLeftLay, 1, 0);
    d->StressTable = new QTableWidget(this);
    d->StressTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d->StressTable->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d->StressTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    d->StressTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    d->StressTable->setSelectionMode(QAbstractItemView::SingleSelection);
    d->StressTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    CentreLay->addWidget(d->StressTable, 1, 1);
	QHBoxLayout* TableToplay = new QHBoxLayout;
	HSpace = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Preferred);
	TableToplay->addItem(HSpace);
    TableToplay->addWidget(d->VariableParCombo[1]);
	HSpace = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Preferred);
	TableToplay->addItem(HSpace);
	CentreLay->addLayout(TableToplay, 0, 1);
	MainLay->addWidget(ConstGroup);

    connect(d->TrancheCombo, SIGNAL(currentTextChanged(QString)), this, SLOT(SetPriceChange()));
    connect(d->TrancheCombo, SIGNAL(currentTextChanged(QString)), this, SLOT(UpdateTable()), Qt::QueuedConnection);
    connect(d->DisplayValuesCombo, SIGNAL(currentTextChanged(QString)), this, SLOT(UpdateTable()), Qt::QueuedConnection);
    connect(d->PriceSpin, SIGNAL(valueChanged(double)), this, SLOT(UpdateTable()), Qt::QueuedConnection);
    connect(d->VariableParCombo[0], SIGNAL(currentTextChanged(QString)), this, SLOT(UpdateCombos()));
    connect(d->VariableParCombo[0], SIGNAL(currentTextChanged(QString)), this, SLOT(UpdateTable()), Qt::QueuedConnection);
    connect(d->VariableParCombo[1], SIGNAL(currentTextChanged(QString)), this, SLOT(UpdateCombos()));
    connect(d->VariableParCombo[1], SIGNAL(currentTextChanged(QString)), this, SLOT(UpdateTable()), Qt::QueuedConnection);
    connect(d->StressTable, &QTableWidget::cellDoubleClicked, this, &StressViewer::SetStressLevel, Qt::QueuedConnection);
    for (int j = 0; j < NumStressDimentsions - 2; ++j) 
        connect(d->ConstParCombo[j], SIGNAL(currentTextChanged(QString)), this, SLOT(UpdateTable()), Qt::QueuedConnection);

}

StressViewer::~StressViewer()
{
    delete d_ptr;
}

QString StressViewer::GetFirstName() const
{
    Q_D(const StressViewer);
     const auto& tempDN = (*(d->StressTarget->GetResults().begin()))->GetDealName();
     if (tempDN.isEmpty())
         return QString();
     return *tempDN.begin();
}

bool StressViewer::LoadStress(const QString& filename)
{
    Q_D(StressViewer);
    d->StressTarget->ResetLoans();
    d->StressTarget->ResetResults();
    d->StressTarget->ResetScenarios();
    if (!d->StressTarget->LoadResultsFromFile(filename)) return false;
    d->AvailableAssumptions[StressViewerPrivate::AssCPR] = d->StressTarget->GetCPRscenarios().toList();
    d->AvailableAssumptions[StressViewerPrivate::AssCDR] = d->StressTarget->GetCDRscenarios().toList();
    d->AvailableAssumptions[StressViewerPrivate::AssLS] = d->StressTarget->GetLSscenarios().toList();
    d->AvailableAssumptions[StressViewerPrivate::AssRecLag] = d->StressTarget->GetRecLagScenarios().toList();
    d->AvailableAssumptions[StressViewerPrivate::AssDelinq] = d->StressTarget->GetDelinqScenarios().toList();
    d->AvailableAssumptions[StressViewerPrivate::AssDelinqLag] = d->StressTarget->GetDelinqLagScenarios().toList();
	for (int i = 0; i < NumStressDimentsions; ++i)
        std::sort(d->AvailableAssumptions[i].begin(), d->AvailableAssumptions[i].end(), [](const QString& a, const QString& b) {return BloombergVector(a).GetValue(0) < BloombergVector(b).GetValue(0); });
    d->TrancheCombo->clear();
    for (int i = 0; i < d->StressTarget->GetStructure().GetTranchesCount(); ++i) {
        d->TrancheCombo->addItem(d->StressTarget->GetStructure().GetTranche(i)->GetTrancheName(), i);
	}
	UpdateCombos();
	UpdateTable();
	return true;
}

AssumptionSet StressViewerPrivate::AssembleAssumption(int r, int c) const {
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

void StressViewer::UpdateCombos()
{
    Q_D(StressViewer);
    d->StressTable->setRowCount(d->AvailableAssumptions[d->VariableParCombo[0]->currentData().toInt()].size());
    d->StressTable->setColumnCount(d->AvailableAssumptions[d->VariableParCombo[1]->currentData().toInt()].size());
    d->StressTable->setVerticalHeaderLabels(d->AvailableAssumptions[d->VariableParCombo[0]->currentData().toInt()]);
    d->StressTable->setHorizontalHeaderLabels(d->AvailableAssumptions[d->VariableParCombo[1]->currentData().toInt()]);
	int ParamIter = 0;
	for (int j = 0; j < NumStressDimentsions; ++j) {
        if (d->VariableParCombo[0]->currentData().toInt() != j) {
            if (d->VariableParCombo[1]->findData(j) < 0) {
                d->VariableParCombo[1]->addItem(d->ParamNames[j], j);
			}
		}
		else 
            d->VariableParCombo[1]->removeItem(d->VariableParCombo[1]->findData(j));
        if (d->VariableParCombo[1]->currentData().toInt() != j) {
            if (d->VariableParCombo[0]->findData(j) < 0) {
                d->VariableParCombo[0]->addItem(d->ParamNames[j], j);
			}
		}
		else 
            d->VariableParCombo[0]->removeItem(d->VariableParCombo[0]->findData(j));
        if (d->VariableParCombo[0]->currentData().toInt() == j || d->VariableParCombo[1]->currentData().toInt() == j) continue;
        d->ConstParCombo[ParamIter]->clear();
        d->ConstParLabel[ParamIter]->setText(d->ParamNames[j]);
        for (auto i = d->AvailableAssumptions[j].constBegin(); i != d->AvailableAssumptions[j].constEnd(); ++i)
            d->ConstParCombo[ParamIter]->addItem(*i);
		ParamIter++;
	}
	for (int j = 0; j < 2; ++j) 
        d->VariableParCombo[j]->model()->sort(0);
}

void StressViewer::UpdateTable()
{
    Q_D(StressViewer);
	QTableWidgetItem* CurrentItem=nullptr;
	double CurrentData;
	double MaxDisc=0.0, MinDisc=0.0;
	bool Minset = false;
    for (int i = 0; i < d->StressTable->rowCount(); ++i) {
        for (int j = 0; j <d->StressTable->columnCount(); ++j) {
			CurrentItem = nullptr;
            const QSharedPointer<Waterfall> CurrentRes = d->StressTarget->GetResults().value(d->AssembleAssumption(i, j));
			if (!CurrentRes) {
				CurrentItem=new QTableWidgetItem("N/A");
				CurrentItem->setData(Qt::UserRole, "N/A");
                CurrentItem->setBackgroundColor(d->GrayFill);
			}
			else {
                const auto CurrTran = CurrentRes->GetTranche(d->TrancheCombo->currentText());
				if (!CurrTran) {
					CurrentItem = new QTableWidgetItem("N/A");
					CurrentItem->setData(Qt::UserRole, "N/A");
                    CurrentItem->setBackgroundColor(d->GrayFill);
				}
				else {
                    switch (d->DisplayValuesCombo->currentData().toInt()) {
					case 0:
						CurrentData = CurrTran->GetLossRate();
						CurrentItem = new QTableWidgetItem(Commarize(CurrentData*100.0, 2) + '%');
						CurrentItem->setData(Qt::UserRole, CurrentData);
                        if (CurrentData >= 1.0) CurrentItem->setBackgroundColor(d->RedFill);
                        else if (CurrentData>0.0) CurrentItem->setBackgroundColor(d->YellowFill);
                        else CurrentItem->setBackgroundColor(d->GreenFill);
						break;
					case 1:
                        if (d->PriceToBeSet) {
                            d->PriceSpin->setValue(CurrTran->GetPrice());
                            d->PriceToBeSet = false; 
                        }
                        CurrentData = CurrTran->GetDiscountMargin(d->PriceSpin->value());
						if ((i == 0 && j == 0) || CurrentData>MaxDisc) 
                            MaxDisc = CurrentData;
						if ((!Minset || CurrentData<MinDisc) && CurrentData >= 1.0) { 
                            MinDisc = CurrentData; 
                            Minset = true;
                        }
						CurrentItem = new QTableWidgetItem(Commarize(CurrentData, 0));
						CurrentItem->setData(Qt::UserRole, CurrentData);
						break;
					case 2:
						CurrentData = CurrTran->GetWALife();
						if ((i == 0 && j == 0) || CurrentData > MaxDisc) MaxDisc = CurrentData;
						if ((!Minset || CurrentData < MinDisc) && CurrentData>0) { MinDisc = CurrentData; Minset = true; }
						CurrentItem = new QTableWidgetItem(Commarize(CurrentData, 2));
						CurrentItem->setData(Qt::UserRole, CurrentData);
						break;
					default:
						CurrentItem = new QTableWidgetItem("N/A");
						CurrentItem->setData(Qt::UserRole, "N/A");
                        CurrentItem->setBackgroundColor(d->GrayFill);
					}
				}
			}
            d->StressTable->setItem(i, j, CurrentItem);
		}
	}
    for (int i = 0; i < d->StressTable->rowCount(); i++) {
        for (int j = 0; j < d->StressTable->columnCount(); j++) {
            CurrentItem = d->StressTable->item(i, j);
			if (CurrentItem->data(Qt::UserRole).toString() == "N/A") 
                continue;
            switch (d->DisplayValuesCombo->currentData().toInt()) {
			case 1:
				CurrentData = CurrentItem->data(Qt::UserRole).toDouble();
                if (CurrentData < 1.0) CurrentItem->setBackgroundColor(d->RedFill);
				else {
                    if (MaxDisc - MinDisc < 1.0) 
                        d->GradientBase.setCurrentTime(100.0);
                    else 
                        d->GradientBase.setCurrentTime(100.0*(CurrentData - MinDisc) / (MaxDisc - MinDisc));
                    CurrentItem->setBackgroundColor(d->GradientBase.currentValue().value<QColor>());
				}
				break;

			case 2:
				CurrentData = CurrentItem->data(Qt::UserRole).toDouble();
                if (CurrentData <= 0.0) CurrentItem->setBackgroundColor(d->RedFill);
				else {
                    if (MaxDisc - MinDisc < 1.0) 
                        d->GradientBase.setCurrentTime(100.0);
                    else 
                        d->GradientBase.setCurrentTime(100.0 - (100.0*(CurrentData - MinDisc) / (MaxDisc - MinDisc)));
                    CurrentItem->setBackgroundColor(d->GradientBase.currentValue().value<QColor>());
				}
				break;
			default:
				break;
			}
		}
	}
    if (d->DisplayValuesCombo->currentData().toInt() == 1) {
        d->PriceSpin->show();
        d->PriceLabel->show();
	}
	else {
        d->PriceSpin->hide();
        d->PriceLabel->hide();
	}
}

void StressViewer::SetPriceChange()
{
    Q_D(StressViewer);
    d->PriceToBeSet = true;
}

void StressViewer::SetStressLevel(int r, int c)
{
    Q_D(StressViewer);
    const QSharedPointer<Waterfall> TempWat = d->StressTarget->GetResults().value(d->AssembleAssumption(r, c), QSharedPointer<Waterfall>(nullptr));
	if (TempWat) 
        emit StressLevelChanged(*TempWat);
}
