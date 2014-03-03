#ifndef Tranche_h__
#define Tranche_h__

#include <QString>
#include <TrancheCashFlow.h>
class Tranche {
public:
	//! Enum defining what type of coupon the tranche is paying
	enum TrancheInterestType{
		FixedInterest, /*!< Fixed Coupon*/
		FloatingInterest /*!< Floating Rate Coupon*/
	};
	Tranche();
	Tranche(const Tranche& a);
	Tranche& operator=(const Tranche& a);
	bool operator<(const Tranche& a)const;
	bool operator>(const Tranche& a)const;
	const QString& GetTrancheName() const;
	const QString& GetCurrency() const;
	double GetOriginalAmount() const;
	double GetOutstandingAmt() const;
	double GetBaseCurrencyOutsanding() const;
	double GetBaseCurrencyOriginal() const;
	TrancheInterestType GetInterestType() const;
	double GetCoupon() const;
	double GetRawCoupon() const;
	const QString& GetReferenceRate() const;
	double GetReferenceRateValue() const;
	double GetPrice() const;
	const QString& GetBloombergExtension() const;
	int GetProrataGroup() const;
	const TrancheCashFlow& GetCashFlow() const;
	TrancheCashFlow& GetCashFlow();
	double GetMinOClevel() const;
	double GetMinIClevel() const;
	const QDate& GetLastPaymentDate() const;
	int GetDayCount() const;
	const QString& GetDefaultRefRate() const;
	double GetExchangeRate() const;
	int GetPaymentFrequency() const;
	void SetTrancheName(const QString& a);
	void SetCurrency(const QString& a);
	void SetOriginalAmount(double a);
	void SetOutstandingAmt(double a);
	void SetInterestType(TrancheInterestType a);
	void SetCoupon(double a);
	void SetReferenceRate(const QString& a);
	void SetReferenceRateValue(double a);
	void SetPrice(double a);
	void SetBloombergExtension(const QString& a);
	void SetProrataGroup(int a);
	void SetCashFlow(const TrancheCashFlow& a);
	void SetMinOClevel(double a);
	void SetMinIClevel(double a);
	void SetLastPaymentDate(const QDate& a);
	void SetDayCount(int a);
	void SetDefaultRefRate(const QString& a);
	void SetExchangeRate(double a);
	void SetPaymentFrequency(int a);
	void AddCashFlow(QDate Dte,double Amt, TrancheCashFlow::ThrancheFlowType FlwTpe);
	void AddCashFlow(const TrancheCashFlow& a);
	double GetBondFactor() const;
	void GetDataFromBloomberg();
	double GetLossRate() const;
	double GetDiscountMargin() const;
	double GetDiscountMargin(double NewPrice) const;
	double GetCurrentOutstanding() const;
	double GetWALife(const QDate& StartDate) const;
	double GetAccruedInterest() const;
	const QDate& GetSettlementDate() const;
	void SetAccruedInterest(double a);
	void SetSettlementDate(const QDate& a);
};
#endif // Tranche_h__