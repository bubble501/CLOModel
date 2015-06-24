#include "ReserveFund.h"
#include "Private/ReserveFund_p.h"
DEFINE_PUBLIC_COMMONS(ReserveFund)
DEFINE_PUBLIC_COMMONS_COPY(ReserveFund)

ReserveFundPrivate::ReserveFundPrivate(ReserveFund *q)
	:BackwardInterfacePrivate(q)
{}
ReserveFund::ReserveFund(ReserveFundPrivate *d, const ReserveFund& other)
	:BackwardInterface(d,other)
{
    {
        int i = 0;
        for (auto j = std::begin(d->m_ReserveVects); j != std::end(d->m_ReserveVects); ++i, ++j)
            d->m_ReserveVects[i] = other.d_func()->m_ReserveVects[i];
    }
    d->m_ReserveFundCurrent = other.d_func()->m_ReserveFundCurrent;
    d->m_ReserveFundFreed = other.d_func()->m_ReserveFundFreed;
    d->m_ReserveToInterest = other.d_func()->m_ReserveToInterest;
    d->m_ReserveFundFlows = other.d_func()->m_ReserveFundFlows;
    d->m_StartingReserve = other.d_func()->m_StartingReserve;
}
ReserveFund& ReserveFund::operator=(const ReserveFund& other){
	Q_D(ReserveFund);
    BackwardInterface::operator=(other);
    for (auto j = std::begin(d->m_ReserveVects); j != std::end(d->m_ReserveVects); ++j)
        *j = other.d_func()->m_ReserveVects[std::distance(std::begin(d->m_ReserveVects), j)];
    d->m_ReserveFundCurrent = other.d_func()->m_ReserveFundCurrent;
    d->m_ReserveFundFreed = other.d_func()->m_ReserveFundFreed;
    d->m_ReserveToInterest = other.d_func()->m_ReserveToInterest;
    d->m_ReserveFundFlows = other.d_func()->m_ReserveFundFlows;
    d->m_StartingReserve = other.d_func()->m_StartingReserve;
	return *this;
}
ReserveFund::ReserveFund(ReserveFundPrivate *d)
	:BackwardInterface(d)
{
    d->m_ReserveFundCurrent = 0.0;
    d->m_ReserveFundFreed=0;
    d->m_StartingReserve=0.0;
    d->m_ReserveToInterest=true;
    d->m_ReserveFundFlows.Aggregate(GenericCashFlow::Monthly);
    for (auto i = std::begin(d->m_ReserveVects); i != std::end(d->m_ReserveVects);++i){
        i->SetVector("0");
        i->SetDivisor(1.0);
    }
   /* for (int i = 0; i < ReserveFund::EnumEnd; i++) {
        m_ReserveVects[i].SetVector("0");
        m_ReserveVects[i].SetDivisor(1.0);
    }*/
    d->m_ReserveFundFlows.SetLabel(static_cast<qint32>(ReserveFlowsType::ReplenishFromInterest), "Replenish from Interest");
    d->m_ReserveFundFlows.SetLabel(static_cast<qint32>(ReserveFlowsType::ReplenishFromPrincipal), "Replenish from Principal");
    d->m_ReserveFundFlows.SetLabel(static_cast<qint32>(ReserveFlowsType::ShortFall), "ShortFall");
}


QDataStream& operator<<(QDataStream & stream, const ReserveFund& flows) 
{
    for (auto i = std::begin(flows.d_func()->m_ReserveVects); i != std::end(flows.d_func()->m_ReserveVects); ++i)
		stream << *i;
	stream 
		<< flows.d_func()->m_ReserveFundCurrent
        << flows.d_func()->m_ReserveFundFreed
        << flows.d_func()->m_ReserveFundFlows
        << flows.d_func()->m_ReserveToInterest
        << flows.d_func()->m_StartingReserve
	;
	return stream;
}
QDataStream& operator>>(QDataStream & stream, ReserveFund& flows) { return flows.LoadOldVersion(stream); }
QDataStream& ReserveFund::LoadOldVersion(QDataStream& stream) {
    Q_D(ReserveFund);
    for (auto i = std::begin(d->m_ReserveVects); i != std::end(d->m_ReserveVects); ++i)
        stream >> *i;
	stream
        >> d->m_ReserveFundCurrent
        >> d->m_ReserveFundFreed
        >> d->m_ReserveFundFlows
        >> d->m_ReserveToInterest
        >> d->m_StartingReserve
		;
	ResetProtocolVersion();
	return stream;
}

const BloombergVector& ReserveFund::GetReserveFundTarget() const
{
    Q_D(const ReserveFund);
    return d->m_ReserveVects[ResTarget];
}

BloombergVector& ReserveFund::GetReserveFundTarget()
{
    Q_D( ReserveFund);
    return d->m_ReserveVects[ResTarget];
}

QString ReserveFund::GetReserveFundTargetString() const
{
    Q_D(const ReserveFund);
    return d->m_ReserveVects[ResTarget].GetVector();
}

void ReserveFund::SetReserveFundTarget(const QString& a)
{
    Q_D( ReserveFund);
    d->m_ReserveVects[ResTarget] = a; 
    d->OriginallyNullAnchor[ResTarget] = d->m_ReserveVects[ResTarget].GetAnchorDate().isNull();
}

BloombergVector& ReserveFund::GetReserveFundMultiple()
{
    Q_D( ReserveFund);
    return d->m_ReserveVects[ResMultiple];
}

const BloombergVector& ReserveFund::GetReserveFundMultiple() const
{
    Q_D(const ReserveFund);
    return d->m_ReserveVects[ResMultiple];
}

QString ReserveFund::GetReserveFundMultipleString() const
{
    Q_D(const ReserveFund);
    return d->m_ReserveVects[ResMultiple].GetVector();
}

void ReserveFund::SetReserveFundMultiple(const QString& a)
{
    Q_D( ReserveFund);
    d->m_ReserveVects[ResMultiple] = a; 
    d->OriginallyNullAnchor[ResMultiple] = d->m_ReserveVects[ResMultiple].GetAnchorDate().isNull();
}

const BloombergVector& ReserveFund::GetReserveFundFloor() const
{
    Q_D(const ReserveFund);
    return d->m_ReserveVects[ResFloor];
}

BloombergVector& ReserveFund::GetReserveFundFloor()
{
    Q_D( ReserveFund);
    return d->m_ReserveVects[ResFloor];
}

QString ReserveFund::GetReserveFundFloorString() const
{
    Q_D(const ReserveFund);
    return d->m_ReserveVects[ResFloor].GetVector();
}

void ReserveFund::SetReserveFundFloor(const QString& a)
{
    Q_D( ReserveFund);
    d->m_ReserveVects[ResFloor] = a;
    d->OriginallyNullAnchor[ResFloor] = d->m_ReserveVects[ResFloor].GetAnchorDate().isNull();
}

const BloombergVector& ReserveFund::GetReserveFundCap() const
{
    Q_D(const ReserveFund);
    return d->m_ReserveVects[ResCap];
}

BloombergVector& ReserveFund::GetReserveFundCap()
{
    Q_D( ReserveFund);
    return d->m_ReserveVects[ResCap];
}

QString ReserveFund::GetReserveFundCapString() const
{
    Q_D(const ReserveFund);
    return d->m_ReserveVects[ResCap].GetVector();
}

void ReserveFund::SetReserveFundCap(const BloombergVector& val)
{
    Q_D( ReserveFund);
    d->m_ReserveVects[ResCap] = val;
    d->OriginallyNullAnchor[ResCap] = d->m_ReserveVects[ResCap].GetAnchorDate().isNull();
}

double ReserveFund::GetReserveFundCurrent(const QDate& RepDate) const
{
    Q_D(const ReserveFund);
    return 
        d->m_ReserveFundFlows.GetFlow(RepDate, static_cast<qint32>(ReserveFlowsType::ReplenishFromInterest)) 
        + d->m_ReserveFundFlows.GetFlow(RepDate, static_cast<qint32>(ReserveFlowsType::ReplenishFromPrincipal));
}

double ReserveFund::GetReserveFundCurrent() const
{
    Q_D(const ReserveFund);
    return d->m_ReserveFundCurrent;
}

double ReserveFund::GetReserveFundCurrent(int a) const
{
    Q_D(const ReserveFund);
    return
        d->m_ReserveFundFlows.GetFlow(a, static_cast<qint32>(ReserveFlowsType::ReplenishFromInterest))
        + d->m_ReserveFundFlows.GetFlow(a, static_cast<qint32>(ReserveFlowsType::ReplenishFromPrincipal));
}

void ReserveFund::SetReserveFundCurrent(double a)
{
    Q_D( ReserveFund);
    d->m_ReserveFundCurrent = a;
}

double ReserveFund::GetShortfall(const QDate& RepDate) const
{
    Q_D(const ReserveFund);
    return d->m_ReserveFundFlows.GetFlow(RepDate, static_cast<qint32>(ReserveFlowsType::ShortFall));
}

double ReserveFund::GetShortfall(int a) const
{
    Q_D(const ReserveFund);
    return d->m_ReserveFundFlows.GetFlow(a, static_cast<qint32>(ReserveFlowsType::ShortFall));
}

const double& ReserveFund::GetStartingReserve() const
{
    Q_D(const ReserveFund);
    return d->m_StartingReserve;
}

void ReserveFund::SetStartingReserve(const double& val)
{
    Q_D( ReserveFund);
    d->m_StartingReserve = val;
}

int ReserveFund::GetReserveFundFreed() const
{
    Q_D(const ReserveFund);
    return d->m_ReserveFundFreed;
}

void ReserveFund::SetReserveFundFreed(int a)
{
    Q_D( ReserveFund);
    d->m_ReserveFundFreed = a;
}

const GenericCashFlow& ReserveFund::GetReserveFundFlow() const
{
    Q_D(const ReserveFund);
    return d->m_ReserveFundFlows;
}

GenericCashFlow& ReserveFund::GetReserveFundFlow()
{
    Q_D( ReserveFund);
    return d->m_ReserveFundFlows;
}

void ReserveFund::SetReserveFundFlow(const GenericCashFlow& source)
{
    Q_D( ReserveFund);
    d->m_ReserveFundFlows = source;
}

bool ReserveFund::GetReserveToInterest() const
{
    Q_D(const ReserveFund);
    return d->m_ReserveToInterest;
}

void ReserveFund::SetReserveToInterest(bool a)
{
    Q_D( ReserveFund);
    d->m_ReserveToInterest = a;
}

void ReserveFund::SetReserveFund(const QString& RFtarget, const QString& RFmultiple, const QString& RFfloor, const QString& RFcap, double RFcurrent, int RFfreed, bool RFtoInterest)
{
    Q_D( ReserveFund);
    d->m_ReserveVects[ResTarget] = RFtarget;
    d->m_ReserveVects[ResMultiple] = RFmultiple;
    d->m_ReserveVects[ResFloor] = RFfloor;
    d->m_ReserveVects[ResCap] = RFcap;
    d->m_ReserveFundCurrent = RFcurrent;
    d->m_StartingReserve = RFcurrent;
    d->m_ReserveFundFreed = RFfreed;
    d->m_ReserveToInterest = RFtoInterest;
}
void ReserveFund::ResetReserve()
{
    Q_D( ReserveFund);
    for (auto i = std::begin(d->m_ReserveVects); i != std::end(d->m_ReserveVects);++i)
		i->SetVector("0");
    d->m_ReserveFundCurrent = 0.0;
    d->m_ReserveFundFreed = 0;
    d->m_StartingReserve = 0.0;
    d->m_ReserveFundFlows.Clear();
    d->m_ReserveToInterest = true;
}
void ReserveFund::FillMissingAnchors(const QDate& NewAnchor)
{
    Q_D(ReserveFund);
    for (auto i = std::begin(d->m_ReserveVects); i != std::end(d->m_ReserveVects); ++i) {
        if (d->OriginallyNullAnchor[std::distance(std::begin(d->m_ReserveVects), i)])
            i->SetAnchorDate(NewAnchor);
    }
}
void ReserveFund::ResetMissingAnchors()
{
    Q_D( ReserveFund);
    for (auto i = std::begin(d->m_ReserveVects); i != std::end(d->m_ReserveVects); ++i) {
        if (d->OriginallyNullAnchor[std::distance(std::begin(d->m_ReserveVects), i)])
            i->RemoveAnchorDate();
    }
}

void ReserveFund::ClearFlows()
{
    Q_D( ReserveFund);
    d->m_ReserveFundFlows.Clear();
}

