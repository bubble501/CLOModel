#ifndef WaterfallCalcThread_h__
#define WaterfallCalcThread_h__
#include "TemplAsyncThread.h"
#include "Waterfall.h"
class WaterfallCalcThreadPrivate;
class CLOMODELLIB_EXPORT WaterfallCalcThread : public TemplAsyncThread<Waterfall>
{
	Q_OBJECT
protected:
    Q_DECLARE_PRIVATE(WaterfallCalcThread)
    WaterfallCalcThread(WaterfallCalcThreadPrivate* d, int ID, QObject* parent = nullptr);
public:
	WaterfallCalcThread(int ID,QObject* parent=nullptr);
    virtual ~WaterfallCalcThread();
	void SetWaterfall(const Waterfall& a);
signals:
	void Calculated(int,const Waterfall&);
protected slots:
	virtual void EmitFromAnonim()override;
public slots :
	void run() override;
};
#endif // WaterfallCalcThread_h__