#include "GenericCashFlow.h"
#include "BackwardCompatibilityInterface_p.h"
#include <QMap>
#include <QSet>
class GenericCashFlowPrivate : public BackwardInterfacePrivate
{
    Q_DECLARE_PUBLIC(GenericCashFlow)
private:
    GenericCashFlowPrivate(const GenericCashFlowPrivate& other);
public:
    GenericCashFlowPrivate(GenericCashFlow* q);
    GenericCashFlowPrivate(GenericCashFlow* q, const GenericCashFlowPrivate& other);
    virtual GenericCashFlowPrivate& operator=(const GenericCashFlowPrivate& other);
    static bool SamePeriod(const QDate& a, const QDate& b, GenericCashFlow::CashFlowAggregation Freq);
    QMap<QDate, QHash<qint32, double>*	> m_CashFlows;
    QHash<qint32, QString> m_CashFlowLabels;
    QSet<qint32> m_Stocks;
    GenericCashFlow::CashFlowAggregation m_AggregationLevel;
    bool m_AdjustHolidays;
    friend QDataStream& operator<<(QDataStream & stream, const GenericCashFlow& flows);
    friend QDataStream& operator>>(QDataStream & stream, GenericCashFlow& flows);
};