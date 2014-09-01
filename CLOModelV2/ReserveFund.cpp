#include "ReserveFund.h"
ReserveFund::ReserveFund() 
:m_ReserveFundCurrent(0.0),
m_ReserveFundFreed(0),
m_StartingReserve(0.0),
m_ReserveToInterest(true)
{
	m_ReserveFundFlows.Aggregate(GenericCashFlow::Monthly);
	for (int i = 0; i < ReserveFund::EnumEnd; i++) {
		m_ReserveVects[i].SetVector("0");
		m_ReserveVects[i].SetDivisor(1.0);
	}
}
ReserveFund::ReserveFund(const ReserveFund& a)
:m_ReserveFundCurrent(a.m_ReserveFundCurrent),
m_ReserveFundFreed(a.m_ReserveFundFreed),
m_ReserveToInterest(a.m_ReserveToInterest),
m_ReserveFundFlows(a.m_ReserveFundFlows),
m_StartingReserve(a.m_StartingReserve)
{
	for (int i = 0; i < ReserveFund::EnumEnd; i++)
		m_ReserveVects[i] = a.m_ReserveVects[i];
}
ReserveFund& ReserveFund::operator=(const ReserveFund& a) {
	for (int i = 0; i < ReserveFund::EnumEnd; i++)
		m_ReserveVects[i] = a.m_ReserveVects[i];
	m_ReserveFundCurrent = a.m_ReserveFundCurrent;
	m_ReserveFundFreed = a.m_ReserveFundFreed;
	m_ReserveToInterest = a.m_ReserveToInterest;
	m_ReserveFundFlows = a.m_ReserveFundFlows;
	m_StartingReserve = a.m_StartingReserve;
	return *this;
}
QDataStream& operator<<(QDataStream & stream, const ReserveFund& flows) {
	for (int i = 0; i < ReserveFund::EnumEnd; i++)
		stream << flows.m_ReserveVects[i];
	stream 
		<< flows.m_ReserveFundCurrent
		<< flows.m_ReserveFundFreed
		<< flows.m_ReserveFundFlows
		<< flows.m_ReserveToInterest
		<< flows.m_StartingReserve
	;
	return stream;
}
QDataStream& operator>>(QDataStream & stream, ReserveFund& flows) { return flows.LoadOldVersion(stream); }
QDataStream& ReserveFund::LoadOldVersion(QDataStream& stream) {
	for (int i = 0; i < EnumEnd; i++)
		stream >> m_ReserveVects[i];
	stream
		>> m_ReserveFundCurrent
		>> m_ReserveFundFreed
		>> m_ReserveFundFlows
		>> m_ReserveToInterest
		>> m_StartingReserve
		;
	ResetProtocolVersion();
	return stream;
}
void ReserveFund::SetReserveFund(const QString& RFtarget, const QString& RFmultiple, const QString& RFfloor, const QString& RFcap, double RFcurrent, int RFfreed, bool RFtoInterest) {
	m_ReserveVects[ResTarget] = RFtarget;
	m_ReserveVects[ResMultiple] = RFmultiple;
	m_ReserveVects[ResFloor] = RFfloor;
	m_ReserveVects[ResCap] = RFcap;
	m_ReserveFundCurrent = RFcurrent;
	m_StartingReserve = RFcurrent;
	m_ReserveFundFreed = RFfreed;
	m_ReserveToInterest = RFtoInterest;
}
void ReserveFund::ResetReserve() {
	for (int i = 0; i < EnumEnd; i++)
		m_ReserveVects[i].SetVector("0");
	m_ReserveFundCurrent = 0.0;
	m_ReserveFundFreed = 0;
	m_StartingReserve = 0.0;
	m_ReserveFundFlows.Clear();
	m_ReserveToInterest = true;
}
void ReserveFund::FillMissingAnchors(const QDate& NewAnchor) {
	for (int i = 0; i < EnumEnd; i++)
		if (OriginallyNullAnchor[i]) m_ReserveVects[i].SetAnchorDate(NewAnchor);
}
void ReserveFund::ResetMissingAnchors() {
	for (int i = 0; i < EnumEnd; i++)
		if (OriginallyNullAnchor[i]) m_ReserveVects[i].RemoveAnchorDate();
}

