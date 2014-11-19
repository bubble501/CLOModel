#include "TrancheViewer.h"
#include "CommonFunctions.h"
#include "Tranche.h"
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QScrollBar>
TrancheViewer::TrancheViewer(QWidget* parent)
	:QWidget(parent)
{
	MainTable=new QTableWidget(this);
	MainTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	MainTable->setColumnCount(5);
	MainTable->setRowCount(0);
	QStringList HeadersStrings;
	HeadersStrings
		<< "Date"
		<< "Outstanding"
		<< "Interest"
		<< "Principal"
		<< "Cumulative Deferred Interest";
	MainTable->setHorizontalHeaderLabels(HeadersStrings);
	MainTable->verticalHeader()->setVisible(false);
	MainTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	MainTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	MainTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	QVBoxLayout* mainLay=new QVBoxLayout(this);
	mainLay->addWidget(MainTable);
}
void TrancheViewer::SetTranche(const Tranche& a){
	MainTable->setRowCount(0);
	const auto& Temp = a.GetCashFlow();
	MainTable->setRowCount(a.GetCashFlow().Count());
	int EmptyDeferred=0;
	for (qint32 i = static_cast<qint32>(TrancheCashFlow::TrancheFlowType::DeferredFlow);i < (static_cast<qint32>(TrancheCashFlow::TrancheFlowType::DeferredFlow) << 1); ++i) {
		if (a.GetCashFlow().HasFlowType(i)) EmptyDeferred++;
	}
	bool EmptyIC = !a.GetCashFlow().HasFlowType(static_cast<qint32>(TrancheCashFlow::TrancheFlowType::ICFlow));
	bool EmptyOC = !a.GetCashFlow().HasFlowType(static_cast<qint32>(TrancheCashFlow::TrancheFlowType::OCFlow));
	bool EmptyPDL = !a.GetCashFlow().HasFlowType(static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PDLOutstanding))
		&&
		!a.GetCashFlow().HasFlowType(static_cast<qint32>(TrancheCashFlow::TrancheFlowType::PDLCured))
	;

	MainTable->setColumnCount(4
		+ EmptyDeferred
		+ (EmptyIC ? 0:1)
		+ (EmptyOC ? 0 : 1)
		+ (EmptyPDL ? 0 : 2)
		);
	QStringList HeadersStrings;
	HeadersStrings
		<< "Date"
		<< "Outstanding"
		<< "Interest"
		<< "Principal";
	for (int i = 0; i < EmptyDeferred; i++) {
		HeadersStrings.append(QString("Cumulative Deferred Interest %1").arg(i + 1));
	}
	if (!EmptyOC) HeadersStrings.append("OC Test");
	if (!EmptyIC) HeadersStrings.append("IC Test");
	if (!EmptyPDL) {HeadersStrings.append("Outstanding PDL"); HeadersStrings.append("PDL Cured");}
	MainTable->setHorizontalHeaderLabels(HeadersStrings);
	int ColumnIter;
	for(int i=0;i<a.GetCashFlow().Count();i++){
		ColumnIter = 0;
		MainTable->setItem(i, ColumnIter++, new QTableWidgetItem(a.GetCashFlow().GetDate(i).toString("MMM-yy")));
		MainTable->setItem(i, ColumnIter++, new QTableWidgetItem(Commarize(a.GetCashFlow().GetAmountOutstanding(i))));
		MainTable->setItem(i, ColumnIter++, new QTableWidgetItem(Commarize(a.GetCashFlow().GetInterest(i))));
		MainTable->setItem(i, ColumnIter++, new QTableWidgetItem(Commarize(a.GetCashFlow().GetPrincipal(i))));
		for (qint32 j = static_cast<qint32>(TrancheCashFlow::TrancheFlowType::DeferredFlow); j < (static_cast<qint32>(TrancheCashFlow::TrancheFlowType::DeferredFlow) << 1); ++j) {
			if (a.GetCashFlow().HasFlowType(j)) MainTable->setItem(i, ColumnIter++, new QTableWidgetItem(Commarize(a.GetCashFlow().GetFlow(i, j))));
		}
		if (!EmptyOC) {
			if (a.GetCashFlow().GetOCTest(i) > 10.0)
				MainTable->setItem(i, ColumnIter++, new QTableWidgetItem("Infinity"));
			else {
				MainTable->setItem(i, ColumnIter, new QTableWidgetItem(QString::number(a.GetCashFlow().GetOCTest(i)*100.0, 'f', 0) + '%'));
				if (a.GetCashFlow().GetOCTest(i) < a.GetMinOClevel() && a.GetMinOClevel() > 0.0)
					MainTable->item(i, ColumnIter)->setTextColor(QColor(Qt::red));
				ColumnIter++;
			}
		}
		if (!EmptyIC) {
			if (a.GetCashFlow().GetICTest(i)>10.0)
				MainTable->setItem(i, ColumnIter++, new QTableWidgetItem("Infinity"));
			else {
				MainTable->setItem(i, ColumnIter, new QTableWidgetItem(QString::number(a.GetCashFlow().GetICTest(i)*100.0, 'f', 0) + '%'));
				if (a.GetCashFlow().GetICTest(i) < a.GetMinIClevel() && a.GetMinIClevel() > 0.0)
					MainTable->item(i, ColumnIter)->setTextColor(QColor(Qt::red));
				ColumnIter++;
			}
		}

		

		if (!EmptyPDL) {
			MainTable->setItem(i, ColumnIter++, new QTableWidgetItem(Commarize(a.GetCashFlow().GetPDLOutstanding(i))));
			MainTable->setItem(i, ColumnIter++, new QTableWidgetItem(Commarize(a.GetCashFlow().GetPDLCured(i))));
		}
	}
}
/*void TrancheViewer::resizeEvent(QResizeEvent *event){
	Q_UNUSED(event);
	for(int i=0;i<MainTable->columnCount();i++){
		MainTable->setColumnWidth(i,(MainTable->width()-MainTable->verticalScrollBar()->width())/MainTable->columnCount());
	}
}*/