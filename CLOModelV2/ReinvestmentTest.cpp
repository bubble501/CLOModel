#include "ReinvestmentTest.h"
#include "CommonFunctions.h"
#include <QtGlobal>
void ReinvestmentTest::SetupReinvBond(
	const QString& IntrVec,
	const QString& CPRVec,
	const QString& CDRVec,
	const QString& LSVec,
	const QString& WALval,
	const QString& PayFreq,
	const QString& AnnuityVec,
	const QString& ReinvPric,
	const QString& ReinvDel,
	const QString& ReinvSpr,
	const QString& FloatingBase,
	const QString& RecoveryLag,
	const QString& Delinquency,
	const QString& DelinquencyLag
){
	ReinvestmentBond.SetAnnuity(AnnuityVec);
	ReinvestmentBond.SetFloatingRateBase(FloatingBase);
	ReinvestmentBond.SetInterest(IntrVec);
	ReinvestmentBond.SetPaymentFreq(PayFreq);
	m_ReinvestmentDelay = ReinvDel;
	ReinvestmentPrice = ReinvPric;
	CDRAssumption=CDRVec;
	CPRAssumption=CPRVec;
	LSAssumption=LSVec;
	WALAssumption=WALval;
	m_RecoveryLag = RecoveryLag;
	m_Delinquency = Delinquency;
	m_DelinquencyLag = DelinquencyLag;
	m_ReinvestmentSpreadOverTime = ReinvSpr;
}
void ReinvestmentTest::SetReinvestementPeriod(
	const QDate& ReinvPer

){
	ReinvestmentPeriod=ReinvPer;

}
ReinvestmentTest::ReinvestmentTest()
	:CDRAssumption("0")
	,CPRAssumption("0")
	,LSAssumption("100")
	,WALAssumption("1")
	, m_ReinvestmentDelay("0")
	, ReinvestmentPrice("100")
	, m_RecoveryLag("0")
	, m_Delinquency("0")
	, m_DelinquencyLag("0")
	, m_ReinvestmentSpreadOverTime("0")
{
	WALAssumption.SetDivisor(1.0);
}
ReinvestmentTest::ReinvestmentTest(const ReinvestmentTest& a)
	:CDRAssumption(a.CDRAssumption)
	,CPRAssumption(a.CPRAssumption)
	,LSAssumption(a.LSAssumption)
	,WALAssumption(a.WALAssumption)
	,ReinvestmentBond(a.ReinvestmentBond)
	,ReinvestmentPeriod(a.ReinvestmentPeriod)
	, m_ReinvestmentDelay(a.m_ReinvestmentDelay)
	, ReinvestmentPrice(a.ReinvestmentPrice)
	, m_RecoveryLag(a.m_RecoveryLag)
	, m_Delinquency(a.m_Delinquency)
	, m_DelinquencyLag(a.m_DelinquencyLag)
	, m_ReinvestmentSpreadOverTime(a.m_ReinvestmentSpreadOverTime)
{
}
ReinvestmentTest& ReinvestmentTest::operator=(const ReinvestmentTest& a){
	CDRAssumption=a.CDRAssumption;
	CPRAssumption=a.CPRAssumption;
	LSAssumption=a.LSAssumption;
	WALAssumption=a.WALAssumption;
	ReinvestmentBond=a.ReinvestmentBond;
	ReinvestmentPeriod=a.ReinvestmentPeriod;
	m_ReinvestmentDelay = a.m_ReinvestmentDelay;
	ReinvestmentPrice = a.ReinvestmentPrice;
	m_RecoveryLag = a.m_RecoveryLag;
	m_Delinquency = a.m_Delinquency;
	m_DelinquencyLag = a.m_DelinquencyLag;
	m_ReinvestmentSpreadOverTime = a.m_ReinvestmentSpreadOverTime;
	return *this;
}
void ReinvestmentTest::CalculateBondCashFlows(double Size, QDate StartDate, int Period){
	ReinvestmentBond.ResetFlows();
	bool NullDates[] = {
		CPRAssumption.GetAnchorDate().isNull()
		, CDRAssumption.GetAnchorDate().isNull()
		, LSAssumption.GetAnchorDate().isNull()
		, m_RecoveryLag.GetAnchorDate().isNull()
		, m_Delinquency.GetAnchorDate().isNull()
		, m_DelinquencyLag.GetAnchorDate().isNull()
	};
	if (NullDates[0]) CPRAssumption.SetAnchorDate(StartDate.addMonths(-Period));
	if (NullDates[1]) CDRAssumption.SetAnchorDate(StartDate.addMonths(-Period));
	if (NullDates[2]) LSAssumption.SetAnchorDate(StartDate.addMonths(-Period));
	if (NullDates[3]) m_RecoveryLag.SetAnchorDate(StartDate.addMonths(-Period));
	if (NullDates[4]) m_Delinquency.SetAnchorDate(StartDate.addMonths(-Period));
	if (NullDates[5]) m_DelinquencyLag.SetAnchorDate(StartDate.addMonths(-Period));
	double CurrentPrice;
	int CurrentDelay = m_ReinvestmentDelay.GetValue(StartDate);
	int currentSpread = m_ReinvestmentSpreadOverTime.GetValue(StartDate);
	Size /= static_cast<double>(currentSpread);
	MtgCashFlow ResultingFlows;
	for (int SpreadIter = 0; SpreadIter < currentSpread; SpreadIter++) {
		if (ReinvestmentPrice.GetAnchorDate().isNull()) CurrentPrice = ReinvestmentPrice.GetValue(Period + CurrentDelay + SpreadIter);
		else CurrentPrice = ReinvestmentPrice.GetValue(StartDate.addMonths(CurrentDelay + SpreadIter));
		ReinvestmentBond.SetProperty("Price", QString("%1").arg(CurrentPrice));
		ReinvestmentBond.SetSize(Size / qMax(CurrentPrice, 0.01));
		if (WALAssumption.GetAnchorDate().isNull()) ReinvestmentBond.SetMaturityDate(StartDate.addMonths(CurrentDelay + SpreadIter).addDays(RoundUp(365.25*WALAssumption.GetValue(Period + CurrentDelay + SpreadIter))));
		else ReinvestmentBond.SetMaturityDate(StartDate.addMonths(CurrentDelay + SpreadIter).addDays(RoundUp(365.25*WALAssumption.GetValue(StartDate.addMonths(CurrentDelay + SpreadIter)))));
		ReinvestmentBond.CalculateCashFlows(
			StartDate.addMonths(CurrentDelay + SpreadIter)
			, CPRAssumption
			, CDRAssumption
			, LSAssumption
			, m_RecoveryLag
			, m_Delinquency
			, m_DelinquencyLag
			, true
			);
		for (int DelayIter = 0; DelayIter < CurrentDelay + SpreadIter; DelayIter++) {
			ReinvestmentBond.AddCashFlow(StartDate.addMonths(DelayIter), Size, MtgCashFlow::MtgFlowType::AmountOutstandingFlow);
		}
		ResultingFlows.AddFlow(ReinvestmentBond.GetCashFlow());
		ReinvestmentBond.ResetFlows();
	}
	ReinvestmentBond.AddCashFlow(ResultingFlows);
	if (NullDates[0]) CPRAssumption.RemoveAnchorDate();
	if (NullDates[1]) CDRAssumption.RemoveAnchorDate();
	if (NullDates[2]) LSAssumption.RemoveAnchorDate();
	if (NullDates[3]) m_RecoveryLag.RemoveAnchorDate();
	if (NullDates[4]) m_Delinquency.RemoveAnchorDate();
	if (NullDates[5]) m_DelinquencyLag.RemoveAnchorDate();
}
void ReinvestmentTest::SetMissingAnchors(const QDate& a) {
	if (WALAssumption.GetAnchorDate().isNull()) WALAssumption.SetAnchorDate(a);
	if (ReinvestmentPrice.GetAnchorDate().isNull()) ReinvestmentPrice.SetAnchorDate(a);
	if (m_ReinvestmentDelay.GetAnchorDate().isNull()) m_ReinvestmentDelay.SetAnchorDate(a);
	if (m_ReinvestmentSpreadOverTime.GetAnchorDate().isNull()) m_ReinvestmentSpreadOverTime.SetAnchorDate(a);
}
const MtgCashFlow& ReinvestmentTest::GetBondCashFlow() const{
	return ReinvestmentBond.GetCashFlow();
}
void ReinvestmentTest::SetCPR(const QString& a){CPRAssumption=a;}
void ReinvestmentTest::SetCDR(const QString& a){CDRAssumption=a;}
void ReinvestmentTest::SetLS(const QString& a){LSAssumption=a;}
void ReinvestmentTest::SetReinvestmentDelay(const QString& a) { m_ReinvestmentDelay = a; }
void ReinvestmentTest::SetRecoveryLag(const QString& a) { m_RecoveryLag = a; }
void ReinvestmentTest::SetDelinquency(const QString& a) { m_Delinquency = a; }
void ReinvestmentTest::SetDelinquencyLag(const QString& a) { m_DelinquencyLag = a; }
QDataStream& operator<<(QDataStream & stream, const ReinvestmentTest& flows){
	stream 
		<< flows.ReinvestmentPeriod
		<< flows.ReinvestmentBond
		<< flows.CPRAssumption
		<< flows.CDRAssumption
		<< flows.LSAssumption
		<< flows.WALAssumption
		<< flows.m_ReinvestmentDelay
		<< flows.ReinvestmentPrice
		<< flows.m_RecoveryLag
		<< flows.m_Delinquency
		<< flows.m_DelinquencyLag
		<< flows.m_ReinvestmentSpreadOverTime
	;
	return stream;
}
QDataStream& ReinvestmentTest::LoadOldVersion(QDataStream& stream) {
	stream
		>> ReinvestmentPeriod;
	ReinvestmentBond.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> ReinvestmentBond;
	CPRAssumption.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> CPRAssumption;
	CDRAssumption.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> CDRAssumption;
	LSAssumption.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> LSAssumption;
	WALAssumption.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> WALAssumption;
	m_ReinvestmentDelay.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_ReinvestmentDelay;
	ReinvestmentPrice.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> ReinvestmentPrice;
	m_RecoveryLag.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_RecoveryLag;
	m_Delinquency.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_Delinquency;
	m_DelinquencyLag.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_DelinquencyLag;
	m_ReinvestmentSpreadOverTime.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_ReinvestmentSpreadOverTime;
	ResetProtocolVersion();
	return stream;
}



QDataStream& operator>>(QDataStream & stream, ReinvestmentTest& flows) {
	return flows.LoadOldVersion(stream);
}