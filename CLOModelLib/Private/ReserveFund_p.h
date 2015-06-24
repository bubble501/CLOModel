#ifndef ReserveFund_p_h__
#define ReserveFund_p_h__

#include "ReserveFund.h"
#include "Private/BackwardCompatibilityInterface_p.h"
#include "BloombergVector.h"
#include "GenericCashFlow.h"
class ReserveFundPrivate : public BackwardInterfacePrivate
{
    DECLARE_PRIVATE_COMMONS(ReserveFund)
    DECLARE_PRIVATE_COMMONS_DATASTREAM(ReserveFund)
public:
    
    bool OriginallyNullAnchor[ReserveFund::EnumEnd];
    BloombergVector m_ReserveVects[ReserveFund::EnumEnd];
    double m_StartingReserve;
    double m_ReserveFundCurrent;
    int m_ReserveFundFreed;
    GenericCashFlow m_ReserveFundFlows;
    bool m_ReserveToInterest;
};
#endif // ReserveFund_p_h__