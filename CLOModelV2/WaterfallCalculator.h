#ifndef WaterfallCalculator_h__
#define WaterfallCalculator_h__
#include <QObject>
#include <QHash>
#include "Waterfall.h"
class WaterfallCalcThread;
class WaterfallCalculator : public QObject{
	Q_OBJECT
public:
	WaterfallCalculator(QObject* parent=0);
	~WaterfallCalculator();
	void AddWaterfall(const Waterfall& a);
	void ResetWaterfalls();
	const QList<Waterfall*>& GetWaterfalls() const{return Cascades;}
	void StartCalculation();
	void SetSequentialComputation(bool a){SequentialComputation=a;}
	bool GetSequentialComputation()const {return SequentialComputation;}
private:
	QHash<int, WaterfallCalcThread*> ThreadPool;
	QList<Waterfall*> Cascades;
	int BeesReturned;
	int BeesSent;
	bool SequentialComputation;
private slots:
	void BeeReturned(int ID,const Waterfall& a);
signals:
	void Calculated();
};
#endif // WaterfallCalculator_h__