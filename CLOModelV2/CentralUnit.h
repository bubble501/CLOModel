#ifndef CentralUnit_h__
#define CentralUnit_h__
#include <QObject>
#include <QHash>
#include "MtgCalculator.h"
#include "Waterfall.h"
#include "StressTest.h"
#include "BaseRateTable.h"
#include <QPointer>
#include <QProgressDialog>
#define NumberOfPlots 9
class WaterfallCalculator;
class CentralUnit : public QObject{
	Q_OBJECT
public:
	~CentralUnit();
	CentralUnit(QObject* parent=0);
	void AddLoan(const Mortgage& TempMtg) {LoansCalculator.AddLoan(TempMtg,LoansCalculator.NumBees());}
	void AddLoan(
		const QDate& Maturity
		, double Size
		, const QString& Interest
		, const QString& Annuity
		, const QString& Freq = "1"
		, const QString& floatBase = "ZERO"
		, const QString& Properties = ""
		);
#ifndef NO_BLOOMBERG
	void AddTranche(const QString& Name, const QString& ProRataGroup, double MinOC = 0.0, double MinIC = 0.0, double Price = 100.0, double FxRate = 1.0, const QString& BbgExt = "Mtge");
#endif
	void AddTranche(
		const QString& Name
		, const QString& ISIN
		, const QString& ProRataGroup
		, double OrigAmnt
		, const QString& Crncy
		, double OutstandingAmt
		, const QList<QString>& Coupon
		, const QList<QString>& RefRte
		, const QDate& LastPayDte
		, const QString& DefRefRte
		, const QString& PayFreq
		, const QDate& SettlementDate
		, double StartingDeferredInterest = 0.0
		//,const QString& RefRteValue=""
		, double MinOC = 0.0
		, double MinIC = 0.0
		, double Price = 100.0
		, double FxRate = 1.0
		, const QString& BbgExt = "Mtge"
        , const QList<QString>& DayCount = QList<QString>() << QString::number(static_cast<qint16>(DayCountConvention::ACT360))
	);
	void SetTrigger(qint32 key,const QSharedPointer<AbstractTrigger>& val) { Structure.SetTrigger(key, val); }
	void AddTranche(const Tranche& a) { Structure.AddTranche(a);	if (Stresser)Stresser->SetStructure(Structure); }
	void CentralUnit::AddWaterfallStep(
		WatFalPrior::WaterfallStepType Tpe
		, const QString& ArgSeniorityGroup
		, const QString& ArgSeniorityGroupLevel
		, const QString& ArgRedemptionGroup
		, const QString& ArgRedemptionGroupLevel
		, const QString& ArgRedemptionShare
		, const QString& ArgAdditionalCollateralShare
		, const QString& ArgSourceofFunding
		, const QString& ArgCouponIndex
		, const QString& ArgTestTargetOverride
		, const QString& ArgIRRtoEquityTarget
		, const QString& ArgReserveIndex
		, const QString& ArgTrigger
		, int ArgAccruePay
	);
	void SetSeniorExpenses(const QString& a){Structure.SetSeniorExpenses(a);}
	void SetSeniorFees(const QString& a) { Structure.SetSeniorFees(a); }
	void SetJuniorFees(const QString& a) { Structure.SetJuniorFees(a); }
	void SetSeniorExpensesFixed(const QString& a) { Structure.SetSeniorExpensesFixed(a); }
	void SetSeniorFeesFixed(const QString& a) { Structure.SetSeniorFeesFixed(a); }
	void SetJuniorFeesFixed(const QString& a) { Structure.SetJuniorFeesFixed(a); }
	void SetPaymentFrequency(const QString& a){Structure.SetPaymentFrequency(a);}
	void SetCCCTestLimit(double a){Structure.SetCCCTestLimit(a);}
	void SetCCChaircut(double a){Structure.SetCCChaircut(a);}
	void SetSchedPrincAvailable(double a) { Structure.SetSchedPrincAvailable(a); }
	void SetPrepPrincAvailable(double a) { Structure.SetPrepPrincAvailable(a); }
	void SetInterestAvailable(double a){Structure.SetInterestAvailable(a);}
	void SetJuniorFeesCoupon(double a){Structure.SetJuniorFeesCoupon(a);}
	void SetPoolValueAtCall(const QString& a){Structure.SetPoolValueAtCall(a);}
	void SetCallMultiple(double a){Structure.SetCallMultiple(a);}
	void SetCallReserve(double a){Structure.SetCallReserve(a);}
	void SetFirstIPDdate(const QDate& a){Structure.SetFirstIPDdate(a);}
	void SetCCCcurve(const QString& a){Structure.SetCCCcurve(a);}
	void SetLastIPDdate(const QDate& a){Structure.SetLastIPDdate(a);}
	void SetCallDate(const QDate& a){Structure.SetCallDate(a);}
	void SetStartingDeferredJunFees(const double& val) { Structure.SetStartingDeferredJunFees(val); }
	void SetGICinterest(const QString& a) { Structure.SetGICinterest(a); }
	void SetGICBaseRate(const QString& a) { Structure.SetGICBaseRate(a); }
	void SetDealDayCountConvention(const QString&  val) { Structure.SetDealDayCountConvention(val); }
	void SetReinvestementPeriod(const QDate& ReinvPer) { Structure.SetReinvestementPeriod(ReinvPer); }
	void SetLegalFinal(const QDate& a) { Structure.SetLegalFinal(a); }
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
		) {
		Structure.SetupReinvBond(IntrVec, CPRVec, CDRVec, LSVec, WALval, PayFreq, AnnuityVec, ReinvPric, ReinvDel, ReinvSpr, FloatingBase, RecoveryLag, Delinquency, DelinquencyLag);
	}
	void Reset();
	void SetupStress(const QString& ConstPar, QList<QString> XSpann, QList<QString> YSpann, int Xvar = 0x1, int Yvar = 0x2);
	void SetPoolCutOff(const QDate& a);
	void SetUseCall(bool a){RunCall=a;}
	const QString& GetMtgOutputAddress() const{return MtgOutputAddress;}
	const QString& GetTranchesOutputAddress() const{return TranchesOutputAddress;}
	void SetMtgOutputAddress(const QString& a){MtgOutputAddress=a;}
	void SetTranchesOutputAddress(const QString& a){TranchesOutputAddress=a;}
	void Calculate();
	void CalculateStress();
	void SetFolderPath(const QString& a);
	const QString& GetFolderPath() const {return FolderPath;}
	bool GetRunCall() const{return RunCall;}
	void SetRunCall(bool a){RunCall=a;}
	bool GetFastStress() const { return UseFastStress; }
	void SetFastStress(bool a) { UseFastStress = a; }
	void SetLossOutputAddress(const QString& a){LossOutputAddress=a;}
	void SetLossOnCallOutputAddress(const QString& a){LossOnCallOutputAddress=a;}
	void SetCreditEnanAddress(const QString& a){CreditEnanAddress=a;}
	void SetStressToCall(bool a){StressToCall=a;}
	void SetPlotsSheet(const QString& a){PlotsSheet=a;}
	void SetPlotIndexes(int val,int index){if(index>=0 && index<NumberOfPlots) PlotIndexes[index]=val;}
	void SetReserveFund(int RFindex, const QString& RFtarget, const QString& RFmultiple, const QString& RFfloor, const QString& RFfcap, double RFcurrent, int RFfreed, bool RFtoInterest = true) { Structure.SetReserveFund(RFindex, RFtarget, RFmultiple, RFfloor, RFfcap, RFcurrent, RFfreed, RFtoInterest); }
	void AddReserveFund(const QString& RFtarget, const QString& RFmultiple, const QString& RFfloor, const QString& RFfcap, double RFcurrent, int RFfreed, bool RFtoInterest = true) { Structure.AddReserveFund(RFtarget, RFmultiple, RFfloor, RFfcap,RFcurrent, RFfreed, RFtoInterest); }
	void SetCumulativeReserves(bool a){Structure.SetCumulativeReserves(a);}
	const  QDate& GetLiborUpdateDate() const { return LiborUpdateDate; }
	bool GetUseForwardCurve() const { return m_UseForwardCurve; }
	void CompileBaseRates(ConstantBaseRateTable& Values);
	void CompileBaseRates(ForwardBaseRateTable& Values);
#ifndef NO_DATABASE
	void GetBaseRatesDatabase(ConstantBaseRateTable& Values, bool DownloadAll = false);
	void GetBaseRatesDatabase(ForwardBaseRateTable& Values, bool DownloadAll=false);
#endif
	void SetDealName(const QString& a) {Structure.SetDealName(a);}
	bool GetBaseCaseToCall() const { return m_BaseCaseToCall; }
	void SetBaseCaseToCall(bool val) { m_BaseCaseToCall = val; }
	bool GetSaveBaseCase() const { return m_SaveBaseCase; }
	void SetSaveBaseCase(bool val) { m_SaveBaseCase = val; }
	const Waterfall& GetStructure() { return Structure; }
	const Waterfall& GetCallStructure() { return CallStructure; }
private:
	AbstractBaseRateTable* LastRateTable;
	QPointer<QProgressDialog> MtgsProgress;
	WaterfallCalculator* ParallWatFalls;
	MtgCalculator LoansCalculator;
	Waterfall Structure;
	Waterfall CallStructure;
	QPointer<StressTest> Stresser;
	QDate PoolCutOff;
	bool RunCall;
	bool RunStress;
	bool StressToCall;
	bool UseFastStress;
	QString MtgOutputAddress;
	QString TranchesOutputAddress;
	QString CallTranchesOutputAddress;
	QString LossOutputAddress;
	QString LossOnCallOutputAddress;
	QString CreditEnanAddress;
	QString FolderPath;
	QString PlotsSheet;
	int PlotIndexes[NumberOfPlots];
	QDate LiborUpdateDate;
	bool m_UseForwardCurve;
	bool m_SaveBaseCase;
	bool m_BaseCaseToCall;
	ForwardBaseRateTable m_OverrideForwards;
	ConstantBaseRateTable m_OverrideConstants;
    void showErrorDialog(const QString& errList);
signals:
	void LoopStarted();
	void StressLoopStarted();
	void CalculationFinished();
private slots:
	void CalculationStep2();
	void CalculationStep1();
	void CheckCalculationDone();
	void StressFinished();
};
#endif // CentralUnit_h__