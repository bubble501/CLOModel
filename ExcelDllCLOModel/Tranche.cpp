#include "Tranche.h"
#include "BloombergWorker.h"
#include "CommonFunctions.h"
Tranche::Tranche()
	:DefaultRefRate("BP0003M")
	,LastPaymentDate(2000,1,1)
	,DayCount(360)
	,Currency("GBP")
	,Price(100)
	,BloombergExtension("Mtge")
	,InterestType(FloatingInterest)
	,ProrataGroup(1)
	,ReferenceRateValue(-1.0)
	,MinIClevel(-1.0)
	,MinOClevel(-1.0)
	,ExchangeRate(1.0)
	,OriginalAmt(0.0)
	,OutstandingAmt(0.0)
	,Coupon(0)
	,PaymentFrequency(3)
	,AccruedInterest(0.0)
{}
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
{}
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
double Tranche::GetCoupon() const {
	if(InterestType==FloatingInterest){
		if(ReferenceRateValue==-1.0){
			BloombergWorker Bee;
			Bee.AddSecurity(ReferenceRate,"Index");
			Bee.AddField("PX_LAST");
			ReferenceRateValue=Bee.StartRequest().value(ReferenceRate).value("PX_LAST").toDouble()/100.0;
		}
		return Coupon+ReferenceRateValue;
	}
	return Coupon;
}
void Tranche::SetBloombergExtension(const QString& a){
	QString AdjustedString(a.trimmed().toLower());
	if(AdjustedString.size()==0) return;
	AdjustedString[0]=AdjustedString.at(0).toUpper();
	if(AdjustedString=="Mtge" || AdjustedString=="Corp") BloombergExtension=a;
}
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
	QHash<QString,QString> Result(Bee.StartRequest().value(TrancheName));
	OutstandingAmt=Result.value("AMT_OUTSTANDING").toDouble()/ExchangeRate;
	CashFlow.SetInitialOutstanding(OutstandingAmt);
	OriginalAmt=Result.value("MTG_ORIG_AMT").toDouble()/ExchangeRate;
	Currency=Result.value("CRNCY");
	LastPaymentDate=QDate::fromString(Result.value("START_ACC_DT"),"yyyy-MM-dd");
	PaymentFrequency=static_cast<int>(12.0/Result.value("CPN_FREQ").toDouble());
	if(Result.value("MTG_TYP").contains("FLT")) InterestType=FloatingInterest;
	else InterestType=FixedInterest;
	ReferenceRateValue=-1.0;
	if(InterestType==FloatingInterest){
		ReferenceRate=Result.value("RESET_IDX");
		Coupon=Result.value("FLT_SPREAD").toDouble()/10000.0;
	}
	else{
		Coupon=Result.value("CPN").toDouble()/100.0;
		QString DeafultRefRateString;
		if(Currency=="EUR")DeafultRefRateString="EUR";
		else if(Currency=="GBP")DeafultRefRateString="BP";
		else if(Currency=="USD")DeafultRefRateString="US";
		else return;
		DeafultRefRateString+=QString("%1M").arg(PaymentFrequency,6-DeafultRefRateString.size(),10,QChar('0'));
		DefaultRefRate=DeafultRefRateString;
	}
	AccruedInterest=Result.value("INT_ACC").toDouble()/100.0;
	SettlementDate=QDate::fromString(Result.value("SETTLE_DT"),"yyyy-MM-dd");
}
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
	if(GetLossRate()>0.0000) return 0.0;
	QList<QDate> FlowsDates;
	QList<double> FlowsValues;
	FlowsDates.append(SettlementDate);
	FlowsValues.append(-OutstandingAmt*((NewPrice/100.0)+AccruedInterest));
	for (int i=0;i<CashFlow.Count();i++){
		FlowsDates.append(CashFlow.GetDate(i));
		FlowsValues.append(CashFlow.GetTotalFlow(i));
	}
	if(ReferenceRateValue==-1.0){
		QString ApplicableRate=(ReferenceRate.isEmpty() ? DefaultRefRate:ReferenceRate);
		BloombergWorker Bee;
		Bee.AddSecurity(ApplicableRate,"Index");
		Bee.AddField("PX_LAST");
		ReferenceRateValue=Bee.StartRequest().value(ApplicableRate).value("PX_LAST").toDouble()/100.0;
	}
	return qMax(0.0,CalculateDM(FlowsDates,FlowsValues,ReferenceRateValue,DayCount));
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
	double RunningSum=0.0, Result=0.0, CurrentPrinc;
	for(int i=0;i<CashFlow.Count();i++){
		CurrentPrinc=CashFlow.GetPrincipal(i);
		if(CurrentPrinc>0){
			RunningSum+=CurrentPrinc;
			Result+=CurrentPrinc*static_cast<double>(StartDate.daysTo(CashFlow.GetDate(i)))/365.25;
		}
	}
	return Result/RunningSum;
}
QDataStream& operator<<(QDataStream & stream, const Tranche& flows){
	stream
		<< flows.TrancheName
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
	;
	return stream;
}
QDataStream& operator>>(QDataStream & stream, Tranche& flows){
	qint32 TempInt;
	stream
		>> flows.TrancheName
		>> flows.OriginalAmt
		>> flows.Currency
		>> flows.OutstandingAmt
		>> TempInt;
		flows.InterestType=Tranche::TrancheInterestType(TempInt);
	stream 
		>> flows.Coupon
		>> flows.ReferenceRate
		>> flows.ReferenceRateValue
		>> flows.Price
		>> flows.BloombergExtension
		>> flows.ProrataGroup
		>> flows.CashFlow
		>> flows.MinOClevel
		>> flows.MinIClevel
		>> flows.LastPaymentDate
		>> flows.DayCount
		>> flows.DefaultRefRate
		>> flows.ExchangeRate
		>> flows.PaymentFrequency
		>> flows.SettlementDate
		>> flows.AccruedInterest
	;
	return stream;
}