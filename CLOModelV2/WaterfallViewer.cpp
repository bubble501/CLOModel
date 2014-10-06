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
		<< "Group Target"
		<< "Redemption Target"
		<< "Redemption Share"
	;

	InterestTable=new QTableWidget(this);
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
	/*switch(a.GetPriorityType()){
	case WatFalPrior::WaterfallStepType::wst_SeniorExpenses:
		if(a.GetRedemptionGroup()==1){
			InterestTable->setRowCount(InterestTable->rowCount()+1);
			InterestTable->setItem(InterestTable->rowCount()-1,0,new QTableWidgetItem("Senior Expenses"));
		}
		else{
			PrincipalTable->setRowCount(PrincipalTable->rowCount()+1);
			PrincipalTable->setItem(PrincipalTable->rowCount()-1,0,new QTableWidgetItem("Senior Expenses"));
		}
	break;
	case WatFalPrior::WaterfallStepType::wst_SeniorFees:
		if(a.GetRedemptionGroup()==1){
			InterestTable->setRowCount(InterestTable->rowCount()+1);
			InterestTable->setItem(InterestTable->rowCount()-1,0,new QTableWidgetItem("Senior Fees"));
		}
		else{
			PrincipalTable->setRowCount(PrincipalTable->rowCount()+1);
			PrincipalTable->setItem(PrincipalTable->rowCount()-1,0,new QTableWidgetItem("Senior Fees"));
		}
	break;
	case WatFalPrior::WaterfallStepType::wst_Interest:
		InterestTable->setRowCount(InterestTable->rowCount()+1);
		InterestTable->setItem(InterestTable->rowCount()-1,0,new QTableWidgetItem("Interest"));
		InterestTable->setItem(InterestTable->rowCount()-1,1,new QTableWidgetItem(QString("%1").arg(a.GetGroupTarget())));
	break;
	case WatFalPrior::WaterfallStepType::wst_Principal:
		PrincipalTable->setRowCount(PrincipalTable->rowCount()+1);
		PrincipalTable->setItem(PrincipalTable->rowCount()-1,0,new QTableWidgetItem("Principal"));
		PrincipalTable->setItem(PrincipalTable->rowCount()-1,1,new QTableWidgetItem(QString("%1").arg(a.GetGroupTarget())));
	break;
	case WatFalPrior::WaterfallStepType::wst_OCTest:
		InterestTable->setRowCount(InterestTable->rowCount()+1);
		InterestTable->setItem(InterestTable->rowCount()-1,0,new QTableWidgetItem("OC Test"));
		InterestTable->setItem(InterestTable->rowCount()-1,1,new QTableWidgetItem(QString("%1").arg(a.GetGroupTarget())));
		if(a.GetRedemptionGroup()>0){
			InterestTable->setItem(InterestTable->rowCount()-1,2,new QTableWidgetItem(QString("%1").arg(a.GetRedemptionGroup())));
			InterestTable->setItem(InterestTable->rowCount()-1,3,new QTableWidgetItem(QString("%1%").arg(a.GetRedemptionShare()*100)));
		}
		else {
			PrincipalTable->setRowCount(PrincipalTable->rowCount() + 1);
			PrincipalTable->setItem(PrincipalTable->rowCount() - 1, 0, new QTableWidgetItem("OC Test"));
			PrincipalTable->setItem(PrincipalTable->rowCount() - 1, 1, new QTableWidgetItem(QString("%1").arg(a.GetGroupTarget())));
			if (a.GetRedemptionGroup() > 0) {
				PrincipalTable->setItem(PrincipalTable->rowCount() - 1, 2, new QTableWidgetItem(QString("%1").arg(a.GetRedemptionGroup())));
				PrincipalTable->setItem(PrincipalTable->rowCount() - 1, 3, new QTableWidgetItem(QString("%1%").arg(a.GetRedemptionShare() * 100)));
			}
		}
		
		break;
	case WatFalPrior::WaterfallStepType::wst_ICTest:
		InterestTable->setRowCount(InterestTable->rowCount()+1);
		InterestTable->setItem(InterestTable->rowCount()-1,0,new QTableWidgetItem("IC Test"));
		InterestTable->setItem(InterestTable->rowCount()-1,1,new QTableWidgetItem(QString("%1").arg(a.GetGroupTarget())));
	break;
	case WatFalPrior::WaterfallStepType::wst_ICTestPrinc:
		PrincipalTable->setRowCount(PrincipalTable->rowCount()+1);
		PrincipalTable->setItem(PrincipalTable->rowCount()-1,0,new QTableWidgetItem("IC Test"));
		PrincipalTable->setItem(PrincipalTable->rowCount()-1,1,new QTableWidgetItem(QString("%1").arg(a.GetGroupTarget())));
	break;
	case WatFalPrior::WaterfallStepType::wst_DeferredInterest:
		InterestTable->setRowCount(InterestTable->rowCount()+1);
		InterestTable->setItem(InterestTable->rowCount()-1,0,new QTableWidgetItem("Deferred Interest"));
		InterestTable->setItem(InterestTable->rowCount()-1,1,new QTableWidgetItem(QString("%1").arg(a.GetGroupTarget())));
	break;
	case WatFalPrior::WaterfallStepType::wst_DeferredPrinc:
		PrincipalTable->setRowCount(PrincipalTable->rowCount()+1);
		PrincipalTable->setItem(PrincipalTable->rowCount()-1,0,new QTableWidgetItem("Deferred Interest"));
		PrincipalTable->setItem(PrincipalTable->rowCount()-1,1,new QTableWidgetItem(QString("%1").arg(a.GetGroupTarget())));
	break;
	case WatFalPrior::WaterfallStepType::wst_juniorFees:
		if(a.GetRedemptionGroup()==1){
			InterestTable->setRowCount(InterestTable->rowCount()+1);
			InterestTable->setItem(InterestTable->rowCount()-1,0,new QTableWidgetItem("Junior Fees"));
		}
		else{
			PrincipalTable->setRowCount(PrincipalTable->rowCount()+1);
			PrincipalTable->setItem(PrincipalTable->rowCount()-1,0,new QTableWidgetItem("Junior Fees"));
		}
	break;
	case WatFalPrior::WaterfallStepType::wst_ReserveReplenish:
		if(a.GetRedemptionGroup()==1){
			InterestTable->setRowCount(InterestTable->rowCount()+1);
			InterestTable->setItem(InterestTable->rowCount()-1,0,new QTableWidgetItem("Reserve Fund Replenishment"));
			InterestTable->setItem(InterestTable->rowCount()-1,1,new QTableWidgetItem(QString("Reserve %1").arg(a.GetGroupTarget())));
		}
		else{
			PrincipalTable->setRowCount(PrincipalTable->rowCount()+1);
			PrincipalTable->setItem(PrincipalTable->rowCount()-1,0,new QTableWidgetItem("Reserve Fund Replenishment"));
			PrincipalTable->setItem(PrincipalTable->rowCount()-1,1,new QTableWidgetItem(QString("Reserve %1").arg(a.GetGroupTarget())));
		}
	break;
	case WatFalPrior::WaterfallStepType::wst_ReinvestmentTest:
		InterestTable->setRowCount(InterestTable->rowCount()+1);
		InterestTable->setItem(InterestTable->rowCount()-1,0,new QTableWidgetItem("Reinvestment Test"));
		InterestTable->setItem(InterestTable->rowCount()-1,1,new QTableWidgetItem(QString("%1").arg(a.GetGroupTarget())));
		if(a.GetRedemptionGroup()>0) InterestTable->setItem(InterestTable->rowCount()-1,2,new QTableWidgetItem(QString("%1").arg(a.GetRedemptionGroup())));
	break;
	case WatFalPrior::WaterfallStepType::wst_Excess:
		InterestTable->setRowCount(InterestTable->rowCount()+1);
		InterestTable->setItem(InterestTable->rowCount()-1,0,new QTableWidgetItem("Excess Spread"));
		InterestTable->setItem(InterestTable->rowCount()-1,2,new QTableWidgetItem(QString("%1").arg(a.GetRedemptionGroup())));
		PrincipalTable->setRowCount(PrincipalTable->rowCount()+1);
		PrincipalTable->setItem(PrincipalTable->rowCount()-1,0,new QTableWidgetItem("Excess Spread"));
		PrincipalTable->setItem(PrincipalTable->rowCount()-1,2,new QTableWidgetItem(QString("%1").arg(a.GetRedemptionGroup())));
		break;
	case WatFalPrior::WaterfallStepType::wst_ReinvestPrincipal:
		PrincipalTable->setRowCount(PrincipalTable->rowCount()+1);
		PrincipalTable->setItem(PrincipalTable->rowCount()-1,0,new QTableWidgetItem("Reinvest"));
		if (a.GetRedemptionGroup() == 1) PrincipalTable->setItem(PrincipalTable->rowCount() - 1, 2, new QTableWidgetItem("Unscheduled"));
		else if (a.GetRedemptionGroup() == 2) PrincipalTable->setItem(PrincipalTable->rowCount() - 1, 2, new QTableWidgetItem("Scheduled"));
		else PrincipalTable->setItem(PrincipalTable->rowCount() - 1, 2, new QTableWidgetItem("All Principal"));
	break;
	case WatFalPrior::WaterfallStepType::wst_RedeemProRata:
		PrincipalTable->setRowCount(PrincipalTable->rowCount() + 1);
		PrincipalTable->setItem(PrincipalTable->rowCount() - 1, 0, new QTableWidgetItem("Redeem Pro rata"));
		PrincipalTable->setItem(PrincipalTable->rowCount() - 1, 1, new QTableWidgetItem(QString("From Seniority %1").arg(qMin(a.GetRedemptionGroup(), a.GetGroupTarget()))));
		PrincipalTable->setItem(PrincipalTable->rowCount() - 1, 2, new QTableWidgetItem(QString("To Seniority %1").arg(qMax(a.GetRedemptionGroup(), a.GetGroupTarget()))));
	break;
	case WatFalPrior::WaterfallStepType::wst_Turbo:
		InterestTable->setRowCount(InterestTable->rowCount() + 1);
		InterestTable->setItem(InterestTable->rowCount() - 1, 0, new QTableWidgetItem("Turbo Redemption"));
		InterestTable->setItem(InterestTable->rowCount() - 1, 2, new QTableWidgetItem(QString("%1").arg(a.GetRedemptionGroup())));
		InterestTable->setItem(InterestTable->rowCount() - 1, 3, new QTableWidgetItem(QString("%1%").arg(a.GetRedemptionShare() * 100)));
	break;
	case WatFalPrior::WaterfallStepType::wst_PDL:
		InterestTable->setRowCount(InterestTable->rowCount() + 1);
		InterestTable->setItem(InterestTable->rowCount() - 1, 0, new QTableWidgetItem("Cure PDL"));
		InterestTable->setItem(InterestTable->rowCount() - 1, 1, new QTableWidgetItem(QString("%1").arg(a.GetGroupTarget())));
		if (a.GetRedemptionGroup() > 0) {
			InterestTable->setItem(InterestTable->rowCount() - 1, 2, new QTableWidgetItem(QString("%1").arg(a.GetRedemptionGroup())));
			InterestTable->setItem(InterestTable->rowCount() - 1, 3, new QTableWidgetItem(QString("%1%").arg(a.GetRedemptionShare() * 100)));
		}
	break;
	case WatFalPrior::WaterfallStepType::wst_FeesFromExcess:
		if (a.GetRedemptionGroup() == 1 || a.GetRedemptionGroup()==3) {
			InterestTable->setRowCount(InterestTable->rowCount() + 1);
			InterestTable->setItem(InterestTable->rowCount() - 1, 0, new QTableWidgetItem("Junior Fees from Excess Spread"));
			InterestTable->setItem(InterestTable->rowCount() - 1, 2, new QTableWidgetItem(QString("%1").arg(
				(a.GetRedemptionGroup() == 1) ? "Excess Interest" : "All Excess"
				)));
			InterestTable->setItem(InterestTable->rowCount() - 1, 3, new QTableWidgetItem(QString("%1%").arg(a.GetRedemptionShare() * 100)));
		}
		else if (a.GetRedemptionGroup() == 2 || a.GetRedemptionGroup() == 3) {
			PrincipalTable->setRowCount(PrincipalTable->rowCount() + 1);
			PrincipalTable->setItem(PrincipalTable->rowCount() - 1, 0, new QTableWidgetItem("Junior Fees from Excess Spread"));
			PrincipalTable->setItem(PrincipalTable->rowCount() - 1, 2, new QTableWidgetItem(QString("%1").arg(
				(a.GetRedemptionGroup() == 2) ? "Excess Principal" : "All Excess"
				)));
			PrincipalTable->setItem(PrincipalTable->rowCount() - 1, 3, new QTableWidgetItem(QString("%1%").arg(a.GetRedemptionShare() * 100)));
		}
	break;
	default:
		QMessageBox::critical(this,"Invalid Step","The step you tried to add is invalid.\nPlease check the Waterfall");
	}*/
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