#include "IRSwap.h"
#include <QDataStream>
QString IRSwap::ReadyToCalculate() const {
	QString Result("");
	if (m_FixedLegRate.IsEmpty()) Result += "Swap Fixed Leg rate\n";
	if (m_FloatingBase.IsEmpty()) Result += "Swap Floating Leg Base Rate\n";
	if (m_FloatingSpread.IsEmpty()) Result += "Swap Floating Leg Margin\n";
	return Result;
}
void IRSwap::SetMissingAnchors(const QDate& a) {
	if (!m_FixedLegRate.IsEmpty() && m_FixedLegRate.GetAnchorDate().isNull()) m_FixedLegRate.SetAnchorDate(a);
	if (!m_FloatingBase.IsEmpty() && m_FixedLegRate.GetAnchorDate().isNull()) m_FixedLegRate.SetAnchorDate(a);
	if (!m_FloatingSpread.IsEmpty() && m_FixedLegRate.GetAnchorDate().isNull()) m_FixedLegRate.SetAnchorDate(a);
}
IRSwap::IRSwap(const IRSwap& a) 
	:m_FixedLegRate(a.m_FixedLegRate)
	, m_FloatingBase(a.m_FloatingBase)
	, m_FloatingSpread(a.m_FloatingSpread)
	, m_FixedToFloating(a.m_FixedToFloating)
	, m_LastPay(a.m_LastPay)
	, m_LastReceive(a.m_LastReceive)
	, m_DayCount(a.m_DayCount)
	, m_FloatingBaseValue(a.m_FloatingBaseValue)
	, m_UseForwardCurves(a.m_UseForwardCurves)
{}

IRSwap::IRSwap() 
	:m_FixedToFloating(true) 
	, m_LastPay(0.0)
	, m_LastReceive(0.0)
	, m_DayCount(DayCountConvention::ACT360)
	, m_UseForwardCurves(false)
{}
IRSwap& IRSwap::operator=(const IRSwap& a) {
	m_FixedLegRate = a.m_FixedLegRate;
	m_FloatingBase = a.m_FloatingBase;
	m_FloatingSpread = a.m_FloatingSpread;
	m_FixedToFloating = a.m_FixedToFloating;
	m_LastPay = a.m_LastPay;
	m_LastReceive=a.m_LastReceive;
	m_DayCount = a.m_DayCount;
	m_FloatingBaseValue = a.m_FloatingBaseValue;
	m_UseForwardCurves = a.m_UseForwardCurves;
	return *this;
}

double IRSwap::CalculateFlow(double Notional, const QDate& StartAccrueDate, const QDate& EndAccrueDate) {
	//TODO Libor fixing at what date?
	if (m_FloatingBaseValue.IsEmpty()) {
#ifndef NO_DATABASE
		if (m_UseForwardCurves)
			m_FloatingBaseValue = m_FloatingBase.GetBaseRatesDatabase(ForwardBaseRateTable());
		else
			m_FloatingBaseValue = m_FloatingBase.GetBaseRatesDatabase(ConstantBaseRateTable());
#else
#ifndef NO_BLOOMBERG
		m_FloatingBaseValue = m_FloatingBase.GetRefRateValueFromBloomberg();
#else
		return 0.0;
#endif
#endif 
	}
	m_LastPay = AdjustCoupon(m_FixedLegRate.GetValue(StartAccrueDate), StartAccrueDate, EndAccrueDate, m_DayCount)*Notional;
	m_LastReceive = AdjustCoupon(m_FloatingSpread.GetValue(StartAccrueDate)+m_FloatingBaseValue.GetValue(StartAccrueDate), StartAccrueDate, EndAccrueDate, m_DayCount)*Notional;
	if (!m_FixedToFloating) {
		double tempD = m_LastPay;
		m_LastPay = m_LastReceive;
		m_LastReceive = tempD;
	}
	return GetLastNetFlow();
}

QDataStream& IRSwap::LoadOldVersion(QDataStream& stream) {
	stream >> m_UseForwardCurves;
	m_FixedLegRate.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_FixedLegRate;
	m_FloatingBase.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_FloatingBase;
	m_FloatingBaseValue.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_FloatingBaseValue;
	m_FloatingSpread.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_FloatingSpread;
	qint32 tempInt;
	stream >> tempInt; m_DayCount = static_cast<DayCountConvention>(tempInt);
	stream >> m_FixedToFloating;
	stream >> m_LastPay;
	stream >> m_LastReceive;
	ResetProtocolVersion();
	return stream;
}

QDataStream& operator<<(QDataStream & stream, const IRSwap& flows) {
	stream << flows.m_UseForwardCurves
		<< flows.m_FixedLegRate
		<< flows.m_FloatingBase
		<< flows.m_FloatingBaseValue
		<< flows.m_FloatingSpread
		<< qint32(flows.m_DayCount)
		<< flows.m_FixedToFloating
		<< flows.m_LastPay
		<< flows.m_LastReceive
		;
	return stream;
}
QDataStream& operator>>(QDataStream & stream, IRSwap& flows) { return flows.LoadOldVersion(stream); }
