#include "ChartPlotter.h"
#include <QPainter>
#include <QImage>
ChartPlotter::ChartPlotter(QWidget* parent)
	:QWidget(parent)
	,ChartBackgroundColor(255,255,255)
	,AutomaticXLimits(true)
	,AutomaticMargins(true)
	,ShowAxes(true)
	,PrintXTick(true)
	,m_ColumnWidth(1.0) //Tempornaneo
{
	Margins[0]=10;
	Margins[1]=10;
	Margins[2]=10;
	Margins[3]=10;
	AutomaticYLimits[0]=true;
	AutomaticYLimits[1]=true;
	PrintYTick[0]=true;
	PrintYTick[1]=true;
	NumberFormat[0]="0.00";
	NumberFormat[1]="0.00";
	Multipliers[0]=1.0;
	Multipliers[1]=1.0;
	setMinimumSize(200,200);
	setWindowTitle("Chart");
}
bool ChartPlotter::GetPrintYTick(ChartSeries::AxisTarget AxisLim) const{
	if(AxisLim==ChartSeries::Secondary) return AutomaticYLimits[1];
	else return AutomaticYLimits[0];
}
void ChartPlotter::SetPrintYTick(bool a, ChartSeries::AxisTarget AxisLim){
	if(AxisLim==ChartSeries::Secondary) AutomaticYLimits[1]=a;
	else AutomaticYLimits[0]=a;
}
void ChartPlotter::SetTitle(const QString& a) {
	ChartTitle=a;
	setWindowTitle(ChartTitle);
}
void ChartPlotter::AddSeries(const ChartSeries& a){
	SeriesCollection.append(new ChartSeries(a));
	if(a.GetTypeOfSeries()==ChartSeries::AreaStackedSeries) AggregateAreaSeries(a.GetAxisTarget());
	else if(a.GetTypeOfSeries()==ChartSeries::ColumnStackedSeries)  AggregateColumnSeries(a.GetAxisTarget());
	
	if(AutomaticXLimits){
		m_MinimiumX=MinimiumX();
		m_MaximumX=MaximumX();
	}
	if(AutomaticYLimits[0]){
		m_MinimiumY[0]=MinimiumY();
		m_MaximumY[0]=MaximumY();
	}
	if(AutomaticYLimits[1]){
		m_MinimiumY[1]=MinimiumY(ChartSeries::Secondary);
		m_MaximumY[1]=MaximumY(ChartSeries::Secondary);
	}
}
ChartPlotter::~ChartPlotter(){
	for(QList<ChartSeries*>::iterator i=SeriesCollection.begin();i!=SeriesCollection.end();i++){
		delete *i;
	}
	for(QList<ChartSeries*>::iterator i=AggregatedAreaSeries[0].begin();i!=AggregatedAreaSeries[0].end();i++){
		delete *i;
	}
	for(QList<ChartSeries*>::iterator i=AggregatedAreaSeries[1].begin();i!=AggregatedAreaSeries[1].end();i++){
		delete *i;
	}
	for(QList<ChartSeries*>::iterator i=AggregatedColumnSeries[0].begin();i!=AggregatedColumnSeries[0].end();i++){
		delete *i;
	}
	for(QList<ChartSeries*>::iterator i=AggregatedColumnSeries[1].begin();i!=AggregatedColumnSeries[1].end();i++){
		delete *i;
	}
}
void ChartPlotter::AggregateAreaSeries(ChartSeries::AxisTarget AxisLim){
	switch(AxisLim){
	case ChartSeries::Invalid:
	case ChartSeries::Primary:
		for(QList<ChartSeries*>::iterator i=AggregatedAreaSeries[0].begin();i!=AggregatedAreaSeries[0].end();i++){
			delete *i;
		}
		AggregatedAreaSeries[0].clear();
		for(QList<ChartSeries*>::const_iterator i=SeriesCollection.begin();i!=SeriesCollection.end();i++){
			if((*i)->GetTypeOfSeries()==ChartSeries::AreaStackedSeries && (*i)->GetAxisTarget()==ChartSeries::Primary){
				AggregatedAreaSeries[0].append(new ChartSeries(**i));
				for(QList<ChartSeries*>::const_iterator j=i+1;j!=SeriesCollection.end();j++){
					if((*j)->GetTypeOfSeries()==ChartSeries::AreaStackedSeries && (*j)->GetAxisTarget()==ChartSeries::Primary){
						AggregatedAreaSeries[0].last()->operator+=(**j);
					}
				}
			}
		}
		if(AxisLim==ChartSeries::Primary) break;
	case ChartSeries::Secondary:
		for(QList<ChartSeries*>::iterator i=AggregatedAreaSeries[1].begin();i!=AggregatedAreaSeries[1].end();i++){
			delete *i;
		}
		AggregatedAreaSeries[1].clear();
		for(QList<ChartSeries*>::const_iterator i=SeriesCollection.begin();i!=SeriesCollection.end();i++){
			if((*i)->GetTypeOfSeries()==ChartSeries::AreaStackedSeries && (*i)->GetAxisTarget()==ChartSeries::Secondary){
				AggregatedAreaSeries[1].append(new ChartSeries(**i));
				for(QList<ChartSeries*>::const_iterator j=i+1;j!=SeriesCollection.end();j++){
					if((*j)->GetTypeOfSeries()==ChartSeries::AreaStackedSeries && (*j)->GetAxisTarget()==ChartSeries::Secondary){
						AggregatedAreaSeries[1].last()->operator+=(**j);
					}
				}
			}
		}
	}
}
void ChartPlotter::AggregateColumnSeries(ChartSeries::AxisTarget AxisLim){
	switch(AxisLim){
	case ChartSeries::Invalid:
	case ChartSeries::Primary:
		for(QList<ChartSeries*>::iterator i=AggregatedColumnSeries[0].begin();i!=AggregatedColumnSeries[0].end();i++){
			delete *i;
		}
		AggregatedColumnSeries[0].clear();
		for(QList<ChartSeries*>::const_iterator i=SeriesCollection.begin();i!=SeriesCollection.end();i++){
			if((*i)->GetTypeOfSeries()==ChartSeries::ColumnStackedSeries && (*i)->GetAxisTarget()==ChartSeries::Primary){
				AggregatedColumnSeries[0].append(new ChartSeries(**i));
				for(QList<ChartSeries*>::const_iterator j=i+1;j!=SeriesCollection.end();j++){
					if((*j)->GetTypeOfSeries()==ChartSeries::ColumnStackedSeries && (*j)->GetAxisTarget()==ChartSeries::Primary){
						AggregatedColumnSeries[0].last()->operator+=(**j);
					}
				}
			}
		}
		if(AxisLim==ChartSeries::Primary) break;
	case ChartSeries::Secondary:
		for(QList<ChartSeries*>::iterator i=AggregatedColumnSeries[1].begin();i!=AggregatedColumnSeries[1].end();i++){
			delete *i;
		}
		AggregatedColumnSeries[1].clear();
		for(QList<ChartSeries*>::const_iterator i=SeriesCollection.begin();i!=SeriesCollection.end();i++){
			if((*i)->GetTypeOfSeries()==ChartSeries::ColumnStackedSeries && (*i)->GetAxisTarget()==ChartSeries::Secondary){
				AggregatedColumnSeries[1].append(new ChartSeries(**i));
				for(QList<ChartSeries*>::const_iterator j=i+1;j!=SeriesCollection.end();j++){
					if((*j)->GetTypeOfSeries()==ChartSeries::ColumnStackedSeries && (*j)->GetAxisTarget()==ChartSeries::Secondary){
						AggregatedColumnSeries[1].last()->operator+=(**j);
					}
				}
			}
		}
	}
}
void ChartPlotter::SetXLimits(){
	AutomaticXLimits=true;
	m_MinimiumX=MinimiumX();
	m_MaximumX=MaximumX();
}
void ChartPlotter::SetYLimits(ChartSeries::AxisTarget AxisLim){
	switch (AxisLim){
	case ChartSeries::Invalid:
	case ChartSeries::Primary:
		AutomaticYLimits[0]=true;
		m_MinimiumY[0]=MinimiumY();
		m_MaximumY[0]=MaximumY();
		if(AxisLim==ChartSeries::Primary) break;
	case ChartSeries::Secondary:
		AutomaticYLimits[1]=true;
		m_MinimiumY[1]=MinimiumY(ChartSeries::Secondary);
		m_MaximumY[1]=MaximumY(ChartSeries::Secondary);
	}
}
void ChartPlotter::SetXLimits(QDate mn, QDate mx){
	AutomaticXLimits=false;
	m_MinimiumX=mn;
	m_MaximumX=mx;
}
void ChartPlotter::SetYLimits(double mn, double mx,ChartSeries::AxisTarget AxisLim){
	switch (AxisLim){
	case ChartSeries::Invalid:
	case ChartSeries::Primary:
		AutomaticYLimits[0]=false;
		m_MinimiumY[0]=mn;
		m_MaximumY[0]=mx;
		if(AxisLim==ChartSeries::Primary) break;
	case ChartSeries::Secondary:
		AutomaticYLimits[1]=false;
		m_MinimiumY[1]=mn;
		m_MaximumY[1]=mx;
	}
}
void ChartPlotter::SetMargins(int topM, int bottomM,int leftM, int rightM){
	Margins[0]=topM;
	Margins[1]=bottomM;
	Margins[2]=leftM;
	Margins[3]=rightM;
	AutomaticMargins=false;
}
void ChartPlotter::SetMargins(){
	AutomaticMargins=true;
}
const QString& ChartPlotter::GetYAxisTitle(ChartSeries::AxisTarget AxisLim)const{
	if(AxisLim==ChartSeries::Secondary) return YAxisTitle[1];
	else return YAxisTitle[0];
}
void ChartPlotter::SetYAxisTitle(const QString& a,ChartSeries::AxisTarget AxisLim){
	if(AxisLim==ChartSeries::Secondary) YAxisTitle[1]=a;
	else YAxisTitle[0]=a;
}
QDate ChartPlotter::MinimiumX() const{
	QDate Result(SeriesCollection.first()->MinimumXValue());
	for(QList<ChartSeries*>::const_iterator i=SeriesCollection.constBegin();i!=SeriesCollection.constEnd();i++){
		if((*i)->MinimumXValue()<Result) Result=(*i)->MinimumXValue();
	}
	return Result;
}
QDate ChartPlotter::MaximumX() const{
	QDate Result(SeriesCollection.first()->MaximumXValue());
	for(QList<ChartSeries*>::const_iterator i=SeriesCollection.constBegin();i!=SeriesCollection.constEnd();i++){
		if((*i)->MaximumXValue()>Result) Result=(*i)->MaximumXValue();
	}
	return Result;
}
double ChartPlotter::MinimiumY(ChartSeries::AxisTarget AxisLim) const{
	if(AxisLim!=ChartSeries::Primary && AxisLim!=ChartSeries::Secondary) return 0.0;
	bool FoundFirst=false;
	double Result=0.0;
	for(QList<ChartSeries*>::const_iterator i=SeriesCollection.constBegin();i!=SeriesCollection.constEnd();i++){
		if((*i)->GetAxisTarget()==AxisLim && ((*i)->MinimumValue()<Result || !FoundFirst)){
			Result=(*i)->MinimumValue();
			FoundFirst=true;
		}
	}
	for(QList<ChartSeries*>::const_iterator i=AggregatedAreaSeries[AxisLim].constBegin();i!=AggregatedAreaSeries[AxisLim].constEnd();i++){
		if((*i)->GetAxisTarget()==AxisLim && ((*i)->MinimumValue()<Result || !FoundFirst)){
			Result=(*i)->MinimumValue();
			FoundFirst=true;
		}
	}
	for(QList<ChartSeries*>::const_iterator i=AggregatedColumnSeries[AxisLim].constBegin();i!=AggregatedColumnSeries[AxisLim].constEnd();i++){
		if((*i)->GetAxisTarget()==AxisLim && ((*i)->MinimumValue()<Result || !FoundFirst)){
			Result=(*i)->MinimumValue();
			FoundFirst=true;
		}
	}
	return Result;
}
double ChartPlotter::MaximumY(ChartSeries::AxisTarget AxisLim) const{
	if(AxisLim!=ChartSeries::Primary && AxisLim!=ChartSeries::Secondary) return 0.0;
	bool FoundFirst=false;
	double Result=0.0;
	for(QList<ChartSeries*>::const_iterator i=SeriesCollection.constBegin();i!=SeriesCollection.constEnd();i++){
		if((*i)->GetAxisTarget()==AxisLim && ((*i)->MaximumValue()>Result || !FoundFirst)){ 
			Result=(*i)->MaximumValue();
			FoundFirst=true;
		}
	}
	for(QList<ChartSeries*>::const_iterator i=AggregatedAreaSeries[AxisLim].constBegin();i!=AggregatedAreaSeries[AxisLim].constEnd();i++){
		if((*i)->GetAxisTarget()==AxisLim && ((*i)->MaximumValue()>Result || !FoundFirst)){ 
			Result=(*i)->MaximumValue();
			FoundFirst=true;
		}
	}
	for(QList<ChartSeries*>::const_iterator i=AggregatedColumnSeries[AxisLim].constBegin();i!=AggregatedColumnSeries[AxisLim].constEnd();i++){
		if((*i)->GetAxisTarget()==AxisLim && ((*i)->MaximumValue()>Result || !FoundFirst)){ 
			Result=(*i)->MaximumValue();
			FoundFirst=true;
		}
	}
	return Result;
}
QString ChartPlotter::FormatNumber(double a, const QString& NumFormat, double Multiplier){
	a/=Multiplier;
	if(NumFormat.at(NumFormat.length()-1)=='%') a*=100.0;
	QStringList FormatParts=NumFormat.trimmed().toUpper().split('.');
	if(FormatParts.size()>2) return "";
	QString Result=QString::number(a,'f',0);
	while(Result.size()<FormatParts.first().size()) Result.prepend('0');
	if(FormatParts.size()>1){
		int Prec=FormatParts.last().size();
		if(NumFormat.at(NumFormat.length()-1)=='%') Prec--;
		Result+='.'+QString::number(a-static_cast<double>(static_cast<int>(a)),'f',Prec);
	}
	if(NumFormat.at(NumFormat.length()-1)=='%') Result+='%';
	return Result;
}
const QString& ChartPlotter::GetNumberFormat(ChartSeries::AxisTarget AxisLim)const{
	if(AxisLim==ChartSeries::Secondary) return NumberFormat[1];
	else return NumberFormat[0];
}
void ChartPlotter::SetNumberFormat(const QString& a,ChartSeries::AxisTarget AxisLim){
	if(AxisLim==ChartSeries::Secondary)  NumberFormat[1]=a;
	else  NumberFormat[0]=a;
}
double ChartPlotter::GetMultiplier(ChartSeries::AxisTarget AxisLim)const{
	if(AxisLim==ChartSeries::Secondary) return Multipliers[1];
	else return Multipliers[0];
}
void ChartPlotter::SetMultiplier(double a,ChartSeries::AxisTarget AxisLim){
	if(AxisLim==ChartSeries::Secondary)  Multipliers[1]=a;
	else  Multipliers[0]=a;
}

void ChartPlotter::paintEvent(QPaintEvent *event){
	QFont GeneralFont("Helvetica",12);
	QPainter MainPainter(this);
	QRect GraphRect(
		QPoint(Margins[2],Margins[0])
		,QPoint(rect().bottomRight().x()-Margins[3],rect().bottomRight().y()-Margins[1])
		);
	QImage PlotImage(rect().size(),QImage::Format_ARGB32_Premultiplied);
	QPainter PlotPainter(&PlotImage);
	PlotPainter.setRenderHint(QPainter::Antialiasing);
	//Paint Title
	if(!ChartTitle.isEmpty()){
		GeneralFont.setBold(true);
		GeneralFont.setPixelSize(MainTitleHeight);
		PlotPainter.setFont(GeneralFont);
		PlotPainter.drawText(
			QRect(GraphRect.topLeft().x(),GraphRect.topLeft().y(),GraphRect.width(),MainTitleHeight+WhiteSpace)
			,Qt::AlignCenter,ChartTitle);
		GraphRect=QRect(GraphRect.topLeft().x(),GraphRect.topLeft().y()+MainTitleHeight+(2*WhiteSpace),GraphRect.width(),GraphRect.height()-MainTitleHeight-(2*WhiteSpace));
		GeneralFont.setBold(false);
	}
	//Paint Axis Title
	if(!XAxisTitle.isEmpty()){
		GeneralFont.setPixelSize(AxisTitleHeight);
		PlotPainter.setFont(GeneralFont);
		PlotPainter.drawText(
			QRect(QPoint(GraphRect.bottomLeft().x(),GraphRect.bottomLeft().y()-AxisTitleHeight-WhiteSpace),QPoint(GraphRect.bottomRight().x(),GraphRect.bottomRight().y()))
		,Qt::AlignCenter,XAxisTitle);
		GraphRect=QRect(GraphRect.topLeft().x(),GraphRect.topLeft().y(),GraphRect.width(),GraphRect.height()-AxisTitleHeight-(2*WhiteSpace));
	}
	if(!YAxisTitle[0].isEmpty()){
		GeneralFont.setPixelSize(AxisTitleHeight);
		PlotPainter.setFont(GeneralFont);
		PlotPainter.translate(GraphRect.topLeft().x(),GraphRect.height()+GraphRect.topLeft().y());
		PlotPainter.rotate(-90);
		PlotPainter.drawText(
			QRect(0,0,GraphRect.height(),AxisTitleHeight+WhiteSpace)
			,Qt::AlignCenter,YAxisTitle[0]);
		PlotPainter.resetTransform();
		GraphRect=QRect(QPoint(GraphRect.topLeft().x()+AxisTitleHeight+(2*WhiteSpace),GraphRect.topLeft().y()),GraphRect.bottomRight());
	}
	if(!YAxisTitle[1].isEmpty()){
		GeneralFont.setPixelSize(AxisTitleHeight);
		PlotPainter.setFont(GeneralFont);
		PlotPainter.translate(GraphRect.topRight().x()-AxisTitleHeight-WhiteSpace,GraphRect.height()+GraphRect.topLeft().y());
		PlotPainter.rotate(-90);
		PlotPainter.drawText(
			QRect(0,0,GraphRect.height(),AxisTitleHeight+WhiteSpace)
			,Qt::AlignCenter,YAxisTitle[1]);
		GraphRect=QRect(GraphRect.topLeft(),QPoint(GraphRect.bottomRight().x()-AxisTitleHeight-(2*WhiteSpace),GraphRect.bottomRight().y()));
		PlotPainter.resetTransform();
	}
	//Print Axis
	PlotPainter.setPen(QPen(QBrush(Qt::black),AxisWidth,Qt::SolidLine));
	if(ShowAxes){
		PlotPainter.drawLine(GraphRect.topLeft(),GraphRect.bottomLeft());
		PlotPainter.drawLine(GraphRect.topLeft(),GraphRect.topRight());
		PlotPainter.drawLine(GraphRect.bottomRight(),GraphRect.bottomLeft());
		PlotPainter.drawLine(GraphRect.bottomRight(),GraphRect.topRight());
		GraphRect=QRect(
			QPoint(GraphRect.topLeft().x()+AxisWidth
			,GraphRect.topLeft().y()+AxisWidth)
			,QPoint(GraphRect.bottomRight().x()-AxisWidth
			,GraphRect.bottomRight().y()-AxisWidth)
			);
	}
	//Print Tick
	GeneralFont.setPixelSize(TickersHeight);
	PlotPainter.setFont(GeneralFont);
	if(PrintXTick){

	}
	

/************************************************************************/
	//Stacked Areas
	QPointF StartPoint, EndPoint;
	double RunningSum;
	bool FirstPointFound, lineDrawn;
	QVector<QPointF> PointsVector;
	for(int AxisIndex=0;AxisIndex<2;AxisIndex++){
		foreach(ChartSeries* SeriesToPlot, AggregatedAreaSeries[AxisIndex]){
			PlotPainter.setPen(QPen(QBrush(SeriesToPlot->GetBrush()),SeriesToPlot->GetWidth(),SeriesToPlot->GetStyle(),Qt::RoundCap,Qt::RoundJoin));
			PlotPainter.setBrush(SeriesToPlot->GetBrush());
			FirstPointFound=lineDrawn=false;
			PointsVector.clear();
			foreach (const ChartValue& SingleValue, SeriesToPlot->GetValue()){
				if(!FirstPointFound){
					if(!SingleValue.IsNull()){
						StartPoint=QPointF(
							static_cast<double>(GraphRect.topLeft().x())+
							(static_cast<double>(GraphRect.width())
							*static_cast<double>(m_MinimiumX.daysTo(SingleValue.GetXValue()))
							/static_cast<double>(m_MinimiumX.daysTo(m_MaximumX)))
							,
							static_cast<double>(GraphRect.topLeft().y())
							+static_cast<double>(GraphRect.height())
							-(static_cast<double>(GraphRect.height())
							*(SingleValue.GetYValue()-m_MinimiumY[SeriesToPlot->GetAxisTarget()])
							/(m_MaximumY[SeriesToPlot->GetAxisTarget()]-m_MinimiumY[SeriesToPlot->GetAxisTarget()]))
							);
						FirstPointFound=true;
						PointsVector.append(QPointF(StartPoint.x(),static_cast<double>(GraphRect.topLeft().y()+GraphRect.height())));
						lineDrawn=false;
					}
					continue;
				}
				if(!SingleValue.IsNull()){
					EndPoint=QPointF(
						static_cast<double>(GraphRect.topLeft().x())+
						static_cast<double>(GraphRect.width())
						*static_cast<double>(m_MinimiumX.daysTo(SingleValue.GetXValue()))
						/static_cast<double>(m_MinimiumX.daysTo(m_MaximumX))
						,
						static_cast<double>(GraphRect.topLeft().y())+
						static_cast<double>(GraphRect.height())
						-(static_cast<double>(GraphRect.height())
						*(SingleValue.GetYValue()-m_MinimiumY[SeriesToPlot->GetAxisTarget()])
						/(m_MaximumY[SeriesToPlot->GetAxisTarget()]-m_MinimiumY[SeriesToPlot->GetAxisTarget()]))
						);
					//PlotPainter.drawLine(StartPoint,EndPoint);

					PointsVector.append(StartPoint);
					PointsVector.append(EndPoint);

					StartPoint=EndPoint;
					lineDrawn=true;
				}
				else{
					if(lineDrawn) PointsVector.append(QPointF(EndPoint.x(),static_cast<double>(GraphRect.topLeft().y()+GraphRect.height())));
					PlotPainter.drawPolygon(QPolygonF(PointsVector));
					PointsVector.clear();
					if(!lineDrawn) PlotPainter.drawLine(StartPoint,QPointF(StartPoint.x(),static_cast<double>(GraphRect.topLeft().y()+GraphRect.height())));
					FirstPointFound=false;
				}
			}
			if(!lineDrawn) PlotPainter.drawLine(StartPoint,QPointF(StartPoint.x(),static_cast<double>(GraphRect.topLeft().y()+GraphRect.height())));
			else {
				PointsVector.append(QPointF(EndPoint.x(),static_cast<double>(GraphRect.topLeft().y()+GraphRect.height())));
				PlotPainter.drawPolygon(QPolygonF(PointsVector));
			}

		}
	}
/************************************************************************/
//Stacked Columns
	for(int AxisIndex=0;AxisIndex<2;AxisIndex++){
		foreach(ChartSeries* SeriesToPlot, AggregatedColumnSeries[AxisIndex]){
			PlotPainter.setPen(QPen(QBrush(SeriesToPlot->GetBrush()),SeriesToPlot->GetWidth(),SeriesToPlot->GetStyle(),Qt::RoundCap,Qt::RoundJoin));
			PlotPainter.setBrush(SeriesToPlot->GetBrush());
			foreach (const ChartValue& SingleValue, SeriesToPlot->GetValue()){
				if(!SingleValue.IsNull()){
					EndPoint=QPointF(
						(static_cast<double>(GraphRect.topLeft().x())+
						static_cast<double>(GraphRect.width())
						*static_cast<double>(m_MinimiumX.daysTo(SingleValue.GetXValue()))
						/static_cast<double>(m_MinimiumX.daysTo(m_MaximumX)))
						-(m_ColumnWidth/2.0)
						,
						static_cast<double>(GraphRect.topLeft().y())+
						static_cast<double>(GraphRect.height())
						-(static_cast<double>(GraphRect.height())
						*(SingleValue.GetYValue()-m_MinimiumY[SeriesToPlot->GetAxisTarget()])
						/(m_MaximumY[SeriesToPlot->GetAxisTarget()]-m_MinimiumY[SeriesToPlot->GetAxisTarget()]))
						);
					StartPoint.setX(EndPoint.x()+m_ColumnWidth);
					StartPoint.setY(static_cast<double>(GraphRect.bottomRight().y()));
					if(EndPoint.x()<GraphRect.topLeft().x()) EndPoint.setX(GraphRect.topLeft().x());
					if(EndPoint.x()>GraphRect.bottomRight().x()) EndPoint.setX(GraphRect.bottomRight().x());
					if(StartPoint.x()<GraphRect.topLeft().x()) StartPoint.setX(GraphRect.topLeft().x());
					if(StartPoint.x()>GraphRect.bottomRight().x()) StartPoint.setX(GraphRect.bottomRight().x());
					PlotPainter.drawRect(QRectF(EndPoint,StartPoint));
				}
			}
		}
	}
/************************************************************************/
//Lines
	foreach(ChartSeries* SeriesToPlot, SeriesCollection){
		PlotPainter.setPen(QPen(QBrush(SeriesToPlot->GetBrush()),SeriesToPlot->GetWidth(),SeriesToPlot->GetStyle(),Qt::RoundCap,Qt::RoundJoin));
		FirstPointFound=lineDrawn=false;
		switch(SeriesToPlot->GetTypeOfSeries()){
		case ChartSeries::LineSeries:
				foreach (const ChartValue& SingleValue, SeriesToPlot->GetValue()){
					if(!FirstPointFound){
						if(!SingleValue.IsNull()){
							StartPoint=QPointF(
								static_cast<double>(GraphRect.topLeft().x())+
								static_cast<double>(GraphRect.width())
								*static_cast<double>(m_MinimiumX.daysTo(SingleValue.GetXValue()))
								/static_cast<double>(m_MinimiumX.daysTo(m_MaximumX))
								,
								static_cast<double>(GraphRect.topLeft().y())+
								static_cast<double>(GraphRect.height())
								-(static_cast<double>(GraphRect.height())
								*(SingleValue.GetYValue()-m_MinimiumY[SeriesToPlot->GetAxisTarget()])
								/(m_MaximumY[SeriesToPlot->GetAxisTarget()]-m_MinimiumY[SeriesToPlot->GetAxisTarget()]))
							);
							FirstPointFound=true;
							lineDrawn=false;
						}
						continue;
					}
					if(!SingleValue.IsNull()){
						EndPoint=QPointF(
							static_cast<double>(GraphRect.topLeft().x())+
							static_cast<double>(GraphRect.width())
							*static_cast<double>(m_MinimiumX.daysTo(SingleValue.GetXValue()))
							/static_cast<double>(m_MinimiumX.daysTo(m_MaximumX))
							,
							static_cast<double>(GraphRect.topLeft().y())+
							static_cast<double>(GraphRect.height())
							-(static_cast<double>(GraphRect.height())
							*(SingleValue.GetYValue()-m_MinimiumY[SeriesToPlot->GetAxisTarget()])
							/(m_MaximumY[SeriesToPlot->GetAxisTarget()]-m_MinimiumY[SeriesToPlot->GetAxisTarget()]))
						);
						PlotPainter.drawLine(StartPoint,EndPoint);
						StartPoint=EndPoint;
						lineDrawn=true;
					}
					else{
						if(!lineDrawn) PlotPainter.drawPoint(StartPoint);
						FirstPointFound=false;
					}
				}
				if(!lineDrawn) PlotPainter.drawPoint(StartPoint);
		break; //End Line Plot
		}
	}


	
/************************************************************************/
	
	MainPainter.fillRect(rect(),ChartBackgroundColor);
	MainPainter.drawImage(rect(),PlotImage);
}
void ChartPlotter::ClearSeries(){
	for(QList<ChartSeries*>::iterator i=SeriesCollection.begin();i!=SeriesCollection.end();i++){
		delete (*i);
	}
	SeriesCollection.clear();
	for(QList<ChartSeries*>::iterator i=AggregatedAreaSeries[1].begin();i!=AggregatedAreaSeries[1].end();i++){
		delete (*i);
	}
	AggregatedAreaSeries[1].clear();
	for(QList<ChartSeries*>::iterator i=AggregatedAreaSeries[0].begin();i!=AggregatedAreaSeries[0].end();i++){
		delete (*i);
	}
	AggregatedAreaSeries[0].clear();
	for(QList<ChartSeries*>::iterator i=AggregatedColumnSeries[1].begin();i!=AggregatedColumnSeries[1].end();i++){
		delete (*i);
	}
	AggregatedColumnSeries[1].clear();
	for(QList<ChartSeries*>::iterator i=AggregatedColumnSeries[0].begin();i!=AggregatedColumnSeries[0].end();i++){
		delete (*i);
	}
	AggregatedColumnSeries[0].clear();
}