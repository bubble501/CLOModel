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
#include <boost/math/tools/roots.hpp>
#include "BaseRateTable.h"
class  Tranche : public BackwardInterface {
public:
	//! Enum defining what type of coupon the tranche is paying
	enum TrancheInterestType{
		FixedInterest, /*!< Fixed Coupon*/
		FloatingInterest /*!< Floating Rate Coupon*/
	};
private:
	mutable double m_optimObjective;
	QString ISINcode;
	QString TrancheName;
	double OriginalAmt;
	QString Currency;
	double OutstandingAmt;
	TrancheInterestType InterestType;
	BloombergVector Coupon;
	BaseRateVector ReferenceRate;
	mutable BloombergVector ReferenceRateValue;
	double Price;
	QString BloombergExtension;
	int ProrataGroup;
	TrancheCashFlow CashFlow;
	double MinOClevel;
	double MinIClevel;
	QDate LastPaymentDate;
	int DayCount;
	BaseRateVector DefaultRefRate;
	double ExchangeRate;
	IntegerVector PaymentFrequency;
	QDate SettlementDate;
	double AccruedInterest;
	
#ifndef NO_BLOOMBERG
	void GetRefRateValueFromBloomberg() const;
#endif
	double operator()(double Price) const { 
		return GetDiscountMargin(Price) - m_optimObjective; 
	} //Used to calculate Price given DM
protected:
	virtual QDataStream& LoadOldVersion(QDataStream& stream);
public:
	Tranche();
	Tranche(const Tranche& a);
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
	TrancheInterestType GetInterestType() const {return InterestType;}
	double GetCoupon(const QDate& index,int Frequency=12) const;
	double GetCoupon(int index = 0, int Frequency = 12) const;
	double GetRawCoupon(int index=0) const;
	double GetRawCoupon(const QDate& index) const;
	QString GetReferenceRate() const{return ReferenceRate.GetVector();}
	const BloombergVector& GetReferenceRateValue() const {return ReferenceRateValue;}
	double GetReferenceRateValue(int index) const {return ReferenceRateValue.GetValue(index);}
	double GetReferenceRateValue(const QDate& index) const {return ReferenceRateValue.GetValue(index);}
	double GetPrice() const {return Price;}
	const QString& GetBloombergExtension() const{return BloombergExtension;}
	int GetProrataGroup() const{return ProrataGroup;}
	const TrancheCashFlow& GetCashFlow() const {return CashFlow;}
	TrancheCashFlow& GetCashFlow() {return CashFlow;}
	double GetMinOClevel() const {return MinOClevel;}
	double GetMinIClevel() const {return MinIClevel;}
	const QDate& GetLastPaymentDate() const {return LastPaymentDate;}
	int GetDayCount() const{return DayCount;}
	QString GetDefaultRefRate() const{return DefaultRefRate.GetVector();}
	double GetExchangeRate() const {return ExchangeRate;}
	QString GetPaymentFrequency() const{return PaymentFrequency.GetVector();}
	const QString& GetISIN() const {return ISINcode;}
	QString GetCouponVector() const{return Coupon.GetVector();}
	void SetISIN(const QString& a){ISINcode=a;}
	void SetTrancheName(const QString& a){TrancheName=a;}
	void SetCurrency(const QString& a){Currency=a;}
	void SetOriginalAmount(double a);
	void SetOutstandingAmt(double a);
	void SetInterestType(TrancheInterestType a){InterestType=a;}
	void SetCoupon(const QString& a){Coupon=a;}
	void SetReferenceRate(const QString& a){ReferenceRate=a;}
	void SetReferenceRateValue(const QString& a){ReferenceRateValue=a;}
	void SetReferenceRateValue(const BloombergVector& a){ReferenceRateValue=a;}
	void CompileReferenceRateValue(ConstantBaseRateTable& Values)const;
	void CompileReferenceRateValue(ForwardBaseRateTable& Values)const;
#ifndef NO_DATABASE
	void GetBaseRatesDatabase(ConstantBaseRateTable& Values)const;
	void GetBaseRatesDatabase(ForwardBaseRateTable& Values)const;
#endif
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
	void SetPaymentFrequency(const QString& a);
	void AddCashFlow(QDate Dte,double Amt, TrancheCashFlow::TrancheFlowType FlwTpe){CashFlow.AddFlow(Dte,Amt,FlwTpe);}
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
	template <class F, class T> friend void  boost::math::tools::detail::bracket(F f, T& a, T& b, T c, T& fa, T& fb, T& d, T& fd);
	template <class F, class T, class Tol, class Policy> friend std::pair<T, T> boost::math::tools::bracket_and_solve_root(F f, const T& guess, T factor, bool rising, Tol tol, boost::uintmax_t& max_iter, const Policy& pol);
};
QDataStream& operator<<(QDataStream & stream, const Tranche& flows);
QDataStream& operator>>(QDataStream & stream, Tranche& flows);
#endif // Tranche_h__