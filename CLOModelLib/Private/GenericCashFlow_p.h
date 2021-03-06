#ifndef GenericCashFlow_p_h__
#define GenericCashFlow_p_h__

#include "GenericCashFlow.h"
#include "Private/BackwardCompatibilityInterface_p.h"
#include <QMap>
#include <QSet>
#include <memory>
class GenericCashFlowPrivate : public BackwardInterfacePrivate
{
    DECLARE_PRIVATE_COMMONS(GenericCashFlow)
    DECLARE_PRIVATE_COMMONS_COPY(GenericCashFlow)
    DECLARE_PRIVATE_COMMONS_DATASTREAM(GenericCashFlow)
public:
    static bool SamePeriod(const QDate& a, const QDate& b, GenericCashFlow::CashFlowAggregation Freq);
    QMap<QDate, std::shared_ptr<QHash<qint32, double> > > m_CashFlows;
    QHash<qint32, QString> m_CashFlowLabels;
    QSet<qint32> m_Stocks;
    GenericCashFlow::CashFlowAggregation m_AggregationLevel;
    bool m_AdjustHolidays;
};
#endif // GenericCashFlow_p_h__