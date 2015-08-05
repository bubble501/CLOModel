#ifndef Tranche_h__
#define Tranche_h__
#include <QHash>
#include "TrancheCashFlow.h"
#include "BloombergVector.h"
#include "BackwardCompatibilityInterface.h"
#include "BaseRateVect.h"
class IntegerVector;
class Seniority;
class TranchePrivate;
class Ratings;
class CLOMODELLIB_EXPORT  Tranche : public BackwardInterface
{
    DECLARE_PUBLIC_COMMONS(Tranche)
    DECLARE_PUBLIC_COMMONS_COPY(Tranche)
protected:
	void GetRefRateValueFromBloomberg() const;
	void ClearInterest();
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
    void SetCouponPart(const QString& val, qint32 CoupIndex, QHash<qint32, T*>& vec) const
    {
        static_assert(std::is_base_of<AbstractBbgVect, T>::value, "SetCouponPart requires a hash of vectors");
        if(CoupIndex < 0 || CoupIndex >= (1 << MaximumInterestsTypes)) 
            return;
        auto iter = vec.find(CoupIndex);
        if (T(val).IsEmpty()) {
            //return; // #TODO check this
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
	~Tranche();
	const QHash<qint32, BloombergVector*>& GetRefRateValues() const;
	double GetStartingDeferredInterest(qint32 CoupIdx = 0) const;
	void SetStartingDeferredInterest(const double& val, qint32 CoupIdx=0);
	const QString& GetTrancheName() const;
	const QString& GetCurrency() const;
	double GetOriginalAmount() const;
	double GetOutstandingAmt() const;
	double GetBaseCurrencyOutsanding() const;
	double GetBaseCurrencyOriginal() const;
    QList<qint32> GetCouponIndexes() const;
    QList<qint32> GetReferenceRateIndexes()const;
    QList<qint32> GetDayCountsIndexes()const;
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
	double GetPrice() const;
	const QString& GetBloombergExtension() const;
	const Seniority& GetProrataGroup() const;
	qint32 GetProrataGroup(int SeliorityScaleLevel) const;
	const TrancheCashFlow& GetCashFlow() const;
	TrancheCashFlow& GetCashFlow();
	double GetMinOClevel() const;
	double GetMinIClevel() const;
	const QDate& GetLastPaymentDate() const;
    BaseRateVector GetDefaultRefRate() const { return GetReferenceRate(-1); }
	double GetExchangeRate() const;
	const IntegerVector& GetPaymentFrequency() const;
	const QSet<QString>& GetISIN() const;
    BloombergVector GetCouponVector(qint32 CoupIndex) const;
	void AddISIN(const QString& a);
    void ClearISIN();
    void SetTrancheName(const QString& a);
	void SetCurrency(const QString& a);
	void SetOriginalAmount(double a);
	void SetOutstandingAmt(double a);
	void SetCoupon(const QString& a, qint32 CoupIndex);
	void SetReferenceRate(const QString& a, qint32 CoupIndex);
	bool GetUseForwardCurve() const;
	void CompileReferenceRateValue(ConstantBaseRateTable& Values)const;
	void CompileReferenceRateValue(ForwardBaseRateTable& Values)const;
    void ClearRefRatesCache() const;
	bool HasCoupon(qint32 CoupIdx) const;
	void GetBaseRatesDatabase(ConstantBaseRateTable& Values, bool DownloadAll = false)const;
	void GetBaseRatesDatabase(ForwardBaseRateTable& Values, bool DownloadAll = false)const;
    DayCountVector GetDayCount(qint32 CoupIndex) const;
    void SetDayCount(QString val, qint32 CoupIndex);
	void SetPrice(double a);
	void SetBloombergExtension(const QString& a);
	void SetProrataGroup(const QString& a);
	void SetCashFlow(const TrancheCashFlow& a);
	void SetMinOClevel(double a);
	void SetMinIClevel(double a);
	void SetLastPaymentDate(const QDate& a);
	void SetDefaultRefRate(const QString& a);
	void SetExchangeRate(double a);
	void SetPaymentFrequency(const QString& a);
	void AddCashFlow(QDate Dte,double Amt, qint32 FlwTpe);
	void AddCashFlow(QDate Dte, double Amt, TrancheCashFlow::TrancheFlowType FlwTpe);
	void SetCashFlow(QDate Dte, double Amt, qint32 FlwTpe);
	void SetCashFlow(QDate Dte, double Amt, TrancheCashFlow::TrancheFlowType FlwTpe);
	void AddCashFlow(const TrancheCashFlow& a);
	double GetBondFactor() const;
	void GetDataFromBloomberg();
	void GetCashFlowsFromBloomberg(bool useFwdRates=false);
	double GetLossRate() const;
	double GetDiscountMargin() const;
	double GetDiscountMargin(double NewPrice) const;
	double GetPrice(double DiscountMargin) const;
	double GetIRR() const;
	double GetIRR(double NewPrice) const;
	double GetWALife(const QDate& StartDate) const;
	double GetWALife() const;
	const QDate& GetSettlementDate() const;
	void SetSettlementDate(const QDate& a);
    double getActualCoupon(const QDate& index,qint32 CouponIdx) const;
    double getTotalActualCoupon(const QDate& index) const;
    double getTotalActualCoupon(int index) const;
    double getActualCoupon(int index, qint32 CouponIdx) const;
    bool saveCashflowsDatabase() const;
    void getCashflowsDatabase();
    const Ratings& getRating() const;
    void setRating(const Ratings& val);
    void getRatingsBloomberg();
	friend CLOMODELLIB_EXPORT QDataStream& operator<<(QDataStream & stream, const Tranche& flows);
	friend CLOMODELLIB_EXPORT QDataStream& operator>>(QDataStream & stream, Tranche& flows);
};
CLOMODELLIB_EXPORT QDataStream& operator<<(QDataStream & stream, const Tranche& flows);
CLOMODELLIB_EXPORT QDataStream& operator>>(QDataStream & stream, Tranche& flows);
#endif // Tranche_h__