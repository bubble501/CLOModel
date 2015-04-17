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
#include "DayCountVect.h"
#include "Seniority.h"
class  Tranche : public BackwardInterface {
private:
    QHash<qint32, DayCountVector*> m_DayCount;
	QString ISINcode;
	QString TrancheName;
	double OriginalAmt;
	QString Currency;
	double OutstandingAmt;
	QHash<qint32, BloombergVector*> Coupon;
	QHash<qint32, BaseRateVector*> ReferenceRate;
	mutable QHash<qint32, BloombergVector*> ReferenceRateValue;
	double Price;
	QString BloombergExtension;
	Seniority ProrataGroup;
	TrancheCashFlow CashFlow;
	double MinOClevel;
	double MinIClevel;
	QDate LastPaymentDate;
	double ExchangeRate;
	IntegerVector PaymentFrequency;
	QDate SettlementDate;
	mutable bool m_UseForwardCurve;
#ifndef NO_BLOOMBERG
	void GetRefRateValueFromBloomberg() const;
#endif
	void ClearInterest();
protected:
	void DownloadBaseRates() const;
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
	static bool AdjHolidays(DayCountConvention a);
    template<typename T>
    T GetCouponPart(qint32 CoupIndex, const QHash<qint32, T*>& vec) const
    {
        static_assert(std::is_base_of<AbstractBbgVect, T>::value, "GetCouponPart requires a hash of vectors");
        if (vec.contains(CoupIndex))
            return *(vec.value(CoupIndex));
        return T();
    }
    template<typename T>
    void SetCouponPart(const QString& val, qint32 CoupIndex, QHash<qint32, T*>& vec)
    {
        static_assert(std::is_base_of<AbstractBbgVect, T>::value, "SetCouponPart requires a hash of vectors");
        if(CoupIndex < 0 || CoupIndex >= (1 << MaximumInterestsTypes)) return;
        auto iter = vec.find(CoupIndex);
        if (T(val).IsEmpty()) {
            //return; //TODO check this
            if (iter != vec.end()) {
                delete iter.value();
                vec.erase(iter);
            }
        }
        if (iter != vec.end())
            iter.value()->operator=(val);
        else
            vec.insert(CoupIndex, new T(val));
    }
public:
	Tranche();
	Tranche(const Tranche& a);
	~Tranche();
	Tranche& operator=(const Tranche& a);
	const QHash<qint32, BloombergVector*>& GetRefRateValues() const { return ReferenceRateValue; }
	double GetStartingDeferredInterest(qint32 CoupIdx = static_cast<qint32>(TrancheCashFlow::TrancheFlowType::DeferredFlow)) const { return CashFlow.GetStartingDeferredInterest(CoupIdx); }
	void SetStartingDeferredInterest(const double& val, qint32 CoupIdx = static_cast<qint32>(TrancheCashFlow::TrancheFlowType::DeferredFlow)) { CashFlow.SetStartingDeferredInterest(val, CoupIdx); }
	const QString& GetTrancheName() const{return TrancheName;}
	const QString& GetCurrency() const{return Currency;}
	double GetOriginalAmount() const {return OriginalAmt;}
	double GetOutstandingAmt() const {return OutstandingAmt;}
	double GetBaseCurrencyOutsanding() const {return OutstandingAmt*ExchangeRate;}
	double GetBaseCurrencyOriginal() const {return OriginalAmt*ExchangeRate;}
    QList<qint32> GetCouponIndexes() const { return Coupon.keys(); }
    QList<qint32> GetReferenceRateIndexes()const { return ReferenceRate.keys(); }
    QList<qint32> GetDayCountsIndexes()const { return m_DayCount.keys(); }
	double GetTotalCoupon(const QDate& index, int Frequency = 12) const;
	double GetTotalCoupon(int index, int Frequency = 12) const;
	double GetCoupon(const QDate& index, qint32 CoupIndex /*= 0*/, int Frequency = 12) const;
	double GetCoupon(int index, qint32 CoupIndex /*= 0*/, int Frequency = 12) const;
	double GetRawCoupon(int index, qint32 CoupIndex /*= 0*/, int Frequency = 12) const;
	double GetRawCoupon(const QDate& index, qint32 CoupIndex /*= 0*/, int Frequency = 12) const;
	BaseRateVector GetReferenceRate(qint32 CoupIndex/*=0*/) const;
    BloombergVector GetReferenceRateValue(qint32 CoupIndex = 0) const;
	double GetReferenceRateValue(int index, qint32 CoupIndex /*= 0*/) const;
	double GetReferenceRateValue(const QDate& index, qint32 CoupIndex /*= 0*/) const;
	double GetPrice() const {return Price;}
	const QString& GetBloombergExtension() const{return BloombergExtension;}
	const Seniority& GetProrataGroup() const{return ProrataGroup;}
	quint32 GetProrataGroup(int SeliorityScaleLevel) const { return ProrataGroup.GetSeniority(SeliorityScaleLevel); }
	const TrancheCashFlow& GetCashFlow() const {return CashFlow;}
	TrancheCashFlow& GetCashFlow() {return CashFlow;}
	double GetMinOClevel() const {return MinOClevel;}
	double GetMinIClevel() const {return MinIClevel;}
	const QDate& GetLastPaymentDate() const {return LastPaymentDate;}
    BaseRateVector GetDefaultRefRate() const { return GetReferenceRate(-1); }
	double GetExchangeRate() const {return ExchangeRate;}
	const IntegerVector& GetPaymentFrequency() const{return PaymentFrequency;}
	const QString& GetISIN() const {return ISINcode;}
    BloombergVector GetCouponVector(qint32 CoupIndex) const;
	void SetISIN(const QString& a){ISINcode=a;}
	void SetTrancheName(const QString& a){TrancheName=a;}
	void SetCurrency(const QString& a){Currency=a;}
	void SetOriginalAmount(double a);
	void SetOutstandingAmt(double a);
	void SetCoupon(const QString& a, qint32 CoupIndex);
	void SetReferenceRate(const QString& a, qint32 CoupIndex);
	bool GetUseForwardCurve() const { return m_UseForwardCurve; }
	void CompileReferenceRateValue(ConstantBaseRateTable& Values)const;
	void CompileReferenceRateValue(ForwardBaseRateTable& Values)const;
	bool HasCoupon(qint32 CoupIdx) const;
#ifndef NO_DATABASE
	void GetBaseRatesDatabase(ConstantBaseRateTable& Values, bool DownloadAll = false)const;
	void GetBaseRatesDatabase(ForwardBaseRateTable& Values, bool DownloadAll = false)const;
#endif
    DayCountVector GetDayCount(qint32 CoupIndex) const;
    void SetDayCount(QString val, qint32 CoupIndex);
	void SetPrice(double a){if(a>0) Price=a;}
	void SetBloombergExtension(const QString& a);
	void SetProrataGroup(const QString& a){ProrataGroup.SetSeniorityScale(a);}
	void SetCashFlow(const TrancheCashFlow& a){CashFlow=a;}
	void SetMinOClevel(double a){if(a>0.0 || a==-1.0) MinOClevel=a;}
	void SetMinIClevel(double a){if(a>0.0 || a==-1.0) MinIClevel=a;}
	void SetLastPaymentDate(const QDate& a){LastPaymentDate=a;}
	void SetDefaultRefRate(const QString& a);
	void SetExchangeRate(double a);
	void SetPaymentFrequency(const QString& a);
	void AddCashFlow(QDate Dte,double Amt, qint32 FlwTpe){CashFlow.AddFlow(Dte,Amt,FlwTpe);}
	void AddCashFlow(QDate Dte, double Amt, TrancheCashFlow::TrancheFlowType FlwTpe) { CashFlow.AddFlow(Dte, Amt, FlwTpe); }
	void SetCashFlow(QDate Dte, double Amt, qint32 FlwTpe) { CashFlow.SetFlow(Dte, Amt, FlwTpe); }
	void SetCashFlow(QDate Dte, double Amt, TrancheCashFlow::TrancheFlowType FlwTpe) { CashFlow.SetFlow(Dte, Amt, FlwTpe); }
	void AddCashFlow(const TrancheCashFlow& a){CashFlow.AddFlow(a);}
	double GetBondFactor() const {return OutstandingAmt/OriginalAmt;}
#ifndef NO_BLOOMBERG
	void GetDataFromBloomberg();
	void GetCashFlowsFromBloomberg();
#endif
	double GetLossRate() const;
	double GetDiscountMargin() const;
	double GetDiscountMargin(double NewPrice) const;
	double GetPrice(double DiscountMargin) const;
	double GetIRR() const;
	double GetIRR(double NewPrice) const;
	double GetWALife(const QDate& StartDate) const;
	double GetWALife() const { return GetWALife(SettlementDate); }
	const QDate& GetSettlementDate() const{return SettlementDate;}
	void SetSettlementDate(const QDate& a){SettlementDate=a;}
	friend QDataStream& operator<<(QDataStream & stream, const Tranche& flows);
	friend QDataStream& operator>>(QDataStream & stream, Tranche& flows);
};
QDataStream& operator<<(QDataStream & stream, const Tranche& flows);
QDataStream& operator>>(QDataStream & stream, Tranche& flows);
#endif // Tranche_h__