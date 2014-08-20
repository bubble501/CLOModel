#ifndef ReserveFund_h__
#define ReserveFund_h__

#include "BackwardCompatibilityInterface.h"
#include "BloombergVector.h"
#include "GenericCashFlow.h"
#include <QDataStream>
class ReserveFund :public BackwardInterface {
public:
	enum ReserveFlowsType{
		ReplenishFromInterest = 0
		, ReplenishFromPrincipal =1
		, ShortFall=13
	};
	ReserveFund();
	ReserveFund(const ReserveFund& a);
	ReserveFund& operator=(const ReserveFund& a);

	const BloombergVector& GetReserveFundTarget()const { return m_ReserveVects[ResTarget]; }
	BloombergVector& GetReserveFundTarget() { return m_ReserveVects[ResTarget]; }
	QString GetReserveFundTargetString()const { return m_ReserveVects[ResTarget].GetVector(); }
	void SetReserveFundTarget(const QString& a) { m_ReserveVects[ResTarget] = a; OriginallyNullAnchor[ResTarget] = m_ReserveVects[ResTarget].GetAnchorDate().isNull(); }

	const BloombergVector& GetReserveFundMultiple()const { return m_ReserveVects[ResMultiple]; }
	BloombergVector& GetReserveFundMultiple() { return m_ReserveVects[ResMultiple]; }
	QString GetReserveFundMultipleString()const { return m_ReserveVects[ResMultiple].GetVector(); }
	void SetReserveFundMultiple(const QString& a) { m_ReserveVects[ResMultiple] = a; OriginallyNullAnchor[ResMultiple] = m_ReserveVects[ResMultiple].GetAnchorDate().isNull(); }

	const BloombergVector& GetReserveFundFloor()const { return m_ReserveVects[ResFloor]; }
	BloombergVector& GetReserveFundFloor(){ return m_ReserveVects[ResFloor]; }
	QString GetReserveFundFloorString()const { return m_ReserveVects[ResFloor].GetVector(); }
	void SetReserveFundFloor(const QString& a) { m_ReserveVects[ResFloor] = a; OriginallyNullAnchor[ResFloor] = m_ReserveVects[ResFloor].GetAnchorDate().isNull(); }

	const BloombergVector& GetReserveFundCap() const { return m_ReserveVects[ResCap]; }
	BloombergVector& GetReserveFundCap() { return m_ReserveVects[ResCap]; }
	QString GetReserveFundCapString()const { return m_ReserveVects[ResCap].GetVector(); }
	void SetReserveFundCap(const BloombergVector& val) { m_ReserveVects[ResCap] = val; OriginallyNullAnchor[ResCap] = m_ReserveVects[ResCap].GetAnchorDate().isNull(); }

	double GetReserveFundCurrent()const { return m_ReserveFundCurrent; }
	double GetReserveFundCurrent(const QDate& RepDate)const { return m_ReserveFundFlows.GetFlow(RepDate, ReplenishFromInterest) + m_ReserveFundFlows.GetFlow(RepDate, ReplenishFromPrincipal); }
	double GetReserveFundCurrent(int a)const { return m_ReserveFundFlows.GetFlow(a, ReplenishFromInterest) + m_ReserveFundFlows.GetFlow(a, ReplenishFromPrincipal); }
	void SetReserveFundCurrent(double a) { m_ReserveFundCurrent = a; }

	double GetShortfall(const QDate& RepDate)const { return m_ReserveFundFlows.GetFlow(RepDate, ShortFall); }
	double GetShortfall(int a)const { return m_ReserveFundFlows.GetFlow(a, ShortFall); }

	const double& GetStartingReserve() const { return m_StartingReserve; }
	void SetStartingReserve(const double& val) { m_StartingReserve = val; }

	int GetReserveFundFreed()const { return m_ReserveFundFreed; }
	void SetReserveFundFreed(int a) { m_ReserveFundFreed = a; }

	const GenericCashFlow& GetReserveFundFlow()const { return m_ReserveFundFlows; }
	GenericCashFlow& GetReserveFundFlow() { return m_ReserveFundFlows; }
	void SetReserveFundFlow(const GenericCashFlow& source) { m_ReserveFundFlows = source; }

	bool GetReserveToInterest()const { return m_ReserveToInterest; }
	void SetReserveToInterest(bool a) { m_ReserveToInterest = a; }

	void SetReserveFund(const QString& RFtarget, const QString& RFmultiple, const QString& RFfloor, const QString& RFcap, double RFcurrent, int RFfreed, bool RFtoInterest = true);
	void ResetReserve();
	
	void FillMissingAnchors(const QDate& NewAnchor);
	void ResetMissingAnchors();

protected:
	virtual QDataStream& LoadOldVersion(QDataStream& stream);
private:
	enum VectorType {
		ResTarget
		,ResMultiple
		,ResFloor
		,ResCap

		//Last step is invalid, used just for size
		,EnumEnd
	};
	bool OriginallyNullAnchor[EnumEnd];
	BloombergVector m_ReserveVects[EnumEnd];
	double m_StartingReserve;
	double m_ReserveFundCurrent;
	int m_ReserveFundFreed;
	GenericCashFlow m_ReserveFundFlows;
	bool m_ReserveToInterest;
	friend QDataStream& operator<<(QDataStream & stream, const ReserveFund& flows);
	friend QDataStream& operator>>(QDataStream & stream, ReserveFund& flows);
};
QDataStream& operator<<(QDataStream & stream, const ReserveFund& flows);
QDataStream& operator>>(QDataStream & stream, ReserveFund& flows);
#endif // ReserveFund_h__
