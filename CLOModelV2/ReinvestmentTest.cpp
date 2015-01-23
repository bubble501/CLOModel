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
	ReinvestmentBond.SetFloatingRateBase(FloatingBase.isEmpty() ? QString("ZERO") : FloatingBase);
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
	m_Reinvested.Aggregate(GenericCashFlow::Monthly);
	m_Reinvested.SetLabel(static_cast<qint32>(MtgCashFlow::MtgFlowType::PrincipalFlow), "Reinvested Funds");
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
	, m_Reinvested(a.m_Reinvested)
	, ReinvestQueue(a.ReinvestQueue)
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
	m_Reinvested = a.m_Reinvested;
	ReinvestQueue = a.ReinvestQueue;
	return *this;
}
void ReinvestmentTest::CalculateBondCashFlows(double Size, QDate StartDate, int Period, const QDate& MaxMaturity) {
	Q_ASSERT_X(Period >= 0, "CalculateBondCashFlows", "Trying to calculate cash flows with negative period");
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
	if (ReinvestmentPrice.GetAnchorDate().isNull()) CurrentPrice = ReinvestmentPrice.GetValue(Period);
	else CurrentPrice = ReinvestmentPrice.GetValue(StartDate);
	ReinvestmentBond.SetProperty("Price", QString("%1").arg(CurrentPrice*100.0));
	ReinvestmentBond.SetProperty("PurchasePrice", QString("%1").arg(CurrentPrice*100.0));
	ReinvestmentBond.SetSize(Size / qMax(CurrentPrice, 0.01));
	if (WALAssumption.GetAnchorDate().isNull()) ReinvestmentBond.SetMaturityDate(StartDate.addDays(RoundUp(365.25*WALAssumption.GetValue(Period))));
	else ReinvestmentBond.SetMaturityDate(StartDate.addDays(RoundUp(365.25*WALAssumption.GetValue(StartDate))));
	if (!MaxMaturity.isNull() && ReinvestmentBond.GetMaturityDate() > MaxMaturity) ReinvestmentBond.SetMaturityDate(MaxMaturity);
	ReinvestmentBond.CalculateCashFlows(
		StartDate
		, CPRAssumption
		, CDRAssumption
		, LSAssumption
		, m_RecoveryLag
		, m_Delinquency
		, m_DelinquencyLag
		, true
		,false
		);
	//ReinvestmentBond.AddCashFlow(StartDate, Size, MtgCashFlow::MtgFlowType::AmountOutstandingFlow);
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
		<< flows.m_Reinvested
		<< flows.ReinvestQueue
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
	m_Reinvested.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_Reinvested;
	stream >> ReinvestQueue;
	ResetProtocolVersion();
	return stream;
}

void ReinvestmentTest::QueueReinvestments(double Amount, const QDate& CurrentDate) {
	if (Amount < 0.01) return;
	int CurrentDelay = m_ReinvestmentDelay.GetValue(CurrentDate);
	int CurrentWindow =m_ReinvestmentSpreadOverTime.GetValue(CurrentDate.addMonths(CurrentDelay));
	for (QDate ReinvIter = CurrentDate.addMonths(CurrentDelay); ReinvIter < CurrentDate.addMonths(CurrentWindow + CurrentDelay); ReinvIter = ReinvIter.addMonths(1)) {
		QDate NormalisedIter = QDate(ReinvIter.year(), ReinvIter.month(), 15);
		if (ReinvestQueue.contains(NormalisedIter)) ReinvestQueue[NormalisedIter] += (Amount / static_cast<double>(CurrentWindow));
		else ReinvestQueue.insert(NormalisedIter, Amount / static_cast<double>(CurrentWindow));
	}
}

const MtgCashFlow& ReinvestmentTest::ProcessQueue(const QDate& CurrentDate, int Period, const QDate& MaxMaturity) {
	QDate Normaliseddate(CurrentDate.year(), CurrentDate.month(), 15);
	ReinvestmentBond.ResetFlows();
	auto ReinvIter = ReinvestQueue.find(Normaliseddate);
	if (ReinvIter == ReinvestQueue.end()) return ReinvestmentBond.GetCashFlow();
	CalculateBondCashFlows(ReinvIter.value(), CurrentDate, Period, MaxMaturity);
	m_Reinvested.AddFlow(CurrentDate, qMax(0.0,ReinvIter.value() - ReinvestmentBond.GetCashFlow().GetTotalFlow(CurrentDate)), static_cast<qint32>(MtgCashFlow::MtgFlowType::PrincipalFlow));
	ReinvestQueue.erase(ReinvIter);
	return ReinvestmentBond.GetCashFlow();
}

double ReinvestmentTest::GetQueuedCash(QDate StartDate) const {
	if (!StartDate.isNull()) StartDate.setDate(StartDate.year(), StartDate.month(), 15);
	double Result = 0.0;
	for (auto i = ReinvestQueue.begin(); i != ReinvestQueue.end(); ++i) {
		if (!StartDate.isNull() && i.key()<StartDate) continue;
		Result += i.value();
	}
	return Result;
}

void ReinvestmentTest::RemoveBondFlow(const QDate& a) {
	if (a.isNull())return;
	MtgCashFlow& crurrFlows=ReinvestmentBond.GetCashFlow();
	crurrFlows.SetFlow(a, 0.0, MtgCashFlow::MtgFlowType::PrincipalFlow);
	crurrFlows.SetFlow(a, 0.0, MtgCashFlow::MtgFlowType::InterestFlow);
	crurrFlows.SetFlow(a, 0.0, MtgCashFlow::MtgFlowType::PrepaymentFlow);
}

QDataStream& operator>>(QDataStream & stream, ReinvestmentTest& flows) {
	return flows.LoadOldVersion(stream);
}