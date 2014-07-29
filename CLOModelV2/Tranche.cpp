#include "Tranche.h"
#include "BloombergWorker.h"
Tranche::Tranche()
	:DefaultRefRate("BP0003M")
	,LastPaymentDate(2000,1,1)
	,DayCount(360)
	,Currency("GBP")
	,Price(100)
	,BloombergExtension("Mtge")
	,InterestType(FloatingInterest)
	,ProrataGroup(1)
	,MinIClevel(-1.0)
	,MinOClevel(-1.0)
	,ExchangeRate(1.0)
	,OriginalAmt(0.0)
	,OutstandingAmt(0.0)
	,Coupon("0")
	,PaymentFrequency("3")
	,AccruedInterest(0.0)
	, m_optimObjective(0.0)
	, m_UseForwardCurve(false)
{
	Coupon.SetDivisor(10000.0);
}
Tranche::Tranche(const Tranche& a)
	:DefaultRefRate(a.DefaultRefRate)
	,LastPaymentDate(a.LastPaymentDate)
	,DayCount(a.DayCount)
	,Currency(a.Currency)
	,Price(a.Price)
	,BloombergExtension(a.BloombergExtension)
	,InterestType(a.InterestType)
	,ProrataGroup(a.ProrataGroup)
	,ReferenceRateValue(a.ReferenceRateValue)
	,MinIClevel(a.MinIClevel)
	,MinOClevel(a.MinOClevel)
	,ExchangeRate(a.ExchangeRate)
	,OriginalAmt(a.OriginalAmt)
	,OutstandingAmt(a.OutstandingAmt)
	,Coupon(a.Coupon)
	,PaymentFrequency(a.PaymentFrequency)
	,TrancheName(a.TrancheName)
	,ReferenceRate(a.ReferenceRate)
	,CashFlow(a.CashFlow)
	,AccruedInterest(a.AccruedInterest)
	,SettlementDate(a.SettlementDate)
	,ISINcode(a.ISINcode)
	, m_optimObjective(a.m_optimObjective)
	, m_UseForwardCurve(a.m_UseForwardCurve)
{
	m_LoadProtocolVersion = a.m_LoadProtocolVersion;
}
Tranche& Tranche::operator=(const Tranche& a){
	DefaultRefRate=a.DefaultRefRate;
	LastPaymentDate=a.LastPaymentDate;
	DayCount=a.DayCount;
	Currency=a.Currency;
	Price=a.Price;
	BloombergExtension=a.BloombergExtension;
	InterestType=a.InterestType;
	ProrataGroup=a.ProrataGroup;
	ReferenceRateValue=a.ReferenceRateValue;
	MinIClevel=a.MinIClevel;
	MinOClevel=a.MinOClevel;
	ExchangeRate=a.ExchangeRate;
	OriginalAmt=a.OriginalAmt;
	OutstandingAmt=a.OutstandingAmt;
	Coupon=a.Coupon;
	PaymentFrequency=a.PaymentFrequency;
	TrancheName=a.TrancheName;
	ReferenceRate=a.ReferenceRate;
	CashFlow=a.CashFlow;
	AccruedInterest=a.AccruedInterest;
	SettlementDate=a.SettlementDate;
	ISINcode=a.ISINcode;
	m_LoadProtocolVersion=a.m_LoadProtocolVersion;
	m_optimObjective = a.m_optimObjective;
	m_UseForwardCurve = a.m_UseForwardCurve;
	
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
	const BaseRateVector& ApplicableRate = (ReferenceRate.IsEmpty() ? DefaultRefRate : ReferenceRate);
	ReferenceRateValue = ApplicableRate.GetRefRateValueFromBloomberg(ConstantBaseRateTable());
	m_UseForwardCurve = false;
}
#endif
void Tranche::CompileReferenceRateValue(ConstantBaseRateTable& Values) const {
	const BaseRateVector& ApplicableRate= (ReferenceRate.IsEmpty()? DefaultRefRate:ReferenceRate);
	ReferenceRateValue=ApplicableRate.CompileReferenceRateValue(Values);
	m_UseForwardCurve = false;
}
void Tranche::CompileReferenceRateValue(ForwardBaseRateTable& Values) const {
	const BaseRateVector& ApplicableRate = (ReferenceRate.IsEmpty() ? DefaultRefRate : ReferenceRate);
	ReferenceRateValue = ApplicableRate.CompileReferenceRateValue(Values);
	m_UseForwardCurve = true;
}
#ifndef NO_DATABASE
void Tranche::GetBaseRatesDatabase(ConstantBaseRateTable& Values, bool DownloadAll) const {
	const BaseRateVector& ApplicableRate = (ReferenceRate.IsEmpty() ? DefaultRefRate : ReferenceRate);
	ReferenceRateValue = ApplicableRate.GetBaseRatesDatabase(Values, DownloadAll);
	m_UseForwardCurve = false;
}
void Tranche::GetBaseRatesDatabase(ForwardBaseRateTable& Values, bool DownloadAll) const {
	const BaseRateVector& ApplicableRate = (ReferenceRate.IsEmpty() ? DefaultRefRate : ReferenceRate);
	ReferenceRateValue = ApplicableRate.GetBaseRatesDatabase(Values, DownloadAll);
	m_UseForwardCurve = true;
}
#endif
double Tranche::GetCoupon(const QDate& index, int Frequency) const {
	BloombergVector TempCoupon=Coupon;
	if(TempCoupon.GetAnchorDate().isNull()) TempCoupon.SetAnchorDate(LastPaymentDate);
	if(TempCoupon.GetAnchorDate().isNull()) return GetCoupon(0);
	if(InterestType==FloatingInterest){
		if (ReferenceRateValue.IsEmpty())
		#ifdef NO_BLOOMBERG
			ReferenceRateValue = "0";
		#else
			GetRefRateValueFromBloomberg();
		#endif	
		bool NullReferenceRateAnch=ReferenceRateValue.GetAnchorDate().isNull();
		if(NullReferenceRateAnch) ReferenceRateValue.SetAnchorDate(LastPaymentDate);
		double Result=(TempCoupon.GetValue(index,Frequency))+ReferenceRateValue.GetValue(index,Frequency);
		if(NullReferenceRateAnch) ReferenceRateValue.RemoveAnchorDate();
		return Result;		 
	}
	return TempCoupon.GetValue(index);
	//return GetCoupon(MonthDiff(index,Anch));
}
double Tranche::GetCoupon(int index, int Frequency) const {
	QDate CoupDate = CashFlow.GetDate(index);
	if(CoupDate.isNull()) return 0.0;
	return GetCoupon(CoupDate);
}
double Tranche::GetRawCoupon(int index) const {
	return Coupon.GetValue(index);
}
double Tranche::GetRawCoupon(const QDate& index) const {
	QDate Anch=Coupon.GetAnchorDate();
	if(Anch.isNull()) Anch=LastPaymentDate;
	if(Anch.isNull()) return GetCoupon(0);
	return GetRawCoupon(MonthDiff(index,Anch));
}
void Tranche::SetBloombergExtension(const QString& a){
	QString AdjustedString(a.trimmed().toLower());
	if(AdjustedString.size()==0) return;
	AdjustedString[0]=AdjustedString.at(0).toUpper();
	if(AdjustedString=="Mtge" || AdjustedString=="Corp") BloombergExtension=a;
}
#ifndef NO_BLOOMBERG
void Tranche::GetDataFromBloomberg(){
	BloombergWorker Bee;
	Bee.AddSecurity(TrancheName,BloombergExtension);
	Bee.AddField("MTG_ORIG_AMT");
	Bee.AddField("AMT_OUTSTANDING");
	Bee.AddField("CRNCY");
	Bee.AddField("START_ACC_DT");
	Bee.AddField("MTG_TYP");
	Bee.AddField("FLT_SPREAD");
	Bee.AddField("RESET_IDX");
	Bee.AddField("CPN");
	Bee.AddField("CPN_FREQ");
	Bee.AddField("INT_ACC");
	Bee.AddField("SETTLE_DT");
	Bee.AddField("NAME");
	Bee.AddField("ID_ISIN");
	QHash<QString,QString> Result(Bee.StartRequest().value(TrancheName));
	OutstandingAmt=Result.value("AMT_OUTSTANDING").toDouble()/ExchangeRate;
	CashFlow.SetInitialOutstanding(OutstandingAmt);
	OriginalAmt=Result.value("MTG_ORIG_AMT").toDouble()/ExchangeRate;
	Currency=Result.value("CRNCY");
	LastPaymentDate=QDate::fromString(Result.value("START_ACC_DT"),"yyyy-MM-dd");
	PaymentFrequency=QString("%1").arg(static_cast<int>(12.0/Result.value("CPN_FREQ").toDouble()));
	TrancheName=Result.value("NAME");
	ISINcode=Result.value("ID_ISIN");
	if(Result.value("MTG_TYP").contains("FLT")) InterestType=FloatingInterest;
	else InterestType=FixedInterest;
	ReferenceRateValue="";
	if(InterestType==FloatingInterest){
		ReferenceRate=Result.value("RESET_IDX");
		Coupon=Result.value("FLT_SPREAD");//.toDouble()/10000.0;
	}
	else{
		Coupon=QString("%1").arg(Result.value("CPN").toDouble()*100.0);//Result.value("CPN").toDouble()/100.0;
		QString DeafultRefRateString;
		if(Currency=="EUR")DeafultRefRateString="EUR";
		else if(Currency=="GBP")DeafultRefRateString="BP";
		else if(Currency=="USD")DeafultRefRateString="US";
		else return;
		DeafultRefRateString+=QString("%1M").arg(static_cast<int>(12.0/Result.value("CPN_FREQ").toDouble()),6-DeafultRefRateString.size(),10,QChar('0'));
		DefaultRefRate=DeafultRefRateString;
	}
	AccruedInterest=Result.value("INT_ACC").toDouble()/100.0;
	SettlementDate=QDate::fromString(Result.value("SETTLE_DT"),"yyyy-MM-dd");
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
	QList<QDate> FlowsDates;
	QList<double> FlowsValues;
	FlowsDates.append(SettlementDate);
	FlowsValues.append(-OutstandingAmt*((NewPrice/100.0)+AccruedInterest));
	for (int i=0;i<CashFlow.Count();i++){
		FlowsDates.append(CashFlow.GetDate(i));
		FlowsValues.append(CashFlow.GetTotalFlow(i));
	}
	if (ReferenceRateValue.IsEmpty())
#ifdef NO_BLOOMBERG
	ReferenceRateValue = "0";
#else
	GetRefRateValueFromBloomberg();
#endif
	return qMax(0.0,CalculateDM(FlowsDates,FlowsValues,ReferenceRateValue,DayCount));
}
double Tranche::GetIRR() const {return GetIRR(Price);}
double Tranche::GetIRR(double NewPrice)const{
	if(/*GetLossRate()>0.0000 ||*/ OutstandingAmt<0.01) return 0.0;
	QList<QDate> FlowsDates;
	QList<double> FlowsValues;
	FlowsDates.append(SettlementDate);
	FlowsValues.append(-OutstandingAmt*((NewPrice/100.0)+AccruedInterest));
	for (int i=0;i<CashFlow.Count();i++){
		FlowsDates.append(CashFlow.GetDate(i));
		FlowsValues.append(CashFlow.GetTotalFlow(i));
	}
	return qMax(0.0,CalculateIRR(FlowsDates,FlowsValues,DayCount));
}
bool Tranche::operator>(const Tranche& a) const {
	return ProrataGroup>a.ProrataGroup;
}
bool Tranche::operator<(const Tranche& a) const {
	return ProrataGroup<a.ProrataGroup;
}
double Tranche::GetCurrentOutstanding()const{
	if(CashFlow.Count()==0) return OutstandingAmt;
	return CashFlow.GetAmountOutstanding(CashFlow.Count()-1);
}
double Tranche::GetWALife(const QDate& StartDate)const{
	if(OutstandingAmt<0.01 || StartDate.isNull()) return 0.0;
	double RunningSum=0.0, Result=0.0, CurrentPrinc;
	for(int i=0;i<CashFlow.Count();i++){
		CurrentPrinc=CashFlow.GetPrincipal(i);
		if(CurrentPrinc>0){
			RunningSum+=CurrentPrinc;
			Result+=CurrentPrinc*static_cast<double>(StartDate.daysTo(CashFlow.GetDate(i)))/365.25;
		}
	}
	if (RunningSum<=0) return 0.0;
	return Result/RunningSum;
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
		<< qint32(flows.InterestType)
		<< flows.Coupon
		<< flows.ReferenceRate
		<< flows.ReferenceRateValue
		<< flows.Price
		<< flows.BloombergExtension
		<< flows.ProrataGroup
		<< flows.CashFlow
		<< flows.MinOClevel
		<< flows.MinIClevel
		<< flows.LastPaymentDate
		<< flows.DayCount
		<< flows.DefaultRefRate
		<< flows.ExchangeRate
		<< flows.PaymentFrequency
		<< flows.SettlementDate
		<< flows.AccruedInterest
		<< flows.m_UseForwardCurve
	;
	return stream;
}
QDataStream& operator>>(QDataStream & stream, Tranche& flows){
	return flows.LoadOldVersion(stream);
}
QDataStream& Tranche::LoadOldVersion(QDataStream& stream){
	qint32 TempInt;
	stream >> TrancheName;
	stream >> ISINcode;
	stream >> OriginalAmt;
	stream >> Currency;
	stream >> OutstandingAmt;
	stream >> TempInt;
	InterestType=Tranche::TrancheInterestType(TempInt);
	Coupon.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> Coupon;
	ReferenceRate.SetLoadProtocolVersion(m_LoadProtocolVersion);  stream >> ReferenceRate;
	ReferenceRateValue.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> ReferenceRateValue;
	stream >> Price;
	stream >> BloombergExtension;
	stream >> ProrataGroup;
	CashFlow.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> CashFlow;
	stream >> MinOClevel;
	stream >> MinIClevel;
	stream >> LastPaymentDate;
	stream >> DayCount;
	DefaultRefRate.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> DefaultRefRate;
	stream >> ExchangeRate;
	PaymentFrequency.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> PaymentFrequency;
	stream >> SettlementDate;
	stream >> AccruedInterest;
	stream >> m_UseForwardCurve;
	ResetProtocolVersion();
	return stream;
}
double Tranche::GetPrice(double DiscountMargin) const {
	m_optimObjective = DiscountMargin;
	boost::math::tools::eps_tolerance<double> tol(std::numeric_limits<double>::digits / 2);
	double StartGuess = Price;
	boost::uintmax_t MaxIter(100);
	std::pair<double, double> Result = boost::math::tools::bracket_and_solve_root(*this, StartGuess, 2.0, false, tol, MaxIter, boost::math::policies::policy<>());
	if (MaxIter >= 100) return -1.0;
	return (Result.first + Result.second) / 2.0;
}
