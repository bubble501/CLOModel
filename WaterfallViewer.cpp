#include "WaterfallViewer.h"
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
	InterestTable->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	InterestTable->setSelectionBehavior(QAbstractItemView::SelectRows);
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
	PrincipalTable->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	PrincipalTable->setSelectionBehavior(QAbstractItemView::SelectRows);
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
void WaterfallViewer::AddStep(WatFalPrior::WaterfallStepType Step,int GroupTarget,int RedemptionGroup,double RedemptionShare){
	WatFalPrior TempStep;
	TempStep.SetPriorityType(Step);
	TempStep.SetGroupTarget(GroupTarget);
	TempStep.SetRedemptionGroup(RedemptionGroup);
	TempStep.SetRedemptionShare(RedemptionShare);
	AddStep(TempStep);
}
void WaterfallViewer::AddStep(const WatFalPrior& a){
	switch(a.GetPriorityType()){
	case WatFalPrior::wst_SeniorExpenses:
		if(a.GetRedemptionGroup()==1){
			InterestTable->setRowCount(InterestTable->rowCount()+1);
			InterestTable->setItem(InterestTable->rowCount()-1,0,new QTableWidgetItem("Senior Expenses"));
		}
		else{
			PrincipalTable->setRowCount(PrincipalTable->rowCount()+1);
			PrincipalTable->setItem(PrincipalTable->rowCount()-1,0,new QTableWidgetItem("Senior Expenses"));
		}
	break;
	case WatFalPrior::wst_SeniorFees:
		if(a.GetRedemptionGroup()==1){
			InterestTable->setRowCount(InterestTable->rowCount()+1);
			InterestTable->setItem(InterestTable->rowCount()-1,0,new QTableWidgetItem("Senior Fees"));
		}
		else{
			PrincipalTable->setRowCount(PrincipalTable->rowCount()+1);
			PrincipalTable->setItem(PrincipalTable->rowCount()-1,0,new QTableWidgetItem("Senior Fees"));
		}
	break;
	case WatFalPrior::wst_Interest:
		InterestTable->setRowCount(InterestTable->rowCount()+1);
		InterestTable->setItem(InterestTable->rowCount()-1,0,new QTableWidgetItem("Interest"));
		InterestTable->setItem(InterestTable->rowCount()-1,1,new QTableWidgetItem(QString("%1").arg(a.GetGroupTarget())));
	break;
	case WatFalPrior::wst_Principal:
		PrincipalTable->setRowCount(PrincipalTable->rowCount()+1);
		PrincipalTable->setItem(PrincipalTable->rowCount()-1,0,new QTableWidgetItem("Principal"));
		PrincipalTable->setItem(PrincipalTable->rowCount()-1,1,new QTableWidgetItem(QString("%1").arg(a.GetGroupTarget())));
	break;
	case WatFalPrior::wst_OCTest:
		InterestTable->setRowCount(InterestTable->rowCount()+1);
		InterestTable->setItem(InterestTable->rowCount()-1,0,new QTableWidgetItem("OC Test"));
		InterestTable->setItem(InterestTable->rowCount()-1,1,new QTableWidgetItem(QString("%1").arg(a.GetGroupTarget())));
		if(a.GetRedemptionGroup()>0){
			InterestTable->setItem(InterestTable->rowCount()-1,2,new QTableWidgetItem(QString("%1").arg(a.GetRedemptionGroup())));
			InterestTable->setItem(InterestTable->rowCount()-1,3,new QTableWidgetItem(QString("%1%").arg(a.GetRedemptionShare()*100)));
		}
	break;
	case WatFalPrior::wst_OCTestPrinc:
		PrincipalTable->setRowCount(PrincipalTable->rowCount()+1);
		PrincipalTable->setItem(PrincipalTable->rowCount()-1,0,new QTableWidgetItem("OC Test"));
		PrincipalTable->setItem(PrincipalTable->rowCount()-1,1,new QTableWidgetItem(QString("%1").arg(a.GetGroupTarget())));
		if(a.GetRedemptionGroup()>0){
			PrincipalTable->setItem(PrincipalTable->rowCount()-1,2,new QTableWidgetItem(QString("%1").arg(a.GetRedemptionGroup())));
			PrincipalTable->setItem(PrincipalTable->rowCount()-1,3,new QTableWidgetItem(QString("%1%").arg(a.GetRedemptionShare()*100)));
		}
	break;
	case WatFalPrior::wst_ICTest:
		InterestTable->setRowCount(InterestTable->rowCount()+1);
		InterestTable->setItem(InterestTable->rowCount()-1,0,new QTableWidgetItem("IC Test"));
		InterestTable->setItem(InterestTable->rowCount()-1,1,new QTableWidgetItem(QString("%1").arg(a.GetGroupTarget())));
	break;
	case WatFalPrior::wst_ICTestPrinc:
		PrincipalTable->setRowCount(PrincipalTable->rowCount()+1);
		PrincipalTable->setItem(PrincipalTable->rowCount()-1,0,new QTableWidgetItem("IC Test"));
		PrincipalTable->setItem(PrincipalTable->rowCount()-1,1,new QTableWidgetItem(QString("%1").arg(a.GetGroupTarget())));
	break;
	case WatFalPrior::wst_DeferredInterest:
		InterestTable->setRowCount(InterestTable->rowCount()+1);
		InterestTable->setItem(InterestTable->rowCount()-1,0,new QTableWidgetItem("Deferred Interest"));
		InterestTable->setItem(InterestTable->rowCount()-1,1,new QTableWidgetItem(QString("%1").arg(a.GetGroupTarget())));
	break;
	case WatFalPrior::wst_DeferredPrinc:
		PrincipalTable->setRowCount(PrincipalTable->rowCount()+1);
		PrincipalTable->setItem(PrincipalTable->rowCount()-1,0,new QTableWidgetItem("Deferred Interest"));
		PrincipalTable->setItem(PrincipalTable->rowCount()-1,1,new QTableWidgetItem(QString("%1").arg(a.GetGroupTarget())));
	break;
	case WatFalPrior::wst_juniorFees:
		if(a.GetRedemptionGroup()==1){
			InterestTable->setRowCount(InterestTable->rowCount()+1);
			InterestTable->setItem(InterestTable->rowCount()-1,0,new QTableWidgetItem("Junior Fees"));
		}
		else{
			PrincipalTable->setRowCount(PrincipalTable->rowCount()+1);
			PrincipalTable->setItem(PrincipalTable->rowCount()-1,0,new QTableWidgetItem("Junior Fees"));
		}
	break;
	case WatFalPrior::wst_ReinvestmentTest:
		InterestTable->setRowCount(InterestTable->rowCount()+1);
		InterestTable->setItem(InterestTable->rowCount()-1,0,new QTableWidgetItem("Reinvestment Test"));
		if(a.GetRedemptionGroup()>0) InterestTable->setItem(InterestTable->rowCount()-1,2,new QTableWidgetItem(QString("%1").arg(a.GetRedemptionGroup())));
	break;
	case WatFalPrior::wst_Excess:
		InterestTable->setRowCount(InterestTable->rowCount()+1);
		InterestTable->setItem(InterestTable->rowCount()-1,0,new QTableWidgetItem("Excess Spread"));
		InterestTable->setItem(InterestTable->rowCount()-1,2,new QTableWidgetItem(QString("%1").arg(a.GetRedemptionGroup())));
		PrincipalTable->setRowCount(PrincipalTable->rowCount()+1);
		PrincipalTable->setItem(PrincipalTable->rowCount()-1,0,new QTableWidgetItem("Excess Spread"));
		PrincipalTable->setItem(PrincipalTable->rowCount()-1,2,new QTableWidgetItem(QString("%1").arg(a.GetRedemptionGroup())));
		break;
	case WatFalPrior::wst_ReinvestPrincipal:
		PrincipalTable->setRowCount(PrincipalTable->rowCount()+1);
		PrincipalTable->setItem(PrincipalTable->rowCount()-1,0,new QTableWidgetItem("Reinvest"));
	break;
	default:
		QMessageBox::critical(this,"Invalid Step","The step you tried to add is invalid.\nPlease check the Waterfall");
	}
}
void WaterfallViewer::resizeEvent(QResizeEvent *event){
	Q_UNUSED(event);
	for(int i=0;i<PrincipalTable->columnCount();i++){
		PrincipalTable->setColumnWidth(i,(PrincipalTable->width()-PrincipalTable->verticalScrollBar()->width())/PrincipalTable->columnCount());
	}
	for(int i=0;i<InterestTable->columnCount();i++){
		InterestTable->setColumnWidth(i,(InterestTable->width()-InterestTable->verticalScrollBar()->width())/InterestTable->columnCount());
	}
}