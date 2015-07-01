#include "WaterfallStepHelperDialog.h"
#include "Private/WaterfallStepHelperDialog_p.h"
#include <QComboBox>
#include <QStackedWidget>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QLineEdit>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include "WatFalPrior.h"
#include "IntegerVector.h"
#include "BloombergVector.h"
#include "TriggerStructHelperWidget.h"
WaterfallStepHelperDialogPrivate::WaterfallStepHelperDialogPrivate(WaterfallStepHelperDialog *q)
    :q_ptr(q)
    , m_InterestWF(true)
    , FirstCombodeleted(false)
    , Cleared(false)
{}
WaterfallStepHelperDialog::WaterfallStepHelperDialog(QWidget *parent)
    : WaterfallStepHelperDialog(new WaterfallStepHelperDialogPrivate(this),parent)
{}
WaterfallStepHelperDialog::WaterfallStepHelperDialog(WaterfallStepHelperDialogPrivate* d, QWidget *parent)
	: QDialog(parent)
	, d_ptr(d)
{
	setWindowIcon(QIcon(":/Icons/Logo.png"));
	setWindowTitle(tr("Edit Waterfall Step"));
	
	d->StepSelectorModel = new QStandardItemModel(this);
    d->StepSelectorModel->setColumnCount(2);
    d->StepSelectorModel->setRowCount(WaterfallStepHelperDialogPrivate::NumberOfStepTypes + 1);
	int IndexCounter = 0;
    d->StepSelectorModel->setData(d->StepSelectorModel->index(0, 0), ""); d->StepSelectorModel->setData(d->StepSelectorModel->index(0, 0), -1, Qt::UserRole); d->StepSelectorModel->setData(d->StepSelectorModel->index(0, 1), WaterfallStepHelperDialogPrivate::AllWaterfalls); d->StepSelectorModel->setData(d->StepSelectorModel->index(0, 1), 0);
    d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), "Senior Expenses"); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), WatFalPrior::WaterfallStepType::wst_SeniorExpenses, Qt::UserRole); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 1), WaterfallStepHelperDialogPrivate::AllWaterfalls); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), IndexCounter, Qt::UserRole + 1); ++IndexCounter;
    d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), "Senior Fees"); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), WatFalPrior::WaterfallStepType::wst_SeniorFees, Qt::UserRole); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 1), WaterfallStepHelperDialogPrivate::AllWaterfalls); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), IndexCounter, Qt::UserRole + 1); ++IndexCounter;
    d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), "Interest"); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), WatFalPrior::WaterfallStepType::wst_Interest, Qt::UserRole); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 1), WaterfallStepHelperDialogPrivate::InterestWFonly); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), IndexCounter, Qt::UserRole + 1); ++IndexCounter;
    d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), "Principal"); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), WatFalPrior::WaterfallStepType::wst_Principal, Qt::UserRole); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 1), WaterfallStepHelperDialogPrivate::PrincipalWFonly); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), IndexCounter, Qt::UserRole + 1); ++IndexCounter;
    d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), "OC Test"); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), WatFalPrior::WaterfallStepType::wst_OCTest, Qt::UserRole); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 1), WaterfallStepHelperDialogPrivate::AllWaterfalls); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), IndexCounter, Qt::UserRole + 1); ++IndexCounter;
    d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), "IC Test"); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), WatFalPrior::WaterfallStepType::wst_ICTest, Qt::UserRole); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 1), WaterfallStepHelperDialogPrivate::AllWaterfalls); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), IndexCounter, Qt::UserRole + 1); ++IndexCounter;
    d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), "Deferred Interest"); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), WatFalPrior::WaterfallStepType::wst_DeferredInterest, Qt::UserRole); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 1), WaterfallStepHelperDialogPrivate::AllWaterfalls); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), IndexCounter, Qt::UserRole + 1); ++IndexCounter;
    d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), "Junior Fees"); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), WatFalPrior::WaterfallStepType::wst_juniorFees, Qt::UserRole); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 1), WaterfallStepHelperDialogPrivate::AllWaterfalls); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), IndexCounter, Qt::UserRole + 1); ++IndexCounter;
    d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), "Reinvestment Test"); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), WatFalPrior::WaterfallStepType::wst_ReinvestmentTest, Qt::UserRole); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 1), WaterfallStepHelperDialogPrivate::AllWaterfalls); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), IndexCounter, Qt::UserRole + 1); ++IndexCounter;
    d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), "Excess"); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), WatFalPrior::WaterfallStepType::wst_Excess, Qt::UserRole); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 1), WaterfallStepHelperDialogPrivate::AllWaterfalls); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), IndexCounter, Qt::UserRole + 1); ++IndexCounter;
    d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), "Reinvest Principal"); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), WatFalPrior::WaterfallStepType::wst_ReinvestPrincipal, Qt::UserRole); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 1), WaterfallStepHelperDialogPrivate::PrincipalWFonly); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), IndexCounter, Qt::UserRole + 1); ++IndexCounter;
    d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), "Reserve Replenish"); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), WatFalPrior::WaterfallStepType::wst_ReserveReplenish, Qt::UserRole); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 1), WaterfallStepHelperDialogPrivate::AllWaterfalls); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), IndexCounter, Qt::UserRole + 1); ++IndexCounter;
    d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), "Turbo"); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), WatFalPrior::WaterfallStepType::wst_Turbo, Qt::UserRole); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 1), WaterfallStepHelperDialogPrivate::InterestWFonly); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), IndexCounter, Qt::UserRole + 1); ++IndexCounter;
    d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), "PDL"); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), WatFalPrior::WaterfallStepType::wst_PDL, Qt::UserRole); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 1), WaterfallStepHelperDialogPrivate::InterestWFonly); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), IndexCounter, Qt::UserRole + 1); ++IndexCounter;
    d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), "Fees From Excess"); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), WatFalPrior::WaterfallStepType::wst_FeesFromExcess, Qt::UserRole); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 1), WaterfallStepHelperDialogPrivate::AllWaterfalls); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), IndexCounter, Qt::UserRole + 1); ++IndexCounter;
    d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), "Allocate Prepay Fees"); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), WatFalPrior::WaterfallStepType::wst_AllocPrepayFees, Qt::UserRole); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 1), WaterfallStepHelperDialogPrivate::AllWaterfalls); d->StepSelectorModel->setData(d->StepSelectorModel->index(IndexCounter + 1, 0), IndexCounter, Qt::UserRole + 1); ++IndexCounter;
    Q_ASSERT(IndexCounter == WaterfallStepHelperDialogPrivate::NumberOfStepTypes);
    d->StepsFilter = new QSortFilterProxyModel(this);
    d->StepsFilter->setSourceModel(d->StepSelectorModel);
    d->StepsFilter->setFilterKeyColumn(1);

	d->StepSelectorCombo = new QComboBox(this);
	d->StepSelectorCombo->setEditable(false);
    d->StepSelectorCombo->setModel(d->StepsFilter);
	d->StepSelectorCombo->setModelColumn(0);
	d->StepSelectorCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	d->StepBuilderBase = new QStackedWidget(this);
	d->StepBuilderBase->setMinimumSize(200, 200);
	d->StepBuilderBase->setStyleSheet("QLabel { qproperty-alignment: 'AlignRight | AlignVCenter'; }");
	d->StepBuilderBase->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	d->StepBuilderBase->addWidget(new QWidget(this)); //No Step Selected
	d->StepBuilderBase->addWidget(new QWidget(this)); //Senior Expenses
	d->StepBuilderBase->addWidget(new QWidget(this)); //Senior Fees
	d->StepBuilderBase->addWidget(d->CreateInterestWidget()); //Interest
    d->StepBuilderBase->addWidget(d->CreatePrincipalWidget()); //Principal
    d->StepBuilderBase->addWidget(d->CreateOCWidget()); //OC Test
    d->StepBuilderBase->addWidget(d->CreateICWidget()); //IC Test
    d->StepBuilderBase->addWidget(d->CreateDeferredInterestWidget()); //Deferred Interest
    d->StepBuilderBase->addWidget(d->CreateJuniorFeesWidget()); //Junior Fees
    d->StepBuilderBase->addWidget(d->CreateOCWidget()); //Reinvestment Test
    d->StepBuilderBase->addWidget(d->CreateExcessWidget()); //Excess
    d->StepBuilderBase->addWidget(d->CreateReinvestPrincipalWidget()); //Reinvest
    d->StepBuilderBase->addWidget(d->CreateReserveReplenishWidget()); //Reserve Replenish
    d->StepBuilderBase->addWidget(d->CreateTurboWidget()); //Turbo
    d->StepBuilderBase->addWidget(d->CreateCurePDLWidget()); //PDL Cure
    d->StepBuilderBase->addWidget(d->CreateFeesFromXSWidget()); //Fees From Excess
    d->StepBuilderBase->addWidget(d->CreateAllocPrepayWidget()); //Allocate Prepay Fees

    connect(d->StepSelectorCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [&](int index) {d_func()->StepBuilderBase->setCurrentIndex(d_func()->StepSelectorCombo->itemData(index, Qt::UserRole + 1).toInt()); });
	connect(d->StepSelectorCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(ClearParameters()));
	connect(d->StepSelectorCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(SetDefaults1())/*, Qt::QueuedConnection*/);
	connect(d->StepSelectorCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(SetDefaults100())/*, Qt::QueuedConnection*/);
	connect(d->StepSelectorCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(SetDefaults0())/*, Qt::QueuedConnection*/);
	connect(d->StepSelectorCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(CheckOkEnabled(int))/*,Qt::QueuedConnection*/);
	connect(d->StepSelectorCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(ResetSoFCombo(int))/*, Qt::QueuedConnection*/);
	connect(d->StepSelectorCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(ResetPayAccrueCombo(int))/*, Qt::QueuedConnection*/);

	QLabel *StepSelectLabel = new QLabel(this);
	StepSelectLabel->setText(tr("Select Step Type"));

	
	d->AcceptButton = new QPushButton(this);
	d->AcceptButton->setText(tr("OK"));
	d->AcceptButton->setDefault(true);
	d->AcceptButton->setEnabled(false);
	connect(d->AcceptButton, &QPushButton::clicked, this, &QDialog::accept);
	QPushButton*  CancelButton = new QPushButton(this);
	CancelButton->setText(tr("Cancel"));
	CancelButton->setDefault(false);
	connect(CancelButton, &QPushButton::clicked, this, &QDialog::reject);
	QPushButton* ClearAcceptButton = new QPushButton(this);
	ClearAcceptButton->setText(tr("Clear"));
	ClearAcceptButton->setDefault(false);
	connect(ClearAcceptButton, &QPushButton::clicked, this, &WaterfallStepHelperDialog::ClearAndAccept);

	QGroupBox *TopGroup = new QGroupBox(this);
	TopGroup->setTitle("Step");
	QGroupBox *ParamGroup = new QGroupBox(this);
	ParamGroup->setTitle("Parameters");
	ParamGroup->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	QHBoxLayout* ParamLay = new QHBoxLayout(ParamGroup);
	ParamLay->addWidget(d->StepBuilderBase);
	QHBoxLayout* TopLay = new QHBoxLayout(TopGroup);
	TopLay->addWidget(StepSelectLabel);
	TopLay->addWidget(d->StepSelectorCombo);

	QHBoxLayout* ButtonsLay = new QHBoxLayout;
	ButtonsLay->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));
	ButtonsLay->addWidget(d->AcceptButton);
	ButtonsLay->addWidget(ClearAcceptButton);
	ButtonsLay->addWidget(CancelButton);

	d->TriggerBuilder = new TriggerStructHelperWidget(QHash<quint32, QSharedPointer<AbstractTrigger> >(), this);
	d->TriggerBuilder->setEnabled(false);
	QGroupBox *TrigGroup = new QGroupBox(this);
	TrigGroup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	TrigGroup->setTitle("Trigger");
	QHBoxLayout* TrigLay = new QHBoxLayout(TrigGroup);
	TrigLay->addWidget(d->TriggerBuilder);
	
	QHBoxLayout* CentreLay=new QHBoxLayout;
	CentreLay->addWidget(ParamGroup);
	CentreLay->addWidget(TrigGroup);

	QVBoxLayout* mainlay = new QVBoxLayout(this);
	mainlay->addWidget(TopGroup);
	mainlay->addLayout(CentreLay);
	mainlay->addLayout(ButtonsLay);

	SetInterestWF(d->m_InterestWF);
}

WaterfallStepHelperDialog::~WaterfallStepHelperDialog()
{
    delete d_ptr;
}

void WaterfallStepHelperDialog::CheckOkEnabled(int index)
{
    Q_D(WaterfallStepHelperDialog);
	if (d->FirstCombodeleted) return;
	d->AcceptButton->setEnabled(index != 0);
	if (index != 0) {
		disconnect(d->StepSelectorCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(CheckOkEnabled(int)));
		QWidget* EmptyWidg = d->StepBuilderBase->widget(0);
		d->StepBuilderBase->removeWidget(EmptyWidg);
		d->StepBuilderBase->setCurrentIndex(d->StepBuilderBase->currentIndex() - 1);
		EmptyWidg->deleteLater();
		d->FirstCombodeleted = true;
		d->StepSelectorModel->removeRow(0);
		d->TriggerBuilder->setEnabled(true);
	}
}



void WaterfallStepHelperDialog::ClearParameters()
{
    Q_D(WaterfallStepHelperDialog);
    d->ResultingParameters.clear();
}

void WaterfallStepHelperDialog::SetParameter(int PrmIndex, QString value)
{
    Q_D(WaterfallStepHelperDialog);
    d->ResultingParameters[PrmIndex] = value;
}

void WaterfallStepHelperDialog::SetSeniorityGroup(QString value)
{
    Q_D(WaterfallStepHelperDialog);
	if (!sender()) return;
	if (d->StepBuilderBase->currentWidget()->children().contains(sender()))
		SetParameter(static_cast<qint32>(WatFalPrior::wstParameters::SeniorityGroup), value);
}

void WaterfallStepHelperDialog::SetSeniorityGroupLevel(QString value)
{
    Q_D(WaterfallStepHelperDialog);
	if (!sender()) return;
	if (d->StepBuilderBase->currentWidget()->children().contains(sender()))
		SetParameter(static_cast<qint32>(WatFalPrior::wstParameters::SeniorityGroupLevel), value);
}

void WaterfallStepHelperDialog::SetRedemptionGroup(QString value)
{
    Q_D(WaterfallStepHelperDialog);
	if (!sender()) return;
	if (d->StepBuilderBase->currentWidget()->children().contains(sender()))
		SetParameter(static_cast<qint32>(WatFalPrior::wstParameters::RedemptionGroup), value);
}

void WaterfallStepHelperDialog::SetRedemptionGroupLevel(QString value)
{
    Q_D(WaterfallStepHelperDialog);
	if (!sender()) return;
	if (d->StepBuilderBase->currentWidget()->children().contains(sender()))
		SetParameter(static_cast<qint32>(WatFalPrior::wstParameters::RedemptionGroupLevel), value);
}

void WaterfallStepHelperDialog::SetRedemptionShare(QString value)
{
    Q_D(WaterfallStepHelperDialog);
	if (!sender()) return;
	if (d->StepBuilderBase->currentWidget()->children().contains(sender()))
		SetParameter(static_cast<qint32>(WatFalPrior::wstParameters::RedemptionShare), value);
}

void WaterfallStepHelperDialog::SetAdditionalCollateralShare(QString value)
{
    Q_D(WaterfallStepHelperDialog);
	if (!sender()) return;
	if (d->StepBuilderBase->currentWidget()->children().contains(sender()))
		SetParameter(static_cast<qint32>(WatFalPrior::wstParameters::AdditionalCollateralShare), value);
}

void WaterfallStepHelperDialog::SetSourceOfFunding(QString value)
{
    Q_D(WaterfallStepHelperDialog);
	if (!sender()) return;
	if (d->StepBuilderBase->currentWidget()->children().contains(sender()))
		SetParameter(static_cast<qint32>(WatFalPrior::wstParameters::SourceOfFunding), value);
}

void WaterfallStepHelperDialog::SetCouponIndex(QString value)
{
    Q_D(WaterfallStepHelperDialog);
	if (!sender()) return;
	if (d->StepBuilderBase->currentWidget()->children().contains(sender()))
		SetParameter(static_cast<qint32>(WatFalPrior::wstParameters::CouponIndex), value);
}

void WaterfallStepHelperDialog::SetTestTargetOverride(QString value)
{
    Q_D(WaterfallStepHelperDialog);
	if (!sender()) return;
	if (d->StepBuilderBase->currentWidget()->children().contains(sender()))
		SetParameter(static_cast<qint32>(WatFalPrior::wstParameters::TestTargetOverride), value);
}

void WaterfallStepHelperDialog::SetIRRtoEquityTarget(QString value)
{
    Q_D(WaterfallStepHelperDialog);
	if (!sender()) return;
	if (d->StepBuilderBase->currentWidget()->children().contains(sender()))
		SetParameter(static_cast<qint32>(WatFalPrior::wstParameters::IRRtoEquityTarget), value);
}

void WaterfallStepHelperDialog::SetReserveIndex(QString value)
{
    Q_D(WaterfallStepHelperDialog);
	if (!sender()) return;
	if (d->StepBuilderBase->currentWidget()->children().contains(sender()))
		SetParameter(static_cast<qint32>(WatFalPrior::wstParameters::ReserveIndex), value);
}
void WaterfallStepHelperDialog::SetPayAccrue(QString value)
{
    Q_D(WaterfallStepHelperDialog);
	if (!sender()) return;
	if (d->StepBuilderBase->currentWidget()->children().contains(sender()))
		SetParameter(static_cast<qint32>(WatFalPrior::wstParameters::PayAccrue), value);
}

void WaterfallStepHelperDialog::SoFComboChanged(int index)
{
    Q_D(WaterfallStepHelperDialog);
	if (!sender()) return;
	if (d->StepBuilderBase->currentWidget()->children().contains(sender()))
		d->ResultingParameters[static_cast<qint32>(WatFalPrior::wstParameters::SourceOfFunding)] = QString::number((index == 0) ? 3 : index);
}
void WaterfallStepHelperDialog::PayAccrueComboChanged(int index)
{
    Q_D(WaterfallStepHelperDialog);
	if (!sender()) return;
	if (d->StepBuilderBase->currentWidget()->children().contains(sender())) {
		switch (index) {
		case 0: 
			d->ResultingParameters[static_cast<qint32>(WatFalPrior::wstParameters::PayAccrue)] = QString::number(static_cast<quint8>(WatFalPrior::wstAccrueOrPay::AccrueAndPay));
		break;
		case 1:
			d->ResultingParameters[static_cast<qint32>(WatFalPrior::wstParameters::PayAccrue)] = QString::number(static_cast<quint8>(WatFalPrior::wstAccrueOrPay::Accrue));
		break;
		case 2:
			d->ResultingParameters[static_cast<qint32>(WatFalPrior::wstParameters::PayAccrue)] = QString::number(static_cast<quint8>(WatFalPrior::wstAccrueOrPay::Pay));
		break;
		default:
			Q_ASSERT(false);
		}
	}
		
}
QString WaterfallStepHelperDialog::GetParameters()
{
    Q_D(WaterfallStepHelperDialog);
	SetbasedOnWaterfall();
	if (d->Cleared) d->ResultingParameters.clear();
	QString Result = (d->Cleared? QString() : QString::number(d->StepSelectorCombo->currentData().toInt()));
	Result += '#' + d->ResultingParameters.value(static_cast<qint32>(WatFalPrior::wstParameters::SeniorityGroup), "");
	Result += '#' + d->ResultingParameters.value(static_cast<qint32>(WatFalPrior::wstParameters::SeniorityGroupLevel), "") ;
	Result += '#' + d->ResultingParameters.value(static_cast<qint32>(WatFalPrior::wstParameters::RedemptionGroup), "") ;
	Result += '#' + d->ResultingParameters.value(static_cast<qint32>(WatFalPrior::wstParameters::RedemptionGroupLevel), "") ;
	Result += '#' + d->ResultingParameters.value(static_cast<qint32>(WatFalPrior::wstParameters::RedemptionShare), "") ;
	Result += '#' + d->ResultingParameters.value(static_cast<qint32>(WatFalPrior::wstParameters::AdditionalCollateralShare), "") ;
	Result += '#' + d->ResultingParameters.value(static_cast<qint32>(WatFalPrior::wstParameters::SourceOfFunding), "") ;
	Result += '#' + d->ResultingParameters.value(static_cast<qint32>(WatFalPrior::wstParameters::CouponIndex), "") ;
	Result += '#' + d->ResultingParameters.value(static_cast<qint32>(WatFalPrior::wstParameters::TestTargetOverride), "") ;
	Result += '#' + d->ResultingParameters.value(static_cast<qint32>(WatFalPrior::wstParameters::IRRtoEquityTarget), "") ;
	Result += '#' + d->ResultingParameters.value(static_cast<qint32>(WatFalPrior::wstParameters::ReserveIndex), "") ;
	Result += '#' + d->ResultingParameters.value(static_cast<qint32>(WatFalPrior::wstParameters::PayAccrue), "");
	Result += '#' + (d->Cleared ? QString() : d->TriggerBuilder->GetResult());
	return Result;
}

void WaterfallStepHelperDialog::SetDefaults1() {
	emit SetTo1("1");
}
void WaterfallStepHelperDialog::SetDefaults100() {
	emit SetTo100("100");
}
void WaterfallStepHelperDialog::SetDefaults0() {
	emit SetTo0("0");
}
void WaterfallStepHelperDialog::ResetSoFCombo(int index)
{
    Q_D(WaterfallStepHelperDialog);
	if (d->StepSelectorCombo->itemData(index).toInt() == WatFalPrior::WaterfallStepType::wst_ReinvestPrincipal) {
		d->ResultingParameters[static_cast<qint32>(WatFalPrior::wstParameters::SourceOfFunding)] = "3";
		emit SetSoFCombo(0);
	}
}
void WaterfallStepHelperDialog::ResetPayAccrueCombo(int index) {
	emit SetPayAccrueCombo(1); 
	emit SetPayAccrueCombo(0); //Ensures it defaults to Accrue and Pay
}
void WaterfallStepHelperDialog::SetbasedOnWaterfall()
{
    Q_D(WaterfallStepHelperDialog);
	switch (static_cast<WatFalPrior::WaterfallStepType>(d->StepSelectorCombo->currentData().toInt())) {
	case WatFalPrior::WaterfallStepType::wst_SeniorExpenses:
	case WatFalPrior::WaterfallStepType::wst_SeniorFees:
	case WatFalPrior::WaterfallStepType::wst_juniorFees:
	case WatFalPrior::WaterfallStepType::wst_OCTest:
	case WatFalPrior::WaterfallStepType::wst_ICTest:
	case WatFalPrior::WaterfallStepType::wst_DeferredInterest:
	case WatFalPrior::WaterfallStepType::wst_ReinvestmentTest:
	case WatFalPrior::WaterfallStepType::wst_Excess:
	case WatFalPrior::WaterfallStepType::wst_ReserveReplenish:
	case WatFalPrior::WaterfallStepType::wst_FeesFromExcess:
	case WatFalPrior::WaterfallStepType::wst_AllocPrepayFees :
		d->ResultingParameters[static_cast<qint32>(WatFalPrior::wstParameters::SourceOfFunding)] = (d->m_InterestWF ? QString("1") : QString("2")); break;
	}
}

void WaterfallStepHelperDialog::SetAvailableTriggers(const QHash<quint32, QSharedPointer<AbstractTrigger> >& AvailableTriggers)
{
    Q_D(WaterfallStepHelperDialog);
    d->TriggerBuilder->SetAvailableTriggers(AvailableTriggers);
    d->Cleared = false;
}

void WaterfallStepHelperDialog::SetCurrentPars(const QString& a)
{
    Q_D(WaterfallStepHelperDialog);
    d->Cleared = false;
    const auto AllPars = a.split('#');
    if (AllPars.size() != 14) return;
    auto i = AllPars.constBegin();
    if (i->isEmpty()) return;
    d->StepSelectorCombo->setCurrentIndex(d->StepSelectorCombo->findData(static_cast<qint16>(i->toInt()))); ++i;
    if (!i->isEmpty()) { emit  ImportSeniorityGroup(*i); } ++i;
    if (!i->isEmpty()) { emit  ImportSeniorityGroupLevel(*i); } ++i;
    if (!i->isEmpty()) { emit  ImportRedemptionGroup(*i); }++i;
    if (!i->isEmpty()) { emit  ImportRedemptionGroupLevel(*i); } ++i;
    if (!i->isEmpty()) { emit  ImportRedemptionShare(*i); }++i;
    if (!i->isEmpty()) { emit  ImportAdditionalCollateralShare(*i); } ++i;
    if (!i->isEmpty()) {
        emit  ImportSourceOfFunding(*i);
        if (d->StepSelectorCombo->currentData().toInt() == static_cast<qint32>(WatFalPrior::WaterfallStepType::wst_ReinvestPrincipal)) {
            d->ResultingParameters[static_cast<qint32>(WatFalPrior::wstParameters::SourceOfFunding)] = *i;
            switch (i->toInt()) {
            case 1: emit SetSoFCombo(1); break;
            case 2: emit SetSoFCombo(2); break;
            default:
            case 3: emit SetSoFCombo(0); break;
            }
        }
    }
    ++i;
    if (!i->isEmpty()) { emit  ImportCouponIndex(*i); }++i;
    if (!i->isEmpty()) { emit  ImportTestTargetOverride(*i); } ++i;
    if (!i->isEmpty()) { emit  ImportIRRtoEquityTarget(*i); }++i;
    if (!i->isEmpty()) { emit  ImportReserveIndex(*i); }++i;
    if (!i->isEmpty()) {
        emit  ImportPayAccrue(*i);
        if (i->toInt() > 0)
            d->ResultingParameters[static_cast<qint32>(WatFalPrior::wstParameters::PayAccrue)] = *i;
        else d->ResultingParameters.remove(static_cast<qint32>(WatFalPrior::wstParameters::PayAccrue));
        switch (i->toInt()) {
        case static_cast<quint8>(WatFalPrior::wstAccrueOrPay::AccrueAndPay) :
            emit SetPayAccrueCombo(0);
            break;
        case static_cast<quint8>(WatFalPrior::wstAccrueOrPay::Accrue) :
            emit SetPayAccrueCombo(1);
            break;
        case static_cast<quint8>(WatFalPrior::wstAccrueOrPay::Pay) :
            emit SetPayAccrueCombo(2);
            break;
        default:
            break;
        }
    }
    ++i;
    d->TriggerBuilder->SetCurrentStructure(*i);
}

const bool& WaterfallStepHelperDialog::GetInterestWF() const
{
    Q_D(const WaterfallStepHelperDialog);
    return d->m_InterestWF;
}

void WaterfallStepHelperDialog::ClearAndAccept()
{
    Q_D(WaterfallStepHelperDialog);
    d->Cleared = true;
    accept();
}

void WaterfallStepHelperDialog::SetInterestWF(const bool& val)
{
    Q_D(WaterfallStepHelperDialog);
    d->m_InterestWF = val;
    QString FilterPattern = "^[" + QString::number(WaterfallStepHelperDialogPrivate::AllWaterfalls);
    if (d->m_InterestWF) FilterPattern += QString::number(WaterfallStepHelperDialogPrivate::InterestWFonly);
    else FilterPattern += QString::number(WaterfallStepHelperDialogPrivate::PrincipalWFonly);
    FilterPattern += "]$";
    d->StepsFilter->setFilterRegExp(QRegExp(FilterPattern));
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
QWidget* WaterfallStepHelperDialogPrivate::CreateJuniorFeesWidget() {
    Q_Q(WaterfallStepHelperDialog);
	QWidget* Result = new QWidget(q);
	QGridLayout *ResLay = new QGridLayout(Result);
	int CountRow = 0;

	QLabel* SrcFndLbl = new QLabel(Result);
	SrcFndLbl->setText(q->tr("Accrue or Pay"));
	QComboBox* SrcFndCmb = new QComboBox(Result);
	SrcFndCmb->setEditable(false);
	SrcFndCmb->addItem("Accrue and Pay", static_cast<quint8>(WatFalPrior::wstAccrueOrPay::AccrueAndPay));
	SrcFndCmb->addItem("Accrue Only", static_cast<quint8>(WatFalPrior::wstAccrueOrPay::Accrue));
	SrcFndCmb->addItem("Pay Only", static_cast<quint8>(WatFalPrior::wstAccrueOrPay::Pay));
	q->connect(q, SIGNAL(SetPayAccrueCombo(int)), SrcFndCmb, SLOT(setCurrentIndex(int)));
	q->connect(SrcFndCmb, SIGNAL(currentIndexChanged(int)), q, SLOT(PayAccrueComboChanged(int)));
	ResLay->addWidget(SrcFndLbl, CountRow, 0);
	ResLay->addWidget(SrcFndCmb, CountRow, 1);
	CountRow++;

	ResLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), CountRow, 0, 1, 2);
	return Result;
}
QWidget* WaterfallStepHelperDialogPrivate::CreateInterestWidget()
{
    Q_Q(WaterfallStepHelperDialog);
	QWidget* Result = new QWidget(q);
	QGridLayout *ResLay = new QGridLayout(Result);
	int CountRow = 0;

	QLabel* SenGrpLbl = new QLabel(Result);
	SenGrpLbl->setText(q->tr("Seniority group that will receive the interest"));
	QLineEdit* SenGrpEdit = new QLineEdit(Result);
	SenGrpEdit->setValidator(IntegerVector().GetValidator(SenGrpEdit));
	SenGrpEdit->setToolTip(q->tr("q is the pro-rata level of the tranches.\nAccepts vectors."));
	q->connect(SenGrpEdit, SIGNAL(textChanged(QString)), q, SLOT(SetSeniorityGroup(QString)));
	q->connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), SenGrpEdit, SLOT(clear()));
	q->connect(q, SIGNAL(ImportSeniorityGroup(QString)), SenGrpEdit, SLOT(setText(QString)), Qt::QueuedConnection);
	ResLay->addWidget(SenGrpLbl, CountRow, 0);
	ResLay->addWidget(SenGrpEdit, CountRow, 1);
	CountRow++;

	QLabel* SenLvlLbl = new QLabel(Result);
	SenLvlLbl->setText(q->tr("Level of seniority the seniority group refers to"));
	QLineEdit* SenLvlEdit = new QLineEdit(Result);
	SenLvlEdit->setValidator(IntegerVector().GetValidator(SenGrpEdit));
	SenLvlEdit->setToolTip(q->tr("q is the depth of the pro-rata group.\nAccepts vectors.\nNormally 1"));
	q->connect(SenLvlEdit, SIGNAL(textChanged(QString)), q, SLOT(SetSeniorityGroupLevel(QString)));
	q->connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), SenLvlEdit, SLOT(clear()));
	q->connect(q, SIGNAL(SetTo1(QString)), SenLvlEdit, SLOT(setText(QString)));
	q->connect(q, SIGNAL(ImportSeniorityGroupLevel(QString)), SenLvlEdit, SLOT(setText(QString)), Qt::QueuedConnection);
	ResLay->addWidget(SenLvlLbl, CountRow, 0);
	ResLay->addWidget(SenLvlEdit, CountRow, 1);
	CountRow++;
	SenLvlEdit->setText("1");

	QLabel* CoupIdxLbl = new QLabel(Result);
	CoupIdxLbl->setText(q->tr("Level coupon to be paid"));
	QLineEdit* CoupIdxEdit = new QLineEdit(Result);
	CoupIdxEdit->setValidator(IntegerVector().GetValidator(SenGrpEdit));
	CoupIdxEdit->setToolTip(q->tr("q is the coupon to be paid.\nAccepts vectors.\nNormally 1"));
	q->connect(CoupIdxEdit, SIGNAL(textChanged(QString)), q, SLOT(SetCouponIndex(QString)));
	q->connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), CoupIdxEdit, SLOT(clear()));
	q->connect(q, SIGNAL(SetTo1(QString)), CoupIdxEdit, SLOT(setText(QString)));
	q->connect(q, SIGNAL(ImportCouponIndex(QString)), CoupIdxEdit, SLOT(setText(QString)), Qt::QueuedConnection);
	ResLay->addWidget(CoupIdxLbl, CountRow, 0);
	ResLay->addWidget(CoupIdxEdit, CountRow, 1);
	CountRow++;
	CoupIdxEdit->setText("1");
	CoupIdxEdit->setEnabled(false);

	QLabel* SrcFndLbl = new QLabel(Result);
	SrcFndLbl->setText(q->tr("Accrue or Pay"));
	QComboBox* SrcFndCmb = new QComboBox(Result);
	SrcFndCmb->setEditable(false);
	SrcFndCmb->addItem("Accrue and Pay", static_cast<quint8>(WatFalPrior::wstAccrueOrPay::AccrueAndPay));
	SrcFndCmb->addItem("Accrue Only", static_cast<quint8>(WatFalPrior::wstAccrueOrPay::Accrue));
	SrcFndCmb->addItem("Pay Only", static_cast<quint8>(WatFalPrior::wstAccrueOrPay::Pay));
	q->connect(q, SIGNAL(SetPayAccrueCombo(int)), SrcFndCmb, SLOT(setCurrentIndex(int)));
	q->connect(SrcFndCmb, SIGNAL(currentIndexChanged(int)), q, SLOT(PayAccrueComboChanged(int)));
	ResLay->addWidget(SrcFndLbl, CountRow, 0);
	ResLay->addWidget(SrcFndCmb, CountRow, 1);
	CountRow++;

	ResLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), CountRow, 0, 1, 2);
	return Result;
}
QWidget* WaterfallStepHelperDialogPrivate::CreatePrincipalWidget()
{
    Q_Q(WaterfallStepHelperDialog);
	QWidget* Result = new QWidget(q);
	QGridLayout *ResLay = new QGridLayout(Result);
	int CountRow = 0;

	QLabel* SenGrpLbl = new QLabel(Result);
	SenGrpLbl->setText(q->tr("Seniority group that will be redeemed"));
	QLineEdit* SenGrpEdit = new QLineEdit(Result);
	SenGrpEdit->setValidator(IntegerVector().GetValidator(SenGrpEdit));
	SenGrpEdit->setToolTip(q->tr("q is the pro-rata level of the tranches.\nAccepts vectors."));
	q->connect(SenGrpEdit, SIGNAL(textChanged(QString)), q, SLOT(SetSeniorityGroup(QString)));
	q->connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), SenGrpEdit, SLOT(clear()));
	q->connect(q, SIGNAL(ImportSeniorityGroup(QString)), SenGrpEdit, SLOT(setText(QString)), Qt::QueuedConnection);
	ResLay->addWidget(SenGrpLbl, CountRow, 0);
	ResLay->addWidget(SenGrpEdit, CountRow, 1);
	CountRow++;

	QLabel* SenLvlLbl = new QLabel(Result);
	SenLvlLbl->setText(q->tr("Level of seniority the seniority group refers to"));
	QLineEdit* SenLvlEdit = new QLineEdit(Result);
	SenLvlEdit->setValidator(IntegerVector().GetValidator(SenGrpEdit));
	SenLvlEdit->setToolTip(q->tr("q is the depth of the pro-rata group.\nAccepts vectors.\nNormally 1"));
	q->connect(SenLvlEdit, SIGNAL(textChanged(QString)), q, SLOT(SetSeniorityGroupLevel(QString)));
	q->connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), SenLvlEdit, SLOT(clear()));
	q->connect(q, SIGNAL(SetTo1(QString)), SenLvlEdit, SLOT(setText(QString)));
	q->connect(q, SIGNAL(ImportSeniorityGroupLevel(QString)), SenLvlEdit, SLOT(setText(QString)), Qt::QueuedConnection);
	ResLay->addWidget(SenLvlLbl, CountRow, 0);
	ResLay->addWidget(SenLvlEdit, CountRow, 1);
	CountRow++;
	SenLvlEdit->setText("1");

	ResLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), CountRow, 0, 1, 2);
	return Result;
}

QWidget* WaterfallStepHelperDialogPrivate::CreateICWidget()
{
    Q_Q(WaterfallStepHelperDialog);
	QWidget* Result = new QWidget(q);
	QGridLayout *ResLay = new QGridLayout(Result);
	int CountRow = 0;

	QLabel* SenGrpLbl = new QLabel(Result);
	SenGrpLbl->setText(q->tr("Seniority group to which the test applies"));
	QLineEdit* SenGrpEdit = new QLineEdit(Result);
	SenGrpEdit->setValidator(IntegerVector().GetValidator(SenGrpEdit));
	SenGrpEdit->setToolTip(q->tr("q is the pro-rata level of the tranches.\nAccepts vectors."));
	q->connect(SenGrpEdit, SIGNAL(textChanged(QString)), q, SLOT(SetSeniorityGroup(QString)));
	q->connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), SenGrpEdit, SLOT(clear()));
	q->connect(q, SIGNAL(ImportSeniorityGroup(QString)), SenGrpEdit, SLOT(setText(QString)), Qt::QueuedConnection);
	ResLay->addWidget(SenGrpLbl, CountRow, 0);
	ResLay->addWidget(SenGrpEdit, CountRow, 1);
	CountRow++;

	QLabel* SenLvlLbl = new QLabel(Result);
	SenLvlLbl->setText(q->tr("Level of seniority the seniority group refers to"));
	QLineEdit* SenLvlEdit = new QLineEdit(Result);
	SenLvlEdit->setValidator(IntegerVector().GetValidator(SenLvlEdit));
	SenLvlEdit->setToolTip(q->tr("q is the depth of the pro-rata group.\nAccepts vectors.\nNormally 1"));
	q->connect(SenLvlEdit, SIGNAL(textChanged(QString)), q, SLOT(SetSeniorityGroupLevel(QString)));
	q->connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), SenLvlEdit, SLOT(clear()));
	q->connect(q, SIGNAL(SetTo1(QString)), SenLvlEdit, SLOT(setText(QString)));
	q->connect(q, SIGNAL(ImportSeniorityGroupLevel(QString)), SenLvlEdit, SLOT(setText(QString)), Qt::QueuedConnection);
	ResLay->addWidget(SenLvlLbl, CountRow, 0);
	ResLay->addWidget(SenLvlEdit, CountRow, 1);
	CountRow++;
	SenLvlEdit->setText("1");

	QLabel* CoupIdxLbl = new QLabel(Result);
	CoupIdxLbl->setText(q->tr("Level coupon to be considered in the test"));
	QLineEdit* CoupIdxEdit = new QLineEdit(Result);
	CoupIdxEdit->setValidator(IntegerVector().GetValidator(CoupIdxEdit));
	CoupIdxEdit->setToolTip(q->tr("q is the coupon to be paid.\nAccepts vectors.\nNormally 1"));
	q->connect(CoupIdxEdit, SIGNAL(textChanged(QString)), q, SLOT(SetCouponIndex(QString)));
	q->connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), CoupIdxEdit, SLOT(clear()));
	q->connect(q, SIGNAL(SetTo1(QString)), CoupIdxEdit, SLOT(setText(QString)));
	q->connect(q, SIGNAL(ImportCouponIndex(QString)), CoupIdxEdit, SLOT(setText(QString)), Qt::QueuedConnection);
	ResLay->addWidget(CoupIdxLbl, CountRow, 0);
	ResLay->addWidget(CoupIdxEdit, CountRow, 1);
	CountRow++;
	CoupIdxEdit->setText("1");
	CoupIdxEdit->setEnabled(false);

	QLabel* TestOvrdLbl = new QLabel(Result);
	TestOvrdLbl->setText(q->tr("Override for the test trigger"));
	QLineEdit* TestOvrdEdit = new QLineEdit(Result);
	TestOvrdEdit->setValidator(BloombergVector().GetValidator(TestOvrdEdit));
	TestOvrdEdit->setToolTip(q->tr("q is the override for the test hurdle.\nAccepts vectors.\nLeave blank if no override applies"));
	q->connect(TestOvrdEdit, SIGNAL(textChanged(QString)), q, SLOT(SetTestTargetOverride(QString)));
	q->connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), TestOvrdEdit, SLOT(clear()));
	q->connect(q, SIGNAL(ImportTestTargetOverride(QString)), TestOvrdEdit, SLOT(setText(QString)), Qt::QueuedConnection);
	ResLay->addWidget(TestOvrdLbl, CountRow, 0);
	ResLay->addWidget(TestOvrdEdit, CountRow, 1);
	CountRow++;

	QLabel* SrcFndLbl = new QLabel(Result);
	SrcFndLbl->setText(q->tr("Accrue or Pay"));
	QComboBox* SrcFndCmb = new QComboBox(Result);
	SrcFndCmb->setEditable(false);
	SrcFndCmb->addItem("Calculate and Cure", static_cast<quint8>(WatFalPrior::wstAccrueOrPay::AccrueAndPay));
	SrcFndCmb->addItem("Calculate Only", static_cast<quint8>(WatFalPrior::wstAccrueOrPay::Accrue));
	SrcFndCmb->addItem("Cure Only", static_cast<quint8>(WatFalPrior::wstAccrueOrPay::Pay));
	q->connect(q, SIGNAL(SetPayAccrueCombo(int)), SrcFndCmb, SLOT(setCurrentIndex(int)));
	q->connect(SrcFndCmb, SIGNAL(currentIndexChanged(int)), q, SLOT(PayAccrueComboChanged(int)));
	ResLay->addWidget(SrcFndLbl, CountRow, 0);
	ResLay->addWidget(SrcFndCmb, CountRow, 1);
	CountRow++;

	ResLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), CountRow, 0, 1, 2);
	return Result;
}

QWidget* WaterfallStepHelperDialogPrivate::CreateOCWidget()
{
    Q_Q(WaterfallStepHelperDialog);
	QWidget* Result = new QWidget(q);
	QGridLayout *ResLay = new QGridLayout(Result);
	int CountRow = 0;

	QLabel* SenGrpLbl = new QLabel(Result);
	SenGrpLbl->setText(q->tr("Seniority group to which the test applies"));
	QLineEdit* SenGrpEdit = new QLineEdit(Result);
	SenGrpEdit->setValidator(IntegerVector().GetValidator(SenGrpEdit));
	SenGrpEdit->setToolTip(q->tr("q is the pro-rata level of the tranches.\nAccepts vectors."));
	q->connect(SenGrpEdit, SIGNAL(textChanged(QString)), q, SLOT(SetSeniorityGroup(QString)));
	q->connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), SenGrpEdit, SLOT(clear()));
	q->connect(q, SIGNAL(ImportSeniorityGroup(QString)), SenGrpEdit, SLOT(setText(QString)), Qt::QueuedConnection);
	ResLay->addWidget(SenGrpLbl, CountRow, 0);
	ResLay->addWidget(SenGrpEdit, CountRow, 1);
	CountRow++;

	QLabel* SenLvlLbl = new QLabel(Result);
	SenLvlLbl->setText(q->tr("Level of seniority the seniority group refers to"));
	QLineEdit* SenLvlEdit = new QLineEdit(Result);
	SenLvlEdit->setValidator(IntegerVector().GetValidator(SenLvlEdit));
	SenLvlEdit->setToolTip(q->tr("q is the depth of the pro-rata group.\nAccepts vectors.\nNormally 1"));
	q->connect(SenLvlEdit, SIGNAL(textChanged(QString)), q, SLOT(SetSeniorityGroupLevel(QString)));
	q->connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), SenLvlEdit, SLOT(clear()));
	q->connect(q, SIGNAL(SetTo1(QString)), SenLvlEdit, SLOT(setText(QString)));
	q->connect(q, SIGNAL(ImportSeniorityGroupLevel(QString)), SenLvlEdit, SLOT(setText(QString)), Qt::QueuedConnection);
	ResLay->addWidget(SenLvlLbl, CountRow, 0);
	ResLay->addWidget(SenLvlEdit, CountRow, 1);
	CountRow++;
	SenLvlEdit->setText("1");

	QLabel* RedGrpLbl = new QLabel(Result);
	RedGrpLbl->setText(q->tr("Group that will be redeemed through test cure payments"));
	QLineEdit* RedGrpEdit = new QLineEdit(Result);
	RedGrpEdit->setValidator(IntegerVector().GetValidator(RedGrpEdit));
	RedGrpEdit->setToolTip(q->tr("q is the pro-rata level of the tranches.\nAccepts vectors.\nLeave blank for sequential"));
	q->connect(RedGrpEdit, SIGNAL(textChanged(QString)), q, SLOT(SetRedemptionGroup(QString)));
	q->connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), RedGrpEdit, SLOT(clear()));
	q->connect(q, SIGNAL(ImportRedemptionGroup(QString)), RedGrpEdit, SLOT(setText(QString)), Qt::QueuedConnection);
	ResLay->addWidget(RedGrpLbl, CountRow, 0);
	ResLay->addWidget(RedGrpEdit, CountRow, 1);
	CountRow++;

	QLabel* RedLvlLbl = new QLabel(Result);
	RedLvlLbl->setText(q->tr("Level of seniority the redemption group refers to"));
	QLineEdit* RedLvlEdit = new QLineEdit(Result);
	RedLvlEdit->setValidator(IntegerVector().GetValidator(RedLvlEdit));
	RedLvlEdit->setToolTip(q->tr("q is the depth of the pro-rata group.\nAccepts vectors.\nNormally 1"));
	q->connect(RedLvlEdit, SIGNAL(textChanged(QString)), q, SLOT(SetRedemptionGroupLevel(QString)));
	q->connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), RedLvlEdit, SLOT(clear()));
	q->connect(q, SIGNAL(SetTo1(QString)), RedLvlEdit, SLOT(setText(QString)));
	q->connect(q, SIGNAL(ImportRedemptionGroupLevel(QString)), RedLvlEdit, SLOT(setText(QString)), Qt::QueuedConnection);
	ResLay->addWidget(RedLvlLbl, CountRow, 0);
	ResLay->addWidget(RedLvlEdit, CountRow, 1);
	CountRow++;
	RedLvlEdit->setText("1");

	QLabel* RedShrLbl = new QLabel(Result);
	RedShrLbl->setText(q->tr("Maximum share of available funds applied to redemption"));
	QLineEdit* RedShrEdit = new QLineEdit(Result);
	RedShrEdit->setValidator(BloombergVector().GetValidator(RedShrEdit));
	RedShrEdit->setToolTip(q->tr("Maximum share of available funds applied to redemption of the notes in order to cure the test.\nAccepts vectors."));
	q->connect(RedShrEdit, SIGNAL(textChanged(QString)), q, SLOT(SetRedemptionShare(QString)));
	q->connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), RedShrEdit, SLOT(clear()));
	q->connect(q, SIGNAL(SetTo100(QString)), RedShrEdit, SLOT(setText(QString)));
	q->connect(q, SIGNAL(ImportRedemptionShare(QString)), RedShrEdit, SLOT(setText(QString)), Qt::QueuedConnection);
	ResLay->addWidget(RedShrLbl, CountRow, 0);
	ResLay->addWidget(RedShrEdit, CountRow, 1);
	CountRow++;

	QLabel* ColShrLbl = new QLabel(Result);
	ColShrLbl->setText(q->tr("Maximum share of available funds applied to reinvestment"));
	QLineEdit* ColShrEdit = new QLineEdit(Result);
	ColShrEdit->setValidator(BloombergVector().GetValidator(ColShrEdit));
	ColShrEdit->setToolTip(q->tr("Maximum share of available funds applied to purchase of additional collateral in order to cure the test.\nAccepts vectors."));
	q->connect(ColShrEdit, SIGNAL(textChanged(QString)), q, SLOT(SetAdditionalCollateralShare(QString)));
	q->connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), ColShrEdit, SLOT(clear()));
	q->connect(q, SIGNAL(SetTo0(QString)), ColShrEdit, SLOT(setText(QString)));
	q->connect(q, SIGNAL(ImportAdditionalCollateralShare(QString)), ColShrEdit, SLOT(setText(QString)), Qt::QueuedConnection);
	ResLay->addWidget(ColShrLbl, CountRow, 0);
	ResLay->addWidget(ColShrEdit, CountRow, 1);
	CountRow++;


	QLabel* TestOvrdLbl = new QLabel(Result);
	TestOvrdLbl->setText(q->tr("Override for the test trigger"));
	QLineEdit* TestOvrdEdit = new QLineEdit(Result);
	TestOvrdEdit->setValidator(BloombergVector().GetValidator(TestOvrdEdit));
	TestOvrdEdit->setToolTip(q->tr("q is the override for the test hurdle.\nAccepts vectors.\nLeave blank if no override applies"));
	q->connect(TestOvrdEdit, SIGNAL(textChanged(QString)), q, SLOT(SetTestTargetOverride(QString)));
	q->connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), TestOvrdEdit, SLOT(clear()));
	q->connect(q, SIGNAL(ImportTestTargetOverride(QString)), TestOvrdEdit, SLOT(setText(QString)), Qt::QueuedConnection);
	ResLay->addWidget(TestOvrdLbl, CountRow, 0);
	ResLay->addWidget(TestOvrdEdit, CountRow, 1);
	CountRow++;

	QLabel* SrcFndLbl = new QLabel(Result);
	SrcFndLbl->setText(q->tr("Accrue or Pay"));
	QComboBox* SrcFndCmb = new QComboBox(Result);
	SrcFndCmb->setEditable(false);
	SrcFndCmb->addItem("Calculate and Cure", static_cast<quint8>(WatFalPrior::wstAccrueOrPay::AccrueAndPay));
	SrcFndCmb->addItem("Calculate Only", static_cast<quint8>(WatFalPrior::wstAccrueOrPay::Accrue));
	SrcFndCmb->addItem("Cure Only", static_cast<quint8>(WatFalPrior::wstAccrueOrPay::Pay));
	q->connect(q, SIGNAL(SetPayAccrueCombo(int)), SrcFndCmb, SLOT(setCurrentIndex(int)));
	q->connect(SrcFndCmb, SIGNAL(currentIndexChanged(int)), q, SLOT(PayAccrueComboChanged(int)));
	ResLay->addWidget(SrcFndLbl, CountRow, 0);
	ResLay->addWidget(SrcFndCmb, CountRow, 1);
	CountRow++;

	ResLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), CountRow, 0, 1, 2);
	return Result;
}

QWidget* WaterfallStepHelperDialogPrivate::CreateDeferredInterestWidget()
{
    Q_Q(WaterfallStepHelperDialog);
	QWidget* Result = new QWidget(q);
	QGridLayout *ResLay = new QGridLayout(Result);
	int CountRow = 0;

	QLabel* SenGrpLbl = new QLabel(Result);
	SenGrpLbl->setText(q->tr("Seniority group that will receive the deferred interest"));
	QLineEdit* SenGrpEdit = new QLineEdit(Result);
	SenGrpEdit->setValidator(IntegerVector().GetValidator(SenGrpEdit));
	SenGrpEdit->setToolTip(q->tr("q is the pro-rata level of the tranches.\nAccepts vectors."));
	q->connect(SenGrpEdit, SIGNAL(textChanged(QString)), q, SLOT(SetSeniorityGroup(QString)));
	q->connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), SenGrpEdit, SLOT(clear()));
	q->connect(q, SIGNAL(ImportSeniorityGroup(QString)), SenGrpEdit, SLOT(setText(QString)), Qt::QueuedConnection);
	ResLay->addWidget(SenGrpLbl, CountRow, 0);
	ResLay->addWidget(SenGrpEdit, CountRow, 1);
	CountRow++;

	QLabel* SenLvlLbl = new QLabel(Result);
	SenLvlLbl->setText(q->tr("Level of seniority the seniority group refers to"));
	QLineEdit* SenLvlEdit = new QLineEdit(Result);
	SenLvlEdit->setValidator(IntegerVector().GetValidator(SenLvlEdit));
	SenLvlEdit->setToolTip(q->tr("q is the depth of the pro-rata group.\nAccepts vectors.\nNormally 1"));
	q->connect(SenLvlEdit, SIGNAL(textChanged(QString)), q, SLOT(SetSeniorityGroupLevel(QString)));
	q->connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), SenLvlEdit, SLOT(clear()));
	q->connect(q, SIGNAL(SetTo1(QString)), SenLvlEdit, SLOT(setText(QString)));
	q->connect(q, SIGNAL(ImportSeniorityGroupLevel(QString)), SenLvlEdit, SLOT(setText(QString)), Qt::QueuedConnection);
	ResLay->addWidget(SenLvlLbl, CountRow, 0);
	ResLay->addWidget(SenLvlEdit, CountRow, 1);
	CountRow++;
	SenLvlEdit->setText("1");

	QLabel* CoupIdxLbl = new QLabel(Result);
	CoupIdxLbl->setText(q->tr("Level coupon to be paid"));
	QLineEdit* CoupIdxEdit = new QLineEdit(Result);
	CoupIdxEdit->setValidator(IntegerVector().GetValidator(CoupIdxEdit));
	CoupIdxEdit->setToolTip(q->tr("q is the coupon to be paid.\nAccepts vectors.\nNormally 1"));
	q->connect(CoupIdxEdit, SIGNAL(textChanged(QString)), q, SLOT(SetCouponIndex(QString)));
	q->connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), CoupIdxEdit, SLOT(clear()));
	q->connect(q, SIGNAL(SetTo1(QString)), CoupIdxEdit, SLOT(setText(QString)));
	q->connect(q, SIGNAL(ImportCouponIndex(QString)), CoupIdxEdit, SLOT(setText(QString)), Qt::QueuedConnection);
	ResLay->addWidget(CoupIdxLbl, CountRow, 0);
	ResLay->addWidget(CoupIdxEdit, CountRow, 1);
	CountRow++;
	CoupIdxEdit->setText("1");
	CoupIdxEdit->setEnabled(false);

	ResLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), CountRow, 0, 1, 2);
	return Result;
}

QWidget* WaterfallStepHelperDialogPrivate::CreateExcessWidget()
{
    Q_Q(WaterfallStepHelperDialog);
	QWidget* Result = new QWidget(q);
	QGridLayout *ResLay = new QGridLayout(Result);
	int CountRow = 0;
	QLabel* RedGrpLbl = new QLabel(Result);
	RedGrpLbl->setText(q->tr("Group that will receive excess spread"));
	QLineEdit* RedGrpEdit = new QLineEdit(Result);
	RedGrpEdit->setValidator(IntegerVector().GetValidator(RedGrpEdit));
	RedGrpEdit->setToolTip(q->tr("q is the pro-rata level of the tranches.\nAccepts vectors.\nLeave blank to allocate to an external \"Excess Spread\" account"));
	q->connect(RedGrpEdit, SIGNAL(textChanged(QString)), q, SLOT(SetRedemptionGroup(QString)));
	q->connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), RedGrpEdit, SLOT(clear()));
	q->connect(q, SIGNAL(ImportRedemptionGroup(QString)), RedGrpEdit, SLOT(setText(QString)), Qt::QueuedConnection);
	ResLay->addWidget(RedGrpLbl, CountRow, 0);
	ResLay->addWidget(RedGrpEdit, CountRow, 1);
	CountRow++;

	QLabel* RedLvlLbl = new QLabel(Result);
	RedLvlLbl->setText(q->tr("Level of seniority the redemption group refers to"));
	QLineEdit* RedLvlEdit = new QLineEdit(Result);
	RedLvlEdit->setValidator(IntegerVector().GetValidator(RedLvlEdit));
	RedLvlEdit->setToolTip(q->tr("q is the depth of the pro-rata group.\nAccepts vectors.\nNormally 1"));
	q->connect(RedLvlEdit, SIGNAL(textChanged(QString)), q, SLOT(SetRedemptionGroupLevel(QString)));
	q->connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), RedLvlEdit, SLOT(clear()));
	q->connect(q, SIGNAL(SetTo1(QString)), RedLvlEdit, SLOT(setText(QString)));
	q->connect(q, SIGNAL(ImportRedemptionGroupLevel(QString)), RedLvlEdit, SLOT(setText(QString)), Qt::QueuedConnection);
	ResLay->addWidget(RedLvlLbl, CountRow, 0);
	ResLay->addWidget(RedLvlEdit, CountRow, 1);
	CountRow++;
	RedLvlEdit->setText("1");

	QLabel* RedShrLbl = new QLabel(Result);
	RedShrLbl->setText(q->tr("Share of available funds applied to equity"));
	QLineEdit* RedShrEdit = new QLineEdit(Result);
	RedShrEdit->setValidator(BloombergVector().GetValidator(RedShrEdit));
	RedShrEdit->setToolTip(q->tr("Share of available funds paid to the notes, the remaining will go to the \"Excess Spread\" account\nAccepts vectors."));
	q->connect(RedShrEdit, SIGNAL(textChanged(QString)), q, SLOT(SetRedemptionShare(QString)));
	q->connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), RedShrEdit, SLOT(clear()));
	q->connect(q, SIGNAL(SetTo100(QString)), RedShrEdit, SLOT(setText(QString)));
	q->connect(q, SIGNAL(ImportRedemptionShare(QString)), RedShrEdit, SLOT(setText(QString)), Qt::QueuedConnection);
	ResLay->addWidget(RedShrLbl, CountRow, 0);
	ResLay->addWidget(RedShrEdit, CountRow, 1);
	CountRow++;

	ResLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), CountRow, 0, 1, 2);
	return Result;
}

QWidget* WaterfallStepHelperDialogPrivate::CreateReinvestPrincipalWidget()
{
    Q_Q(WaterfallStepHelperDialog);
	QWidget* Result = new QWidget(q);
	QGridLayout *ResLay = new QGridLayout(Result);
	int CountRow = 0;

	QLabel* SrcFndLbl = new QLabel(Result);
	SrcFndLbl->setText(q->tr("Type of funds to be applied"));
	QComboBox* SrcFndCmb = new QComboBox(Result);
	SrcFndCmb->setEditable(false);
	SrcFndCmb->addItem("All Principal", 3);
	SrcFndCmb->addItem("Prepayments", 1);
	SrcFndCmb->addItem("Scheduled", 2);
	q->connect(q, SIGNAL(SetSoFCombo(int)), SrcFndCmb, SLOT(setCurrentIndex(int)));
	q->connect(SrcFndCmb, SIGNAL(currentIndexChanged(int)), q, SLOT(SoFComboChanged(int)));
	ResLay->addWidget(SrcFndLbl, CountRow, 0);
	ResLay->addWidget(SrcFndCmb, CountRow, 1);
	CountRow++;

	QLabel* ColShrLbl = new QLabel(Result);
	ColShrLbl->setText(q->tr("Share of available funds applied to reinvestment"));
	QLineEdit* ColShrEdit = new QLineEdit(Result);
	ColShrEdit->setValidator(BloombergVector().GetValidator(ColShrEdit));
	ColShrEdit->setToolTip(q->tr("Share of available funds applied to purchase of additional collateral.\nAccepts vectors."));
	q->connect(ColShrEdit, SIGNAL(textChanged(QString)), q, SLOT(SetAdditionalCollateralShare(QString)));
	q->connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), ColShrEdit, SLOT(clear()));
	q->connect(q, SIGNAL(SetTo0(QString)), ColShrEdit, SLOT(setText(QString)));
	q->connect(q, SIGNAL(ImportAdditionalCollateralShare(QString)), ColShrEdit, SLOT(setText(QString)), Qt::QueuedConnection);
	ResLay->addWidget(ColShrLbl, CountRow, 0);
	ResLay->addWidget(ColShrEdit, CountRow, 1);
	CountRow++;

	ResLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), CountRow, 0, 1, 2);
	return Result;
}

QWidget* WaterfallStepHelperDialogPrivate::CreateReserveReplenishWidget()
{
    Q_Q(WaterfallStepHelperDialog);
	QWidget* Result = new QWidget(q);
	QGridLayout *ResLay = new QGridLayout(Result);
	int CountRow = 0;

	QLabel* ResIdxLbl = new QLabel(Result);
	ResIdxLbl->setText(q->tr("Index of the reserve to replenish"));
	QLineEdit* ResIdxEdit = new QLineEdit(Result);
	ResIdxEdit->setValidator(IntegerVector().GetValidator(ResIdxEdit));
	ResIdxEdit->setToolTip(q->tr("Accepts vectors."));
	q->connect(ResIdxEdit, SIGNAL(textChanged(QString)), q, SLOT(SetReserveIndex(QString)));
	q->connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), ResIdxEdit, SLOT(clear()));
	q->connect(q, SIGNAL(ImportReserveIndex(QString)), ResIdxEdit, SLOT(setText(QString)), Qt::QueuedConnection);
	ResLay->addWidget(ResIdxLbl, CountRow, 0);
	ResLay->addWidget(ResIdxEdit, CountRow, 1);
	CountRow++;

	ResLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), CountRow, 0, 1, 2);
	return Result;
}
QWidget* WaterfallStepHelperDialogPrivate::CreateTurboWidget()
{
    Q_Q(WaterfallStepHelperDialog);
	QWidget* Result = new QWidget(q);
	QGridLayout *ResLay = new QGridLayout(Result);
	int CountRow = 0;
	QLabel* RedGrpLbl = new QLabel(Result);
	RedGrpLbl->setText(q->tr("Group that will be redeemed"));
	QLineEdit* RedGrpEdit = new QLineEdit(Result);
	RedGrpEdit->setValidator(IntegerVector().GetValidator(RedGrpEdit));
	RedGrpEdit->setToolTip(q->tr("q is the pro-rata level of the tranches.\nAccepts vectors."));
	q->connect(RedGrpEdit, SIGNAL(textChanged(QString)), q, SLOT(SetRedemptionGroup(QString)));
	q->connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), RedGrpEdit, SLOT(clear()));
	q->connect(q, SIGNAL(ImportRedemptionGroup(QString)), RedGrpEdit, SLOT(setText(QString)), Qt::QueuedConnection);
	ResLay->addWidget(RedGrpLbl, CountRow, 0);
	ResLay->addWidget(RedGrpEdit, CountRow, 1);
	CountRow++;

	QLabel* RedLvlLbl = new QLabel(Result);
	RedLvlLbl->setText(q->tr("Level of seniority the redemption group refers to"));
	QLineEdit* RedLvlEdit = new QLineEdit(Result);
	RedLvlEdit->setValidator(IntegerVector().GetValidator(RedLvlEdit));
	RedLvlEdit->setToolTip(q->tr("q is the depth of the pro-rata group.\nAccepts vectors.\nNormally 1"));
	q->connect(RedLvlEdit, SIGNAL(textChanged(QString)), q, SLOT(SetRedemptionGroupLevel(QString)));
	q->connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), RedLvlEdit, SLOT(clear()));
	q->connect(q, SIGNAL(SetTo1(QString)), RedLvlEdit, SLOT(setText(QString)));
	q->connect(q, SIGNAL(ImportRedemptionGroupLevel(QString)), RedLvlEdit, SLOT(setText(QString)), Qt::QueuedConnection);
	ResLay->addWidget(RedLvlLbl, CountRow, 0);
	ResLay->addWidget(RedLvlEdit, CountRow, 1);
	CountRow++;
	RedLvlEdit->setText("1");

	QLabel* RedShrLbl = new QLabel(Result);
	RedShrLbl->setText(q->tr("Share of available funds applied to the indicated tranche"));
	QLineEdit* RedShrEdit = new QLineEdit(Result);
	RedShrEdit->setValidator(BloombergVector().GetValidator(RedShrEdit));
	RedShrEdit->setToolTip(q->tr("Share of available funds applied toward the redemption of to the notes.\nAccepts vectors."));
	q->connect(RedShrEdit, SIGNAL(textChanged(QString)), q, SLOT(SetRedemptionShare(QString)));
	q->connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), RedShrEdit, SLOT(clear()));
	q->connect(q, SIGNAL(SetTo100(QString)), RedShrEdit, SLOT(setText(QString)));
	q->connect(q, SIGNAL(ImportRedemptionShare(QString)), RedShrEdit, SLOT(setText(QString)), Qt::QueuedConnection);
	ResLay->addWidget(RedShrLbl, CountRow, 0);
	ResLay->addWidget(RedShrEdit, CountRow, 1);
	CountRow++;

	ResLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), CountRow, 0, 1, 2);
	return Result;
}

QWidget* WaterfallStepHelperDialogPrivate::CreateCurePDLWidget()
{
    Q_Q(WaterfallStepHelperDialog);
	QWidget* Result = new QWidget(q);
	QGridLayout *ResLay = new QGridLayout(Result);
	int CountRow = 0;

	QLabel* SenGrpLbl = new QLabel(Result);
	SenGrpLbl->setText(q->tr("Seniority group to which the PDL applies"));
	QLineEdit* SenGrpEdit = new QLineEdit(Result);
	SenGrpEdit->setValidator(IntegerVector().GetValidator(SenGrpEdit));
	SenGrpEdit->setToolTip(q->tr("q is the pro-rata level of the tranches.\nAccepts vectors."));
	q->connect(SenGrpEdit, SIGNAL(textChanged(QString)), q, SLOT(SetSeniorityGroup(QString)));
	q->connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), SenGrpEdit, SLOT(clear()));
	q->connect(q, SIGNAL(ImportSeniorityGroup(QString)), SenGrpEdit, SLOT(setText(QString)), Qt::QueuedConnection);
	ResLay->addWidget(SenGrpLbl, CountRow, 0);
	ResLay->addWidget(SenGrpEdit, CountRow, 1);
	CountRow++;

	QLabel* SenLvlLbl = new QLabel(Result);
	SenLvlLbl->setText(q->tr("Level of seniority the seniority group refers to"));
	QLineEdit* SenLvlEdit = new QLineEdit(Result);
	SenLvlEdit->setValidator(IntegerVector().GetValidator(SenLvlEdit));
	SenLvlEdit->setToolTip(q->tr("q is the depth of the pro-rata group.\nAccepts vectors.\nNormally 1"));
	q->connect(SenLvlEdit, SIGNAL(textChanged(QString)), q, SLOT(SetSeniorityGroupLevel(QString)));
	q->connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), SenLvlEdit, SLOT(clear()));
	q->connect(q, SIGNAL(SetTo1(QString)), SenLvlEdit, SLOT(setText(QString)));
	q->connect(q, SIGNAL(ImportSeniorityGroupLevel(QString)), SenLvlEdit, SLOT(setText(QString)), Qt::QueuedConnection);
	ResLay->addWidget(SenLvlLbl, CountRow, 0);
	ResLay->addWidget(SenLvlEdit, CountRow, 1);
	CountRow++;
	SenLvlEdit->setText("1");

	QLabel* RedGrpLbl = new QLabel(Result);
	RedGrpLbl->setText(q->tr("Group that will be redeemed to cure PDL"));
	QLineEdit* RedGrpEdit = new QLineEdit(Result);
	RedGrpEdit->setValidator(IntegerVector().GetValidator(RedGrpEdit));
	RedGrpEdit->setToolTip(q->tr("q is the pro-rata level of the tranches.\nAccepts vectors.\nLeave blank for sequential"));
	q->connect(RedGrpEdit, SIGNAL(textChanged(QString)), q, SLOT(SetRedemptionGroup(QString)));
	q->connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), RedGrpEdit, SLOT(clear()));
	q->connect(q, SIGNAL(ImportRedemptionGroup(QString)), RedGrpEdit, SLOT(setText(QString)), Qt::QueuedConnection);
	ResLay->addWidget(RedGrpLbl, CountRow, 0);
	ResLay->addWidget(RedGrpEdit, CountRow, 1);
	CountRow++;

	QLabel* RedLvlLbl = new QLabel(Result);
	RedLvlLbl->setText(q->tr("Level of seniority the redemption group refers to"));
	QLineEdit* RedLvlEdit = new QLineEdit(Result);
	RedLvlEdit->setValidator(IntegerVector().GetValidator(RedLvlEdit));
	RedLvlEdit->setToolTip(q->tr("q is the depth of the pro-rata group.\nAccepts vectors.\nNormally 1"));
	q->connect(RedLvlEdit, SIGNAL(textChanged(QString)), q, SLOT(SetRedemptionGroupLevel(QString)));
	q->connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), RedLvlEdit, SLOT(clear()));
	q->connect(q, SIGNAL(SetTo1(QString)), RedLvlEdit, SLOT(setText(QString)));
	q->connect(q, SIGNAL(ImportRedemptionGroupLevel(QString)), RedLvlEdit, SLOT(setText(QString)), Qt::QueuedConnection);
	ResLay->addWidget(RedLvlLbl, CountRow, 0);
	ResLay->addWidget(RedLvlEdit, CountRow, 1);
	CountRow++;
	RedLvlEdit->setText("1");

	QLabel* RedShrLbl = new QLabel(Result);
	RedShrLbl->setText(q->tr("Share of available funds applied to turbo"));
	QLineEdit* RedShrEdit = new QLineEdit(Result);
	RedShrEdit->setValidator(BloombergVector().GetValidator(RedShrEdit));
	RedShrEdit->setToolTip(q->tr("Share of available funds applied to redemption of the indicated class of notes, the remaining will go sequential.\nAccepts vectors."));
	q->connect(RedShrEdit, SIGNAL(textChanged(QString)), q, SLOT(SetRedemptionShare(QString)));
	q->connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), RedShrEdit, SLOT(clear()));
	q->connect(q, SIGNAL(SetTo0(QString)), RedShrEdit, SLOT(setText(QString)));
	q->connect(q, SIGNAL(ImportRedemptionShare(QString)), RedShrEdit, SLOT(setText(QString)), Qt::QueuedConnection);
	ResLay->addWidget(RedShrLbl, CountRow, 0);
	ResLay->addWidget(RedShrEdit, CountRow, 1);
	CountRow++;


	ResLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), CountRow, 0, 1, 2);
	return Result;
}

QWidget* WaterfallStepHelperDialogPrivate::CreateFeesFromXSWidget()
{
    Q_Q(WaterfallStepHelperDialog);
	QWidget* Result = new QWidget(q);
	QGridLayout *ResLay = new QGridLayout(Result);
	int CountRow = 0;
	QLabel* RedShrLbl = new QLabel(Result);
	RedShrLbl->setText(q->tr("Share of available funds applied paid as fees"));
	QLineEdit* RedShrEdit = new QLineEdit(Result);
	RedShrEdit->setValidator(BloombergVector().GetValidator(RedShrEdit));
	RedShrEdit->setToolTip(q->tr("Share of available funds paid as fees, q will appear as junior fees cash flow.\nAccepts vectors."));
	q->connect(RedShrEdit, SIGNAL(textChanged(QString)), q, SLOT(SetRedemptionShare(QString)));
	q->connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), RedShrEdit, SLOT(clear()));
	q->connect(q, SIGNAL(SetTo0(QString)), RedShrEdit, SLOT(setText(QString)));
	q->connect(q, SIGNAL(ImportRedemptionShare(QString)), RedShrEdit, SLOT(setText(QString)), Qt::QueuedConnection);
	ResLay->addWidget(RedShrLbl, CountRow, 0);
	ResLay->addWidget(RedShrEdit, CountRow, 1);
	CountRow++;
	ResLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), CountRow, 0, 1, 2);
	return Result;
}

QWidget* WaterfallStepHelperDialogPrivate::CreateAllocPrepayWidget()
{
    Q_Q(WaterfallStepHelperDialog);
	QWidget* Result = new QWidget(q);
	QGridLayout *ResLay = new QGridLayout(Result);
	int CountRow = 0;
	QLabel* RedGrpLbl = new QLabel(Result);
	RedGrpLbl->setText(q->tr("Group that will receive prepayment fees"));
	QLineEdit* RedGrpEdit = new QLineEdit(Result);
	RedGrpEdit->setValidator(IntegerVector().GetValidator(RedGrpEdit));
	RedGrpEdit->setToolTip(q->tr("q is the pro-rata level of the tranches.\nAccepts vectors."));
	q->connect(RedGrpEdit, SIGNAL(textChanged(QString)), q, SLOT(SetRedemptionGroup(QString)));
	q->connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), RedGrpEdit, SLOT(clear()));
	q->connect(q, SIGNAL(ImportRedemptionGroup(QString)), RedGrpEdit, SLOT(setText(QString)), Qt::QueuedConnection);
	ResLay->addWidget(RedGrpLbl, CountRow, 0);
	ResLay->addWidget(RedGrpEdit, CountRow, 1);
	CountRow++;

	QLabel* RedLvlLbl = new QLabel(Result);
	RedLvlLbl->setText(q->tr("Level of seniority the redemption group refers to"));
	QLineEdit* RedLvlEdit = new QLineEdit(Result);
	RedLvlEdit->setValidator(IntegerVector().GetValidator(RedLvlEdit));
	RedLvlEdit->setToolTip(q->tr("q is the depth of the pro-rata group.\nAccepts vectors.\nNormally 1"));
	q->connect(RedLvlEdit, SIGNAL(textChanged(QString)), q, SLOT(SetRedemptionGroupLevel(QString)));
	q->connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), RedLvlEdit, SLOT(clear()));
	q->connect(q, SIGNAL(SetTo1(QString)), RedLvlEdit, SLOT(setText(QString)));
	q->connect(q, SIGNAL(ImportRedemptionGroupLevel(QString)), RedLvlEdit, SLOT(setText(QString)), Qt::QueuedConnection);
	ResLay->addWidget(RedLvlLbl, CountRow, 0);
	ResLay->addWidget(RedLvlEdit, CountRow, 1);
	CountRow++;
	RedLvlEdit->setText("1");

	QLabel* RedShrLbl = new QLabel(Result);
	RedShrLbl->setText(q->tr("Share of prepayment fees applied to indicated tranche"));
	QLineEdit* RedShrEdit = new QLineEdit(Result);
	RedShrEdit->setValidator(BloombergVector().GetValidator(RedShrEdit));
	RedShrEdit->setToolTip(q->tr("Share of prepayment fees applied to the indicated notes.\nAccepts vectors."));
	q->connect(RedShrEdit, SIGNAL(textChanged(QString)), q, SLOT(SetRedemptionShare(QString)));
	q->connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), RedShrEdit, SLOT(clear()));
	q->connect(q, SIGNAL(SetTo100(QString)), RedShrEdit, SLOT(setText(QString)));
	q->connect(q, SIGNAL(ImportRedemptionShare(QString)), RedShrEdit, SLOT(setText(QString)), Qt::QueuedConnection);
	ResLay->addWidget(RedShrLbl, CountRow, 0);
	ResLay->addWidget(RedShrEdit, CountRow, 1);
	CountRow++;

	ResLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), CountRow, 0, 1, 2);
	return Result;
}







