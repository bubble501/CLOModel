#ifndef ReinvestmentTest_p_h__
#define ReinvestmentTest_p_h__

#include "ReinvestmentTest.h"
#include "Private/BackwardCompatibilityInterface_p.h"
#include <QDate>
#include "Mortgage.h"
#include "BloombergVector.h"
#include "IntegerVector.h"
class ReinvestmentTestPrivate : public BackwardInterfacePrivate
{
    DECLARE_PRIVATE_COMMONS(ReinvestmentTest)
    DECLARE_PRIVATE_COMMONS_COPY(ReinvestmentTest)
    DECLARE_PRIVATE_COMMONS_DATASTREAM(ReinvestmentTest)
public:
    GenericCashFlow m_Reinvested;
    QDate ReinvestmentPeriod;
    Mortgage ReinvestmentBond;
    BloombergVector CPRAssumption;
    BloombergVector CDRAssumption;
    BloombergVector LSAssumption;
    BloombergVector WALAssumption;
    BloombergVector ReinvestmentPrice;
    IntegerVector m_ReinvestmentDelay;
    IntegerVector m_RecoveryLag;
    BloombergVector m_Delinquency;
    IntegerVector m_DelinquencyLag;
    IntegerVector m_ReinvestmentSpreadOverTime;
    QHash<QDate, double> ReinvestQueue;
};
#endif // ReinvestmentTest_p_h__