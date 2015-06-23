#ifndef PDLtrigger_p_h__
#define PDLtrigger_p_h__

#include "Private/AbstractTrigger_p.h"
#include "PDLtrigger.h"
#include <QDate>
#include <IntegerVector.h>
#include <BloombergVector.h>
#include "Tranche.h"
class PDLTriggerPrivate : public AbstractTriggerPrivate
{
    DECLARE_PRIVATE_COMMONS(PDLTrigger)
public:
    IntegerVector m_TargetSeniorityLevel;
    IntegerVector m_TargetSeniority;
    PDLTrigger::TriggerSizeSide m_SizeSide;
    PDLTrigger::TriggerSenioritySide m_SenioritySide;
    BloombergVector m_TargetSize;
    template<class T>
    bool PassingTemplate(const QList<Tranche*>& currentTranches, double Assets, const T& CurrentPeriod) const
    {
        Q_Q(const PDLTrigger);
        static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "PassingTemplate can be used only with int or QDate");
        if (!q->ReadyToCalculate().isEmpty()) return false;
        double PDLAmount = Assets;
        double BelowTarget = 0.0;
        for (auto i = currentTranches.constBegin(); i != currentTranches.constEnd(); ++i) {
            PDLAmount -= (*i)->GetCashFlow().GetAmountOutstanding(CurrentPeriod);
            if (
                (*i)->GetProrataGroup(m_TargetSeniorityLevel.GetValue(CurrentPeriod)) > m_TargetSeniority.GetValue(CurrentPeriod)
                || ((static_cast<quint8>(m_SenioritySide)& static_cast<quint8>(PDLTrigger::TriggerSenioritySide::Exactly)) == 0 && (*i)->GetProrataGroup(m_TargetSeniorityLevel.GetValue(CurrentPeriod)) == m_TargetSeniority.GetValue(CurrentPeriod))
                ) BelowTarget += (*i)->GetCashFlow().GetAmountOutstanding(CurrentPeriod);
        }
        PDLAmount = -qMin(0.0, PDLAmount);
        if (static_cast<quint8>(m_SenioritySide)& static_cast<quint8>(PDLTrigger::TriggerSenioritySide::SeniorOrEqual))
            PDLAmount = qMax(0.0, PDLAmount - BelowTarget);
        else
            PDLAmount = qMin(PDLAmount, BelowTarget);
        bool Result = false;
        if (static_cast<quint8>(m_SizeSide)& static_cast<quint8>(PDLTrigger::TriggerSizeSide::Bigger)) {
            Result = Result || PDLAmount > m_TargetSize.GetValue(CurrentPeriod);
        }
        if (static_cast<quint8>(m_SizeSide)& static_cast<quint8>(PDLTrigger::TriggerSizeSide::Smaller)) {
            Result = Result || PDLAmount < m_TargetSize.GetValue(CurrentPeriod);
        }
        if (static_cast<quint8>(m_SizeSide)& static_cast<quint8>(PDLTrigger::TriggerSizeSide::Exactly)) {
            Result = Result || PDLAmount == m_TargetSize.GetValue(CurrentPeriod);
        }
        return Result;
    }
};
#endif // PDLtrigger_p_h__