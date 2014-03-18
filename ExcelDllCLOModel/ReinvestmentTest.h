#ifndef ReinvestmentTest_h__
#define ReinvestmentTest_h__

#include <QDate>
#include <QString>
#include "Mortgage.h"
#include "BloombergVector.h"
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
	double GetShare(ShareType Tpe)const{return ReinvestmentShare[Tpe];}
	const Mortgage& GetReinvestmentBond() const {return ReinvestmentBond;}
	QString GetCPRAssumption() const {return CPRAssumption.GetVector();}
	QString GetCDRAssumption() const {return CDRAssumption.GetVector();}
	QString GetLSAssumption() const {return LSAssumption.GetVector();}
	QString GetWALAssumption() const {return WALAssumption.GetVector();}
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
	BloombergVector CPRAssumption;
	BloombergVector CDRAssumption;
	BloombergVector LSAssumption;
	BloombergVector WALAssumption;

};
QDataStream& operator<<(QDataStream & stream, const ReinvestmentTest& flows);
QDataStream& operator>>(QDataStream & stream, ReinvestmentTest& flows);
#endif // ReinvestmentTest_h__
