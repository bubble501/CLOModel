#ifndef Waterfall_h__
#define Waterfall_h__
#include <MtgCashFlow.h>
#include <TrancheCashFlow.h>
#include <WatFalPrior.h>
class Tranche;
class ReinvestmentTest;
class Waterfall{
public:
	Waterfall();
	Waterfall(const Waterfall& a);
	~Waterfall();
	Waterfall& operator=(const Waterfall& a);
	double GetSeniorExpenses() const;
	double GetSeniorFees() const;
	double GetJuniorFees() const;
	int GetPaymentFrequency() const;
	double GetCCCTestLimit() const;
	double GetCCChaircut() const;
	bool GetUseTurbo() const;
	double GetPrincipalAvailable() const;
	double GetInterestAvailable() const;
	double GetJuniorFeesCoupon() const;
	double GetPoolValueAtCall() const;
	bool GetUseCall() const;
	double GetCallMultiple() const;
	double GetCallReserve() const;
	const MtgCashFlow& GetMortgagesPayments() const;
	const TrancheCashFlow& GetExcessCashFlow() const;
	const TrancheCashFlow& GetTotalSeniorExpenses() const;
	const TrancheCashFlow& GetTotalSeniorFees() const;
	const TrancheCashFlow& GetTotalJuniorFees() const;
	const ReinvestmentTest& GetReinvestmentTest() const;
	ReinvestmentTest& GetReinvestmentTest();
	const QString& GetCCCcurve() const;
	const TrancheCashFlow& GetRawAnnualizedExcess() const;
	double GetAnnualizedExcess(int index, bool AsShareOfLoans=false)const;
	double GetEquityReturn(int index)const;
	double GetCumulativeEquityReturn(int index)const;
	double GetCallEquityRatio(int index)const;
	const QDate& GetFirstIPDdate() const;
	const TrancheCashFlow& GetReinvested() const;
	const QDate& GetLastIPDdate() const;
	const QDate& GetCallDate() const;
	QDate GetCalledPeriod() const;
	QDate GetStructureMaturity() const;
	const WatFalPrior* GetStep(int Index) const;
	const Tranche* GetTranche(int Index) const;
	const Tranche* GetTranche(const QString& TrancheName) const;
	double GetWACostOfCapital(int index)const;
	double GetCreditEnhancement(const QString& Tranchename,int TimeIndex=-1)const;
	double GetCreditEnhancement(int TrancheIndex,int TimeIndex=-1)const;
	int GetTranchesCount()const;
	int GetStepsCount()const;
	const MtgCashFlow& GetCalculatedMtgPayments() const;
	//////////////////////////////////////////////////////////////////////////
	void SetSeniorExpenses(double a);
	void SetSeniorFees(double a);
	void SetJuniorFees(double a);
	void SetPaymentFrequency(int a);
	void SetCCCTestLimit(double a);
	void SetCCChaircut(double a);
	void SetUseTurbo(bool a);
	void SetPrincipalAvailable(double a);
	void SetInterestAvailable(double a);
	void SetJuniorFeesCoupon(double a);
	void SetPoolValueAtCall(double a);
	void SetUseCall(bool a);
	void SetCallMultiple(double a);
	void SetCallReserve(double a);
	void AddMortgagesFlows(const MtgCashFlow& a);
	void ResetMtgFlows();
	void SetCCCcurve(const QString& a);
	void SetFirstIPDdate(const QDate& a);
	void SetLastIPDdate(const QDate& a);
	void SetCallDate(const QDate& a);
	void SetupReinvestmentTest(const QDate& ReinvPer,double TstLvl, double IIshare,double IRshare,double OIshare,double ORshare);
	void SetupReinvBond(const QString& IntrVec,const QString& CPRVec,const QString& CDRVec,const QString& LSVec,const QString& WALval,int PayFreq=1,const QString& AnnuityVec="N",double FloatingBase=0.0);
	void AddStep(const WatFalPrior& a);
	void ResetSteps();
	void AddTranche(const Tranche& a);
	void ResetTranches();
	bool CalculateTranchesCashFlows();
	QString ReadyToCalculate() const;
};
#endif // Waterfall_h__