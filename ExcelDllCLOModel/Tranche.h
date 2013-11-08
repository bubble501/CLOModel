#ifndef Tranche_h__
#define Tranche_h__

#include <QString>
#include <QObject>
#include "TrancheCashFlow.h"
class Tranche {
public:
	enum TrancheInterestType{
		FixedInterest,
		FloatingInterest
	};
private:
	QString TrancheName;
	double OriginalAmt;
	QString Currency;
	double OutstandingAmt;
	TrancheInterestType InterestType;
	double Coupon;
	QString ReferenceRate;
	mutable double ReferenceRateValue;
	double Price;
	QString BloombergExtension;
	int ProrataGroup;
	TrancheCashFlow CashFlow;
	double MinOClevel;
	double MinIClevel;
	QDate LastPaymentDate;
	int DayCount;
	QString DefaultRefRate;
	double ExchangeRate;
	int PaymentFrequency;
	QDate SettlementDate;
	double AccruedInterest;
public:
	Tranche();
	Tranche(const Tranche& a);
	Tranche& operator=(const Tranche& a);
	bool operator<(const Tranche& a)const;
	bool operator>(const Tranche& a)const;
	const QString& GetTrancheName() const{return TrancheName;}
	const QString& GetCurrency() const{return Currency;}
	double GetOriginalAmount() const {return OriginalAmt;}
	double GetOutstandingAmt() const {return OutstandingAmt;}
	double GetBaseCurrencyOutsanding() const {return OutstandingAmt*ExchangeRate;}
	double GetBaseCurrencyOriginal() const {return OriginalAmt*ExchangeRate;}
	TrancheInterestType GetInterestType() const {return InterestType;}
	double GetCoupon() const;
	double GetRawCoupon() const{return Coupon;}
	const QString& GetReferenceRate() const{return ReferenceRate;}
	double GetReferenceRateValue() const {return ReferenceRateValue;}
	double GetPrice() const {return Price;}
	const QString& GetBloombergExtension() const{return BloombergExtension;}
	int GetProrataGroup() const{return ProrataGroup;}
	const TrancheCashFlow& GetCashFlow() const {return CashFlow;}
	TrancheCashFlow& GetCashFlow() {return CashFlow;}
	double GetMinOClevel() const {return MinOClevel;}
	double GetMinIClevel() const {return MinIClevel;}
	const QDate& GetLastPaymentDate() const {return LastPaymentDate;}
	int GetDayCount() const{return DayCount;}
	const QString& GetDefaultRefRate() const{return DefaultRefRate;}
	double GetExchangeRate() const {return ExchangeRate;}
	int GetPaymentFrequency() const{return PaymentFrequency;}
	void SetTrancheName(const QString& a){TrancheName=a;}
	void SetCurrency(const QString& a){Currency=a;}
	void SetOriginalAmount(double a);
	void SetOutstandingAmt(double a);
	void SetInterestType(TrancheInterestType a){InterestType=a;}
	void SetCoupon(double a){Coupon=a;}
	void SetReferenceRate(const QString& a){ReferenceRate=a;}
	void SetReferenceRateValue(double a){ReferenceRateValue=a;}
	void SetPrice(double a){if(a>0) Price=a;}
	void SetBloombergExtension(const QString& a);
	void SetProrataGroup(int a){if(a>0) ProrataGroup=a;}
	void SetCashFlow(const TrancheCashFlow& a){CashFlow=a;}
	void SetMinOClevel(double a){if(a>0.0 || a==-1.0) MinOClevel=a;}
	void SetMinIClevel(double a){if(a>0.0 || a==-1.0) MinIClevel=a;}
	void SetLastPaymentDate(const QDate& a){LastPaymentDate=a;}
	void SetDayCount(int a){DayCount=a;}
	void SetDefaultRefRate(const QString& a){DefaultRefRate=a;}
	void SetExchangeRate(double a);
	void SetPaymentFrequency(int a){if(a>1) PaymentFrequency=a;}
	void AddCashFlow(QDate Dte,double Amt, TrancheCashFlow::ThrancheFlowType FlwTpe){CashFlow.AddFlow(Dte,Amt,FlwTpe);}
	void AddCashFlow(const TrancheCashFlow& a){CashFlow.AddFlow(a);}
	double GetBondFactor() const {return OutstandingAmt/OriginalAmt;}
	void GetDataFromBloomberg();
	double GetLossRate() const;
	double GetDiscountMargin() const;
	double GetDiscountMargin(double NewPrice) const;
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