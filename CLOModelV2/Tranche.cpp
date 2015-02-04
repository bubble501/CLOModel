#include "Tranche.h"
#include "QBbgWorker.h"
#include "QSingleBbgRequest.h"
#include <boost/math/tools/roots.hpp>
#ifndef NO_DATABASE
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>
#endif
Tranche::Tranche()
	: LastPaymentDate(2000, 1, 1)
	, m_DayCount(DayCountConvention::ACT360)
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
	InterestType.insert(0, FloatingInterest);
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
	
	Coupon.clear();
	ReferenceRateValue.clear();
	ReferenceRate.clear();
}
Tranche::Tranche(const Tranche& a)
	: LastPaymentDate(a.LastPaymentDate)
	, m_DayCount(a.m_DayCount)
	, Currency(a.Currency)
	, Price(a.Price)
	, BloombergExtension(a.BloombergExtension)
	, InterestType(a.InterestType)
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
}
Tranche& Tranche::operator=(const Tranche& a){
	LastPaymentDate=a.LastPaymentDate;
	m_DayCount = a.m_DayCount;
	Currency=a.Currency;
	Price=a.Price;
	BloombergExtension=a.BloombergExtension;
	InterestType=a.InterestType;
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
	if(InterestType.value(CoupIndex,InvalidType)==FloatingInterest){
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
	{
		QBloombergLib::QBbgWorker Bee;
		QBloombergLib::QBbgRequest TempReq;
		QString IdentityCode = (ISINcode.isEmpty() ? TrancheName:ISINcode);
		if (IdentityCode.isEmpty()) return;
		auto BdpExt = QBloombergLib::QBbgRequest::String2YellowKey(BloombergExtension);
		TempReq.AddRequest(1, IdentityCode, "MTG_ORIG_AMT", BdpExt);
		TempReq.AddRequest(2, IdentityCode, "AMT_OUTSTANDING", BdpExt);
		TempReq.AddRequest(3, IdentityCode, "CRNCY", BdpExt);
		TempReq.AddRequest(4, IdentityCode, "START_ACC_DT", BdpExt);
		TempReq.AddRequest(5, IdentityCode, "MTG_TYP", BdpExt);
		TempReq.AddRequest(6, IdentityCode, "FLT_SPREAD", BdpExt);
		TempReq.AddRequest(7, IdentityCode, "RESET_IDX", BdpExt);
		TempReq.AddRequest(8, IdentityCode, "CPN", BdpExt);
		TempReq.AddRequest(9, IdentityCode, "CPN_FREQ", BdpExt);
		TempReq.AddRequest(11, IdentityCode, "SETTLE_DT", BdpExt);
		TempReq.AddRequest(12, IdentityCode, "NAME", BdpExt);
		TempReq.AddRequest(13, IdentityCode, "ID_ISIN", BdpExt);
		TempReq.AddRequest(14, IdentityCode, "DAY_CNT", BdpExt);


		Bee.StartRequestSync(TempReq);
		if (Bee.GetRequest().HasErrors()) return;

		if (Bee.GetResult(1)->HasErrors()) OriginalAmt = 0.0;
		else OutstandingAmt = Bee.GetResult(1)->GetDouble() / ExchangeRate;

		if (Bee.GetResult(2)->HasErrors()) OutstandingAmt = 0.0;
		else OutstandingAmt = Bee.GetResult(2)->GetDouble() / ExchangeRate;

		if (Bee.GetResult(3)->HasErrors()) Currency = "EUR";
		else Currency = Bee.GetResult(3)->GetString();

		if (Bee.GetResult(4)->HasErrors()) LastPaymentDate = QDate();
		else LastPaymentDate = Bee.GetResult(4)->GetDate();

		if (Bee.GetResult(5)->GetString().contains("FLT")) InterestType[0] = FloatingInterest;
		else InterestType[0] = FixedInterest;

		if (InterestType[0] == FloatingInterest) {
			if (Bee.GetResult(6)->HasErrors()) { if (Coupon.contains(0)) *(Coupon[0]) = ""; else Coupon.insert(0, new BloombergVector()); }
			else { if (Coupon.contains(0)) *(Coupon[0]) = Bee.GetResult(6)->GetString(); else Coupon.insert(0, new BloombergVector(Bee.GetResult(6)->GetString())); }

			if (Bee.GetResult(7)->HasErrors()) { if (ReferenceRate.contains(0)) *(ReferenceRate[0]) = ""; else ReferenceRate.insert(0, new BaseRateVector()); }
			else {
				if (ReferenceRate.contains(0)) *(ReferenceRate[0]) = Bee.GetResult(7)->GetString(); else ReferenceRate.insert(0, new BaseRateVector(Bee.GetResult(7)->GetString()));
				if (ReferenceRate.contains(-1)) *(ReferenceRate[-1]) = Bee.GetResult(7)->GetString(); else ReferenceRate.insert(-1, new BaseRateVector(Bee.GetResult(7)->GetString()));
			}
		}
		else {
			if (Bee.GetResult(8)->HasErrors()) { if (Coupon.contains(0)) *(Coupon[0]) = ""; else Coupon.insert(0, new BloombergVector()); }
			else { if (Coupon.contains(0)) *(Coupon[0]) = QString("%1").arg(Bee.GetResult(8)->GetDouble()*100.0); else Coupon.insert(0, new BloombergVector(QString("%1").arg(Bee.GetResult(8)->GetDouble()*100.0))); }


			QString DeafultRefRateString;
			if (!Bee.GetResult(9)->HasErrors()) {
				if (Currency == "EUR")DeafultRefRateString = "EUR";
				else if (Currency == "GBP")DeafultRefRateString = "BP";
				else if (Currency == "USD")DeafultRefRateString = "US";
				if (!DeafultRefRateString.isEmpty())
					DeafultRefRateString +=
					QString("%1M")
					.arg(static_cast<int>(12.0 / Bee.GetResult(9)->GetDouble()), 6 - DeafultRefRateString.size(), 10, QChar('0'));
			}
			if (ReferenceRate.contains(-1)) *(ReferenceRate[-1]) = DeafultRefRateString; else ReferenceRate.insert(-1, new BaseRateVector(DeafultRefRateString));
		}

		if (Bee.GetResult(9)->HasErrors()) PaymentFrequency = "";
		else PaymentFrequency = QString("%1").arg(static_cast<int>(12.0 / Bee.GetResult(9)->GetDouble()));

		if (Bee.GetResult(11)->HasErrors()) SettlementDate = QDate();
		else SettlementDate = Bee.GetResult(11)->GetDate();

		if (Bee.GetResult(12)->HasErrors()) TrancheName = "";
		else TrancheName = Bee.GetResult(12)->GetString();

		if (Bee.GetResult(13)->HasErrors()) ISINcode = "";
		else ISINcode = Bee.GetResult(13)->GetString();

		if (Bee.GetResult(14)->HasErrors()) m_DayCount = QString::number(static_cast<qint16>(DayCountConvention::ACT360));
		else m_DayCount = QString::number(static_cast<int>(Bee.GetResult(14)->GetDouble()));
	}
	DownloadBaseRates();
	GetCashFlowsFromBloomberg();
	CashFlow.SetInitialOutstanding(OutstandingAmt);
}
void Tranche::GetCashFlowsFromBloomberg() {
	CashFlow.Clear();
	QBloombergLib::QBbgWorker Bee;
	QBloombergLib::QBbgRequest TempReq;
	QString IdentityCode = (ISINcode.isEmpty() ? TrancheName : ISINcode);
	if (IdentityCode.isEmpty()) return;
	QBloombergLib::Overrides Ovr;
#ifndef NO_DATABASE
	Db_Mutex.lock();
	{
		QSqlDatabase db = QSqlDatabase::database("TwentyFourDB", false);
		if (!db.isValid()) {
			db = QSqlDatabase::addDatabase(GetFromConfig("Database", "DBtype", "QODBC"), "TwentyFourDB");
			db.setDatabaseName(
				"Driver={" + GetFromConfig("Database", "Driver", "SQL Server")
				+ "}; "
				+ GetFromConfig("Database", "DataSource", R"(Server=SYNSERVER2\SQLExpress; Initial Catalog = ABSDB; Integrated Security = SSPI; Trusted_Connection = Yes;)")
				);
			db.setConnectOptions("SQL_ATTR_ODBC_VERSION=SQL_OV_ODBC3");
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
						Ovr[GetPrepaySpeedRecord.fieldName(RecIter)] = GetPrepaySpeedRecord.value(RecIter).toString();
					}
				}
			}
		}
	}
	Db_Mutex.unlock();
#endif // !NO_DATABASE
	QString FieldName;
	auto BdpExt = QBloombergLib::QBbgRequest::String2YellowKey(BloombergExtension);
	switch (BdpExt) {
	case QBloombergLib::QBbgRequest::Invalid: return;
		break;
	case QBloombergLib::QBbgRequest::Govt: FieldName = "DES_CASH_FLOW";
		break;
	case QBloombergLib::QBbgRequest::Corp: FieldName = "DES_CASH_FLOW";
		break;
	case QBloombergLib::QBbgRequest::Mtge: FieldName = "MTG_CASH_FLOW";
		break;
	case QBloombergLib::QBbgRequest::MMkt: return;
		break;
	case QBloombergLib::QBbgRequest::Muni: FieldName = "DES_CASH_FLOW";
		break;
	case QBloombergLib::QBbgRequest::Pfd: FieldName = "DES_CASH_FLOW";
		break;
	case QBloombergLib::QBbgRequest::Equity: return;
		break;
	case QBloombergLib::QBbgRequest::Comdty: return;
		break;
	case QBloombergLib::QBbgRequest::Index: return;
		break;
	case QBloombergLib::QBbgRequest::Curncy: return;
		break;
	default:
		break;
	}
	TempReq.AddRequest(1, IdentityCode, FieldName, Ovr, BdpExt);
	Bee.StartRequestSync(TempReq);
	if (Bee.GetRequest().HasErrors()) return;
	if (Bee.GetResult(1)->HasErrors()) return;
	CashFlow.GetCashFlowsBloomberg(*Bee.GetResult(1));
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
	
	double AccruedInterest = AdjustCoupon(GetTotalCoupon(0), LastPaymentDate, SettlementDate, m_DayCount.GetValue(SettlementDate));
	QDate TempDate;
	QList<QDate> FlowsDates;
	QList<double> FlowsValues;
	FlowsDates.append(SettlementDate);
	FlowsValues.append(-OutstandingAmt*((NewPrice/100.0)+AccruedInterest));
	for (int i=0;i<CashFlow.Count();i++){
		TempDate = CashFlow.GetDate(i);
		while (AdjHolidays(m_DayCount.GetValue(TempDate)) && IsHoliday(TempDate))
			TempDate = TempDate.addDays(1);
		FlowsDates.append(TempDate);
		FlowsValues.append(CashFlow.GetTotalFlow(i));
	}
	const BloombergVector* ApplicableRate = ReferenceRateValue.value(0, ReferenceRateValue.value(-1, nullptr));
	if (!ApplicableRate) {
		DownloadBaseRates();
		ApplicableRate = ReferenceRateValue.value(0, ReferenceRateValue.value(-1, nullptr));
		if (!ApplicableRate) return 0.0;
	}
	if (ApplicableRate->IsEmpty()) {
		DownloadBaseRates();
		ApplicableRate = ReferenceRateValue.value(0, ReferenceRateValue.value(-1, nullptr));
		if (ApplicableRate->IsEmpty()) return 0.0;
	}
	return qMax(0.0, CalculateDM(FlowsDates, FlowsValues, *ApplicableRate, m_DayCount));
}
double Tranche::GetIRR() const {return GetIRR(Price);}
double Tranche::GetIRR(double NewPrice)const{
	if(/*GetLossRate()>0.0000 ||*/ OutstandingAmt<0.01) return 0.0;

	double AccruedInterest = AdjustCoupon(GetTotalCoupon(0), LastPaymentDate, SettlementDate, m_DayCount.GetValue(SettlementDate));
	QList<QDate> FlowsDates;
	QList<double> FlowsValues;
	QDate TempDate;
	FlowsDates.append(SettlementDate);
	FlowsValues.append(-OutstandingAmt*((NewPrice/100.0)+AccruedInterest));
	for (int i=0;i<CashFlow.Count();i++){
		TempDate = CashFlow.GetDate(i);
		while (AdjHolidays(m_DayCount.GetValue(TempDate)) && IsHoliday(TempDate)) TempDate = TempDate.addDays(1);
		FlowsDates.append(TempDate);
		FlowsValues.append(CashFlow.GetTotalFlow(i));
	}
	return qMax(0.0,CalculateIRR(FlowsDates,FlowsValues,m_DayCount));
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
		<< flows.m_DayCount
		<< flows.ExchangeRate
		<< flows.PaymentFrequency
		<< flows.SettlementDate
		<< flows.m_UseForwardCurve
	;
	
	stream << static_cast<qint32>(flows.InterestType.size());
	for (QHash<qint32, Tranche::TrancheInterestType>::const_iterator i = flows.InterestType.constBegin(); i != flows.InterestType.constEnd(); ++i) 
		stream << i.key() << static_cast<qint32>(i.value());
	stream << static_cast<qint32>(flows.Coupon.size());
	for (QHash<qint32, BloombergVector*>::const_iterator i = flows.Coupon.constBegin(); i != flows.Coupon.constEnd(); ++i) 
		stream << i.key() << *(i.value());
	stream << static_cast<qint32>(flows.ReferenceRate.size());
	for (QHash<qint32, BaseRateVector*>::const_iterator i = flows.ReferenceRate.constBegin(); i != flows.ReferenceRate.constEnd(); ++i) 
		stream << i.key() << *(i.value());
	stream << static_cast<qint32>(flows.ReferenceRateValue.size());
	for (QHash<qint32, BloombergVector*>::const_iterator i = flows.ReferenceRateValue.constBegin(); i != flows.ReferenceRateValue.constEnd(); ++i) 
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
	m_DayCount.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_DayCount;
	stream >> ExchangeRate;
	PaymentFrequency.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> PaymentFrequency;
	stream >> SettlementDate;
	stream >> m_UseForwardCurve;

	ClearInterest();
	stream >> TempSize;
	for (qint32 i = 0; i < TempSize; i++) {
		stream >> TempKey >> TempInt;
		InterestType.insert(TempKey, static_cast<TrancheInterestType>(TempInt));
	}
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

QString Tranche::GetReferenceRate(qint32 CoupIndex) const {
	if (ReferenceRate.contains(CoupIndex)) 
		return ReferenceRate.value(CoupIndex)->GetVector(); 
	return "";
}
QString Tranche::GetReferenceRateValue(qint32 CoupIndex) const {
	if (ReferenceRateValue.contains(CoupIndex))
		return ReferenceRateValue.value(CoupIndex)->GetVector();
	return "";
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
QString Tranche::GetCouponVector(qint32 CoupIndex ) const {
	if (Coupon.contains(CoupIndex))
		return Coupon.value(CoupIndex)->GetVector();
	return "";
}
void Tranche::SetCoupon(const QString& a, qint32 CoupIndex ) {
	if (CoupIndex<0 || CoupIndex>=(1 << MaximumInterestsTypes)) return;
	if (Coupon.contains(CoupIndex)) 
		(*(Coupon[CoupIndex])) = a;
	else 
		Coupon.insert(CoupIndex, new BloombergVector(a));
}
void Tranche::SetReferenceRate(const QString& a, qint32 CoupIndex ) {
	if (CoupIndex<0 || CoupIndex >= (1 << MaximumInterestsTypes)) return;
	if (BaseRateVector(a).IsEmpty() /*&& !a.isEmpty()*/) return;
	if (ReferenceRate.contains(CoupIndex))
		(*(ReferenceRate[CoupIndex])) = a;
	else
		ReferenceRate.insert(CoupIndex, new BaseRateVector(a));
}

void Tranche::SetDefaultRefRate(const QString& a) {
	if (BaseRateVector(a).IsEmpty()) return;
	if (ReferenceRate.contains(-1))
		(*(ReferenceRate[-1])) = a;
	else
		ReferenceRate.insert(-1, new BaseRateVector(a));
}

void Tranche::SetInterestType(TrancheInterestType a, qint32 CoupIndex ) {
	if (CoupIndex<0 || CoupIndex >= (1 << MaximumInterestsTypes)) return;
	InterestType[CoupIndex] = a;
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
		InterestType.contains(CoupIdx)
		&& Coupon.contains(CoupIdx)
		&& (
			InterestType.value(CoupIdx) != TrancheInterestType::FloatingInterest
			|| ReferenceRate.contains(CoupIdx)
		);
}




