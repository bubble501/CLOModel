#include "Mortgage.h"
#include "BackwardCompatibilityInterface_p.h"
class MortgagePrivate : public BackwardInterfacePrivate
{
    DECLARE_PRIVATE_COMMONS(Mortgage)
    DECLARE_PRIVATE_COMMONS_DATASTREAM(Mortgage)
public:
    BaseRateVector m_FloatRateBase;
    BloombergVector m_FloatingRateBaseValue;
    BloombergVector m_InterestVect;
    IntegerVector m_PaymentFreq;
    MtgCashFlow m_CashFlows;
    QDate m_MaturityDate;
    QString m_Properties;
    RepaymentVector m_AnnuityVect;
    bool m_UseForwardCurve;
    double m_Size;
};