#ifndef LoanAssumption_p_h__
#define LoanAssumption_p_h__

#include "LoanAssumption.h"
#include "Private/BackwardCompatibilityInterface_p.h"
class LoanAssumptionPrivate : public BackwardInterfacePrivate
{
    DECLARE_PRIVATE_COMMONS(LoanAssumption)
    DECLARE_PRIVATE_COMMONS_DATASTREAM(LoanAssumption)
public:
    enum { SenioritySize = 2 };
    enum { AssumptionTypeSize = 14 };
    QString m_ScenarioName;
    QStringList m_Aliases;
    QString m_Assumptions[AssumptionTypeSize][SenioritySize];
    QDate m_LastUpdate[SenioritySize];
};
#endif // LoanAssumption_p_h__