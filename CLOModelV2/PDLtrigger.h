#ifndef PDLtrigger_h__
#define PDLtrigger_h__
#include "AbstractTrigger.h"
#include <IntegerVector.h>
#include <BloombergVector.h>
class Tranche;
class PDLTrigger : public AbstractTrigger
{
public:
    enum class TriggerSizeSide : quint8
    {
        Invalid = 0
        , Bigger = 0x1
        , Smaller = 0x2
        , Exactly = 0x4
        , BiggerOrEqual = Bigger | Exactly
        , SmallerOrEqual = Smaller | Exactly
    };
    enum class TriggerSenioritySide : quint8
    {
        Invalid = 0
        , Senior = 0x1
        , Junior = 0x2
        , Exactly = 0x4
        , SeniorOrEqual = Senior | Exactly
        , JuniorOrEqual = Junior | Exactly
    };
    QString TriggerSizeSideToString(TriggerSizeSide a) const;
    QString TriggerSenioritySideToString(TriggerSenioritySide a) const;
    PDLTrigger(QString TargetSenior, QString TargetSeniorLevel, QString TargetSize = "0", TriggerSizeSide szd = TriggerSizeSide::Invalid, TriggerSenioritySide ssd = TriggerSenioritySide::Invalid, const QString& lab = QString());
    PDLTrigger(const QString& lab = QString());
    PDLTrigger(const PDLTrigger& a);
    virtual bool Passing(const QList<Tranche*>& currentTranches, double Assets, const QDate& CurrentPeriod) const { return PassingTemplate(currentTranches, Assets, CurrentPeriod); }
    virtual bool Passing(const QList<Tranche*>& currentTranches, double Assets, int CurrentPeriod) const { return PassingTemplate(currentTranches, Assets, CurrentPeriod); }
    virtual bool Failing(const QList<Tranche*>& currentTranches, double Assets, const QDate& CurrentPeriod) const { return !Passing(currentTranches, Assets, CurrentPeriod); }
    virtual bool Failing(const QList<Tranche*>& currentTranches, double Assets, int CurrentPeriod) const { return !Passing(currentTranches, Assets, CurrentPeriod); }
    virtual QString ReadyToCalculate() const override;
    const PDLTrigger::TriggerSizeSide& GetSizeSide() const { return m_SizeSide; }
    void SetSizeSide(const PDLTrigger::TriggerSizeSide& val) { m_SizeSide = val; }
    const PDLTrigger::TriggerSenioritySide& GetSenioritySide() const { return m_SenioritySide; }
    void SetSenioritySide(const PDLTrigger::TriggerSenioritySide& val) { m_SenioritySide = val; }
    const IntegerVector& GetTargetSeniorityLevel() const { return m_TargetSeniorityLevel; }
    void SetTargetSeniorityLevel(const QString& val) { m_TargetSeniorityLevel = val; }
    const IntegerVector& GetTargetSeniority() const { return m_TargetSeniority; }
    void SetTargetSeniority(const QString& val) { m_TargetSeniority = val; }
    const BloombergVector& GetTargetSize() const { return m_TargetSize; }
    void SetTargetSize(const QString& val) { m_TargetSize = val; }
    void SetSizeMultiplier(double mult);
    void ResetSizeMultiplier() { m_TargetSize.SetDivisor(1.0); }
    virtual QString ToString() const override;
    bool HasAnchor()const;
    void FillMissingAnchorDate(const QDate& a);
    void SetAnchorDate(const QDate& a) { m_TargetSize.SetAnchorDate(a); m_TargetSeniorityLevel.SetAnchorDate(a); m_TargetSeniority.SetAnchorDate(a); }
    void RemoveAnchorDate() { m_TargetSize.RemoveAnchorDate(); m_TargetSeniorityLevel.RemoveAnchorDate(); m_TargetSeniority.RemoveAnchorDate(); }
protected:
    template<class T>
    bool PassingTemplate(const QList<Tranche*>& currentTranches, double Assets, const T& CurrentPeriod) const
    {
        static_assert(std::is_same<T, QDate>::value || std::is_integral<T>::value, "PassingTemplate can be used only with int or QDate");
        if (!ReadyToCalculate().isEmpty()) return false;
        double PDLAmount = Assets;
        double BelowTarget = 0.0;
        for (auto i = currentTranches.constBegin(); i != currentTranches.constEnd(); ++i) {
            PDLAmount -= (*i)->GetCashFlow().GetAmountOutstanding(CurrentPeriod);
            if (
                (*i)->GetProrataGroup(m_TargetSeniorityLevel.GetValue(CurrentPeriod)) > m_TargetSeniority.GetValue(CurrentPeriod)
                || ((static_cast<quint8>(m_SenioritySide)& static_cast<quint8>(TriggerSenioritySide::Exactly)) == 0 && (*i)->GetProrataGroup(m_TargetSeniorityLevel.GetValue(CurrentPeriod)) == m_TargetSeniority.GetValue(CurrentPeriod))
                ) BelowTarget += (*i)->GetCashFlow().GetAmountOutstanding(CurrentPeriod);
        }
        PDLAmount = -qMin(0.0, PDLAmount);
        if (static_cast<quint8>(m_SenioritySide)& static_cast<quint8>(TriggerSenioritySide::SeniorOrEqual))
            PDLAmount = qMax(0.0, PDLAmount - BelowTarget);
        else
            PDLAmount = qMin(PDLAmount, BelowTarget);
        bool Result = false;
        if (static_cast<quint8>(m_SizeSide)& static_cast<quint8>(TriggerSizeSide::Bigger)) {
            Result = Result || PDLAmount > m_TargetSize.GetValue(CurrentPeriod);
        }
        if (static_cast<quint8>(m_SizeSide)& static_cast<quint8>(TriggerSizeSide::Smaller)) {
            Result = Result || PDLAmount < m_TargetSize.GetValue(CurrentPeriod);
        }
        if (static_cast<quint8>(m_SizeSide)& static_cast<quint8>(TriggerSizeSide::Exactly)) {
            Result = Result || PDLAmount == m_TargetSize.GetValue(CurrentPeriod);
        }
        return Result;
    }
    virtual QDataStream& WriteToStream(QDataStream& stream) const override;
    virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
    IntegerVector m_TargetSeniorityLevel;
    IntegerVector m_TargetSeniority;
    TriggerSizeSide m_SizeSide;
    TriggerSenioritySide m_SenioritySide;
    BloombergVector m_TargetSize;
};
#endif // PDLtrigger_h__