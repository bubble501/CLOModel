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
	TrancheCashFlow m_ExcessCashFlow; 
	TrancheCashFlow m_TotalSeniorExpenses; 
	TrancheCashFlow m_TotalSeniorFees; 
	TrancheCashFlow m_TotalJuniorFees; 
	ReinvestmentTest m_ReinvestmentTest; 
	double m_CCCTestLimit; 
	BloombergVector m_CCCcurve; 
	double m_CCChaircut; 
	bool m_UseTurbo; 
	double m_PrincipalAvailable; 
	double m_InterestAvailable; 
	double m_JuniorFeesCoupon; 
	TrancheCashFlow m_AnnualizedExcess; 
	TrancheCashFlow m_EquityIncome;
	QDate m_FirstIPDdate;
	TrancheCashFlow m_Reinvested; 
	QDate m_LastIPDdate; 
	QDate m_CallDate; 
	double m_PoolValueAtCall; 
	bool m_UseCall; 
	double m_CallMultiple; 
	double m_CallReserve;
	QString m_DealName;
	int FindMostJuniorLevel() const;
	void SortByProRataGroup();
	double GroupOutstanding(int GroupTarget)const;
	double GroupWACoupon(int GroupTarget, const QDate& Period, qint32 CouponType=0)const;
	double GroupWACoupon(int GroupTarget, const QDate& Period, QList<qint32> CouponTypes)const;
	double RedeemNotes(double AvailableFunds,int GroupTarget, int PeriodIndex);
	double RedeemProRata(double AvailableFunds, int PeriodIndex,QList<int> Groups);
	double RedeemSequential(double AvailableFunds, int PeriodIndex,int MaxGroup=-1);
	int FindTrancheIndex(const QString& Tranchename)const;
	void FillAllDates();
protected:
	virtual QDataStream& LoadOldVersion(QDataStream& stream);
public:
	Waterfall();
	Waterfall(const Waterfall& a);
	~Waterfall();
	Waterfall& operator=(const Waterfall& a);
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
	double GetPrincipalAvailable() const {return m_PrincipalAvailable;} 
	double GetInterestAvailable() const {return m_InterestAvailable;} 
	double GetJuniorFeesCoupon() const {return m_JuniorFeesCoupon;} 
	double GetPoolValueAtCall() const {return m_PoolValueAtCall;} 
	bool GetUseCall() const {return m_UseCall;} 
	double GetCallMultiple() const {return m_CallMultiple;} 
	double GetCallReserve() const {return m_CallReserve;}
	const MtgCashFlow& GetMortgagesPayments() const {return m_MortgagesPayments;}
	const TrancheCashFlow& GetExcessCashFlow() const {return m_ExcessCashFlow;} 
	const TrancheCashFlow& GetTotalSeniorExpenses() const {return m_TotalSeniorExpenses;} 
	const TrancheCashFlow& GetTotalSeniorFees() const {return m_TotalSeniorFees;} 
	const TrancheCashFlow& GetTotalJuniorFees() const {return m_TotalJuniorFees;} 
	const ReinvestmentTest& GetReinvestmentTest() const {return m_ReinvestmentTest;}
	MtgCashFlow& GetMortgagesPayments() {return m_MortgagesPayments;}
	TrancheCashFlow& GetExcessCashFlow() {return m_ExcessCashFlow;} 
	TrancheCashFlow& GetTotalSeniorExpenses() {return m_TotalSeniorExpenses;} 
	TrancheCashFlow& GetTotalSeniorFees() {return m_TotalSeniorFees;} 
	TrancheCashFlow& GetTotalJuniorFees() {return m_TotalJuniorFees;} 
	ReinvestmentTest& GetReinvestmentTest() {return m_ReinvestmentTest;}
	QString GetCCCcurve() const {return m_CCCcurve.GetVector();}
	const TrancheCashFlow& GetRawAnnualizedExcess() const {return m_AnnualizedExcess;}
	TrancheCashFlow& GetRawAnnualizedExcess(){return m_AnnualizedExcess;}
	double GetAnnualizedExcess(int index, bool AsShareOfLoans=false)const;
	double GetEquityReturn(int index)const;
	double GetCumulativeEquityReturn(int index)const;
	double GetCallEquityRatio(int index)const;
	const QDate& GetFirstIPDdate() const {return m_FirstIPDdate;}
	const TrancheCashFlow& GetReinvested() const {return m_Reinvested;} 
	TrancheCashFlow& GetReinvested() {return m_Reinvested;} 
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
	//////////////////////////////////////////////////////////////////////////
	void SetDealName(const QString& a) { m_DealName = a; }
	void SetCumulativeReserves(bool a){m_CumulativeReserves=a;}
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
	void SetPrincipalAvailable(double a){m_PrincipalAvailable=a;}
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