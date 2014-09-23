#ifndef IRSwap_h__
#define IRSwap_h__
#include "BackwardCompatibilityInterface.h"
#include "BloombergVector.h"
#include "BaseRateVect.h"
#include "DayCountVect.h"
class IRSwap : public BackwardInterface {
public:
	IRSwap();
	IRSwap(const IRSwap& a);
	IRSwap& operator=(const IRSwap& a);
	void SetMissingAnchors(const QDate& a);
	void SetFixedToFloating() { m_FixedToFloating = true; }
	void SetFloatingToFixed() { m_FixedToFloating = false; }
	bool IsFixedToFloating() const { return m_FixedToFloating; }
	bool IsFloatingToFixed() const { return !m_FixedToFloating; }
	QString GetFixedLegRate() const { return m_FixedLegRate.GetVector(); }
	QString GetFloatingBase() const { return m_FloatingBase.GetVector(); }
	QString GetFloatingSpread() const { return m_FloatingSpread.GetVector(); }
	void SetFixedLegRate(const QString& val) { m_FixedLegRate = val; }
	void SetFloatingSpread(const QString& val) { m_FloatingSpread = val; }
	void SetFloatingBase(const QString& val) { m_FloatingBase = val; }
	QString ReadyToCalculate() const;
	double GetLastReceive() const { return m_LastReceive; }
	double GetLastPay() const { return m_LastPay; }
	double GetLastNetFlow() const { return m_LastReceive - m_LastPay; }
	const DayCountVector& GetDayCount() const { return m_DayCount; }
	void SetDayCount(const QString& val) { m_DayCount = val; }
	double CalculateFlow(double Notional, const QDate& StartAccrueDate, const QDate& EndAccrueDate);
	bool GetUseForwardCurves() const { return m_UseForwardCurves; }
	void SetUseForwardCurves( bool val) { m_UseForwardCurves = val; }
	void CompileBaseRates(ConstantBaseRateTable& a) { m_UseForwardCurves = false; m_FloatingBaseValue = m_FloatingBase.CompileReferenceRateValue(a); }
	void CompileBaseRates(ForwardBaseRateTable& a) { m_UseForwardCurves = true; m_FloatingBaseValue = m_FloatingBase.CompileReferenceRateValue(a); }
#ifndef NO_DATABASE
	void GetBaseRatesDatabase(ForwardBaseRateTable& a) { m_UseForwardCurves = true; m_FloatingBaseValue = m_FloatingBase.GetBaseRatesDatabase(a); }
	void GetBaseRatesDatabase(ConstantBaseRateTable& a) { m_UseForwardCurves = false; m_FloatingBaseValue = m_FloatingBase.GetBaseRatesDatabase(a); }
#endif
#ifndef NO_BLOOMBERG
	void GetRefRateValueFromBloomberg(ForwardBaseRateTable& a) { m_UseForwardCurves = true; m_FloatingBaseValue = m_FloatingBase.GetRefRateValueFromBloomberg(a); }
	void GetRefRateValueFromBloomberg(ConstantBaseRateTable& a) { m_UseForwardCurves = false; m_FloatingBaseValue = m_FloatingBase.GetRefRateValueFromBloomberg(a); }
#endif
protected:
	bool m_UseForwardCurves;
	BloombergVector m_FixedLegRate;
	BaseRateVector m_FloatingBase;
	BloombergVector m_FloatingBaseValue;
	BloombergVector m_FloatingSpread;
	DayCountVector m_DayCount;
	bool m_FixedToFloating;
	double m_LastPay;
	double m_LastReceive;
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
	friend QDataStream& operator<<(QDataStream & stream, const IRSwap& flows);
	friend QDataStream& operator>>(QDataStream & stream, IRSwap& flows);
};
QDataStream& operator<<(QDataStream & stream, const IRSwap& flows);
QDataStream& operator>>(QDataStream & stream, IRSwap& flows);
#endif // IRSwap_h__