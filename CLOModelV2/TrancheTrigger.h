#ifndef TrancheTrigger_h__
#define TrancheTrigger_h__
#include "AbstractTrigger.h"
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
	TrancheTrigger(qint32 TargetSenior, qint32 TargetSeniorLevel = 0, double TargetSize=0.0, TriggerSizeSide szd = TriggerSizeSide::Invalid, TriggerSenioritySide ssd = TriggerSenioritySide::Invalid, const QString& lab = QString());
	TrancheTrigger(const QString& lab = QString());
	TrancheTrigger(const TrancheTrigger& a);
	virtual bool Passing(const QList<Tranche*>& currentTranches) const;
	virtual bool Failing(const QList<Tranche*>& currentTranches) const { return !Passing(currentTranches); }
	virtual QString ReadyToCalculate() const override;
	const TrancheTrigger::TriggerSizeSide& GetSizeSide() const { return m_SizeSide; }
	void SetSizeSide(const TrancheTrigger::TriggerSizeSide& val) { m_SizeSide = val; }
	const TrancheTrigger::TriggerSenioritySide& GetSenioritySide() const { return m_SenioritySide; }
	void SetSenioritySide(const TrancheTrigger::TriggerSenioritySide& val) { m_SenioritySide = val; }
	const qint32& GetTargetSeniorityLevel() const { return m_TargetSeniorityLevel; }
	void SetTargetSeniorityLevel(const qint32& val) { m_TargetSeniorityLevel = val; }
	const qint32& GetTargetSeniority() const { return m_TargetSeniority; }
	void SetTargetSeniority(const qint32& val) { m_TargetSeniority = val; }
	const double& GetTargetSize() const { return m_TargetSize; }
	void SetTargetSize(const double& val) { m_TargetSize = val; }
protected:
	virtual QDataStream& WriteToStream(QDataStream& stream) const override;
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
	qint32 m_TargetSeniorityLevel;
	qint32 m_TargetSeniority;
	TriggerSizeSide m_SizeSide;
	TriggerSenioritySide m_SenioritySide;
	double m_TargetSize;
};
#endif // TrancheTrigger_h__