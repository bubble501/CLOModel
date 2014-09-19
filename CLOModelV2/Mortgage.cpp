#include "Mortgage.h"
#include "CommonFunctions.h"
#include <qmath.h>
Mortgage::Mortgage()
	:m_LossMultiplier("100")
	, m_PrepayMultiplier("100")
	, m_PaymentFreq("1")
	, m_AnnuityVect("N")
	, m_InterestVect("0")
	, m_HaircutVector("0")
	, m_FloatRateBase("ZERO")
	, m_FloatingRateBaseValue("0")
	, m_Size(0.0)
	, m_UseForwardCurve(false)
	, m_Properties("")
	, m_DayCountConvention(DayCountConvention::CompN30360)
{}
void Mortgage::SetAnnuity(const QString& a){
	m_AnnuityVect=a;
}
void Mortgage::SetInterest(const QString& a){
	m_InterestVect=a;
}
 Mortgage::Mortgage(const Mortgage& a)
	 :m_LossMultiplier(a.m_LossMultiplier)
	 ,m_PrepayMultiplier(a.m_PrepayMultiplier)
	 , m_FloatRateBase(a.m_FloatRateBase)
	 , m_FloatingRateBaseValue(a.m_FloatingRateBaseValue)
	 ,m_PaymentFreq(a.m_PaymentFreq)
	 ,m_AnnuityVect(a.m_AnnuityVect)
	 ,m_InterestVect(a.m_InterestVect)
	 ,m_Size(a.m_Size)
	 ,m_MaturityDate(a.m_MaturityDate)
	 ,m_CashFlows(a.m_CashFlows)
	 ,m_HaircutVector(a.m_HaircutVector)
	 , m_UseForwardCurve(a.m_UseForwardCurve)
	 ,m_Properties(a.m_Properties)
	 , m_DayCountConvention(a.m_DayCountConvention)
 {}
 const Mortgage& Mortgage::operator=(const Mortgage& a){
	 m_LossMultiplier=a.m_LossMultiplier;
	 m_PrepayMultiplier=a.m_PrepayMultiplier;
	 m_FloatRateBase = a.m_FloatRateBase;
	 m_FloatingRateBaseValue = a.m_FloatingRateBaseValue;
	 m_PaymentFreq=a.m_PaymentFreq;
	 m_AnnuityVect=a.m_AnnuityVect;
	 m_InterestVect=a.m_InterestVect;
	 m_Size=a.m_Size;
	 m_MaturityDate=a.m_MaturityDate;
	 m_CashFlows=a.m_CashFlows;
	 m_HaircutVector=a.m_HaircutVector;
	 m_UseForwardCurve = a.m_UseForwardCurve;
	 m_Properties = a.m_Properties;
	 m_DayCountConvention = a.m_DayCountConvention;
	 return *this;
 }
 bool Mortgage::CalculateCashFlows(const QDate& StartDate, const QString& CPRVecs, const QString& CDRVecs, const QString& LossVecs, const QString& RecoveryLag, const QString& Delinquency, const QString& DelinquencyLag) {
	 BloombergVector CPRVec(CPRVecs);
	 BloombergVector CDRVec(CDRVecs);
	 BloombergVector LossVec(LossVecs);
	 IntegerVector RecoveryLagVec(RecoveryLag);
	 BloombergVector DelinquencyVec(Delinquency);
	 IntegerVector DelinquencyLagVec(DelinquencyLag);
	 return CalculateCashFlows(StartDate, CPRVec, CDRVec, LossVec, RecoveryLagVec, DelinquencyVec, DelinquencyLagVec);
 }
 bool Mortgage::CalculateCashFlows(
	 const QDate& StartDate,
	 BloombergVector CPRVec,
	 BloombergVector CDRVec,
	 BloombergVector LossVec,
	 IntegerVector  RecoveryLag,
	 BloombergVector Delinquency,
	 IntegerVector DelinquencyLag,
	 bool OverrideProperties
	 )
 {
	 QDate AdjStartDate(StartDate.year(), StartDate.month(), 15);
	 MtgCashFlow DelinquenciesFlows; //Delinquencies will be calculated separately and joined at the end
	 m_CashFlows.Clear();
	 QDate AdjMaturityDate = QDate(m_MaturityDate.year(), m_MaturityDate.month(), 15);
	 if (MonthDiff(m_MaturityDate, StartDate) < 1) {
		 m_CashFlows.AddFlow(AdjStartDate, m_Size, MtgCashFlow::MtgFlowType::PrincipalFlow);
		 return true;
	 }
	 if (!OverrideProperties){
		 if (HasProperty("CPR")) CPRVec = GetProperty("CPR");
		 if (HasProperty("CDR")) CDRVec = GetProperty("CDR");
		 if (HasProperty("LS")) LossVec = GetProperty("LS");
		 if (HasProperty("RecoveryLag")) RecoveryLag = GetProperty("RecoveryLag");
		 if (HasProperty("Delinquency")) Delinquency = GetProperty("Delinquency");
		 if (HasProperty("DelinquencyLag")) DelinquencyLag = GetProperty("DelinquencyLag");
	 }
	 if (
		 CPRVec.IsEmpty(0.0,1.0)
		 || CDRVec.IsEmpty(0.0,1.0)
		 || LossVec.IsEmpty(0.0,1.0)
		 || RecoveryLag.IsEmpty(0)
		 || Delinquency.IsEmpty(0.0,1.0)
		 || DelinquencyLag.IsEmpty(0)
		 ) return false;
	 //if (!ReadyToCalculate().isEmpty()) return false;
	 bool NullAnchorDates[] = {
		 m_InterestVect.GetAnchorDate().isNull()
		 , m_LossMultiplier.GetAnchorDate().isNull()
		 , m_PrepayMultiplier.GetAnchorDate().isNull()
		 , m_HaircutVector.GetAnchorDate().isNull()
		 , m_AnnuityVect.GetAnchorDate().isNull()
		 , m_PaymentFreq.GetAnchorDate().isNull()
	 };
	 if (CPRVec.GetAnchorDate().isNull()) CPRVec.SetAnchorDate(AdjStartDate);
	 if (CDRVec.GetAnchorDate().isNull()) CDRVec.SetAnchorDate(AdjStartDate);
	 if (LossVec.GetAnchorDate().isNull()) LossVec.SetAnchorDate(AdjStartDate);
	 if (RecoveryLag.GetAnchorDate().isNull()) RecoveryLag.SetAnchorDate(AdjStartDate);
	 if (Delinquency.GetAnchorDate().isNull()) Delinquency.SetAnchorDate(AdjStartDate);
	 if (DelinquencyLag.GetAnchorDate().isNull()) DelinquencyLag.SetAnchorDate(AdjStartDate);
	 if (NullAnchorDates[0]) m_InterestVect.SetAnchorDate(AdjStartDate);
	 if (NullAnchorDates[1]) m_LossMultiplier.SetAnchorDate(AdjStartDate);
	 if (NullAnchorDates[2]) m_PrepayMultiplier.SetAnchorDate(AdjStartDate);
	 if (NullAnchorDates[3]) m_HaircutVector.SetAnchorDate(AdjStartDate);
	 if (NullAnchorDates[4]) m_AnnuityVect.SetAnchorDate(AdjStartDate);
	 if (NullAnchorDates[5]) m_PaymentFreq.SetAnchorDate(AdjStartDate);
	 
	 double CurrentInterest, TempFlow1, TempFlow2;
	 int TempStep;
	 double CurrentAmtOut = m_Size*(1.0 - m_HaircutVector.GetValue(AdjStartDate));
	 m_CashFlows.AddFlow(AdjStartDate, m_Size*m_HaircutVector.GetValue(AdjStartDate), MtgCashFlow::MtgFlowType::LossFlow);
	 m_CashFlows.AddFlow(AdjStartDate, CurrentAmtOut, MtgCashFlow::MtgFlowType::AmountOutstandingFlow);
	 m_CashFlows.AddFlow(AdjStartDate, CurrentAmtOut*GetInterest(AdjStartDate), MtgCashFlow::MtgFlowType::WACouponFlow);
	 QDate NextPaymentDate = AdjStartDate.addMonths(m_PaymentFreq.GetValue(AdjStartDate));
	 if (NextPaymentDate > AdjMaturityDate) NextPaymentDate = AdjMaturityDate;
	 for (QDate CurrentMonth = AdjStartDate.addMonths(1); CurrentMonth <= AdjMaturityDate; CurrentMonth = CurrentMonth.addMonths(1)) {
		 
		 

		 //Accrue Interest for the month
		 CurrentInterest = GetInterest(CurrentMonth,CurrentMonth.addMonths(-1),CurrentMonth);
		 TempFlow1 = (CurrentInterest*CurrentAmtOut) + ((1.0 + CurrentInterest)*m_CashFlows.GetAccruedInterest(CurrentMonth.addMonths(-1))) + m_CashFlows.GetInterestRecoveries(CurrentMonth);
		 m_CashFlows.AddFlow(CurrentMonth, TempFlow1, MtgCashFlow::MtgFlowType::AccruedInterestFlow);
		 //Add back the recoveries
		 CurrentAmtOut += m_CashFlows.GetRecoveries(CurrentMonth);
		 if (CurrentMonth >= NextPaymentDate) { //It's a payment date
			 TempFlow1 = m_CashFlows.GetAccruedInterest(CurrentMonth); // Total accrued Interest
			 if (!(m_AnnuityVect.GetValue(CurrentMonth) & RepaymentVector::Capitalization) || CurrentMonth >= AdjMaturityDate) { //If repayment type is capitalization pay only at maturity
				 m_CashFlows.AddFlow(CurrentMonth, TempFlow1*(1.0 - Delinquency.GetValue(CurrentMonth)), MtgCashFlow::MtgFlowType::InterestFlow);// Non delinquent share of interest
				 m_CashFlows.AddFlow(CurrentMonth, -TempFlow1*(1.0 - Delinquency.GetValue(CurrentMonth)), MtgCashFlow::MtgFlowType::AccruedInterestFlow);
				 TempStep = DelinquencyLag.GetValue(CurrentMonth);
				 if (TempStep == 0) { //Delinquencies are not recovered
					 m_CashFlows.AddFlow(CurrentMonth, TempFlow1*Delinquency.GetValue(CurrentMonth), MtgCashFlow::MtgFlowType::LossOnInterestFlow);
					 m_CashFlows.AddFlow(CurrentMonth, -TempFlow1*Delinquency.GetValue(CurrentMonth), MtgCashFlow::MtgFlowType::AccruedInterestFlow);
				 }
				 else { //Accrue interest on delinquent part and pay it out
					 MtgCashFlow CurrentDelinq;
					 QDate RollingdateCounter;
					 CurrentDelinq.AddFlow(CurrentMonth, TempFlow1*Delinquency.GetValue(CurrentMonth), MtgCashFlow::MtgFlowType::AccruedInterestFlow);
					 m_CashFlows.AddFlow(CurrentMonth, -TempFlow1*Delinquency.GetValue(CurrentMonth), MtgCashFlow::MtgFlowType::AccruedInterestFlow);
					 for (RollingdateCounter = CurrentMonth.addMonths(1); RollingdateCounter < CurrentMonth.addMonths(TempStep); RollingdateCounter = RollingdateCounter.addMonths(1)) {
						 CurrentDelinq.AddFlow(RollingdateCounter, (1.0 + GetInterest(RollingdateCounter, RollingdateCounter.addMonths(-1), RollingdateCounter))*CurrentDelinq.GetAccruedInterest(RollingdateCounter.addMonths(-1)), MtgCashFlow::MtgFlowType::AccruedInterestFlow);
					 }
					 CurrentDelinq.AddFlow(CurrentMonth.addMonths(TempStep), (1.0 + GetInterest(CurrentMonth.addMonths(TempStep), CurrentMonth.addMonths(TempStep-1), CurrentMonth.addMonths(TempStep)))*CurrentDelinq.GetAccruedInterest(CurrentMonth.addMonths(TempStep - 1)), MtgCashFlow::MtgFlowType::InterestFlow);
					 //for (int RemAccrIter = 0; RemAccrIter < CurrentDelinq.Count(); RemAccrIter++) CurrentDelinq.AddFlow(CurrentDelinq.GetDate(RemAccrIter), -CurrentDelinq.GetAccruedInterest(RemAccrIter), MtgCashFlow::MtgFlowType::AccruedInterestFlow);
					 DelinquenciesFlows.AddFlow(CurrentDelinq);
				 }
			 }

			 //Pay Principal
			 if (
				 CurrentMonth == AdjMaturityDate ||
				 (m_AnnuityVect.GetValue(CurrentMonth) & (RepaymentVector::InterestOnly | RepaymentVector::Capitalization))
			){
				 if (CurrentMonth == AdjMaturityDate) TempFlow2 = CurrentAmtOut;
				 else TempFlow2 = 0;
			 }
			 else if (m_AnnuityVect.GetValue(CurrentMonth) & (RepaymentVector::Linear | RepaymentVector::Annuity)) {
				 //Count the payments left
				 int CountPeriods = 0;
				 QDate RollingdateCounter = CurrentMonth;
				 while (RollingdateCounter < AdjMaturityDate) {
					 CountPeriods++;
					 RollingdateCounter = RollingdateCounter.addMonths(m_PaymentFreq.GetValue(RollingdateCounter));
				 }CountPeriods++;
				 if (m_AnnuityVect.GetValue(CurrentMonth) & RepaymentVector::Linear) {
					 TempFlow2 = CurrentAmtOut / static_cast<double>(CountPeriods);
				 }
				 else { // RepaymentVector::Annuity
					 TempFlow2 = qAbs(pmt(GetInterest(CurrentMonth, CurrentMonth.addMonths(-m_PaymentFreq.GetValue(CurrentMonth)), CurrentMonth), CountPeriods, CurrentAmtOut)) - TempFlow1;
				 }
			 }
			 m_CashFlows.AddFlow(CurrentMonth, TempFlow2, MtgCashFlow::MtgFlowType::PrincipalFlow);
			 CurrentAmtOut = CurrentAmtOut - TempFlow2;
			 NextPaymentDate = NextPaymentDate.addMonths(m_PaymentFreq.GetValue(CurrentMonth));
			 if (NextPaymentDate > AdjMaturityDate) NextPaymentDate = AdjMaturityDate;
		 }
		 if (CurrentAmtOut > 0) {
			 //////////////////////////////////////////////////////////////////////////
			 // Haircut
			 if (m_HaircutVector.GetValue(CurrentMonth.addMonths(-1)) == 1.0) {
				 TempFlow1 = CurrentAmtOut * m_HaircutVector.GetValue(CurrentMonth);
				 TempFlow2 = m_CashFlows.GetAccruedInterest(CurrentMonth) * m_HaircutVector.GetValue(CurrentMonth);
			 }
			 else {
				 TempFlow1 = CurrentAmtOut - (CurrentAmtOut * (1.0 - m_HaircutVector.GetValue(CurrentMonth)) / (1.0 - m_HaircutVector.GetValue(CurrentMonth.addMonths(-1))));
				 TempFlow2 = m_CashFlows.GetAccruedInterest(CurrentMonth) - (m_CashFlows.GetAccruedInterest(CurrentMonth) * (1.0 - m_HaircutVector.GetValue(CurrentMonth)) / (1.0 - m_HaircutVector.GetValue(CurrentMonth.addMonths(-1))));
			 }
			 m_CashFlows.AddFlow(CurrentMonth, qMin(TempFlow1, CurrentAmtOut), MtgCashFlow::MtgFlowType::LossFlow);
			 CurrentAmtOut = qMax(0.0, CurrentAmtOut - TempFlow1);
			 m_CashFlows.AddFlow(CurrentMonth, qMin(TempFlow2, m_CashFlows.GetAccruedInterest(CurrentMonth)), MtgCashFlow::MtgFlowType::LossOnInterestFlow);
			 m_CashFlows.AddFlow(CurrentMonth, -qMin(TempFlow2, m_CashFlows.GetAccruedInterest(CurrentMonth)), MtgCashFlow::MtgFlowType::AccruedInterestFlow);
			 //////////////////////////////////////////////////////////////////////////
			 //Prepayments
			 TempFlow1 = qMin(CurrentAmtOut, CurrentAmtOut * CPRVec.GetSMM(CurrentMonth, 1) * m_PrepayMultiplier.GetValue(CurrentMonth));
			 m_CashFlows.AddFlow(CurrentMonth, TempFlow1, MtgCashFlow::MtgFlowType::PrepaymentFlow);
			 CurrentAmtOut -= TempFlow1;
			 TempFlow1 = qMin(m_CashFlows.GetAccruedInterest(CurrentMonth), m_CashFlows.GetAccruedInterest(CurrentMonth) * CPRVec.GetSMM(CurrentMonth, 1) * m_PrepayMultiplier.GetValue(CurrentMonth));
			 m_CashFlows.AddFlow(CurrentMonth, TempFlow1, MtgCashFlow::MtgFlowType::InterestFlow);
			 m_CashFlows.AddFlow(CurrentMonth, -TempFlow1, MtgCashFlow::MtgFlowType::AccruedInterestFlow);
			//////////////////////////////////////////////////////////////////////////
			//Defaults
			 TempFlow1 = qMin(CurrentAmtOut, CurrentAmtOut * CDRVec.GetSMM(CurrentMonth, 1));
			 m_CashFlows.AddFlow(CurrentMonth, TempFlow1, MtgCashFlow::MtgFlowType::PrincipalDefault);
			 CurrentAmtOut -= TempFlow1;
			 //Assign losses and recoveries
			 m_CashFlows.AddFlow(CurrentMonth.addMonths(RecoveryLag.GetValue(CurrentMonth)), qMin(TempFlow1,TempFlow1*LossVec.GetValue(CurrentMonth)*m_LossMultiplier.GetValue(CurrentMonth)), MtgCashFlow::MtgFlowType::LossFlow);
			 m_CashFlows.AddFlow(CurrentMonth.addMonths(RecoveryLag.GetValue(CurrentMonth)), qMin(TempFlow1, TempFlow1*(1.0-(LossVec.GetValue(CurrentMonth)*m_LossMultiplier.GetValue(CurrentMonth)))), MtgCashFlow::MtgFlowType::PrincipalRecovered);
			 //In case of no lag in recoveries add the principal back straight away
			 if (RecoveryLag.GetValue(CurrentMonth) == 0) CurrentAmtOut += qMin(TempFlow1, TempFlow1*(1.0 - (LossVec.GetValue(CurrentMonth)*m_LossMultiplier.GetValue(CurrentMonth))));
			 //Calculate defaulted interest
			 TempFlow1 = qMin(m_CashFlows.GetAccruedInterest(CurrentMonth), m_CashFlows.GetAccruedInterest(CurrentMonth) * CDRVec.GetSMM(CurrentMonth, 1));
			 m_CashFlows.AddFlow(CurrentMonth, -TempFlow1, MtgCashFlow::MtgFlowType::AccruedInterestFlow);
			 //Assign losses and recoveries on interest
			 m_CashFlows.AddFlow(CurrentMonth.addMonths(RecoveryLag.GetValue(CurrentMonth)), qMin(TempFlow1, TempFlow1*(1.0 - (LossVec.GetValue(CurrentMonth)*m_LossMultiplier.GetValue(CurrentMonth)))), MtgCashFlow::MtgFlowType::InterestRecovered);
			 m_CashFlows.AddFlow(CurrentMonth.addMonths(RecoveryLag.GetValue(CurrentMonth)), qMin(TempFlow1, TempFlow1*(LossVec.GetValue(CurrentMonth)*m_LossMultiplier.GetValue(CurrentMonth))), MtgCashFlow::MtgFlowType::LossOnInterestFlow);
			 if (RecoveryLag.GetValue(CurrentMonth) == 0) m_CashFlows.AddFlow(CurrentMonth, qMin(TempFlow1, TempFlow1*(1.0 - (LossVec.GetValue(CurrentMonth)*m_LossMultiplier.GetValue(CurrentMonth)))), MtgCashFlow::MtgFlowType::AccruedInterestFlow);
			 //////////////////////////////////////////////////////////////////////////

		 }

		 m_CashFlows.AddFlow(CurrentMonth, CurrentAmtOut, MtgCashFlow::MtgFlowType::AmountOutstandingFlow);
		 m_CashFlows.AddFlow(CurrentMonth, CurrentAmtOut*GetInterest(CurrentMonth), MtgCashFlow::MtgFlowType::WACouponFlow);
		 m_CashFlows.AddFlow(CurrentMonth, CurrentAmtOut*m_PrepayMultiplier.GetValue(CurrentMonth), MtgCashFlow::MtgFlowType::WAPrepayMult);
		 m_CashFlows.AddFlow(CurrentMonth, CurrentAmtOut*m_LossMultiplier.GetValue(CurrentMonth), MtgCashFlow::MtgFlowType::WALossMult);
		 if (CurrentAmtOut < 0.01) break;
	 }
	 for (QDate CurrentMonth = AdjMaturityDate.addMonths(1); CurrentMonth <= m_CashFlows.MaturityDate(); CurrentMonth = CurrentMonth.addMonths(1)) {
		 //Recoveries after maturity get in in cash
		 if (m_CashFlows.GetInterestRecoveries(CurrentMonth)>0.0) m_CashFlows.AddFlow(CurrentMonth, m_CashFlows.GetInterestRecoveries(CurrentMonth), MtgCashFlow::MtgFlowType::InterestFlow);
		 if (m_CashFlows.GetRecoveries(CurrentMonth)>0.0) m_CashFlows.AddFlow(CurrentMonth, m_CashFlows.GetRecoveries(CurrentMonth), MtgCashFlow::MtgFlowType::PrincipalFlow);
	 }
	 m_CashFlows.AddFlow(DelinquenciesFlows);

	 {
		 double LoanPrice;
		 if (HasProperty("Price")) LoanPrice=GetProperty("Price").toDouble()/100.0;
		 else LoanPrice = 1.0;
		 for (int WAPriceIter = 0; WAPriceIter < m_CashFlows.Count(); ++WAPriceIter) {
			 m_CashFlows.AddFlow(
				 m_CashFlows.GetDate(WAPriceIter)
				 , LoanPrice*m_CashFlows.GetFlow(WAPriceIter, MtgCashFlow::MtgFlowType::AmountOutstandingFlow)
				 , MtgCashFlow::MtgFlowType::WAPrice
			);
		 }
	 }

	 if (NullAnchorDates[0]) m_InterestVect.RemoveAnchorDate();
	 if (NullAnchorDates[1]) m_LossMultiplier.RemoveAnchorDate();
	 if (NullAnchorDates[2]) m_PrepayMultiplier.RemoveAnchorDate();
	 if (NullAnchorDates[3]) m_HaircutVector.RemoveAnchorDate();
	 if (NullAnchorDates[4]) m_AnnuityVect.RemoveAnchorDate();
	 if (NullAnchorDates[5]) m_PaymentFreq.RemoveAnchorDate();
	 
	 return true;
 }
 double Mortgage::pmt(double Interest, int Periods, double PresentValue){
	 if(Periods<=0) return 0.0;
	 if(Interest==0.0){
		 return PresentValue/static_cast<double>(Periods);
	 }
	 return (PresentValue*qPow(1.0+Interest,Periods))/((qPow(1.0+Interest,Periods)-1.0)/Interest);
 }
 QDataStream& operator<<(QDataStream & stream, const Mortgage& flows){
	 stream
		 << flows.m_MaturityDate
		 << flows.m_AnnuityVect
		 << flows.m_Size
		 << flows.m_LossMultiplier
		 << flows.m_PrepayMultiplier
		 << flows.m_InterestVect
		 << flows.m_CashFlows
		 << flows.m_FloatRateBase
		 << flows.m_FloatingRateBaseValue
		 << flows.m_PaymentFreq
		 << flows.m_HaircutVector
		 << flows.m_UseForwardCurve
		 << flows.m_Properties
		 << static_cast<qint16>(flows.m_DayCountConvention)
	;
	 return stream;
 }
 QDataStream& Mortgage::LoadOldVersion(QDataStream& stream) {
	 qint16 TempDaycount;
	 stream >> m_MaturityDate;
	 m_AnnuityVect.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_AnnuityVect;
	 stream >> m_Size;
	 m_LossMultiplier.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_LossMultiplier;
	 m_PrepayMultiplier.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_PrepayMultiplier;
	 m_InterestVect.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_InterestVect;
	 m_CashFlows.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_CashFlows;
	 m_FloatRateBase.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_FloatRateBase;
	 m_FloatingRateBaseValue.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_FloatingRateBaseValue;
	 m_PaymentFreq.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_PaymentFreq;
	 m_HaircutVector.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_HaircutVector;
	 stream >> m_UseForwardCurve >> m_Properties >> TempDaycount;
	 m_DayCountConvention = static_cast<DayCountConvention>(TempDaycount);
	 ResetProtocolVersion();
	 return stream;
 }
 QDataStream& operator>>(QDataStream & stream, Mortgage& flows) {
	 return flows.LoadOldVersion(stream);
 }
 QString Mortgage::ReadyToCalculate()const{
	QString Result;
	if(m_MaturityDate<QDate(2000,1,1)) Result+="Loan Maturity Date\n";
	if (m_AnnuityVect.IsEmpty())Result += "Loan Annuity Vector\n";
	if(m_Size<0.0)Result+="Loan Size\n";
	if (m_LossMultiplier.IsEmpty()) Result += "Loss Multiplier\n";
	if (m_PrepayMultiplier.IsEmpty()) Result += "Prepay Multiplier\n";
	if (m_InterestVect.IsEmpty())Result += "Loan Coupon\n";
	if (m_HaircutVector.IsEmpty())Result += "Haircut Vector\n";
	if (m_PaymentFreq.IsEmpty(1))Result += "Loan Payment Frequency\n";
	if (m_DayCountConvention == DayCountConvention::Invalid)Result += "Loan Day Count Convention\n";
	if(!Result.isEmpty()) return Result.left(Result.size()-1);
	return Result;
 }

 double Mortgage::GetInterest(const QDate& a, const QDate& StartAccrue, const QDate& EndAccrue) {
	 if (m_FloatRateBase.IsEmpty()) //Fixed rate
		 return AdjustCoupon(m_InterestVect.GetValue(a),StartAccrue,EndAccrue,m_DayCountConvention);
	 else {
		 if (m_FloatingRateBaseValue.IsEmpty()) {
#ifdef NO_BLOOMBERG
			 m_FloatingRateBaseValue="0";
#else
			 m_FloatingRateBaseValue = m_FloatRateBase.GetRefRateValueFromBloomberg(ConstantBaseRateTable());
			 m_UseForwardCurve = false;
#endif
		 }
		 return  AdjustCoupon((m_InterestVect + m_FloatingRateBaseValue).GetValue(a),StartAccrue, EndAccrue, m_DayCountConvention);
	 }
 }
 double Mortgage::GetInterest(int a, const QDate& StartAccrue, const QDate& EndAccrue) {
	 if (m_FloatRateBase.IsEmpty()) //Fixed rate
		 return AdjustCoupon(m_InterestVect.GetValue(a), StartAccrue, EndAccrue, m_DayCountConvention);
	 else {
		 if (m_FloatingRateBaseValue.IsEmpty()) {
#ifdef NO_BLOOMBERG
			 m_FloatingRateBaseValue = "0";
#else
			 m_FloatingRateBaseValue = m_FloatRateBase.GetRefRateValueFromBloomberg(ConstantBaseRateTable());
			 m_UseForwardCurve = false;
#endif
		 }
		 return  AdjustCoupon((m_InterestVect + m_FloatingRateBaseValue).GetValue(a), StartAccrue, EndAccrue, m_DayCountConvention);
	 }
 }
 double Mortgage::GetInterest(const QDate& a) {
	 if (m_FloatRateBase.IsEmpty()) //Fixed rate
		 return m_InterestVect.GetValue(a);
	 else {
		 if (m_FloatingRateBaseValue.IsEmpty()) {
#ifdef NO_BLOOMBERG
			 m_FloatingRateBaseValue = "0";
#else
			 m_FloatingRateBaseValue = m_FloatRateBase.GetRefRateValueFromBloomberg(ConstantBaseRateTable());
			 m_UseForwardCurve = false;
#endif
		 }
		 return  (m_InterestVect + m_FloatingRateBaseValue).GetValue(a);
	 }
 }
 double Mortgage::GetInterest(int a) {
	 if (m_FloatRateBase.IsEmpty()) //Fixed rate
		 return m_InterestVect.GetValue(a);
	 else {
		 if (m_FloatingRateBaseValue.IsEmpty()) {
#ifdef NO_BLOOMBERG
			 m_FloatingRateBaseValue = "0";
#else
			 m_FloatingRateBaseValue = m_FloatRateBase.GetRefRateValueFromBloomberg(ConstantBaseRateTable());
			 m_UseForwardCurve = false;
#endif
		 }
		 return  (m_InterestVect + m_FloatingRateBaseValue).GetValue(a);
	 }
 }
 void Mortgage::SetProperty(const QString& PropName, const QString& Value) {
	 if (PropName.isEmpty() || Value.isEmpty() || PropName.contains(QRegExp("#[<>=]#")) || Value.contains(QRegExp("#[<>=]#"))) return;
	 if (HasProperty(PropName)) {
		 QRegExp TempReg("#<#" + PropName.trimmed() + "#=#(.+)#>#", Qt::CaseInsensitive);
		 TempReg.setMinimal(true);
		 m_Properties.replace(TempReg, "#<#" + PropName.trimmed() + "#=#" + Value.trimmed() + "#>#");
	 }
	 else {
		 m_Properties+="#<#" + PropName.trimmed() + "#=#" + Value.trimmed() + "#>#";
		 LOGDEBUG(m_Properties);
	 }
 }

QString Mortgage::GetProperty(const QString& PropName) const {
	QRegExp TempReg("#<#" + PropName.trimmed() + "#=#(.+)#>#", Qt::CaseInsensitive);
	TempReg.setMinimal(true);
	if (TempReg.indexIn(m_Properties)!=-1) {
		return TempReg.cap(1);
	}
	return QString();
}

QString Mortgage::GetProperty(qint32 PropIndex, bool PropValue) const {
	if (PropIndex < 0) return QString();
	qint32 pos=0;
	QRegExp TempReg("#<#(.+)#=#(.+)#>#", Qt::CaseInsensitive);
	TempReg.setMinimal(true);
	for (qint32 i = 0; i <= PropIndex; ++i) {
		if ((pos=TempReg.indexIn(m_Properties, pos)) == -1) return QString();
	}
	return TempReg.cap(PropValue ? 2:1);
}

bool Mortgage::HasProperty(const QString& PropName) const {
	QRegExp TempReg('#<#' + PropName.trimmed() + "#=#(.+)#>#", Qt::CaseInsensitive);
	TempReg.setMinimal(true);
	return  TempReg.indexIn(m_Properties) != -1;
}

qint32 Mortgage::GetNumProperties() const {
	return m_Properties.count("#=#");
}

void Mortgage::RemoveProperty(const QString& PropName) {
	if (HasProperty(PropName)) {
		QRegExp TempReg("#<#" + PropName.trimmed() + "#=#.+#>#", Qt::CaseInsensitive);
		TempReg.setMinimal(true);
		m_Properties.replace(TempReg, "");
	}
}
void Mortgage::RemoveProperty(qint32 PropIndex) {
	if (PropIndex < 0) return;
	qint32 pos = 0;
	QRegExp TempReg("#<#(.+)#=#.+#>#", Qt::CaseInsensitive);
	TempReg.setMinimal(true);
	for (qint32 i = 0; i <= PropIndex; ++i) {
		if ((pos=TempReg.indexIn(m_Properties, pos)) == -1) return;
	}
	return RemoveProperty(TempReg.cap(1));
}
