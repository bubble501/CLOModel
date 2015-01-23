#include "WaterfallViewer.h"
#include "Waterfall.h"
#include <QTableWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QAbstractItemView>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QScrollBar>
WaterfallViewer::WaterfallViewer(QWidget* parent/* =0 */)
	:QWidget(parent)
{
	setWindowIcon(QIcon(":/Icons/Logo.png"));
	setWindowTitle("Waterfalls");
	setMinimumSize(480,640);
	QStringList HeadersStrings;
	HeadersStrings 
		<< "Step"
		<< "Seniority Group"
		<< "Seniority Group Level"
		<< "Redemption Group"
		<< "Redemption Group Level"
		<< "Redemption Share"
		<< "Additional Collateral Share"
		<< "Source of Funding"
		<< "Coupon Index"
		<< "Test Target Override"
		<< "IRR to Equity Target"
		<< "Reserve Index"
		<< "Triggers"
	;

	InterestTable=new QTableWidget(this);
	InterestTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	InterestTable->setRowCount(0);
	InterestTable->setColumnCount(HeadersStrings.size());
	InterestTable->setHorizontalHeaderLabels(HeadersStrings);
	InterestTable->verticalHeader()->setVisible(false);
	InterestTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	//InterestTable->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	InterestTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	InterestTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	InterestTable->setObjectName("InterestTable");
	InterestLabel=new QLabel(this);
	InterestLabel->setObjectName("InterestLabel");
	InterestLabel->setText("<b>Interest Waterfall</b>");
	QVBoxLayout* InterestLay=new QVBoxLayout;
	InterestLay->addWidget(InterestLabel);
	InterestLay->addWidget(InterestTable);

	PrincipalTable=new QTableWidget(this);
	PrincipalTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	PrincipalTable->setRowCount(0);
	PrincipalTable->setColumnCount(HeadersStrings.size());
	PrincipalTable->setHorizontalHeaderLabels(HeadersStrings);
	PrincipalTable->verticalHeader()->setVisible(false);
	PrincipalTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	//PrincipalTable->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	PrincipalTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	PrincipalTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	PrincipalTable->setObjectName("PrincipalTable");
	PrincipalLabel=new QLabel(this);
	PrincipalLabel->setObjectName("PrincipalLabel");
	PrincipalLabel->setText("<b>Principal Waterfall</b>");
	QVBoxLayout* PrincipalLay=new QVBoxLayout;
	PrincipalLay->addWidget(PrincipalLabel);
	PrincipalLay->addWidget(PrincipalTable);

	QHBoxLayout* MainLay=new QHBoxLayout(this);
	MainLay->addLayout(InterestLay);
	MainLay->addLayout(PrincipalLay);

}
void WaterfallViewer::ResetSteps(){
	InterestTable->setRowCount(0);
	PrincipalTable->setRowCount(0);
	m_IWUsedCols.clear();
	m_PWUsedCols.clear();
}
void WaterfallViewer::AddStep(
	WatFalPrior::WaterfallStepType Step
	, const QString& ArgSeniorityGroup
	, const QString& ArgSeniorityGroupLevel
	, const QString& ArgRedemptionGroup
	, const QString& ArgRedemptionGroupLevel
	, const QString& ArgRedemptionShare
	, const QString& ArgAdditionalCollateralShare
	, const QString& ArgSourceofFunding
	, const QString& ArgCouponIndex
	, const QString& ArgTestTargetOverride
	, const QString& ArgIRRtoEquityTarget
	, const QString& ArgReserveIndex
	, const QString& ArgTrigger
	){
	WatFalPrior TempStep;
	TempStep.SetPriorityType(Step);
	if (!ArgSeniorityGroup.isEmpty()) { TempStep.SetParameter(WatFalPrior::wstParameters::SeniorityGroup, ArgSeniorityGroup); }
	if (!ArgSeniorityGroupLevel.isEmpty()) { TempStep.SetParameter(WatFalPrior::wstParameters::SeniorityGroupLevel, ArgSeniorityGroupLevel); }
	if (!ArgRedemptionGroup.isEmpty()) { TempStep.SetParameter(WatFalPrior::wstParameters::RedemptionGroup, ArgRedemptionGroup); }
	if (!ArgRedemptionGroupLevel.isEmpty()) { TempStep.SetParameter(WatFalPrior::wstParameters::RedemptionGroupLevel, ArgRedemptionGroupLevel); }
	if (!ArgRedemptionShare.isEmpty()) { TempStep.SetParameter(WatFalPrior::wstParameters::RedemptionShare, ArgRedemptionShare); }
	if (!ArgAdditionalCollateralShare.isEmpty()) { TempStep.SetParameter(WatFalPrior::wstParameters::AdditionalCollateralShare, ArgAdditionalCollateralShare); }
	if (!ArgSourceofFunding.isEmpty()) { TempStep.SetParameter(WatFalPrior::wstParameters::SourceOfFunding, ArgSourceofFunding); }
	if (!ArgCouponIndex.isEmpty()) { TempStep.SetParameter(WatFalPrior::wstParameters::CouponIndex, ArgCouponIndex); }
	if (!ArgTestTargetOverride.isEmpty()) { TempStep.SetParameter(WatFalPrior::wstParameters::TestTargetOverride, ArgTestTargetOverride); }
	if (!ArgIRRtoEquityTarget.isEmpty()) { TempStep.SetParameter(WatFalPrior::wstParameters::IRRtoEquityTarget, ArgIRRtoEquityTarget); }
	if (!ArgReserveIndex.isEmpty()) { TempStep.SetParameter(WatFalPrior::wstParameters::ReserveIndex, ArgReserveIndex); }
	AddStep(TempStep);
}
void WaterfallViewer::AddStep(const WatFalPrior& a){
	QTableWidget* ApplicableTable=nullptr;
	QSet<qint16>* ApplicableUsedCols=nullptr;
	switch(a.GetPriorityType()){
	case WatFalPrior::WaterfallStepType::wst_SeniorExpenses:
		if (a.GetParameter(WatFalPrior::wstParameters::SourceOfFunding).value<IntegerVector>().GetValue(0) == 1) {
			ApplicableTable = InterestTable;
			ApplicableUsedCols = &m_IWUsedCols;
		}
		else {
			ApplicableTable = PrincipalTable;
			ApplicableUsedCols = &m_PWUsedCols;
		}
		ApplicableUsedCols->insert(ciStep);
		ApplicableTable->setRowCount(ApplicableTable->rowCount() + 1);
		ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciStep, new QTableWidgetItem("Senior Expenses"));		
	break;
	case WatFalPrior::WaterfallStepType::wst_SeniorFees:
		if (a.GetParameter(WatFalPrior::wstParameters::SourceOfFunding).value<IntegerVector>().GetValue(0) == 1) {
			ApplicableTable = InterestTable;
			ApplicableUsedCols = &m_IWUsedCols;
		}
		else {
			ApplicableTable = PrincipalTable;
			ApplicableUsedCols = &m_PWUsedCols;
		}
		ApplicableUsedCols->insert(ciStep);
		ApplicableTable->setRowCount(ApplicableTable->rowCount() + 1);
		ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciStep, new QTableWidgetItem("Senior Fees"));
	break;
	case WatFalPrior::WaterfallStepType::wst_Interest:
		m_IWUsedCols << ciStep << ciSeniorityGroup << ciSeniorityGroupLevel << ciCouponIndex;
		InterestTable->setRowCount(InterestTable->rowCount()+1);
		InterestTable->setItem(InterestTable->rowCount() - 1, ciStep, new QTableWidgetItem("Interest"));
		InterestTable->setItem(InterestTable->rowCount() - 1, ciSeniorityGroup, new QTableWidgetItem(a.GetParameter(WatFalPrior::wstParameters::SeniorityGroup).value<IntegerVector>().GetVector()));
		InterestTable->setItem(InterestTable->rowCount() - 1, ciSeniorityGroupLevel, new QTableWidgetItem(a.GetParameter(WatFalPrior::wstParameters::SeniorityGroupLevel).value<IntegerVector>().GetVector()));
		InterestTable->setItem(InterestTable->rowCount() - 1, ciCouponIndex, new QTableWidgetItem(a.GetParameter(WatFalPrior::wstParameters::CouponIndex).value<IntegerVector>().GetVector()));
	break;
	case WatFalPrior::WaterfallStepType::wst_Principal:
		m_PWUsedCols << ciStep << ciSeniorityGroup << ciSeniorityGroupLevel;
		PrincipalTable->setRowCount(PrincipalTable->rowCount()+1);
		PrincipalTable->setItem(PrincipalTable->rowCount() - 1, ciStep, new QTableWidgetItem("Principal"));
		PrincipalTable->setItem(PrincipalTable->rowCount() - 1, ciSeniorityGroup, new QTableWidgetItem(a.GetParameter(WatFalPrior::wstParameters::SeniorityGroup).value<IntegerVector>().GetVector()));
		PrincipalTable->setItem(PrincipalTable->rowCount() - 1, ciSeniorityGroupLevel, new QTableWidgetItem(a.GetParameter(WatFalPrior::wstParameters::SeniorityGroupLevel).value<IntegerVector>().GetVector()));
	break;
	case WatFalPrior::WaterfallStepType::wst_ReinvestmentTest:
	case WatFalPrior::WaterfallStepType::wst_OCTest:
		if (a.GetParameter(WatFalPrior::wstParameters::SourceOfFunding).value<IntegerVector>().GetValue(0) == 1) {
			ApplicableTable = InterestTable;
			ApplicableUsedCols = &m_IWUsedCols;
		}
		else {
			ApplicableTable = PrincipalTable;
			ApplicableUsedCols = &m_PWUsedCols;
		}
		(*ApplicableUsedCols) << ciStep << ciSeniorityGroup << ciSeniorityGroupLevel;
		ApplicableTable->setRowCount(ApplicableTable->rowCount() + 1);
		ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciStep, new QTableWidgetItem((a.GetPriorityType() == WatFalPrior::WaterfallStepType::wst_OCTest) ? QString("OC Test") : QString("Reinvestment Test")));
		ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciSeniorityGroup, new QTableWidgetItem(a.GetParameter(WatFalPrior::wstParameters::SeniorityGroup).value<IntegerVector>().GetVector()));
		ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciSeniorityGroupLevel, new QTableWidgetItem(a.GetParameter(WatFalPrior::wstParameters::SeniorityGroupLevel).value<IntegerVector>().GetVector()));
		if (a.HasParameter(WatFalPrior::wstParameters::RedemptionGroup)) {
			(*ApplicableUsedCols) << ciRedemptionGroup << ciRedemptionGroupLevel << ciRedemptionShare;
			ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciRedemptionGroup, new QTableWidgetItem(a.GetParameter(WatFalPrior::wstParameters::RedemptionGroup).value<IntegerVector>().GetVector()));
			ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciRedemptionGroupLevel, new QTableWidgetItem(a.GetParameter(WatFalPrior::wstParameters::RedemptionGroupLevel).value<IntegerVector>().GetVector()));
			ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciRedemptionShare, new QTableWidgetItem(a.GetParameter(WatFalPrior::wstParameters::RedemptionShare).value<BloombergVector>().GetVector()));
		}
		if (a.HasParameter(WatFalPrior::wstParameters::AdditionalCollateralShare)) {
			(*ApplicableUsedCols) << ciAdditionalCollateralShare;
			ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciAdditionalCollateralShare, new QTableWidgetItem(a.GetParameter(WatFalPrior::wstParameters::AdditionalCollateralShare).value<BloombergVector>().GetVector()));
		}		
		if (a.HasParameter(WatFalPrior::wstParameters::TestTargetOverride)) {
			(*ApplicableUsedCols) << ciTestTargetOverride;
			ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciTestTargetOverride, new QTableWidgetItem(a.GetParameter(WatFalPrior::wstParameters::TestTargetOverride).value<BloombergVector>().GetVector()));
		}
	break;
	case WatFalPrior::WaterfallStepType::wst_ICTest:
		if (a.GetParameter(WatFalPrior::wstParameters::SourceOfFunding).value<IntegerVector>().GetValue(0) == 1) {
			ApplicableTable = InterestTable;
			ApplicableUsedCols = &m_IWUsedCols;
		}
		else {
			ApplicableTable = PrincipalTable;
			ApplicableUsedCols = &m_PWUsedCols;
		}
		(*ApplicableUsedCols) << ciStep << ciSeniorityGroup << ciSeniorityGroupLevel << ciCouponIndex;
		ApplicableTable->setRowCount(ApplicableTable->rowCount() + 1);
		ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciStep, new QTableWidgetItem("IC Test"));
		ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciSeniorityGroup, new QTableWidgetItem(a.GetParameter(WatFalPrior::wstParameters::SeniorityGroup).value<IntegerVector>().GetVector()));
		ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciSeniorityGroupLevel, new QTableWidgetItem(a.GetParameter(WatFalPrior::wstParameters::SeniorityGroupLevel).value<IntegerVector>().GetVector()));
		ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciCouponIndex, new QTableWidgetItem(a.GetParameter(WatFalPrior::wstParameters::CouponIndex).value<IntegerVector>().GetVector()));
		if (a.HasParameter(WatFalPrior::wstParameters::TestTargetOverride)) {
			(*ApplicableUsedCols) << ciTestTargetOverride;
			ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciTestTargetOverride, new QTableWidgetItem(a.GetParameter(WatFalPrior::wstParameters::TestTargetOverride).value<BloombergVector>().GetVector()));
		}		
	break;
	case WatFalPrior::WaterfallStepType::wst_DeferredInterest:
		if (a.GetParameter(WatFalPrior::wstParameters::SourceOfFunding).value<IntegerVector>().GetValue(0) == 1) {
			ApplicableTable = InterestTable;
			ApplicableUsedCols = &m_IWUsedCols;
		}
		else {
			ApplicableTable = PrincipalTable;
			ApplicableUsedCols = &m_PWUsedCols;
		}
		(*ApplicableUsedCols) << ciStep << ciSeniorityGroup << ciSeniorityGroupLevel << ciCouponIndex;
		ApplicableTable->setRowCount(ApplicableTable->rowCount() + 1);
		ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciStep, new QTableWidgetItem("Deferred Interest"));
		ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciSeniorityGroup, new QTableWidgetItem(a.GetParameter(WatFalPrior::wstParameters::SeniorityGroup).value<IntegerVector>().GetVector()));
		ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciSeniorityGroupLevel, new QTableWidgetItem(a.GetParameter(WatFalPrior::wstParameters::SeniorityGroupLevel).value<IntegerVector>().GetVector()));
		ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciCouponIndex, new QTableWidgetItem(a.GetParameter(WatFalPrior::wstParameters::CouponIndex).value<IntegerVector>().GetVector()));
	break;
	case WatFalPrior::WaterfallStepType::wst_juniorFees:
		if (a.GetParameter(WatFalPrior::wstParameters::SourceOfFunding).value<IntegerVector>().GetValue(0) == 1) {
			ApplicableTable = InterestTable;
			ApplicableUsedCols = &m_IWUsedCols;
		}
		else {
			ApplicableTable = PrincipalTable;
			ApplicableUsedCols = &m_PWUsedCols;
		}
		ApplicableUsedCols->insert(ciStep);
		ApplicableTable->setRowCount(ApplicableTable->rowCount() + 1);
		ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciStep, new QTableWidgetItem("Junior Fees"));
		if (a.HasParameter(WatFalPrior::wstParameters::IRRtoEquityTarget)) {
			ApplicableUsedCols->insert(ciIRRtoEquityTarget);
			ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciIRRtoEquityTarget, new QTableWidgetItem(a.GetParameter(WatFalPrior::wstParameters::IRRtoEquityTarget).value<BloombergVector>().GetVector()));
		}
	break;
	case WatFalPrior::WaterfallStepType::wst_FeesFromExcess:
		for (int FakeIter = 0; FakeIter < 2; ++FakeIter) {
			ApplicableTable = nullptr;
			if (FakeIter==0 && (a.GetParameter(WatFalPrior::wstParameters::SourceOfFunding).value<IntegerVector>().GetValue(0) & 1)) {
				ApplicableTable = InterestTable;
				ApplicableUsedCols = &m_IWUsedCols;
			}
			if (FakeIter > 0 && (a.GetParameter(WatFalPrior::wstParameters::SourceOfFunding).value<IntegerVector>().GetValue(0) >=2)) {
				ApplicableTable = PrincipalTable;
				ApplicableUsedCols = &m_PWUsedCols;
			}
			if (!ApplicableTable) continue;
			(*ApplicableUsedCols) << ciStep << ciRedemptionShare;
			ApplicableTable->setRowCount(ApplicableTable->rowCount() + 1);
			ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciStep, new QTableWidgetItem("Fees from Excess Spread"));
			ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciRedemptionShare, new QTableWidgetItem(a.GetParameter(WatFalPrior::wstParameters::RedemptionShare).value<BloombergVector>().GetVector()));

			if (a.HasParameter(WatFalPrior::wstParameters::IRRtoEquityTarget)) {
				ApplicableUsedCols->insert(ciIRRtoEquityTarget);
				ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciIRRtoEquityTarget, new QTableWidgetItem(a.GetParameter(WatFalPrior::wstParameters::IRRtoEquityTarget).value<BloombergVector>().GetVector()));
			}
		}
	break;
	case WatFalPrior::WaterfallStepType::wst_ReserveReplenish:
		if (a.GetParameter(WatFalPrior::wstParameters::SourceOfFunding).value<IntegerVector>().GetValue(0) == 1) {
			ApplicableTable = InterestTable;
			ApplicableUsedCols = &m_IWUsedCols;
		}
		else {
			ApplicableTable = PrincipalTable;
			ApplicableUsedCols = &m_PWUsedCols;
		}
		(*ApplicableUsedCols) << ciStep << ciReserveIndex;
		ApplicableTable->setRowCount(ApplicableTable->rowCount() + 1);
		ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciStep, new QTableWidgetItem("Fees from Excess Spread"));
		ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciReserveIndex, new QTableWidgetItem(a.GetParameter(WatFalPrior::wstParameters::ReserveIndex).value<IntegerVector>().GetVector()));
	break;
	case WatFalPrior::WaterfallStepType::wst_Excess:
		for (int FakeIter = 0; FakeIter < 2; ++FakeIter) {
			ApplicableTable = nullptr;
			if (FakeIter == 0 && (a.GetParameter(WatFalPrior::wstParameters::SourceOfFunding).value<IntegerVector>().GetValue(0) & 1)) {
				ApplicableTable = InterestTable;
				ApplicableUsedCols = &m_IWUsedCols;
			}
			if (FakeIter > 0 && (a.GetParameter(WatFalPrior::wstParameters::SourceOfFunding).value<IntegerVector>().GetValue(0) >= 2)) {
				ApplicableTable = PrincipalTable;
				ApplicableUsedCols = &m_PWUsedCols;
			}
			if (!ApplicableTable) continue;
			(*ApplicableUsedCols) << ciStep;
			ApplicableTable->setRowCount(ApplicableTable->rowCount() + 1);
			ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciStep, new QTableWidgetItem("Excess Spread"));
			if (a.HasParameter(WatFalPrior::wstParameters::RedemptionGroup)) {
				(*ApplicableUsedCols) << ciRedemptionGroup << ciRedemptionGroupLevel << ciRedemptionShare;
				ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciRedemptionGroup, new QTableWidgetItem(a.GetParameter(WatFalPrior::wstParameters::RedemptionGroup).value<IntegerVector>().GetVector()));
				ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciRedemptionGroupLevel, new QTableWidgetItem(a.GetParameter(WatFalPrior::wstParameters::RedemptionGroupLevel).value<IntegerVector>().GetVector()));
				ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciRedemptionShare, new QTableWidgetItem(a.GetParameter(WatFalPrior::wstParameters::RedemptionShare).value<BloombergVector>().GetVector()));
			}
			
			
		}
	break;
	case WatFalPrior::WaterfallStepType::wst_ReinvestPrincipal:{
		ApplicableTable = PrincipalTable;
		ApplicableUsedCols = &m_PWUsedCols;
		QString FundingString = a.GetParameter(WatFalPrior::wstParameters::SourceOfFunding).value<IntegerVector>().GetVector();
		FundingString.replace("1", "Prepayments");
		FundingString.replace("2", "Scheduled Principal");
		FundingString.replace("3", "All Principal");
		(*ApplicableUsedCols) << ciStep << ciSourceofFunding << ciAdditionalCollateralShare;
		ApplicableTable->setRowCount(ApplicableTable->rowCount() + 1);
		ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciStep, new QTableWidgetItem("Reinvest"));
		ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciAdditionalCollateralShare, new QTableWidgetItem(a.GetParameter(WatFalPrior::wstParameters::AdditionalCollateralShare).value<BloombergVector>().GetVector()));
		ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciSourceofFunding, new QTableWidgetItem(FundingString));
	}
	break;
	case WatFalPrior::WaterfallStepType::wst_Turbo:
		ApplicableTable = InterestTable;
		ApplicableUsedCols = &m_IWUsedCols;
		(*ApplicableUsedCols) << ciStep << ciRedemptionGroup << ciRedemptionGroupLevel << ciRedemptionShare;
		ApplicableTable->setRowCount(ApplicableTable->rowCount() + 1);
		ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciStep, new QTableWidgetItem("Turbo"));
		ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciRedemptionGroup, new QTableWidgetItem(a.GetParameter(WatFalPrior::wstParameters::RedemptionGroup).value<IntegerVector>().GetVector()));
		ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciRedemptionGroupLevel, new QTableWidgetItem(a.GetParameter(WatFalPrior::wstParameters::RedemptionGroupLevel).value<IntegerVector>().GetVector()));
		ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciRedemptionShare, new QTableWidgetItem(a.GetParameter(WatFalPrior::wstParameters::RedemptionShare).value<BloombergVector>().GetVector()));
	break;
	case WatFalPrior::WaterfallStepType::wst_PDL:
		ApplicableTable = InterestTable;
		ApplicableUsedCols = &m_IWUsedCols;
		(*ApplicableUsedCols) << ciStep << ciSeniorityGroup << ciSeniorityGroupLevel;
		ApplicableTable->setRowCount(ApplicableTable->rowCount() + 1);
		ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciStep, new QTableWidgetItem("Cure PDL"));
		ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciSeniorityGroup, new QTableWidgetItem(a.GetParameter(WatFalPrior::wstParameters::SeniorityGroup).value<IntegerVector>().GetVector()));
		ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciSeniorityGroupLevel, new QTableWidgetItem(a.GetParameter(WatFalPrior::wstParameters::SeniorityGroupLevel).value<IntegerVector>().GetVector()));
		if (a.HasParameter(WatFalPrior::wstParameters::RedemptionGroup)) {
			(*ApplicableUsedCols) << ciRedemptionGroup << ciRedemptionGroupLevel << ciRedemptionShare;
			ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciRedemptionGroup, new QTableWidgetItem(a.GetParameter(WatFalPrior::wstParameters::RedemptionGroup).value<IntegerVector>().GetVector()));
			ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciRedemptionGroupLevel, new QTableWidgetItem(a.GetParameter(WatFalPrior::wstParameters::RedemptionGroupLevel).value<IntegerVector>().GetVector()));
			ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciRedemptionShare, new QTableWidgetItem(a.GetParameter(WatFalPrior::wstParameters::RedemptionShare).value<BloombergVector>().GetVector()));
		}
	break;
	case WatFalPrior::WaterfallStepType::wst_AllocPrepayFees:
		ApplicableTable = InterestTable;
		ApplicableUsedCols = &m_IWUsedCols;
		(*ApplicableUsedCols) << ciStep << ciRedemptionGroup << ciRedemptionGroupLevel << ciRedemptionShare << ciSourceofFunding;
		ApplicableTable->setRowCount(ApplicableTable->rowCount() + 1);
		ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciStep, new QTableWidgetItem("Allocate Prepayment Fees"));
		ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciRedemptionGroup, new QTableWidgetItem(a.GetParameter(WatFalPrior::wstParameters::RedemptionGroup).value<IntegerVector>().GetVector()));
		ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciRedemptionGroupLevel, new QTableWidgetItem(a.GetParameter(WatFalPrior::wstParameters::RedemptionGroupLevel).value<IntegerVector>().GetVector()));
		ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciRedemptionShare, new QTableWidgetItem(a.GetParameter(WatFalPrior::wstParameters::RedemptionShare).value<BloombergVector>().GetVector()));
		ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciSourceofFunding, new QTableWidgetItem((a.GetParameter(WatFalPrior::wstParameters::SourceOfFunding).value<IntegerVector>().GetValue(0) == 1) ? QString("Income") : QString("Redemption")));
	break;
	default:
		QMessageBox::critical(this,"Invalid Step","The step you tried to add is invalid.\nPlease check the Waterfall");
	}
	if (a.HasParameter(WatFalPrior::wstParameters::Trigger)) {
		QString TriggerString = a.GetParameter(WatFalPrior::wstParameters::Trigger).toString();
		TriggerString.replace("!0*0", "FALSE");
		TriggerString.replace("!0*0", "TRUE");
		TriggerString.replace("*", " AND ");
		TriggerString.replace("+", " OR ");
		TriggerString.replace("/", " NAND ");
		TriggerString.replace("-", " NOR ");
		TriggerString.replace("!", "NOT ");
		TriggerString.replace("^", "NOT ");
		ApplicableUsedCols->insert(ciTriggers);
		ApplicableTable->setItem(ApplicableTable->rowCount() - 1, ciTriggers, new QTableWidgetItem(TriggerString));
	}
	for (int i = 0; i < InterestTable->columnCount(); ++i) {
		InterestTable->setColumnHidden(i, !m_IWUsedCols.contains(i));
	}
	for (int i = 0; i < PrincipalTable->columnCount(); ++i) {
		PrincipalTable->setColumnHidden(i, !m_PWUsedCols.contains(i));
	}

}
void WaterfallViewer::SetWaterfall(const Waterfall& a){
	for(int i=0;i<a.GetStepsCount();i++){
		AddStep(*(a.GetStep(i)));
	}
}
/*void WaterfallViewer::resizeEvent(QResizeEvent *event){
	Q_UNUSED(event);
	for(int i=0;i<PrincipalTable->columnCount();i++){
		PrincipalTable->setColumnWidth(i,(PrincipalTable->width()-PrincipalTable->verticalScrollBar()->width())/PrincipalTable->columnCount());
	}
	for(int i=0;i<InterestTable->columnCount();i++){
		InterestTable->setColumnWidth(i,(InterestTable->width()-InterestTable->verticalScrollBar()->width())/InterestTable->columnCount());
	}
}*/