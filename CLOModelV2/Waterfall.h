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
class PrincipalRecip {
protected:
	double m_Scheduled;
	double m_Prepay;
public:
	const double& GetPrepay() const { return m_Prepay; }
	void SetPrepay(const double& val) { if (qAbs(val) >= 0.01) m_Prepay = val; }
	void AddPrepay(const double& val) { if (qAbs(val) >= 0.01) m_Prepay += val; }
	const double& GetScheduled() const { return m_Scheduled; }
	void SetScheduled(const double& val) { if (qAbs(val) >= 0.01) m_Scheduled = val; }
	void AddScheduled(const double& val) { if (qAbs(val) >= 0.01) m_Scheduled += val; }
	PrincipalRecip() :m_Scheduled(0.0), m_Prepay(0.0) {}
	PrincipalRecip(const PrincipalRecip& a) :m_Scheduled(a.m_Scheduled), m_Prepay(a.m_Prepay) {}
	PrincipalRecip& operator=(const PrincipalRecip& a) { m_Scheduled = a.m_Scheduled; m_Prepay = a.m_Prepay; return *this; }
	PrincipalRecip operator-(double a) { PrincipalRecip Result(*this); Result -= a; return Result; }
	PrincipalRecip& operator-=(double a) { 
		if (qAbs(a) < 0.01) return *this;
		if (a < 0.0) return operator+=(-a);
		double Temp=qMin(a,m_Scheduled); 
		m_Scheduled -= Temp;
		m_Prepay -= a - Temp;
		return *this;
	}
	PrincipalRecip& operator+=(double a) { 
		if (qAbs(a) < 0.01) return *this;
		if (a < 0.0) return operator-=(-a);
		m_Scheduled += a;
		return *this;
	}
	PrincipalRecip operator+(double a) { PrincipalRecip Result(*this); Result += a; return Result; }
	bool operator<(double a){return m_Scheduled<a || m_Prepay<a;}
	double Total() const { return m_Scheduled + m_Prepay; }
	void Erase() { m_Scheduled = m_Prepay = 0.0; }
};
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
	bool m_UseTurbo; 
	PrincipalRecip m_PrincipalAvailable;
	double m_InterestAvailable; 
	double m_JuniorFeesCoupon; 
	GenericCashFlow m_AnnualizedExcess;
	GenericCashFlow m_EquityIncome;
	QDate m_FirstIPDdate;
	GenericCashFlow m_Reinvested;
	QDate m_LastIPDdate; 
	QDate m_CallDate; 
	double m_PoolValueAtCall; 
	bool m_UseCall; 
	double m_CallMultiple; 
	double m_CallReserve;
	QString m_DealName;
	BloombergVector m_GICinterest;
	double m_StartingDeferredJunFees;
	GenericCashFlow m_GICflows;
	int FindMostJuniorLevel() const;
	void SortByProRataGroup();
	double GroupOutstanding(int GroupTarget)const;
	double GroupWACoupon(int GroupTarget, const QDate& Period, qint32 CouponType=0)const;
	double GroupWACoupon(int GroupTarget, const QDate& Period, QList<qint32> CouponTypes)const;
	double RedeemNotes(double AvailableFunds, int GroupTarget, const QDate& PeriodIndex);
	double RedeemProRata(double AvailableFunds, const QDate& PeriodIndex, QList<int> Groups);
	double RedeemSequential(double AvailableFunds, const QDate& PeriodIndex, int MaxGroup = -1);
	int FindTrancheIndex(const QString& Tranchename)const;
	void FillAllDates();
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
	bool GetUseTurbo() const {return m_UseTurbo;} 
	double GetPrincipalAvailable() const {return m_PrincipalAvailable.Total();} 
	double GetScheduledPrincipalAvailable() const { return m_PrincipalAvailable.GetScheduled(); }
	double GetPrepayPrincipalAvailable() const { return m_PrincipalAvailable.GetPrepay(); }
	double GetInterestAvailable() const {return m_InterestAvailable;} 
	double GetJuniorFeesCoupon() const {return m_JuniorFeesCoupon;} 
	double GetPoolValueAtCall() const {return m_PoolValueAtCall;} 
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
	QDate GetCalledPeriod() const;
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
	//////////////////////////////////////////////////////////////////////////
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
	void SetUseTurbo(bool a){m_UseTurbo=a;}
	void SetSchedPrincAvailable(double a){m_PrincipalAvailable.SetScheduled(a);}
	void SetPrepPrincAvailable(double a) { m_PrincipalAvailable.SetPrepay(a); }
	void SetInterestAvailable(double a){m_InterestAvailable=a;}
	void SetJuniorFeesCoupon(double a){if(a>=0.0) m_JuniorFeesCoupon=a;}
	void SetPoolValueAtCall(double a){if(a>=0.0) m_PoolValueAtCall=a;}
	void SetUseCall(bool a){m_UseCall=a;}
	void SetCallMultiple(double a){if(a>=0.0) m_CallMultiple=a;}
	void SetCallReserve(double a){if(a>=0.0) m_CallReserve=a;} 
	void AddMortgagesFlows(const MtgCashFlow& a){m_MortgagesPayments+=a;}
	void ResetMtgFlows(){m_MortgagesPayments.Clear();}
	void SetCCCcurve(const QString& a);
	void SetFirstIPDdate(const QDate& a){m_FirstIPDdate=a;}
	void SetLastIPDdate(const QDate& a){m_LastIPDdate=a;}
	void SetCallDate(const QDate& a){m_CallDate=a;}
	void SetupReinvestmentTest(const QDate& ReinvPer,double TstLvl, double IIshare,double IRshare,double OIshare,double ORshare);
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
	friend QDataStream& operator<<(QDataStream & stream, const Waterfall& flows);
	friend QDataStream& operator>>(QDataStream & stream, Waterfall& flows);
};
Q_DECLARE_METATYPE(Waterfall)
QDataStream& operator<<(QDataStream & stream, const Waterfall& flows);
QDataStream& operator>>(QDataStream & stream, Waterfall& flows);
#endif // Waterfall_h__