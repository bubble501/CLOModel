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
#include <qmath.h>
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
	ChartsList->clear();
}
void ChartsWidget::PlotStructure(const Waterfall& a){
	ResetCharts();
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
		AnnualizedExcessLine->setType(KDChart::LineDiagram::Stacked);
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
			TempIndex=ChartsModels.last()->index(i,3);
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
		AnnualizedExcessLine->setModel(ChartsModels.last());

		ChartsModels.append(new QStandardItemModel(a.GetTranche(0)->GetCashFlow().Count(),1,this));
		QDate TempCallDate=a.GetCalledPeriod();
		bool CallDatePlaced=false;
		for(int i=0;i<a.GetTranche(0)->GetCashFlow().Count();i++){
			QModelIndex TempIndex=ChartsModels.last()->index(i,0);
			if(a.GetTranche(0)->GetCashFlow().GetDate(i)>=TempCallDate && !CallDatePlaced){
				ChartsModels.last()->setData(TempIndex,1.0);
				ChartsModels.last()->setData(TempIndex,"Call Date",Qt::ToolTipRole);
				CallDatePlaced=true;
			}
			else ChartsModels.last()->setData(TempIndex,0.0);
		}
		TranchesLabels.clear(); TranchesLabels << "Call Date";
		ChartsModels.last()->setHorizontalHeaderLabels(TranchesLabels);
		KDChart::BarDiagram* CallDiagram=new KDChart::BarDiagram;
		CallDiagram->setModel(ChartsModels.last());
		QBrush TmpCallBrush=CallDiagram->brush(0);
		TmpCallBrush.setColor(Qt::lightGray);
		TmpCallBrush.setStyle(Qt::DiagCrossPattern);
		CallDiagram->setBrush(0,TmpCallBrush);

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

		KDChart::CartesianAxis* XAxis=new KDChart::CartesianAxis(CallDiagram);
		KDChart::TextAttributes RotatedText(XAxis->textAttributes());
		RotatedText.setRotation(-90);
		XAxis->setTextAttributes(RotatedText);
		XAxis->setLabels(DatesLabels);
		XAxis->setPosition(KDChart::CartesianAxis::Bottom);

		KDChart::CartesianAxis* YAxis=new KDChart::CartesianAxis(AnnualizedExcessLine);
		YAxis->setPosition(KDChart::CartesianAxis::Left);
		YAxis->setTitleText("Notes Outstanding (Millions)");

		KDChart::CartesianAxis* YAxis2=new KDChart::CartesianAxis(CallDiagram);
		YAxis2->setPosition(KDChart::CartesianAxis::Right);
		KDChart::RulerAttributes tmpRlrAttr(YAxis2->rulerAttributes());
		tmpRlrAttr.setShowFirstTick(false);
		tmpRlrAttr.setShowMajorTickMarks(false);
		tmpRlrAttr.setShowMinorTickMarks(false);
		tmpRlrAttr.setShowRulerLine(false);
		YAxis2->setRulerAttributes(tmpRlrAttr);

		AnnualizedExcessLine->addAxis(YAxis);
		CallDiagram->addAxis(YAxis2);
		CallDiagram->addAxis(XAxis);
		Charts.append(new KDChart::Chart(this));
		KDChart::CartesianCoordinatePlane* plane2 = new KDChart::CartesianCoordinatePlane(Charts.last());
		Charts.last()->coordinatePlane()->replaceDiagram(AnnualizedExcessLine);
		plane2->setReferenceCoordinatePlane(Charts.last()->coordinatePlane());
		plane2->replaceDiagram(CallDiagram);
		Charts.last()->addCoordinatePlane(plane2);	
		KDChart::CartesianCoordinatePlane* plane =static_cast <KDChart::CartesianCoordinatePlane*>( AnnualizedExcessLine->coordinatePlane() );
		KDChart::GridAttributes ga (plane->gridAttributes(Qt::Horizontal));
		ga.setGridVisible(false);
		KDChart::GridAttributes gv (plane2->gridAttributes(Qt::Horizontal));
		gv.setGridVisible(false);
		plane->setGridAttributes(Qt::Horizontal,ga);
		plane2->setGridAttributes(Qt::Horizontal,gv);
		plane2->setGridAttributes(Qt::Vertical,gv);
		KDChart::Legend* ChartLegend=new KDChart::Legend(AnnualizedExcessLine,Charts.last());
		ChartLegend->addDiagram(CallDiagram);
		ChartLegend->setPosition( KDChart::Position::East );
		ChartLegend->setAlignment( Qt::AlignCenter );
		ChartLegend->setShowLines( false );
		ChartLegend->setOrientation( Qt::Vertical );
		ChartLegend->setBrush(a.GetTranchesCount(),TmpCallBrush);
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
		AnnualizedExcessLine->setModel(ChartsModels.last());

		ChartsModels.append(new QStandardItemModel(a.GetTranche(0)->GetCashFlow().Count(),1,this));
		QDate TempCallDate=a.GetCalledPeriod();
		bool CallDatePlaced=false;
		for(int i=0;i<a.GetTranche(0)->GetCashFlow().Count();i++){
			QModelIndex TempIndex=ChartsModels.last()->index(i,0);
			if(a.GetTranche(0)->GetCashFlow().GetDate(i)>=TempCallDate && !CallDatePlaced){
				ChartsModels.last()->setData(TempIndex,1.0);
				ChartsModels.last()->setData(TempIndex,"Call Date",Qt::ToolTipRole);
				CallDatePlaced=true;
			}
			else ChartsModels.last()->setData(TempIndex,0.0);
		}
		TranchesLabels.clear(); TranchesLabels << "Call Date";
		ChartsModels.last()->setHorizontalHeaderLabels(TranchesLabels);
		KDChart::BarDiagram* CallDiagram=new KDChart::BarDiagram;
		CallDiagram->setModel(ChartsModels.last());
		QBrush TmpCallBrush=CallDiagram->brush(0);
		TmpCallBrush.setColor(Qt::lightGray);
		TmpCallBrush.setStyle(Qt::DiagCrossPattern);
		CallDiagram->setBrush(0,TmpCallBrush);

		ChartTile->setText("OC Test");
		ChartTile->setPosition(KDChart::Position::North);
		AnnualizedExcessLine->setUnitSuffix("%",Qt::Vertical);
		for (int j=0;j<a.GetTranchesCount();j++){
			QPen TempPen(AnnualizedExcessLine->pen(j));
			TempPen.setWidth(ChartLinesWeight);
			AnnualizedExcessLine->setPen(j,TempPen);
		}

		
		for(int j=0;j<ColumnsCount;j++){
			KDChart::LineAttributes HideMissing=AnnualizedExcessLine->lineAttributes(j);
			HideMissing.setMissingValuesPolicy(KDChart::LineAttributes::MissingValuesHideSegments);
			AnnualizedExcessLine->setLineAttributes(j,HideMissing);
		}
		KDChart::CartesianAxis* XAxis=new KDChart::CartesianAxis(CallDiagram);
		KDChart::TextAttributes RotatedText(XAxis->textAttributes());
		RotatedText.setRotation(-90);
		XAxis->setTextAttributes(RotatedText);
		XAxis->setLabels(DatesLabels);
		XAxis->setPosition(KDChart::CartesianAxis::Bottom);
		KDChart::CartesianAxis* YAxis=new KDChart::CartesianAxis(AnnualizedExcessLine);
		YAxis->setPosition(KDChart::CartesianAxis::Left);
		YAxis->setTitleText("OC Test");
		KDChart::CartesianAxis* YAxis2=new KDChart::CartesianAxis(CallDiagram);
		YAxis2->setPosition(KDChart::CartesianAxis::Right);
		KDChart::RulerAttributes tmpRlrAttr(YAxis2->rulerAttributes());
		tmpRlrAttr.setShowFirstTick(false);
		tmpRlrAttr.setShowMajorTickMarks(false);
		tmpRlrAttr.setShowMinorTickMarks(false);
		tmpRlrAttr.setShowRulerLine(false);
		YAxis2->setRulerAttributes(tmpRlrAttr);
		AnnualizedExcessLine->addAxis(YAxis);
		CallDiagram->addAxis(YAxis2);
		CallDiagram->addAxis(XAxis);
		Charts.append(new KDChart::Chart(this));
		KDChart::CartesianCoordinatePlane* plane2 = new KDChart::CartesianCoordinatePlane(Charts.last());
		Charts.last()->coordinatePlane()->replaceDiagram(AnnualizedExcessLine);
		plane2->setReferenceCoordinatePlane(Charts.last()->coordinatePlane());
		plane2->replaceDiagram(CallDiagram);
		Charts.last()->addCoordinatePlane(plane2);
		KDChart::CartesianCoordinatePlane* plane =static_cast <KDChart::CartesianCoordinatePlane*>( AnnualizedExcessLine->coordinatePlane() );
		KDChart::GridAttributes ga (plane->gridAttributes(Qt::Horizontal));
		ga.setGridVisible(false);
		KDChart::GridAttributes gv (plane2->gridAttributes(Qt::Horizontal));
		gv.setGridVisible(false);
		plane->setGridAttributes(Qt::Horizontal,ga);
		plane2->setGridAttributes(Qt::Horizontal,gv);
		plane2->setGridAttributes(Qt::Vertical,gv);
		KDChart::Legend* ChartLegend=new KDChart::Legend(AnnualizedExcessLine,Charts.last());
		ChartLegend->addDiagram(CallDiagram);
		ChartLegend->setPosition( KDChart::Position::East );
		ChartLegend->setAlignment( Qt::AlignCenter );
		ChartLegend->setShowLines( false );
		ChartLegend->setOrientation( Qt::Vertical );
		ChartLegend->setBrush(a.GetTranchesCount(),TmpCallBrush);
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
					ChartsModels.last()->setData(TempIndex,qMax(0.0,qMin(MaxICtoPlot,a.GetTranche(j)->GetCashFlow().GetICTest(i))*100.0));
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
		AnnualizedExcessLine->setModel(ChartsModels.last());

		ChartsModels.append(new QStandardItemModel(a.GetTranche(0)->GetCashFlow().Count(),1,this));
		QDate TempCallDate=a.GetCalledPeriod();
		bool CallDatePlaced=false;
		for(int i=0;i<a.GetTranche(0)->GetCashFlow().Count();i++){
			QModelIndex TempIndex=ChartsModels.last()->index(i,0);
			if(a.GetTranche(0)->GetCashFlow().GetDate(i)>=TempCallDate && !CallDatePlaced){
				ChartsModels.last()->setData(TempIndex,1.0);
				ChartsModels.last()->setData(TempIndex,"Call Date",Qt::ToolTipRole);
				CallDatePlaced=true;
			}
			else ChartsModels.last()->setData(TempIndex,0.0);
		}
		TranchesLabels.clear(); TranchesLabels << "Call Date";
		ChartsModels.last()->setHorizontalHeaderLabels(TranchesLabels);
		KDChart::BarDiagram* CallDiagram=new KDChart::BarDiagram;
		CallDiagram->setModel(ChartsModels.last());
		QBrush TmpCallBrush=CallDiagram->brush(0);
		TmpCallBrush.setColor(Qt::lightGray);
		TmpCallBrush.setStyle(Qt::DiagCrossPattern);
		CallDiagram->setBrush(0,TmpCallBrush);

		KDChart::HeaderFooter* ChartTile=new KDChart::HeaderFooter;
		ChartTile->setText("IC Test");
		ChartTile->setPosition(KDChart::Position::North);
		AnnualizedExcessLine->setUnitSuffix("%",Qt::Vertical);
		for (int j=0;j<a.GetTranchesCount();j++){
			QPen TempPen(AnnualizedExcessLine->pen(j));
			TempPen.setWidth(ChartLinesWeight);
			AnnualizedExcessLine->setPen(j,TempPen);
		}
		KDChart::CartesianAxis* XAxis=new KDChart::CartesianAxis(CallDiagram);
		KDChart::TextAttributes RotatedText(XAxis->textAttributes());
		RotatedText.setRotation(-90);
		XAxis->setTextAttributes(RotatedText);
		XAxis->setLabels(DatesLabels);
		XAxis->setPosition(KDChart::CartesianAxis::Bottom);
		KDChart::CartesianAxis* YAxis=new KDChart::CartesianAxis(AnnualizedExcessLine);
		YAxis->setPosition(KDChart::CartesianAxis::Left);
		YAxis->setTitleText("IC Test");
		KDChart::CartesianAxis* YAxis2=new KDChart::CartesianAxis(CallDiagram);
		YAxis2->setPosition(KDChart::CartesianAxis::Right);
		KDChart::RulerAttributes tmpRlrAttr(YAxis2->rulerAttributes());
		tmpRlrAttr.setShowFirstTick(false);
		tmpRlrAttr.setShowMajorTickMarks(false);
		tmpRlrAttr.setShowMinorTickMarks(false);
		tmpRlrAttr.setShowRulerLine(false);
		YAxis2->setRulerAttributes(tmpRlrAttr);
		AnnualizedExcessLine->addAxis(YAxis);
		CallDiagram->addAxis(YAxis2);
		CallDiagram->addAxis(XAxis);
		Charts.append(new KDChart::Chart(this));
		KDChart::CartesianCoordinatePlane* plane2 = new KDChart::CartesianCoordinatePlane(Charts.last());
		Charts.last()->coordinatePlane()->replaceDiagram(AnnualizedExcessLine);
		plane2->setReferenceCoordinatePlane(Charts.last()->coordinatePlane());
		plane2->replaceDiagram(CallDiagram);
		Charts.last()->addCoordinatePlane(plane2);
		KDChart::CartesianCoordinatePlane* plane =static_cast <KDChart::CartesianCoordinatePlane*>( AnnualizedExcessLine->coordinatePlane() );
		KDChart::GridAttributes ga (plane->gridAttributes(Qt::Horizontal));
		ga.setGridVisible(false);
		KDChart::GridAttributes gv (plane2->gridAttributes(Qt::Horizontal));
		gv.setGridVisible(false);
		plane->setGridAttributes(Qt::Horizontal,ga);
		plane2->setGridAttributes(Qt::Horizontal,gv);
		plane2->setGridAttributes(Qt::Vertical,gv);
		KDChart::Legend* ChartLegend=new KDChart::Legend(AnnualizedExcessLine,Charts.last());
		ChartLegend->addDiagram(CallDiagram);
		ChartLegend->setPosition( KDChart::Position::East );
		ChartLegend->setAlignment( Qt::AlignCenter );
		ChartLegend->setShowLines( false );
		ChartLegend->setOrientation( Qt::Vertical );
		ChartLegend->setBrush(a.GetTranchesCount(),TmpCallBrush);
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
		AnnualizedExcessLine->setModel(ChartsModels.last());

		ChartsModels.append(new QStandardItemModel(a.GetTranche(0)->GetCashFlow().Count(),1,this));
		QDate TempCallDate=a.GetCalledPeriod();
		bool CallDatePlaced=false;
		for(int i=0;i<a.GetTranche(0)->GetCashFlow().Count();i++){
			QModelIndex TempIndex=ChartsModels.last()->index(i,0);
			if(a.GetTranche(0)->GetCashFlow().GetDate(i)>=TempCallDate && !CallDatePlaced){
				ChartsModels.last()->setData(TempIndex,1.0);
				ChartsModels.last()->setData(TempIndex,"Call Date",Qt::ToolTipRole);
				CallDatePlaced=true;
			}
			else ChartsModels.last()->setData(TempIndex,0.0);
		}
		KDChart::BarDiagram* CallDiagram=new KDChart::BarDiagram;
		CallDiagram->setModel(ChartsModels.last());
		QBrush TmpCallBrush=CallDiagram->brush(0);
		TmpCallBrush.setColor(Qt::lightGray);
		TmpCallBrush.setStyle(Qt::DiagCrossPattern);
		CallDiagram->setBrush(0,TmpCallBrush);

		KDChart::HeaderFooter* ChartTile=new KDChart::HeaderFooter;
		ChartTile->setText("Annualized Excess Spread");
		ChartTile->setPosition(KDChart::Position::North);
		AnnualizedExcessLine->setUnitSuffix("%",Qt::Vertical);
		QPen TempPen(AnnualizedExcessLine->pen(0));
		TempPen.setWidth(ChartLinesWeight);
		TempPen.setColor(Qt::darkBlue);
		AnnualizedExcessLine->setPen(0,TempPen);
		KDChart::CartesianAxis* XAxis=new KDChart::CartesianAxis(CallDiagram);
		KDChart::TextAttributes RotatedText(XAxis->textAttributes());
		RotatedText.setRotation(-90);
		XAxis->setTextAttributes(RotatedText);
		XAxis->setLabels(DatesLabels);
		XAxis->setPosition(KDChart::CartesianAxis::Bottom);
		KDChart::CartesianAxis* YAxis=new KDChart::CartesianAxis(AnnualizedExcessLine);
		YAxis->setPosition(KDChart::CartesianAxis::Left);
		YAxis->setTitleText("Excess Spread");
		KDChart::CartesianAxis* YAxis2=new KDChart::CartesianAxis(CallDiagram);
		YAxis2->setPosition(KDChart::CartesianAxis::Right);
		KDChart::RulerAttributes tmpRlrAttr(YAxis2->rulerAttributes());
		tmpRlrAttr.setShowFirstTick(false);
		tmpRlrAttr.setShowMajorTickMarks(false);
		tmpRlrAttr.setShowMinorTickMarks(false);
		tmpRlrAttr.setShowRulerLine(false);
		YAxis2->setRulerAttributes(tmpRlrAttr);
		AnnualizedExcessLine->addAxis(YAxis);
		CallDiagram->addAxis(YAxis2);
		CallDiagram->addAxis(XAxis);
		Charts.append(new KDChart::Chart(this));
		KDChart::CartesianCoordinatePlane* plane2 = new KDChart::CartesianCoordinatePlane(Charts.last());
		Charts.last()->coordinatePlane()->replaceDiagram(AnnualizedExcessLine);
		plane2->setReferenceCoordinatePlane(Charts.last()->coordinatePlane());
		plane2->replaceDiagram(CallDiagram);
		Charts.last()->addCoordinatePlane(plane2);
		KDChart::CartesianCoordinatePlane* plane =static_cast <KDChart::CartesianCoordinatePlane*>( AnnualizedExcessLine->coordinatePlane() );
		KDChart::GridAttributes ga (plane->gridAttributes(Qt::Horizontal));
		ga.setGridVisible(false);
		KDChart::GridAttributes gv (plane2->gridAttributes(Qt::Horizontal));
		gv.setGridVisible(false);
		plane->setGridAttributes(Qt::Horizontal,ga);
		plane2->setGridAttributes(Qt::Horizontal,gv);
		plane2->setGridAttributes(Qt::Vertical,gv);
		Charts.last()->addHeaderFooter(ChartTile);
		ChartsArea->addWidget(Charts.last());
		ChartsList->addItem("Annualized Excess Spread");
	}

	{//WA Cost of Funding
		QStringList DatesLabels;
		ChartsModels.append(new QStandardItemModel(a.GetTranche(0)->GetCashFlow().Count(),2,this));
		for(int i=0;i<a.GetTranche(0)->GetCashFlow().Count();i++){
			QModelIndex TempIndex=ChartsModels.last()->index(i,0);
			if(a.GetWACostOfCapital(i)>0.0)
				ChartsModels.last()->setData(TempIndex, a.GetWACostOfCapital(i)*100.0);
			ChartsModels.last()->setData(TempIndex,"Date: "+a.GetTranche(0)->GetCashFlow().GetDate(i).toString("MMM-yy")+" - Value: "+QString::number(a.GetWACostOfCapital(i)*100.0,'f',2)+'%',Qt::ToolTipRole);
			
			TempIndex=ChartsModels.last()->index(i,1);
			if(a.GetCalculatedMtgPayments().GetWAcoupon(a.GetTranche(0)->GetCashFlow().GetDate(i))>0.0)
				ChartsModels.last()->setData(TempIndex,a.GetCalculatedMtgPayments().GetWAcoupon(a.GetTranche(0)->GetCashFlow().GetDate(i))*100.0);
			ChartsModels.last()->setData(TempIndex,"Date: "+a.GetTranche(0)->GetCashFlow().GetDate(i).toString("MMM-yy")+" - Value: "+QString::number(a.GetCalculatedMtgPayments().GetWAcoupon(a.GetTranche(0)->GetCashFlow().GetDate(i))*100.0,'f',2)+'%',Qt::ToolTipRole);
			
			DatesLabels << a.GetTranche(0)->GetCashFlow().GetDate(i).toString("MMM-yy");
		}
		QStringList CategoryLabels; 
		CategoryLabels << "WA Cost of Funding"<< "WA Loans Coupon";
		ChartsModels.last()->setHorizontalHeaderLabels(CategoryLabels);
		KDChart::LineDiagram* AnnualizedExcessLine=new KDChart::LineDiagram;
		AnnualizedExcessLine->setModel(ChartsModels.last());

		ChartsModels.append(new QStandardItemModel(a.GetTranche(0)->GetCashFlow().Count(),1,this));
		QDate TempCallDate=a.GetCalledPeriod();
		bool CallDatePlaced=false;
		for(int i=0;i<a.GetTranche(0)->GetCashFlow().Count();i++){
			QModelIndex TempIndex=ChartsModels.last()->index(i,0);
			if(a.GetTranche(0)->GetCashFlow().GetDate(i)>=TempCallDate && !CallDatePlaced){
				ChartsModels.last()->setData(TempIndex,1.0);
				ChartsModels.last()->setData(TempIndex,"Call Date",Qt::ToolTipRole);
				CallDatePlaced=true;
			}
			else ChartsModels.last()->setData(TempIndex,0.0);
		}
		CategoryLabels.clear(); CategoryLabels << "Call Date";
		ChartsModels.last()->setHorizontalHeaderLabels(CategoryLabels);
		KDChart::BarDiagram* CallDiagram=new KDChart::BarDiagram;
		CallDiagram->setModel(ChartsModels.last());
		QBrush TmpCallBrush=CallDiagram->brush(0);
		TmpCallBrush.setColor(Qt::lightGray);
		TmpCallBrush.setStyle(Qt::DiagCrossPattern);
		CallDiagram->setBrush(0,TmpCallBrush);

		KDChart::LineAttributes HideMissing=AnnualizedExcessLine->lineAttributes(0);
		HideMissing.setMissingValuesPolicy(KDChart::LineAttributes::MissingValuesHideSegments);
		AnnualizedExcessLine->setLineAttributes(0,HideMissing);
		AnnualizedExcessLine->setLineAttributes(1,HideMissing);
		KDChart::HeaderFooter* ChartTile=new KDChart::HeaderFooter;
		ChartTile->setText("Costs and Revenues");
		ChartTile->setPosition(KDChart::Position::North);
		AnnualizedExcessLine->setUnitSuffix("%",Qt::Vertical);
		QPen TempPen(AnnualizedExcessLine->pen(0));
		TempPen.setWidth(ChartLinesWeight);
		TempPen.setColor(Qt::darkBlue);
		AnnualizedExcessLine->setPen(0,TempPen);
		QPen TempPen2(AnnualizedExcessLine->pen(1));
		TempPen2.setWidth(ChartLinesWeight);
		TempPen2.setColor(Qt::darkRed);
		AnnualizedExcessLine->setPen(1,TempPen2);

		KDChart::CartesianAxis* XAxis=new KDChart::CartesianAxis(CallDiagram);
		KDChart::TextAttributes RotatedText(XAxis->textAttributes());
		RotatedText.setRotation(-90);
		XAxis->setTextAttributes(RotatedText);
		XAxis->setLabels(DatesLabels);
		XAxis->setPosition(KDChart::CartesianAxis::Bottom);
		KDChart::CartesianAxis* YAxis=new KDChart::CartesianAxis(AnnualizedExcessLine);
		YAxis->setPosition(KDChart::CartesianAxis::Left);
		KDChart::CartesianAxis* YAxis2=new KDChart::CartesianAxis(CallDiagram);
		YAxis2->setPosition(KDChart::CartesianAxis::Right);
		KDChart::RulerAttributes tmpRlrAttr(YAxis2->rulerAttributes());
		tmpRlrAttr.setShowFirstTick(false);
		tmpRlrAttr.setShowMajorTickMarks(false);
		tmpRlrAttr.setShowMinorTickMarks(false);
		tmpRlrAttr.setShowRulerLine(false);
		YAxis2->setRulerAttributes(tmpRlrAttr);
		AnnualizedExcessLine->addAxis(YAxis);
		CallDiagram->addAxis(YAxis2);
		CallDiagram->addAxis(XAxis);
		Charts.append(new KDChart::Chart(this));
		KDChart::CartesianCoordinatePlane* plane2 = new KDChart::CartesianCoordinatePlane(Charts.last());
		Charts.last()->coordinatePlane()->replaceDiagram(AnnualizedExcessLine);
		plane2->setReferenceCoordinatePlane(Charts.last()->coordinatePlane());
		plane2->replaceDiagram(CallDiagram);
		Charts.last()->addCoordinatePlane(plane2);
		KDChart::CartesianCoordinatePlane* plane =static_cast <KDChart::CartesianCoordinatePlane*>( AnnualizedExcessLine->coordinatePlane() );
		KDChart::GridAttributes ga (plane->gridAttributes(Qt::Horizontal));
		ga.setGridVisible(false);
		KDChart::GridAttributes gv (plane2->gridAttributes(Qt::Horizontal));
		gv.setGridVisible(false);
		plane->setGridAttributes(Qt::Horizontal,ga);
		plane2->setGridAttributes(Qt::Horizontal,gv);
		plane2->setGridAttributes(Qt::Vertical,gv);
		KDChart::Legend* ChartLegend=new KDChart::Legend(AnnualizedExcessLine,Charts.last());
		ChartLegend->addDiagram(CallDiagram);
		ChartLegend->setPosition( KDChart::Position::East );
		ChartLegend->setAlignment( Qt::AlignCenter );
		ChartLegend->setShowLines( false );
		ChartLegend->setOrientation( Qt::Vertical );
		ChartLegend->setColor(0,Qt::darkBlue);
		ChartLegend->setColor(1,Qt::darkRed);
		ChartLegend->setBrush(2,TmpCallBrush);
		Charts.last()->addLegend(ChartLegend);
		Charts.last()->addHeaderFooter(ChartTile);
		ChartsArea->addWidget(Charts.last());
		ChartsList->addItem("Costs and Revenues");
	}

	{//Call Value to Equity
		QStringList DatesLabels;
		ChartsModels.append(new QStandardItemModel(a.GetTranche(0)->GetCashFlow().Count(),1,this));
		for(int i=0;i<a.GetTranche(0)->GetCashFlow().Count();i++){
			QModelIndex TempIndex=ChartsModels.last()->index(i,0);
			ChartsModels.last()->setData(TempIndex,a.GetCallEquityRatio(i)*100.0);
			ChartsModels.last()->setData(TempIndex,"Date: "+a.GetTranche(0)->GetCashFlow().GetDate(i).toString("MMM-yy")+" - Value: "+QString::number(a.GetCallEquityRatio(i)*100.0,'f',2)+'%',Qt::ToolTipRole);
			DatesLabels << a.GetTranche(0)->GetCashFlow().GetDate(i).toString("MMM-yy");
		}
		KDChart::LineDiagram* AnnualizedExcessLine=new KDChart::LineDiagram;
		KDChart::HeaderFooter* ChartTile=new KDChart::HeaderFooter;
		ChartTile->setText("Pool Call Value to Equity Ratio");
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
		ChartsList->addItem("Pool Call Value to Equity Ratio");
	}

	{//CPR and Loss Rate
		QStringList DatesLabels;
		double TempValue;
		ChartsModels.append(new QStandardItemModel(a.GetCalculatedMtgPayments().Count(),1,this));
		for(int i=0;i<a.GetCalculatedMtgPayments().Count();i++){
			QModelIndex TempIndex=ChartsModels.last()->index(i,0);
			TempValue=1.0-qPow(1.0-(a.GetCalculatedMtgPayments().GetLoss(i)/(a.GetCalculatedMtgPayments().GetAmountOut(i-1)-a.GetCalculatedMtgPayments().GetScheduled(i))),12.0);
			ChartsModels.last()->setData(TempIndex,TempValue*100.0);
			ChartsModels.last()->setData(TempIndex,"Date: "+a.GetCalculatedMtgPayments().GetDate(i).toString("MMM-yy")+" - Value: "+QString::number(TempValue*100.0,'f',2)+'%',Qt::ToolTipRole);
			if(i%3==0) DatesLabels << a.GetCalculatedMtgPayments().GetDate(i).toString("MMM-yy");
			else DatesLabels << "";
		}
		QStringList CategoryLabels; CategoryLabels << "Loss Rate";
		ChartsModels.last()->setHorizontalHeaderLabels(CategoryLabels);
		KDChart::LineDiagram* LRline=new KDChart::LineDiagram;
		LRline->setModel(ChartsModels.last());
		ChartsModels.append(new QStandardItemModel(a.GetCalculatedMtgPayments().Count(),1,this));
		for(int i=0;i<a.GetCalculatedMtgPayments().Count();i++){
			QModelIndex TempIndex=ChartsModels.last()->index(i,0);
			TempValue=1.0-qPow(1.0-(a.GetCalculatedMtgPayments().GetPrepay(i)/(a.GetCalculatedMtgPayments().GetAmountOut(i-1)-a.GetCalculatedMtgPayments().GetScheduled(i))),12.0);
			ChartsModels.last()->setData(TempIndex,TempValue*100.0);
			ChartsModels.last()->setData(TempIndex,"Date: "+a.GetCalculatedMtgPayments().GetDate(i).toString("MMM-yy")+" - Value: "+QString::number(TempValue*100.0,'f',2)+'%',Qt::ToolTipRole);
		}
		CategoryLabels.clear(); CategoryLabels << "CPR";
		ChartsModels.last()->setHorizontalHeaderLabels(CategoryLabels);
		KDChart::LineDiagram* CPRline=new KDChart::LineDiagram;
		CPRline->setModel(ChartsModels.last());
		KDChart::CartesianAxis* XAxis=new KDChart::CartesianAxis(CPRline);
		KDChart::CartesianAxis* YAxis=new KDChart::CartesianAxis(LRline);
		KDChart::CartesianAxis* YAxis2=new KDChart::CartesianAxis(CPRline);
		XAxis->setPosition(KDChart::CartesianAxis::Bottom);
		YAxis->setPosition(KDChart::CartesianAxis::Left);
		YAxis2->setPosition(KDChart::CartesianAxis::Right);
		YAxis->setTitleText("Loss Rate");
		YAxis2->setTitleText("CPR");
		LRline->addAxis(YAxis);
		CPRline->addAxis(XAxis);
		CPRline->addAxis(YAxis2);
		LRline->setUnitSuffix("%",Qt::Vertical);
		CPRline->setUnitSuffix("%",Qt::Vertical);
		KDChart::HeaderFooter* ChartTile=new KDChart::HeaderFooter;
		ChartTile->setText("CPR and Loss Rate");
		ChartTile->setPosition(KDChart::Position::North);
		QPen TempPen(LRline->pen(0));
		TempPen.setWidth(ChartLinesWeight);
		TempPen.setColor(Qt::darkBlue);
		LRline->setPen(0,TempPen);
		QPen TempPen2(CPRline->pen(0));
		TempPen2.setWidth(ChartLinesWeight);
		TempPen2.setColor(Qt::darkRed);
		CPRline->setPen(0,TempPen2);
		KDChart::TextAttributes RotatedText(XAxis->textAttributes());
		RotatedText.setRotation(-90);
		XAxis->setTextAttributes(RotatedText);
		XAxis->setLabels(DatesLabels);
		Charts.append(new KDChart::Chart(this));
		KDChart::CartesianCoordinatePlane* plane2 = new KDChart::CartesianCoordinatePlane(Charts.last());
		Charts.last()->coordinatePlane()->replaceDiagram(LRline);
		plane2->setReferenceCoordinatePlane(Charts.last()->coordinatePlane());
		plane2->replaceDiagram(CPRline);
		Charts.last()->addCoordinatePlane(plane2);		
		KDChart::Legend* ChartLegend=new KDChart::Legend(LRline,Charts.last());
		ChartLegend->addDiagram(CPRline);
		ChartLegend->setPosition( KDChart::Position::East );
		ChartLegend->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
		ChartLegend->setShowLines(false);
		ChartLegend->setOrientation(Qt::Vertical);
		ChartLegend->setColor(0,Qt::darkBlue);
		ChartLegend->setColor(1,Qt::darkRed);
		Charts.last()->addLegend(ChartLegend);
		Charts.last()->addHeaderFooter(ChartTile);
		KDChart::GridAttributes gv( plane2->gridAttributes(Qt::Horizontal));
		gv.setGridVisible( false );
		plane2->setGridAttributes( Qt::Horizontal, gv );
		KDChart::CartesianCoordinatePlane* plane =qobject_cast <KDChart::CartesianCoordinatePlane*>(Charts.last()->coordinatePlane());
		KDChart::GridAttributes ga (plane->gridAttributes(Qt::Horizontal));
		ga.setGridVisible(false);
		plane->setGridAttributes(Qt::Horizontal,ga);
		plane->setGridAttributes(Qt::Vertical,ga);
		ChartsArea->addWidget(Charts.last());
		ChartsList->addItem("CPR and LR");
	}
	{//Equity Return
		QStringList DatesLabels;
		double TempValue;
		ChartsModels.append(new QStandardItemModel(a.GetTranche(0)->GetCashFlow().Count(),1,this));
		for(int i=0;i<a.GetTranche(0)->GetCashFlow().Count();i++){
			QModelIndex TempIndex=ChartsModels.last()->index(i,0);
			TempValue=a.GetEquityReturn(i)*100.0;
			ChartsModels.last()->setData(TempIndex,TempValue);
			ChartsModels.last()->setData(TempIndex,"Date: "+a.GetTranche(0)->GetCashFlow().GetDate(i).toString("MMM-yy")+" - Value: "+QString::number(TempValue,'f',2)+'%',Qt::ToolTipRole);
			DatesLabels << a.GetTranche(0)->GetCashFlow().GetDate(i).toString("MMM-yy");
		}
		QStringList CategoryLabels; CategoryLabels << "Equity Return";
		ChartsModels.last()->setHorizontalHeaderLabels(CategoryLabels);
		KDChart::LineDiagram* LRline=new KDChart::LineDiagram;
		LRline->setModel(ChartsModels.last());

		ChartsModels.append(new QStandardItemModel(a.GetTranche(0)->GetCashFlow().Count(),1,this));
		for(int i=0;i<a.GetTranche(0)->GetCashFlow().Count();i++){
			QModelIndex TempIndex=ChartsModels.last()->index(i,0);
			TempValue=a.GetCumulativeEquityReturn(i)*100.0;
			ChartsModels.last()->setData(TempIndex,TempValue);
			ChartsModels.last()->setData(TempIndex,"Date: "+a.GetTranche(0)->GetCashFlow().GetDate(i).toString("MMM-yy")+" - Value: "+QString::number(TempValue,'f',2)+'%',Qt::ToolTipRole);
		}
		CategoryLabels.clear(); CategoryLabels << "Cumulative";
		ChartsModels.last()->setHorizontalHeaderLabels(CategoryLabels);
		KDChart::LineDiagram* CPRline=new KDChart::LineDiagram;
		CPRline->setModel(ChartsModels.last());

		ChartsModels.append(new QStandardItemModel(a.GetTranche(0)->GetCashFlow().Count(),1,this));
		QDate TempCallDate=a.GetCalledPeriod();
		bool CallDatePlaced=false;
		for(int i=0;i<a.GetTranche(0)->GetCashFlow().Count();i++){
			QModelIndex TempIndex=ChartsModels.last()->index(i,0);
			if(a.GetTranche(0)->GetCashFlow().GetDate(i)>=TempCallDate && !CallDatePlaced){
				ChartsModels.last()->setData(TempIndex,1.0);
				ChartsModels.last()->setData(TempIndex,"Call Date",Qt::ToolTipRole);
				CallDatePlaced=true;
			}
			else ChartsModels.last()->setData(TempIndex,0.0);
		}
		CategoryLabels.clear(); CategoryLabels << "Call Date";
		ChartsModels.last()->setHorizontalHeaderLabels(CategoryLabels);
		KDChart::BarDiagram* CallDiagram=new KDChart::BarDiagram;
		CallDiagram->setModel(ChartsModels.last());
		QBrush TmpCallBrush=CallDiagram->brush(0);
		TmpCallBrush.setColor(Qt::lightGray);
		TmpCallBrush.setStyle(Qt::DiagCrossPattern);
		CallDiagram->setBrush(0,TmpCallBrush);

		KDChart::CartesianAxis* XAxis=new KDChart::CartesianAxis(CPRline);
		KDChart::CartesianAxis* YAxis=new KDChart::CartesianAxis(LRline);
		KDChart::CartesianAxis* YAxis2=new KDChart::CartesianAxis(CPRline);
		XAxis->setPosition(KDChart::CartesianAxis::Bottom);
		YAxis->setPosition(KDChart::CartesianAxis::Left);
		YAxis2->setPosition(KDChart::CartesianAxis::Right);
		YAxis->setTitleText("Equity Return");
		YAxis2->setTitleText("Cumulative Equity Return");

		KDChart::CartesianAxis* YAxis3=new KDChart::CartesianAxis(CallDiagram);
		YAxis3->setPosition(KDChart::CartesianAxis::Right);
		KDChart::RulerAttributes tmpRlrAttr(YAxis3->rulerAttributes());
		tmpRlrAttr.setShowFirstTick(false);
		tmpRlrAttr.setShowMajorTickMarks(false);
		tmpRlrAttr.setShowMinorTickMarks(false);
		tmpRlrAttr.setShowRulerLine(false);
		YAxis3->setRulerAttributes(tmpRlrAttr);

		LRline->addAxis(YAxis);
		CPRline->addAxis(XAxis);
		CPRline->addAxis(YAxis2);
		CallDiagram->addAxis(YAxis3);
		LRline->setUnitSuffix("%",Qt::Vertical);
		CPRline->setUnitSuffix("%",Qt::Vertical);
		KDChart::HeaderFooter* ChartTile=new KDChart::HeaderFooter;
		ChartTile->setText("Equity Return");
		ChartTile->setPosition(KDChart::Position::North);
		QPen TempPen(LRline->pen(0));
		TempPen.setWidth(ChartLinesWeight);
		TempPen.setColor(Qt::darkBlue);
		LRline->setPen(0,TempPen);
		QPen TempPen2(CPRline->pen(0));
		TempPen2.setWidth(ChartLinesWeight);
		TempPen2.setColor(Qt::darkRed);
		CPRline->setPen(0,TempPen2);
		KDChart::TextAttributes RotatedText(XAxis->textAttributes());
		RotatedText.setRotation(-90);
		XAxis->setTextAttributes(RotatedText);
		XAxis->setLabels(DatesLabels);
		Charts.append(new KDChart::Chart(this));
		KDChart::CartesianCoordinatePlane* plane2 = new KDChart::CartesianCoordinatePlane(Charts.last());
		KDChart::CartesianCoordinatePlane* plane3 = new KDChart::CartesianCoordinatePlane(Charts.last());
		Charts.last()->coordinatePlane()->replaceDiagram(LRline);
		plane2->setReferenceCoordinatePlane(Charts.last()->coordinatePlane());
		plane2->replaceDiagram(CPRline);
		Charts.last()->addCoordinatePlane(plane2);
		plane3->setReferenceCoordinatePlane(Charts.last()->coordinatePlane());
		plane3->replaceDiagram(CallDiagram);
		Charts.last()->addCoordinatePlane(plane3);	
		KDChart::Legend* ChartLegend=new KDChart::Legend(LRline,Charts.last());
		ChartLegend->addDiagram(CPRline);
		ChartLegend->addDiagram(CallDiagram);
		ChartLegend->setPosition( KDChart::Position::East );
		ChartLegend->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
		ChartLegend->setShowLines(false);
		ChartLegend->setOrientation(Qt::Vertical);
		ChartLegend->setColor(0,Qt::darkBlue);
		ChartLegend->setColor(1,Qt::darkRed);
		Charts.last()->addLegend(ChartLegend);
		Charts.last()->addHeaderFooter(ChartTile);
		KDChart::GridAttributes gv( plane2->gridAttributes(Qt::Horizontal));
		gv.setGridVisible( false );
		plane2->setGridAttributes( Qt::Horizontal, gv );
		KDChart::CartesianCoordinatePlane* plane =qobject_cast <KDChart::CartesianCoordinatePlane*>(Charts.last()->coordinatePlane());
		KDChart::GridAttributes ga (plane->gridAttributes(Qt::Horizontal));
		ga.setGridVisible(false);
		plane->setGridAttributes(Qt::Horizontal,ga);
		plane->setGridAttributes(Qt::Vertical,ga);
		plane3->setGridAttributes(Qt::Horizontal,ga);
		plane3->setGridAttributes(Qt::Vertical,ga);
		ChartsArea->addWidget(Charts.last());
		ChartsList->addItem("Equity Return");
	}
	{//Reserve Funds
		QStringList DatesLabels;
		QStringList ReserveLabels;
		ReserveLabels << "Actual Reserve" << "Target Reserve";
		GenericCashFlow CurrentResFlows;
		for(int ReserveIter=0;ReserveIter<a.GetNumReserves();ReserveIter++){
			CurrentResFlows = a.GetReserveFund(ReserveIter)->GetReserveFundFlow();
			if(CurrentResFlows.IsEmpty()) continue;
			DatesLabels.clear();
			ChartsModels.append(new QStandardItemModel(CurrentResFlows.Count(),2,this));
			for(int i=0;i<CurrentResFlows.Count();i++){
				QModelIndex TempIndex=ChartsModels.last()->index(i,0);
				ChartsModels.last()->setData(TempIndex, (CurrentResFlows.GetFlow(i, static_cast<qint32>(ReserveFund::ReserveFlowsType::ReplenishFromInterest)) + CurrentResFlows.GetFlow(i, static_cast<qint32>(ReserveFund::ReserveFlowsType::ReplenishFromPrincipal))) / 1000000.0);
				ChartsModels.last()->setData(TempIndex, "Reserve Level - Date: " + CurrentResFlows.GetDate(i).toString("MMM-yy") + " - Value: " + Commarize((CurrentResFlows.GetFlow(i, static_cast<qint32>(ReserveFund::ReserveFlowsType::ReplenishFromInterest)) + CurrentResFlows.GetFlow(i, static_cast<qint32>(ReserveFund::ReserveFlowsType::ReplenishFromPrincipal)))), Qt::ToolTipRole);
				TempIndex=ChartsModels.last()->index(i,1);
				ChartsModels.last()->setData(TempIndex, CurrentResFlows.GetFlow(i, static_cast<qint32>(ReserveFund::ReserveFlowsType::ShortFall)) / 1000000.0);
				ChartsModels.last()->setData(TempIndex, "Reserve Target - Date: " + CurrentResFlows.GetDate(i).toString("MMM-yy") + " - Value: " + Commarize(CurrentResFlows.GetFlow(i, static_cast<qint32>(ReserveFund::ReserveFlowsType::ShortFall))), Qt::ToolTipRole);
				DatesLabels << CurrentResFlows.GetDate(i).toString("MMM-yy");
			}
			ChartsModels.last()->setHorizontalHeaderLabels(ReserveLabels);
			KDChart::LineDiagram* AnnualizedExcessLine=new KDChart::LineDiagram;
			AnnualizedExcessLine->setType(KDChart::LineDiagram::Stacked);
			AnnualizedExcessLine->setModel(ChartsModels.last());
			
			ChartsModels.append(new QStandardItemModel(a.GetTranche(0)->GetCashFlow().Count(),1,this));
			QDate TempCallDate=a.GetCalledPeriod();
			bool CallDatePlaced=false;
			for(int i=0;i<a.GetTranche(0)->GetCashFlow().Count();i++){
				QModelIndex TempIndex=ChartsModels.last()->index(i,0);
				if(a.GetTranche(0)->GetCashFlow().GetDate(i)>=TempCallDate && !CallDatePlaced){
					ChartsModels.last()->setData(TempIndex,1.0);
					ChartsModels.last()->setData(TempIndex,"Call Date",Qt::ToolTipRole);
					CallDatePlaced=true;
				}
				else ChartsModels.last()->setData(TempIndex,0.0);
			}
			QStringList TranchesLabels; TranchesLabels << "Call Date";
			ChartsModels.last()->setHorizontalHeaderLabels(TranchesLabels);
			KDChart::BarDiagram* CallDiagram=new KDChart::BarDiagram;
			CallDiagram->setModel(ChartsModels.last());
			QBrush TmpCallBrush=CallDiagram->brush(0);
			TmpCallBrush.setColor(Qt::lightGray);
			TmpCallBrush.setStyle(Qt::DiagCrossPattern);
			CallDiagram->setBrush(0,TmpCallBrush);

			for(int j=0;j<2;j++){
				KDChart::LineAttributes HideMissing=AnnualizedExcessLine->lineAttributes(j);
				HideMissing.setMissingValuesPolicy(KDChart::LineAttributes::MissingValuesHideSegments);
				HideMissing.setDisplayArea(true);
				AnnualizedExcessLine->setLineAttributes(j,HideMissing);
				QPen TempPen(AnnualizedExcessLine->pen(j));
				TempPen.setStyle(Qt::NoPen);
				AnnualizedExcessLine->setPen(j,TempPen);
			}
			QBrush ActualBrush(AnnualizedExcessLine->brush(0));
			ActualBrush.setColor(Qt::darkBlue);
			AnnualizedExcessLine->setBrush(0,ActualBrush);
			QBrush TargetBrush(AnnualizedExcessLine->brush(1));
			TargetBrush.setColor(Qt::darkRed);
			AnnualizedExcessLine->setBrush(1,TargetBrush);

			KDChart::HeaderFooter* ChartTile=new KDChart::HeaderFooter;
			ChartTile->setText(QString("Reserve %1").arg(ReserveIter+1));
			ChartTile->setPosition(KDChart::Position::North);

			KDChart::CartesianAxis* XAxis=new KDChart::CartesianAxis(CallDiagram);
			KDChart::TextAttributes RotatedText(XAxis->textAttributes());
			RotatedText.setRotation(-90);
			XAxis->setTextAttributes(RotatedText);
			XAxis->setLabels(DatesLabels);
			XAxis->setPosition(KDChart::CartesianAxis::Bottom);

			KDChart::CartesianAxis* YAxis=new KDChart::CartesianAxis(AnnualizedExcessLine);
			YAxis->setPosition(KDChart::CartesianAxis::Left);
			YAxis->setTitleText("Reserve Fund (Millions)");

			KDChart::CartesianAxis* YAxis2=new KDChart::CartesianAxis(CallDiagram);
			YAxis2->setPosition(KDChart::CartesianAxis::Right);
			KDChart::RulerAttributes tmpRlrAttr(YAxis2->rulerAttributes());
			tmpRlrAttr.setShowFirstTick(false);
			tmpRlrAttr.setShowMajorTickMarks(false);
			tmpRlrAttr.setShowMinorTickMarks(false);
			tmpRlrAttr.setShowRulerLine(false);
			YAxis2->setRulerAttributes(tmpRlrAttr);

			AnnualizedExcessLine->addAxis(YAxis);
			CallDiagram->addAxis(YAxis2);
			CallDiagram->addAxis(XAxis);
			Charts.append(new KDChart::Chart(this));
			KDChart::CartesianCoordinatePlane* plane2 = new KDChart::CartesianCoordinatePlane(Charts.last());
			Charts.last()->coordinatePlane()->replaceDiagram(AnnualizedExcessLine);
			plane2->setReferenceCoordinatePlane(Charts.last()->coordinatePlane());
			plane2->replaceDiagram(CallDiagram);
			Charts.last()->addCoordinatePlane(plane2);	
			KDChart::CartesianCoordinatePlane* plane =static_cast <KDChart::CartesianCoordinatePlane*>( AnnualizedExcessLine->coordinatePlane() );
			KDChart::GridAttributes ga (plane->gridAttributes(Qt::Horizontal));
			ga.setGridVisible(false);
			KDChart::GridAttributes gv (plane2->gridAttributes(Qt::Horizontal));
			gv.setGridVisible(false);
			plane->setGridAttributes(Qt::Horizontal,ga);
			plane2->setGridAttributes(Qt::Horizontal,gv);
			plane2->setGridAttributes(Qt::Vertical,gv);
			KDChart::Legend* ChartLegend=new KDChart::Legend(AnnualizedExcessLine,Charts.last());
			ChartLegend->addDiagram(CallDiagram);
			ChartLegend->setPosition( KDChart::Position::East );
			ChartLegend->setAlignment( Qt::AlignCenter );
			ChartLegend->setShowLines( false );
			ChartLegend->setOrientation( Qt::Vertical );
			ChartLegend->setBrush(a.GetTranchesCount(),TmpCallBrush);
			Charts.last()->addLegend(ChartLegend);
			Charts.last()->addHeaderFooter(ChartTile);
			ChartsArea->addWidget(Charts.last());
			ChartsList->addItem(QString("Reserve %1").arg(ReserveIter+1));
		}
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