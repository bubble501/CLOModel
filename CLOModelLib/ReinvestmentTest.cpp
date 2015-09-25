#include "ReinvestmentTest.h"
#include "Private/ReinvestmentTest_p.h"
#include "CommonFunctions.h"
DEFINE_PUBLIC_COMMONS(ReinvestmentTest)
DEFINE_PUBLIC_COMMONS_COPY(ReinvestmentTest)
ReinvestmentTest::~ReinvestmentTest()
{

}
ReinvestmentTestPrivate::~ReinvestmentTestPrivate()
{

}
ReinvestmentTestPrivate::ReinvestmentTestPrivate(ReinvestmentTest *q,const ReinvestmentTestPrivate& other)
	:BackwardInterfacePrivate(q,other)
    , CDRAssumption(other.CDRAssumption)
    , CPRAssumption(other.CPRAssumption)
    , LSAssumption(other.LSAssumption)
    , WALAssumption(other.WALAssumption)
    , ReinvestmentBond(other.ReinvestmentBond)
    , ReinvestmentPeriod(other.ReinvestmentPeriod)
    , m_ReinvestmentDelay(other.m_ReinvestmentDelay)
    , ReinvestmentPrice(other.ReinvestmentPrice)
    , m_RecoveryLag(other.m_RecoveryLag)
    , m_Delinquency(other.m_Delinquency)
    , m_DelinquencyLag(other.m_DelinquencyLag)
    , m_ReinvestmentSpreadOverTime(other.m_ReinvestmentSpreadOverTime)
    , m_Reinvested(other.m_Reinvested)
    , ReinvestQueue(other.ReinvestQueue)
{}
ReinvestmentTestPrivate::ReinvestmentTestPrivate(ReinvestmentTest *q)
	:BackwardInterfacePrivate(q)
    , CDRAssumption("0")
    , CPRAssumption("0")
    , LSAssumption("100")
    , WALAssumption("1")
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
ReinvestmentTest::ReinvestmentTest(ReinvestmentTestPrivate *d, const ReinvestmentTest& other)
	:BackwardInterface(d,other)
{}
ReinvestmentTest& ReinvestmentTest::operator=(const ReinvestmentTest& other){
	Q_D(ReinvestmentTest);
	BackwardInterface::operator=(other);
    d->CDRAssumption = other.d_func()->CDRAssumption;
    d->CPRAssumption = other.d_func()->CPRAssumption;
    d->LSAssumption = other.d_func()->LSAssumption;
    d->WALAssumption = other.d_func()->WALAssumption;
    d->ReinvestmentBond = other.d_func()->ReinvestmentBond;
    d->ReinvestmentPeriod = other.d_func()->ReinvestmentPeriod;
    d->m_ReinvestmentDelay = other.d_func()->m_ReinvestmentDelay;
    d->ReinvestmentPrice = other.d_func()->ReinvestmentPrice;
    d->m_RecoveryLag = other.d_func()->m_RecoveryLag;
    d->m_Delinquency = other.d_func()->m_Delinquency;
    d->m_DelinquencyLag = other.d_func()->m_DelinquencyLag;
    d->m_ReinvestmentSpreadOverTime = other.d_func()->m_ReinvestmentSpreadOverTime;
    d->m_Reinvested = other.d_func()->m_Reinvested;
    d->ReinvestQueue = other.d_func()->ReinvestQueue;
	return *this;
}
ReinvestmentTest::ReinvestmentTest(ReinvestmentTestPrivate *d)
	:BackwardInterface(d)
{}

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
    )
{
    Q_D(ReinvestmentTest);
    d->ReinvestmentBond.SetAnnuity(AnnuityVec);
    d->ReinvestmentBond.SetFloatingRateBase(FloatingBase.isEmpty() ? QString("ZERO") : FloatingBase);
    d->ReinvestmentBond.SetInterest(IntrVec);
    d->ReinvestmentBond.SetPaymentFreq(PayFreq);
    d->m_ReinvestmentDelay = ReinvDel;
    d->ReinvestmentPrice = ReinvPric;
    d->CDRAssumption = CDRVec;
    d->CPRAssumption = CPRVec;
    d->LSAssumption = LSVec;
    d->WALAssumption = WALval;
    d->m_RecoveryLag = RecoveryLag;
    d->m_Delinquency = Delinquency;
    d->m_DelinquencyLag = DelinquencyLag;
    d->m_ReinvestmentSpreadOverTime = ReinvSpr;
}
void ReinvestmentTest::SetReinvestementPeriod(const QDate& ReinvPer)
{
    Q_D(ReinvestmentTest);
    d->ReinvestmentPeriod = ReinvPer;

}

void ReinvestmentTest::CalculateBondCashFlows(double Size, QDate StartDate, int Period, const QDate& MaxMaturity) {
    Q_D(ReinvestmentTest);
	Q_ASSERT_X(Period >= 0, "CalculateBondCashFlows", "Trying to calculate cash flows with negative period");
    d->ReinvestmentBond.ResetFlows();
	bool NullDates[] = {
        d->CPRAssumption.GetAnchorDate().isNull()
        , d->CDRAssumption.GetAnchorDate().isNull()
        , d->LSAssumption.GetAnchorDate().isNull()
        , d->m_RecoveryLag.GetAnchorDate().isNull()
        , d->m_Delinquency.GetAnchorDate().isNull()
        , d->m_DelinquencyLag.GetAnchorDate().isNull()
	};
    if (NullDates[0]) d->CPRAssumption.SetAnchorDate(StartDate.addMonths(-Period));
    if (NullDates[1]) d->CDRAssumption.SetAnchorDate(StartDate.addMonths(-Period));
    if (NullDates[2]) d->LSAssumption.SetAnchorDate(StartDate.addMonths(-Period));
    if (NullDates[3]) d->m_RecoveryLag.SetAnchorDate(StartDate.addMonths(-Period));
    if (NullDates[4]) d->m_Delinquency.SetAnchorDate(StartDate.addMonths(-Period));
    if (NullDates[5]) d->m_DelinquencyLag.SetAnchorDate(StartDate.addMonths(-Period));
	double CurrentPrice;
    if (d->ReinvestmentPrice.GetAnchorDate().isNull()) CurrentPrice = d->ReinvestmentPrice.GetValue(Period);
    else CurrentPrice = d->ReinvestmentPrice.GetValue(StartDate);
    d->ReinvestmentBond.SetProperty("Price", QString("%1").arg(CurrentPrice*100.0));
    d->ReinvestmentBond.SetProperty("PurchasePrice", QString("%1").arg(CurrentPrice*100.0));
    d->ReinvestmentBond.SetSize(Size / qMax(CurrentPrice, 0.01));
    if (d->WALAssumption.GetAnchorDate().isNull()) d->ReinvestmentBond.SetMaturityDate(StartDate.addDays(std::ceil(365.25*d->WALAssumption.GetValue(Period))));
    else d->ReinvestmentBond.SetMaturityDate(StartDate.addDays(std::ceil(365.25*d->WALAssumption.GetValue(StartDate))));
    if (!MaxMaturity.isNull() && d->ReinvestmentBond.GetMaturityDate() > MaxMaturity) d->ReinvestmentBond.SetMaturityDate(MaxMaturity);
    d->ReinvestmentBond.CalculateCashFlows(
		StartDate
        , d->CPRAssumption
        , d->CDRAssumption
        , d->LSAssumption
        , d->m_RecoveryLag
        , d->m_Delinquency
        , d->m_DelinquencyLag
		, true
		, false
		);
	//ReinvestmentBond.AddCashFlow(StartDate, Size, MtgCashFlow::MtgFlowType::AmountOutstandingFlow);
    if (NullDates[0]) d->CPRAssumption.RemoveAnchorDate();
    if (NullDates[1]) d->CDRAssumption.RemoveAnchorDate();
    if (NullDates[2]) d->LSAssumption.RemoveAnchorDate();
    if (NullDates[3]) d->m_RecoveryLag.RemoveAnchorDate();
    if (NullDates[4]) d->m_Delinquency.RemoveAnchorDate();
    if (NullDates[5]) d->m_DelinquencyLag.RemoveAnchorDate();
}
void ReinvestmentTest::SetMissingAnchors(const QDate& a) {
    Q_D(ReinvestmentTest);
    if (d->WALAssumption.GetAnchorDate().isNull()) 
        d->WALAssumption.SetAnchorDate(a);
    if (d->ReinvestmentPrice.GetAnchorDate().isNull()) 
        d->ReinvestmentPrice.SetAnchorDate(a);
    if (d->m_ReinvestmentDelay.GetAnchorDate().isNull()) 
        d->m_ReinvestmentDelay.SetAnchorDate(a);
    if (d->m_ReinvestmentSpreadOverTime.GetAnchorDate().isNull()) 
        d->m_ReinvestmentSpreadOverTime.SetAnchorDate(a);
}
const MtgCashFlow& ReinvestmentTest::GetBondCashFlow() const{
    Q_D(const ReinvestmentTest);
    return d->ReinvestmentBond.GetCashFlow();
}
void ReinvestmentTest::SetCPR(const QString& a)
{
    Q_D( ReinvestmentTest);
    d->CPRAssumption = a;
}
void ReinvestmentTest::SetCDR(const QString& a)
{
    Q_D( ReinvestmentTest);
    d->CDRAssumption = a;
}
void ReinvestmentTest::SetLS(const QString& a)
{
    Q_D( ReinvestmentTest);
    d->LSAssumption = a;
}
void ReinvestmentTest::SetReinvestmentDelay(const QString& a)
{
    Q_D( ReinvestmentTest);
    d->m_ReinvestmentDelay = a;
}
void ReinvestmentTest::SetRecoveryLag(const QString& a)
{
    Q_D( ReinvestmentTest);
    d->m_RecoveryLag = a;
}
void ReinvestmentTest::SetDelinquency(const QString& a)
{
    Q_D( ReinvestmentTest);
    d->m_Delinquency = a;
}
void ReinvestmentTest::SetDelinquencyLag(const QString& a)
{
    Q_D( ReinvestmentTest);
    d->m_DelinquencyLag = a;
}

const IntegerVector& ReinvestmentTest::GetDelinquencyLag() const
{
    Q_D(const ReinvestmentTest);
    return d->m_DelinquencyLag;
}

const IntegerVector& ReinvestmentTest::GetRecoveryLag() const
{
    Q_D(const ReinvestmentTest);
    return d->m_RecoveryLag;
}

const BloombergVector& ReinvestmentTest::GetDelinquency() const
{
    Q_D(const ReinvestmentTest);
    return d->m_Delinquency;
}

IntegerVector ReinvestmentTest::GetReinvestmentSpreadOverTime() const
{
    Q_D(const ReinvestmentTest);
    return d->m_ReinvestmentSpreadOverTime.GetVector();
}

void ReinvestmentTest::SetReinvestmentSpreadOverTime(const QString& val)
{
    Q_D( ReinvestmentTest);
    d->m_ReinvestmentSpreadOverTime = val;
}

QDataStream& operator<<(QDataStream & stream, const ReinvestmentTest& flows)
{
	stream 
		<< flows.d_func()->ReinvestmentPeriod
        << flows.d_func()->ReinvestmentBond
        << flows.d_func()->CPRAssumption
        << flows.d_func()->CDRAssumption
        << flows.d_func()->LSAssumption
        << flows.d_func()->WALAssumption
        << flows.d_func()->m_ReinvestmentDelay
        << flows.d_func()->ReinvestmentPrice
        << flows.d_func()->m_RecoveryLag
        << flows.d_func()->m_Delinquency
        << flows.d_func()->m_DelinquencyLag
        << flows.d_func()->m_ReinvestmentSpreadOverTime
        << flows.d_func()->m_Reinvested
        << flows.d_func()->ReinvestQueue
	;
	return stream;
}
QDataStream& ReinvestmentTest::LoadOldVersion(QDataStream& stream) {
    Q_D( ReinvestmentTest);
	stream
        >> d->ReinvestmentPeriod;
    d->ReinvestmentBond.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->ReinvestmentBond;
    d->CPRAssumption.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->CPRAssumption;
    d->CDRAssumption.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->CDRAssumption;
    d->LSAssumption.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->LSAssumption;
    d->WALAssumption.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->WALAssumption;
    d->m_ReinvestmentDelay.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->m_ReinvestmentDelay;
    d->ReinvestmentPrice.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->ReinvestmentPrice;
    d->m_RecoveryLag.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->m_RecoveryLag;
    d->m_Delinquency.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->m_Delinquency;
    d->m_DelinquencyLag.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->m_DelinquencyLag;
    d->m_ReinvestmentSpreadOverTime.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->m_ReinvestmentSpreadOverTime;
    d->m_Reinvested.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->m_Reinvested;
    stream >> d->ReinvestQueue;
	ResetProtocolVersion();
	return stream;
}

void ReinvestmentTest::QueueReinvestments(double Amount, const QDate& CurrentDate) {
    Q_D( ReinvestmentTest);
	if (Amount < 0.01) return;
    int CurrentDelay = d->m_ReinvestmentDelay.GetValue(CurrentDate);
    int CurrentWindow = d->m_ReinvestmentSpreadOverTime.GetValue(CurrentDate.addMonths(CurrentDelay));
	for (QDate ReinvIter = CurrentDate.addMonths(CurrentDelay); ReinvIter < CurrentDate.addMonths(CurrentWindow + CurrentDelay); ReinvIter = ReinvIter.addMonths(1)) {
		QDate NormalisedIter = QDate(ReinvIter.year(), ReinvIter.month(), 15);
        if (d->ReinvestQueue.contains(NormalisedIter)) d->ReinvestQueue[NormalisedIter] += (Amount / static_cast<double>(CurrentWindow));
        else d->ReinvestQueue.insert(NormalisedIter, Amount / static_cast<double>(CurrentWindow));
	}
}

const MtgCashFlow& ReinvestmentTest::ProcessQueue(const QDate& CurrentDate, int Period, const QDate& MaxMaturity) {
    Q_D( ReinvestmentTest);
	QDate Normaliseddate(CurrentDate.year(), CurrentDate.month(), 15);
    d->ReinvestmentBond.ResetFlows();
    auto ReinvIter = d->ReinvestQueue.find(Normaliseddate);
    if (ReinvIter == d->ReinvestQueue.end()) 
        return d->ReinvestmentBond.GetCashFlow();
	CalculateBondCashFlows(ReinvIter.value(), CurrentDate, Period, MaxMaturity);
    d->m_Reinvested.AddFlow(CurrentDate, qMax(0.0, ReinvIter.value() - d->ReinvestmentBond.GetCashFlow().GetTotalFlow(CurrentDate)), static_cast<qint32>(MtgCashFlow::MtgFlowType::PrincipalFlow));
    d->ReinvestQueue.erase(ReinvIter);
    return d->ReinvestmentBond.GetCashFlow();
}

const GenericCashFlow& ReinvestmentTest::GetReinvested() const
{
    Q_D(const ReinvestmentTest);
    return d->m_Reinvested;
}

const QDate& ReinvestmentTest::GetReinvestmentPeriod() const
{
    Q_D(const ReinvestmentTest);
    return d->ReinvestmentPeriod;
}

const Mortgage& ReinvestmentTest::GetReinvestmentBond() const
{
    Q_D(const ReinvestmentTest);
    return d->ReinvestmentBond;
}

const BloombergVector& ReinvestmentTest::GetCPRAssumption() const
{
    Q_D(const ReinvestmentTest);
    return d->CPRAssumption;
}

const BloombergVector& ReinvestmentTest::GetCDRAssumption() const
{
    Q_D(const ReinvestmentTest);
    return d->CDRAssumption;
}

const BloombergVector& ReinvestmentTest::GetLSAssumption() const
{
    Q_D(const ReinvestmentTest);
    return d->LSAssumption;
}

const BloombergVector& ReinvestmentTest::GetWALAssumption() const
{
    Q_D(const ReinvestmentTest);
    return d->WALAssumption;
}

const IntegerVector& ReinvestmentTest::GetReinvDelay() const
{
    Q_D(const ReinvestmentTest);
    return d->m_ReinvestmentDelay;
}

const BloombergVector& ReinvestmentTest::GetReinvPrice() const
{
    Q_D(const ReinvestmentTest);
    return d->ReinvestmentPrice;
}

void ReinvestmentTest::ClearFlows()
{
    Q_D( ReinvestmentTest);
    d->m_Reinvested.Clear();
    d->ReinvestmentBond.ResetFlows();
}

double ReinvestmentTest::GetQueuedCash(QDate StartDate) const
{
    Q_D(const ReinvestmentTest);
    if (StartDate.isNull())
        return 0.0;
    StartDate.setDate(StartDate.year(), StartDate.month(), 15);
    double Result = 0.0;
    for (auto i = d->ReinvestQueue.constBegin(); i != d->ReinvestQueue.constEnd(); ++i) {
        if (i.key() < StartDate)
            continue;
        Result += i.value();
    }
    return Result;
}

bool ReinvestmentTest::ReinvestQueueueEmpty() const
{
    Q_D(const ReinvestmentTest);
    return d->ReinvestQueue.isEmpty();
}

void ReinvestmentTest::ResetReinvestQueueue()
{
    Q_D( ReinvestmentTest);
    d->ReinvestQueue.clear();
}

void ReinvestmentTest::CompileReferenceRateValue(ForwardBaseRateTable& Values)
{
    Q_D( ReinvestmentTest);
    d->ReinvestmentBond.CompileReferenceRateValue(Values);
}

void ReinvestmentTest::CompileReferenceRateValue(ConstantBaseRateTable& Values)
{
    Q_D( ReinvestmentTest);
    d->ReinvestmentBond.CompileReferenceRateValue(Values);
}

void ReinvestmentTest::RemoveBondFlow(const QDate& a)
{
    Q_D( ReinvestmentTest);
	if (a.isNull())return;
    MtgCashFlow& crurrFlows = d->ReinvestmentBond.GetCashFlow();
	crurrFlows.SetFlow(a, 0.0, MtgCashFlow::MtgFlowType::PrincipalFlow);
	crurrFlows.SetFlow(a, 0.0, MtgCashFlow::MtgFlowType::InterestFlow);
	crurrFlows.SetFlow(a, 0.0, MtgCashFlow::MtgFlowType::PrepaymentFlow);
}

void ReinvestmentTest::GetBaseRatesDatabase(ConstantBaseRateTable& Values, bool DownloadAll /*= false*/)
{
    Q_D( ReinvestmentTest);
    d->ReinvestmentBond.GetBaseRatesDatabase(Values, DownloadAll);
}

void ReinvestmentTest::GetBaseRatesDatabase(ForwardBaseRateTable& Values, bool DownloadAll /*= false*/)
{
    Q_D( ReinvestmentTest);
    d->ReinvestmentBond.GetBaseRatesDatabase(Values, DownloadAll);
}

QDataStream& operator>>(QDataStream & stream, ReinvestmentTest& flows)
{
	return flows.LoadOldVersion(stream);
}