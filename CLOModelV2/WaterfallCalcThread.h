#ifndef WaterfallCalcThread_h__
#define WaterfallCalcThread_h__

#include <QThread>
#include "Waterfall.h"
class WaterfallCalcThread : public QThread{
	Q_OBJECT
public:
	WaterfallCalcThread(int ID,QObject* parent=0);
	void SetWaterfall(const Waterfall& a){LocalFall=a;}

private:
	Waterfall LocalFall;
	int Identifier;
signals:
	void Calculated(int,const Waterfall&);
public slots:
	void run();
	void stop(){exit(0);}
};
#endif // WaterfallCalcThread_h__