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
	MainTable->setColumnCount(7);
	MainTable->setRowCount(0);
	QStringList HeadersStrings;
	HeadersStrings 
		<< "Date"
		<< "Outstanding"
		<< "Interest"
		<< "Principal"
		<< "Cumulative Deferred Interest"
		<< "OC test"
		<< "IC Test";
	MainTable->setHorizontalHeaderLabels(HeadersStrings);
	MainTable->verticalHeader()->setVisible(false);
	MainTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	MainTable->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	QVBoxLayout* mainLay=new QVBoxLayout(this);
	mainLay->addWidget(MainTable);
}
void TrancheViewer::SetTranche(const Tranche& a){
	MainTable->setRowCount(0);
	MainTable->setRowCount(a.GetCashFlow().Count());
	for(int i=0;i<a.GetCashFlow().Count();i++){
		MainTable->setItem(i,0,new QTableWidgetItem(a.GetCashFlow().GetDate(i).toString("MMM-yy")));
		MainTable->setItem(i,1,new QTableWidgetItem(Commarize(a.GetCashFlow().GetAmountOutstanding(i))));
		MainTable->setItem(i,2,new QTableWidgetItem(Commarize(a.GetCashFlow().GetInterest(i))));
		MainTable->setItem(i,3,new QTableWidgetItem(Commarize(a.GetCashFlow().GetPrincipal(i))));
		MainTable->setItem(i,4,new QTableWidgetItem(Commarize(a.GetCashFlow().GetDeferred(i))));
		if(a.GetCashFlow().GetICTest(i)>10.0)
			MainTable->setItem(i,6,new QTableWidgetItem("Infinity"));
		else{
			MainTable->setItem(i,6,new QTableWidgetItem(QString::number(a.GetCashFlow().GetICTest(i)*100.0,'f',0)+'%'));
			if(a.GetCashFlow().GetICTest(i)<a.GetMinIClevel() && a.GetMinIClevel()>0.0)
				MainTable->item(i,6)->setTextColor(QColor(Qt::red));
		}
		if(a.GetCashFlow().GetOCTest(i)>10.0)
			MainTable->setItem(i,5,new QTableWidgetItem("Infinity"));
		else{
			MainTable->setItem(i,5,new QTableWidgetItem(QString::number(a.GetCashFlow().GetOCTest(i)*100.0,'f',0)+'%'));
			if(a.GetCashFlow().GetOCTest(i)<a.GetMinOClevel() && a.GetMinOClevel()>0.0)
				MainTable->item(i,5)->setTextColor(QColor(Qt::red));
		}
	}
}
void TrancheViewer::resizeEvent(QResizeEvent *event){
	Q_UNUSED(event);
	for(int i=0;i<MainTable->columnCount();i++){
		MainTable->setColumnWidth(i,(MainTable->width()-MainTable->verticalScrollBar()->width())/MainTable->columnCount());
	}
}