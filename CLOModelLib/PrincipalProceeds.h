#ifndef PrincipalProceeds_h__
#define PrincipalProceeds_h__
#include <QtGlobal>
#include "BackwardCompatibilityInterface.h"
class PrincipalRecipPrivate;
class CLOMODELLIB_EXPORT PrincipalRecip :public BackwardInterface
{
    Q_DECLARE_PRIVATE(PrincipalRecip)
protected:
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
    PrincipalRecip(PrincipalRecipPrivate* d);
public:
	virtual const double& GetPrepay() const;
    virtual void SetPrepay(const double& val);
    virtual void AddPrepay(const double& val);
    virtual const double& GetScheduled() const;
    virtual void SetScheduled(const double& val);
    virtual void AddScheduled(const double& val);
	PrincipalRecip();
	PrincipalRecip(const PrincipalRecip& a);
    virtual PrincipalRecip& operator=(const PrincipalRecip& a);
    virtual PrincipalRecip operator-(double a);
    virtual PrincipalRecip& operator-=(double a);
    virtual PrincipalRecip& operator+=(double a);
    virtual PrincipalRecip operator+(double a);
    virtual bool operator<(double a) const;
    virtual double Total() const;
    virtual void Erase();
	friend QDataStream& operator<<(QDataStream & stream, const PrincipalRecip& flows);
	friend QDataStream& operator>>(QDataStream & stream, PrincipalRecip& flows);
};
QDataStream& operator<<(QDataStream & stream, const PrincipalRecip& flows);
QDataStream& operator>>(QDataStream & stream, PrincipalRecip& flows);
#endif // PrincipalProceeds_h__