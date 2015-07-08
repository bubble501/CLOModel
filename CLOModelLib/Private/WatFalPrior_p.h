#ifndef WatFalPrior_p_h__
#define WatFalPrior_p_h__
#include <QHash>
#include "WatFalPrior.h"
#include "Private/BackwardCompatibilityInterface_p.h"
#include <QSet>
class WatFalPriorPrivate : public BackwardInterfacePrivate
{
    DECLARE_PRIVATE_COMMONS(WatFalPrior)
    DECLARE_PRIVATE_COMMONS_COPY(WatFalPrior)
    DECLARE_PRIVATE_COMMONS_DATASTREAM(WatFalPrior)
public:
    QHash<qint32, IntegerVector*> IntParameters;
    QHash<qint32, BloombergVector*> DoubleParameters;
    QString TriggerStruc;
    WatFalPrior::wstAccrueOrPay m_AccrueOrPay;
    WatFalPrior::WaterfallStepType PriorityType;
    QSet<qint32> FilledNullAnchors;
};
#endif // WatFalPrior_p_h__