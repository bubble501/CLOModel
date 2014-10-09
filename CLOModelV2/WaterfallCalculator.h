#ifndef WaterfallCalculator_h__
#define WaterfallCalculator_h__
#include <QObject>
#include <QHash>
#include <QSet>
#include "Waterfall.h"
class WaterfallCalcThread;
class WaterfallCalculator : public QObject{
	Q_OBJECT
public:
	WaterfallCalculator(QObject* parent=0);
	~WaterfallCalculator();
	void AddWaterfall(const Waterfall& a, qint32 ID);
	void AddWaterfall(const Waterfall& a) { AddWaterfall(a, Cascades.size()); }
	void ResetWaterfalls();
	const QHash<qint32, Waterfall*>& GetWaterfalls() const { return Cascades; }
	const Waterfall* GetWaterfall(qint32 ID) { return Cascades.value(ID, nullptr); }
	void SetSequentialComputation(bool a){SequentialComputation=a;}
	bool GetSequentialComputation()const {return SequentialComputation;}
private:
	QHash<qint32, WaterfallCalcThread*> ThreadPool;
	QHash<qint32, Waterfall*> Cascades;
	QSet<qint32> SentIDs;
	quint32 BeesReturned;
	bool SequentialComputation;
	bool KeepRunning;
private slots:
	void BeeReturned(int ID,const Waterfall& a);
	void HandleErrorCalc(int ID);
public slots:
	void Stop();
	void StartCalculation();
signals:
	void Calculated();
	void ErrorInCalculation(int);
	void CurrentProgress(double);
};
#endif // WaterfallCalculator_h__