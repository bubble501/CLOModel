#ifndef PrincipalProceeds_p_h__
#define PrincipalProceeds_p_h__

#include "PrincipalProceeds.h"
#include "Private/BackwardCompatibilityInterface_p.h"
class PrincipalRecipPrivate : public BackwardInterfacePrivate
{
    DECLARE_PRIVATE_COMMONS(PrincipalRecip)
    DECLARE_PRIVATE_COMMONS_COPY(PrincipalRecip)
    DECLARE_PRIVATE_COMMONS_DATASTREAM(PrincipalRecip)
public:
    double m_Scheduled;
    double m_Prepay;
};
#endif // PrincipalProceeds_p_h__