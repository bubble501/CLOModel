#ifndef PrincipalProceeds_h__
#define PrincipalProceeds_h__
#include "BackwardCompatibilityInterface.h"
class PrincipalRecipPrivate;
class CLOMODELLIB_EXPORT PrincipalRecip :public BackwardInterface
{
    DECLARE_PUBLIC_COMMONS(PrincipalRecip)
    DECLARE_PUBLIC_COMMONS_COPY(PrincipalRecip)
protected:
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
    virtual void NormaliseValues();
public:
	virtual const double& GetPrepay() const;
    virtual void SetPrepay(const double& val);
    virtual void AddPrepay(const double& val);
    virtual const double& GetScheduled() const;
    virtual void SetScheduled(const double& val);
    virtual void AddScheduled(const double& val);
    virtual PrincipalRecip operator-(double a);
    virtual PrincipalRecip& operator-=(double a);
    virtual PrincipalRecip& operator+=(double a);
    virtual PrincipalRecip operator+(double a);
    virtual bool operator<(double a) const;
    virtual double Total() const;
    virtual void Erase();
	friend CLOMODELLIB_EXPORT QDataStream& operator<<(QDataStream & stream, const PrincipalRecip& flows);
	friend CLOMODELLIB_EXPORT QDataStream& operator>>(QDataStream & stream, PrincipalRecip& flows);
};
CLOMODELLIB_EXPORT QDataStream& operator<<(QDataStream & stream, const PrincipalRecip& flows);
CLOMODELLIB_EXPORT QDataStream& operator>>(QDataStream & stream, PrincipalRecip& flows);
#endif // PrincipalProceeds_h__