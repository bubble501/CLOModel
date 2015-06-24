#ifndef TrancheTrigger_h__
#define TrancheTrigger_h__
#include "AbstractTrigger.h"
#include <QString>
class TrancheTriggerPrivate;
class Tranche;
class QDate;
class IntegerVector;
class TrancheTrigger : public AbstractTrigger {
    DECLARE_PUBLIC_COMMONS(TrancheTrigger)
    DECLARE_PUBLIC_COMMONS_COPY(TrancheTrigger)
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
	QString TriggerSizeSideToString(TriggerSizeSide a) const;
	QString TriggerSenioritySideToString(TriggerSenioritySide a) const;
	TrancheTrigger(QString TargetSenior, QString TargetSeniorLevel, QString TargetSize = "0", TriggerSizeSide szd = TriggerSizeSide::Invalid, TriggerSenioritySide ssd = TriggerSenioritySide::Invalid, const QString& lab = QString());
	TrancheTrigger(const QString& lab);
	virtual bool Passing(const QList<Tranche*>& currentTranches, const QDate& CurrentPeriod) const;
	virtual bool Passing(const QList<Tranche*>& currentTranches, int CurrentPeriod) const;
	virtual bool Failing(const QList<Tranche*>& currentTranches, const QDate& CurrentPeriod) const;
	virtual bool Failing(const QList<Tranche*>& currentTranches, int CurrentPeriod) const;
	virtual QString ReadyToCalculate() const override;
	const TrancheTrigger::TriggerSizeSide& GetSizeSide() const;
	void SetSizeSide(const TrancheTrigger::TriggerSizeSide& val);
	const TrancheTrigger::TriggerSenioritySide& GetSenioritySide() const;
	void SetSenioritySide(const TrancheTrigger::TriggerSenioritySide& val);
	const IntegerVector& GetTargetSeniorityLevel() const;
	void SetTargetSeniorityLevel(const QString& val);
	const IntegerVector& GetTargetSeniority() const;
	void SetTargetSeniority(const QString& val);
	const BloombergVector& GetTargetSize() const;
	void SetTargetSize(const QString& val);
	void SetSizeMultiplier(double mult);
	void ResetSizeMultiplier();
	virtual QString ToString() const override;
	bool HasAnchor()const;
	void FillMissingAnchorDate(const QDate& a);
	void SetAnchorDate(const QDate& a);
	void RemoveAnchorDate();
protected:
	virtual QDataStream& WriteToStream(QDataStream& stream) const override;
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
	
};
#endif // TrancheTrigger_h__