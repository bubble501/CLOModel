#ifndef ReinvestmentTest_h__
#define ReinvestmentTest_h__

#include <QDate>
#include <QString>
#include "Mortgage.h"
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
	const QDate& GetReinvestmentPeriod() const {return ReinvestmentPeriod;}
	double GetTestLevel() const{return ReinvestmentTestLevel;}
	double GetShare(ShareType Tpe){return ReinvestmentShare[Tpe];}
	const Mortgage& GetReinvestmentBond() const {return ReinvestmentBond;}
	const QString& GetCPRAssumption() const {return CPRAssumption;}
	const QString& GetCDRAssumption() const {return CDRAssumption;}
	const QString& GetLSAssumption() const {return LSAssumption;}
	const QString& GetWALAssumption() const {return WALAssumption;}
	void SetupReinvBond(const QString& IntrVec,const QString& CPRVec,const QString& CDRVec,const QString& LSVec,const QString& WALval,int PayFreq=1,const QString& AnnuityVec="N",double FloatingBase=0.0);
	void SetupTest(const QDate& ReinvPer,double TstLvl, double IIshare,double IRshare,double OIshare,double ORshare);
	void CalculateBondCashFlows(double Size, QDate StartDate, int Period=0);
	const MtgCashFlow& GetBondCashFlow() const;
	void SetCPR(const QString& a);
	void SetCDR(const QString& a);
	void SetLS(const QString& a);
	friend QDataStream& operator<<(QDataStream & stream, const ReinvestmentTest& flows);
	friend QDataStream& operator>>(QDataStream & stream, ReinvestmentTest& flows);
private:
	QDate ReinvestmentPeriod;
	double ReinvestmentTestLevel;
	double ReinvestmentShare[4];
	Mortgage ReinvestmentBond;
	QString CPRAssumption;
	QString CDRAssumption;
	QString LSAssumption;
	QString WALAssumption;

};
QDataStream& operator<<(QDataStream & stream, const ReinvestmentTest& flows);
QDataStream& operator>>(QDataStream & stream, ReinvestmentTest& flows);
#endif // ReinvestmentTest_h__
