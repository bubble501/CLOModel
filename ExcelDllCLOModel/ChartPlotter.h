#ifndef ChartPlotter_h__
#define ChartPlotter_h__

#include <QWidget>
#include <QList>
#include <QDate>
#include "ChartSeries.h"
class ChartPlotter : public QWidget{
	Q_OBJECT
private:
	QList<ChartSeries*> SeriesCollection;
	QColor ChartBackgroundColor;
	int Margins[4];
	QDate m_MinimiumX;
	QDate m_MaximumX;
	double m_MinimiumY[2];
	double m_MaximumY[2];
	double m_ColumnWidth;
	bool AutomaticXLimits;
	bool AutomaticYLimits[2];
	bool AutomaticMargins;
	bool ShowAxes;
	bool PrintXTick;
	bool PrintYTick[2];
	QString ChartTitle;
	QString YAxisTitle[2];
	QString XAxisTitle;
	QString NumberFormat[2];
	double Multipliers[2];
	QList<ChartSeries*> AggregatedAreaSeries[2];
	QList<ChartSeries*> AggregatedColumnSeries[2];
	void AggregateAreaSeries(ChartSeries::AxisTarget AxisLim=ChartSeries::Invalid);
	void AggregateColumnSeries(ChartSeries::AxisTarget AxisLim=ChartSeries::Invalid);
	enum{MainTitleHeight=16};
	enum{AxisTitleHeight=14};
	enum{TickersHeight=12};
	enum{WhiteSpace=5};
	enum{AxisWidth=2};
	QString FormatNumber(double a, const QString& NumFormat="0", double Multiplier=1.0);
public:
	const QString& GetNumberFormat(ChartSeries::AxisTarget AxisLim=ChartSeries::Primary)const;
	void SetNumberFormat(const QString& a,ChartSeries::AxisTarget AxisLim=ChartSeries::Primary);
	double GetMultiplier(ChartSeries::AxisTarget AxisLim=ChartSeries::Primary)const;
	void SetMultiplier(double a,ChartSeries::AxisTarget AxisLim=ChartSeries::Primary);
	bool GetPrintXTick() const {return PrintXTick;}
	void SetPrintXTick(bool a){PrintXTick=a;}
	bool GetPrintYTick(ChartSeries::AxisTarget AxisLim=ChartSeries::Primary) const;
	void SetPrintYTick(bool a, ChartSeries::AxisTarget AxisLim=ChartSeries::Primary);
	bool GetShowAxes() const {return ShowAxes;}
	void SetShowAxes(bool a){ShowAxes=a;}
	const QString& GetYAxisTitle(ChartSeries::AxisTarget AxisLim=ChartSeries::Primary)const;
	void SetYAxisTitle(const QString& a,ChartSeries::AxisTarget AxisLim=ChartSeries::Primary);
	void SetXAxisTitle(const QString& a) {XAxisTitle=a;}
	const QString& GetXAxisTitle() const{return XAxisTitle;}
	const QString& GetTitle() const{return ChartTitle;}
	void SetTitle(const QString& a);
	QDate MinimiumX() const;
	QDate MaximumX() const;
	double MinimiumY(ChartSeries::AxisTarget AxisLim=ChartSeries::Primary) const;
	double MaximumY(ChartSeries::AxisTarget AxisLim=ChartSeries::Primary) const;
	ChartPlotter(QWidget* parent=0);
	~ChartPlotter();
	void SetLimits(QDate mn, QDate mx,double vmn, double vmx,ChartSeries::AxisTarget AxisLim=ChartSeries::Primary){SetXLimits(mn,mx); SetYLimits(vmn,vmx,AxisLim);}
	void SetLimits(ChartSeries::AxisTarget AxisLim=ChartSeries::Invalid){SetXLimits(); SetYLimits(AxisLim);}
	void SetXLimits();
	void SetYLimits(ChartSeries::AxisTarget AxisLim=ChartSeries::Invalid);
	void SetXLimits(QDate mn, QDate mx);
	void SetYLimits(double mn, double mx,ChartSeries::AxisTarget AxisLim=ChartSeries::Primary);
	int SeriesCount() const {return SeriesCollection.size();}
	void SetMargins();
	void SetMargins(int topM, int bottomM,int leftM, int rightM);
	void AddSeries(const ChartSeries& a);
	void ClearSeries();
protected:
	void paintEvent(QPaintEvent *event);
};
#endif // ChartPlotter_h__