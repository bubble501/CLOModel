#ifndef MtgCalculator_p_h__
#define MtgCalculator_p_h__

#include "Private/AbstrAsyncCalculator_p.h"
#include "MtgCalculator.h"
#include <QHash>
#include <QString>
class MtgCalculatorPrivate : public AbstrAsyncCalculatorPrivate
{
    DECLARE_PRIVATE_COMMONS(MtgCalculator)
    DECLARE_PRIVATE_COMMONS_DATASTREAM(MtgCalculator)
public:
    QHash<qint32, Mortgage*> Loans;
    QString m_CPRass;
    QString m_CDRass;
    QString m_LSass;
    QString m_RecoveryLag;
    QString m_Delinquency;
    QString m_DelinquencyLag;
    bool m_OverrideAssumptions;
    bool m_DownloadScenario;
    bool m_UseStoredCashFlows;
    bool m_SaveIndividualFlows;
    QDate StartDate;
    MtgCashFlow m_AggregatedRes;
    QHash<qint32, QHash<QString, QString>* > TempProperties;
};
#endif // MtgCalculator_p_h__