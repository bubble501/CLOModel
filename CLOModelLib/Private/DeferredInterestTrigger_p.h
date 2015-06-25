#ifndef DeferredInterestTrigger_p_h__
#define DeferredInterestTrigger_p_h__

#include "Private/AbstractTrigger_p.h"
#include "DeferredInterestTrigger.h"
#include "BloombergVector.h"
#include "IntegerVector.h"
#include "Tranche.h"
class DeferredInterestTriggerPrivate : public AbstractTriggerPrivate
{
    DECLARE_PRIVATE_COMMONS(DeferredInterestTrigger)
public:
    IntegerVector m_TargetSeniorityLevel;
    IntegerVector m_TargetSeniority;
    IntegerVector m_TargetCoupon;
    DeferredInterestTrigger::TriggerSizeSide m_SizeSide;
    DeferredInterestTrigger::TriggerSenioritySide m_SenioritySide;
    DeferredInterestTrigger::TriggerCouponSide m_CouponSide;

    BloombergVector m_TargetSize;

    template <typename T>
    bool passingTemplate(const QList<Tranche*>& currentTranches, const T& CurrentPeriod) const
    {
        Q_Q(const DeferredInterestTrigger);
        if (!q->ReadyToCalculate().isEmpty()) return false;
        bool ApplicableTranche;
        double SumDeferred = 0.0;
        qint32 CurrentCoupon = m_TargetCoupon.GetValue(CurrentPeriod);
        for (auto TranIter = currentTranches.constBegin(); TranIter != currentTranches.constEnd(); ++TranIter) {
            ApplicableTranche =
                ((*TranIter)->GetProrataGroup(m_TargetSeniorityLevel.GetValue(CurrentPeriod)) < m_TargetSeniority.GetValue(CurrentPeriod) && (static_cast<quint8>(m_SenioritySide)& static_cast<quint8>(DeferredInterestTrigger::TriggerSenioritySide::Senior)))
                || ((*TranIter)->GetProrataGroup(m_TargetSeniorityLevel.GetValue(CurrentPeriod)) > m_TargetSeniority.GetValue(CurrentPeriod) && (static_cast<quint8>(m_SenioritySide)& static_cast<quint8>(DeferredInterestTrigger::TriggerSenioritySide::Junior)))
                || ((*TranIter)->GetProrataGroup(m_TargetSeniorityLevel.GetValue(CurrentPeriod)) == m_TargetSeniority.GetValue(CurrentPeriod) && (static_cast<quint8>(m_SenioritySide)& static_cast<quint8>(DeferredInterestTrigger::TriggerSenioritySide::Exactly)))
                ;
            for (
                qint32 CoupIter = 0
                ; ApplicableTranche && CoupIter < CurrentCoupon && (static_cast<quint8>(m_CouponSide)& static_cast<quint8>(DeferredInterestTrigger::TriggerCouponSide::Smaller))
                ; ++CoupIter
                ) {
                SumDeferred += (*TranIter)->GetCashFlow().GetDeferred(CurrentPeriod, CoupIter);
            }
            if (ApplicableTranche && (static_cast<quint8>(m_CouponSide)& static_cast<quint8>(DeferredInterestTrigger::TriggerCouponSide::Exactly))) {
                SumDeferred += (*TranIter)->GetCashFlow().GetDeferred(CurrentPeriod, CurrentCoupon);
            }
            for (
                qint32 CoupIter = CurrentCoupon + 1
                ; ApplicableTranche && CoupIter < TrancheCashFlow::TrancheFlowType::InterestFlow && (static_cast<quint8>(m_CouponSide)& static_cast<quint8>(DeferredInterestTrigger::TriggerCouponSide::Bigger))
                ; ++CoupIter
                ) {
                SumDeferred += (*TranIter)->GetCashFlow().GetDeferred(CurrentPeriod, CoupIter);
            }
        }
        bool Result = false;
        if (static_cast<quint8>(m_SizeSide)& static_cast<quint8>(DeferredInterestTrigger::TriggerSizeSide::Bigger)) {
            Result = Result || SumDeferred > m_TargetSize.GetValue(CurrentPeriod);
        }
        if (static_cast<quint8>(m_SizeSide)& static_cast<quint8>(DeferredInterestTrigger::TriggerSizeSide::Smaller)) {
            Result = Result || SumDeferred < m_TargetSize.GetValue(CurrentPeriod);
        }
        if (static_cast<quint8>(m_SizeSide)& static_cast<quint8>(DeferredInterestTrigger::TriggerSizeSide::Exactly)) {
            Result = Result || SumDeferred == m_TargetSize.GetValue(CurrentPeriod);
        }
        return Result;
    }
};
#endif // DeferredInterestTrigger_p_h__