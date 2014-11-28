#include "WaterfallStepHelperDialog.h"
#include <QComboBox>
#include <QStackedWidget>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QLineEdit>
#include "WatFalPrior.h"
#include "IntegerVector.h"
WaterfallStepHelperDialog::WaterfallStepHelperDialog(QWidget *parent)
	: QDialog(parent)
	, m_InterestWF(true)
	, FirstCombodeleted(false)
{
	setWindowIcon(QIcon(":/Icons/Logo.png"));
	setWindowTitle("Edit Waterfall Step");
	StepSelectorCombo = new QComboBox(this);
	StepSelectorCombo->setEditable(false);
	StepSelectorCombo->addItem("", -1);
	StepSelectorCombo->addItem("Senior Expenses", static_cast<qint16>(WatFalPrior::WaterfallStepType::wst_SeniorExpenses));
	StepSelectorCombo->addItem("Senior Fees", static_cast<qint16>(WatFalPrior::WaterfallStepType::wst_SeniorFees));
	StepSelectorCombo->addItem("Interest", static_cast<qint16>(WatFalPrior::WaterfallStepType::wst_Interest));
	StepSelectorCombo->addItem("Principal", static_cast<qint16>(WatFalPrior::WaterfallStepType::wst_Principal));
	StepSelectorCombo->addItem("OC Test", static_cast<qint16>(WatFalPrior::WaterfallStepType::wst_OCTest));
	StepSelectorCombo->addItem("IC Test", static_cast<qint16>(WatFalPrior::WaterfallStepType::wst_ICTest));
	StepSelectorCombo->addItem("Deferred Interest", static_cast<qint16>(WatFalPrior::WaterfallStepType::wst_DeferredInterest));
	StepSelectorCombo->addItem("Junior Fees", static_cast<qint16>(WatFalPrior::WaterfallStepType::wst_juniorFees));
	StepSelectorCombo->addItem("Reinvestment Test", static_cast<qint16>(WatFalPrior::WaterfallStepType::wst_ReinvestmentTest));
	StepSelectorCombo->addItem("Excess", static_cast<qint16>(WatFalPrior::WaterfallStepType::wst_Excess));
	StepSelectorCombo->addItem("Reinvest Principal", static_cast<qint16>(WatFalPrior::WaterfallStepType::wst_ReinvestPrincipal));
	StepSelectorCombo->addItem("Reserve Replenish", static_cast<qint16>(WatFalPrior::WaterfallStepType::wst_ReserveReplenish));
	StepSelectorCombo->addItem("Turbo", static_cast<qint16>(WatFalPrior::WaterfallStepType::wst_Turbo));
	StepSelectorCombo->addItem("PDL", static_cast<qint16>(WatFalPrior::WaterfallStepType::wst_PDL));
	StepSelectorCombo->addItem("Fees From Excess", static_cast<qint16>(WatFalPrior::WaterfallStepType::wst_FeesFromExcess));
	StepSelectorCombo->addItem("Allocate Prepay Fees", static_cast<qint16>(WatFalPrior::WaterfallStepType::wst_AllocPrepayFees));

	StepBuilderBase = new QStackedWidget(this);
	StepBuilderBase->setMinimumSize(200, 200);
	StepBuilderBase->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	StepBuilderBase->addWidget(new QWidget(this)); //No Step Selected
	StepBuilderBase->addWidget(new QWidget(this)); //Senior Expenses
	StepBuilderBase->addWidget(new QWidget(this)); //Senior Fees
	StepBuilderBase->addWidget(CreateInterestWidget()); //Interest
	StepBuilderBase->addWidget(CreatePrincipalWidget()); //Principal
	connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), StepBuilderBase, SLOT(setCurrentIndex(int)));
	connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(ClearParameters()));
	connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(SetDefaults1()));
	connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(CheckOkEnabled(int)),Qt::QueuedConnection);
	QLabel *StepSelectLabel = new QLabel(this);
	StepSelectLabel->setText(tr("Select Step Type"));

	
	AcceptButton = new QPushButton(this);
	AcceptButton->setText(tr("OK"));
	AcceptButton->setDefault(true);
	AcceptButton->setEnabled(false);
	connect(AcceptButton, &QPushButton::clicked, this, &QDialog::accept);
	QPushButton*  CancelButton = new QPushButton(this);
	CancelButton->setText(tr("Cancel"));
	CancelButton->setDefault(false);
	connect(CancelButton, &QPushButton::clicked, this, &QDialog::reject);

	QGroupBox *TopGroup = new QGroupBox(this);
	TopGroup->setTitle("Step");
	QGroupBox *ParamGroup = new QGroupBox(this);
	ParamGroup->setTitle("Parameters");
	QHBoxLayout* ParamLay = new QHBoxLayout(ParamGroup);
	ParamLay->addWidget(StepBuilderBase);
	QHBoxLayout* TopLay = new QHBoxLayout(TopGroup);
	TopLay->addWidget(StepSelectLabel);
	TopLay->addWidget(StepSelectorCombo);

	QHBoxLayout* ButtonsLay = new QHBoxLayout;
	ButtonsLay->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));
	ButtonsLay->addWidget(AcceptButton);
	ButtonsLay->addWidget(CancelButton);

	QVBoxLayout* mainlay = new QVBoxLayout(this);
	mainlay->addWidget(TopGroup);
	mainlay->addWidget(ParamGroup);
	mainlay->addLayout(ButtonsLay);

}

void WaterfallStepHelperDialog::CheckOkEnabled(int index) {
	if (FirstCombodeleted) return;
	AcceptButton->setEnabled(index != 0);
	if (index != 0) {
		QWidget* EmptyWidg = StepBuilderBase->widget(0);
		StepBuilderBase->removeWidget(EmptyWidg);
		StepBuilderBase->setCurrentIndex(StepBuilderBase->currentIndex() - 1);
		EmptyWidg->deleteLater();
		FirstCombodeleted = true;
		StepSelectorCombo->removeItem(0);
		disconnect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(CheckOkEnabled(int)));
	}
}



void WaterfallStepHelperDialog::SetSeniorityGroup(QString value) {
	SetParameter(static_cast<qint32>(WatFalPrior::wstParameters::SeniorityGroup), value);
}

void WaterfallStepHelperDialog::SetSeniorityGroupLevel(QString value) {
	SetParameter(static_cast<qint32>(WatFalPrior::wstParameters::SeniorityGroupLevel), value);
}

void WaterfallStepHelperDialog::SetRedemptionGroup(QString value) {
	SetParameter(static_cast<qint32>(WatFalPrior::wstParameters::RedemptionGroup), value);
}

void WaterfallStepHelperDialog::SetRedemptionGroupLevel(QString value) {
	SetParameter(static_cast<qint32>(WatFalPrior::wstParameters::RedemptionGroupLevel), value);
}

void WaterfallStepHelperDialog::SetRedemptionShare(QString value) {
	SetParameter(static_cast<qint32>(WatFalPrior::wstParameters::RedemptionShare), value);
}

void WaterfallStepHelperDialog::SetAdditionalCollateralShare(QString value) {
	SetParameter(static_cast<qint32>(WatFalPrior::wstParameters::AdditionalCollateralShare), value);
}

void WaterfallStepHelperDialog::SetSourceOfFunding(QString value) {
	SetParameter(static_cast<qint32>(WatFalPrior::wstParameters::SourceOfFunding), value);
}

void WaterfallStepHelperDialog::SetCouponIndex(QString value) {
	SetParameter(static_cast<qint32>(WatFalPrior::wstParameters::CouponIndex), value);
}

void WaterfallStepHelperDialog::SetTestTargetOverride(QString value) {
	SetParameter(static_cast<qint32>(WatFalPrior::wstParameters::TestTargetOverride), value);
}

void WaterfallStepHelperDialog::SetIRRtoEquityTarget(QString value) {
	SetParameter(static_cast<qint32>(WatFalPrior::wstParameters::IRRtoEquityTarget), value);
}

void WaterfallStepHelperDialog::SetReserveIndex(QString value) {
	SetParameter(static_cast<qint32>(WatFalPrior::wstParameters::ReserveIndex), value);
}

void WaterfallStepHelperDialog::SetTrigger(QString value) {
	SetParameter(static_cast<qint32>(WatFalPrior::wstParameters::Trigger), value);
}

QStringList WaterfallStepHelperDialog::GetParameters() const {
	QStringList Result;
	for (int i = 0; i < NumwstParameters; ++i) Result << ResultingParameters[i];
	return Result;
}

void WaterfallStepHelperDialog::SetDefaults1() {
	emit SetDefaults("1");
}
QWidget* WaterfallStepHelperDialog::CreateInterestWidget() {
	QWidget* Result = new QWidget(this);
	QGridLayout *ResLay = new QGridLayout(Result);
	int CountRow = 0;

	QLabel* SenGrpLbl = new QLabel(Result);
	SenGrpLbl->setText(tr("Seniority group that will receive the interest"));
	QLineEdit* SenGrpEdit = new QLineEdit(Result);
	SenGrpEdit->setValidator(IntegerVector().GetValidator(SenGrpEdit));
	SenGrpEdit->setToolTip(tr("This is the pro-rata level of the tranches.\nAccepts vectors."));
	connect(SenGrpEdit, SIGNAL(textChanged(QString)), this, SLOT(SetSeniorityGroup(QString)));
	connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), SenGrpEdit, SLOT(clear()));
	ResLay->addWidget(SenGrpLbl, CountRow, 0);
	ResLay->addWidget(SenGrpEdit, CountRow, 1);
	CountRow++;

	QLabel* SenLvlLbl = new QLabel(Result);
	SenLvlLbl->setText(tr("Level of seniority the seniority group refers to"));
	QLineEdit* SenLvlEdit = new QLineEdit(Result);
	SenLvlEdit->setValidator(IntegerVector().GetValidator(SenGrpEdit));
	SenLvlEdit->setToolTip(tr("This is the depth of the pro-rata group.\nAccepts vectors.\nNormally 1"));
	connect(SenLvlEdit, SIGNAL(textChanged(QString)), this, SLOT(SetSeniorityGroupLevel(QString)));
	connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), SenLvlEdit, SLOT(clear()));
	connect(this, SIGNAL(SetDefaults(QString)), SenLvlEdit, SLOT(setText(QString)));
	ResLay->addWidget(SenLvlLbl, CountRow, 0);
	ResLay->addWidget(SenLvlEdit, CountRow, 1);
	CountRow++;
	SenLvlEdit->setText("1");

	QLabel* CoupIdxLbl = new QLabel(Result);
	CoupIdxLbl->setText(tr("Level coupon to be paid"));
	QLineEdit* CoupIdxEdit = new QLineEdit(Result);
	CoupIdxEdit->setValidator(IntegerVector().GetValidator(SenGrpEdit));
	CoupIdxEdit->setToolTip(tr("This is the coupon to be paid.\nAccepts vectors.\nNormally 1"));
	connect(CoupIdxEdit, SIGNAL(textChanged(QString)), this, SLOT(SetCouponIndex(QString)));
	connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), CoupIdxEdit, SLOT(clear()));
	connect(this, SIGNAL(SetDefaults(QString)), CoupIdxEdit, SLOT(setText(QString)));
	ResLay->addWidget(CoupIdxLbl, CountRow, 0);
	ResLay->addWidget(CoupIdxEdit, CountRow, 1);
	CountRow++;
	CoupIdxEdit->setText("1");
	CoupIdxEdit->setEnabled(false);

	ResLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), CountRow, 0, 1, 2);
	return Result;
}
QWidget* WaterfallStepHelperDialog::CreatePrincipalWidget() {
	QWidget* Result = new QWidget(this);
	QGridLayout *ResLay = new QGridLayout(Result);
	int CountRow = 0;

	QLabel* SenGrpLbl = new QLabel(Result);
	SenGrpLbl->setText(tr("Seniority group that will be redeemed"));
	QLineEdit* SenGrpEdit = new QLineEdit(Result);
	SenGrpEdit->setValidator(IntegerVector().GetValidator(SenGrpEdit));
	SenGrpEdit->setToolTip(tr("This is the pro-rata level of the tranches.\nAccepts vectors."));
	connect(SenGrpEdit, SIGNAL(textChanged(QString)), this, SLOT(SetSeniorityGroup(QString)));
	connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), SenGrpEdit, SLOT(clear()));
	ResLay->addWidget(SenGrpLbl, CountRow, 0);
	ResLay->addWidget(SenGrpEdit, CountRow, 1);
	CountRow++;

	QLabel* SenLvlLbl = new QLabel(Result);
	SenLvlLbl->setText(tr("Level of seniority the seniority group refers to"));
	QLineEdit* SenLvlEdit = new QLineEdit(Result);
	SenLvlEdit->setValidator(IntegerVector().GetValidator(SenGrpEdit));
	SenLvlEdit->setToolTip(tr("This is the depth of the pro-rata group.\nAccepts vectors.\nNormally 1"));
	connect(SenLvlEdit, SIGNAL(textChanged(QString)), this, SLOT(SetSeniorityGroupLevel(QString)));
	connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), SenLvlEdit, SLOT(clear()));
	connect(this, SIGNAL(SetDefaults(QString)), SenLvlEdit, SLOT(setText(QString)));
	ResLay->addWidget(SenLvlLbl, CountRow, 0);
	ResLay->addWidget(SenLvlEdit, CountRow, 1);
	CountRow++;
	SenLvlEdit->setText("1");

	ResLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), CountRow, 0, 1, 2);
	return Result;
}
