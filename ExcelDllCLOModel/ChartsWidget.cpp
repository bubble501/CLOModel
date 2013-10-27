#include "ChartsWidget.h"
#include "Waterfall.h"
#include "CommonFunctions.h"
#include <QListWidget>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QStandardItemModel>
#include <KDChartLineDiagram>
#include <KDChartBarDiagram>
#include <KDChartCartesianAxis>
ChartsWidget::ChartsWidget(QWidget* parent)
	:QWidget(parent)
{
	InitialSetUp();
}
ChartsWidget::ChartsWidget(const Waterfall& a,QWidget* parent)
	:QWidget(parent)
{
	InitialSetUp();
	PlotStructure(a);
}
void ChartsWidget::InitialSetUp(){
	MaxOCtoPlot=2.0;
	MaxICtoPlot=10.0;
	ChartLinesWeight=3;
	ChartsArea=new QStackedWidget(this);
	ChartsList=new QListWidget(this);
	ChartsList->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Expanding);
	connect(ChartsList,SIGNAL(currentRowChanged(int)),ChartsArea,SLOT(setCurrentIndex(int)));
	QHBoxLayout* MainLay=new QHBoxLayout(this);
	MainLay->addWidget(ChartsList);
	MainLay->addWidget(ChartsArea);
}
void ChartsWidget::ResetCharts(){
	for (QList<KDChart::Chart*>::iterator i=Charts.begin();i!=Charts.end();i++)
		(*i)->deleteLater();
	for (QList<QStandardItemModel*>::iterator i=ChartsModels.begin();i!=ChartsModels.end();i++)
		(*i)->deleteLater();
	Charts.clear();
	ChartsModels.clear();
}
void ChartsWidget::PlotStructure(const Waterfall& a){
	if(a.GetTranchesCount()<1) return;
	{//Loans
		QStringList DatesLabels;
		ChartsModels.append(new QStandardItemModel(a.GetCalculatedMtgPayments().Count(),1,this));
		for(int i=0;i<a.GetCalculatedMtgPayments().Count();i++){
			QModelIndex TempIndex=ChartsModels.last()->index(i,0);
			ChartsModels.last()->setData(TempIndex,a.GetCalculatedMtgPayments().GetAmountOut(i)/1000000.0);
			ChartsModels.last()->setData(TempIndex,"Date: "+a.GetCalculatedMtgPayments().GetDate(i).toString("MMM-yy")+" - Value: "+Commarize(a.GetCalculatedMtgPayments().GetAmountOut(i)),Qt::ToolTipRole);
			if(i%3==0) DatesLabels << a.GetCalculatedMtgPayments().GetDate(i).toString("MMM-yy");
			else DatesLabels << "";
		}
		KDChart::LineDiagram* AnnualizedExcessLine=new KDChart::LineDiagram;
		KDChart::LineAttributes HideMissing=AnnualizedExcessLine->lineAttributes(0);
		HideMissing.setDisplayArea(true);
		AnnualizedExcessLine->setLineAttributes(0,HideMissing);
		KDChart::HeaderFooter* ChartTile=new KDChart::HeaderFooter;
		ChartTile->setText("Loans");
		ChartTile->setPosition(KDChart::Position::North);
		QPen TempPen(AnnualizedExcessLine->pen(0));
		TempPen.setStyle(Qt::NoPen);
		AnnualizedExcessLine->setPen(0,TempPen);
		AnnualizedExcessLine->setBrush(0,QBrush(Qt::darkBlue));
		AnnualizedExcessLine->setModel(ChartsModels.last());
		KDChart::CartesianAxis* XAxis=new KDChart::CartesianAxis(AnnualizedExcessLine);
		KDChart::TextAttributes RotatedText(XAxis->textAttributes());
		RotatedText.setRotation(-90);
		XAxis->setTextAttributes(RotatedText);
		XAxis->setLabels(DatesLabels);
		XAxis->setPosition(KDChart::CartesianAxis::Bottom);
		KDChart::CartesianAxis* YAxis=new KDChart::CartesianAxis(AnnualizedExcessLine);
		YAxis->setPosition(KDChart::CartesianAxis::Left);
		YAxis->setTitleText("Loans Outstanding (Millions)");
		Charts.append(new KDChart::Chart(this));
		Charts.last()->coordinatePlane()->replaceDiagram(AnnualizedExcessLine);
		KDChart::CartesianCoordinatePlane* plane =static_cast <KDChart::CartesianCoordinatePlane*>( AnnualizedExcessLine->coordinatePlane() );
		KDChart::GridAttributes ga (plane->gridAttributes(Qt::Horizontal));
		ga.setGridVisible(false);
		plane->setGridAttributes(Qt::Horizontal,ga);
		Charts.last()->addHeaderFooter(ChartTile);

		
		ChartsModels.append(new QStandardItemModel(a.GetCalculatedMtgPayments().Count(),4,this));
		for(int i=0;i<a.GetCalculatedMtgPayments().Count();i++){
			QModelIndex TempIndex=ChartsModels.last()->index(i,0);
			ChartsModels.last()->setData(TempIndex,a.GetCalculatedMtgPayments().GetLoss(i)/1000000.0);
			ChartsModels.last()->setData(TempIndex,"Date: "+a.GetCalculatedMtgPayments().GetDate(i).toString("MMM-yy")+" - Value: "+Commarize(a.GetCalculatedMtgPayments().GetLoss(i)),Qt::ToolTipRole);
			TempIndex=ChartsModels.last()->index(i,1);
			ChartsModels.last()->setData(TempIndex,a.GetCalculatedMtgPayments().GetPrepay(i)/1000000.0);
			ChartsModels.last()->setData(TempIndex,"Date: "+a.GetCalculatedMtgPayments().GetDate(i).toString("MMM-yy")+" - Value: "+Commarize(a.GetCalculatedMtgPayments().GetPrepay(i)),Qt::ToolTipRole);
			TempIndex=ChartsModels.last()->index(i,2);
			ChartsModels.last()->setData(TempIndex,a.GetCalculatedMtgPayments().GetScheduled(i)/1000000.0);
			ChartsModels.last()->setData(TempIndex,"Date: "+a.GetCalculatedMtgPayments().GetDate(i).toString("MMM-yy")+" - Value: "+Commarize(a.GetCalculatedMtgPayments().GetScheduled(i)),Qt::ToolTipRole);
			TempIndex=ChartsModels.last()->index(i,2);
			ChartsModels.last()->setData(TempIndex,a.GetCalculatedMtgPayments().GetInterest(i)/1000000.0);
			ChartsModels.last()->setData(TempIndex,"Date: "+a.GetCalculatedMtgPayments().GetDate(i).toString("MMM-yy")+" - Value: "+Commarize(a.GetCalculatedMtgPayments().GetInterest(i)),Qt::ToolTipRole);
		}
		QStringList CategoryLabels;
		CategoryLabels << "Loss"<< "Prepayments"<< "Principal"<< "Interest";
		ChartsModels.last()->setHorizontalHeaderLabels(CategoryLabels);
		KDChart::BarDiagram* FlowsDiag=new KDChart::BarDiagram;
		FlowsDiag->setType(KDChart::BarDiagram::Stacked);
		FlowsDiag->setModel(ChartsModels.last());
		KDChart::CartesianCoordinatePlane* FlowsPlane = new KDChart::CartesianCoordinatePlane(Charts.last());
		FlowsPlane->replaceDiagram(FlowsDiag);
		KDChart::GridAttributes Flowsga (FlowsPlane->gridAttributes(Qt::Horizontal));
		Flowsga.setGridVisible(false);
		FlowsPlane->setGridAttributes(Qt::Horizontal,Flowsga);
		Charts.last()->addCoordinatePlane(FlowsPlane);
		KDChart::Legend* ChartLegend=new KDChart::Legend(FlowsDiag,Charts.last());
		ChartLegend->setPosition( KDChart::Position::East );
		ChartLegend->setAlignment( Qt::AlignCenter );
		ChartLegend->setShowLines( false );
		ChartLegend->setOrientation( Qt::Vertical );
		Charts.last()->addLegend(ChartLegend);
		KDChart::CartesianAxis* FlowsYAxis=new KDChart::CartesianAxis(FlowsDiag);
		FlowsYAxis->setPosition(KDChart::CartesianAxis::Left);
		FlowsYAxis->setTitleText("Loans Cash Flows (Millions)");
		FlowsDiag->addAxis(FlowsYAxis);
		FlowsDiag->addAxis(XAxis);
		AnnualizedExcessLine->addAxis(YAxis);
		

		ChartsArea->addWidget(Charts.last());
		ChartsList->addItem("Loans");
	}
	{//Notes Outstanding
		QStringList DatesLabels;
		QStringList TranchesLabels;
		ChartsModels.append(new QStandardItemModel(a.GetTranche(0)->GetCashFlow().Count(),a.GetTranchesCount(),this));
		for (int j=0;j<a.GetTranchesCount();j++){
			for(int i=0;i<a.GetTranche(j)->GetCashFlow().Count();i++){
				QModelIndex TempIndex=ChartsModels.last()->index(i,a.GetTranchesCount()-1-j);
				ChartsModels.last()->setData(TempIndex,a.GetTranche(j)->GetCashFlow().GetAmountOutstanding(i)/1000000.0);
				ChartsModels.last()->setData(TempIndex,a.GetTranche(j)->GetTrancheName()+" - Date: "+a.GetTranche(j)->GetCashFlow().GetDate(i).toString("MMM-yy")+" - Value: "+Commarize(a.GetTranche(j)->GetCashFlow().GetAmountOutstanding(i)),Qt::ToolTipRole);
				DatesLabels << a.GetTranche(0)->GetCashFlow().GetDate(i).toString("MMM-yy");
			}
			TranchesLabels.prepend(a.GetTranche(j)->GetTrancheName());
		}
		ChartsModels.last()->setHorizontalHeaderLabels(TranchesLabels);
		KDChart::LineDiagram* AnnualizedExcessLine=new KDChart::LineDiagram;
		AnnualizedExcessLine->setType(KDChart::LineDiagram::Stacked);
		for(int j=0;j<a.GetTranchesCount();j++){
			KDChart::LineAttributes HideMissing=AnnualizedExcessLine->lineAttributes(j);
			HideMissing.setMissingValuesPolicy(KDChart::LineAttributes::MissingValuesHideSegments);
			HideMissing.setDisplayArea(true);
			AnnualizedExcessLine->setLineAttributes(j,HideMissing);
		}
		KDChart::HeaderFooter* ChartTile=new KDChart::HeaderFooter;
		ChartTile->setText("Notes Outstanding");
		ChartTile->setPosition(KDChart::Position::North);
		for (int j=0;j<a.GetTranchesCount();j++){
			QPen TempPen(AnnualizedExcessLine->pen(j));
			TempPen.setStyle(Qt::NoPen);
			AnnualizedExcessLine->setPen(j,TempPen);
		}
		AnnualizedExcessLine->setModel(ChartsModels.last());
		KDChart::CartesianAxis* XAxis=new KDChart::CartesianAxis(AnnualizedExcessLine);
		KDChart::TextAttributes RotatedText(XAxis->textAttributes());
		RotatedText.setRotation(-90);
		XAxis->setTextAttributes(RotatedText);
		XAxis->setLabels(DatesLabels);
		XAxis->setPosition(KDChart::CartesianAxis::Bottom);
		KDChart::CartesianAxis* YAxis=new KDChart::CartesianAxis(AnnualizedExcessLine);
		YAxis->setPosition(KDChart::CartesianAxis::Left);
		YAxis->setTitleText("Notes Outstanding (Millions)");
		AnnualizedExcessLine->addAxis(XAxis);
		AnnualizedExcessLine->addAxis(YAxis);
		Charts.append(new KDChart::Chart(this));
		Charts.last()->coordinatePlane()->replaceDiagram(AnnualizedExcessLine);
		KDChart::CartesianCoordinatePlane* plane =static_cast <KDChart::CartesianCoordinatePlane*>( AnnualizedExcessLine->coordinatePlane() );
		KDChart::GridAttributes ga (plane->gridAttributes(Qt::Horizontal));
		ga.setGridVisible(false);
		plane->setGridAttributes(Qt::Horizontal,ga);
		KDChart::Legend* ChartLegend=new KDChart::Legend(AnnualizedExcessLine,Charts.last());
		ChartLegend->setPosition( KDChart::Position::East );
		ChartLegend->setAlignment( Qt::AlignCenter );
		ChartLegend->setShowLines( false );
		ChartLegend->setOrientation( Qt::Vertical );
		Charts.last()->addLegend(ChartLegend);
		Charts.last()->addHeaderFooter(ChartTile);
		ChartsArea->addWidget(Charts.last());
		ChartsList->addItem("Notes Outstanding");
	}
	{//OC Test
		QStringList DatesLabels;
		QStringList TranchesLabels;
		int ColumnsCount=0;
		bool FirstInifinityFound=false;
		for (int j=0;j<a.GetTranchesCount();j++){if(a.GetTranche(j)->GetMinOClevel()>0.0)ColumnsCount++;}
		ChartsModels.append(new QStandardItemModel(a.GetTranche(0)->GetCashFlow().Count(),ColumnsCount,this));
		ColumnsCount=0;
		for (int j=0;j<a.GetTranchesCount();j++){
			if(a.GetTranche(j)->GetMinOClevel()<=0.0) continue;
			FirstInifinityFound=false;
			for(int i=0;i<a.GetTranche(j)->GetCashFlow().Count();i++){
				QModelIndex TempIndex=ChartsModels.last()->index(i,ColumnsCount);
				if(a.GetTranche(j)->GetCashFlow().GetOCTest(i)<MaxOCtoPlot || !FirstInifinityFound){
					ChartsModels.last()->setData(TempIndex,qMin(MaxOCtoPlot,a.GetTranche(j)->GetCashFlow().GetOCTest(i))*100.0);
					if(a.GetTranche(j)->GetCashFlow().GetOCTest(i)<MaxOCtoPlot)FirstInifinityFound=false;
					else FirstInifinityFound=true;
				}
				ChartsModels.last()->setData(TempIndex,a.GetTranche(j)->GetTrancheName()+" - Date: "+a.GetTranche(j)->GetCashFlow().GetDate(i).toString("MMM-yy")+" - Value: "+QString::number(a.GetTranche(j)->GetCashFlow().GetOCTest(i)*100.0,'f',2)+'%',Qt::ToolTipRole);
				DatesLabels << a.GetTranche(0)->GetCashFlow().GetDate(i).toString("MMM-yy");
			}
			ColumnsCount++;
			TranchesLabels.append(a.GetTranche(j)->GetTrancheName());
		}
		ChartsModels.last()->setHorizontalHeaderLabels(TranchesLabels);
		KDChart::LineDiagram* AnnualizedExcessLine=new KDChart::LineDiagram;
		KDChart::HeaderFooter* ChartTile=new KDChart::HeaderFooter;
		ChartTile->setText("OC Test");
		ChartTile->setPosition(KDChart::Position::North);
		AnnualizedExcessLine->setUnitSuffix("%",Qt::Vertical);
		for (int j=0;j<a.GetTranchesCount();j++){
			QPen TempPen(AnnualizedExcessLine->pen(j));
			TempPen.setWidth(ChartLinesWeight);
			AnnualizedExcessLine->setPen(j,TempPen);
		}
		AnnualizedExcessLine->setModel(ChartsModels.last());
		KDChart::CartesianAxis* XAxis=new KDChart::CartesianAxis(AnnualizedExcessLine);
		for(int j=0;j<ColumnsCount;j++){
			KDChart::LineAttributes HideMissing=AnnualizedExcessLine->lineAttributes(j);
			HideMissing.setMissingValuesPolicy(KDChart::LineAttributes::MissingValuesHideSegments);
			AnnualizedExcessLine->setLineAttributes(j,HideMissing);
		}
		KDChart::TextAttributes RotatedText(XAxis->textAttributes());
		RotatedText.setRotation(-90);
		XAxis->setTextAttributes(RotatedText);
		XAxis->setLabels(DatesLabels);
		XAxis->setPosition(KDChart::CartesianAxis::Bottom);
		KDChart::CartesianAxis* YAxis=new KDChart::CartesianAxis(AnnualizedExcessLine);
		YAxis->setPosition(KDChart::CartesianAxis::Left);
		YAxis->setTitleText("OC Test");
		AnnualizedExcessLine->addAxis(XAxis);
		AnnualizedExcessLine->addAxis(YAxis);
		Charts.append(new KDChart::Chart(this));
		Charts.last()->coordinatePlane()->replaceDiagram(AnnualizedExcessLine);
		KDChart::CartesianCoordinatePlane* plane =static_cast <KDChart::CartesianCoordinatePlane*>( AnnualizedExcessLine->coordinatePlane() );
		KDChart::GridAttributes ga (plane->gridAttributes(Qt::Horizontal));
		ga.setGridVisible(false);
		plane->setGridAttributes(Qt::Horizontal,ga);
		KDChart::Legend* ChartLegend=new KDChart::Legend(AnnualizedExcessLine,Charts.last());
		ChartLegend->setPosition( KDChart::Position::East );
		ChartLegend->setAlignment( Qt::AlignCenter );
		ChartLegend->setShowLines( false );
		ChartLegend->setOrientation( Qt::Vertical );
		Charts.last()->addLegend(ChartLegend);
		Charts.last()->addHeaderFooter(ChartTile);
		ChartsArea->addWidget(Charts.last());
		ChartsList->addItem("OC Test");
	}
	{//IC Test
		QStringList DatesLabels;
		QStringList TranchesLabels;
		int ColumnsCount=0;
		bool FirstInifinityFound=false;
		for (int j=0;j<a.GetTranchesCount();j++){if(a.GetTranche(j)->GetMinIClevel()>0.0)ColumnsCount++;}
		ChartsModels.append(new QStandardItemModel(a.GetTranche(0)->GetCashFlow().Count(),ColumnsCount,this));
		ColumnsCount=0;
		for (int j=0;j<a.GetTranchesCount();j++){
			if(a.GetTranche(j)->GetMinIClevel()<=0.0) continue;
			FirstInifinityFound=false;
			for(int i=0;i<a.GetTranche(j)->GetCashFlow().Count();i++){
				QModelIndex TempIndex=ChartsModels.last()->index(i,ColumnsCount);
				if(a.GetTranche(j)->GetCashFlow().GetICTest(i)<MaxICtoPlot || !FirstInifinityFound){
					ChartsModels.last()->setData(TempIndex,qMin(MaxICtoPlot,a.GetTranche(j)->GetCashFlow().GetICTest(i))*100.0);
					if(a.GetTranche(j)->GetCashFlow().GetICTest(i)<MaxICtoPlot)FirstInifinityFound=false;
					else FirstInifinityFound=true;
				}
				ChartsModels.last()->setData(TempIndex,a.GetTranche(j)->GetTrancheName()+" - Date: "+a.GetTranche(j)->GetCashFlow().GetDate(i).toString("MMM-yy")+" - Value: "+QString::number(a.GetTranche(j)->GetCashFlow().GetICTest(i)*100.0,'f',2)+'%',Qt::ToolTipRole);
				DatesLabels << a.GetTranche(0)->GetCashFlow().GetDate(i).toString("MMM-yy");
			}
			ColumnsCount++;
			TranchesLabels.append(a.GetTranche(j)->GetTrancheName());
		}
		ChartsModels.last()->setHorizontalHeaderLabels(TranchesLabels);
		KDChart::LineDiagram* AnnualizedExcessLine=new KDChart::LineDiagram;
		for(int j=0;j<ColumnsCount;j++){
			KDChart::LineAttributes HideMissing=AnnualizedExcessLine->lineAttributes(j);
			HideMissing.setMissingValuesPolicy(KDChart::LineAttributes::MissingValuesHideSegments);
			AnnualizedExcessLine->setLineAttributes(0,HideMissing);
		}
		KDChart::HeaderFooter* ChartTile=new KDChart::HeaderFooter;
		ChartTile->setText("IC Test");
		ChartTile->setPosition(KDChart::Position::North);
		AnnualizedExcessLine->setUnitSuffix("%",Qt::Vertical);
		for (int j=0;j<a.GetTranchesCount();j++){
			QPen TempPen(AnnualizedExcessLine->pen(j));
			TempPen.setWidth(ChartLinesWeight);
			AnnualizedExcessLine->setPen(j,TempPen);
		}
		AnnualizedExcessLine->setModel(ChartsModels.last());
		KDChart::CartesianAxis* XAxis=new KDChart::CartesianAxis(AnnualizedExcessLine);
		KDChart::TextAttributes RotatedText(XAxis->textAttributes());
		RotatedText.setRotation(-90);
		XAxis->setTextAttributes(RotatedText);
		XAxis->setLabels(DatesLabels);
		XAxis->setPosition(KDChart::CartesianAxis::Bottom);
		KDChart::CartesianAxis* YAxis=new KDChart::CartesianAxis(AnnualizedExcessLine);
		YAxis->setPosition(KDChart::CartesianAxis::Left);
		YAxis->setTitleText("IC Test");
		AnnualizedExcessLine->addAxis(XAxis);
		AnnualizedExcessLine->addAxis(YAxis);
		Charts.append(new KDChart::Chart(this));
		Charts.last()->coordinatePlane()->replaceDiagram(AnnualizedExcessLine);
		KDChart::CartesianCoordinatePlane* plane =static_cast <KDChart::CartesianCoordinatePlane*>( AnnualizedExcessLine->coordinatePlane() );
		KDChart::GridAttributes ga (plane->gridAttributes(Qt::Horizontal));
		ga.setGridVisible(false);
		plane->setGridAttributes(Qt::Horizontal,ga);
		KDChart::Legend* ChartLegend=new KDChart::Legend(AnnualizedExcessLine,Charts.last());
		ChartLegend->setPosition( KDChart::Position::East );
		ChartLegend->setAlignment( Qt::AlignCenter );
		ChartLegend->setShowLines( false );
		ChartLegend->setOrientation( Qt::Vertical );
		Charts.last()->addLegend(ChartLegend);
		Charts.last()->addHeaderFooter(ChartTile);
		ChartsArea->addWidget(Charts.last());
		ChartsList->addItem("IC Test");
	}

	{//Annualized Excess Spread
		QStringList DatesLabels;
		ChartsModels.append(new QStandardItemModel(a.GetTranche(0)->GetCashFlow().Count(),1,this));
		for(int i=0;i<a.GetTranche(0)->GetCashFlow().Count();i++){
			QModelIndex TempIndex=ChartsModels.last()->index(i,0);
			ChartsModels.last()->setData(TempIndex,a.GetAnnualizedExcess(i)*100.0);
			ChartsModels.last()->setData(TempIndex,"Date: "+a.GetTranche(0)->GetCashFlow().GetDate(i).toString("MMM-yy")+" - Value: "+QString::number(a.GetAnnualizedExcess(i)*100.0,'f',2)+'%',Qt::ToolTipRole);
			DatesLabels << a.GetTranche(0)->GetCashFlow().GetDate(i).toString("MMM-yy");
		}
		KDChart::LineDiagram* AnnualizedExcessLine=new KDChart::LineDiagram;
		KDChart::HeaderFooter* ChartTile=new KDChart::HeaderFooter;
		ChartTile->setText("Annualized Excess Spread");
		ChartTile->setPosition(KDChart::Position::North);
		AnnualizedExcessLine->setUnitSuffix("%",Qt::Vertical);
		QPen TempPen(AnnualizedExcessLine->pen(0));
		TempPen.setWidth(ChartLinesWeight);
		TempPen.setColor(Qt::darkBlue);
		AnnualizedExcessLine->setPen(0,TempPen);
		AnnualizedExcessLine->setModel(ChartsModels.last());
		KDChart::CartesianAxis* XAxis=new KDChart::CartesianAxis(AnnualizedExcessLine);
		KDChart::TextAttributes RotatedText(XAxis->textAttributes());
		RotatedText.setRotation(-90);
		XAxis->setTextAttributes(RotatedText);
		XAxis->setLabels(DatesLabels);
		XAxis->setPosition(KDChart::CartesianAxis::Bottom);
		KDChart::CartesianAxis* YAxis=new KDChart::CartesianAxis(AnnualizedExcessLine);
		YAxis->setPosition(KDChart::CartesianAxis::Left);
		YAxis->setTitleText("Excess Spread");
		AnnualizedExcessLine->addAxis(XAxis);
		AnnualizedExcessLine->addAxis(YAxis);
		Charts.append(new KDChart::Chart(this));
		Charts.last()->coordinatePlane()->replaceDiagram(AnnualizedExcessLine);
		KDChart::CartesianCoordinatePlane* plane =static_cast <KDChart::CartesianCoordinatePlane*>( AnnualizedExcessLine->coordinatePlane() );
		KDChart::GridAttributes ga (plane->gridAttributes(Qt::Horizontal));
		ga.setGridVisible(false);
		plane->setGridAttributes(Qt::Horizontal,ga);
		Charts.last()->addHeaderFooter(ChartTile);
		ChartsArea->addWidget(Charts.last());
		ChartsList->addItem("Annualized Excess Spread");
	}

	{//WA Cost of Funding
		QStringList DatesLabels;
		ChartsModels.append(new QStandardItemModel(a.GetTranche(0)->GetCashFlow().Count(),1,this));
		for(int i=0;i<a.GetTranche(0)->GetCashFlow().Count();i++){
			QModelIndex TempIndex=ChartsModels.last()->index(i,0);
			if(a.GetWACostOfCapital(i)>0.0)
				ChartsModels.last()->setData(TempIndex, a.GetWACostOfCapital(i)*100.0);
			ChartsModels.last()->setData(TempIndex,"Date: "+a.GetTranche(0)->GetCashFlow().GetDate(i).toString("MMM-yy")+" - Value: "+QString::number(a.GetWACostOfCapital(i)*100.0,'f',2)+'%',Qt::ToolTipRole);
			DatesLabels << a.GetTranche(0)->GetCashFlow().GetDate(i).toString("MMM-yy");
		}
		KDChart::LineDiagram* AnnualizedExcessLine=new KDChart::LineDiagram;
		KDChart::LineAttributes HideMissing=AnnualizedExcessLine->lineAttributes(0);
		HideMissing.setMissingValuesPolicy(KDChart::LineAttributes::MissingValuesHideSegments);
		AnnualizedExcessLine->setLineAttributes(0,HideMissing);
		KDChart::HeaderFooter* ChartTile=new KDChart::HeaderFooter;
		ChartTile->setText("WA Cost of Funding");
		ChartTile->setPosition(KDChart::Position::North);
		AnnualizedExcessLine->setUnitSuffix("%",Qt::Vertical);
		QPen TempPen(AnnualizedExcessLine->pen(0));
		TempPen.setWidth(ChartLinesWeight);
		TempPen.setColor(Qt::darkBlue);
		AnnualizedExcessLine->setPen(0,TempPen);
		AnnualizedExcessLine->setModel(ChartsModels.last());
		KDChart::CartesianAxis* XAxis=new KDChart::CartesianAxis(AnnualizedExcessLine);
		KDChart::TextAttributes RotatedText(XAxis->textAttributes());
		RotatedText.setRotation(-90);
		XAxis->setTextAttributes(RotatedText);
		XAxis->setLabels(DatesLabels);
		XAxis->setPosition(KDChart::CartesianAxis::Bottom);
		KDChart::CartesianAxis* YAxis=new KDChart::CartesianAxis(AnnualizedExcessLine);
		YAxis->setPosition(KDChart::CartesianAxis::Left);
		YAxis->setTitleText("Cost of Funding");
		AnnualizedExcessLine->addAxis(XAxis);
		AnnualizedExcessLine->addAxis(YAxis);
		Charts.append(new KDChart::Chart(this));
		Charts.last()->coordinatePlane()->replaceDiagram(AnnualizedExcessLine);
		KDChart::CartesianCoordinatePlane* plane =static_cast <KDChart::CartesianCoordinatePlane*>( AnnualizedExcessLine->coordinatePlane() );
		KDChart::GridAttributes ga (plane->gridAttributes(Qt::Horizontal));
		ga.setGridVisible(false);
		plane->setGridAttributes(Qt::Horizontal,ga);
		Charts.last()->addHeaderFooter(ChartTile);
		ChartsArea->addWidget(Charts.last());
		ChartsList->addItem("WA Cost of Funding");
	}
	for(QList<KDChart::Chart*>::iterator i=Charts.begin();i!=Charts.end();i++){
		KDChart::BackgroundAttributes BackColor((*i)->backgroundAttributes());
		BackColor.setBrush(QBrush(Qt::white));
		BackColor.setVisible(true);
		(*i)->setBackgroundAttributes(BackColor);
		(*i)->setGlobalLeading( 10, 10, 10, 10 );
	}

	ChartsList->setCurrentRow(0);
}
int ChartsWidget::ChartsCount() const {return ChartsList->count();}