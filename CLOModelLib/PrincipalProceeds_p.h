#ifndef PrincipalProceeds_p_h__
#define PrincipalProceeds_p_h__

#include "PrincipalProceeds.h"
#include "BackwardCompatibilityInterface_p.h"
class PrincipalRecipPrivate : public BackwardInterfacePrivate
{
    Q_DECLARE_PUBLIC(PrincipalRecip)
private:
    PrincipalRecipPrivate(const PrincipalRecipPrivate& other);
public:
    PrincipalRecipPrivate(PrincipalRecip* q);
    PrincipalRecipPrivate(PrincipalRecip* q, const PrincipalRecipPrivate& other);
    virtual PrincipalRecipPrivate& operator=(const PrincipalRecipPrivate& other);
    double m_Scheduled;
    double m_Prepay;
    virtual void NormaliseValues();
    friend QDataStream& operator<<(QDataStream & stream, const PrincipalRecip& flows);
    friend QDataStream& operator>>(QDataStream & stream, PrincipalRecip& flows);
};
#endif // PrincipalProceeds_p_h__