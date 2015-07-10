#ifndef MtgCalculatorThread_p_h__
#define MtgCalculatorThread_p_h__

#include "Private/AbstrAsyncThread_p.h"
#include "MtgCalculatorThread.h"
#include <QString>
#include <QDate>
#include "Mortgage.h"
class MtgCalculatorThreadPrivate : public AbstrAsyncThreadPrivate
{
    DECLARE_PRIVATE_COMMONS(MtgCalculatorThread)
public:
    Mortgage Loan;
    QString CPRass;
    QString CDRass;
    QString LSass;
    QString RecoveryLag;
    QString Delinquency;
    QString DelinquencyLag;
    QDate StartDate;
    bool m_OverrideAssumptions;
    bool m_DownloadScenario;
};
#endif // MtgCalculatorThread_p_h__