#ifndef ChartsWidget_h__
#define ChartsWidget_h__
#include <QWidget>
#include <KDChart>
#include <QList>
class QStandardItemModel;
class QListWidget;
class QStackedWidget;
class Waterfall;
class ChartsWidget: public QWidget{
	Q_OBJECT
public:
	ChartsWidget(QWidget* parent=0);
	ChartsWidget(const Waterfall& a,QWidget* parent=0);
	void ResetCharts();
	void PlotStructure(const Waterfall& a);
	void SetMaxOCtoPlot(double a){if(a>0.0)MaxOCtoPlot=a;}
	void SetMaxICtoPlot(double a){if(a>0.0)MaxICtoPlot=a;}
	double GetMaxOCtoPlot() const{return MaxOCtoPlot;}
	double GetMaxICtoPlot() const{return MaxICtoPlot;}
	int GetChartLinesWeight() const{return ChartLinesWeight;}
	void SetChartLinesWeight(int a){if(a>0) ChartLinesWeight=a;}
	int ChartsCount() const;
private:
	void InitialSetUp();
	QListWidget* ChartsList;
	QStackedWidget* ChartsArea;
	QList<KDChart::Chart*> Charts;
	QList<QStandardItemModel*> ChartsModels;
	int ChartLinesWeight;
	double MaxOCtoPlot;
	double MaxICtoPlot;
};
#endif // ChartsWidget_h__