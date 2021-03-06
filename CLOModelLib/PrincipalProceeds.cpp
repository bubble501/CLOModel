#include "PrincipalProceeds.h"
#include "Private/PrincipalProceeds_p.h"
#include <QDataStream>
DEFINE_PUBLIC_COMMONS(PrincipalRecip)
DEFINE_PUBLIC_COMMONS_COPY(PrincipalRecip)
PrincipalRecip::~PrincipalRecip() {}
PrincipalRecipPrivate::~PrincipalRecipPrivate() {}
PrincipalRecipPrivate::PrincipalRecipPrivate(PrincipalRecip *q)
	:BackwardInterfacePrivate(q)
    , m_Scheduled(0.0)
    , m_Prepay(0.0)
{}
PrincipalRecipPrivate::PrincipalRecipPrivate(PrincipalRecip *q,const PrincipalRecipPrivate& other)
	:BackwardInterfacePrivate(q,other)
    , m_Scheduled(other.m_Scheduled)
    , m_Prepay(other.m_Prepay)
{}
PrincipalRecip::PrincipalRecip(PrincipalRecipPrivate *d, const PrincipalRecip& other)
	:BackwardInterface(d,other)
{}
PrincipalRecip::PrincipalRecip(PrincipalRecipPrivate *d)
	:BackwardInterface(d)
{}

PrincipalRecip& PrincipalRecip::operator=(const PrincipalRecip& other)
{
    BackwardInterface::operator=(other);
    Q_D(PrincipalRecip);
    d->m_Scheduled = other.d_func()->m_Scheduled;
    d->m_Prepay = other.d_func()->m_Prepay;
    return *this;
}
QDataStream& PrincipalRecip::LoadOldVersion(QDataStream& stream) {
    Q_D(PrincipalRecip);
    stream >> d->m_Scheduled >> d->m_Prepay;
	ResetProtocolVersion();
	return stream;
}

void PrincipalRecip::NormaliseValues()
{
    Q_D(PrincipalRecip);
    if (qAbs(d->m_Scheduled) < 0.01)
        d->m_Scheduled = 0.0;
    if (qAbs(d->m_Prepay) < 0.01)
        d->m_Prepay = 0.0;
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
    NormaliseValues();
}

void PrincipalRecip::AddPrepay(const double& val)
{
    Q_D(PrincipalRecip);
    if (qAbs(val) >= 0.01) 
        d->m_Prepay += val; 
    NormaliseValues();
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
    NormaliseValues();
}

void PrincipalRecip::AddScheduled(const double& val)
{
    Q_D(PrincipalRecip);
    if (qAbs(val) >= 0.01) 
        d->m_Scheduled += val;
    NormaliseValues();
}


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
    NormaliseValues();
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
    NormaliseValues();
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
