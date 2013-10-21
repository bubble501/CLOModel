#ifndef CentralUnit_h__
#define CentralUnit_h__

#include <QObject>
#include "MtgCalculator.h"
#include "Waterfall.h"
#include "StressTest.h"
class WaterfallCalculator;
class CentralUnit : public QObject{
	Q_OBJECT
public:
	CentralUnit(QObject* parent=0);
	void AddLoan(const QDate& Maturity, double Size, const QString& Interest, const QString& Annuity, int Freq=1,double floatBase=0.0, int LossMult=100, int PrepayMult=100);
	void AddTranche(const QString& Name,int ProRataGroup, double MinOC=0.0, double MinIC=0.0, double Price=100.0,double FxRate= 1.0,const QString& BbgExt="Mtge", int DayCount=360);
	void AddTranche(
		const QString& Name
		,int ProRataGroup
		,double OrigAmnt
		,const QString& Crncy
		,double OutstandingAmt
		,Tranche::TrancheInterestType IntrTyp
		,double Coupon
		,const QString& RefRte
		,const QDate& LastPayDte
		,const QString& DefRefRte
		,int PayFreq
		,double RefRteValue=-1.0
		,double MinOC=0.0
		,double MinIC=0.0
		,double Price=100.0
		,double FxRate= 1.0
		,const QString& BbgExt="Mtge"
		,int DayCount=360
	);
	void AddWaterfallStep(WatFalPrior::WaterfallStepType Tpe, int GrpTgt, int RdmpGrp=0, double RdmpShare=0.0);
	void SetSeniorExpenses(double a){Structure.SetSeniorExpenses(a);}
	void SetSeniorFees(double a){Structure.SetSeniorFees(a);}
	void SetJuniorFees(double a){Structure.SetJuniorFees(a);}
	void SetPaymentFrequency(int a){Structure.SetPaymentFrequency(a);}
	void SetCCCTestLimit(double a){Structure.SetCCCTestLimit(a);}
	void SetCCChaircut(double a){Structure.SetCCChaircut(a);}
	void SetUseTurbo(bool a){Structure.SetUseTurbo(a);}
	void SetPrincipalAvailable(double a){Structure.SetPrincipalAvailable(a);}
	void SetInterestAvailable(double a){Structure.SetInterestAvailable(a);}
	void SetJuniorFeesCoupon(double a){Structure.SetJuniorFeesCoupon(a);}
	void SetPoolValueAtCall(double a){Structure.SetPoolValueAtCall(a);}
	void SetCallMultiple(double a){Structure.SetCallMultiple(a);}
	void SetCallReserve(double a){Structure.SetCallReserve(a);}
	void SetFirstIPDdate(const QDate& a){Structure.SetFirstIPDdate(a);}
	void SetCCCcurve(const QString& a){Structure.SetCCCcurve(a);}
	void SetLastIPDdate(const QDate& a){Structure.SetLastIPDdate(a);}
	void SetCallDate(const QDate& a){Structure.SetCallDate(a);}
	void SetupReinvestmentTest(const QDate& ReinvPer,double TstLvl, double IIshare,double IRshare,double OIshare,double ORshare){Structure.SetupReinvestmentTest(ReinvPer,TstLvl,IIshare,IRshare,OIshare,ORshare);}
	void SetupReinvBond(const QString& IntrVec,const QString& CPRVec,const QString& CDRVec,const QString& LSVec,const QString& WALval,int PayFreq=1,const QString& AnnuityVec="N",double FloatingBase=0.0){Structure.SetupReinvBond(IntrVec,CPRVec,CDRVec,LSVec,WALval,PayFreq,AnnuityVec,FloatingBase);}
	void Reset();
	void SetupStress(const QString& ConstPar,QList<QString> XSpann,QList<QString> YSpann, StressTest::StressVariability Xvar=StressTest::ChangingCDR,StressTest::StressVariability Yvar=StressTest::ChangingLS);
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
	void SetLossOutputAddress(const QString& a){LossOutputAddress=a;}
	void SetLossOnCallOutputAddress(const QString& a){LossOnCallOutputAddress=a;}
	void SetCreditEnanAddress(const QString& a){CreditEnanAddress=a;}
	void SetStressToCall(bool a){StressToCall=a;}
private:
	WaterfallCalculator* ParallWatFalls;
	MtgCalculator LoansCalculator;
	Waterfall Structure;
	Waterfall CallStructure;
	StressTest* Stresser;
	QDate PoolCutOff;
	bool RunCall;
	bool RunStress;
	bool StressToCall;
	QString MtgOutputAddress;
	QString TranchesOutputAddress;
	QString CallTranchesOutputAddress;
	QString LossOutputAddress;
	QString LossOnCallOutputAddress;
	QString CreditEnanAddress;
	QString FolderPath;
signals:
	void LoopStarted();
	void StressLoopStarted();
private slots:
	void CalculationStep2();
	void CalculationStep1();
	void CheckCalculationDone();
	void StressFinished();


};
#endif // CentralUnit_h__