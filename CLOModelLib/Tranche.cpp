#include "Tranche.h"
#include "Private/Tranche_p.h"
#include "Private/InternalItems.h"
#include <boost/math/tools/roots.hpp>
#include "DayCountVect.h"
#ifndef NO_DATABASE
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>
#endif
#ifndef NO_BLOOMBERG
#include <QBbgManager.h>
#include <QbbgReferenceDataRequest.h>
#include <QBbgRequestGroup.h>
#include <QBbgReferenceDataResponse.h>
#include <QBbgOverride.h>
#endif // !NO_BLOOMBERG
DEFINE_PUBLIC_COMMONS(Tranche)
DEFINE_PUBLIC_COMMONS_COPY(Tranche)
TranchePrivate::TranchePrivate(Tranche *q,const TranchePrivate& other)
    :BackwardInterfacePrivate(q, other)
    , LastPaymentDate(other.LastPaymentDate)
    , Currency(other.Currency)
    , Price(other.Price)
    , BloombergExtension(other.BloombergExtension)
    , ProrataGroup(other.ProrataGroup)
    , MinIClevel(other.MinIClevel)
    , MinOClevel(other.MinOClevel)
    , ExchangeRate(other.ExchangeRate)
    , OriginalAmt(other.OriginalAmt)
    , OutstandingAmt(other.OutstandingAmt)
    , PaymentFrequency(other.PaymentFrequency)
    , TrancheName(other.TrancheName)
    , CashFlow(other.CashFlow)
    , SettlementDate(other.SettlementDate)
    , ISINcode(other.ISINcode)
    , m_UseForwardCurve(other.m_UseForwardCurve)
    , m_rating(other.m_rating)
{
    for (QHash<qint32, BloombergVector*>::const_iterator i = other.Coupon.constBegin(); i != other.Coupon.constEnd(); ++i)
        Coupon.insert(i.key(), new BloombergVector(*(i.value())));
    for (QHash<qint32, BloombergVector*>::const_iterator i = other.ReferenceRateValue.constBegin(); i != other.ReferenceRateValue.constEnd(); ++i)
        ReferenceRateValue.insert(i.key(), new BloombergVector(*(i.value())));
    for (QHash<qint32, BaseRateVector*>::const_iterator i = other.ReferenceRate.constBegin(); i != other.ReferenceRate.constEnd(); ++i)
        ReferenceRate.insert(i.key(), new BaseRateVector(*(i.value())));
    for (auto i = other.m_DayCount.constBegin(); i != other.m_DayCount.constEnd(); ++i)
        m_DayCount.insert(i.key(), new DayCountVector(*(i.value())));
}
TranchePrivate::TranchePrivate(Tranche *q)
	:BackwardInterfacePrivate(q)
    , LastPaymentDate(2000, 1, 1)
    , Currency("GBP")
    , Price(100)
    , BloombergExtension("Mtge")
    , ProrataGroup("1")
    , MinIClevel(-1.0)
    , MinOClevel(-1.0)
    , ExchangeRate(1.0)
    , OriginalAmt(0.0)
    , OutstandingAmt(0.0)
    , PaymentFrequency("3")
    , m_UseForwardCurve(false)
{
    BloombergVector* TempCoup = new BloombergVector("0");
    TempCoup->SetDivisor(10000.0);
    Coupon.insert(0, TempCoup);
    m_DayCount.insert(0, new DayCountVector(DayCountConvention::ACT360));
    ReferenceRate.insert(-1, new BaseRateVector("BP0003M"));
}
Tranche::Tranche(TranchePrivate *d, const Tranche& other)
	:BackwardInterface(d,other)
{}
Tranche& Tranche::operator=(const Tranche& other){
	Q_D(Tranche);
	BackwardInterface::operator=(other);
    d->LastPaymentDate = other.d_func()->LastPaymentDate;
    d->Currency = other.d_func()->Currency;
    d->Price = other.d_func()->Price;
    d->BloombergExtension = other.d_func()->BloombergExtension;
    d->ProrataGroup = other.d_func()->ProrataGroup;
    d->MinIClevel = other.d_func()->MinIClevel;
    d->MinOClevel = other.d_func()->MinOClevel;
    d->ExchangeRate = other.d_func()->ExchangeRate;
    d->OriginalAmt = other.d_func()->OriginalAmt;
    d->OutstandingAmt = other.d_func()->OutstandingAmt;
    d->PaymentFrequency = other.d_func()->PaymentFrequency;
    d->TrancheName = other.d_func()->TrancheName;
    d->CashFlow = other.d_func()->CashFlow;
    d->SettlementDate = other.d_func()->SettlementDate;
    d->ISINcode = other.d_func()->ISINcode;
    d->m_LoadProtocolVersion = other.d_func()->m_LoadProtocolVersion;
    d->m_UseForwardCurve = other.d_func()->m_UseForwardCurve;
    d->m_rating = other.d_func()->m_rating;
    ClearInterest();
    for (auto i = other.d_func()->Coupon.constBegin(); i != other.d_func()->Coupon.constEnd(); ++i)
        d->Coupon.insert(i.key(), new BloombergVector(*(i.value())));
    for (auto i = other.d_func()->ReferenceRateValue.constBegin(); i != other.d_func()->ReferenceRateValue.constEnd(); ++i)
        d->ReferenceRateValue.insert(i.key(), new BloombergVector(*(i.value())));
    for (auto i = other.d_func()->ReferenceRate.constBegin(); i != other.d_func()->ReferenceRate.constEnd(); ++i)
        d->ReferenceRate.insert(i.key(), new BaseRateVector(*(i.value())));
    for (auto i = other.d_func()->m_DayCount.constBegin(); i != other.d_func()->m_DayCount.constEnd(); ++i)
        d->m_DayCount.insert(i.key(), new DayCountVector(*(i.value())));
    return *this;
}
Tranche::Tranche(TranchePrivate *d)
	:BackwardInterface(d)
{}

Tranche::~Tranche() {
	ClearInterest();
}

const QHash<qint32, BloombergVector*>& Tranche::GetRefRateValues() const
{
    Q_D(const Tranche);
    return d->ReferenceRateValue;
}

void Tranche::ClearInterest()
{
    Q_D(Tranche);
    for (auto i = d->Coupon.begin(); i != d->Coupon.end(); ++i)
		delete (i.value());
    for (auto i = d->ReferenceRateValue.begin(); i != d->ReferenceRateValue.end(); ++i)
		delete (i.value());
    for (auto i = d->ReferenceRate.begin(); i != d->ReferenceRate.end(); ++i)
		delete (i.value());
    for (auto i = d->m_DayCount.begin(); i != d->m_DayCount.end(); ++i)
        delete (i.value());
    d->Coupon.clear();
    d->ReferenceRateValue.clear();
    d->ReferenceRate.clear();
    d->m_DayCount.clear();
}

void Tranche::SetTrancheName(const QString& a)
{
    Q_D(Tranche);
    d->TrancheName = a;
}

void Tranche::SetCurrency(const QString& a)
{
    Q_D(Tranche);
    d->Currency = a;
}

void Tranche::SetOriginalAmount(double a) {
    Q_D(Tranche); 
    if (a >= 0.0) 
        d->OriginalAmt = a / d->ExchangeRate;
}
void Tranche::SetOutstandingAmt(double a){
    Q_D(Tranche);
	if(a<0.0) 
        return;
    d->OutstandingAmt = a / d->ExchangeRate;
    d->CashFlow.SetInitialOutstanding(d->OutstandingAmt);
}
void Tranche::SetExchangeRate(double a){
    Q_D(Tranche);
	if(a<=0.0) return;
    d->OriginalAmt *= d->ExchangeRate;
    d->OutstandingAmt *= d->ExchangeRate;
    d->ExchangeRate = a;
    d->OriginalAmt /= d->ExchangeRate;
    d->OutstandingAmt /= d->ExchangeRate;
    d->CashFlow.SetInitialOutstanding(d->OutstandingAmt);
}

void Tranche::GetRefRateValueFromBloomberg()const{
#ifndef NO_BLOOMBERG
    Q_D(const Tranche);
    for (auto i = d->ReferenceRate.constBegin(); i != d->ReferenceRate.constEnd(); ++i) {
        if (d->ReferenceRateValue.contains(i.key())) {
            (*(d->ReferenceRateValue[i.key()])) = i.value()->GetRefRateValueFromBloomberg(d->m_CnstRateCache);
		}
		else {
            d->ReferenceRateValue.insert(i.key(), new BloombergVector(i.value()->GetRefRateValueFromBloomberg(d->m_CnstRateCache)));
		}
	}
    d->m_UseForwardCurve = false;
#endif
}
void Tranche::CompileReferenceRateValue(ConstantBaseRateTable& Values) const {
    Q_D(const Tranche);
    for (auto i = d->ReferenceRate.constBegin(); i != d->ReferenceRate.constEnd(); ++i) {
        if (d->ReferenceRateValue.contains(i.key())) {
            (*(d->ReferenceRateValue[i.key()])) = i.value()->CompileReferenceRateValue(Values);
		}
		else {
            d->ReferenceRateValue.insert(i.key(), new BloombergVector(i.value()->CompileReferenceRateValue(Values)));
		}
	}
    d->m_UseForwardCurve = false;
}
void Tranche::CompileReferenceRateValue(ForwardBaseRateTable& Values) const {
    Q_D(const Tranche);
    for (auto i = d->ReferenceRate.constBegin(); i != d->ReferenceRate.constEnd(); ++i) {
        if (d->ReferenceRateValue.contains(i.key())) {
            (*(d->ReferenceRateValue[i.key()])) = i.value()->CompileReferenceRateValue(Values);
		}
		else {
            d->ReferenceRateValue.insert(i.key(), new BloombergVector(i.value()->CompileReferenceRateValue(Values)));
		}
	}
    d->m_UseForwardCurve = true;
}

void Tranche::ClearRefRatesCache() const
{
    Q_D(const Tranche);
    d->m_FrwRateCache.Clear();
    d->m_CnstRateCache.Clear();
}


void Tranche::GetBaseRatesDatabase(ConstantBaseRateTable& Values, bool DownloadAll) const {
#ifndef NO_DATABASE
    Q_D(const Tranche);
    for (auto i = d->ReferenceRate.constBegin(); i != d->ReferenceRate.constEnd(); ++i) {
        if (d->ReferenceRateValue.contains(i.key())) {
            (*(d->ReferenceRateValue[i.key()])) = i.value()->GetBaseRatesDatabase(Values, DownloadAll);
		}
		else {
            d->ReferenceRateValue.insert(i.key(), new BloombergVector(i.value()->GetBaseRatesDatabase(Values, DownloadAll)));
		}
	}
    d->m_UseForwardCurve = false;
#endif
}
void Tranche::GetBaseRatesDatabase(ForwardBaseRateTable& Values, bool DownloadAll) const {
#ifndef NO_DATABASE
    Q_D(const Tranche);
    for (auto i = d->ReferenceRate.constBegin(); i != d->ReferenceRate.constEnd(); ++i) {
        if (d->ReferenceRateValue.contains(i.key())) {
            (*(d->ReferenceRateValue[i.key()])) = i.value()->GetBaseRatesDatabase(Values, DownloadAll);
		}
		else {
            d->ReferenceRateValue.insert(i.key(), new BloombergVector(i.value()->GetBaseRatesDatabase(Values, DownloadAll)));
		}
	}
    d->m_UseForwardCurve = true;
#endif
}

void Tranche::DownloadBaseRates() const {
    Q_D(const Tranche);
	#ifndef NO_DATABASE
        if (d->m_UseForwardCurve)
            GetBaseRatesDatabase(d->m_FrwRateCache);
		else
            GetBaseRatesDatabase( d->m_CnstRateCache);
	#else
		#ifndef NO_BLOOMBERG
			GetRefRateValueFromBloomberg();
		#else
            for (auto i =  d->ReferenceRate.constBegin(); i !=  d->ReferenceRate.constEnd(); ++i) {
                if ( d->ReferenceRateValue.contains(i.key())) {
                    (*( d->ReferenceRateValue[i.key()])) = "0";
				}
				else {
                    d->ReferenceRateValue.insert(i.key(), new BloombergVector("0"));
				}
			}
		#endif
	#endif 
}
double Tranche::GetCoupon(const QDate& index, qint32 CoupIndex , int Frequency) const {
    Q_D(const Tranche);
    if (!d->Coupon.contains(CoupIndex)) return 0.0;
    BloombergVector TempCoupon = *(d->Coupon.value(CoupIndex));
    if (TempCoupon.GetAnchorDate().isNull()) TempCoupon.SetAnchorDate(d->LastPaymentDate);
	if (TempCoupon.GetAnchorDate().isNull()) TempCoupon.SetAnchorDate(index);
    if (!d->ReferenceRate.value(CoupIndex)->IsEmpty()) {
        bool DownloadRates = !d->ReferenceRateValue.contains(CoupIndex);
        if (!DownloadRates) DownloadRates = d->ReferenceRateValue.value(CoupIndex)->IsEmpty();
		if (DownloadRates)
		{
			#ifndef NO_DATABASE
                if (d->m_UseForwardCurve)
                    GetBaseRatesDatabase(d->m_FrwRateCache);
				else
                    GetBaseRatesDatabase( d->m_CnstRateCache);
			#else
				#ifndef NO_BLOOMBERG
					GetRefRateValueFromBloomberg();
				#else
					if (ReferenceRateValue.contains(CoupIndex)) {
						(*(ReferenceRateValue[CoupIndex])) = "0";
					}
					else {
						ReferenceRateValue.insert(CoupIndex, new BloombergVector("0"));
					}
				#endif
			#endif 
		}

	
        bool NullReferenceRateAnch = d->ReferenceRateValue.value(CoupIndex)->GetAnchorDate().isNull();
        if (NullReferenceRateAnch)  d->ReferenceRateValue.operator[](CoupIndex)->SetAnchorDate(d->LastPaymentDate);
        double Result = (TempCoupon.GetValue(index, Frequency)) + d->ReferenceRateValue.value(CoupIndex)->GetValue(index, Frequency);
        if (NullReferenceRateAnch)  d->ReferenceRateValue.operator[](CoupIndex)->RemoveAnchorDate();
		return Result;
	}
	return TempCoupon.GetValue(index,Frequency);
	//return GetCoupon(MonthDiff(index,Anch));
}
double Tranche::GetCoupon(int index, qint32 CoupIndex, int Frequency) const {
    Q_D(const Tranche);
    QDate CoupDate = d->CashFlow.GetDate(index);
	if(CoupDate.isNull()) return 0.0;
	return GetCoupon(CoupDate, CoupIndex, Frequency);
}

double Tranche::GetStartingDeferredInterest(qint32 CoupIdx) const
{
    Q_D(const Tranche);
    return  d->CashFlow.GetStartingDeferredInterest(CoupIdx);
}

void Tranche::SetStartingDeferredInterest(const double& val, qint32 CoupIdx )
{
    Q_D(Tranche);
    d->CashFlow.SetStartingDeferredInterest(val, CoupIdx);
}

const QString& Tranche::GetTrancheName() const
{
    Q_D(const Tranche);
    return  d->TrancheName;
}

const QString& Tranche::GetCurrency() const
{
    Q_D(const Tranche);
    return  d->Currency;
}

double Tranche::GetOriginalAmount() const
{
    Q_D(const Tranche);
    return  d->OriginalAmt;
}

double Tranche::GetOutstandingAmt() const
{
    Q_D(const Tranche);
    return  d->OutstandingAmt;
}

double Tranche::GetBaseCurrencyOutsanding() const
{
    Q_D(const Tranche);
    return  d->OutstandingAmt* d->ExchangeRate;
}

double Tranche::GetBaseCurrencyOriginal() const
{
    Q_D(const Tranche);
    return  d->OriginalAmt* d->ExchangeRate;
}

QList<qint32> Tranche::GetCouponIndexes() const
{
    Q_D(const Tranche);
    return  d->Coupon.keys();
}

QList<qint32> Tranche::GetReferenceRateIndexes() const
{
    Q_D(const Tranche);
    return  d->ReferenceRate.keys();
}

QList<qint32> Tranche::GetDayCountsIndexes() const
{
    Q_D(const Tranche);
    return d->m_DayCount.keys();
}

double Tranche::GetTotalCoupon(const QDate& index, int Frequency) const
{
    Q_D(const Tranche);
	double Result = 0.0;
    for (auto i = d->Coupon.constBegin(); i != d->Coupon.constEnd(); ++i) {
		Result += GetCoupon(index, i.key(), Frequency);
	}
	return Result;
}
double Tranche::GetTotalCoupon(int index, int Frequency) const {
    Q_D(const Tranche);
	double Result = 0.0;
    for (auto i = d->Coupon.constBegin(); i != d->Coupon.constEnd(); ++i) {
		Result += GetCoupon(index, i.key(), Frequency);
	}
	return Result;
}
double Tranche::GetRawCoupon(int index, qint32 CoupIndex, int Frequency) const {
    Q_D(const Tranche);
    if (!d->Coupon.contains(CoupIndex))
        return 0.0;
    return  d->Coupon.value(CoupIndex)->GetValue(index, Frequency);
}
double Tranche::GetRawCoupon(const QDate& index, qint32 CoupIndex, int Frequency) const {
    Q_D(const Tranche);
    if (!d->Coupon.contains(CoupIndex)) return 0.0;
    bool NullCoupAnch = d->Coupon.value(CoupIndex)->GetAnchorDate().isNull();
    if (NullCoupAnch)  d->Coupon.operator[](CoupIndex)->SetAnchorDate(d->LastPaymentDate);
    if (d->Coupon.value(CoupIndex)->GetAnchorDate().isNull())  d->Coupon.operator[](CoupIndex)->SetAnchorDate(index);
    double Result = d->Coupon.value(CoupIndex)->GetValue(index, Frequency);
    if (NullCoupAnch)  d->Coupon.operator[](CoupIndex)->RemoveAnchorDate();
	return Result;
}
void Tranche::SetBloombergExtension(const QString& a){
    Q_D(Tranche);
	QString AdjustedString(a.trimmed().toLower());
	if(AdjustedString.size()==0) return;
	AdjustedString[0]=AdjustedString.at(0).toUpper();
    if (AdjustedString == "Mtge" || AdjustedString == "Corp")  
        d->BloombergExtension = a;
}

void Tranche::SetProrataGroup(const QString& a)
{
    Q_D(Tranche);
    d->ProrataGroup.SetSeniorityScale(a);
}



void Tranche::SetCashFlow(QDate Dte, double Amt, TrancheCashFlow::TrancheFlowType FlwTpe)
{
    Q_D(Tranche);
    d->CashFlow.SetFlow(Dte, Amt, FlwTpe);
}

void Tranche::SetCashFlow(QDate Dte, double Amt, qint32 FlwTpe)
{
    Q_D(Tranche);
    d->CashFlow.SetFlow(Dte, Amt, FlwTpe);
}

void Tranche::SetCashFlow(const TrancheCashFlow& a)
{
    Q_D(Tranche);
    d->CashFlow = a;
}

void Tranche::SetMinOClevel(double a)
{
    Q_D(Tranche);
    if (a > 0.0 || a == -1.0)
        d->MinOClevel = a;
}

void Tranche::AddCashFlow(const TrancheCashFlow& a)
{
    Q_D(Tranche);
    d->CashFlow.AddFlow(a);
}

void Tranche::AddCashFlow(QDate Dte, double Amt, TrancheCashFlow::TrancheFlowType FlwTpe)
{
    Q_D(Tranche);
    d->CashFlow.AddFlow(Dte, Amt, FlwTpe);
}

void Tranche::AddCashFlow(QDate Dte, double Amt, qint32 FlwTpe)
{
    Q_D(Tranche);
    d->CashFlow.AddFlow(Dte, Amt, FlwTpe);
}

double Tranche::GetBondFactor() const
{
    Q_D(const Tranche);
    return  d->OutstandingAmt / d->OriginalAmt;
}
void Tranche::getRatingsBloomberg()
{
#ifndef NO_BLOOMBERG
    Q_D(Tranche);
    d->m_rating.reset();
    QBbgLib::QBbgManager Bee;
    QBbgLib::QBbgRequestGroup ReqGr;
    QBbgLib::QBbgReferenceDataRequest TempReq;
    const QString IdentityCode = d->downloadISIN();
    if (IdentityCode.isEmpty())
        return;
    const auto BdpExt = QBbgLib::QBbgSecurity::stringToYellowKey(d->BloombergExtension);
    if (BdpExt == QBbgLib::QBbgSecurity::Invalid)
        return;
    TempReq.setSecurity(QBbgLib::QBbgSecurity(IdentityCode, BdpExt));
    TempReq.setField("RTG_SP"); ReqGr.addRequest(TempReq);
    TempReq.setField("RTG_MOODY"); ReqGr.addRequest(TempReq);
    TempReq.setField("RTG_FITCH"); ReqGr.addRequest(TempReq);
    TempReq.setField("RTG_DBRS"); ReqGr.addRequest(TempReq);
    const auto& TmpResponse = Bee.processRequest(ReqGr);
    for (auto i = TmpResponse.constBegin(); i != TmpResponse.constEnd(); ++i)
        d->m_rating.downloadRatings(i.value());
#endif
}
void Tranche::GetDataFromBloomberg()
{
#ifndef NO_BLOOMBERG
    Q_D(Tranche);
    double currentDefer = 0;
	{
		QBbgLib::QBbgManager Bee;
        QBbgLib::QBbgRequestGroup ReqGr;
		QBbgLib::QBbgReferenceDataRequest TempReq;
        const QString IdentityCode = d->downloadISIN();
		if (IdentityCode.isEmpty()) 
            return;
        const auto BdpExt = QBbgLib::QBbgSecurity::stringToYellowKey(d->BloombergExtension);
        if (BdpExt == QBbgLib::QBbgSecurity::Invalid)
            return;
        TempReq.setSecurity(QBbgLib::QBbgSecurity(IdentityCode, BdpExt));
        TempReq.setField("MTG_ORIG_AMT"); TempReq.setID(1); ReqGr.addRequest(TempReq);
        TempReq.setField("AMT_OUTSTANDING"); TempReq.setID(2); ReqGr.addRequest(TempReq);
        TempReq.setField("CRNCY"); TempReq.setID(3); ReqGr.addRequest(TempReq);
        TempReq.setField("START_ACC_DT"); TempReq.setID(4); ReqGr.addRequest(TempReq);
        TempReq.setField("MTG_TYP"); TempReq.setID(5); ReqGr.addRequest(TempReq);
        TempReq.setField("FLT_SPREAD"); TempReq.setID(6); ReqGr.addRequest(TempReq);
        TempReq.setField("RESET_IDX"); TempReq.setID(7); ReqGr.addRequest(TempReq);
        TempReq.setField("CPN"); TempReq.setID(8); ReqGr.addRequest(TempReq);
        TempReq.setField("CPN_FREQ"); TempReq.setID(9); ReqGr.addRequest(TempReq);
        TempReq.setField("SETTLE_DT"); TempReq.setID(11); ReqGr.addRequest(TempReq);
        TempReq.setField("NAME"); TempReq.setID(12); ReqGr.addRequest(TempReq);
        TempReq.setField("ID_ISIN"); TempReq.setID(13); ReqGr.addRequest(TempReq);
        TempReq.setField("DAY_CNT"); TempReq.setID(14); ReqGr.addRequest(TempReq);
        TempReq.setField("MTG_INT_DEFERRED"); TempReq.setID(15); ReqGr.addRequest(TempReq);
        TempReq.setField("RTG_SP"); TempReq.setID(16); ReqGr.addRequest(TempReq);
        TempReq.setField("RTG_MOODY"); TempReq.setID(17); ReqGr.addRequest(TempReq);
        TempReq.setField("RTG_FITCH"); TempReq.setID(18); ReqGr.addRequest(TempReq);
        TempReq.setField("RTG_DBRS"); TempReq.setID(19); ReqGr.addRequest(TempReq);
        const auto& TmpResponse=Bee.processRequest(ReqGr);

        if (TmpResponse.value(1)->hasErrors())
            d->OriginalAmt = 0.0;
        else 
            d->OriginalAmt = dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(TmpResponse.value(1))->value().toDouble() / d->ExchangeRate;

        if (TmpResponse.value(2)->hasErrors()) 
            d->OutstandingAmt = 0.0;
        else
            d->OutstandingAmt = dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(TmpResponse.value(2))->value().toDouble() / d->ExchangeRate;

        if (TmpResponse.value(3)->hasErrors())
            d->Currency = "EUR";
        else
            d->Currency = dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(TmpResponse.value(3))->value().toString();

        if (TmpResponse.value(4)->hasErrors()) 
            d->LastPaymentDate = QDate();
        else 
            d->LastPaymentDate = dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(TmpResponse.value(4))->value().toDate();

        bool TempCheck = TmpResponse.value(6)->hasErrors();
        if (!TempCheck)
            TempCheck = !(dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(TmpResponse.value(5))->value().toString().contains("FLT"));
        if (!TempCheck) {
            if (TmpResponse.value(6)->hasErrors()) { 
                if (d->Coupon.contains(0))
                    *(d->Coupon[0]) = "";
                else 
                    d->Coupon.insert(0, new BloombergVector());
            }
			else { 
                if (d->Coupon.contains(0))
                    *(d->Coupon[0]) = dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(TmpResponse.value(6))->value().toString();
                else 
                    d->Coupon.insert(0, new BloombergVector(dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(TmpResponse.value(6))->value().toString()));
            }

            if (TmpResponse.value(7)->hasErrors()) { 
                if (d->ReferenceRate.contains(0))
                    *(d->ReferenceRate[0]) = "";
                else 
                    d->ReferenceRate.insert(0, new BaseRateVector());
            }
			else {
                if (d->ReferenceRate.contains(0))
                    *(d->ReferenceRate[0]) = dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(TmpResponse.value(7))->value().toString();
                else 
                    d->ReferenceRate.insert(0, new BaseRateVector(dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(TmpResponse.value(7))->value().toString()));
                if (d->ReferenceRate.contains(-1))
                    *(d->ReferenceRate[-1]) = dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(TmpResponse.value(7))->value().toString();
                else
                    d->ReferenceRate.insert(-1, new BaseRateVector(dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(TmpResponse.value(7))->value().toString()));
			}
		}
		else {
            if (TmpResponse.value(8)->hasErrors()) {
                if (d->Coupon.contains(0))
                    *(d->Coupon[0]) = "";
                else 
                    d->Coupon.insert(0, new BloombergVector());
            }
			else { 
                if (d->Coupon.contains(0))
                    *(d->Coupon[0]) = QString("%1").arg(dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(TmpResponse.value(8))->value().toDouble()*100.0);
                else 
                    d->Coupon.insert(0, new BloombergVector(QString("%1").arg(dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(TmpResponse.value(8))->value().toDouble()*100.0)));
            }


			QString DeafultRefRateString;
            if (!TmpResponse.value(9)->hasErrors()) {
                if (d->Currency == "EUR")
                    DeafultRefRateString = "EUR";
                else if (d->Currency == "GBP")
                    DeafultRefRateString = "BP";
                else if (d->Currency == "USD")
                    DeafultRefRateString = "US";
				if (!DeafultRefRateString.isEmpty())
					DeafultRefRateString +=
					QString("%1M")
                    .arg(static_cast<int>(12.0 / dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(TmpResponse.value(9))->value().toDouble()), 6 - DeafultRefRateString.size(), 10, QChar('0'));
			}
            if (d->ReferenceRate.contains(-1))
                *(d->ReferenceRate[-1]) = DeafultRefRateString;
            else 
                d->ReferenceRate.insert(-1, new BaseRateVector(DeafultRefRateString));
		}

        if (TmpResponse.value(9)->hasErrors()) 
            d->PaymentFrequency = "";
        else 
            d->PaymentFrequency = QString("%1").arg(static_cast<int>(12.0 / dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(TmpResponse.value(9))->value().toDouble()));

        if (TmpResponse.value(11)->hasErrors()) 
            d->SettlementDate = QDate();
        else 
            d->SettlementDate = dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(TmpResponse.value(11))->value().toDate();

        if (TmpResponse.value(12)->hasErrors()) 
            d->TrancheName = "";
        else 
            d->TrancheName = dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(TmpResponse.value(12))->value().toString();

        if (!TmpResponse.value(13)->hasErrors()) 
            d->ISINcode.insert(dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(TmpResponse.value(13))->value().toString());

        if (TmpResponse.value(14)->hasErrors()) {
            if (d->m_DayCount.contains(0))
                d->m_DayCount[0]->operator=(QString::number(static_cast<qint16>(DayCountConvention::ACT360)));
            else
                d->m_DayCount.insert(0, new DayCountVector(DayCountConvention::ACT360));
        }
        else {
            if (d->m_DayCount.contains(0))
                d->m_DayCount[0]->operator=(dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(TmpResponse.value(14))->value().toString());
            else
                d->m_DayCount.insert(0, new DayCountVector(dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(TmpResponse.value(14))->value().toString()));
        }
        if (!TmpResponse.value(15)->hasErrors()) {
            currentDefer = dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(TmpResponse.value(15))->value().toDouble();
        }
        for (int i = 16; i <= 19; ++i)
            d->m_rating.downloadRatings(TmpResponse.value(i));
	}
	DownloadBaseRates();
	GetCashFlowsFromBloomberg();
    d->CashFlow.SetInitialOutstanding(d->OutstandingAmt);
    d->CashFlow.SetStartingDeferredInterest(currentDefer, 0);
#endif
}
void Tranche::GetCashFlowsFromBloomberg(bool useFwdRates)
{
    Q_D(Tranche);
    d->CashFlow.Clear();
    QBbgLib::QBbgSecurity TempSecurity(d->ISINcode.isEmpty() ? d->TrancheName : *d->ISINcode.constBegin(), QBbgLib::QBbgSecurity::stringToYellowKey(d->BloombergExtension));
    if (TempSecurity.name().isEmpty() || TempSecurity.extension() == QBbgLib::QBbgSecurity::Invalid) return;
    QBbgLib::QBbgOverride Ovr;
#ifndef NO_DATABASE
    Db_Mutex.lock();
	{
		QSqlDatabase db = QSqlDatabase::database("TwentyFourDB", false);
		if (!db.isValid()) {
			db = QSqlDatabase::addDatabase(GetFromConfig("Database", "DBtype"), "TwentyFourDB");
			db.setDatabaseName(
				"Driver={" + GetFromConfig("Database", "Driver")
				+ "}; "
				+ GetFromConfig("Database", "DataSource")
				);
			
		}
		bool DbOpen = db.isOpen();
		if (!DbOpen) DbOpen = db.open();
        if (DbOpen) {
            for (auto ISINiter = d->ISINcode.constBegin(); ISINiter != d->ISINcode.constEnd(); ++ISINiter) {
                QSqlQuery GetPrepaySpeedQuery(db);
                GetPrepaySpeedQuery.setForwardOnly(true);
                GetPrepaySpeedQuery.prepare("{CALL " + GetFromConfig("Database", "GetPrepaySpeedStoredProc") + "}");
                GetPrepaySpeedQuery.bindValue(":TrancheID", *ISINiter);
                if (GetPrepaySpeedQuery.exec()) {
                    if (GetPrepaySpeedQuery.next()) {
                        auto GetPrepaySpeedRecord = GetPrepaySpeedQuery.record();
                        for (int RecIter = 0; RecIter < GetPrepaySpeedRecord.count(); ++RecIter) {
                            if (GetPrepaySpeedRecord.value(RecIter).toString().isEmpty()) continue;
                            Ovr.setOverride(GetPrepaySpeedRecord.fieldName(RecIter), GetPrepaySpeedRecord.value(RecIter).toString());
                        }
                        break;
                    }
                }
            }
        }
	}
	Db_Mutex.unlock();
#endif // !NO_DATABASE
    Ovr.setOverride("APPLY_FWD_RATE", useFwdRates ? "Y" : "N");
    QBbgLib::QBbgRequestGroup TempReq;
    QBbgLib::QBbgReferenceDataRequest SingleRq;
    SingleRq.setSecurity(TempSecurity);
    SingleRq.setOverrides(Ovr);
    SingleRq.setField("MTG_CASH_FLOW");
    SingleRq.setID(1);
    TempReq.addRequest(SingleRq);
    SingleRq.setField("DES_CASH_FLOW");
    SingleRq.setID(2);
    TempReq.addRequest(SingleRq);
    QBbgLib::QBbgManager bbgMan;
    const auto& Results=bbgMan.processRequest(TempReq);
    if (Results.value(1, nullptr)) {
        if (d->CashFlow.GetCashFlowsBloomberg(Results.value(1)))
                return;
        if (Results.value(2, nullptr)){
            if (d->CashFlow.GetCashFlowsBloomberg(Results.value(2)))
                return;
        }
        return d->CashFlow.Clear();
    }
}
double Tranche::GetLossRate() const{
    Q_D(const Tranche);
	double Result=0.0;
    if (d->OutstandingAmt <= 0) return 0.0;
    for (int i = 0; i<d->CashFlow.Count(); i++) {
        Result += d->CashFlow.GetPrincipal(i);
	}
    Result = 1 - (Result / d->OutstandingAmt);
	if(Result<0.00001) return 0;
	return Result;
}
double Tranche::GetDiscountMargin() const {
    Q_D(const Tranche);
    return GetDiscountMargin(d->Price); 
}
double Tranche::GetDiscountMargin(double NewPrice)const{
    Q_D(const Tranche);
    if (/*GetLossRate()>0.0000 ||*/ d->OutstandingAmt<0.01) return 0.0;
	
    double AccruedInterest = deannualiseCoupon(GetTotalCoupon(0), d->LastPaymentDate, d->SettlementDate, d->m_DayCount.value(0)->GetValue(d->SettlementDate));
	QDate TempDate;
	QList<QDate> FlowsDates;
	QList<double> FlowsValues;
    FlowsDates.append(d->SettlementDate);
    FlowsValues.append(-d->OutstandingAmt*((NewPrice / 100.0) + AccruedInterest));
    for (int i = 0; i<d->CashFlow.Count(); i++) {
        TempDate = d->CashFlow.GetDate(i);
        while (AdjHolidays(d->m_DayCount.value(0)->GetValue(TempDate)) && IsHoliday(TempDate))
			TempDate = TempDate.addDays(1);
		FlowsDates.append(TempDate);
        FlowsValues.append(d->CashFlow.GetTotalFlow(i));
	}
    Q_ASSERT(d->ReferenceRate.contains(-1));
    BaseRateVector ApplicableRate = *(d->ReferenceRate.value(0, d->ReferenceRate.value(-1)));
    ApplicableRate.replaceValue("ZERO", *d->ReferenceRate.value(-1));
    BloombergVector ApplicableRateValue;
    if (d->m_UseForwardCurve)
        ApplicableRateValue = ApplicableRate.GetBaseRatesDatabase(d->m_FrwRateCache);
    else
        ApplicableRateValue = ApplicableRate.GetBaseRatesDatabase(d->m_CnstRateCache);
    if (ApplicableRateValue.IsEmpty() || d->m_DayCount.value(0)->IsEmpty())
         return 0.0;
    return qMax(0.0, CalculateDM(FlowsDates, FlowsValues, ApplicableRateValue, *(d->m_DayCount.value(0))));
}
double Tranche::GetIRR() const {
    Q_D(const Tranche);
    return GetIRR(d->Price);
}
double Tranche::GetIRR(double NewPrice)const{
    Q_D(const Tranche);
    if (/*GetLossRate()>0.0000 ||*/ d->OutstandingAmt<0.01) return 0.0;
    double AccruedInterest = deannualiseCoupon(GetTotalCoupon(0), d->LastPaymentDate, d->SettlementDate, d->m_DayCount.value(0)->GetValue(d->SettlementDate));
	QList<QDate> FlowsDates;
	QList<double> FlowsValues;
	QDate TempDate;
    FlowsDates.append(d->SettlementDate);
    FlowsValues.append(-d->OutstandingAmt*((NewPrice / 100.0) + AccruedInterest));
    for (int i = 0; i<d->CashFlow.Count(); i++) {
        TempDate = d->CashFlow.GetDate(i);
        while (AdjHolidays(d->m_DayCount.value(0)->GetValue(TempDate)) && IsHoliday(TempDate)) TempDate = TempDate.addDays(1);
		FlowsDates.append(TempDate);
        FlowsValues.append(d->CashFlow.GetTotalFlow(i));
	}
    return qMax(0.0, CalculateIRR(FlowsDates, FlowsValues, *(d->m_DayCount.value(0))));
}
double Tranche::GetWALife(const QDate& StartDate)const{
    Q_D(const Tranche);
    if (d->OutstandingAmt<0.01 || StartDate.isNull()) return 0.0;
	double RunningSum=0.0, Result=0.0, CurrentPrinc;
    for (int i = 0; i<d->CashFlow.Count(); i++) {
        CurrentPrinc = d->CashFlow.GetPrincipal(i);
		if(CurrentPrinc>0){
			RunningSum+=CurrentPrinc;
            Result += CurrentPrinc*static_cast<double>(StartDate.daysTo(d->CashFlow.GetDate(i)));
		}
	}
	if (RunningSum<=0) return 0.0;
	return (Result / 365.25) / RunningSum;
}

double Tranche::GetWALife() const
{
    Q_D(const Tranche);
    return GetWALife(d->SettlementDate);
}

const QDate& Tranche::GetSettlementDate() const
{
    Q_D(const Tranche);
    return d->SettlementDate;
}

void Tranche::SetSettlementDate(const QDate& a)
{
    Q_D(Tranche);
    d->SettlementDate = a;
}



double Tranche::getActualCoupon(const QDate& index, qint32 CouponIdx) const
{
    if (CouponIdx<0 || CouponIdx>(1 << MaximumInterestsTypes))
        return 0.0;
    Q_D(const Tranche);
    if(!d->CashFlow.HasFlowType(TrancheCashFlow::InterestFlow | CouponIdx))
        return 0.0;
    const QDate prevIPD = d->CashFlow.prevFlowDate(index);
    Q_ASSERT(d->m_DayCount.value(CouponIdx, nullptr));
    DayCountVector currDayCount(*(d->m_DayCount.value(CouponIdx)));
    if (currDayCount.GetAnchorDate().isNull())
        currDayCount.SetAnchorDate(d->SettlementDate);
    const double outstanding = prevIPD.isValid() ? (d->CashFlow.GetAmountOutstanding(prevIPD)) : GetOutstandingAmt();
    if (outstanding == 0.0)
        return 0.0;
    return annualiseCoupon(d->CashFlow.GetInterest(index, CouponIdx) / outstanding, prevIPD.isNull() ? d->LastPaymentDate : prevIPD, index, currDayCount.GetValue(index));

}

double Tranche::getActualCoupon(int index, qint32 CouponIdx) const
{
    Q_D(const Tranche);
    return getActualCoupon(d->CashFlow.GetDate(index), CouponIdx);
}


QString TranchePrivate::downloadISIN() const
{
    QString applicableIsin = ISINcode.isEmpty() ? QString() : *ISINcode.constBegin();
#ifndef NO_BLOOMBERG
    if (applicableIsin.isEmpty()) {
        DEBG_LOG("Downloading ISIN from Bloomberg");
        QBbgLib::QBbgReferenceDataRequest isinReq;
        isinReq.setSecurity(QBbgLib::QBbgSecurity(TrancheName, QBbgLib::QBbgSecurity::stringToYellowKey(BloombergExtension)));
        isinReq.setField("ID_ISIN");
        if (!isinReq.isValidReq())
            return QString();
        QBbgLib::QBbgRequestGroup allReq;
        allReq.addRequest(isinReq);
        QBbgLib::QBbgManager bbgMan;
        const QBbgLib::QBbgReferenceDataResponse* const isinRes = dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(bbgMan.processRequest(allReq).begin().value());
        if (isinRes->hasErrors())
            return QString();
        applicableIsin = isinRes->value().toString();
    }
#endif // !NO_BLOOMBERG
    return applicableIsin.trimmed();
}

bool Tranche::saveCashflowsDatabase() const
{
#ifndef NO_DATABASE
    Q_D(const Tranche);
    bool oneBondFound = false;
    auto applicableIsin = d->ISINcode;
    if (applicableIsin.isEmpty()) 
        applicableIsin << d->downloadISIN();
    if (applicableIsin.constBegin()->isEmpty()) {
        DEBG_LOG("saveCashflowsDatabase() Invalid ISIN");
        return false;
    }
    {
        QMutexLocker dbLocker(&Db_Mutex);
        QSqlDatabase db = QSqlDatabase::database("TwentyFourDB", false);
        if (!db.isValid()) {
            db = QSqlDatabase::addDatabase(GetFromConfig("Database", "DBtype"), "TwentyFourDB");
            db.setDatabaseName(
                "Driver={" + GetFromConfig("Database", "Driver")
                + "}; "
                + GetFromConfig("Database", "DataSource")
                );
            
        }
        bool DbOpen = db.isOpen();
        if (!DbOpen) DbOpen = db.open();
        for (auto i = applicableIsin.constBegin(); DbOpen && i != applicableIsin.constEnd();++i) {
            {
                //Check bond exist in the database
                QSqlQuery CheckBondExistQuery(db);
                CheckBondExistQuery.setForwardOnly(true);
                CheckBondExistQuery.prepare("{CALL " + GetFromConfig("Database", "GetBondDetailsStoredProc") + "}");
                CheckBondExistQuery.bindValue(":isin", *i);
                if (!CheckBondExistQuery.exec()) {
                    DEBG_LOG("saveCashflowsDatabase() Failed to run GetBondDetailsStoredProc");
                    return false;
                }
                if (!CheckBondExistQuery.next()) {
                    DEBG_LOG("saveCashflowsDatabase() Bond not found in Database");
                    continue;
                }
            }
            oneBondFound = true;
            bool dbError;
            QVariantList isinPar, datePar, deferredPar, interestPar, principalPar, balancePar, couponPar;
            isinPar.reserve(d->CashFlow.Count());
            datePar.reserve(d->CashFlow.Count());
            deferredPar.reserve(d->CashFlow.Count());
            interestPar.reserve(d->CashFlow.Count());
            principalPar.reserve(d->CashFlow.Count());
            balancePar.reserve(d->CashFlow.Count());
            couponPar.reserve(d->CashFlow.Count());
            for (int CashFlowIter = 0; CashFlowIter < d->CashFlow.Count(); ++CashFlowIter) {
                isinPar.append(*i);
                datePar.append(d->CashFlow.GetDate(CashFlowIter).toString(Qt::ISODate));
                deferredPar.append(d->CashFlow.GetTotalDeferred(CashFlowIter));
                interestPar.append(d->CashFlow.GetTotalInterest(CashFlowIter));
                principalPar.append(d->CashFlow.GetPrincipal(CashFlowIter));
                balancePar.append(d->CashFlow.GetAmountOutstanding(CashFlowIter));
                couponPar.append(100.0*getTotalActualCoupon(CashFlowIter));
            }
            db.transaction();
            {
                QSqlQuery EraseCashflowQuery(db);
                EraseCashflowQuery.setForwardOnly(true);
                EraseCashflowQuery.prepare("{CALL " + GetFromConfig("Database", "DeleteCashflowsStoredProc") + "}");
                EraseCashflowQuery.bindValue(":ISIN", *i);
                dbError = !EraseCashflowQuery.exec();
                DEBG_LOG_CONDITION("saveCashflowsDatabase() Failed to delete previous cash flows", dbError);
            }
            if (!dbError) {
                QSqlQuery InsertCashflowQuery(db);
                InsertCashflowQuery.setForwardOnly(true);
                InsertCashflowQuery.prepare(GetFromConfig("Database", "InsertCashFlowsQuery"));
                InsertCashflowQuery.bindValue(":isin", isinPar);
                InsertCashflowQuery.bindValue(":date", datePar);
                InsertCashflowQuery.bindValue(":deferred", deferredPar);
                InsertCashflowQuery.bindValue(":interest", interestPar);
                InsertCashflowQuery.bindValue(":principal", principalPar);
                InsertCashflowQuery.bindValue(":balance", balancePar);
                InsertCashflowQuery.bindValue(":coupon", couponPar);
                dbError = !InsertCashflowQuery.execBatch();
                DEBG_LOG_CONDITION("saveCashflowsDatabase() Failed to upload new cash flows", dbError);
            }
            if (dbError) {
                db.rollback();
                DEBG_LOG("saveCashflowsDatabase() Reached Rollback");
                return false;
            }
            else {
                db.commit();
            }
        }
        return oneBondFound;
    }
    DEBG_LOG("saveCashflowsDatabase() Failed to open DB");
#endif // !NO_DATABASE
    return false;
}

void Tranche::getCashflowsDatabase()
{
    Q_D(Tranche);
    d->CashFlow.Clear();
#ifndef NO_DATABASE 
    auto allIsins = d->ISINcode;
    if (allIsins.isEmpty())
        allIsins << d->downloadISIN();
    if (allIsins.constBegin()->isEmpty()) {
        DEBG_LOG("GetDataFromDatabase() Invalid ISIN");
        return;
    }
    QMutexLocker dbLocker(&Db_Mutex);
    QSqlDatabase db = QSqlDatabase::database("TwentyFourDB", false);
    if (!db.isValid()) {
        db = QSqlDatabase::addDatabase(GetFromConfig("Database", "DBtype"), "TwentyFourDB");
        db.setDatabaseName(
            "Driver={" + GetFromConfig("Database", "Driver")
            + "}; "
            + GetFromConfig("Database", "DataSource")
            );

    }
    bool DbOpen = db.isOpen();
    if (!DbOpen) DbOpen = db.open();
    for (auto applicableIsin = allIsins.constBegin(); DbOpen && applicableIsin != allIsins.constEnd(); ++applicableIsin) {
        QSqlQuery CashFlowsQuery(db);
        CashFlowsQuery.setForwardOnly(true);
        CashFlowsQuery.prepare("{CALL " + GetFromConfig("Database", "GetCashFlowsProc") + "}");
        CashFlowsQuery.bindValue(":isin", *applicableIsin);
        DEBG_LOG(QString("Asking Cash Flows Query: %1").arg(getLastExecutedQuery(CashFlowsQuery)));
        if (!CashFlowsQuery.exec()) {
            DEBG_LOG("getCashflowsDatabase() Failed to run GetCashFlowsProc");
                return;
        }
        bool firstFlow = true;
        while (CashFlowsQuery.next()) {
            const QSqlRecord currRec = CashFlowsQuery.record();
            const QDate flowDate = currRec.value("Date").toDate();
            if (firstFlow) {
                d->CashFlow.SetInitialOutstanding(currRec.value("Balance").toDouble() + currRec.value("Deferred").toDouble());
                firstFlow = false;
            }
            d->CashFlow.SetFlow(flowDate, currRec.value("Interest").toDouble(), TrancheCashFlow::InterestFlow);
            d->CashFlow.SetFlow(flowDate, currRec.value("Principal").toDouble(), TrancheCashFlow::PrincipalFlow);
            d->CashFlow.SetFlow(flowDate, currRec.value("Balance").toDouble(), TrancheCashFlow::AmountOutstandingFlow);
            d->CashFlow.SetFlow(flowDate, currRec.value("Deferred").toDouble(), TrancheCashFlow::DeferredFlow);
        }
        if (!firstFlow)
            return;
        DEBG_LOG_CONDITION("getCashflowsDatabase() No cashflows in DB. ISIN: " + *applicableIsin  , d->CashFlow.Count() == 0);
    }
#endif
}

const Ratings& Tranche::getRating() const
{
    Q_D(const Tranche);
    return d->m_rating;
}

void Tranche::setRating(const Ratings& val)
{
    Q_D(Tranche);
    d->m_rating=val;
}



double Tranche::getTotalActualCoupon(const QDate& index) const
{
    Q_D(const Tranche);
    return d->getTotalActualCoupon(index);
}

double Tranche::getTotalActualCoupon(int index) const
{
    Q_D(const Tranche);
    return  d->getTotalActualCoupon(index);
}

void Tranche::SetPaymentFrequency(const QString& a)
{
    Q_D(Tranche);
	IntegerVector TempVect(a);
    if (!TempVect.IsEmpty(1)) d->PaymentFrequency = TempVect;
}
QDataStream& operator<<(QDataStream & stream, const Tranche& flows){
	stream
		<< flows.d_func()->TrancheName
        << flows.d_func()->ISINcode
        << flows.d_func()->OriginalAmt
        << flows.d_func()->Currency
        << flows.d_func()->OutstandingAmt
        << flows.d_func()->Price
        << flows.d_func()->BloombergExtension
        << flows.d_func()->ProrataGroup
        << flows.d_func()->CashFlow
        << flows.d_func()->MinOClevel
        << flows.d_func()->MinIClevel
        << flows.d_func()->LastPaymentDate
        << flows.d_func()->ExchangeRate
        << flows.d_func()->PaymentFrequency
        << flows.d_func()->SettlementDate
        << flows.d_func()->m_UseForwardCurve
        << flows.d_func()->m_rating
	;
    stream << static_cast<qint32>(flows.d_func()->Coupon.size());
    for (auto i = flows.d_func()->Coupon.constBegin(); i != flows.d_func()->Coupon.constEnd(); ++i)
		stream << i.key() << *(i.value());
    stream << static_cast<qint32>(flows.d_func()->ReferenceRate.size());
    for (auto i = flows.d_func()->ReferenceRate.constBegin(); i != flows.d_func()->ReferenceRate.constEnd(); ++i)
		stream << i.key() << *(i.value());
    stream << static_cast<qint32>(flows.d_func()->ReferenceRateValue.size());
    for (auto i = flows.d_func()->ReferenceRateValue.constBegin(); i != flows.d_func()->ReferenceRateValue.constEnd(); ++i)
		stream << i.key() << *(i.value());
    stream << static_cast<qint32>(flows.d_func()->m_DayCount.size());
    for (auto i = flows.d_func()->m_DayCount.constBegin(); i != flows.d_func()->m_DayCount.constEnd(); ++i)
        stream << i.key() << *(i.value());
	return stream;
}
QDataStream& operator>>(QDataStream & stream, Tranche& flows){
	return flows.LoadOldVersion(stream);
}
QDataStream& Tranche::LoadOldVersion(QDataStream& stream){
    Q_D(Tranche);
	qint32 TempSize, TempKey;
	BloombergVector* TempBV;
	BaseRateVector* TempBRV;
    DayCountVector* TempDCV;
	stream >> d->TrancheName;
    if(loadProtocolVersion()<190){
        QString tempISIN;
        stream >> tempISIN;
        d->ISINcode.insert(tempISIN);
    }
    else {
        stream >> d->ISINcode;
    }
    stream >> d->OriginalAmt;
    stream >> d->Currency;
    stream >> d->OutstandingAmt;
    stream >> d->Price;
    stream >> d->BloombergExtension;
    stream >> d->ProrataGroup;
    d->CashFlow.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->CashFlow;
    stream >> d->MinOClevel;
    stream >> d->MinIClevel;
    stream >> d->LastPaymentDate;
    stream >> d->ExchangeRate;
    d->PaymentFrequency.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->PaymentFrequency;
    stream >> d->SettlementDate;
    stream >> d->m_UseForwardCurve;
    if (loadProtocolVersion() > 188)
        stream >> d->m_rating;

	ClearInterest();
	stream >> TempSize;
	for (qint32 i = 0; i < TempSize; i++) {
		TempBV = new BloombergVector();
        TempBV->SetLoadProtocolVersion(loadProtocolVersion());
		stream >> TempKey >> (*TempBV);
        d->Coupon.insert(TempKey, TempBV);
	}
	stream >> TempSize;
	for (qint32 i = 0; i < TempSize; i++) {
		TempBRV = new BaseRateVector();
        TempBRV->SetLoadProtocolVersion(loadProtocolVersion());
		stream >> TempKey >> (*TempBRV);
        d->ReferenceRate.insert(TempKey, TempBRV);
	}
	stream >> TempSize;
	for (qint32 i = 0; i < TempSize; i++) {
		TempBV = new BloombergVector();
        TempBV->SetLoadProtocolVersion(loadProtocolVersion());
		stream >> TempKey >> (*TempBV);
        d->ReferenceRateValue.insert(TempKey, TempBV);
	}
    stream >> TempSize;
    for (qint32 i = 0; i < TempSize; i++) {
        TempDCV = new DayCountVector();
        TempDCV->SetLoadProtocolVersion(loadProtocolVersion());
        stream >> TempKey >> (*TempDCV);
        d->m_DayCount.insert(TempKey, TempDCV);
    }
	ResetProtocolVersion();
	return stream;
}
double Tranche::GetPrice(double DiscountMargin) const {
    Q_D(const Tranche);
	boost::math::tools::eps_tolerance<double> tol(std::numeric_limits<double>::digits / 2);
	double StartGuess = d->Price;
	boost::uintmax_t MaxIter(MaximumIRRIterations);
	std::pair<double, double> Result = boost::math::tools::bracket_and_solve_root(
		[this, &DiscountMargin](double Price) -> double {return GetDiscountMargin(Price) - DiscountMargin; }
		, StartGuess, 2.0, false, tol, MaxIter, boost::math::policies::policy<>());
	if (MaxIter >= MaximumIRRIterations) return -1.0;
	return (Result.first + Result.second) / 2.0;
}

double Tranche::GetPrice() const
{
    Q_D(const Tranche);
    return d->Price;
}

const QString& Tranche::GetBloombergExtension() const
{
    Q_D(const Tranche);
    return d->BloombergExtension;
}

BaseRateVector Tranche::GetReferenceRate(qint32 CoupIndex) const
{
    Q_D(const Tranche);
    return GetCouponPart(CoupIndex, d->ReferenceRate);
}
BloombergVector Tranche::GetReferenceRateValue(qint32 CoupIndex) const {
    Q_D(const Tranche);
    return GetCouponPart(CoupIndex, d->ReferenceRateValue);
}
double Tranche::GetReferenceRateValue(int index, qint32 CoupIndex) const { 
    Q_D(const Tranche);
    if (d->ReferenceRateValue.contains(CoupIndex))
        return d->ReferenceRateValue.value(CoupIndex)->GetValue(index);
	return 0.0;
}
double Tranche::GetReferenceRateValue(const QDate& index, qint32 CoupIndex) const { 
    Q_D(const Tranche);
    if (d->ReferenceRateValue.contains(CoupIndex))
        return d->ReferenceRateValue.value(CoupIndex)->GetValue(index);
	return 0.0;
}

qint32 Tranche::GetProrataGroup(int SeliorityScaleLevel) const
{
    Q_D(const Tranche);
    return d->ProrataGroup.GetSeniority(SeliorityScaleLevel);
}

const Seniority& Tranche::GetProrataGroup() const
{
    Q_D(const Tranche);
    return d->ProrataGroup;
}

TrancheCashFlow& Tranche::GetCashFlow()
{
    Q_D(Tranche);
    return d->CashFlow;
}

const TrancheCashFlow& Tranche::GetCashFlow() const
{
    Q_D(const Tranche);
    return d->CashFlow;
}

double Tranche::GetMinOClevel() const
{
    Q_D(const Tranche);
    return d->MinOClevel;
}

double Tranche::GetMinIClevel() const
{
    Q_D(const Tranche);
    return d->MinIClevel;
}

const QDate& Tranche::GetLastPaymentDate() const
{
    Q_D(const Tranche);
    return d->LastPaymentDate;
}

double Tranche::GetExchangeRate() const
{
    Q_D(const Tranche);
    return d->ExchangeRate;
}

const IntegerVector& Tranche::GetPaymentFrequency() const
{
    Q_D(const Tranche);
    return d->PaymentFrequency;
}

const QSet<QString>& Tranche::GetISIN() const
{
    Q_D(const Tranche);
    return d->ISINcode;
}

BloombergVector Tranche::GetCouponVector(qint32 CoupIndex) const
{
    Q_D(const Tranche);
    return GetCouponPart(CoupIndex, d->Coupon);
}

void Tranche::AddISIN(const QString& a)
{
    Q_D(Tranche);
    if (a.isEmpty())
        return;
    d->ISINcode.insert(a);
}
void Tranche::ClearISIN()
{
    Q_D(Tranche);
    d->ISINcode.clear();
}

DayCountVector Tranche::GetDayCount(qint32 CoupIndex) const
{
    Q_D(const Tranche);
    return GetCouponPart(CoupIndex, d->m_DayCount);
}
void Tranche::SetCoupon(const QString& a, qint32 CoupIndex ) {
    Q_D(Tranche);
    SetCouponPart(a, CoupIndex, d->Coupon);
    if (d->Coupon.contains(CoupIndex))
        d->Coupon[CoupIndex]->SetDivisor(10000);
}
void Tranche::SetReferenceRate(const QString& a, qint32 CoupIndex)
{
    Q_D(Tranche);
    SetCouponPart(a, CoupIndex, d->ReferenceRate);
}

bool Tranche::GetUseForwardCurve() const
{
    Q_D(const Tranche);
    return d->m_UseForwardCurve;
}

void Tranche::SetMinIClevel(double a)
{
    Q_D(Tranche);
    if (a > 0.0 || a == -1.0) 
        d->MinIClevel = a;
}

void Tranche::SetLastPaymentDate(const QDate& a)
{
    Q_D(Tranche);
    d->LastPaymentDate = a;
}

void Tranche::SetDefaultRefRate(const QString& a)
{
    Q_D(Tranche);
	if (BaseRateVector(a).IsEmpty()) return;
    if (d->ReferenceRate.contains(-1))
        (*(d->ReferenceRate[-1])) = a;
	else
        d->ReferenceRate.insert(-1, new BaseRateVector(a));
}
void Tranche::SetDayCount(QString val, qint32 CoupIndex)
{
    Q_D(Tranche);
    SetCouponPart(val, CoupIndex, d->m_DayCount);
}


void Tranche::SetPrice(double a)
{
    Q_D(Tranche);
    if (a > 0) 
        d->Price = a;
}

bool Tranche::AdjHolidays(DayCountConvention a)
{
	return
		a == DayCountConvention::ACT360
		|| a == DayCountConvention::ACT365
		|| a == DayCountConvention::ACTACT
		|| a == DayCountConvention::AFBACTACT
		|| a == DayCountConvention::ISDAACTACT
		;
}

bool Tranche::HasCoupon(qint32 CoupIdx) const {
    Q_D(const Tranche);
	return 
        d->Coupon.contains(CoupIdx)
        && d->ReferenceRate.contains(CoupIdx)
	;
}








