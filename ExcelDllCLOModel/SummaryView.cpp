#include "SummaryView.h"
#include "Mortgage.h"
#include "TrancheViewer.h"
#include "CommonFunctions.h"
#include "WaterfallViewer.h"
#include "ChartsWidget.h"
#include <QTableWidget>
#include <QHeaderView>
#include <QScrollBar>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QListWidget>
#include <QSizePolicy>
#include <QAbstractItemView>
#include <QLineEdit>
#include <QDoubleValidator>
SummaryView::SummaryView(QWidget* parent)
	:QWidget(parent)
	,ExpensesTable(NULL)
{
	setWindowIcon(QIcon(":/Icons/Logo.png"));
	if(!QMetaType::isRegistered(qMetaTypeId<Waterfall>()))
		qRegisterMetaType<Waterfall>("Waterfall");
	setWindowTitle("Stress Scenario");
	setMinimumSize(1024,768);
	QStringList HeadersStrings;
	MainWidget=new QTabWidget(this);

	StructureTable=new QTableWidget(this);
	HeadersStrings 
		<< "Bond"
		<< "Pro rata Group"
		<< "Currency"
		<< "Original Amount"
		<< "Current Amount"
		<< "Bond Factor"
		<< "Implied CE"
		<< "Fixed/Float"
		<< "Reference Rate"
		<< "Coupon"
		<< "Price"
		<< "IRR"
		<< "WAL"
		<< "DM"
		<< "Loss"
	;
	StructureTable->setRowCount(0);
	StructureTable->setColumnCount(HeadersStrings.size());
	StructureTable->setHorizontalHeaderLabels(HeadersStrings);
	StructureTable->verticalHeader()->setVisible(false);
	StructureTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	StructureTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	StructureTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	MainWidget->addTab(StructureTable,"Structure");

	WatStructView=new WaterfallViewer(this);
	MainWidget->addTab(WatStructView,"Waterfall Structure");

	MtgTable=new QTableWidget(this);
	MtgTable->setColumnCount(8);
	MtgTable->setRowCount(0);
	HeadersStrings.clear();
	HeadersStrings 
			<< "Date"
			<< "Outstanding"
			<< "Interest"
			<< "Principal"
			<< "Prepayments"
			<< "Accrued Interest"
			<< "Loss on Principal"
			<< "Loss on Interests";
	MtgTable->setHorizontalHeaderLabels(HeadersStrings);
	MtgTable->verticalHeader()->setVisible(false);
	MtgTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	MtgTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	MtgTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	MainWidget->addTab(MtgTable,"Mortgages Results");

	QWidget* TranchesWidget=new QWidget(this);
	QHBoxLayout* TranchesLay=new QHBoxLayout(TranchesWidget);
	TranchesArea=new QStackedWidget(TranchesWidget);
	TranchesList=new QListWidget(TranchesWidget);
	TranchesList->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Expanding);
	connect(TranchesList,SIGNAL(currentRowChanged(int)),TranchesArea,SLOT(setCurrentIndex(int)));
	TranchesLay->addWidget(TranchesList);
	TranchesLay->addWidget(TranchesArea);
	MainWidget->addTab(TranchesWidget,"Tranches Results");

	QWidget* CallTranchesWidget=new QWidget(this);
	QHBoxLayout* CallTranchesLay=new QHBoxLayout(CallTranchesWidget);
	CallTranchesArea=new QStackedWidget(CallTranchesWidget);
	CallTranchesList=new QListWidget(CallTranchesWidget);
	CallTranchesList->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Expanding);
	connect(CallTranchesList,SIGNAL(currentRowChanged(int)),CallTranchesArea,SLOT(setCurrentIndex(int)));
	CallTranchesLay->addWidget(CallTranchesList);
	CallTranchesLay->addWidget(CallTranchesArea);
	MainWidget->addTab(CallTranchesWidget,"Call Tranches Results");
	
	ExpensesTable=new QTableWidget(this);
	ExpensesTable->setColumnCount(6);
	ExpensesTable->setRowCount(0);
	HeadersStrings.clear();
	HeadersStrings
		<< "Date"
		<< "Senior Expenses"
		<< "Senior Fees"
		<< "Junior Fees"
		<< "Annualized Excess Spread"
		<< "WA Cost of Funding";
	ExpensesTable->setHorizontalHeaderLabels(HeadersStrings);
	ExpensesTable->verticalHeader()->setVisible(false);
	ExpensesTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ExpensesTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	ExpensesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	MainWidget->addTab(ExpensesTable,"Waterfall Infos");

	ReinvestmentsTable=new QTableWidget(this);
	HeadersStrings.clear();
	HeadersStrings
		<< "Date"
		<< "Interest Reinvested"
		<< "Principal Reinvested"
	;
	ReinvestmentsTable->setRowCount(0);
	ReinvestmentsTable->setColumnCount(3);
	ReinvestmentsTable->setHorizontalHeaderLabels(HeadersStrings);
	ReinvestmentsTable->verticalHeader()->setVisible(false);
	ReinvestmentsTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ReinvestmentsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	ReinvestmentsTable->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	MainWidget->addTab(ReinvestmentsTable,"Reinvestments");

	ChartPlotter=new ChartsWidget(this);
	MainWidget->addTab(ChartPlotter,"Charts");

	QHBoxLayout* mainLay=new QHBoxLayout(this);
	mainLay->addWidget(MainWidget);
	//connect(MainWidget,SIGNAL(currentChanged(int)),this,SLOT(AdjustTableSizes()));
}


/*void SummaryView::resizeEvent(QResizeEvent *event){
	Q_UNUSED(event);
	AdjustTableSizes();
}*/
void SummaryView::ResetTranches(){
	for(QList<TrancheViewer*>::iterator i=TrancheTables.begin();i!=TrancheTables.end();i++)
		(*i)->deleteLater();
	TrancheTables.clear();
	TranchesList->clear();
}
void SummaryView::ResetCharts(){
	ChartPlotter->ResetCharts();
}
void SummaryView::DisplayStructure(){
	QLinearGradient OutstandingGradient(0,0,0,100);
	OutstandingGradient.setColorAt(0.0,"#FFEAB1");
	OutstandingGradient.setColorAt(1.0,"#F5CB81");
	setWindowTitle(
		"Stress Scenario - "
		"CPR: "+Structure.GetReinvestmentTest().GetCPRAssumption()
		+" | CDR: "+Structure.GetReinvestmentTest().GetCDRAssumption()
		+" | LS: "+Structure.GetReinvestmentTest().GetLSAssumption()
	);
	ResetTranches();
	for (int i=0;i<Structure.GetTranchesCount();i++){
		TrancheTables.append(new TrancheViewer(this));
		TrancheTables.last()->SetTranche(*(Structure.GetTranche(i)));
		TranchesArea->addWidget(TrancheTables.last());
		TranchesList->addItem(Structure.GetTranche(i)->GetTrancheName());
	}
	for (int i=0;i<CallStructure.GetTranchesCount();i++){
		CallTrancheTables.append(new TrancheViewer(this));
		CallTrancheTables.last()->SetTranche(*(CallStructure.GetTranche(i)));
		CallTranchesArea->addWidget(CallTrancheTables.last());
		CallTranchesList->addItem(CallStructure.GetTranche(i)->GetTrancheName());
	}

	ChartPlotter->ResetCharts();
	ChartPlotter->PlotStructure(Structure);
	
	WatStructView->ResetSteps();
	WatStructView->SetWaterfall(Structure);

	MtgTable->setRowCount(0);
	MtgTable->setRowCount(Structure.GetCalculatedMtgPayments().Count());
	for (int i=0;i<Structure.GetCalculatedMtgPayments().Count();i++){
		MtgTable->setItem(i,0,new QTableWidgetItem(Structure.GetCalculatedMtgPayments().GetDate(i).toString("MMM-yy")));
		MtgTable->setItem(i,1,new QTableWidgetItem(Commarize(Structure.GetCalculatedMtgPayments().GetAmountOut(i))));
		MtgTable->setItem(i,2,new QTableWidgetItem(Commarize(Structure.GetCalculatedMtgPayments().GetInterest(i))));
		MtgTable->setItem(i,3,new QTableWidgetItem(Commarize(Structure.GetCalculatedMtgPayments().GetScheduled(i))));
		MtgTable->setItem(i,4,new QTableWidgetItem(Commarize(Structure.GetCalculatedMtgPayments().GetPrepay(i))));
		MtgTable->setItem(i,5,new QTableWidgetItem(Commarize(Structure.GetCalculatedMtgPayments().GetAccruedInterest(i))));
		MtgTable->setItem(i,6,new QTableWidgetItem(Commarize(Structure.GetCalculatedMtgPayments().GetLoss(i))));
		MtgTable->setItem(i,7,new QTableWidgetItem(Commarize(Structure.GetCalculatedMtgPayments().GetLossOnInterest(i))));
	}
	{
		TrancheCashFlow TempResFlows[2];
		int ColumnCount;
		for (int i=0;i<2;i++) TempResFlows[i]=Structure.GetReserveFundFlow(i);
		ExpensesTable->setRowCount(0);
		ExpensesTable->setRowCount(Structure.GetTotalSeniorExpenses().Count());
		ExpensesTable->setColumnCount(6+(TempResFlows[0].Count()>0 ? 1:0)+(TempResFlows[1].Count()>0 ? 1:0));
		for (int i=0;i<Structure.GetTotalSeniorExpenses().Count();i++){
			ColumnCount=0;
			ExpensesTable->setItem(i,ColumnCount++,new QTableWidgetItem(Structure.GetTotalSeniorExpenses().GetDate(i).toString("MMM-yy")));
			ExpensesTable->setItem(i,ColumnCount++,new QTableWidgetItem(Commarize(Structure.GetTotalSeniorExpenses().GetTotalFlow(i))));
			ExpensesTable->setItem(i,ColumnCount++,new QTableWidgetItem(Commarize(Structure.GetTotalSeniorFees().GetTotalFlow(i))));
			ExpensesTable->setItem(i,ColumnCount++,new QTableWidgetItem(Commarize(Structure.GetTotalJuniorFees().GetTotalFlow(i))));
			ExpensesTable->setItem(i,ColumnCount++,new QTableWidgetItem(Commarize(100.0*Structure.GetAnnualizedExcess(i),2)+'%'));
			ExpensesTable->setItem(i,ColumnCount++,new QTableWidgetItem(Commarize(100.0*Structure.GetWACostOfCapital(i),2)+'%'));
			for (int j=0;j<2;j++){
				if(TempResFlows[j].Count()>0)
					ExpensesTable->setItem(i,ColumnCount++,new QTableWidgetItem(Commarize(TempResFlows[j].GetTotalFlow(i))));
			}
		}
	}
	ReinvestmentsTable->setRowCount(0);
	ReinvestmentsTable->setRowCount(Structure.GetReinvested().Count());
	for (int i=0;i<Structure.GetReinvested().Count();i++){
		ReinvestmentsTable->setItem(i,0,new QTableWidgetItem(Structure.GetReinvested().GetDate(i).toString("MMM-yy")));
		ReinvestmentsTable->setItem(i,1,new QTableWidgetItem(Commarize(Structure.GetReinvested().GetInterest(i))));
		ReinvestmentsTable->setItem(i,2,new QTableWidgetItem(Commarize(Structure.GetReinvested().GetPrincipal(i))));
	}
	StructureTable->setRowCount(0);
	StructureTable->setRowCount(Structure.GetTranchesCount());
	ResetPricesLabel();
	for (int i=0;i<Structure.GetTranchesCount();i++){
		StructureTable->setItem(i,0,new QTableWidgetItem(Structure.GetTranche(i)->GetTrancheName()));
		StructureTable->setItem(i,1,new QTableWidgetItem(QString("%1").arg(Structure.GetTranche(i)->GetProrataGroup())));
		StructureTable->setItem(i,2,new QTableWidgetItem(Structure.GetTranche(i)->GetCurrency()));
		StructureTable->setItem(i,3,new QTableWidgetItem(Commarize(Structure.GetTranche(i)->GetOriginalAmount())));
		StructureTable->setItem(i,4,new QTableWidgetItem(Commarize(Structure.GetTranche(i)->GetOutstandingAmt())));
		StructureTable->setItem(i,5,new QTableWidgetItem(Commarize(Structure.GetTranche(i)->GetBondFactor()*100.0,2U)+'%'));
		StructureTable->setItem(i,6,new QTableWidgetItem(Commarize(Structure.GetCreditEnhancement(i)*100.0,2U)+'%'));
		StructureTable->setItem(i,7,new QTableWidgetItem(Structure.GetTranche(i)->GetInterestType()==Tranche::FloatingInterest ? "Float":"Fixed"));
		StructureTable->setItem(i,8,new QTableWidgetItem(Structure.GetTranche(i)->GetReferenceRate()));
		StructureTable->setItem(i,9,new QTableWidgetItem(Structure.GetTranche(i)->GetCouponVector()));
		StructureTable->setItem(i,11,new QTableWidgetItem(Structure.GetTranche(i)->GetDiscountMargin()<=0 ? QString("0%"):Commarize(((Structure.GetTranche(i)->GetDiscountMargin()/10000.0)+Structure.GetTranche(i)->GetReferenceRateValue())*100.0,2U)+'%'));
		StructureTable->setItem(i,12,new QTableWidgetItem(Commarize(Structure.GetTranche(i)->GetWALife(Structure.GetMortgagesPayments().GetDate(0)),2U)));
		StructureTable->setItem(i,13,new QTableWidgetItem(Commarize(Structure.GetTranche(i)->GetDiscountMargin())));
		StructureTable->setItem(i,14,new QTableWidgetItem(Commarize(Structure.GetTranche(i)->GetLossRate()*100.0,2U)+'%'));

		PricesLabel.append(new QLineEdit(this));
		PricesLabel.last()->setText(Commarize(Structure.GetTranche(i)->GetPrice(),2U));
		PricesLabel.last()->setValidator(new QDoubleValidator(0.0,200.0,2,this));
		PricesLabel.last()->setObjectName(QString("%1").arg(i));
		connect(PricesLabel.last(),SIGNAL(textEdited(QString)),this,SLOT(PriceChanged()));
		StructureTable->setCellWidget(i,10,PricesLabel.last());

		for(int j=0;j<StructureTable->columnCount();j++){
			if(j!=10)
				StructureTable->item(i,j)->setBackgroundColor(
					Structure.GetTranche(i)->GetProrataGroup()%2==0 ? QColor(235,241,222) : QColor(216,228,188)
				);
		}
	}
}
void SummaryView::SetStructure(const Waterfall& a,const Waterfall& ca){Structure=a; CallStructure=ca; DisplayStructure();}
/*void SummaryView::AdjustTableSizes(){
	StructureTable->resizeColumnToContents(0);
	for(int i=1;i<StructureTable->columnCount();i++){
		StructureTable->setColumnWidth(i,(StructureTable->width()-StructureTable->verticalScrollBar()->width()-StructureTable->columnWidth(0))/(StructureTable->columnCount()-1));
	}
	for(int i=0;i<ReinvestmentsTable->columnCount();i++){
		ReinvestmentsTable->setColumnWidth(i,(ReinvestmentsTable->width()-ReinvestmentsTable->verticalScrollBar()->width())/ReinvestmentsTable->columnCount());
	}
	for(int i=0;i<MtgTable->columnCount();i++){
		MtgTable->setColumnWidth(i,(MtgTable->width()-MtgTable->verticalScrollBar()->width())/MtgTable->columnCount());
	}
	for(int i=0;i<ExpensesTable->columnCount();i++){
		ExpensesTable->setColumnWidth(i,(ExpensesTable->width()-ExpensesTable->verticalScrollBar()->width())/ExpensesTable->columnCount());
	}	
}*/
void SummaryView::ResetPricesLabel(){
	for(QList<QLineEdit*>::iterator i=PricesLabel.begin();i!=PricesLabel.end();i++)
		(*i)->deleteLater();
	PricesLabel.clear();
}
void SummaryView::PriceChanged(){
	if(!sender())return;
	int RowToChange=sender()->objectName().toInt();
	Tranche TempTranche=*Structure.GetTranche(RowToChange);
	TempTranche.SetPrice(static_cast<QLineEdit*>(sender())->text().toDouble());
	StructureTable->item(RowToChange,11)->setText(
		TempTranche.GetDiscountMargin()<=0 ? QString("0%"):
		Commarize(((TempTranche.GetDiscountMargin()/10000.0)+TempTranche.GetReferenceRateValue())*100.0,2U)+'%'
	);
	StructureTable->item(RowToChange,13)->setText(Commarize(((TempTranche.GetDiscountMargin()))));
}
void SummaryView::ShowCallStructure(bool a){
	MainWidget->setTabEnabled(4,a);
}