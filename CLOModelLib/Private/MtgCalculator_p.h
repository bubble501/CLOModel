#ifndef MtgCalculator_p_h__
#define MtgCalculator_p_h__

#include "Private/AbstrAsyncCalculator_p.h"
#include "MtgCalculator.h"
#include <QHash>
#include <QString>
#include "MemoryMappedDevice.h"
#include <QTemporaryFile>
class MtgCalculatorPrivate : public AbstrAsyncCalculatorPrivate
{
    DECLARE_PRIVATE_COMMONS(MtgCalculator)
    DECLARE_PRIVATE_COMMONS_DATASTREAM(MtgCalculator)
public:
    QTemporaryFile m_loansFile;
    MemoryMappedDevice m_Loans;
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
        const auto loansKeys = m_Loans.keys();
        for (auto i = loansKeys.constBegin(); i != loansKeys.constEnd(); ++i) {
            auto tempMtg = std::get<1>(m_Loans.value<Mortgage>(*i));
            tempMtg.CompileReferenceRateValue(table);
            q->SetLoan(tempMtg, *i);
        }
    }
    template <class T> void GetBaseRatesDatabaseTemplate(T& table, bool DownloadAll)
    {
        Q_Q(MtgCalculator);
        static_assert(std::is_base_of<AbstractBaseRateTable, T>::value, "GetBaseRatesDatabaseTemplate can only be used with a base rate table");
        const auto loansKeys = m_Loans.keys();
        for (auto i = loansKeys.constBegin(); i != loansKeys.constEnd(); ++i) {
            auto tempMtg = std::get<1>(m_Loans.value<Mortgage>(*i));
            tempMtg.GetBaseRatesDatabase(table, DownloadAll);
            q->SetLoan(tempMtg, *i);
        }
    }
};
#endif // MtgCalculator_p_h__