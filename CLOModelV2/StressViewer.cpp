#include "StressViewer.h"
#include <QTableWidget>
#include <QLabel>
#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QScrollBar>
#include <QHeaderView>
#include <QCloseEvent>
#include <QDoubleSpinBox>
#include <QPainter>
#include "CommonFunctions.h"
StressViewer::StressViewer(QWidget* parent)
	:QWidget(parent)
	,TrancheTarg(0)
	,TypeTarg(0)
{
	setWindowIcon(QIcon(":/Icons/Logo.png"));
	setWindowTitle("CLO Stress Test Viewer");
	setMinimumSize(1024,768);
	if(!QMetaType::isRegistered(qMetaTypeId<Waterfall>()))
		qRegisterMetaType<Waterfall>("Waterfall");

	TypeLabel=new QLabel(this);
	TypeLabel->setText("Table Shows:");
	TypeCombo=new QComboBox(this);
	TypeCombo->addItem("Loss Rate");
	TypeCombo->addItem("Discount Margin");
	TypeCombo->addItem("WA Life");
	connect(TypeCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(TableTargetChanged(int)));

	TrancheLabel=new QLabel(this);
	TrancheLabel->setText("Tranche:");
	TrancheCombo=new QComboBox(this);
	connect(TrancheCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(TrancheTargetChanged(int)));

	ConstParLabel=new QLabel(this);

	PriceLabel=new QLabel(this);
	PriceLabel->setText("Price: ");
	PriceSpin=new QDoubleSpinBox(this);
	PriceSpin->setMinimum(0.0);
	PriceSpin->setMaximum(200.0);
	PriceSpin->setSingleStep(0.1);
	connect(PriceSpin,SIGNAL(valueChanged(double)),this,SLOT(PriceChanged(double)));
	PriceLabel->hide();
	PriceSpin->hide();

	QHBoxLayout *TopLay=new QHBoxLayout;
	TopLay->addWidget(TrancheLabel);
	TopLay->addWidget(TrancheCombo);
	QSpacerItem* HorSpacer1=new QSpacerItem(20,20,QSizePolicy::Expanding);
	TopLay->addItem(HorSpacer1);
	TopLay->addWidget(PriceLabel);
	TopLay->addWidget(PriceSpin);
	QSpacerItem* HorSpacer2=new QSpacerItem(20,20,QSizePolicy::Expanding);
	TopLay->addItem(HorSpacer2);
	TopLay->addWidget(ConstParLabel);
	QSpacerItem* HorSpacer3=new QSpacerItem(20,20,QSizePolicy::Expanding);
	TopLay->addItem(HorSpacer3);
	TopLay->addWidget(TypeLabel);
	TopLay->addWidget(TypeCombo);

	XDimLabel=new QLabel(this);
	XDimLabel->setAlignment(Qt::AlignCenter);
	XDimLabel->hide();
	YDimLabel=new QLabel(this);
	YDimLabel->setAlignment(Qt::AlignCenter);
	YDimLabel->hide();
	Table=new QTableWidget(this);
	Table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	Table->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	Table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	Table->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	Table->setSelectionMode(QAbstractItemView::SingleSelection);
	connect(Table,SIGNAL(cellClicked(int,int)),this,SLOT(CellSelected(int,int)));

	QGridLayout* tableLay=new QGridLayout;
	tableLay->addWidget(Table,1,1);
	tableLay->addWidget(XDimLabel,1,0);
	tableLay->addWidget(YDimLabel,0,1);

	QVBoxLayout* MainLay=new QVBoxLayout(this);
	MainLay->addLayout(TopLay);
	MainLay->addLayout(tableLay);




	GradientBase.setEasingCurve(QEasingCurve::Linear) ;
	GradientBase.setDuration(100.0) ;
	GradientBase.setKeyValueAt(0.0, QColor(Qt::yellow));
	GradientBase.setKeyValueAt(1.0, QColor(Qt::green));
}
bool StressViewer::LoadStress(const QString& filename){
	QString StressVarnames[3]={"CDR","LS","CPR"};
	StressTarget.ResetLoans();
	StressTarget.ResetStressLevels();
	StressTarget.ResetXSpann();
	StressTarget.ResetYSpann();
	StressTarget.ResetResult();
	if(!StressTarget.LoadResultsFromFile(filename)) return false;
	Table->setRowCount(0);
	Table->setColumnCount(0);
	Table->setRowCount(StressTarget.GetXSpann().size());
	Table->setColumnCount(StressTarget.GetYSpann().size());
	Table->setVerticalHeaderLabels(StressTarget.GetXSpann());
	Table->setHorizontalHeaderLabels(StressTarget.GetYSpann());

	Waterfall TmpWat=StressTarget.GetResults().value(StressTarget.GetXSpann().at(0)).value(StressTarget.GetYSpann().at(0));
	disconnect(TrancheCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(TrancheTargetChanged(int)));
	TrancheCombo->clear();
	connect(TrancheCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(TrancheTargetChanged(int)));
	for(int i=0;i<TmpWat.GetTranchesCount();i++){
		TrancheCombo->addItem(TmpWat.GetTranche(i)->GetTrancheName());
	}
	

	XDimLabel->setText(StressVarnames[StressTarget.GetXVariability()]);
	XDimLabel->show();
	YDimLabel->setText(StressVarnames[StressTarget.GetYVariability()]);
	YDimLabel->show();
	ConstParLabel->setText(StressVarnames[3-StressTarget.GetYVariability()-StressTarget.GetXVariability()] + ": " + StressTarget.GetConstantPar());
	UpdateTable();
	return true;
}
/*void StressViewer::AdjustTableSize(){
	for(int i=0;i<Table->columnCount();i++){
		Table->setColumnWidth(i,(Table->width()-Table->verticalScrollBar()->width()-Table->verticalHeader()->width())/Table->columnCount());
	}
	for(int i=0;i<Table->rowCount();i++){
		Table->setRowHeight(i,(Table->height()-Table->horizontalScrollBar()->height()-Table->horizontalHeader()->height())/Table->rowCount());
	}
}*/
/*void StressViewer::resizeEvent(QResizeEvent *event){
	Q_UNUSED(event);
	AdjustTableSize();
}*/
void StressViewer::UpdateTable(){
	QTableWidgetItem* TempItem;
	double lossValue;
	double MaxDisc, MinDisc;
	bool Minset=false;
	for(int i=0;i<StressTarget.GetXSpann().size();i++){
		for(int j=0;j<StressTarget.GetYSpann().size();j++){
			if(TypeTarg==0){
				lossValue=StressTarget.GetResults().value(StressTarget.GetXSpann().at(i)).value(StressTarget.GetYSpann().at(j)).GetTranche(TrancheTarg)->GetLossRate();
				TempItem = new QTableWidgetItem(Commarize(lossValue*100.0, 2) + "%");
				TempItem->setData(Qt::UserRole, lossValue);
				Table->setItem(i, j, TempItem);
				if(lossValue>=1.0) Table->item(i,j)->setBackgroundColor(Qt::red);
				else if(lossValue>0.0) Table->item(i,j)->setBackgroundColor(Qt::yellow);
				else Table->item(i,j)->setBackgroundColor(Qt::green);
				PriceLabel->hide();
				PriceSpin->hide();
			}else if(TypeTarg==1){
				Tranche marginTranche=*(StressTarget.GetResults().value(StressTarget.GetXSpann().at(i)).value(StressTarget.GetYSpann().at(j)).GetTranche(TrancheTarg));
				double currentDisc=marginTranche.GetDiscountMargin();
				if((i==0 && j==0) || currentDisc>MaxDisc) MaxDisc=currentDisc;
				if((!Minset || currentDisc<MinDisc) && currentDisc>0) {MinDisc=currentDisc; Minset=true;}
				TempItem = new QTableWidgetItem(Commarize(currentDisc, 0));
				TempItem->setData(Qt::UserRole, currentDisc);
				Table->setItem(i, j, TempItem);
				PriceSpin->setValue(marginTranche.GetPrice());
				PriceLabel->show();
				PriceSpin->show();
			}else if(TypeTarg==2){
				Tranche marginTranche=*(StressTarget.GetResults().value(StressTarget.GetXSpann().at(i)).value(StressTarget.GetYSpann().at(j)).GetTranche(TrancheTarg));
				double currentWAL=marginTranche.GetWALife(marginTranche.GetSettlementDate());
				if((i==0 && j==0) || currentWAL>MaxDisc) MaxDisc=currentWAL;
				if((!Minset || currentWAL<MinDisc) && currentWAL>0) {MinDisc=currentWAL; Minset=true;}
				TempItem = new QTableWidgetItem(Commarize(currentWAL, 2));
				TempItem->setData(Qt::UserRole, currentWAL);
				Table->setItem(i, j, TempItem);
				PriceLabel->hide();
				PriceSpin->hide();
			}
		}
	}
	if(TypeTarg==2){
		for(int i=0;i<StressTarget.GetXSpann().size();i++){
			for(int j=0;j<StressTarget.GetYSpann().size();j++){
				double CurrentVal = Table->item(i, j)->data(Qt::UserRole).toDouble();
				if(CurrentVal<=0.0) Table->item(i,j)->setBackgroundColor(Qt::red);
				else{
					if(MaxDisc-MinDisc<1.0) GradientBase.setCurrentTime(100.0);
					else GradientBase.setCurrentTime(100.0-(100.0*(CurrentVal-MinDisc)/(MaxDisc-MinDisc)));
					Table->item(i,j)->setBackgroundColor(
						GradientBase.currentValue().value<QColor>()
					);
				}
			}
		}
	}
	else if(TypeTarg==1){
		for(int i=0;i<StressTarget.GetXSpann().size();i++){
			for(int j=0;j<StressTarget.GetYSpann().size();j++){
				double CurrentVal = Table->item(i, j)->data(Qt::UserRole).toDouble();
				if(CurrentVal<=0.0) Table->item(i,j)->setBackgroundColor(Qt::red);
				else{
					if(MaxDisc-MinDisc<1.0) GradientBase.setCurrentTime(100.0);
					else GradientBase.setCurrentTime(100.0*(CurrentVal-MinDisc)/(MaxDisc-MinDisc));
					Table->item(i,j)->setBackgroundColor(
						GradientBase.currentValue().value<QColor>()
						);
				}
			}
		}
	}
}
void StressViewer::closeEvent(QCloseEvent *event){
	emit Closing();
	event->accept();
}
void StressViewer::CellSelected(int r,int c){
	emit StressLevelChanged(StressTarget.GetResults().value(
			StressTarget.GetXSpann().at(r)
		).value(
			StressTarget.GetYSpann().at(c)
		));
}
void StressViewer::PriceChanged(double a){
	double MaxDisc, MinDisc;
	bool Minset=false;
	QHash<int,QHash<int,double> > OriginalDiscounts;
	for(int i=0;i<StressTarget.GetXSpann().size();i++){
		for(int j=0;j<StressTarget.GetYSpann().size();j++){
			Tranche marginTranche=*(StressTarget.GetResults().value(StressTarget.GetXSpann().at(i)).value(StressTarget.GetYSpann().at(j)).GetTranche(TrancheTarg));
			double currentDisc=marginTranche.GetDiscountMargin(a);
			OriginalDiscounts[i][j]=currentDisc;
			if((i==0 && j==0) || currentDisc>MaxDisc) MaxDisc=currentDisc;
			if((!Minset || currentDisc<MinDisc) && currentDisc>0) {MinDisc=currentDisc; Minset=true;}
			Table->item(i,j)->setText(Commarize(currentDisc,0));
		}
	}
	for(int i=0;i<StressTarget.GetXSpann().size();i++){
		for(int j=0;j<StressTarget.GetYSpann().size();j++){
			double CurrentVal=OriginalDiscounts[i][j];
			if(CurrentVal<=0) Table->item(i,j)->setBackgroundColor(Qt::red);
			else{
				if(MaxDisc-MinDisc<1.0) GradientBase.setCurrentTime(100.0);
				else GradientBase.setCurrentTime(100.0*(CurrentVal-MinDisc)/(MaxDisc-MinDisc));
				Table->item(i,j)->setBackgroundColor(
					GradientBase.currentValue().value<QColor>()
					);
			}
		}
	}
}