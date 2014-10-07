#ifndef Waterfall_h__
#define Waterfall_h__
#include <QList>
#include <QMetaType>
#include "Tranche.h"
#include "WatFalPrior.h"
#include "MtgCashFlow.h"
#include "ReinvestmentTest.h"
#include "BloombergVector.h"
#include "BackwardCompatibilityInterface.h"
#include "IntegerVector.h"
#include "ReserveFund.h"
#include "DayCountVect.h"
#include <QSharedPointer>
#include "AbstractTrigger.h"
#include "PrincipalProceeds.h"
#include "TriggersResults.h"
class Waterfall : public BackwardInterface{
private:
	QList<ReserveFund*> m_Reserves;
	bool m_CumulativeReserves;
	BloombergVector m_SeniorExpenses;
	BloombergVector m_SeniorFees;
	BloombergVector m_JuniorFees;
	BloombergVector m_SeniorExpensesFixed;
	BloombergVector m_SeniorFeesFixed;
	BloombergVector m_JuniorFeesFixed;
	QList<Tranche*> m_Tranches;
	QList<WatFalPrior*>	m_WaterfallStesps;
	MtgCashFlow m_MortgagesPayments; 
	MtgCashFlow m_CalculatedMtgPayments;
	IntegerVector m_PaymentFrequency;
	GenericCashFlow m_ExcessCashFlow; 
	GenericCashFlow m_TotalSeniorExpenses;
	GenericCashFlow m_TotalSeniorFees;
	GenericCashFlow m_TotalJuniorFees;
	ReinvestmentTest m_ReinvestmentTest; 
	double m_CCCTestLimit; 
	BloombergVector m_CCCcurve; 
	double m_CCChaircut; 
	PrincipalRecip m_PrincipalAvailable;
	double m_InterestAvailable; 
	double m_JuniorFeesCoupon; 
	GenericCashFlow m_AnnualizedExcess;
	GenericCashFlow m_EquityIncome;
	QDate m_FirstIPDdate;
	GenericCashFlow m_Reinvested;
	QDate m_LastIPDdate; 
	QDate m_CallDate; 
	BloombergVector m_PoolValueAtCall; 
	bool m_UseCall; 
	double m_CallMultiple; 
	double m_CallReserve;
	QString m_DealName;
	BloombergVector m_GICinterest;
	BaseRateVector m_GICBaseRate;
	BloombergVector m_GICBaseRateValue;
	double m_StartingDeferredJunFees;
	GenericCashFlow m_GICflows;
	DayCountVector m_DealDayCountConvention;
	QDate m_CalledPeriod;
	TriggersResults m_TriggersResults;
	QHash <quint32, QSharedPointer<AbstractTrigger> > m_Triggers;
	int FindMostJuniorLevel(int SeliorityScaleLevel) const;
	void SortByProRataGroup();
	double GroupOutstanding(int GroupTarget, int SeliorityScaleLevel)const;
	double GroupWACoupon(int GroupTarget, int SeliorityScaleLevel, const QDate& Period, qint32 CouponType = 0)const;
	double GroupWACoupon(int GroupTarget, int SeliorityScaleLevel, const QDate& Period, QList<qint32> CouponTypes)const;
	double RedeemNotes(double AvailableFunds, int GroupTarget, int SeliorityScaleLevel, const QDate& PeriodIndex);
	double RedeemProRata(double AvailableFunds, const QDate& PeriodIndex, QList<int> Groups, int SeliorityScaleLevel);
	double RedeemSequential(double AvailableFunds, const QDate& PeriodIndex, int SeliorityScaleLevel, int MaxGroup = -1 );
	int FindTrancheIndex(const QString& Tranchename)const;
	void FillAllDates();
	bool TriggerPassing(const QString& TriggerStructure, int PeriodIndex, const QDate& CurrentIPD, bool IsCallDate);
	bool EvaluateTrigger(quint32 TrigID, int PeriodIndex, const QDate& CurrentIPD, bool IsCallDate)const;
	bool ValidTriggerStructure(const QString& TriggerStructure)const;
protected:
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
public:
	Waterfall();
	Waterfall(const Waterfall& a);
	~Waterfall();
	Waterfall& operator=(const Waterfall& a);
	QString GetGICinterest() const { return m_GICinterest.GetVector(); }
	QString GetSeniorExpenses() const { return m_SeniorExpenses.GetVector(); }
	QString GetSeniorFees() const { return m_SeniorFees.GetVector(); }
	QString GetJuniorFees() const { return m_JuniorFees.GetVector(); }
	QString GetSeniorExpensesFixed() const { return m_SeniorExpensesFixed.GetVector(); }
	QString GetSeniorFeesFixed() const { return m_SeniorFeesFixed.GetVector(); }
	QString GetJuniorFeesFixed() const { return m_JuniorFeesFixed.GetVector(); }
	QString GetPaymentFrequency() const {return m_PaymentFrequency.GetVector();}
	double GetCCCTestLimit() const {return m_CCCTestLimit;} 
	double GetCCChaircut() const {return m_CCChaircut;} 
	double GetPrincipalAvailable() const {return m_PrincipalAvailable.Total();} 
	double GetScheduledPrincipalAvailable() const { return m_PrincipalAvailable.GetScheduled(); }
	double GetPrepayPrincipalAvailable() const { return m_PrincipalAvailable.GetPrepay(); }
	double GetInterestAvailable() const {return m_InterestAvailable;} 
	double GetJuniorFeesCoupon() const {return m_JuniorFeesCoupon;} 
	QString GetPoolValueAtCall() const {return m_PoolValueAtCall.GetVector();} 
	bool GetUseCall() const {return m_UseCall;} 
	double GetCallMultiple() const {return m_CallMultiple;} 
	double GetCallReserve() const {return m_CallReserve;}
	const MtgCashFlow& GetMortgagesPayments() const {return m_MortgagesPayments;}
	const GenericCashFlow& GetExcessCashFlow() const {return m_ExcessCashFlow;} 
	const GenericCashFlow& GetTotalSeniorExpenses() const { return m_TotalSeniorExpenses; }
	const GenericCashFlow& GetTotalSeniorFees() const { return m_TotalSeniorFees; }
	const GenericCashFlow& GetTotalJuniorFees() const { return m_TotalJuniorFees; }
	const ReinvestmentTest& GetReinvestmentTest() const {return m_ReinvestmentTest;}
	const GenericCashFlow& GetGICflows() const { return m_GICflows; }
	MtgCashFlow& GetMortgagesPayments() {return m_MortgagesPayments;}
	GenericCashFlow& GetExcessCashFlow() { return m_ExcessCashFlow; }
	GenericCashFlow& GetTotalSeniorExpenses() { return m_TotalSeniorExpenses; }
	GenericCashFlow& GetTotalSeniorFees() { return m_TotalSeniorFees; }
	GenericCashFlow& GetTotalJuniorFees() { return m_TotalJuniorFees; }
	ReinvestmentTest& GetReinvestmentTest() {return m_ReinvestmentTest;}
	QString GetCCCcurve() const {return m_CCCcurve.GetVector();}
	const GenericCashFlow& GetRawAnnualizedExcess() const { return m_AnnualizedExcess; }
	GenericCashFlow& GetRawAnnualizedExcess() { return m_AnnualizedExcess; }
	double GetAnnualizedExcess(int index, bool AsShareOfLoans=false)const;
	double GetEquityReturn(int index)const;
	double GetCumulativeEquityReturn(int index)const;
	double GetCallEquityRatio(int index)const;
	const QDate& GetFirstIPDdate() const {return m_FirstIPDdate;}
	const GenericCashFlow& GetReinvested() const { return m_Reinvested; }
	GenericCashFlow& GetReinvested() { return m_Reinvested; }
	const QDate& GetLastIPDdate() const {return m_LastIPDdate;} 
	const QDate& GetCallDate() const {return m_CallDate;}
	const QDate& GetCalledPeriod() const;
	QDate GetStructureMaturity() const;
	const WatFalPrior* GetStep(int Index) const;
	WatFalPrior* GetStep(int Index);
	Tranche* GetTranche(const QString& TrancheName);
	const Tranche* GetTranche(const QString& TrancheName) const;
	Tranche* GetTranche(int Index);
	const Tranche* GetTranche(int Index) const;
	double GetWACostOfCapital(int index)const;
	double GetCreditEnhancement(const QString& Tranchename,int TimeIndex=-1)const{return GetCreditEnhancement(FindTrancheIndex(Tranchename),TimeIndex);}
	double GetCreditEnhancement(int TrancheIndex,int TimeIndex=-1)const;
	int GetTranchesCount()const{return m_Tranches.size();}
	int GetStepsCount()const{return m_WaterfallStesps.size();}
	const MtgCashFlow& GetCalculatedMtgPayments() const {return m_CalculatedMtgPayments;}
	MtgCashFlow& GetCalculatedMtgPayments() {return m_CalculatedMtgPayments;}
	const ReserveFund* const GetReserveFund(int RFindex) const {
		if (RFindex < 0 || RFindex >= m_Reserves.size()) return NULL;
		return m_Reserves.at(RFindex);
	}
	bool GetCumulativeReserves() const {return m_CumulativeReserves;}
	int GetNumReserves()const { return m_Reserves.size(); }
	const QString& GetDealName() const { return m_DealName; }
	const double& GetStartingDeferredJunFees() const { return m_StartingDeferredJunFees; }
	QString GetGICBaseRate() const { return m_GICBaseRate.GetVector(); }
	const DayCountVector& GetDealDayCountConvention() const { return m_DealDayCountConvention; }
	const QSharedPointer<AbstractTrigger> GetTrigger(quint32 key) const { return m_Triggers.value(key, QSharedPointer<AbstractTrigger>()); }
	QSharedPointer<AbstractTrigger> GetTrigger(quint32 key) { if (m_Triggers.contains(key)) return m_Triggers[key]; return QSharedPointer<AbstractTrigger>(); }
	GenericCashFlow GetAggregatedReinvestment() const;
	GenericCashFlow GetAggregatedGIC() const;
	MtgCashFlow GetAggregatedMtgFlows() const;
	//////////////////////////////////////////////////////////////////////////
	const TriggersResults& GetTriggersResults() const { return m_TriggersResults; }
	void SetTrigger(quint32 key, QSharedPointer<AbstractTrigger> val);
	void ResetTriggers();
	void RemoveTrigger(quint32 key) { m_Triggers.remove(key); }
	void SetDealDayCountConvention(const QString&  val) { m_DealDayCountConvention = val; }
	void SetGICBaseRate(const QString& a) { m_GICBaseRate = a; }
	void SetStartingDeferredJunFees(const double& val) { m_StartingDeferredJunFees = val; }
	void SetDealName(const QString& a) { m_DealName = a; }
	void SetCumulativeReserves(bool a){m_CumulativeReserves=a;}
	void SetGICinterest(const QString& a) { m_GICinterest = a; }
	void SetSeniorExpenses(const QString& a) { m_SeniorExpenses = a; }
	void SetSeniorFees(const QString& a) { m_SeniorFees = a; }
	void SetJuniorFees(const QString& a) { m_JuniorFees = a; }
	void SetSeniorExpensesFixed(const QString& a) { m_SeniorExpensesFixed = a; }
	void SetSeniorFeesFixed(const QString& a) { m_SeniorFeesFixed = a; }
	void SetJuniorFeesFixed(const QString& a) { m_JuniorFeesFixed = a; }
	void SetPaymentFrequency(const QString& a);
	void SetCCCTestLimit(double a){if(a>=0.0 && a<=1.0) m_CCCTestLimit=a;}
	void SetCCChaircut(double a){if(a>=0.0 && a<=1.0) m_CCChaircut=a;}
	void SetSchedPrincAvailable(double a){m_PrincipalAvailable.SetScheduled(a);}
	void SetPrepPrincAvailable(double a) { m_PrincipalAvailable.SetPrepay(a); }
	void SetInterestAvailable(double a){m_InterestAvailable=a;}
	void SetJuniorFeesCoupon(double a){if(a>=0.0) m_JuniorFeesCoupon=a;}
	void SetPoolValueAtCall(const QString& a){m_PoolValueAtCall=a;}
	void SetUseCall(bool a){m_UseCall=a;}
	void SetCallMultiple(double a){if(a>=0.0) m_CallMultiple=a;}
	void SetCallReserve(double a){if(a>=0.0) m_CallReserve=a;} 
	void AddMortgagesFlows(const MtgCashFlow& a){m_MortgagesPayments+=a;}
	void ResetMtgFlows(){m_MortgagesPayments.Clear();}
	void SetCCCcurve(const QString& a);
	void SetFirstIPDdate(const QDate& a){m_FirstIPDdate=a;}
	void SetLastIPDdate(const QDate& a){m_LastIPDdate=a;}
	void SetCallDate(const QDate& a){m_CallDate=a;}
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
		, const QString& ReinvSpr = "0"
		, const QString& FloatingBase = "ZERO"
		, const QString& RecoveryLag = "0"
		, const QString& Delinquency = "0"
		, const QString& DelinquencyLag = "0"
		);
		
	void AddStep(const WatFalPrior& a){m_WaterfallStesps.append(new WatFalPrior(a));}
	void SetReserveFund(int RFindex, const QString& RFtarget, const QString& RFmultiple, const QString& RFfloor, const QString& RFcap,double RFcurrent, int RFfreed, bool RFtoInterest = true);
	void SetReserveFund(int RFindex, const ReserveFund& source);
	void AddReserveFund(const QString& RFtarget, const QString& RFmultiple, const QString& RFfloor, const QString& RFcap,double RFcurrent, int RFfreed, bool RFtoInterest = true);
	void AddReserveFund(const ReserveFund& source);
	void ResetReserve(int RFindex=-1);
	void RemoveReserve(int RFindex = -1);
	void ResetSteps();
	void AddTranche(const Tranche& a){m_Tranches.append(new Tranche(a));}
	void ResetTranches();
	bool CalculateTranchesCashFlows();
	QString ReadyToCalculate() const;
	void CompileReferenceRateValue(ForwardBaseRateTable& Values);
	void CompileReferenceRateValue(ConstantBaseRateTable& Values);
#ifndef NO_DATABASE
	void GetBaseRatesDatabase(ConstantBaseRateTable& Values, bool DownloadAll = false);
	void GetBaseRatesDatabase(ForwardBaseRateTable& Values, bool DownloadAll = false);
#endif
	friend QDataStream& operator<<(QDataStream & stream, const Waterfall& flows);
	friend QDataStream& operator>>(QDataStream & stream, Waterfall& flows);
};
Q_DECLARE_METATYPE(Waterfall)
QDataStream& operator<<(QDataStream & stream, const Waterfall& flows);
QDataStream& operator>>(QDataStream & stream, Waterfall& flows);
#endif // Waterfall_h__