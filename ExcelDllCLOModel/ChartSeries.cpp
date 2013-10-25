#include "ChartSeries.h"
#include <QtAlgorithms>
ChartSeries::ChartSeries(const QString& a,SeriesType b)
	:SeriesName(a)
	,TypeOfSeries(b)
	,SeriesStyle(Qt::SolidLine)
	,SeriesWidth(5)
	,SeriesBrush(QColor(0,0,0))
	,PlotOnAxis(Primary)
{}
ChartSeries::ChartSeries(SeriesType b)
	:TypeOfSeries(b)
	,SeriesStyle(Qt::SolidLine)
	,SeriesWidth(5)
	,SeriesBrush(QColor(0,0,0))
	,PlotOnAxis(Primary)
{}
ChartSeries::ChartSeries(const ChartSeries& a)
	:SeriesName(a.SeriesName)
	,ValuesSeries(a.ValuesSeries)
	,TypeOfSeries(a.TypeOfSeries)
	,SeriesBrush(a.SeriesBrush)
	,SeriesStyle(a.SeriesStyle)
	,SeriesWidth(a.SeriesWidth)
	,PlotOnAxis(a.PlotOnAxis)
{}
ChartSeries& ChartSeries::operator=(const ChartSeries& a){
	ValuesSeries=a.ValuesSeries;
	SeriesName=a.SeriesName;
	TypeOfSeries=a.TypeOfSeries;
	SeriesBrush=a.SeriesBrush;
	SeriesStyle=a.SeriesStyle;
	SeriesWidth=a.SeriesWidth;
	PlotOnAxis=a.PlotOnAxis;
	return *this;
}
void ChartSeries::Clear(){
	ValuesSeries.clear();
}
double ChartSeries::MaximumValue() const{
	double Result=ValuesSeries.first().GetYValue();
	for(QList<ChartValue>::const_iterator i=ValuesSeries.constBegin()+1;i!=ValuesSeries.constEnd();i++){
		if(i->GetYValue()>Result && !i->IsNull()) Result=i->GetYValue();
	}
	return Result;
}
double ChartSeries::MinimumValue() const{
	double Result=ValuesSeries.first().GetYValue();
	for(QList<ChartValue>::const_iterator i=ValuesSeries.constBegin()+1;i!=ValuesSeries.constEnd();i++){
		if(i->GetYValue()<Result && !i->IsNull()) Result=i->GetYValue();
	}
	return Result;
}
QDate ChartSeries::MaximumXValue() const{
	QDate Result=ValuesSeries.first().GetXValue();
	for(QList<ChartValue>::const_iterator i=ValuesSeries.constBegin()+1;i!=ValuesSeries.constEnd();i++){
		if(i->GetXValue()>Result) Result=i->GetXValue();
	}
	return Result;
}
QDate ChartSeries::MinimumXValue() const{
	QDate Result=ValuesSeries.first().GetXValue();
	for(QList<ChartValue>::const_iterator i=ValuesSeries.constBegin()+1;i!=ValuesSeries.constEnd();i++){
		if(i->GetXValue()<Result) Result=i->GetXValue();
	}
	return Result;
}
void ChartSeries::AddValue(const ChartValue& a){ValuesSeries.append(a); qSort(ValuesSeries);}
void ChartSeries::SetValue(const QDate& a, double b){
	for(QList<ChartValue>::iterator i=ValuesSeries.begin();i!=ValuesSeries.end();i++){
		if(i->GetXValue()==a){
			i->SetYValue(b);
			return;
		}
	}
	return AddValue(a,b);
}
void ChartSeries::SetValue(const QDate& a){
	for(QList<ChartValue>::iterator i=ValuesSeries.begin();i!=ValuesSeries.end();i++){
		if(i->GetXValue()==a){
			i->SetNullValue();
			return;
		}
	}
	return AddValue(a);
}
void ChartSeries::SetValue(const ChartValue& a){
	for(QList<ChartValue>::iterator i=ValuesSeries.begin();i!=ValuesSeries.end();i++){
		if(i->GetXValue()==a.GetXValue()){
			i->SetYValue(a.GetYValue());
			return;
		}
	}
	return AddValue(a);
}
ChartValue ChartSeries::GetValue(int a)const{
	if(a>=0 && a<ValuesSeries.size()) return ValuesSeries.at(a);
	else return ChartValue();
}
ChartValue ChartSeries::GetValue(const QDate& a)const{
	for(QList<ChartValue>::const_iterator i=ValuesSeries.constBegin();i!=ValuesSeries.constEnd();i++){
		if(i->GetXValue()==a) return *i;
	}
	return ChartValue();
}
void ChartSeries::SetColor(const QColor& a){
	SeriesBrush.setColor(a);
}
ChartSeries& ChartSeries::operator+=(const ChartSeries& a){
	for(QList<ChartValue>::const_iterator i=a.ValuesSeries.constBegin();i!=a.ValuesSeries.constEnd();i++){
		operator+=(*i);
	}
	return *this;
}
ChartSeries& ChartSeries::operator+=(const ChartValue& a){
	for(QList<ChartValue>::iterator i=ValuesSeries.begin();i!=ValuesSeries.end();i++){
		if(i->GetXValue()==a.GetXValue() && !(i->IsNull() && a.IsNull())){
			i->SetYValue(i->GetYValue()+a.GetYValue());
			return *this;
		}
	}
	AddValue(a);
	return *this;
}
void ChartSeries::SetColor(unsigned int ColIndex){
	//TODO
	switch (ColIndex){
	case 0:
		SeriesBrush.setColor(Qt::red); break;
	case 1:
		SeriesBrush.setColor(Qt::green);break;
	case 2:
		SeriesBrush.setColor(Qt::blue);break;
	case 3:
		SeriesBrush.setColor(Qt::magenta);break;
	default:
		SeriesBrush.setColor(Qt::gray);break;
	}
}