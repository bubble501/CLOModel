#ifndef WaterfallCalcThread_h__
#define WaterfallCalcThread_h__

#include <TemplAsyncThread.h>
#include "Waterfall.h"
class WaterfallCalcThread : public TemplAsyncThread<Waterfall> {
	Q_OBJECT
public:
	WaterfallCalcThread(int ID,QObject* parent=nullptr);
	void SetWaterfall(const Waterfall& a){LocalFall=a;}
protected:
	Waterfall LocalFall;
signals:
	void Calculated(int,const Waterfall&);
protected slots:
	virtual void EmitFromAnonim()override {
		emit Calculated(Identifier, LocalFall);
	}
public slots :
	void run() override;
};
#endif // WaterfallCalcThread_h__