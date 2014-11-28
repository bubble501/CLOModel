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
#include "BloombergVector.h"
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
	StepBuilderBase->setStyleSheet("QLabel { qproperty-alignment: 'AlignRight | AlignVCenter'; }");
	StepBuilderBase->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	StepBuilderBase->addWidget(new QWidget(this)); //No Step Selected
	StepBuilderBase->addWidget(new QWidget(this)); //Senior Expenses
	StepBuilderBase->addWidget(new QWidget(this)); //Senior Fees
	StepBuilderBase->addWidget(CreateInterestWidget()); //Interest
	StepBuilderBase->addWidget(CreatePrincipalWidget()); //Principal
	StepBuilderBase->addWidget(CreateOCWidget()); //OC Test
	StepBuilderBase->addWidget(CreateICWidget()); //IC Test
	StepBuilderBase->addWidget(CreateDeferredInterestWidget()); //Deferred Interest
	StepBuilderBase->addWidget(new QWidget(this)); //Junior Fees
	StepBuilderBase->addWidget(CreateOCWidget()); //Reinvestment Test
	StepBuilderBase->addWidget(CreateExcessWidget()); //Excess
	StepBuilderBase->addWidget(CreateReinvestPrincipalWidget()); //Reinvest
	StepBuilderBase->addWidget(CreateReserveReplenishWidget()); //Reserve Replenish
	StepBuilderBase->addWidget(CreateTurboWidget()); //Turbo
	StepBuilderBase->addWidget(CreateCurePDLWidget()); //PDL Cure
	StepBuilderBase->addWidget(CreateFeesFromXSWidget()); //Fees From Excess
	StepBuilderBase->addWidget(CreateAllocPrepayWidget()); //Allocate Prepay Fees

	connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), StepBuilderBase, SLOT(setCurrentIndex(int)));
	connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(ClearParameters()));
	connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(SetDefaults1()), Qt::QueuedConnection);
	connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(SetDefaults100()), Qt::QueuedConnection);
	connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(SetDefaults0()), Qt::QueuedConnection);
	connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(SetbasedOnWaterfall(int)), Qt::QueuedConnection);
	connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(CheckOkEnabled(int)),Qt::QueuedConnection);
	connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(ResetSoFCombo(int)), Qt::QueuedConnection);
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
	if (!sender()) return;
	if (StepBuilderBase->currentWidget()->children().contains(sender()))
		SetParameter(static_cast<qint32>(WatFalPrior::wstParameters::SeniorityGroup), value);
}

void WaterfallStepHelperDialog::SetSeniorityGroupLevel(QString value) {
	if (!sender()) return;
	if (StepBuilderBase->currentWidget()->children().contains(sender()))
		SetParameter(static_cast<qint32>(WatFalPrior::wstParameters::SeniorityGroupLevel), value);
}

void WaterfallStepHelperDialog::SetRedemptionGroup(QString value) {
	if (!sender()) return;
	if (StepBuilderBase->currentWidget()->children().contains(sender()))
		SetParameter(static_cast<qint32>(WatFalPrior::wstParameters::RedemptionGroup), value);
}

void WaterfallStepHelperDialog::SetRedemptionGroupLevel(QString value) {
	if (!sender()) return;
	if (StepBuilderBase->currentWidget()->children().contains(sender()))
		SetParameter(static_cast<qint32>(WatFalPrior::wstParameters::RedemptionGroupLevel), value);
}

void WaterfallStepHelperDialog::SetRedemptionShare(QString value) {
	if (!sender()) return;
	if (StepBuilderBase->currentWidget()->children().contains(sender()))
		SetParameter(static_cast<qint32>(WatFalPrior::wstParameters::RedemptionShare), value);
}

void WaterfallStepHelperDialog::SetAdditionalCollateralShare(QString value) {
	if (!sender()) return;
	if (StepBuilderBase->currentWidget()->children().contains(sender()))
		SetParameter(static_cast<qint32>(WatFalPrior::wstParameters::AdditionalCollateralShare), value);
}

void WaterfallStepHelperDialog::SetSourceOfFunding(QString value) {
	if (!sender()) return;
	if (StepBuilderBase->currentWidget()->children().contains(sender()))
		SetParameter(static_cast<qint32>(WatFalPrior::wstParameters::SourceOfFunding), value);
}

void WaterfallStepHelperDialog::SetCouponIndex(QString value) {
	if (!sender()) return;
	if (StepBuilderBase->currentWidget()->children().contains(sender()))
		SetParameter(static_cast<qint32>(WatFalPrior::wstParameters::CouponIndex), value);
}

void WaterfallStepHelperDialog::SetTestTargetOverride(QString value) {
	if (!sender()) return;
	if (StepBuilderBase->currentWidget()->children().contains(sender()))
		SetParameter(static_cast<qint32>(WatFalPrior::wstParameters::TestTargetOverride), value);
}

void WaterfallStepHelperDialog::SetIRRtoEquityTarget(QString value) {
	if (!sender()) return;
	if (StepBuilderBase->currentWidget()->children().contains(sender()))
		SetParameter(static_cast<qint32>(WatFalPrior::wstParameters::IRRtoEquityTarget), value);
}

void WaterfallStepHelperDialog::SetReserveIndex(QString value) {
	if (!sender()) return;
	if (StepBuilderBase->currentWidget()->children().contains(sender()))
		SetParameter(static_cast<qint32>(WatFalPrior::wstParameters::ReserveIndex), value);
}

void WaterfallStepHelperDialog::SetTrigger(QString value) {
	if (!sender()) return;
	if (StepBuilderBase->currentWidget()->children().contains(sender()))
		SetParameter(static_cast<qint32>(WatFalPrior::wstParameters::Trigger), value);
}
void WaterfallStepHelperDialog::SoFComboChanged(int index) {
	if (!sender()) return;
	if (StepBuilderBase->currentWidget()->children().contains(sender()))
		ResultingParameters[static_cast<qint32>(WatFalPrior::wstParameters::SourceOfFunding)] = QString::number((index == 0) ? 3 : index);
}
QString WaterfallStepHelperDialog::GetParameters() const {
	QString Result = QString::number(StepSelectorCombo->currentData().toInt());
	Result += '#' + ResultingParameters.value(static_cast<qint32>(WatFalPrior::wstParameters::SeniorityGroup), "");
	Result += '#' + ResultingParameters.value(static_cast<qint32>(WatFalPrior::wstParameters::SeniorityGroupLevel), "") ;
	Result += '#' + ResultingParameters.value(static_cast<qint32>(WatFalPrior::wstParameters::RedemptionGroup), "") ;
	Result += '#' + ResultingParameters.value(static_cast<qint32>(WatFalPrior::wstParameters::RedemptionGroupLevel), "") ;
	Result += '#' + ResultingParameters.value(static_cast<qint32>(WatFalPrior::wstParameters::RedemptionShare), "") ;
	Result += '#' + ResultingParameters.value(static_cast<qint32>(WatFalPrior::wstParameters::AdditionalCollateralShare), "") ;
	Result += '#' + ResultingParameters.value(static_cast<qint32>(WatFalPrior::wstParameters::SourceOfFunding), "") ;
	Result += '#' + ResultingParameters.value(static_cast<qint32>(WatFalPrior::wstParameters::CouponIndex), "") ;
	Result += '#' + ResultingParameters.value(static_cast<qint32>(WatFalPrior::wstParameters::TestTargetOverride), "") ;
	Result += '#' + ResultingParameters.value(static_cast<qint32>(WatFalPrior::wstParameters::IRRtoEquityTarget), "") ;
	Result += '#' + ResultingParameters.value(static_cast<qint32>(WatFalPrior::wstParameters::ReserveIndex), "") ;
	Result += '#' + ResultingParameters.value(static_cast<qint32>(WatFalPrior::wstParameters::Trigger), "") ;
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
void WaterfallStepHelperDialog::ResetSoFCombo(int index) {
	if (StepSelectorCombo->itemData(index).toInt() == static_cast<qint32>(WatFalPrior::WaterfallStepType::wst_ReinvestPrincipal)) {
		ResultingParameters[static_cast<qint32>(WatFalPrior::wstParameters::SourceOfFunding)] = "3";
		emit SetSoFCombo(0);
	}
}
void WaterfallStepHelperDialog::SetbasedOnWaterfall(int index) {
	switch (static_cast<WatFalPrior::WaterfallStepType>(StepSelectorCombo->itemData(index).toInt())) {
	case WatFalPrior::WaterfallStepType::wst_SeniorExpenses:
	case WatFalPrior::WaterfallStepType::wst_SeniorFees:
	case WatFalPrior::WaterfallStepType::wst_juniorFees:
	case WatFalPrior::WaterfallStepType::wst_OCTest:
	case WatFalPrior::WaterfallStepType::wst_ICTest:
	case WatFalPrior::WaterfallStepType::wst_ReinvestmentTest:
	case WatFalPrior::WaterfallStepType::wst_Excess:
	case WatFalPrior::WaterfallStepType::wst_ReserveReplenish:
	case WatFalPrior::WaterfallStepType::wst_FeesFromExcess:
	case WatFalPrior::WaterfallStepType::wst_AllocPrepayFees :
		ResultingParameters[static_cast<qint32>(WatFalPrior::wstParameters::SourceOfFunding)] = (m_InterestWF ? QString("1") : QString("2")); break;
	}
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
	connect(this, SIGNAL(SetTo1(QString)), SenLvlEdit, SLOT(setText(QString)));
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
	connect(this, SIGNAL(SetTo1(QString)), CoupIdxEdit, SLOT(setText(QString)));
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
	connect(this, SIGNAL(SetTo1(QString)), SenLvlEdit, SLOT(setText(QString)));
	ResLay->addWidget(SenLvlLbl, CountRow, 0);
	ResLay->addWidget(SenLvlEdit, CountRow, 1);
	CountRow++;
	SenLvlEdit->setText("1");

	ResLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), CountRow, 0, 1, 2);
	return Result;
}

QWidget* WaterfallStepHelperDialog::CreateICWidget() {
	QWidget* Result = new QWidget(this);
	QGridLayout *ResLay = new QGridLayout(Result);
	int CountRow = 0;

	QLabel* SenGrpLbl = new QLabel(Result);
	SenGrpLbl->setText(tr("Seniority group to which the test applies"));
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
	connect(this, SIGNAL(SetTo1(QString)), SenLvlEdit, SLOT(setText(QString)));
	ResLay->addWidget(SenLvlLbl, CountRow, 0);
	ResLay->addWidget(SenLvlEdit, CountRow, 1);
	CountRow++;
	SenLvlEdit->setText("1");

	QLabel* CoupIdxLbl = new QLabel(Result);
	CoupIdxLbl->setText(tr("Level coupon to be considered in the test"));
	QLineEdit* CoupIdxEdit = new QLineEdit(Result);
	CoupIdxEdit->setValidator(IntegerVector().GetValidator(SenGrpEdit));
	CoupIdxEdit->setToolTip(tr("This is the coupon to be paid.\nAccepts vectors.\nNormally 1"));
	connect(CoupIdxEdit, SIGNAL(textChanged(QString)), this, SLOT(SetCouponIndex(QString)));
	connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), CoupIdxEdit, SLOT(clear()));
	connect(this, SIGNAL(SetTo1(QString)), CoupIdxEdit, SLOT(setText(QString)));
	ResLay->addWidget(CoupIdxLbl, CountRow, 0);
	ResLay->addWidget(CoupIdxEdit, CountRow, 1);
	CountRow++;
	CoupIdxEdit->setText("1");
	CoupIdxEdit->setEnabled(false);

	QLabel* TestOvrdLbl = new QLabel(Result);
	TestOvrdLbl->setText(tr("Override for the test trigger"));
	QLineEdit* TestOvrdEdit = new QLineEdit(Result);
	TestOvrdEdit->setValidator(BloombergVector().GetValidator(SenGrpEdit));
	TestOvrdEdit->setToolTip(tr("This is the override for the test hurdle.\nAccepts vectors.\nLeave blank if no override applies"));
	connect(TestOvrdEdit, SIGNAL(textChanged(QString)), this, SLOT(SetTestTargetOverride(QString)));
	connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), TestOvrdEdit, SLOT(clear()));
	ResLay->addWidget(TestOvrdLbl, CountRow, 0);
	ResLay->addWidget(TestOvrdEdit, CountRow, 1);
	CountRow++;

	ResLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), CountRow, 0, 1, 2);
	return Result;
}

QWidget* WaterfallStepHelperDialog::CreateOCWidget() {
	QWidget* Result = new QWidget(this);
	QGridLayout *ResLay = new QGridLayout(Result);
	int CountRow = 0;

	QLabel* SenGrpLbl = new QLabel(Result);
	SenGrpLbl->setText(tr("Seniority group to which the test applies"));
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
	SenLvlEdit->setValidator(IntegerVector().GetValidator(SenLvlEdit));
	SenLvlEdit->setToolTip(tr("This is the depth of the pro-rata group.\nAccepts vectors.\nNormally 1"));
	connect(SenLvlEdit, SIGNAL(textChanged(QString)), this, SLOT(SetSeniorityGroupLevel(QString)));
	connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), SenLvlEdit, SLOT(clear()));
	connect(this, SIGNAL(SetTo1(QString)), SenLvlEdit, SLOT(setText(QString)));
	ResLay->addWidget(SenLvlLbl, CountRow, 0);
	ResLay->addWidget(SenLvlEdit, CountRow, 1);
	CountRow++;
	SenLvlEdit->setText("1");

	QLabel* RedGrpLbl = new QLabel(Result);
	RedGrpLbl->setText(tr("Group that will be redeemed through test cure payments"));
	QLineEdit* RedGrpEdit = new QLineEdit(Result);
	RedGrpEdit->setValidator(IntegerVector().GetValidator(RedGrpEdit));
	RedGrpEdit->setToolTip(tr("This is the pro-rata level of the tranches.\nAccepts vectors.\nLeave blank for sequential"));
	connect(RedGrpEdit, SIGNAL(textChanged(QString)), this, SLOT(SetRedemptionGroup(QString)));
	connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), RedGrpEdit, SLOT(clear()));
	ResLay->addWidget(RedGrpLbl, CountRow, 0);
	ResLay->addWidget(RedGrpEdit, CountRow, 1);
	CountRow++;

	QLabel* RedLvlLbl = new QLabel(Result);
	RedLvlLbl->setText(tr("Level of seniority the redemption group refers to"));
	QLineEdit* RedLvlEdit = new QLineEdit(Result);
	RedLvlEdit->setValidator(IntegerVector().GetValidator(RedLvlEdit));
	RedLvlEdit->setToolTip(tr("This is the depth of the pro-rata group.\nAccepts vectors.\nNormally 1"));
	connect(RedLvlEdit, SIGNAL(textChanged(QString)), this, SLOT(SetRedemptionGroupLevel(QString)));
	connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), RedLvlEdit, SLOT(clear()));
	connect(this, SIGNAL(SetTo1(QString)), RedLvlEdit, SLOT(setText(QString)));
	ResLay->addWidget(RedLvlLbl, CountRow, 0);
	ResLay->addWidget(RedLvlEdit, CountRow, 1);
	CountRow++;
	RedLvlEdit->setText("1");

	QLabel* RedShrLbl = new QLabel(Result);
	RedShrLbl->setText(tr("Maximum share of available funds applied to redemption"));
	QLineEdit* RedShrEdit = new QLineEdit(Result);
	RedShrEdit->setValidator(BloombergVector().GetValidator(RedShrEdit));
	RedShrEdit->setToolTip(tr("Maximum share of available funds applied to redemption of the notes in order to cure the test.\nAccepts vectors."));
	connect(RedShrEdit, SIGNAL(textChanged(QString)), this, SLOT(SetRedemptionShare(QString)));
	connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), RedShrEdit, SLOT(clear()));
	connect(this, SIGNAL(SetTo100(QString)), RedShrEdit, SLOT(setText(QString)));
	ResLay->addWidget(RedShrLbl, CountRow, 0);
	ResLay->addWidget(RedShrEdit, CountRow, 1);
	CountRow++;

	QLabel* ColShrLbl = new QLabel(Result);
	ColShrLbl->setText(tr("Maximum share of available funds applied to reinvestment"));
	QLineEdit* ColShrEdit = new QLineEdit(Result);
	ColShrEdit->setValidator(BloombergVector().GetValidator(ColShrEdit));
	ColShrEdit->setToolTip(tr("Maximum share of available funds applied to purchase of additional collateral in order to cure the test.\nAccepts vectors."));
	connect(ColShrEdit, SIGNAL(textChanged(QString)), this, SLOT(SetAdditionalCollateralShare(QString)));
	connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), ColShrEdit, SLOT(clear()));
	connect(this, SIGNAL(SetTo0(QString)), ColShrEdit, SLOT(setText(QString)));
	ResLay->addWidget(ColShrLbl, CountRow, 0);
	ResLay->addWidget(ColShrEdit, CountRow, 1);
	CountRow++;


	QLabel* TestOvrdLbl = new QLabel(Result);
	TestOvrdLbl->setText(tr("Override for the test trigger"));
	QLineEdit* TestOvrdEdit = new QLineEdit(Result);
	TestOvrdEdit->setValidator(BloombergVector().GetValidator(TestOvrdEdit));
	TestOvrdEdit->setToolTip(tr("This is the override for the test hurdle.\nAccepts vectors.\nLeave blank if no override applies"));
	connect(TestOvrdEdit, SIGNAL(textChanged(QString)), this, SLOT(SetTestTargetOverride(QString)));
	connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), TestOvrdEdit, SLOT(clear()));
	ResLay->addWidget(TestOvrdLbl, CountRow, 0);
	ResLay->addWidget(TestOvrdEdit, CountRow, 1);
	CountRow++;

	ResLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), CountRow, 0, 1, 2);
	return Result;
}

QWidget* WaterfallStepHelperDialog::CreateDeferredInterestWidget() {
	QWidget* Result = new QWidget(this);
	QGridLayout *ResLay = new QGridLayout(Result);
	int CountRow = 0;

	QLabel* SenGrpLbl = new QLabel(Result);
	SenGrpLbl->setText(tr("Seniority group that will receive the deferred interest"));
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
	SenLvlEdit->setValidator(IntegerVector().GetValidator(SenLvlEdit));
	SenLvlEdit->setToolTip(tr("This is the depth of the pro-rata group.\nAccepts vectors.\nNormally 1"));
	connect(SenLvlEdit, SIGNAL(textChanged(QString)), this, SLOT(SetSeniorityGroupLevel(QString)));
	connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), SenLvlEdit, SLOT(clear()));
	connect(this, SIGNAL(SetTo1(QString)), SenLvlEdit, SLOT(setText(QString)));
	ResLay->addWidget(SenLvlLbl, CountRow, 0);
	ResLay->addWidget(SenLvlEdit, CountRow, 1);
	CountRow++;
	SenLvlEdit->setText("1");

	QLabel* CoupIdxLbl = new QLabel(Result);
	CoupIdxLbl->setText(tr("Level coupon to be paid"));
	QLineEdit* CoupIdxEdit = new QLineEdit(Result);
	CoupIdxEdit->setValidator(IntegerVector().GetValidator(CoupIdxEdit));
	CoupIdxEdit->setToolTip(tr("This is the coupon to be paid.\nAccepts vectors.\nNormally 1"));
	connect(CoupIdxEdit, SIGNAL(textChanged(QString)), this, SLOT(SetCouponIndex(QString)));
	connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), CoupIdxEdit, SLOT(clear()));
	connect(this, SIGNAL(SetTo1(QString)), CoupIdxEdit, SLOT(setText(QString)));
	ResLay->addWidget(CoupIdxLbl, CountRow, 0);
	ResLay->addWidget(CoupIdxEdit, CountRow, 1);
	CountRow++;
	CoupIdxEdit->setText("1");
	CoupIdxEdit->setEnabled(false);

	ResLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), CountRow, 0, 1, 2);
	return Result;
}

QWidget* WaterfallStepHelperDialog::CreateExcessWidget() {
	QWidget* Result = new QWidget(this);
	QGridLayout *ResLay = new QGridLayout(Result);
	int CountRow = 0;
	QLabel* RedGrpLbl = new QLabel(Result);
	RedGrpLbl->setText(tr("Group that will receive excess spread"));
	QLineEdit* RedGrpEdit = new QLineEdit(Result);
	RedGrpEdit->setValidator(IntegerVector().GetValidator(RedGrpEdit));
	RedGrpEdit->setToolTip(tr("This is the pro-rata level of the tranches.\nAccepts vectors.\nLeave blank to allocate to an external \"Excess Spread\" account"));
	connect(RedGrpEdit, SIGNAL(textChanged(QString)), this, SLOT(SetRedemptionGroup(QString)));
	connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), RedGrpEdit, SLOT(clear()));
	ResLay->addWidget(RedGrpLbl, CountRow, 0);
	ResLay->addWidget(RedGrpEdit, CountRow, 1);
	CountRow++;

	QLabel* RedLvlLbl = new QLabel(Result);
	RedLvlLbl->setText(tr("Level of seniority the redemption group refers to"));
	QLineEdit* RedLvlEdit = new QLineEdit(Result);
	RedLvlEdit->setValidator(IntegerVector().GetValidator(RedLvlEdit));
	RedLvlEdit->setToolTip(tr("This is the depth of the pro-rata group.\nAccepts vectors.\nNormally 1"));
	connect(RedLvlEdit, SIGNAL(textChanged(QString)), this, SLOT(SetRedemptionGroupLevel(QString)));
	connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), RedLvlEdit, SLOT(clear()));
	connect(this, SIGNAL(SetTo1(QString)), RedLvlEdit, SLOT(setText(QString)));
	ResLay->addWidget(RedLvlLbl, CountRow, 0);
	ResLay->addWidget(RedLvlEdit, CountRow, 1);
	CountRow++;
	RedLvlEdit->setText("1");

	QLabel* RedShrLbl = new QLabel(Result);
	RedShrLbl->setText(tr("Share of available funds applied to equity"));
	QLineEdit* RedShrEdit = new QLineEdit(Result);
	RedShrEdit->setValidator(BloombergVector().GetValidator(RedShrEdit));
	RedShrEdit->setToolTip(tr("Share of available funds paid to the notes, the remaining will go to the \"Excess Spread\" account\nAccepts vectors."));
	connect(RedShrEdit, SIGNAL(textChanged(QString)), this, SLOT(SetRedemptionShare(QString)));
	connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), RedShrEdit, SLOT(clear()));
	connect(this, SIGNAL(SetTo100(QString)), RedShrEdit, SLOT(setText(QString)));
	ResLay->addWidget(RedShrLbl, CountRow, 0);
	ResLay->addWidget(RedShrEdit, CountRow, 1);
	CountRow++;

	ResLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), CountRow, 0, 1, 2);
	return Result;
}

QWidget* WaterfallStepHelperDialog::CreateReinvestPrincipalWidget() {
	QWidget* Result = new QWidget(this);
	QGridLayout *ResLay = new QGridLayout(Result);
	int CountRow = 0;

	QLabel* SrcFndLbl = new QLabel(Result);
	SrcFndLbl->setText(tr("Type of funds to be applied"));
	QComboBox* SrcFndCmb = new QComboBox(Result);
	SrcFndCmb->setEditable(false);
	SrcFndCmb->addItem("All Principal", 3);
	SrcFndCmb->addItem("Prepayments", 1);
	SrcFndCmb->addItem("Scheduled", 2);
	connect(this, SIGNAL(SetSoFCombo(int)), SrcFndCmb, SLOT(setCurrentIndex(int)));
	connect(SrcFndCmb, SIGNAL(currentIndexChanged(int)), this, SLOT(SoFComboChanged(int)));

	ResLay->addWidget(SrcFndLbl, CountRow, 0);
	ResLay->addWidget(SrcFndCmb, CountRow, 1);
	CountRow++;

	QLabel* ColShrLbl = new QLabel(Result);
	ColShrLbl->setText(tr("Share of available funds applied to reinvestment"));
	QLineEdit* ColShrEdit = new QLineEdit(Result);
	ColShrEdit->setValidator(BloombergVector().GetValidator(ColShrEdit));
	ColShrEdit->setToolTip(tr("Share of available funds applied to purchase of additional collateral.\nAccepts vectors."));
	connect(ColShrEdit, SIGNAL(textChanged(QString)), this, SLOT(SetAdditionalCollateralShare(QString)));
	connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), ColShrEdit, SLOT(clear()));
	connect(this, SIGNAL(SetTo0(QString)), ColShrEdit, SLOT(setText(QString)));
	ResLay->addWidget(ColShrLbl, CountRow, 0);
	ResLay->addWidget(ColShrEdit, CountRow, 1);
	CountRow++;

	ResLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), CountRow, 0, 1, 2);
	return Result;
}

QWidget* WaterfallStepHelperDialog::CreateReserveReplenishWidget() {
	QWidget* Result = new QWidget(this);
	QGridLayout *ResLay = new QGridLayout(Result);
	int CountRow = 0;

	QLabel* RedLvlLbl = new QLabel(Result);
	RedLvlLbl->setText(tr("Index of the reserve to replenish"));
	QLineEdit* RedLvlEdit = new QLineEdit(Result);
	RedLvlEdit->setValidator(IntegerVector().GetValidator(RedLvlEdit));
	RedLvlEdit->setToolTip(tr("This is the depth of the pro-rata group.\nAccepts vectors.\nNormally 1"));
	connect(RedLvlEdit, SIGNAL(textChanged(QString)), this, SLOT(SetReserveIndex(QString)));
	connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), RedLvlEdit, SLOT(clear()));
	ResLay->addWidget(RedLvlLbl, CountRow, 0);
	ResLay->addWidget(RedLvlEdit, CountRow, 1);
	CountRow++;

	ResLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), CountRow, 0, 1, 2);
	return Result;
}
QWidget* WaterfallStepHelperDialog::CreateTurboWidget() {
	QWidget* Result = new QWidget(this);
	QGridLayout *ResLay = new QGridLayout(Result);
	int CountRow = 0;
	QLabel* RedGrpLbl = new QLabel(Result);
	RedGrpLbl->setText(tr("Group that will be redeemed"));
	QLineEdit* RedGrpEdit = new QLineEdit(Result);
	RedGrpEdit->setValidator(IntegerVector().GetValidator(RedGrpEdit));
	RedGrpEdit->setToolTip(tr("This is the pro-rata level of the tranches.\nAccepts vectors."));
	connect(RedGrpEdit, SIGNAL(textChanged(QString)), this, SLOT(SetRedemptionGroup(QString)));
	connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), RedGrpEdit, SLOT(clear()));
	ResLay->addWidget(RedGrpLbl, CountRow, 0);
	ResLay->addWidget(RedGrpEdit, CountRow, 1);
	CountRow++;

	QLabel* RedLvlLbl = new QLabel(Result);
	RedLvlLbl->setText(tr("Level of seniority the redemption group refers to"));
	QLineEdit* RedLvlEdit = new QLineEdit(Result);
	RedLvlEdit->setValidator(IntegerVector().GetValidator(RedLvlEdit));
	RedLvlEdit->setToolTip(tr("This is the depth of the pro-rata group.\nAccepts vectors.\nNormally 1"));
	connect(RedLvlEdit, SIGNAL(textChanged(QString)), this, SLOT(SetRedemptionGroupLevel(QString)));
	connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), RedLvlEdit, SLOT(clear()));
	connect(this, SIGNAL(SetTo1(QString)), RedLvlEdit, SLOT(setText(QString)));
	ResLay->addWidget(RedLvlLbl, CountRow, 0);
	ResLay->addWidget(RedLvlEdit, CountRow, 1);
	CountRow++;
	RedLvlEdit->setText("1");

	QLabel* RedShrLbl = new QLabel(Result);
	RedShrLbl->setText(tr("Share of available funds applied to the indicated tranche"));
	QLineEdit* RedShrEdit = new QLineEdit(Result);
	RedShrEdit->setValidator(BloombergVector().GetValidator(RedShrEdit));
	RedShrEdit->setToolTip(tr("Share of available funds applied toward the redemption of to the notes.\nAccepts vectors."));
	connect(RedShrEdit, SIGNAL(textChanged(QString)), this, SLOT(SetRedemptionShare(QString)));
	connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), RedShrEdit, SLOT(clear()));
	connect(this, SIGNAL(SetTo100(QString)), RedShrEdit, SLOT(setText(QString)));
	ResLay->addWidget(RedShrLbl, CountRow, 0);
	ResLay->addWidget(RedShrEdit, CountRow, 1);
	CountRow++;

	ResLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), CountRow, 0, 1, 2);
	return Result;
}

QWidget* WaterfallStepHelperDialog::CreateCurePDLWidget() {
	QWidget* Result = new QWidget(this);
	QGridLayout *ResLay = new QGridLayout(Result);
	int CountRow = 0;

	QLabel* SenGrpLbl = new QLabel(Result);
	SenGrpLbl->setText(tr("Seniority group to which the PDL applies"));
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
	SenLvlEdit->setValidator(IntegerVector().GetValidator(SenLvlEdit));
	SenLvlEdit->setToolTip(tr("This is the depth of the pro-rata group.\nAccepts vectors.\nNormally 1"));
	connect(SenLvlEdit, SIGNAL(textChanged(QString)), this, SLOT(SetSeniorityGroupLevel(QString)));
	connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), SenLvlEdit, SLOT(clear()));
	connect(this, SIGNAL(SetTo1(QString)), SenLvlEdit, SLOT(setText(QString)));
	ResLay->addWidget(SenLvlLbl, CountRow, 0);
	ResLay->addWidget(SenLvlEdit, CountRow, 1);
	CountRow++;
	SenLvlEdit->setText("1");

	QLabel* RedGrpLbl = new QLabel(Result);
	RedGrpLbl->setText(tr("Group that will be redeemed to cure PDL"));
	QLineEdit* RedGrpEdit = new QLineEdit(Result);
	RedGrpEdit->setValidator(IntegerVector().GetValidator(RedGrpEdit));
	RedGrpEdit->setToolTip(tr("This is the pro-rata level of the tranches.\nAccepts vectors.\nLeave blank for sequential"));
	connect(RedGrpEdit, SIGNAL(textChanged(QString)), this, SLOT(SetRedemptionGroup(QString)));
	connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), RedGrpEdit, SLOT(clear()));
	ResLay->addWidget(RedGrpLbl, CountRow, 0);
	ResLay->addWidget(RedGrpEdit, CountRow, 1);
	CountRow++;

	QLabel* RedLvlLbl = new QLabel(Result);
	RedLvlLbl->setText(tr("Level of seniority the redemption group refers to"));
	QLineEdit* RedLvlEdit = new QLineEdit(Result);
	RedLvlEdit->setValidator(IntegerVector().GetValidator(RedLvlEdit));
	RedLvlEdit->setToolTip(tr("This is the depth of the pro-rata group.\nAccepts vectors.\nNormally 1"));
	connect(RedLvlEdit, SIGNAL(textChanged(QString)), this, SLOT(SetRedemptionGroupLevel(QString)));
	connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), RedLvlEdit, SLOT(clear()));
	connect(this, SIGNAL(SetTo1(QString)), RedLvlEdit, SLOT(setText(QString)));
	ResLay->addWidget(RedLvlLbl, CountRow, 0);
	ResLay->addWidget(RedLvlEdit, CountRow, 1);
	CountRow++;
	RedLvlEdit->setText("1");

	QLabel* RedShrLbl = new QLabel(Result);
	RedShrLbl->setText(tr("Share of available funds applied to turbo"));
	QLineEdit* RedShrEdit = new QLineEdit(Result);
	RedShrEdit->setValidator(BloombergVector().GetValidator(RedShrEdit));
	RedShrEdit->setToolTip(tr("Share of available funds applied to redemption of the indicated class of notes, the remaining will go sequential.\nAccepts vectors."));
	connect(RedShrEdit, SIGNAL(textChanged(QString)), this, SLOT(SetRedemptionShare(QString)));
	connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), RedShrEdit, SLOT(clear()));
	connect(this, SIGNAL(SetTo0(QString)), RedShrEdit, SLOT(setText(QString)));
	ResLay->addWidget(RedShrLbl, CountRow, 0);
	ResLay->addWidget(RedShrEdit, CountRow, 1);
	CountRow++;


	ResLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), CountRow, 0, 1, 2);
	return Result;
}

QWidget* WaterfallStepHelperDialog::CreateFeesFromXSWidget() {
	QWidget* Result = new QWidget(this);
	QGridLayout *ResLay = new QGridLayout(Result);
	int CountRow = 0;
	QLabel* RedShrLbl = new QLabel(Result);
	RedShrLbl->setText(tr("Share of available funds applied paid as fees"));
	QLineEdit* RedShrEdit = new QLineEdit(Result);
	RedShrEdit->setValidator(BloombergVector().GetValidator(RedShrEdit));
	RedShrEdit->setToolTip(tr("Share of available funds paid as fees, this will appear as junior fees cash flow.\nAccepts vectors."));
	connect(RedShrEdit, SIGNAL(textChanged(QString)), this, SLOT(SetRedemptionShare(QString)));
	connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), RedShrEdit, SLOT(clear()));
	connect(this, SIGNAL(SetTo0(QString)), RedShrEdit, SLOT(setText(QString)));
	ResLay->addWidget(RedShrLbl, CountRow, 0);
	ResLay->addWidget(RedShrEdit, CountRow, 1);
	CountRow++;
	ResLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), CountRow, 0, 1, 2);
	return Result;
}

QWidget* WaterfallStepHelperDialog::CreateAllocPrepayWidget() {
	QWidget* Result = new QWidget(this);
	QGridLayout *ResLay = new QGridLayout(Result);
	int CountRow = 0;
	QLabel* RedGrpLbl = new QLabel(Result);
	RedGrpLbl->setText(tr("Group that will recieve prepayment fees"));
	QLineEdit* RedGrpEdit = new QLineEdit(Result);
	RedGrpEdit->setValidator(IntegerVector().GetValidator(RedGrpEdit));
	RedGrpEdit->setToolTip(tr("This is the pro-rata level of the tranches.\nAccepts vectors."));
	connect(RedGrpEdit, SIGNAL(textChanged(QString)), this, SLOT(SetRedemptionGroup(QString)));
	connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), RedGrpEdit, SLOT(clear()));
	ResLay->addWidget(RedGrpLbl, CountRow, 0);
	ResLay->addWidget(RedGrpEdit, CountRow, 1);
	CountRow++;

	QLabel* RedLvlLbl = new QLabel(Result);
	RedLvlLbl->setText(tr("Level of seniority the redemption group refers to"));
	QLineEdit* RedLvlEdit = new QLineEdit(Result);
	RedLvlEdit->setValidator(IntegerVector().GetValidator(RedLvlEdit));
	RedLvlEdit->setToolTip(tr("This is the depth of the pro-rata group.\nAccepts vectors.\nNormally 1"));
	connect(RedLvlEdit, SIGNAL(textChanged(QString)), this, SLOT(SetRedemptionGroupLevel(QString)));
	connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), RedLvlEdit, SLOT(clear()));
	connect(this, SIGNAL(SetTo1(QString)), RedLvlEdit, SLOT(setText(QString)));
	ResLay->addWidget(RedLvlLbl, CountRow, 0);
	ResLay->addWidget(RedLvlEdit, CountRow, 1);
	CountRow++;
	RedLvlEdit->setText("1");

	QLabel* RedShrLbl = new QLabel(Result);
	RedShrLbl->setText(tr("Share of prepayment fees applied to indicated tranche"));
	QLineEdit* RedShrEdit = new QLineEdit(Result);
	RedShrEdit->setValidator(BloombergVector().GetValidator(RedShrEdit));
	RedShrEdit->setToolTip(tr("Share of prepayment fees applied to the indicated notes.\nAccepts vectors."));
	connect(RedShrEdit, SIGNAL(textChanged(QString)), this, SLOT(SetRedemptionShare(QString)));
	connect(StepSelectorCombo, SIGNAL(currentIndexChanged(int)), RedShrEdit, SLOT(clear()));
	connect(this, SIGNAL(SetTo100(QString)), RedShrEdit, SLOT(setText(QString)));
	ResLay->addWidget(RedShrLbl, CountRow, 0);
	ResLay->addWidget(RedShrEdit, CountRow, 1);
	CountRow++;

	ResLay->addItem(new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding), CountRow, 0, 1, 2);
	return Result;
}



