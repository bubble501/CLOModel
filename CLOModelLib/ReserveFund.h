#ifndef ReserveFund_h__
#define ReserveFund_h__

#include "BackwardCompatibilityInterface.h"
class ReserveFund;
class BloombergVector;
class GenericCashFlow;
class ReserveFund :public BackwardInterface {
    DECLARE_PUBLIC_COMMONS(ReserveFund)
    DECLARE_PUBLIC_COMMONS_COPY(ReserveFund)
public:
	enum class ReserveFlowsType : qint32{
		ReplenishFromInterest = 1 << MaximumInterestsTypes
		, ReplenishFromPrincipal =1
		, ShortFall = 1 << (MaximumInterestsTypes + 1)
	};

	const BloombergVector& GetReserveFundTarget()const;
	BloombergVector& GetReserveFundTarget();
	QString GetReserveFundTargetString()const;
	void SetReserveFundTarget(const QString& a);

	const BloombergVector& GetReserveFundMultiple()const;
	BloombergVector& GetReserveFundMultiple();
	QString GetReserveFundMultipleString()const;
	void SetReserveFundMultiple(const QString& a);

	const BloombergVector& GetReserveFundFloor()const;
	BloombergVector& GetReserveFundFloor();
	QString GetReserveFundFloorString()const;
	void SetReserveFundFloor(const QString& a);

	const BloombergVector& GetReserveFundCap() const;
	BloombergVector& GetReserveFundCap();
	QString GetReserveFundCapString()const;
	void SetReserveFundCap(const BloombergVector& val);

	double GetReserveFundCurrent()const;
	double GetReserveFundCurrent(const QDate& RepDate)const;
	double GetReserveFundCurrent(int a)const;
	void SetReserveFundCurrent(double a);

	double GetShortfall(const QDate& RepDate)const;
	double GetShortfall(int a)const;

	const double& GetStartingReserve() const;
	void SetStartingReserve(const double& val);

	int GetReserveFundFreed()const;
	void SetReserveFundFreed(int a);

	const GenericCashFlow& GetReserveFundFlow()const;
	GenericCashFlow& GetReserveFundFlow();
	void SetReserveFundFlow(const GenericCashFlow& source);

	bool GetReserveToInterest()const;
	void SetReserveToInterest(bool a);

	void SetReserveFund(const QString& RFtarget, const QString& RFmultiple, const QString& RFfloor, const QString& RFcap, double RFcurrent, int RFfreed, bool RFtoInterest = true);
	void ResetReserve();
	
	void FillMissingAnchors(const QDate& NewAnchor);
	void ResetMissingAnchors();

	void ClearFlows();

protected:
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
    enum VectorType
    {
        ResTarget = 0
        , ResMultiple
        , ResFloor
        , ResCap

        //Last step is invalid, used just for size
        , EnumEnd
    };
private:
	friend QDataStream& operator<<(QDataStream & stream, const ReserveFund& flows);
	friend QDataStream& operator>>(QDataStream & stream, ReserveFund& flows);
};
QDataStream& operator<<(QDataStream & stream, const ReserveFund& flows);
QDataStream& operator>>(QDataStream & stream, ReserveFund& flows);
#endif // ReserveFund_h__
