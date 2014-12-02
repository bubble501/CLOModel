#ifndef TrancheTrigger_h__
#define TrancheTrigger_h__
#include "AbstractTrigger.h"
#include <IntegerVector.h>
#include <BloombergVector.h>
class Tranche;
class TrancheTrigger : public AbstractTrigger {
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
	TrancheTrigger(const QString& lab = QString());
	TrancheTrigger(const TrancheTrigger& a);
	virtual bool Passing(const QList<Tranche*>& currentTranches, const QDate& CurrentPeriod) const;
	virtual bool Passing(const QList<Tranche*>& currentTranches, int CurrentPeriod) const;
	virtual bool Failing(const QList<Tranche*>& currentTranches, const QDate& CurrentPeriod) const { return !Passing(currentTranches, CurrentPeriod); }
	virtual bool Failing(const QList<Tranche*>& currentTranches, int CurrentPeriod) const { return !Passing(currentTranches, CurrentPeriod); }
	virtual QString ReadyToCalculate() const override;
	const TrancheTrigger::TriggerSizeSide& GetSizeSide() const { return m_SizeSide; }
	void SetSizeSide(const TrancheTrigger::TriggerSizeSide& val) { m_SizeSide = val; }
	const TrancheTrigger::TriggerSenioritySide& GetSenioritySide() const { return m_SenioritySide; }
	void SetSenioritySide(const TrancheTrigger::TriggerSenioritySide& val) { m_SenioritySide = val; }
	const IntegerVector& GetTargetSeniorityLevel() const { return m_TargetSeniorityLevel; }
	void SetTargetSeniorityLevel(const QString& val) { m_TargetSeniorityLevel = val; }
	const IntegerVector& GetTargetSeniority() const { return m_TargetSeniority; }
	void SetTargetSeniority(const QString& val) { m_TargetSeniority = val; }
	const BloombergVector& GetTargetSize() const { return m_TargetSize; }
	void SetTargetSize(const QString& val) { m_TargetSize = val; }
	void SetSizeMultiplier(double mult) { m_TargetSize.SetDivisor(100.0*(1.0/mult)); }
	void ResetSizeMultiplier() { m_TargetSize.SetDivisor(100.0); }
	virtual QString ToString() const override;
	bool HasAnchor()const;
	void FillMissingAnchorDate(const QDate& a);
	void SetAnchorDate(const QDate& a) { m_TargetSize.SetAnchorDate(a); m_TargetSeniorityLevel.SetAnchorDate(a); m_TargetSeniority.SetAnchorDate(a); }
	void RemoveAnchorDate() { m_TargetSize.RemoveAnchorDate(); m_TargetSeniorityLevel.RemoveAnchorDate(); m_TargetSeniority.RemoveAnchorDate(); }
protected:
	virtual QDataStream& WriteToStream(QDataStream& stream) const override;
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
	IntegerVector m_TargetSeniorityLevel;
	IntegerVector m_TargetSeniority;
	TriggerSizeSide m_SizeSide;
	TriggerSenioritySide m_SenioritySide;
	BloombergVector m_TargetSize;
};
#endif // TrancheTrigger_h__