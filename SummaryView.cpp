#include "SummaryView.h"
#include "Mortgage.h"
#include "TrancheViewer.h"
#include "CommonFunctions.h"
#include <QTableWidget>
#include <QHeaderView>
#include <QScrollBar>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QListWidget>
#include <QSizePolicy>
#include <QAbstractItemView>
SummaryView::SummaryView(QWidget* parent)
	:QWidget(parent)
	,ExpensesTable(NULL)
{
	setWindowIcon(QIcon(":/Icons/Logo.png"));
	qRegisterMetaType<Waterfall>("Waterfall");
	MtgTable=new QTableWidget(this);
	MtgTable->setColumnCount(8);
	MtgTable->setRowCount(0);
	QStringList HeadersStrings;
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
	MtgTable->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	MtgTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	MainWidget=new QTabWidget(this);
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
	ExpensesTable->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	ExpensesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	MainWidget->addTab(ExpensesTable,"Waterfall Infos");

	QHBoxLayout* mainLay=new QHBoxLayout(this);
	mainLay->addWidget(MainWidget);
	connect(MainWidget,SIGNAL(currentChanged(int)),this,SLOT(AdjustTableSizes()));
}


void SummaryView::resizeEvent(QResizeEvent *event){
	Q_UNUSED(event);
	AdjustTableSizes();
}
void SummaryView::DisplayStructure(){
	for (int i=0;i<Structure.GetTranchesCount();i++) TrancheTables[i]->SetTranche(*(Structure.GetTranche(i)));
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
	ExpensesTable->setRowCount(0);
	ExpensesTable->setRowCount(Structure.GetTotalSeniorExpenses().Count());
	for (int i=0;i<Structure.GetTotalSeniorExpenses().Count();i++){
		ExpensesTable->setItem(i,0,new QTableWidgetItem(Structure.GetTotalSeniorExpenses().GetDate(i).toString("MMM-yy")));
		ExpensesTable->setItem(i,1,new QTableWidgetItem(Commarize(Structure.GetTotalSeniorExpenses().GetTotalFlow(i))));
		ExpensesTable->setItem(i,2,new QTableWidgetItem(Commarize(Structure.GetTotalSeniorFees().GetTotalFlow(i))));
		ExpensesTable->setItem(i,3,new QTableWidgetItem(Commarize(Structure.GetTotalJuniorFees().GetTotalFlow(i))));
		ExpensesTable->setItem(i,4,new QTableWidgetItem(Commarize(100.0*Structure.GetAnnualizedExcess(i),2)+'%'));
		ExpensesTable->setItem(i,5,new QTableWidgetItem(Commarize(100.0*Structure.GetWACostOfCapital(i),2)+'%'));
	}
	AdjustTableSizes();
}
void SummaryView::SetStructure(const Waterfall& a){Structure=a; DisplayStructure();}
void SummaryView::AdjustTableSizes(){
	for(int i=0;i<MtgTable->columnCount();i++){
		MtgTable->setColumnWidth(i,(MtgTable->width()-MtgTable->verticalScrollBar()->width())/MtgTable->columnCount());
	}
	for(int i=0;i<ExpensesTable->columnCount();i++){
		ExpensesTable->setColumnWidth(i,(ExpensesTable->width()-ExpensesTable->verticalScrollBar()->width())/ExpensesTable->columnCount());
	}	
}