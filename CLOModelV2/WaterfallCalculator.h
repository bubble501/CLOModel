#ifndef WaterfallCalculator_h__
#define WaterfallCalculator_h__
#include "TemplAsyncCalculator.h"
#include <QHash>
#include <QSet>
#include "Waterfall.h"
#include "WaterfallCalcThread.h"
class WaterfallCalculator : public TemplAsyncCalculator <WaterfallCalcThread, Waterfall>{
	Q_OBJECT
public:
	WaterfallCalculator(QObject* parent=0);
	virtual ~WaterfallCalculator() { Reset(); }
	void AddWaterfall(const Waterfall& a, qint32 ID);
	virtual QString ReadyToCalculate() const override;
	virtual int NumBees() const override { return m_Cascades.size(); }
	virtual void Reset() override;
	virtual void ClearWaterfalls();
public slots:
	virtual bool StartCalculation() override;
protected:
	QHash<qint32, Waterfall*> m_Cascades;
	virtual void BeeReturned(int Ident, const Waterfall& a) override;
};
#endif // WaterfallCalculator_h__