#ifndef AssumptionSet_p_h__
#define AssumptionSet_p_h__

#include "Private/BackwardCompatibilityInterface_p.h"
#include "AssumptionSet.h"
class AssumptionSetPrivate : public BackwardInterfacePrivate
{
    DECLARE_PRIVATE_COMMONS(AssumptionSet)
    DECLARE_PRIVATE_COMMONS_COPY(AssumptionSet)
    DECLARE_PRIVATE_COMMONS_DATASTREAM(AssumptionSet)
public:
    QString m_CDRscenario;
    QString m_CPRscenario;
    QString m_LSscenario;
    QString m_RecLagScenario;
    QString m_DelinqScenario;
    QString m_DelinqLagScenario;
    friend bool operator==(const AssumptionSet &e1, const AssumptionSet &e2);
};
#endif // AssumptionSet_p_h__