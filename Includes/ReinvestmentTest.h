#ifndef ReinvestmentTest_h__
#define ReinvestmentTest_h__
#include <QDate>
#include <QString>
#include <Mortgage.h>
class ReinvestmentTest{
public:
	enum ShareType{
		InReinvShare,
		InRedempShare,
		OutReinvShare,
		OutRedempShare
	};
	ReinvestmentTest();
	ReinvestmentTest(const ReinvestmentTest& a);
	ReinvestmentTest& operator=(const ReinvestmentTest& a);
	const QDate& GetReinvestmentPeriod() const;
	double GetTestLevel() const;
	double GetShare(ShareType Tpe)const;
	const Mortgage& GetReinvestmentBond() const;
	const QString& GetCPRAssumption() const;
	const QString& GetCDRAssumption() const;
	const QString& GetLSAssumption() const;
	const QString& GetWALAssumption() const;
	void SetupReinvBond(const QString& IntrVec,const QString& CPRVec,const QString& CDRVec,const QString& LSVec,const QString& WALval,int PayFreq=1,const QString& AnnuityVec="N",double FloatingBase=0.0);
	void SetupTest(const QDate& ReinvPer,double TstLvl, double IIshare,double IRshare,double OIshare,double ORshare);
	void CalculateBondCashFlows(double Size, QDate StartDate, int Period=0);
	const MtgCashFlow& GetBondCashFlow() const;
	void SetCPR(const QString& a);
	void SetCDR(const QString& a);
	void SetLS(const QString& a);
};
#endif // ReinvestmentTest_h__
