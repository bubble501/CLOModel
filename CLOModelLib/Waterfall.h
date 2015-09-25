#ifndef Waterfall_h__
#define Waterfall_h__
#include "BackwardCompatibilityInterface.h"
#include <QMetaType>
#include <QString>
#include <QSet>
#include <memory>
class MtgCashFlow;
class GenericCashFlow;
class AssumptionSet;
class WaterfallPrivate;
class ReinvestmentTest;
class WatFalPrior;
class ForwardBaseRateTable;
class ConstantBaseRateTable;
class ReserveFund;
class WaterfallPrivate;
class TriggersResults;
class AbstractTrigger;
class Tranche;
class CLOMODELLIB_EXPORT Waterfall : public BackwardInterface
{
    DECLARE_PUBLIC_COMMONS(Waterfall)
    DECLARE_PUBLIC_COMMONS_COPY(Waterfall)
protected:
    void SortByProRataGroup();
    double GroupWACoupon(int GroupTarget, int SeliorityScaleLevel, const QDate& Period, qint32 CouponType = 0)const;
    double GroupWACoupon(int GroupTarget, int SeliorityScaleLevel, const QDate& Period, QList<qint32> CouponTypes)const;
    double RedeemNotes(double AvailableFunds, int GroupTarget, int SeliorityScaleLevel, const QDate& PeriodIndex);
    //double RedeemProRata(double AvailableFunds, const QDate& PeriodIndex, QList<int> Groups, int SeliorityScaleLevel);
    double RedeemSequential(double AvailableFunds, const QDate& PeriodIndex, int SeliorityScaleLevel, int MaxGroup = -1);
    int FindTrancheIndex(const QString& Tranchename)const;
    void FillAllDates();
    bool TriggerPassing(const QString& TriggerStructure, int PeriodIndex, const QDate& CurrentIPD, bool IsCallDate);
    bool EvaluateTrigger(quint32 TrigID, int PeriodIndex, const QDate& CurrentIPD, bool IsCallDate)const;
    bool ValidTriggerStructure(const QString& TriggerStructure)const;
    int FindMostJuniorLevel(int SeliorityScaleLevel) const;
    virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
    double RedeemChilds(double AvailableFunds, int ParentGroupTarget, int SeliorityScaleLevel, const QDate& TargetDate);
public:
    ~Waterfall();
    const QDate& GetLegalFinal() const;
    void SetLegalFinal(const QDate& val);
    const QHash<quint32, std::shared_ptr<AbstractTrigger> >& GetTriggers() const;
    QString GetGICinterest() const;
    QString GetSeniorExpenses() const;
    QString GetSeniorFees() const;
    QString GetJuniorFees() const;
    QString GetSeniorExpensesFixed() const;
    QString GetSeniorFeesFixed() const;
    QString GetJuniorFeesFixed() const;
    QString GetPaymentFrequency() const;
    double GetCCCTestLimit() const;
    double GetCCChaircut() const;
    double GetPrincipalAvailable() const;
    double GetScheduledPrincipalAvailable() const;
    double GetPrepayPrincipalAvailable() const;
    double GetInterestAvailable() const;
    double GetJuniorFeesCoupon() const;
    QString GetPoolValueAtCall() const;
    bool GetUseCall() const;
    double GetCallMultiple() const;
    double GetCallReserve() const;
    const MtgCashFlow& GetMortgagesPayments() const;
    const GenericCashFlow& GetExcessCashFlow() const;
    const GenericCashFlow& GetTotalSeniorExpenses() const;
    const GenericCashFlow& GetTotalSeniorFees() const;
    const GenericCashFlow& GetTotalJuniorFees() const;
    const ReinvestmentTest& GetReinvestmentTest() const;
    const GenericCashFlow& GetGICflows() const;
    MtgCashFlow& GetMortgagesPayments();
    GenericCashFlow& GetExcessCashFlow();
    GenericCashFlow& GetTotalSeniorExpenses();
    GenericCashFlow& GetTotalSeniorFees();
    GenericCashFlow& GetTotalJuniorFees();
    ReinvestmentTest& GetReinvestmentTest();
    QString GetCCCcurve() const;
    const GenericCashFlow& GetRawAnnualizedExcess() const;
    GenericCashFlow& GetRawAnnualizedExcess();
    double GetAnnualizedExcess(int index, bool AsShareOfLoans = false)const;
    double GetEquityReturn(int index)const;
    double GetCumulativeEquityReturn(int index)const;
    double GetCallEquityRatio(int index)const;
    const QDate& GetFirstIPDdate() const;
    const GenericCashFlow& GetReinvested() const;
    const QDate& GetLastIPDdate() const;
    const QDate& GetCallDate() const;
    const QDate& GetCalledPeriod() const;
    QDate GetStructureMaturity() const;
    const std::shared_ptr<WatFalPrior> GetStep(int Index) const;
    std::shared_ptr<WatFalPrior> GetStep(int Index);
    std::shared_ptr<Tranche> GetTranche(const QString& TrancheName);
    const std::shared_ptr<Tranche> GetTranche(const QString& TrancheName) const;
    std::shared_ptr<Tranche> GetTranche(int Index);
    const std::shared_ptr<Tranche> GetTranche(int Index) const;
    double GetWACostOfCapital(int index)const;
    double GetCreditEnhancement(const QString& Tranchename, int TimeIndex = -1)const { return GetCreditEnhancement(FindTrancheIndex(Tranchename), TimeIndex); }
    double GetCreditEnhancement(int TrancheIndex, int TimeIndex = -1)const;
    int GetTranchesCount()const;
    int GetStepsCount()const;
    const MtgCashFlow& GetCalculatedMtgPayments() const;
    MtgCashFlow& GetCalculatedMtgPayments();
    const std::shared_ptr<ReserveFund> GetReserveFund(int RFindex) const;
    bool GetCumulativeReserves() const;
    int GetNumReserves()const;
    const QSet<QString>& GetDealName() const;
    const double& GetStartingDeferredJunFees() const;
    QString GetGICBaseRate() const;
    const DayCountVector& GetDealDayCountConvention() const;
    const std::shared_ptr<AbstractTrigger> GetTrigger(quint32 key) const;
    std::shared_ptr<AbstractTrigger> GetTrigger(quint32 key);
    GenericCashFlow GetAggregatedReinvestment() const;
    GenericCashFlow GetAggregatedGIC() const;
    MtgCashFlow GetAggregatedMtgFlows() const;
    const bool& GetIsStressTest() const;
    //////////////////////////////////////////////////////////////////////////
    void SetIsStressTest(const bool& val);
    void SetAssumptions(const AssumptionSet& a);
    const TriggersResults& GetTriggersResults() const;
    void SetTrigger(quint32 key, std::shared_ptr<AbstractTrigger> val);
    void ResetTriggers();
    void RemoveTrigger(quint32 key);
    void SetDealDayCountConvention(const QString&  val);
    void SetGICBaseRate(const QString& a);
    void SetStartingDeferredJunFees(const double& val);
    void AddDealName(const QString& a);
    void ClearDealName();
    void SetCumulativeReserves(bool a);
    void SetGICinterest(const QString& a);
    void SetSeniorExpenses(const QString& a);
    void SetSeniorFees(const QString& a);
    void SetJuniorFees(const QString& a);
    void SetSeniorExpensesFixed(const QString& a);
    void SetSeniorFeesFixed(const QString& a);
    void SetJuniorFeesFixed(const QString& a);
    void SetPaymentFrequency(const QString& a);
    void SetCCCTestLimit(double a);
    void SetCCChaircut(double a);
    void SetSchedPrincAvailable(double a);
    void SetPrepPrincAvailable(double a);
    void SetInterestAvailable(double a);
    void SetJuniorFeesCoupon(double a);
    void SetPoolValueAtCall(const QString& a);
    void SetUseCall(bool a);
    void SetCallMultiple(double a);
    void SetCallReserve(double a);
    void AddMortgagesFlows(const MtgCashFlow& a);
    void ResetMtgFlows();
    void SetCCCcurve(const QString& a);
    void SetFirstIPDdate(const QDate& a);
    void SetLastIPDdate(const QDate& a);
    void SetCallDate(const QDate& a);
    void SetReinvestementPeriod(const QDate& ReinvPer);
    void SetupReinvBond(
        const QString& IntrVec
        , const QString& CPRVec
        , const QString& CDRVec
        , const QString& LSVec
        , const QString& WALval
        , const QString& PayFreq = "1"
        , const QString& AnnuityVec = "I"
        , const QString& ReinvPric = "100"
        , const QString& ReinvDel = "0"
        , const QString& ReinvSpr = "1"
        , const QString& FloatingBase = "ZERO"
        , const QString& RecoveryLag = "0"
        , const QString& Delinquency = "0"
        , const QString& DelinquencyLag = "0"
        );
    void AddStep(const WatFalPrior& a);
    void SetReserveFund(int RFindex, const QString& RFtarget, const QString& RFmultiple, const QString& RFfloor, const QString& RFcap, double RFcurrent, int RFfreed, bool RFtoInterest = true);
    void SetReserveFund(int RFindex, const ReserveFund& source);
    void AddReserveFund(const QString& RFtarget, const QString& RFmultiple, const QString& RFfloor, const QString& RFcap, double RFcurrent, int RFfreed, bool RFtoInterest = true);
    void AddReserveFund(const ReserveFund& source);
    void ResetReserve(int RFindex = -1);
    void RemoveReserve(int RFindex = -1);
    void ResetSteps();
    void AddTranche(const Tranche& a);
    void ResetTranches();
    bool CalculateTranchesCashFlows();
    QString ReadyToCalculate() const;
    void CompileReferenceRateValue(ForwardBaseRateTable& Values);
    void CompileReferenceRateValue(ConstantBaseRateTable& Values);
    void GetBaseRatesDatabase(ConstantBaseRateTable& Values, bool DownloadAll = false);
    void GetBaseRatesDatabase(ForwardBaseRateTable& Values, bool DownloadAll = false);
    friend CLOMODELLIB_EXPORT QDataStream& operator<<(QDataStream & stream, const Waterfall& flows);
    friend CLOMODELLIB_EXPORT QDataStream& operator>>(QDataStream & stream, Waterfall& flows);
};
Q_DECLARE_METATYPE(Waterfall)
CLOMODELLIB_EXPORT QDataStream& operator<<(QDataStream & stream, const Waterfall& flows);
CLOMODELLIB_EXPORT QDataStream& operator>>(QDataStream & stream, Waterfall& flows);
#endif // Waterfall_h__