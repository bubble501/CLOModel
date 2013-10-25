#ifndef ChartSeries_h__
#define ChartSeries_h__

#include "ChartValue.h"
#include <QList>
#include <QString>
#include <QBrush>
class ChartSeries{
public:
	enum AxisTarget{
		Primary=0,
		Secondary=1,
		Invalid
	};
	enum SeriesType{
		AreaStackedSeries,
		ColumnStackedSeries,
		LineSeries
	};
	ChartSeries(SeriesType b=LineSeries);
	ChartSeries(const QString& a,SeriesType b=LineSeries);
	ChartSeries(const ChartSeries& a);
	ChartSeries& operator=(const ChartSeries& a);
	ChartSeries& operator+=(const ChartSeries& a);
	ChartSeries& operator+=(const ChartValue& a);
	void SetName(const QString& a){SeriesName=a;}
	const QString& GetName() const{return SeriesName;}
	int Count() const {return ValuesSeries.size();}
	void AddValue(const ChartValue& a);
	void AddValue(const QDate& a, double b){AddValue(ChartValue(a,b));}
	void AddValue(const QDate& a){AddValue(ChartValue(a));}
	void SetValue(const QDate& a, double b);
	void SetValue(const QDate& a);
	void SetValue(const ChartValue& a);
	const QList<ChartValue>& GetValue()const {return ValuesSeries;}
	ChartValue GetValue(int a)const;
	ChartValue GetValue(const QDate&  a)const;
	double MaximumValue() const;
	double MinimumValue() const;
	QDate MaximumXValue() const;
	QDate MinimumXValue() const;
	QColor GetColor() const {return SeriesBrush.color();}
	const QBrush& GetBrush() const {return SeriesBrush;}
	void SetColor(const QColor& a);
	void SetColor(unsigned int ColIndex);
	void SetBrush(const QBrush& a){SeriesBrush=a;}
	int GetWidth() const {return SeriesWidth;}
	void SetWidth(int a){if (a>0) SeriesWidth=a;}
	Qt::PenStyle GetStyle() const{return SeriesStyle;}
	void SetStyle(Qt::PenStyle a){SeriesStyle=a;}
	AxisTarget GetAxisTarget() const {return PlotOnAxis;}
	void SetAxisTarget(AxisTarget a){PlotOnAxis=a;}
	SeriesType GetTypeOfSeries() const {return TypeOfSeries;}
	void SetTypeOfSeries(SeriesType a=LineSeries){TypeOfSeries=a;}
	void Clear();
private:
	QList<ChartValue> ValuesSeries;
	QString SeriesName;
	SeriesType TypeOfSeries;
	QBrush SeriesBrush;
	Qt::PenStyle SeriesStyle;
	int SeriesWidth;
	AxisTarget PlotOnAxis;
};
#endif // ChartSeries_h__