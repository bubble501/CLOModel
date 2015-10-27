#ifndef WaterfallCalculator_h__
#define WaterfallCalculator_h__
#include "TemplAsyncCalculator.h"
#include "WaterfallCalcThread.h"
class WaterfallCalculatorPrivate;
class CLOMODELLIB_EXPORT WaterfallCalculator : public TemplAsyncCalculator <WaterfallCalcThread, Waterfall>
{
	Q_OBJECT
protected:
    inline WaterfallCalculatorPrivate* d_func() { return reinterpret_cast<WaterfallCalculatorPrivate *>(qGetPtrHelper(BackwardInterface::d_ptr)); }
    inline const WaterfallCalculatorPrivate* d_func() const { return reinterpret_cast<const WaterfallCalculatorPrivate *>(qGetPtrHelper(BackwardInterface::d_ptr)); }
    friend class WaterfallCalculatorPrivate;
    WaterfallCalculator(WaterfallCalculatorPrivate *d, QObject* parent = nullptr);
    virtual void BeeReturned(int Ident, const Waterfall& a) override;
    virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
public:
	WaterfallCalculator(QObject* parent=0);
	virtual ~WaterfallCalculator();
	void AddWaterfall(const Waterfall& a, qint32 ID);
	virtual QString ReadyToCalculate() const override;
	virtual int NumBees() const override;
	virtual void Reset() override;
	virtual void ClearWaterfalls();
public slots:
    virtual std::tuple<bool, QString> StartCalculation() override;
    virtual std::tuple<bool, QString> StartCalculation(bool ignoreCheck);
    friend CLOMODELLIB_EXPORT QDataStream& operator<<(QDataStream & stream, const WaterfallCalculator& flows);
    friend CLOMODELLIB_EXPORT QDataStream& operator>>(QDataStream & stream, WaterfallCalculator& flows);
};
CLOMODELLIB_EXPORT QDataStream& operator<<(QDataStream & stream, const WaterfallCalculator& flows);
CLOMODELLIB_EXPORT QDataStream& operator>>(QDataStream & stream, WaterfallCalculator& flows);
#endif // WaterfallCalculator_h__