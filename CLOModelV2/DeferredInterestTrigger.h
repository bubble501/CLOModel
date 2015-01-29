#ifndef DeferredInterestTrigger_h__
#define DeferredInterestTrigger_h__
#include "AbstractTrigger.h"
#include <IntegerVector.h>
#include <BloombergVector.h>
#include "Tranche.h"
class DeferredInterestTrigger : public AbstractTrigger {
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
	DeferredInterestTrigger(const QString& lab = QString());
	DeferredInterestTrigger(const DeferredInterestTrigger& a);
	virtual bool Passing(const QList<Tranche*>& currentTranches, const QDate& CurrentPeriod) const { return passingTemplate(currentTranches, CurrentPeriod); }
	virtual bool Passing(const QList<Tranche*>& currentTranches, int CurrentPeriod) const { return passingTemplate(currentTranches, CurrentPeriod); }
	virtual bool Failing(const QList<Tranche*>& currentTranches, const QDate& CurrentPeriod) const { return !Passing(currentTranches, CurrentPeriod); }
	virtual bool Failing(const QList<Tranche*>& currentTranches, int CurrentPeriod) const { return !Passing(currentTranches, CurrentPeriod); }
	virtual QString ReadyToCalculate() const override;
	const DeferredInterestTrigger::TriggerSizeSide& GetSizeSide() const { return m_SizeSide; }
	void SetSizeSide(const DeferredInterestTrigger::TriggerSizeSide& val) { m_SizeSide = val; }
	const DeferredInterestTrigger::TriggerSenioritySide& GetSenioritySide() const { return m_SenioritySide; }
	void SetSenioritySide(const DeferredInterestTrigger::TriggerSenioritySide& val) { m_SenioritySide = val; }
	const IntegerVector& GetTargetSeniorityLevel() const { return m_TargetSeniorityLevel; }
	void SetTargetSeniorityLevel(const QString& val) { m_TargetSeniorityLevel = val; }
	const IntegerVector& GetTargetSeniority() const { return m_TargetSeniority; }
	void SetTargetSeniority(const QString& val) { m_TargetSeniority = val; }
	const BloombergVector& GetTargetSize() const { return m_TargetSize; }
	void SetTargetSize(const QString& val) { m_TargetSize = val; }
	const DeferredInterestTrigger::TriggerCouponSide& GetCouponSide() const { return m_CouponSide; }
	void SetCouponSide(const DeferredInterestTrigger::TriggerCouponSide& val) { m_CouponSide = val; }
	const IntegerVector& GetTargetCoupon() const { return m_TargetCoupon; }
	void SetTargetCoupon(const QString& val) { m_TargetCoupon = val; }
	void SetSizeMultiplier(double mult);
	void ResetSizeMultiplier() { m_TargetSize.SetDivisor(1.0); }
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
	IntegerVector m_TargetCoupon;
	TriggerSizeSide m_SizeSide;
	TriggerSenioritySide m_SenioritySide;
	TriggerCouponSide m_CouponSide;
	
	BloombergVector m_TargetSize;
private:
	template <typename T>
	bool passingTemplate(const QList<Tranche*>& currentTranches, const T& CurrentPeriod) const{
		if (!ReadyToCalculate().isEmpty()) return false;
		bool ApplicableTranche;
		double SumDeferred = 0.0;
		qint32 CurrentCoupon = m_TargetCoupon.GetValue(CurrentPeriod);
		for (auto TranIter = currentTranches.constBegin(); TranIter != currentTranches.constEnd(); ++TranIter) {
			ApplicableTranche = 
				((*TranIter)->GetProrataGroup(m_TargetSeniorityLevel.GetValue(CurrentPeriod)) < m_TargetSeniority.GetValue(CurrentPeriod) && (static_cast<quint8>(m_SenioritySide)& static_cast<quint8>(TriggerSenioritySide::Senior)))
				|| ((*TranIter)->GetProrataGroup(m_TargetSeniorityLevel.GetValue(CurrentPeriod)) > m_TargetSeniority.GetValue(CurrentPeriod) && (static_cast<quint8>(m_SenioritySide)& static_cast<quint8>(TriggerSenioritySide::Junior)))
				|| ((*TranIter)->GetProrataGroup(m_TargetSeniorityLevel.GetValue(CurrentPeriod)) == m_TargetSeniority.GetValue(CurrentPeriod) && (static_cast<quint8>(m_SenioritySide)& static_cast<quint8>(TriggerSenioritySide::Exactly)))
			;
			for (
				qint32 CoupIter = 0
				; ApplicableTranche && CoupIter < CurrentCoupon && (static_cast<quint8>(m_CouponSide) & static_cast<quint8>(TriggerCouponSide::Smaller))
				; ++CoupIter
			) {
				SumDeferred += (*TranIter)->GetCashFlow().GetDeferred(CurrentPeriod, CoupIter);
			}
			if (ApplicableTranche && (static_cast<quint8>(m_CouponSide)& static_cast<quint8>(TriggerCouponSide::Exactly))) {
				SumDeferred += (*TranIter)->GetCashFlow().GetDeferred(CurrentPeriod, CurrentCoupon);
			}
			for (
				qint32 CoupIter = CurrentCoupon + 1
				; ApplicableTranche && CoupIter < static_cast<qint32>(TrancheCashFlow::TrancheFlowType::InterestFlow) && (static_cast<quint8>(m_CouponSide)& static_cast<quint8>(TriggerCouponSide::Bigger))
				; ++CoupIter
			) {
				SumDeferred += (*TranIter)->GetCashFlow().GetDeferred(CurrentPeriod, CoupIter);
			}
		}
		bool Result = false;
		if (static_cast<quint8>(m_SizeSide)& static_cast<quint8>(TriggerSizeSide::Bigger)) {
			Result = Result || SumDeferred > m_TargetSize.GetValue(CurrentPeriod);
		}
		if (static_cast<quint8>(m_SizeSide)& static_cast<quint8>(TriggerSizeSide::Smaller)) {
			Result = Result || SumDeferred < m_TargetSize.GetValue(CurrentPeriod);
		}
		if (static_cast<quint8>(m_SizeSide)& static_cast<quint8>(TriggerSizeSide::Exactly)) {
			Result = Result || SumDeferred == m_TargetSize.GetValue(CurrentPeriod);
		}
		return Result;
	}
};

#endif // DeferredInterestTrigger_h__