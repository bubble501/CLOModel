#ifndef Tranche_h__
#define Tranche_h__
#include <QString>
#include <QObject>
#include "TrancheCashFlow.h"
#include "BloombergVector.h"
#include "BackwardCompatibilityInterface.h"
#include "BaseRateVect.h"
#include "IntegerVector.h"
#include <QHash>
#include "BaseRateTable.h"
class  Tranche : public BackwardInterface {
public:
	//! Enum defining what type of coupon the tranche is paying
	enum TrancheInterestType{
		InvalidType=-1,
		FixedInterest, /*!< Fixed Coupon*/
		FloatingInterest /*!< Floating Rate Coupon*/
	};
private:
	DayCountConvention m_DayCount;
	QString ISINcode;
	QString TrancheName;
	double OriginalAmt;
	QString Currency;
	double OutstandingAmt;
	QHash<qint32,TrancheInterestType> InterestType;
	QHash<qint32, BloombergVector*> Coupon;
	QHash<qint32, BaseRateVector*> ReferenceRate;
	mutable QHash<qint32, BloombergVector*> ReferenceRateValue;
	double Price;
	QString BloombergExtension;
	int ProrataGroup;
	TrancheCashFlow CashFlow;
	double MinOClevel;
	double MinIClevel;
	QDate LastPaymentDate;
	double ExchangeRate;
	IntegerVector PaymentFrequency;
	QDate SettlementDate;
	double AccruedInterest;
	mutable bool m_UseForwardCurve;
#ifndef NO_BLOOMBERG
	void GetRefRateValueFromBloomberg() const;
#endif
	void ClearInterest();
	void DownloadBaseRates() const;
protected:
	virtual QDataStream& LoadOldVersion(QDataStream& stream);
public:
	Tranche();
	Tranche(const Tranche& a);
	~Tranche();
	Tranche& operator=(const Tranche& a);
	bool operator<(const Tranche& a)const;
	bool operator>(const Tranche& a)const;
	const double& GetStartingDeferredInterest() const { return CashFlow.GetStartingDeferredInterest(); }
	void SetStartingDeferredInterest(const double& val) { CashFlow.SetStartingDeferredInterest(val); }
	const QString& GetTrancheName() const{return TrancheName;}
	const QString& GetCurrency() const{return Currency;}
	double GetOriginalAmount() const {return OriginalAmt;}
	double GetOutstandingAmt() const {return OutstandingAmt;}
	double GetBaseCurrencyOutsanding() const {return OutstandingAmt*ExchangeRate;}
	double GetBaseCurrencyOriginal() const {return OriginalAmt*ExchangeRate;}
	QList<qint32> GetCouponIndexes() { return Coupon.keys(); }
	QList<qint32> GetInterestTypeIndexes() { return InterestType.keys(); }
	QList<qint32> GetReferenceRateIndexes() { return ReferenceRate.keys(); }
	TrancheInterestType GetInterestType(qint32 CoupIndex /*= 0*/) const { return InterestType.value(CoupIndex, InvalidType); }
	double GetTotalCoupon(const QDate& index, int Frequency = 12) const;
	double GetTotalCoupon(int index, int Frequency = 12) const;
	double GetCoupon(const QDate& index, qint32 CoupIndex /*= 0*/, int Frequency = 12) const;
	double GetCoupon(int index, qint32 CoupIndex /*= 0*/, int Frequency = 12) const;
	double GetRawCoupon(int index, qint32 CoupIndex /*= 0*/, int Frequency = 12) const;
	double GetRawCoupon(const QDate& index, qint32 CoupIndex /*= 0*/, int Frequency = 12) const;
	QString GetReferenceRate(qint32 CoupIndex/*=0*/) const;
	QString GetReferenceRateValue(qint32 CoupIndex=0) const;
	double GetReferenceRateValue(int index, qint32 CoupIndex /*= 0*/) const;
	double GetReferenceRateValue(const QDate& index, qint32 CoupIndex /*= 0*/) const;
	double GetPrice() const {return Price;}
	const QString& GetBloombergExtension() const{return BloombergExtension;}
	int GetProrataGroup() const{return ProrataGroup;}
	const TrancheCashFlow& GetCashFlow() const {return CashFlow;}
	TrancheCashFlow& GetCashFlow() {return CashFlow;}
	double GetMinOClevel() const {return MinOClevel;}
	double GetMinIClevel() const {return MinIClevel;}
	const QDate& GetLastPaymentDate() const {return LastPaymentDate;}
	QString GetDefaultRefRate() const { return GetReferenceRate(-1); }
	double GetExchangeRate() const {return ExchangeRate;}
	QString GetPaymentFrequency() const{return PaymentFrequency.GetVector();}
	const QString& GetISIN() const {return ISINcode;}
	QString GetCouponVector(qint32 CoupIndex /*= 0*/) const;
	void SetISIN(const QString& a){ISINcode=a;}
	void SetTrancheName(const QString& a){TrancheName=a;}
	void SetCurrency(const QString& a){Currency=a;}
	void SetOriginalAmount(double a);
	void SetOutstandingAmt(double a);
	void SetInterestType(TrancheInterestType a, qint32 CoupIndex /*= 0*/);
	void SetCoupon(const QString& a, qint32 CoupIndex /*= 0*/);
	void SetReferenceRate(const QString& a, qint32 CoupIndex /*= 0*/);
	bool GetUseForwardCurve() const { return m_UseForwardCurve; }
	void CompileReferenceRateValue(ConstantBaseRateTable& Values)const;
	void CompileReferenceRateValue(ForwardBaseRateTable& Values)const;
#ifndef NO_DATABASE
	void GetBaseRatesDatabase(ConstantBaseRateTable& Values, bool DownloadAll=false)const;
	void GetBaseRatesDatabase(ForwardBaseRateTable& Values, bool DownloadAll=false)const;
#endif
	DayCountConvention GetDayCount() const { return m_DayCount; }
	void SetDayCount(DayCountConvention val) { m_DayCount = val; }
	void SetPrice(double a){if(a>0) Price=a;}
	void SetBloombergExtension(const QString& a);
	void SetProrataGroup(int a){if(a>0) ProrataGroup=a;}
	void SetCashFlow(const TrancheCashFlow& a){CashFlow=a;}
	void SetMinOClevel(double a){if(a>0.0 || a==-1.0) MinOClevel=a;}
	void SetMinIClevel(double a){if(a>0.0 || a==-1.0) MinIClevel=a;}
	void SetLastPaymentDate(const QDate& a){LastPaymentDate=a;}
	void SetDefaultRefRate(const QString& a);
	void SetExchangeRate(double a);
	void SetPaymentFrequency(const QString& a);
	void AddCashFlow(QDate Dte,double Amt, qint32 FlwTpe){CashFlow.AddFlow(Dte,Amt,FlwTpe);}
	void AddCashFlow(QDate Dte, double Amt, TrancheCashFlow::TrancheFlowType FlwTpe) { AddCashFlow(Dte, Amt, static_cast<qint32>(FlwTpe)); }
	void AddCashFlow(const TrancheCashFlow& a){CashFlow.AddFlow(a);}
	double GetBondFactor() const {return OutstandingAmt/OriginalAmt;}
#ifndef NO_BLOOMBERG
	void GetDataFromBloomberg();
#endif
	double GetLossRate() const;
	double GetDiscountMargin() const;
	double GetDiscountMargin(double NewPrice) const;
	double GetPrice(double DiscountMargin) const;
	double GetIRR() const;
	double GetIRR(double NewPrice) const;
	double GetCurrentOutstanding() const;
	double GetWALife(const QDate& StartDate) const;
	double GetAccruedInterest() const{return AccruedInterest;}
	const QDate& GetSettlementDate() const{return SettlementDate;}
	void SetAccruedInterest(double a) {if(a>=0.0) AccruedInterest=a;}
	void SetSettlementDate(const QDate& a){SettlementDate=a;}
	friend QDataStream& operator<<(QDataStream & stream, const Tranche& flows);
	friend QDataStream& operator>>(QDataStream & stream, Tranche& flows);
};
QDataStream& operator<<(QDataStream & stream, const Tranche& flows);
QDataStream& operator>>(QDataStream & stream, Tranche& flows);
#endif // Tranche_h__