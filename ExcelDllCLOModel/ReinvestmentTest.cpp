#include "ReinvestmentTest.h"
#include "CommonFunctions.h"
#include <QtGlobal>
void ReinvestmentTest::SetupReinvBond(
	const QString& IntrVec,
	const QString& CPRVec,
	const QString& CDRVec,
	const QString& LSVec,
	const QString& WALval,
	int PayFreq,
	const QString& AnnuityVec,
	double FloatingBase
){
	ReinvestmentBond.SetAnnuity(AnnuityVec);
	ReinvestmentBond.SetFloatingRateBase(FloatingBase);
	ReinvestmentBond.SetInterest(IntrVec);
	ReinvestmentBond.SetPaymentFreq(PayFreq);
	CDRAssumption=CDRVec;
	CPRAssumption=CPRVec;
	LSAssumption=LSVec;
	WALAssumption=WALval;
}
void ReinvestmentTest::SetupTest(
	const QDate& ReinvPer
	,double TstLvl
	,double IIshare
	,double IRshare
	,double OIshare
	,double ORshare
){
	ReinvestmentPeriod=ReinvPer;
	ReinvestmentTestLevel=TstLvl;
	if(IIshare>=0.0 && IIshare<=1.0) ReinvestmentShare[InReinvShare]=IIshare;
	if(IRshare>=0.0 && IRshare<=1.0) ReinvestmentShare[InRedempShare]=IRshare;
	if(OIshare>=0.0 && OIshare<=1.0) ReinvestmentShare[OutReinvShare]=OIshare;
	if(ORshare>=0.0 && ORshare<=1.0) ReinvestmentShare[OutRedempShare]=ORshare;
}
ReinvestmentTest::ReinvestmentTest()
	:CDRAssumption("0")
	,CPRAssumption("0")
	,LSAssumption("100")
	,WALAssumption("1")
{
	ReinvestmentShare[InReinvShare]=0.0;
	ReinvestmentShare[InRedempShare]=0.0;
	ReinvestmentShare[OutReinvShare]=0.0;
	ReinvestmentShare[OutRedempShare]=0.0;
}
ReinvestmentTest::ReinvestmentTest(const ReinvestmentTest& a)
	:CDRAssumption(a.CDRAssumption)
	,CPRAssumption(a.CPRAssumption)
	,LSAssumption(a.LSAssumption)
	,WALAssumption(a.WALAssumption)
	,ReinvestmentBond(a.ReinvestmentBond)
	,ReinvestmentPeriod(a.ReinvestmentPeriod)
	,ReinvestmentTestLevel(a.ReinvestmentTestLevel)
{
		ReinvestmentShare[InReinvShare]=a.ReinvestmentShare[InReinvShare];
		ReinvestmentShare[InRedempShare]=a.ReinvestmentShare[InRedempShare];
		ReinvestmentShare[OutReinvShare]=a.ReinvestmentShare[OutReinvShare];
		ReinvestmentShare[OutRedempShare]=a.ReinvestmentShare[OutRedempShare];
}
ReinvestmentTest& ReinvestmentTest::operator=(const ReinvestmentTest& a){
	ReinvestmentShare[InReinvShare]=a.ReinvestmentShare[InReinvShare];
	ReinvestmentShare[InRedempShare]=a.ReinvestmentShare[InRedempShare];
	ReinvestmentShare[OutReinvShare]=a.ReinvestmentShare[OutReinvShare];
	ReinvestmentShare[OutRedempShare]=a.ReinvestmentShare[OutRedempShare];
	CDRAssumption=a.CDRAssumption;
	CPRAssumption=a.CPRAssumption;
	LSAssumption=a.LSAssumption;
	WALAssumption=a.WALAssumption;
	ReinvestmentBond=a.ReinvestmentBond;
	ReinvestmentPeriod=a.ReinvestmentPeriod;
	ReinvestmentTestLevel=a.ReinvestmentTestLevel;
	return *this;
}
void ReinvestmentTest::CalculateBondCashFlows(double Size, QDate StartDate, int Period){
	ReinvestmentBond.ResetFlows();
	ReinvestmentBond.SetSize(Size);
	QList<double>UnpackedWAL=UnpackVect(WALAssumption,1,false);
	ReinvestmentBond.SetMaturityDate(StartDate.addDays(RoundUp(365.25*100.0*UnpackedWAL.at(qMin(UnpackedWAL.size()-1,Period)))));
	ReinvestmentBond.CalculateCashFlows(
		ShiftBloombergVector(CPRAssumption,Period)
		,ShiftBloombergVector(CDRAssumption,Period)
		,ShiftBloombergVector(LSAssumption,Period)
		,StartDate
	);
}
const MtgCashFlow& ReinvestmentTest::GetBondCashFlow() const{
	return ReinvestmentBond.GetCashFlow();
}
void ReinvestmentTest::SetCPR(const QString& a){if(ValidBloombergVector(a)) CPRAssumption=a;}
void ReinvestmentTest::SetCDR(const QString& a){if(ValidBloombergVector(a)) CDRAssumption=a;}
void ReinvestmentTest::SetLS(const QString& a){if(ValidBloombergVector(a)) LSAssumption=a;}
QDataStream& operator<<(QDataStream & stream, const ReinvestmentTest& flows){
	stream 
		<< flows.ReinvestmentPeriod
		<< flows.ReinvestmentTestLevel
		<< flows.ReinvestmentShare[0]
		<< flows.ReinvestmentShare[1]
		<< flows.ReinvestmentShare[2]
		<< flows.ReinvestmentShare[3]
		<< flows.ReinvestmentBond
		<< flows.CPRAssumption
		<< flows.CDRAssumption
		<< flows.LSAssumption
		<< flows.WALAssumption
	;
	return stream;
}
QDataStream& operator>>(QDataStream & stream, ReinvestmentTest& flows){
	stream 
		>> flows.ReinvestmentPeriod
		>> flows.ReinvestmentTestLevel
		>> flows.ReinvestmentShare[0]
		>> flows.ReinvestmentShare[1]
		>> flows.ReinvestmentShare[2]
		>> flows.ReinvestmentShare[3]
		>> flows.ReinvestmentBond
		>> flows.CPRAssumption
		>> flows.CDRAssumption
		>> flows.LSAssumption
		>> flows.WALAssumption
	;
	return stream;
}