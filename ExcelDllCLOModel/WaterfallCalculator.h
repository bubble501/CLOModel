#ifndef WaterfallCalculator_h__
#define WaterfallCalculator_h__

#include <QObject>
#include "Waterfall.h"
class WaterfallCalculator : public QObject{
	Q_OBJECT
public:
	WaterfallCalculator(QObject* parent=0);
	~WaterfallCalculator(){ResetWaterfalls();}
	void AddWaterfall(const Waterfall& a);
	void ResetWaterfalls();
	const QList<Waterfall*>& GetWaterfalls() const{return Cascades;}
	void StartCalculation();
private:
	QList<Waterfall*> Cascades;
	int BeesReturned;
	int BeesSent;
private slots:
	void BeeReturned(int ID,const Waterfall& a);
signals:
	void Calculated();
};
#endif // WaterfallCalculator_h__