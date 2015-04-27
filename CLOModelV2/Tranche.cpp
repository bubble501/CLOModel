#include "Tranche.h"
#include <boost/math/tools/roots.hpp>
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
Tranche::Tranche()
	: LastPaymentDate(2000, 1, 1)
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
Tranche::~Tranche() {
	ClearInterest();
}
void Tranche::ClearInterest() {
	for (QHash<qint32, BloombergVector*>::iterator i = Coupon.begin(); i != Coupon.end(); ++i)
		delete (i.value());
	for (QHash<qint32, BloombergVector*>::iterator i = ReferenceRateValue.begin(); i != ReferenceRateValue.end(); ++i)
		delete (i.value());
	for (QHash<qint32, BaseRateVector*>::iterator i = ReferenceRate.begin(); i != ReferenceRate.end(); ++i)
		delete (i.value());
    for (auto i = m_DayCount.begin(); i != m_DayCount.end(); ++i)
        delete (i.value());
	Coupon.clear();
	ReferenceRateValue.clear();
	ReferenceRate.clear();
    m_DayCount.clear();
}
Tranche::Tranche(const Tranche& a)
	: LastPaymentDate(a.LastPaymentDate)
	, Currency(a.Currency)
	, Price(a.Price)
	, BloombergExtension(a.BloombergExtension)
	, ProrataGroup(a.ProrataGroup)
	, MinIClevel(a.MinIClevel)
	, MinOClevel(a.MinOClevel)
	, ExchangeRate(a.ExchangeRate)
	, OriginalAmt(a.OriginalAmt)
	, OutstandingAmt(a.OutstandingAmt)
	, PaymentFrequency(a.PaymentFrequency)
	, TrancheName(a.TrancheName)
	, CashFlow(a.CashFlow)
	, SettlementDate(a.SettlementDate)
	, ISINcode(a.ISINcode)
	, m_UseForwardCurve(a.m_UseForwardCurve)
{
	m_LoadProtocolVersion = a.m_LoadProtocolVersion;
	for (QHash<qint32, BloombergVector*>::const_iterator i = a.Coupon.constBegin(); i != a.Coupon.constEnd(); ++i)
		Coupon.insert(i.key(), new BloombergVector(*(i.value())));
	for (QHash<qint32, BloombergVector*>::const_iterator i = a.ReferenceRateValue.constBegin(); i != a.ReferenceRateValue.constEnd(); ++i)
		ReferenceRateValue.insert(i.key(), new BloombergVector(*(i.value())));
	for (QHash<qint32, BaseRateVector*>::const_iterator i = a.ReferenceRate.constBegin(); i != a.ReferenceRate.constEnd(); ++i)
		ReferenceRate.insert(i.key(), new BaseRateVector(*(i.value())));
    for (auto i = a.m_DayCount.constBegin(); i != a.m_DayCount.constEnd(); ++i)
        m_DayCount.insert(i.key(), new DayCountVector(*(i.value())));
}
Tranche& Tranche::operator=(const Tranche& a){
	LastPaymentDate=a.LastPaymentDate;
	Currency=a.Currency;
	Price=a.Price;
	BloombergExtension=a.BloombergExtension;
	ProrataGroup=a.ProrataGroup;
	MinIClevel=a.MinIClevel;
	MinOClevel=a.MinOClevel;
	ExchangeRate=a.ExchangeRate;
	OriginalAmt=a.OriginalAmt;
	OutstandingAmt=a.OutstandingAmt;
	PaymentFrequency=a.PaymentFrequency;
	TrancheName=a.TrancheName;
	CashFlow=a.CashFlow;
	SettlementDate=a.SettlementDate;
	ISINcode=a.ISINcode;
	m_LoadProtocolVersion=a.m_LoadProtocolVersion;
	m_UseForwardCurve = a.m_UseForwardCurve;
	ClearInterest();
	for (QHash<qint32, BloombergVector*>::const_iterator i = a.Coupon.constBegin(); i != a.Coupon.constEnd(); ++i)
		Coupon.insert(i.key(), new BloombergVector(*(i.value())));
	for (QHash<qint32, BloombergVector*>::const_iterator i = a.ReferenceRateValue.constBegin(); i != a.ReferenceRateValue.constEnd(); ++i)
		ReferenceRateValue.insert(i.key(), new BloombergVector(*(i.value())));
	for (QHash<qint32, BaseRateVector*>::const_iterator i = a.ReferenceRate.constBegin(); i != a.ReferenceRate.constEnd(); ++i)
		ReferenceRate.insert(i.key(), new BaseRateVector(*(i.value())));
    for (auto i = a.m_DayCount.constBegin(); i != a.m_DayCount.constEnd(); ++i)
        m_DayCount.insert(i.key(), new DayCountVector(*(i.value())));
	return *this;
}
void Tranche::SetOriginalAmount(double a){if(a>=0.0) OriginalAmt=a/ExchangeRate;}
void Tranche::SetOutstandingAmt(double a){
	if(a<0.0) return;
	OutstandingAmt=a/ExchangeRate;
	CashFlow.SetInitialOutstanding(OutstandingAmt);
}
void Tranche::SetExchangeRate(double a){
	if(a<=0.0) return;
	OriginalAmt*=ExchangeRate;
	OutstandingAmt*=ExchangeRate;
	ExchangeRate=a;
	OriginalAmt/=ExchangeRate;
	OutstandingAmt/=ExchangeRate;
	CashFlow.SetInitialOutstanding(OutstandingAmt);
}
#ifndef NO_BLOOMBERG
void Tranche::GetRefRateValueFromBloomberg()const{
	ConstantBaseRateTable TempTable;
	for (QHash<qint32, BaseRateVector*>::const_iterator i = ReferenceRate.constBegin(); i != ReferenceRate.constEnd(); ++i) {
		if (ReferenceRateValue.contains(i.key())) {
			(*(ReferenceRateValue[i.key()])) = i.value()->GetRefRateValueFromBloomberg(TempTable);
		}
		else {
			ReferenceRateValue.insert(i.key(), new BloombergVector(i.value()->GetRefRateValueFromBloomberg(TempTable)));
		}
	}
	m_UseForwardCurve = false;
}
#endif
void Tranche::CompileReferenceRateValue(ConstantBaseRateTable& Values) const {
	for (QHash<qint32, BaseRateVector*>::const_iterator i = ReferenceRate.constBegin(); i != ReferenceRate.constEnd(); ++i) {
		if (ReferenceRateValue.contains(i.key())) {
			(*(ReferenceRateValue[i.key()])) = i.value()->CompileReferenceRateValue(Values);
		}
		else {
			ReferenceRateValue.insert(i.key(), new BloombergVector(i.value()->CompileReferenceRateValue(Values)));
		}
	}
	m_UseForwardCurve = false;
}
void Tranche::CompileReferenceRateValue(ForwardBaseRateTable& Values) const {
	for (QHash<qint32, BaseRateVector*>::const_iterator i = ReferenceRate.constBegin(); i != ReferenceRate.constEnd(); ++i) {
		if (ReferenceRateValue.contains(i.key())) {
			(*(ReferenceRateValue[i.key()])) = i.value()->CompileReferenceRateValue(Values);
		}
		else {
			ReferenceRateValue.insert(i.key(), new BloombergVector(i.value()->CompileReferenceRateValue(Values)));
		}
	}
	m_UseForwardCurve = true;
}
#ifndef NO_DATABASE
void Tranche::GetBaseRatesDatabase(ConstantBaseRateTable& Values, bool DownloadAll) const {
	for (QHash<qint32, BaseRateVector*>::const_iterator i = ReferenceRate.constBegin(); i != ReferenceRate.constEnd(); ++i) {
		if (ReferenceRateValue.contains(i.key())) {
			(*(ReferenceRateValue[i.key()])) = i.value()->GetBaseRatesDatabase(Values, DownloadAll);
		}
		else {
			ReferenceRateValue.insert(i.key(), new BloombergVector(i.value()->GetBaseRatesDatabase(Values, DownloadAll)));
		}
	}
	m_UseForwardCurve = false;
}
void Tranche::GetBaseRatesDatabase(ForwardBaseRateTable& Values, bool DownloadAll) const {
	for (QHash<qint32, BaseRateVector*>::const_iterator i = ReferenceRate.constBegin(); i != ReferenceRate.constEnd(); ++i) {
		if (ReferenceRateValue.contains(i.key())) {
			(*(ReferenceRateValue[i.key()])) = i.value()->GetBaseRatesDatabase(Values, DownloadAll);
		}
		else {
			ReferenceRateValue.insert(i.key(), new BloombergVector(i.value()->GetBaseRatesDatabase(Values, DownloadAll)));
		}
	}
	m_UseForwardCurve = true;
}
#endif
void Tranche::DownloadBaseRates() const {
	#ifndef NO_DATABASE
		if (m_UseForwardCurve)
			GetBaseRatesDatabase(ForwardBaseRateTable());
		else
			GetBaseRatesDatabase(ConstantBaseRateTable());
	#else
		#ifndef NO_BLOOMBERG
			GetRefRateValueFromBloomberg();
		#else
			for (QHash<qint32, BaseRateVector*>::const_iterator i = ReferenceRate.constBegin(); i != ReferenceRate.constEnd(); ++i) {
				if (ReferenceRateValue.contains(i.key())) {
					(*(ReferenceRateValue[i.key()])) = "0";
				}
				else {
					ReferenceRateValue.insert(i.key(), new BloombergVector("0"));
				}
			}
		#endif
	#endif 
}
double Tranche::GetCoupon(const QDate& index, qint32 CoupIndex , int Frequency) const {
	if (!Coupon.contains(CoupIndex)) return 0.0;
	BloombergVector TempCoupon=*(Coupon.value(CoupIndex));
	if(TempCoupon.GetAnchorDate().isNull()) TempCoupon.SetAnchorDate(LastPaymentDate);
	if (TempCoupon.GetAnchorDate().isNull()) TempCoupon.SetAnchorDate(index);
    if (!ReferenceRate.value(CoupIndex)->IsEmpty()) {
		bool DownloadRates = !ReferenceRateValue.contains(CoupIndex);
		if (!DownloadRates) DownloadRates = ReferenceRateValue.value(CoupIndex)->IsEmpty();
		if (DownloadRates)
		{
			#ifndef NO_DATABASE
				if (m_UseForwardCurve)
					GetBaseRatesDatabase(ForwardBaseRateTable());
				else
					GetBaseRatesDatabase(ConstantBaseRateTable());
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

	
		bool NullReferenceRateAnch = ReferenceRateValue.value(CoupIndex)->GetAnchorDate().isNull();
		if (NullReferenceRateAnch) ReferenceRateValue.operator[](CoupIndex)->SetAnchorDate(LastPaymentDate);
		double Result = (TempCoupon.GetValue(index, Frequency)) + ReferenceRateValue.value(CoupIndex)->GetValue(index, Frequency);
		if (NullReferenceRateAnch) ReferenceRateValue.operator[](CoupIndex)->RemoveAnchorDate();
		return Result;
	}
	return TempCoupon.GetValue(index,Frequency);
	//return GetCoupon(MonthDiff(index,Anch));
}
double Tranche::GetCoupon(int index, qint32 CoupIndex, int Frequency) const {
	QDate CoupDate = CashFlow.GetDate(index);
	if(CoupDate.isNull()) return 0.0;
	return GetCoupon(CoupDate, CoupIndex, Frequency);
}
double Tranche::GetTotalCoupon(const QDate& index, int Frequency) const {
	double Result = 0.0;
	for (auto i = Coupon.constBegin(); i != Coupon.constEnd(); ++i) {
		Result += GetCoupon(index, i.key(), Frequency);
	}
	return Result;
}
double Tranche::GetTotalCoupon(int index, int Frequency) const {
	double Result = 0.0;
	for (auto i = Coupon.constBegin(); i != Coupon.constEnd(); ++i) {
		Result += GetCoupon(index, i.key(), Frequency);
	}
	return Result;
}
double Tranche::GetRawCoupon(int index, qint32 CoupIndex, int Frequency) const {
	if (!Coupon.contains(CoupIndex)) return 0.0;
	return Coupon.value(CoupIndex)->GetValue(index, Frequency);
}
double Tranche::GetRawCoupon(const QDate& index, qint32 CoupIndex, int Frequency) const {
	if (!Coupon.contains(CoupIndex)) return 0.0;
	bool NullCoupAnch = Coupon.value(CoupIndex)->GetAnchorDate().isNull();
	if (NullCoupAnch) Coupon.operator[](CoupIndex)->SetAnchorDate(LastPaymentDate);
	if (Coupon.value(CoupIndex)->GetAnchorDate().isNull()) Coupon.operator[](CoupIndex)->SetAnchorDate(index);
	double Result = Coupon.value(CoupIndex)->GetValue(index, Frequency);
	if (NullCoupAnch) Coupon.operator[](CoupIndex)->RemoveAnchorDate();
	return Result;
}
void Tranche::SetBloombergExtension(const QString& a){
	QString AdjustedString(a.trimmed().toLower());
	if(AdjustedString.size()==0) return;
	AdjustedString[0]=AdjustedString.at(0).toUpper();
	if(AdjustedString=="Mtge" || AdjustedString=="Corp") BloombergExtension=a;
}
#ifndef NO_BLOOMBERG
void Tranche::GetDataFromBloomberg() {
    double currentDefer = 0;
	{
		QBbgLib::QBbgManager Bee;
        QBbgLib::QBbgRequestGroup ReqGr;
		QBbgLib::QBbgReferenceDataRequest TempReq;
		QString IdentityCode = (ISINcode.isEmpty() ? TrancheName:ISINcode);
		if (IdentityCode.isEmpty()) return;
        auto BdpExt = QBbgLib::QBbgSecurity::stringToYellowKey(BloombergExtension);
        if (BdpExt == QBbgLib::QBbgSecurity::Invalid) return;
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
        const auto& TmpResponse=Bee.processRequest(ReqGr);

        if (TmpResponse.value(1)->hasErrors())
            OriginalAmt = 0.0;
        else 
            OriginalAmt = dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(TmpResponse.value(1))->value().toDouble() / ExchangeRate;

        if (TmpResponse.value(2)->hasErrors()) 
            OutstandingAmt = 0.0;
        else
            OutstandingAmt = dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(TmpResponse.value(2))->value().toDouble() / ExchangeRate;

        if (TmpResponse.value(3)->hasErrors())
            Currency = "EUR";
        else
            Currency = dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(TmpResponse.value(3))->value().toString();

        if (TmpResponse.value(4)->hasErrors()) 
            LastPaymentDate = QDate();
        else 
            LastPaymentDate = dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(TmpResponse.value(4))->value().toDate();

        bool TempCheck = TmpResponse.value(6)->hasErrors();
        if (!TempCheck)
            TempCheck = !(dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(TmpResponse.value(5))->value().toString().contains("FLT"));
        if (!TempCheck) {
            if (TmpResponse.value(6)->hasErrors()) { 
                if (Coupon.contains(0)) 
                    *(Coupon[0]) = ""; 
                else 
                    Coupon.insert(0, new BloombergVector()); 
            }
			else { 
                if (Coupon.contains(0)) 
                    *(Coupon[0]) = dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(TmpResponse.value(6))->value().toString();
                else 
                    Coupon.insert(0, new BloombergVector(dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(TmpResponse.value(6))->value().toString()));
            }

            if (TmpResponse.value(7)->hasErrors()) { 
                if (ReferenceRate.contains(0))
                    *(ReferenceRate[0]) = ""; 
                else 
                    ReferenceRate.insert(0, new BaseRateVector());
            }
			else {
                if (ReferenceRate.contains(0)) 
                    *(ReferenceRate[0]) = dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(TmpResponse.value(7))->value().toString(); 
                else 
                    ReferenceRate.insert(0, new BaseRateVector(dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(TmpResponse.value(7))->value().toString()));
                if (ReferenceRate.contains(-1))
                    *(ReferenceRate[-1]) = dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(TmpResponse.value(7))->value().toString(); 
                else
                    ReferenceRate.insert(-1, new BaseRateVector(dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(TmpResponse.value(7))->value().toString()));
			}
		}
		else {
            if (TmpResponse.value(8)->hasErrors()) {
                if (Coupon.contains(0)) 
                    *(Coupon[0]) = ""; 
                else 
                    Coupon.insert(0, new BloombergVector()); 
            }
			else { 
                if (Coupon.contains(0))
                    *(Coupon[0]) = QString("%1").arg(dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(TmpResponse.value(8))->value().toDouble()*100.0);
                else 
                    Coupon.insert(0, new BloombergVector(QString("%1").arg(dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(TmpResponse.value(8))->value().toDouble()*100.0)));
            }


			QString DeafultRefRateString;
            if (!TmpResponse.value(9)->hasErrors()) {
				if (Currency == "EUR")
                    DeafultRefRateString = "EUR";
				else if (Currency == "GBP")
                    DeafultRefRateString = "BP";
				else if (Currency == "USD")
                    DeafultRefRateString = "US";
				if (!DeafultRefRateString.isEmpty())
					DeafultRefRateString +=
					QString("%1M")
                    .arg(static_cast<int>(12.0 / dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(TmpResponse.value(9))->value().toDouble()), 6 - DeafultRefRateString.size(), 10, QChar('0'));
			}
			if (ReferenceRate.contains(-1)) 
                *(ReferenceRate[-1]) = DeafultRefRateString; 
            else 
                ReferenceRate.insert(-1, new BaseRateVector(DeafultRefRateString));
		}

        if (TmpResponse.value(9)->hasErrors()) 
            PaymentFrequency = "";
        else 
            PaymentFrequency = QString("%1").arg(static_cast<int>(12.0 / dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(TmpResponse.value(9))->value().toDouble()));

        if (TmpResponse.value(11)->hasErrors()) 
            SettlementDate = QDate();
        else 
            SettlementDate = dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(TmpResponse.value(11))->value().toDate();

        if (TmpResponse.value(12)->hasErrors()) 
            TrancheName = "";
        else 
            TrancheName = dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(TmpResponse.value(12))->value().toString();

        if (TmpResponse.value(13)->hasErrors()) 
            ISINcode = "";
        else 
            ISINcode = dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(TmpResponse.value(13))->value().toString();

        if (TmpResponse.value(14)->hasErrors()) {
            if (m_DayCount.contains(0))
                m_DayCount[0]->operator=(QString::number(static_cast<qint16>(DayCountConvention::ACT360)));
            else
                m_DayCount.insert(0, new DayCountVector(DayCountConvention::ACT360));
        }
        else {
            if (m_DayCount.contains(0))
                m_DayCount[0]->operator=(dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(TmpResponse.value(14))->value().toString());
            else
                m_DayCount.insert(0, new DayCountVector(dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(TmpResponse.value(14))->value().toString()));
        }
        if (!TmpResponse.value(15)->hasErrors()) {
            currentDefer = dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(TmpResponse.value(15))->value().toDouble();
        }
	}
	DownloadBaseRates();
	GetCashFlowsFromBloomberg();
	CashFlow.SetInitialOutstanding(OutstandingAmt);
    CashFlow.SetStartingDeferredInterest(currentDefer, 0);
}
void Tranche::GetCashFlowsFromBloomberg(bool useFwdRates)
{
	CashFlow.Clear();
    QBbgLib::QBbgSecurity TempSecurity(ISINcode.isEmpty() ? TrancheName : ISINcode, QBbgLib::QBbgSecurity::stringToYellowKey(BloombergExtension));
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
			QSqlQuery GetPrepaySpeedQuery(db);
			GetPrepaySpeedQuery.setForwardOnly(true);
			GetPrepaySpeedQuery.prepare("{CALL " + GetFromConfig("Database", "GetPrepaySpeedStoredProc") + "}");
			GetPrepaySpeedQuery.bindValue(":TrancheID", ISINcode);
			if (GetPrepaySpeedQuery.exec()) {
				if (GetPrepaySpeedQuery.next()){
					auto GetPrepaySpeedRecord = GetPrepaySpeedQuery.record();
					for (int RecIter = 0; RecIter < GetPrepaySpeedRecord.count(); ++RecIter) {
						if (GetPrepaySpeedRecord.value(RecIter).toString().isEmpty()) continue;
                        Ovr.setOverride(GetPrepaySpeedRecord.fieldName(RecIter), GetPrepaySpeedRecord.value(RecIter).toString());
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
        if (CashFlow.GetCashFlowsBloomberg(Results.value(1)))
                return;
        if (Results.value(2, nullptr)){
            if(CashFlow.GetCashFlowsBloomberg(Results.value(2)))
                return;
        }
        return CashFlow.Clear();
    }
}
#endif
double Tranche::GetLossRate() const{
	double Result=0.0;
	if(OutstandingAmt<=0) return 0.0;
	for (int i=0;i<CashFlow.Count();i++){
		Result+=CashFlow.GetPrincipal(i);
	}
	Result=1-(Result/OutstandingAmt);
	if(Result<0.00001) return 0;
	return Result;
}
double Tranche::GetDiscountMargin() const {return GetDiscountMargin(Price);}
double Tranche::GetDiscountMargin(double NewPrice)const{
	if(/*GetLossRate()>0.0000 ||*/ OutstandingAmt<0.01) return 0.0;
	
	double AccruedInterest = AdjustCoupon(GetTotalCoupon(0), LastPaymentDate, SettlementDate, m_DayCount.value(0)->GetValue(SettlementDate));
	QDate TempDate;
	QList<QDate> FlowsDates;
	QList<double> FlowsValues;
	FlowsDates.append(SettlementDate);
	FlowsValues.append(-OutstandingAmt*((NewPrice/100.0)+AccruedInterest));
	for (int i=0;i<CashFlow.Count();i++){
		TempDate = CashFlow.GetDate(i);
        while (AdjHolidays(m_DayCount.value(0)->GetValue(TempDate)) && IsHoliday(TempDate))
			TempDate = TempDate.addDays(1);
		FlowsDates.append(TempDate);
		FlowsValues.append(CashFlow.GetTotalFlow(i));
	}
	BloombergVector* ApplicableRate = ReferenceRateValue.value(0, ReferenceRateValue.value(-1, nullptr));
	if (!ApplicableRate) {
		DownloadBaseRates();
		ApplicableRate = ReferenceRateValue.value(0, ReferenceRateValue.value(-1, nullptr));
		if (!ApplicableRate) return 0.0;
	}
	if (ApplicableRate->IsEmpty()) {
		DownloadBaseRates();
		ApplicableRate = ReferenceRateValue.value(0, ReferenceRateValue.value(-1, nullptr));
        if (!ApplicableRate) return 0.0;
		if (ApplicableRate->IsEmpty()) return 0.0;
	}
    return qMax(0.0, CalculateDM(FlowsDates, FlowsValues, *ApplicableRate, *(m_DayCount.value(0))));
}
double Tranche::GetIRR() const {return GetIRR(Price);}
double Tranche::GetIRR(double NewPrice)const{
	if(/*GetLossRate()>0.0000 ||*/ OutstandingAmt<0.01) return 0.0;

    double AccruedInterest = AdjustCoupon(GetTotalCoupon(0), LastPaymentDate, SettlementDate, m_DayCount.value(0)->GetValue(SettlementDate));
	QList<QDate> FlowsDates;
	QList<double> FlowsValues;
	QDate TempDate;
	FlowsDates.append(SettlementDate);
	FlowsValues.append(-OutstandingAmt*((NewPrice/100.0)+AccruedInterest));
	for (int i=0;i<CashFlow.Count();i++){
		TempDate = CashFlow.GetDate(i);
        while (AdjHolidays(m_DayCount.value(0)->GetValue(TempDate)) && IsHoliday(TempDate)) TempDate = TempDate.addDays(1);
		FlowsDates.append(TempDate);
		FlowsValues.append(CashFlow.GetTotalFlow(i));
	}
    return qMax(0.0, CalculateIRR(FlowsDates, FlowsValues, *(m_DayCount.value(0))));
}
double Tranche::GetWALife(const QDate& StartDate)const{
	if(OutstandingAmt<0.01 || StartDate.isNull()) return 0.0;
	double RunningSum=0.0, Result=0.0, CurrentPrinc;
	for(int i=0;i<CashFlow.Count();i++){
		CurrentPrinc=CashFlow.GetPrincipal(i);
		if(CurrentPrinc>0){
			RunningSum+=CurrentPrinc;
			Result+=CurrentPrinc*static_cast<double>(StartDate.daysTo(CashFlow.GetDate(i)));
		}
	}
	if (RunningSum<=0) return 0.0;
	return (Result / 365.25) / RunningSum;
}
void Tranche::SetPaymentFrequency(const QString& a){
	IntegerVector TempVect(a);
	if (!TempVect.IsEmpty(1)) PaymentFrequency = TempVect;
}
QDataStream& operator<<(QDataStream & stream, const Tranche& flows){
	stream
		<< flows.TrancheName
		<< flows.ISINcode
		<< flows.OriginalAmt
		<< flows.Currency
		<< flows.OutstandingAmt
		<< flows.Price
		<< flows.BloombergExtension
		<< flows.ProrataGroup
		<< flows.CashFlow
		<< flows.MinOClevel
		<< flows.MinIClevel
		<< flows.LastPaymentDate
		<< flows.ExchangeRate
		<< flows.PaymentFrequency
		<< flows.SettlementDate
		<< flows.m_UseForwardCurve
	;
	stream << static_cast<qint32>(flows.Coupon.size());
	for (QHash<qint32, BloombergVector*>::const_iterator i = flows.Coupon.constBegin(); i != flows.Coupon.constEnd(); ++i) 
		stream << i.key() << *(i.value());
	stream << static_cast<qint32>(flows.ReferenceRate.size());
	for (QHash<qint32, BaseRateVector*>::const_iterator i = flows.ReferenceRate.constBegin(); i != flows.ReferenceRate.constEnd(); ++i) 
		stream << i.key() << *(i.value());
	stream << static_cast<qint32>(flows.ReferenceRateValue.size());
	for (QHash<qint32, BloombergVector*>::const_iterator i = flows.ReferenceRateValue.constBegin(); i != flows.ReferenceRateValue.constEnd(); ++i) 
		stream << i.key() << *(i.value());
    stream << static_cast<qint32>(flows.m_DayCount.size());
    for (auto i = flows.m_DayCount.constBegin(); i != flows.m_DayCount.constEnd(); ++i)
        stream << i.key() << *(i.value());
	return stream;
}
QDataStream& operator>>(QDataStream & stream, Tranche& flows){
	return flows.LoadOldVersion(stream);
}
QDataStream& Tranche::LoadOldVersion(QDataStream& stream){
	qint32 TempSize, TempInt, TempKey;
	qint16 TempShort;
	BloombergVector* TempBV;
	BaseRateVector* TempBRV;
    DayCountVector* TempDCV;
	stream >> TrancheName;
	stream >> ISINcode;
	stream >> OriginalAmt;
	stream >> Currency;
	stream >> OutstandingAmt;
	stream >> Price;
	stream >> BloombergExtension;
	stream >> ProrataGroup;
	CashFlow.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> CashFlow;
	stream >> MinOClevel;
	stream >> MinIClevel;
	stream >> LastPaymentDate;
	stream >> ExchangeRate;
	PaymentFrequency.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> PaymentFrequency;
	stream >> SettlementDate;
	stream >> m_UseForwardCurve;

	ClearInterest();
	stream >> TempSize;
	for (qint32 i = 0; i < TempSize; i++) {
		TempBV = new BloombergVector();
		TempBV->SetLoadProtocolVersion(m_LoadProtocolVersion);
		stream >> TempKey >> (*TempBV);
		Coupon.insert(TempKey, TempBV);
	}
	stream >> TempSize;
	for (qint32 i = 0; i < TempSize; i++) {
		TempBRV = new BaseRateVector();
		TempBRV->SetLoadProtocolVersion(m_LoadProtocolVersion);
		stream >> TempKey >> (*TempBRV);
		ReferenceRate.insert(TempKey, TempBRV);
	}
	stream >> TempSize;
	for (qint32 i = 0; i < TempSize; i++) {
		TempBV = new BloombergVector();
		TempBV->SetLoadProtocolVersion(m_LoadProtocolVersion);
		stream >> TempKey >> (*TempBV);
		ReferenceRateValue.insert(TempKey, TempBV);
	}
    stream >> TempSize;
    for (qint32 i = 0; i < TempSize; i++) {
        TempDCV = new DayCountVector();
        TempDCV->SetLoadProtocolVersion(m_LoadProtocolVersion);
        stream >> TempKey >> (*TempDCV);
        m_DayCount.insert(TempKey, TempDCV);
    }
	ResetProtocolVersion();
	return stream;
}
double Tranche::GetPrice(double DiscountMargin) const {
	boost::math::tools::eps_tolerance<double> tol(std::numeric_limits<double>::digits / 2);
	double StartGuess = Price;
	boost::uintmax_t MaxIter(MaximumIRRIterations);
	std::pair<double, double> Result = boost::math::tools::bracket_and_solve_root(
		[this, &DiscountMargin](double Price) -> double {return GetDiscountMargin(Price) - DiscountMargin; }
		, StartGuess, 2.0, false, tol, MaxIter, boost::math::policies::policy<>());
	if (MaxIter >= MaximumIRRIterations) return -1.0;
	return (Result.first + Result.second) / 2.0;
}

BaseRateVector Tranche::GetReferenceRate(qint32 CoupIndex) const {
    return GetCouponPart(CoupIndex, ReferenceRate);
}
BloombergVector Tranche::GetReferenceRateValue(qint32 CoupIndex) const {
    return GetCouponPart(CoupIndex, ReferenceRateValue);
}
double Tranche::GetReferenceRateValue(int index, qint32 CoupIndex) const { 
	if (ReferenceRateValue.contains(CoupIndex))
		return ReferenceRateValue.value(CoupIndex)->GetValue(index);
	return 0.0;
}
double Tranche::GetReferenceRateValue(const QDate& index, qint32 CoupIndex) const { 
	if (ReferenceRateValue.contains(CoupIndex))
		return ReferenceRateValue.value(CoupIndex)->GetValue(index);
	return 0.0;
}
BloombergVector Tranche::GetCouponVector(qint32 CoupIndex) const
{
    return GetCouponPart(CoupIndex, Coupon);
}
DayCountVector Tranche::GetDayCount(qint32 CoupIndex) const
{
    return GetCouponPart(CoupIndex, m_DayCount);
}
void Tranche::SetCoupon(const QString& a, qint32 CoupIndex ) {
    SetCouponPart(a, CoupIndex, Coupon);
    if (Coupon.contains(CoupIndex))
        Coupon[CoupIndex]->SetDivisor(10000);
}
void Tranche::SetReferenceRate(const QString& a, qint32 CoupIndex ) {
    SetCouponPart(a, CoupIndex, ReferenceRate);
}

void Tranche::SetDefaultRefRate(const QString& a) {
	if (BaseRateVector(a).IsEmpty()) return;
	if (ReferenceRate.contains(-1))
		(*(ReferenceRate[-1])) = a;
	else
		ReferenceRate.insert(-1, new BaseRateVector(a));
}
void Tranche::SetDayCount(QString val, qint32 CoupIndex)
{
    SetCouponPart(val, CoupIndex, m_DayCount);
}


bool Tranche::AdjHolidays(DayCountConvention a) {
	return
		a == DayCountConvention::ACT360
		|| a == DayCountConvention::ACT365
		|| a == DayCountConvention::ACTACT
		|| a == DayCountConvention::AFBACTACT
		|| a == DayCountConvention::ISDAACTACT
		;
}

bool Tranche::HasCoupon(qint32 CoupIdx) const {
	return 
        Coupon.contains(CoupIdx)
		&& ReferenceRate.contains(CoupIdx)
	;
}








