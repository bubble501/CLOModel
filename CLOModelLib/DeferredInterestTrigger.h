#ifndef DeferredInterestTrigger_h__
#define DeferredInterestTrigger_h__
#include "AbstractTrigger.h"
class Tranche;
class IntegerVector;
class DeferredInterestTriggerPrivate;
class DeferredInterestTriggerSettingWidget;
class QWidget;
class CLOMODELLIB_EXPORT DeferredInterestTrigger : public AbstractTrigger
{
    DECLARE_PUBLIC_COMMONS(DeferredInterestTrigger)
    DECLARE_PUBLIC_COMMONS_COPY(DeferredInterestTrigger)
public:
	enum class TriggerSizeSide : quint8 {
		Invalid = 0
		, Bigger = 0x1
		, Smaller = 0x2
		, Exactly = 0x4
		, BiggerOrEqual = Bigger | Exactly
		, SmallerOrEqual = Smaller | Exactly
	};
	enum class TriggerSenioritySide : quint8 {
		Invalid = 0
		, Senior = 0x1
		, Junior = 0x2
		, Exactly = 0x4
		, SeniorOrEqual = Senior | Exactly
		, JuniorOrEqual = Junior | Exactly
	};
	enum class TriggerCouponSide : quint8 {
		Invalid = 0
		, Bigger = 0x1
		, Smaller = 0x2
		, Exactly = 0x4
		, BiggerOrEqual = Bigger | Exactly
		, SmallerOrEqual = Smaller | Exactly
	};

	QString TriggerSizeSideToString(TriggerSizeSide a) const;
	QString TriggerSenioritySideToString(TriggerSenioritySide a) const;
	QString TriggerCouponSideToString(TriggerCouponSide a) const;
	DeferredInterestTrigger(
		QString TargetSenior
		, QString TargetSeniorLevel
		, QString TargetSize = "0"
		, QString TargetCoupon = "0"
		, TriggerSizeSide szd = TriggerSizeSide::Invalid
		, TriggerSenioritySide ssd = TriggerSenioritySide::Invalid
		, TriggerCouponSide scd = TriggerCouponSide::Invalid
		, const QString& lab = QString()
	);
	DeferredInterestTrigger(const QString& lab);
	virtual bool Passing(const QList<Tranche*>& currentTranches, const QDate& CurrentPeriod) const;
	virtual bool Passing(const QList<Tranche*>& currentTranches, int CurrentPeriod) const;
	virtual bool Failing(const QList<Tranche*>& currentTranches, const QDate& CurrentPeriod) const;
	virtual bool Failing(const QList<Tranche*>& currentTranches, int CurrentPeriod) const;
	virtual QString ReadyToCalculate() const override;
	const DeferredInterestTrigger::TriggerSizeSide& GetSizeSide() const;
	void SetSizeSide(const DeferredInterestTrigger::TriggerSizeSide& val);
	const DeferredInterestTrigger::TriggerSenioritySide& GetSenioritySide() const;
	void SetSenioritySide(const DeferredInterestTrigger::TriggerSenioritySide& val);
	const IntegerVector& GetTargetSeniorityLevel() const;
	void SetTargetSeniorityLevel(const QString& val);
	const IntegerVector& GetTargetSeniority() const;
	void SetTargetSeniority(const QString& val);
	const BloombergVector& GetTargetSize() const;
	void SetTargetSize(const QString& val);
	const DeferredInterestTrigger::TriggerCouponSide& GetCouponSide() const;
	void SetCouponSide(const DeferredInterestTrigger::TriggerCouponSide& val);
	const IntegerVector& GetTargetCoupon() const;
	void SetTargetCoupon(const QString& val);
	void SetSizeMultiplier(double mult);
	void ResetSizeMultiplier();
	virtual QString ToString() const override;
	bool HasAnchor()const;
	void FillMissingAnchorDate(const QDate& a);
	void SetAnchorDate(const QDate& a);
	void RemoveAnchorDate();
    static DeferredInterestTriggerSettingWidget* createSettingsWidget(QWidget* parent = nullptr);
protected:
    virtual QDataStream& WriteToStream(QDataStream& stream) const override;
    virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
};

#endif // DeferredInterestTrigger_h__