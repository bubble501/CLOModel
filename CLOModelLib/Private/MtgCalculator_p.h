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
    QHash<qint32, QString > m_LoansPath;
    QString m_CPRass;
    QString m_CDRass;
    QString m_LSass;
    QString m_RecoveryLag;
    QString m_Delinquency;
    QString m_DelinquencyLag;
    bool m_OverrideAssumptions;
    bool m_DownloadScenario;
    bool m_UseStoredCashFlows;
    QDate StartDate;
    MtgCashFlow m_AggregatedRes;
    QHash<qint32, std::shared_ptr<QHash<QString, QString> > > TempProperties;
    template <class T> void CompileReferenceRateValueTemplate(T& table)
    {
        Q_Q(MtgCalculator);
        static_assert(std::is_base_of<AbstractBaseRateTable, T>::value, "CompileReferenceRateValueTemplate can only be used with a base rate table");
        for (auto i = m_LoansPath.constBegin(); i != m_LoansPath.constEnd(); ++i) {
            auto tempMtg = q->readTempFile<Mortgage>(i.value());
            tempMtg.CompileReferenceRateValue(table);
            q->SetLoan(tempMtg, i.key());
        }
    }
    template <class T> void GetBaseRatesDatabaseTemplate(T& table, bool DownloadAll)
    {
        Q_Q(MtgCalculator);
        static_assert(std::is_base_of<AbstractBaseRateTable, T>::value, "GetBaseRatesDatabaseTemplate can only be used with a base rate table");
        for (auto i = m_LoansPath.constBegin(); i != m_LoansPath.constEnd(); ++i) {
            auto tempMtg = q->readTempFile<Mortgage>(i.value());
            tempMtg.GetBaseRatesDatabase(table, DownloadAll);
            q->SetLoan(tempMtg, i.key());
        }
    }
};
#endif // MtgCalculator_p_h__