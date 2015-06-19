#include "PrincipalProceeds.h"
#include "PrincipalProceeds_p.h"
#include <QDataStream>
QDataStream& PrincipalRecip::LoadOldVersion(QDataStream& stream) {
    Q_D(PrincipalRecip);
    stream >> d->m_Scheduled >> d->m_Prepay;
	ResetProtocolVersion();
	return stream;
}

void PrincipalRecipPrivate::NormaliseValues()
{
    if (qAbs(m_Scheduled) < 0.01)
        m_Scheduled = 0.0;
    if (qAbs(m_Prepay) < 0.01)
        m_Prepay = 0.0;
}

PrincipalRecip& PrincipalRecip::operator=(const PrincipalRecip& a)
{
    Q_D(PrincipalRecip);
    d->operator=(*a.d_func());
    return *this;
}

const double& PrincipalRecip::GetPrepay() const
{
    Q_D(const PrincipalRecip);
    return d->m_Prepay;
}

void PrincipalRecip::SetPrepay(const double& val)
{
    Q_D(PrincipalRecip);
    d->m_Prepay = qMax(val, 0.0);
    d->NormaliseValues();
}

void PrincipalRecip::AddPrepay(const double& val)
{
    Q_D(PrincipalRecip);
    if (qAbs(val) >= 0.01) 
        d->m_Prepay += val; 
    d->NormaliseValues();
}

const double& PrincipalRecip::GetScheduled() const
{
    Q_D(const PrincipalRecip);
    return d->m_Scheduled;
}

void PrincipalRecip::SetScheduled(const double& val)
{
    Q_D(PrincipalRecip);
    d->m_Scheduled = qMax(val, 0.0);
    d->NormaliseValues();
}

void PrincipalRecip::AddScheduled(const double& val)
{
    Q_D(PrincipalRecip);
    if (qAbs(val) >= 0.01) 
        d->m_Scheduled += val;
    d->NormaliseValues();
}

PrincipalRecip::PrincipalRecip(const PrincipalRecip& a)
    :BackwardInterface(new PrincipalRecipPrivate(this,*a.d_func()))
{}

PrincipalRecip::PrincipalRecip()
    :BackwardInterface(new PrincipalRecipPrivate(this))
{}

PrincipalRecip& PrincipalRecip::operator-=(double a)
{
    Q_D(PrincipalRecip);
    if (qAbs(a) < 0.01) 
        return *this;
    if (a < 0.0) 
        return operator+=(-a);
    double Temp = qMin(a, d->m_Scheduled);
    d->m_Scheduled -= Temp;
    d->m_Prepay -= a - Temp;
    d->NormaliseValues();
    return *this;
}

PrincipalRecip PrincipalRecip::operator-(double a)
{
    PrincipalRecip Result(*this);
    Result -= a; 
    return Result;
}

PrincipalRecip& PrincipalRecip::operator+=(double a)
{
    Q_D(PrincipalRecip);
    if (qAbs(a) < 0.01) 
        return *this;
    if (a < 0.0) 
        return operator-=(-a);
    d->m_Scheduled += a;
    d->NormaliseValues();
    return *this;
}

PrincipalRecip PrincipalRecip::operator+(double a)
{
    PrincipalRecip Result(*this); Result += a; return Result;
}

bool PrincipalRecip::operator<(double a) const
{
    Q_D(const PrincipalRecip);
    return d->m_Scheduled < a || d->m_Prepay < a;
}

double PrincipalRecip::Total() const
{
    Q_D(const PrincipalRecip);
    return d->m_Scheduled + d->m_Prepay;
}

void PrincipalRecip::Erase()
{
    Q_D(PrincipalRecip);
    d->m_Scheduled = d->m_Prepay = 0.0;
}

QDataStream& operator<<(QDataStream & stream, const PrincipalRecip& flows)
{
    stream << flows.d_func()->m_Scheduled << flows.d_func()->m_Prepay;
	return stream;
}
QDataStream& operator>>(QDataStream & stream, PrincipalRecip& flows) {
	return flows.LoadOldVersion(stream);
}
PrincipalRecipPrivate::PrincipalRecipPrivate(PrincipalRecip* q)
    :BackwardInterfacePrivate(q)
    , m_Scheduled(0.0)
    , m_Prepay(0.0)
{}

PrincipalRecipPrivate::PrincipalRecipPrivate(PrincipalRecip* q, const PrincipalRecipPrivate& other)
    : BackwardInterfacePrivate(q, other)
    , m_Scheduled(other.m_Scheduled)
    , m_Prepay(other.m_Prepay)
{}

PrincipalRecipPrivate& PrincipalRecipPrivate::operator=(const PrincipalRecipPrivate& other)
{
    BackwardInterfacePrivate::operator=(other);
    m_Scheduled = other.m_Scheduled;
    m_Prepay = other.m_Prepay;
    return *this;
}