#include "SummaryView.h"
#include "Private/SummaryView_p.h"
#include "Mortgage.h"
#include "Private/TrancheViewer.h"
#include "CommonFunctions.h"
#include "Private/WaterfallViewer.h"
#include "Private/ChartsWidget.h"
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
#include "ReinvestmentTest.h"
#include "Tranche.h"
#include "Private/InternalItems.h"
#include "ReserveFund.h"
#include "Seniority.h"

SummaryViewPrivate::SummaryViewPrivate(SummaryView *q)
    :q_ptr(q)
    , ExpensesTable(nullptr)
{}
SummaryView::SummaryView( QWidget* parent)
    : SummaryView(new SummaryViewPrivate(this),parent)
{}
SummaryView::SummaryView(SummaryViewPrivate* d, QWidget* parent)
	:QWidget(parent)
    , d_ptr(d)
{
	setWindowIcon(QIcon(":/Icons/Logo.png"));
	if(!QMetaType::isRegistered(qMetaTypeId<Waterfall>()))
		qRegisterMetaType<Waterfall>("Waterfall");
	setWindowTitle("Stress Scenario");
	setMinimumSize(1024,768);
	QStringList HeadersStrings;
	d->MainWidget=new QTabWidget(this);

    d->StructureTable = new QTableWidget(this);
	HeadersStrings 
		<< "Bond"
		<< "Pro rata Group"
		<< "Currency"
		<< "Original Amount"
		<< "Current Amount"
		<< "Bond Factor"
		<< "Implied CE"
		<< "Reference Rate"
		<< "Coupon"
		<< "Price"
		<< "IRR"
		<< "WAL"
		<< "DM"
		<< "Loss"
	;
    d->StructureTable->setRowCount(0);
    d->StructureTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    d->StructureTable->setColumnCount(HeadersStrings.size());
    d->StructureTable->setHorizontalHeaderLabels(HeadersStrings);
    d->StructureTable->verticalHeader()->setVisible(false);
    d->StructureTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d->StructureTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    d->StructureTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    d->StructureTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    d->MainWidget->addTab(d->StructureTable, "Structure");

    d->WatStructView = new WaterfallViewer(this);
    d->MainWidget->addTab(d->WatStructView, "Waterfall Structure");

    d->MtgTable = new QTableWidget(this);
    d->MtgTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    d->MtgTable->setColumnCount(8);
    d->MtgTable->setRowCount(0);
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
    d->MtgTable->setHorizontalHeaderLabels(HeadersStrings);
    d->MtgTable->verticalHeader()->setVisible(false);
    d->MtgTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d->MtgTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    d->MtgTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    d->MainWidget->addTab(d->MtgTable, "Mortgages Results");

	QWidget* TranchesWidget=new QWidget(this);
	QHBoxLayout* TranchesLay=new QHBoxLayout(TranchesWidget);
    d->TranchesArea = new QStackedWidget(TranchesWidget);
    d->TranchesList = new QListWidget(TranchesWidget);
    d->TranchesList->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    connect(d->TranchesList, SIGNAL(currentRowChanged(int)), d->TranchesArea, SLOT(setCurrentIndex(int)));
    TranchesLay->addWidget(d->TranchesList);
    TranchesLay->addWidget(d->TranchesArea);
    d->MainWidget->addTab(TranchesWidget, "Tranches Results");

	QWidget* CallTranchesWidget=new QWidget(this);
	QHBoxLayout* CallTranchesLay=new QHBoxLayout(CallTranchesWidget);
    d->CallTranchesArea = new QStackedWidget(CallTranchesWidget);
    d->CallTranchesList = new QListWidget(CallTranchesWidget);
    d->CallTranchesList->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    connect(d->CallTranchesList, SIGNAL(currentRowChanged(int)), d->CallTranchesArea, SLOT(setCurrentIndex(int)));
    CallTranchesLay->addWidget(d->CallTranchesList);
    CallTranchesLay->addWidget(d->CallTranchesArea);
    d->MainWidget->addTab(CallTranchesWidget, "Call Tranches Results");
	
    d->ExpensesTable = new QTableWidget(this);
    d->ExpensesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	//ExpensesTable->setColumnCount(6);
    d->ExpensesTable->setRowCount(0);
	HeadersStrings.clear();
	/*HeadersStrings
		<< "Date"
		<< "Senior Expenses"
		<< "Senior Fees"
		<< "Junior Fees"
		<< "Annualized Excess Spread"
		<< "WA Cost of Funding";
	ExpensesTable->setHorizontalHeaderLabels(HeadersStrings);*/
    d->ExpensesTable->verticalHeader()->setVisible(false);
    d->ExpensesTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d->ExpensesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    d->ExpensesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    d->MainWidget->addTab(d->ExpensesTable, "Waterfall Infos");

    d->ReinvestmentsTable = new QTableWidget(this);
    d->ReinvestmentsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	HeadersStrings.clear();
	HeadersStrings
		<< "Date"
		<< "Funds Reinvested"
	;
    d->ReinvestmentsTable->setRowCount(0);
    d->ReinvestmentsTable->setColumnCount(2);
    d->ReinvestmentsTable->setHorizontalHeaderLabels(HeadersStrings);
    d->ReinvestmentsTable->verticalHeader()->setVisible(false);
    d->ReinvestmentsTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d->ReinvestmentsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    d->ReinvestmentsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    d->MainWidget->addTab(d->ReinvestmentsTable, "Reinvestments");

    d->GICTable = new QTableWidget(this);
    d->GICTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	HeadersStrings.clear();
	HeadersStrings
		<< "Date"
		<< "Interest On Revenues Cash"
		<< "Interest On Principal Cash"
		<< "Interest On Reserve Funds"
		<< "Interest On Re-investable Cash"
		;
    d->GICTable->setRowCount(0);
    d->GICTable->setColumnCount(5);
    d->GICTable->setHorizontalHeaderLabels(HeadersStrings);
    d->GICTable->verticalHeader()->setVisible(false);
    d->GICTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d->GICTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    d->GICTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    d->MainWidget->addTab(d->GICTable, "GIC");

    d->ChartPlotter = new ChartsWidget(this);
    d->MainWidget->addTab(d->ChartPlotter, "Charts");

	QHBoxLayout* mainLay=new QHBoxLayout(this);
    mainLay->addWidget(d->MainWidget);
	//connect(MainWidget,SIGNAL(currentChanged(int)),this,SLOT(AdjustTableSizes()));
}


/*void SummaryView::resizeEvent(QResizeEvent *event){
	Q_UNUSED(event);
	AdjustTableSizes();
}*/
void SummaryViewPrivate::ResetTranches(){
    for (auto i = TrancheTables.begin(); i != TrancheTables.end(); i++)
		(*i)->deleteLater();
    TrancheTables.clear();
   TranchesList->clear();
}
void SummaryViewPrivate::ResetCharts()
{
	ChartPlotter->ResetCharts();
}
void SummaryViewPrivate::DisplayStructure()
{
    Q_Q(SummaryView);
	QLinearGradient OutstandingGradient(0,0,0,100);
	OutstandingGradient.setColorAt(0.0,"#FFEAB1");
	OutstandingGradient.setColorAt(1.0,"#F5CB81");
	q->setWindowTitle(
		"Stress Scenario - "
		"CPR: "+Structure.GetReinvestmentTest().GetCPRAssumption().GetVector()
		+ " | CDR: " + Structure.GetReinvestmentTest().GetCDRAssumption().GetVector()
		+ " | LS: " + Structure.GetReinvestmentTest().GetLSAssumption().GetVector()
	);
	ResetTranches();
	for (int i=0;i<Structure.GetTranchesCount();i++){
		TrancheTables.append(new TrancheViewer(q));
		TrancheTables.last()->SetTranche(*(Structure.GetTranche(i)));
		TranchesArea->addWidget(TrancheTables.last());
		TranchesList->addItem(Structure.GetTranche(i)->GetTrancheName());
	}
	for (int i=0;i<CallStructure.GetTranchesCount();i++){
		CallTrancheTables.append(new TrancheViewer(q));
		CallTrancheTables.last()->SetTranche(*(CallStructure.GetTranche(i)));
		CallTranchesArea->addWidget(CallTrancheTables.last());
		CallTranchesList->addItem(CallStructure.GetTranche(i)->GetTrancheName());
	}

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
		int ColumnCount=6;
		int NumRes = Structure.GetNumReserves();
		QStringList HeadersStrings;
		HeadersStrings
			<< "Date"
			<< "Senior Expenses"
			<< "Senior Fees"
			<< "Junior Fees"
			<< "Annualized Excess Spread"
			<< "WA Cost of Funding";
		ExpensesTable->setRowCount(0);
		ExpensesTable->setRowCount(Structure.GetTotalSeniorExpenses().Count());
		for (int i = 0; i < NumRes; i++) {
			if (Structure.GetReserveFund(i)->GetReserveFundFlow().Count()>0) {
				ColumnCount++;
				HeadersStrings << QString("Reserve %1").arg(i + 1);
			}
		}
		ExpensesTable->setColumnCount(ColumnCount);
		for (int i=0;i<Structure.GetTotalSeniorExpenses().Count();i++){
			ColumnCount=0;
			ExpensesTable->setItem(i,ColumnCount++,new QTableWidgetItem(Structure.GetTotalSeniorExpenses().GetDate(i).toString("MMM-yy")));
			ExpensesTable->setItem(i, ColumnCount++, new QTableWidgetItem(Commarize(Structure.GetTotalSeniorExpenses().GetFlow(i, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow)) + Structure.GetTotalSeniorExpenses().GetFlow(i, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PrincipalFlow)))));
			ExpensesTable->setItem(i, ColumnCount++, new QTableWidgetItem(Commarize(Structure.GetTotalSeniorFees().GetFlow(i, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow)) + Structure.GetTotalSeniorFees().GetFlow(i, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PrincipalFlow)))));
			ExpensesTable->setItem(i, ColumnCount++, new QTableWidgetItem(Commarize(Structure.GetTotalJuniorFees().GetFlow(i, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow)) + Structure.GetTotalJuniorFees().GetFlow(i, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PrincipalFlow)))));
			ExpensesTable->setItem(i,ColumnCount++,new QTableWidgetItem(Commarize(100.0*Structure.GetAnnualizedExcess(i),2)+'%'));
			ExpensesTable->setItem(i,ColumnCount++,new QTableWidgetItem(Commarize(100.0*Structure.GetWACostOfCapital(i),2)+'%'));
			for (int j = 0; j<NumRes; j++) {
				if (Structure.GetReserveFund(j)->GetReserveFundFlow().Count()>0) {
					ExpensesTable->setItem(i, ColumnCount++, new QTableWidgetItem(Commarize(Structure.GetReserveFund(j)->GetReserveFundCurrent(i))));
				}
			}
		}
		ExpensesTable->setHorizontalHeaderLabels(HeadersStrings);
	}
	ReinvestmentsTable->setRowCount(0);
	ReinvestmentsTable->setRowCount(Structure.GetReinvested().Count());
	for (int i=0;i<Structure.GetReinvested().Count();i++){
		ReinvestmentsTable->setItem(i,0,new QTableWidgetItem(Structure.GetReinvested().GetDate(i).toString("MMM-yy")));
		ReinvestmentsTable->setItem(i, 1, new QTableWidgetItem(Commarize(Structure.GetReinvested().GetTotalFlow(i))));
	}
	GICTable->setRowCount(0);
	GICTable->setRowCount(Structure.GetGICflows().Count());
	for (int i = 0; i < Structure.GetGICflows().Count(); i++) {
		GICTable->setItem(i, 0, new QTableWidgetItem(Structure.GetGICflows().GetDate(i).toString("MMM-yy")));
		GICTable->setItem(i, 1, new QTableWidgetItem(Commarize(Structure.GetGICflows().GetFlow(i, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow)))));
		GICTable->setItem(i, 2, new QTableWidgetItem(Commarize(Structure.GetGICflows().GetFlow(i, static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PrincipalFlow)))));
		GICTable->setItem(i, 3, new QTableWidgetItem(Commarize(Structure.GetGICflows().GetFlow(i, 1+static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow)))));
		GICTable->setItem(i, 4, new QTableWidgetItem(Commarize(Structure.GetGICflows().GetFlow(i, 2+static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow)))));
	}
	StructureTable->setRowCount(0);
	StructureTable->setRowCount(Structure.GetTranchesCount());
	ResetPricesLabel();
	for (int i=0;i<Structure.GetTranchesCount();i++){
		StructureTable->setItem(i,0,new QTableWidgetItem(Structure.GetTranche(i)->GetTrancheName()));
		StructureTable->setItem(i,1,new QTableWidgetItem(Structure.GetTranche(i)->GetProrataGroup().ToString()));
		StructureTable->setItem(i,2,new QTableWidgetItem(Structure.GetTranche(i)->GetCurrency()));
		StructureTable->setItem(i,3,new QTableWidgetItem(Commarize(Structure.GetTranche(i)->GetOriginalAmount())));
		StructureTable->setItem(i,4,new QTableWidgetItem(Commarize(Structure.GetTranche(i)->GetOutstandingAmt())));
		StructureTable->setItem(i,5,new QTableWidgetItem(Commarize(Structure.GetTranche(i)->GetBondFactor()*100.0,2U)+'%'));
		StructureTable->setItem(i,6,new QTableWidgetItem(Commarize(Structure.GetCreditEnhancement(i)*100.0,2U)+'%'));
		StructureTable->setItem(i,7,new QTableWidgetItem(Structure.GetTranche(i)->GetReferenceRate(0).GetVector()));
        StructureTable->setItem(i, 8, new QTableWidgetItem(Structure.GetTranche(i)->GetCouponVector(0).GetVector()));
		StructureTable->setItem(i,10,new QTableWidgetItem(Commarize(Structure.GetTranche(i)->GetIRR()*100.0,2U)+'%'));
		StructureTable->setItem(i,11,new QTableWidgetItem(Commarize(Structure.GetTranche(i)->GetWALife(),2U)));
		StructureTable->setItem(i,12,new QTableWidgetItem(Commarize(Structure.GetTranche(i)->GetDiscountMargin())));
		StructureTable->setItem(i,13,new QTableWidgetItem(Commarize(Structure.GetTranche(i)->GetLossRate()*100.0,2U)+'%'));

		PricesLabel.append(new QLineEdit(q));
		PricesLabel.last()->setText(Commarize(Structure.GetTranche(i)->GetPrice(),2U));
		PricesLabel.last()->setValidator(new QDoubleValidator(0.0,200.0,2,q));
		PricesLabel.last()->setObjectName(QString("%1").arg(i));
        q->connect(PricesLabel.last(), SIGNAL(textEdited(QString)), q, SLOT(PriceChanged()));
		StructureTable->setCellWidget(i,9,PricesLabel.last());

		for(int j=0;j<StructureTable->columnCount();j++){
			if(j!=9)
				StructureTable->item(i,j)->setBackgroundColor(
					Structure.GetTranche(i)->GetProrataGroup(0)%2==0 ? QColor(235,241,222) : QColor(216,228,188)
				);
		}
	}
}
void SummaryView::SetStructure(const Waterfall& a,const Waterfall& ca){
    Q_D(SummaryView);
    d->Structure=a; 
    d->CallStructure = ca;
    d->DisplayStructure();
}

void SummaryView::SetStructure(const Waterfall& a)
{
    SetStructure(a, Waterfall());
}

void SummaryViewPrivate::ResetPricesLabel()
{
	for(QList<QLineEdit*>::iterator i=PricesLabel.begin();i!=PricesLabel.end();i++)
		(*i)->deleteLater();
	PricesLabel.clear();
}
void SummaryView::PriceChanged()
{
    Q_D(SummaryView);
	if(!sender())return;
	int RowToChange=sender()->objectName().toInt();
    Tranche TempTranche = *(d->Structure.GetTranche(RowToChange));
	TempTranche.SetPrice(static_cast<QLineEdit*>(sender())->text().toDouble());
    d->StructureTable->item(RowToChange, 11)->setText(
		TempTranche.GetDiscountMargin()<=0 ? QString("0%"):
		Commarize(TempTranche.GetIRR()*100.0,2U)+'%'
	);
    d->StructureTable->item(RowToChange, 13)->setText(Commarize(((TempTranche.GetDiscountMargin()))));
}
void SummaryView::ShowCallStructure(bool a)
{
    Q_D(SummaryView);
	d->MainWidget->setTabEnabled(4,a);
}