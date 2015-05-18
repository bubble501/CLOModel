#include "Mortgage.h"
#include "CommonFunctions.h"
#include <qmath.h>
#ifndef NO_DATABASE
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <QSqlRecord>
#endif
#include "LoanAssumption.h"

Mortgage::Mortgage()
	: m_PaymentFreq("1")
	, m_AnnuityVect("N")
	, m_InterestVect("0")
	, m_FloatRateBase("ZERO")
	, m_FloatingRateBaseValue("0")
	, m_Size(0.0)
	, m_UseForwardCurve(false)
	, m_Properties("")
{}
void Mortgage::SetAnnuity(const QString& a){
	m_AnnuityVect=a;
}
void Mortgage::SetInterest(const QString& a){
	m_InterestVect=a;
}
 Mortgage::Mortgage(const Mortgage& a)
	 : m_FloatRateBase(a.m_FloatRateBase)
	 , m_FloatingRateBaseValue(a.m_FloatingRateBaseValue)
	 ,m_PaymentFreq(a.m_PaymentFreq)
	 ,m_AnnuityVect(a.m_AnnuityVect)
	 ,m_InterestVect(a.m_InterestVect)
	 ,m_Size(a.m_Size)
	 ,m_MaturityDate(a.m_MaturityDate)
	 ,m_CashFlows(a.m_CashFlows)
	 , m_UseForwardCurve(a.m_UseForwardCurve)
	 ,m_Properties(a.m_Properties)
 {}
 const Mortgage& Mortgage::operator=(const Mortgage& a){
	 m_FloatRateBase = a.m_FloatRateBase;
	 m_FloatingRateBaseValue = a.m_FloatingRateBaseValue;
	 m_PaymentFreq=a.m_PaymentFreq;
	 m_AnnuityVect=a.m_AnnuityVect;
	 m_InterestVect=a.m_InterestVect;
	 m_Size=a.m_Size;
	 m_MaturityDate=a.m_MaturityDate;
	 m_CashFlows=a.m_CashFlows;
	 m_UseForwardCurve = a.m_UseForwardCurve;
	 m_Properties = a.m_Properties;
	 return *this;
 }
 bool Mortgage::CalculateCashFlows(const QDate& StartDate, const QString& CPRVecs, const QString& CDRVecs, const QString& LossVecs, const QString& RecoveryLag, const QString& Delinquency, const QString& DelinquencyLag, bool OverrideProperties , bool DownloadScenario) {
	 BloombergVector CPRVec(CPRVecs);
	 BloombergVector CDRVec(CDRVecs);
	 BloombergVector LossVec(LossVecs);
	 IntegerVector RecoveryLagVec(RecoveryLag);
	 BloombergVector DelinquencyVec(Delinquency);
	 IntegerVector DelinquencyLagVec(DelinquencyLag);
	 return CalculateCashFlows(StartDate, CPRVec, CDRVec, LossVec, RecoveryLagVec, DelinquencyVec, DelinquencyLagVec,OverrideProperties,DownloadScenario);
 }
 bool Mortgage::CalculateCashFlows(
	 const QDate& StartDate,
	 BloombergVector CPRVec,
	 BloombergVector CDRVec,
	 BloombergVector LossVec,
	 IntegerVector  RecoveryLag,
	 BloombergVector Delinquency,
	 IntegerVector DelinquencyLag,
	 bool OverrideProperties,
	 bool DownloadScenario
	 ) {
	 QDate AdjStartDate(StartDate.year(), StartDate.month(), 15);
	 MtgCashFlow DelinquenciesFlows; //Delinquencies will be calculated separately and joined at the end
	 m_CashFlows.Clear();
	 qint32 MaturityExtension = 0;
	 double StartingHaircut = 0.0;
	 BloombergVector PrepaymentFee("0");
	 BloombergVector HaircutVector("0");
	 BloombergVector PrepayMultiplier("100");
	 BloombergVector LossMultiplier("100");
	 DayCountVector CurrentDayCountConvention(QString("%1").arg(static_cast<qint16>(DayCountConvention::CompN30360)));

	 auto ApplyProperty = &([&]() {
		 if (HasProperty("MaturityExtension")) MaturityExtension = GetProperty("MaturityExtension").toInt();
		 if (HasProperty("StartingHaircut")) StartingHaircut = GetProperty("StartingHaircut").toDouble();
		 if (HasProperty("PrepaymentFee")) PrepaymentFee = GetProperty("PrepaymentFee");
		 if (HasProperty("DayCount")) { const DayCountVector TempDCV(GetProperty("DayCount")); if (!TempDCV.IsEmpty()) CurrentDayCountConvention = TempDCV; }
		 if (HasProperty("Haircut")) HaircutVector = GetProperty("Haircut");
		 if (HasProperty("PrepayMultiplier")) PrepayMultiplier = GetProperty("PrepayMultiplier");
		 if (HasProperty("LossMultiplier")) LossMultiplier = GetProperty("LossMultiplier");
		 if (!OverrideProperties) {
			 if (HasProperty("CPR")) CPRVec = GetProperty("CPR");
			 if (HasProperty("CDR")) CDRVec = GetProperty("CDR");
			 if (HasProperty("LS")) LossVec = GetProperty("LS");
			 if (HasProperty("RecoveryLag")) RecoveryLag = GetProperty("RecoveryLag");
			 if (HasProperty("Delinquency")) Delinquency = GetProperty("Delinquency");
			 if (HasProperty("DelinquencyLag")) DelinquencyLag = GetProperty("DelinquencyLag");
		 }
	 });

#ifndef Assumptions_ExcelOverDB
	 (*ApplyProperty)();
#endif

#ifndef NO_DATABASE
	 if (HasProperty("Scenario") && HasProperty("Mezzanine") && DownloadScenario) {
		 //Download Scenario From Database
		 Db_Mutex.lock();
		 QSqlDatabase db = QSqlDatabase::database("TwentyFourDB", false);
		 if (!db.isValid()) {
			 if (!db.isValid()) {
				 db = QSqlDatabase::addDatabase(GetFromConfig("Database", "DBtype"), "TwentyFourDB");
				 db.setDatabaseName(
					 "Driver={" + GetFromConfig("Database", "Driver")
					 + "}; "
					 + GetFromConfig("Database", "DataSource")
					 );
				 
			 }
		 }
		 bool DbOpen = db.isOpen();
		 if (!DbOpen) DbOpen = db.open();
		 if (DbOpen) {
			 QSqlQuery LoanAssQuerry(db);
			 LoanAssQuerry.setForwardOnly(true);
			 LoanAssQuerry.prepare("{CALL " + GetFromConfig("Database", "GetLoanAssumptionStoredProc") + "}");
			 LoanAssQuerry.bindValue(":scenarioName", GetProperty("Scenario"));
			 LoanAssQuerry.bindValue(":isSenior", static_cast<bool>(GetProperty("Mezzanine").compare("Yes", Qt::CaseInsensitive) != 0));
			 if (LoanAssQuerry.exec()) {
				 if (LoanAssQuerry.next()) {
					 auto DbgRecord = LoanAssQuerry.record();
					 int FieldCount = 0;
					 QString VecsAnchor;
                     if (!DbgRecord.isNull(FieldCount)) VecsAnchor = "A " + DbgRecord.value(FieldCount).toDate().toString("MM/dd/aaaa") + ' '; ++FieldCount;
					 if (!DbgRecord.isNull(FieldCount)) MaturityExtension = DbgRecord.value(FieldCount).toInt(); ++FieldCount;
					 if (!DbgRecord.isNull(FieldCount)) StartingHaircut = DbgRecord.value(FieldCount).toDouble(); ++FieldCount;
					 if (!DbgRecord.isNull(FieldCount)) PrepaymentFee = VecsAnchor + DbgRecord.value(FieldCount).toString(); ++FieldCount;
					 if (!DbgRecord.isNull(FieldCount)) CurrentDayCountConvention = VecsAnchor + DbgRecord.value(FieldCount).toString(); ++FieldCount;
					 if (!OverrideProperties) {
						 if (!DbgRecord.isNull(FieldCount)) CPRVec = VecsAnchor + DbgRecord.value(FieldCount).toString(); ++FieldCount;
						 if (!DbgRecord.isNull(FieldCount)) CDRVec = VecsAnchor + DbgRecord.value(FieldCount).toString(); ++FieldCount;
						 if (!DbgRecord.isNull(FieldCount)) LossVec = VecsAnchor + DbgRecord.value(FieldCount).toString(); ++FieldCount;
						 if (!DbgRecord.isNull(FieldCount)) RecoveryLag = VecsAnchor + DbgRecord.value(FieldCount).toString(); ++FieldCount;
						 if (!DbgRecord.isNull(FieldCount)) Delinquency = VecsAnchor + DbgRecord.value(FieldCount).toString(); ++FieldCount;
						 if (!DbgRecord.isNull(FieldCount)) DelinquencyLag = VecsAnchor + DbgRecord.value(FieldCount).toString(); ++FieldCount;
					 }
					 else FieldCount += 6;
					 if (!HasProperty("Price") && !DbgRecord.isNull(FieldCount)) SetProperty("Price", QString::number(DbgRecord.value(FieldCount).toDouble(), 'f')); ++FieldCount;
					 if (!DbgRecord.isNull(FieldCount)) HaircutVector = VecsAnchor + DbgRecord.value(FieldCount).toString(); ++FieldCount;
					 if (!DbgRecord.isNull(FieldCount)) PrepayMultiplier = VecsAnchor + DbgRecord.value(FieldCount).toString(); ++FieldCount;
					 if (!DbgRecord.isNull(FieldCount)) LossMultiplier = VecsAnchor + DbgRecord.value(FieldCount).toString(); ++FieldCount;
				 }
			 }
		 }
		 Db_Mutex.unlock();
	 }
#endif
#ifdef Assumptions_ExcelOverDB
	 (*ApplyProperty)();
#endif
     StartingHaircut /= 100.0;
	 if (
		 CPRVec.IsEmpty(0.0, 1.0)
		 || CDRVec.IsEmpty(0.0, 1.0)
		 || LossVec.IsEmpty(0.0, 1.0)
		 || RecoveryLag.IsEmpty(0)
		 || Delinquency.IsEmpty(0.0, 1.0)
		 || DelinquencyLag.IsEmpty(0)
		 || MaturityExtension<0
		 || StartingHaircut<0.0
		 || StartingHaircut>1.0
		 || PrepaymentFee.IsEmpty(0.0)
		 || HaircutVector.IsEmpty(0.0,1.0)
		 || LossMultiplier.IsEmpty(0.0)
		 || PrepayMultiplier.IsEmpty(0.0)
		 ) return false;
	 
	 QDate AdjMaturityDate = QDate(m_MaturityDate.year(), m_MaturityDate.month(), 15).addMonths(MaturityExtension);

	 if (CPRVec.GetAnchorDate().isNull()) CPRVec.SetAnchorDate(AdjStartDate);
	 if (CDRVec.GetAnchorDate().isNull()) CDRVec.SetAnchorDate(AdjStartDate);
	 if (LossVec.GetAnchorDate().isNull()) LossVec.SetAnchorDate(AdjStartDate);
	 if (RecoveryLag.GetAnchorDate().isNull()) RecoveryLag.SetAnchorDate(AdjStartDate);
	 if (Delinquency.GetAnchorDate().isNull()) Delinquency.SetAnchorDate(AdjStartDate);
	 if (DelinquencyLag.GetAnchorDate().isNull()) DelinquencyLag.SetAnchorDate(AdjStartDate);
	 if (PrepaymentFee.GetAnchorDate().isNull()) PrepaymentFee.SetAnchorDate(AdjStartDate);
	 if (HaircutVector.GetAnchorDate().isNull()) HaircutVector.SetAnchorDate(AdjStartDate);
	 if (PrepayMultiplier.GetAnchorDate().isNull()) PrepayMultiplier.SetAnchorDate(AdjStartDate);
	 if (LossMultiplier.GetAnchorDate().isNull()) LossMultiplier.SetAnchorDate(AdjStartDate);
	 if (CurrentDayCountConvention.GetAnchorDate().isNull()) CurrentDayCountConvention.SetAnchorDate(AdjStartDate);
	 
	 if (Delinquency.GetValue(AdjStartDate)>0.0) m_CashFlows.AddFlow(AdjStartDate, m_Size* Delinquency.GetValue(AdjStartDate), MtgCashFlow::MtgFlowType::DelinquentOutstanding);
	 if (MonthDiff(AdjMaturityDate, AdjStartDate) < 1) {
		 m_CashFlows.AddFlow(AdjStartDate, m_Size, MtgCashFlow::MtgFlowType::PrincipalFlow);
		 return true;
	 }
	 
	 
	 //if (!ReadyToCalculate().isEmpty()) return false;
	 bool NullAnchorDates[] = {
		 m_InterestVect.GetAnchorDate().isNull()
		 , m_AnnuityVect.GetAnchorDate().isNull()
		 , m_PaymentFreq.GetAnchorDate().isNull()
	 };
	 if (NullAnchorDates[0]) m_InterestVect.SetAnchorDate(AdjStartDate);
	 if (NullAnchorDates[1]) m_AnnuityVect.SetAnchorDate(AdjStartDate);
	 if (NullAnchorDates[2])  m_PaymentFreq.SetAnchorDate(AdjStartDate);


	 
	 double CurrentInterest, TempFlow1, TempFlow2;
	 int TempStep;
	 double CurrentAmtOut = m_Size*qMax(0.0,1.0 - StartingHaircut-HaircutVector.GetValue(AdjStartDate));
	 m_CashFlows.SetFlow(AdjStartDate, m_Size*qMin(1.0, StartingHaircut + HaircutVector.GetValue(AdjStartDate)), MtgCashFlow::MtgFlowType::LossFlow);
	 m_CashFlows.SetFlow(AdjStartDate, CurrentAmtOut, MtgCashFlow::MtgFlowType::AmountOutstandingFlow);
	 m_CashFlows.SetFlow(AdjStartDate, CurrentAmtOut*GetInterest(AdjStartDate), MtgCashFlow::MtgFlowType::WACouponFlow);
	 QDate NextPaymentDate = AdjStartDate.addMonths(m_PaymentFreq.GetValue(AdjStartDate));
	 if (NextPaymentDate > AdjMaturityDate) NextPaymentDate = AdjMaturityDate;
	 m_CashFlows.AddFlow(AdjStartDate, CurrentAmtOut*static_cast<double>(MonthDiff(AdjMaturityDate, AdjStartDate)) / 12.0, MtgCashFlow::MtgFlowType::WALlevel);
	 for (QDate CurrentMonth = AdjStartDate.addMonths(1); CurrentMonth <= AdjMaturityDate; CurrentMonth = CurrentMonth.addMonths(1)) {
		 
		 

		 //Accrue Interest for the month
		 CurrentInterest = GetInterest(CurrentMonth,CurrentMonth.addMonths(-1),CurrentMonth,CurrentDayCountConvention.GetValue(CurrentMonth));
		 TempFlow1 = (CurrentInterest*(CurrentAmtOut + m_CashFlows.GetAccruedInterest(CurrentMonth))) + m_CashFlows.GetInterestRecoveries(CurrentMonth);
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
						 CurrentDelinq.AddFlow(
							 RollingdateCounter
							 , GetInterest(RollingdateCounter, RollingdateCounter.addMonths(-1), RollingdateCounter, CurrentDayCountConvention.GetValue(RollingdateCounter))*CurrentDelinq.GetAccruedInterest(RollingdateCounter)
							 , MtgCashFlow::MtgFlowType::AccruedInterestFlow
						);
					 }
					 double TempDelinq =  GetInterest(CurrentMonth.addMonths(TempStep), CurrentMonth.addMonths(TempStep - 1), CurrentMonth.addMonths(TempStep), CurrentDayCountConvention.GetValue(CurrentMonth.addMonths(TempStep)))*CurrentDelinq.GetAccruedInterest(CurrentMonth.addMonths(TempStep));
					 CurrentDelinq.AddFlow(CurrentMonth.addMonths(TempStep), TempDelinq, MtgCashFlow::MtgFlowType::InterestFlow);
					 CurrentDelinq.SetFlow(CurrentMonth.addMonths(TempStep), 0.0, MtgCashFlow::MtgFlowType::AccruedInterestFlow);
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
					 TempFlow2 = qAbs(pmt(GetInterest(CurrentMonth, CurrentMonth.addMonths(-m_PaymentFreq.GetValue(CurrentMonth)), CurrentMonth,CurrentDayCountConvention.GetValue(CurrentMonth)), CountPeriods, CurrentAmtOut)) - TempFlow1;
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
			 if (HaircutVector.GetValue(CurrentMonth.addMonths(-1)) == 1.0) {
				 TempFlow1 = CurrentAmtOut * HaircutVector.GetValue(CurrentMonth);
				 TempFlow2 = m_CashFlows.GetAccruedInterest(CurrentMonth) * HaircutVector.GetValue(CurrentMonth);
			 }
			 else {
				 TempFlow1 = CurrentAmtOut - (CurrentAmtOut * (1.0 - HaircutVector.GetValue(CurrentMonth)) / (1.0 - HaircutVector.GetValue(CurrentMonth.addMonths(-1))));
				 TempFlow2 = m_CashFlows.GetAccruedInterest(CurrentMonth) *( 1.0 - ((1.0 - HaircutVector.GetValue(CurrentMonth)) / (1.0 - HaircutVector.GetValue(CurrentMonth.addMonths(-1)))));
			 }
			 m_CashFlows.AddFlow(CurrentMonth, qMin(TempFlow1, CurrentAmtOut), MtgCashFlow::MtgFlowType::LossFlow);
			 CurrentAmtOut = qMax(0.0, CurrentAmtOut - TempFlow1);
			 m_CashFlows.AddFlow(CurrentMonth, qMin(TempFlow2, m_CashFlows.GetAccruedInterest(CurrentMonth)), MtgCashFlow::MtgFlowType::LossOnInterestFlow);
			 m_CashFlows.AddFlow(CurrentMonth, -qMin(TempFlow2, m_CashFlows.GetAccruedInterest(CurrentMonth)), MtgCashFlow::MtgFlowType::AccruedInterestFlow);
			 //////////////////////////////////////////////////////////////////////////
			 //Prepayments
			 TempFlow1 = qMin(CurrentAmtOut, CurrentAmtOut * CPRVec.GetSMM(CurrentMonth, 1) * PrepayMultiplier.GetValue(CurrentMonth));
			 m_CashFlows.AddFlow(CurrentMonth, TempFlow1, MtgCashFlow::MtgFlowType::PrepaymentFlow);
			 CurrentAmtOut -= TempFlow1;
			 TempFlow2 = TempFlow1*PrepaymentFee.GetValue(CurrentMonth);
			 TempFlow1 = m_CashFlows.GetAccruedInterest(CurrentMonth) * qMin(1.0,CPRVec.GetSMM(CurrentMonth, 1) * PrepayMultiplier.GetValue(CurrentMonth));
			 m_CashFlows.AddFlow(CurrentMonth, TempFlow1+TempFlow2, MtgCashFlow::MtgFlowType::InterestFlow);
			 m_CashFlows.AddFlow(CurrentMonth, TempFlow2, MtgCashFlow::MtgFlowType::PrepaymentFees);
			 m_CashFlows.AddFlow(CurrentMonth, -TempFlow1, MtgCashFlow::MtgFlowType::AccruedInterestFlow);
			//////////////////////////////////////////////////////////////////////////
			//Defaults
			 TempFlow1 = qMin(CurrentAmtOut, CurrentAmtOut * CDRVec.GetSMM(CurrentMonth, 1));
			 m_CashFlows.AddFlow(CurrentMonth, TempFlow1, MtgCashFlow::MtgFlowType::PrincipalDefault);
			 CurrentAmtOut -= TempFlow1;
			 //Assign losses and recoveries
			 m_CashFlows.AddFlow(CurrentMonth.addMonths(RecoveryLag.GetValue(CurrentMonth)), qMin(TempFlow1,TempFlow1*LossVec.GetValue(CurrentMonth)*LossMultiplier.GetValue(CurrentMonth)), MtgCashFlow::MtgFlowType::LossFlow);
			 m_CashFlows.AddFlow(CurrentMonth.addMonths(RecoveryLag.GetValue(CurrentMonth)), qMin(TempFlow1, TempFlow1*(1.0-(LossVec.GetValue(CurrentMonth)*LossMultiplier.GetValue(CurrentMonth)))), MtgCashFlow::MtgFlowType::PrincipalRecovered);
			 //In case of no lag in recoveries add the principal back straight away
			 if (RecoveryLag.GetValue(CurrentMonth) == 0) CurrentAmtOut += qMin(TempFlow1, TempFlow1*(1.0 - (LossVec.GetValue(CurrentMonth)*LossMultiplier.GetValue(CurrentMonth))));
			 //Calculate defaulted interest
			 TempFlow1 = m_CashFlows.GetAccruedInterest(CurrentMonth)* qMin(1.0, CDRVec.GetSMM(CurrentMonth, 1));
			 m_CashFlows.AddFlow(CurrentMonth, -TempFlow1, MtgCashFlow::MtgFlowType::AccruedInterestFlow);
			 //Assign losses and recoveries on interest
			 m_CashFlows.AddFlow(CurrentMonth.addMonths(RecoveryLag.GetValue(CurrentMonth)), qMin(TempFlow1, TempFlow1*(1.0 - (LossVec.GetValue(CurrentMonth)*LossMultiplier.GetValue(CurrentMonth)))), MtgCashFlow::MtgFlowType::InterestRecovered);
			 m_CashFlows.AddFlow(CurrentMonth.addMonths(RecoveryLag.GetValue(CurrentMonth)), qMin(TempFlow1, TempFlow1*(LossVec.GetValue(CurrentMonth)*LossMultiplier.GetValue(CurrentMonth))), MtgCashFlow::MtgFlowType::LossOnInterestFlow);
			 if (RecoveryLag.GetValue(CurrentMonth) == 0) m_CashFlows.AddFlow(CurrentMonth, qMin(TempFlow1, TempFlow1*(1.0 - (LossVec.GetValue(CurrentMonth)*LossMultiplier.GetValue(CurrentMonth)))), MtgCashFlow::MtgFlowType::AccruedInterestFlow);
			 //////////////////////////////////////////////////////////////////////////

		 }

		 m_CashFlows.SetFlow(CurrentMonth, CurrentAmtOut, MtgCashFlow::MtgFlowType::AmountOutstandingFlow);
		 if (Delinquency.GetValue(AdjStartDate)>0.0) m_CashFlows.SetFlow(CurrentMonth, CurrentAmtOut*Delinquency.GetValue(CurrentMonth), MtgCashFlow::MtgFlowType::DelinquentOutstanding);
		 m_CashFlows.SetFlow(CurrentMonth, CurrentAmtOut*GetInterest(CurrentMonth), MtgCashFlow::MtgFlowType::WACouponFlow);
		 m_CashFlows.SetFlow(CurrentMonth, CurrentAmtOut*PrepayMultiplier.GetValue(CurrentMonth), MtgCashFlow::MtgFlowType::WAPrepayMult);
		 m_CashFlows.SetFlow(CurrentMonth, CurrentAmtOut*LossMultiplier.GetValue(CurrentMonth), MtgCashFlow::MtgFlowType::WALossMult);
		 m_CashFlows.SetFlow(CurrentMonth, CurrentAmtOut*PrepaymentFee.GetValue(CurrentMonth), MtgCashFlow::MtgFlowType::WAPrepayFees);
		 m_CashFlows.SetFlow(CurrentMonth, CurrentAmtOut*static_cast<double>(MonthDiff(AdjMaturityDate, CurrentMonth)) / 12.0, MtgCashFlow::MtgFlowType::WALlevel);
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
			 m_CashFlows.SetFlow(
				 m_CashFlows.GetDate(WAPriceIter)
				 , LoanPrice*m_CashFlows.GetFlow(WAPriceIter, MtgCashFlow::MtgFlowType::AmountOutstandingFlow)
				 , MtgCashFlow::MtgFlowType::WAPrice
			);
		 }
	 }

	 if (NullAnchorDates[0]) m_InterestVect.RemoveAnchorDate();
	 if (NullAnchorDates[1]) m_AnnuityVect.RemoveAnchorDate();
	 if (NullAnchorDates[2]) m_PaymentFreq.RemoveAnchorDate();
	 FillDiscountOutstanding();
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
		 << flows.m_InterestVect
		 << flows.m_CashFlows
		 << flows.m_FloatRateBase
		 << flows.m_FloatingRateBaseValue
		 << flows.m_PaymentFreq
		 << flows.m_UseForwardCurve
		 << flows.m_Properties
	;
	 return stream;
 }
 QDataStream& Mortgage::LoadOldVersion(QDataStream& stream) {
	 stream >> m_MaturityDate;
	 m_AnnuityVect.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_AnnuityVect;
	 stream >> m_Size;
	 m_InterestVect.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_InterestVect;
	 m_CashFlows.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_CashFlows;
	 m_FloatRateBase.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_FloatRateBase;
	 m_FloatingRateBaseValue.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_FloatingRateBaseValue;
	 m_PaymentFreq.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_PaymentFreq;
	 stream >> m_UseForwardCurve >> m_Properties;
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
	if (HasProperty("LossMultiplier")){if (BloombergVector(GetProperty("LossMultiplier")).IsEmpty(0.0)) Result += "Loss Multiplier\n";}
	if (HasProperty("PrepayMultiplier")){if (BloombergVector(GetProperty("PrepayMultiplier")).IsEmpty(0.0)) Result += "Prepay Multiplier\n";}
	if (m_InterestVect.IsEmpty())Result += "Loan Coupon\n";
	if (HasProperty("Haircut")) {if (BloombergVector(GetProperty("Haircut")).IsEmpty(0.0, 1.0))Result += "Haircut Vector\n";}
	if (m_PaymentFreq.IsEmpty(1))Result += "Loan Payment Frequency\n";
	if (m_FloatRateBase.IsEmpty())Result += "Loan Base Rate\n";
	if (HasProperty("DayCount")) { if (DayCountVector(GetProperty("DayCount")).IsEmpty()) Result += "Loan Day Count Convention\n"; }
	if (HasProperty("CPR")) {if (BloombergVector(GetProperty("CPR")).IsEmpty(0.0, 1.0)) Result += "Loan CPR Assumption\n";}
	if (HasProperty("CDR")) { if (BloombergVector(GetProperty("CDR")).IsEmpty(0.0, 1.0)) Result += "Loan CDR Assumption\n"; }
	if (HasProperty("LS")) { if (BloombergVector(GetProperty("LS")).IsEmpty(0.0, 1.0)) Result += "Loan LS Assumption\n"; }
	if (HasProperty("RecoveryLag")) { if (IntegerVector(GetProperty("RecoveryLag")).IsEmpty(0)) Result += "Loan Recovery Lag Assumption\n"; }
	if (HasProperty("Delinquency")) { if (BloombergVector(GetProperty("Delinquency")).IsEmpty(0.0, 1.0)) Result += "Loan Delinquency Assumption\n"; }
	if (HasProperty("DelinquencyLag")) { if (IntegerVector(GetProperty("DelinquencyLag")).IsEmpty(0)) Result += "Loan Delinquency Lag Assumption\n"; }
	if (HasProperty("Price")) { if (BloombergVector(GetProperty("Price")).IsEmpty(0.0)) Result += "Loan Price\n"; }
	if (HasProperty("MaturityExtension")) {if (GetProperty("MaturityExtension").toInt() < 0) Result += "Loan Maturity Extension\n";}
	if (HasProperty("StartingHaircut")) { double TempHC = GetProperty("StartingHaircut").toDouble(); if (TempHC< 0.0 || TempHC>1.0) Result += "Loan Starting Haircut\n"; }
	if (HasProperty("PrepaymentFee")) { if (BloombergVector(GetProperty("PrepaymentFee")).IsEmpty(0.0)) Result += "Loan Prepayment Fee\n"; }
	if(!Result.isEmpty()) return Result.left(Result.size()-1);
	return Result;
 }

 double Mortgage::GetInterest(const QDate& a, const QDate& StartAccrue, const QDate& EndAccrue, DayCountConvention DayCnt) {
		 if (m_FloatingRateBaseValue.IsEmpty()) {
#ifdef NO_BLOOMBERG
			 m_FloatingRateBaseValue="0";
#else
			 m_FloatingRateBaseValue = m_FloatRateBase.GetRefRateValueFromBloomberg(ConstantBaseRateTable());
			 m_UseForwardCurve = false;
#endif
		 }
		 return  AdjustCoupon((m_InterestVect + m_FloatingRateBaseValue).GetValue(a), StartAccrue, EndAccrue, DayCnt);
 }
 double Mortgage::GetInterest(int a, const QDate& StartAccrue, const QDate& EndAccrue, DayCountConvention DayCnt) {

		 if (m_FloatingRateBaseValue.IsEmpty()) {
#ifdef NO_BLOOMBERG
			 m_FloatingRateBaseValue = "0";
#else
			 m_FloatingRateBaseValue = m_FloatRateBase.GetRefRateValueFromBloomberg(ConstantBaseRateTable());
			 m_UseForwardCurve = false;
#endif
		 }
		 return  AdjustCoupon((m_InterestVect + m_FloatingRateBaseValue).GetValue(a), StartAccrue, EndAccrue, DayCnt);
 }
 double Mortgage::GetInterest(const QDate& a) {
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
 double Mortgage::GetInterest(int a) {
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
 void Mortgage::SetProperty(const QString& PropName, const QString& Value) {
	 if (PropName.isEmpty() || Value.isEmpty() || PropName.contains(QRegExp("#[<>=]#")) || Value.contains(QRegExp("#[<>=]#"))) return;
	 if (HasProperty(PropName)) {
		 QRegExp TempReg("#<#" + PropName.trimmed() + "#=#(.+)#>#", Qt::CaseInsensitive);
		 TempReg.setMinimal(true);
		 m_Properties.replace(TempReg, "#<#" + PropName.trimmed() + "#=#" + Value.trimmed() + "#>#");
	 }
	 else {
		 m_Properties+="#<#" + PropName.trimmed() + "#=#" + Value.trimmed() + "#>#";
		 //LOGDEBUG("Property Set: "+m_Properties);
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

void Mortgage::FillDiscountOutstanding() {
	if (!(HasProperty("PurchasePrice") && HasProperty("DiscountLimit"))) return;
	double PurchPrice = GetProperty("PurchasePrice").toDouble();
	double DiscountLimit = GetProperty("DiscountLimit").toDouble();
	if (PurchPrice > 1) PurchPrice /= 100.0;
	if (DiscountLimit > 1) DiscountLimit /= 100.0;
	if (PurchPrice >= DiscountLimit) return;
	for (int i = 0; i < m_CashFlows.Count(); ++i) {
		m_CashFlows.AddFlow(m_CashFlows.GetDate(i), m_CashFlows.GetAmountOut(i)*PurchPrice, static_cast<qint32>(MtgCashFlow::MtgFlowType::OutstandingForOC));
	}
}

void Mortgage::SetScenario(const LoanAssumption& value, bool OverrideCurrent) {
	if (!HasProperty("Mezzanine")) return;
	auto IsSenior = (GetProperty("Mezzanine").compare("Yes", Qt::CaseInsensitive) != 0) ? LoanAssumption::Senior : LoanAssumption::Mezz;

	if (!HasProperty("MaturityExtension") || OverrideCurrent) SetProperty("MaturityExtension", value.GetAssumption(LoanAssumption::MaturityExtension, IsSenior));
	if (!HasProperty("StartingHaircut") || OverrideCurrent) SetProperty("StartingHaircut", value.GetAssumption(LoanAssumption::InitialHaircut, IsSenior));
	if (!HasProperty("PrepaymentFee") || OverrideCurrent) SetProperty("PrepaymentFee", value.GetAssumption(LoanAssumption::PrepaymentFee, IsSenior));
	if (!HasProperty("DayCount") || OverrideCurrent) SetProperty("DayCount", value.GetAssumption(LoanAssumption::DayCount, IsSenior));
	if (!HasProperty("Haircut") || OverrideCurrent) SetProperty("Haircut", value.GetAssumption(LoanAssumption::Haircut, IsSenior));
	if (!HasProperty("PrepayMultiplier") || OverrideCurrent) SetProperty("PrepayMultiplier", value.GetAssumption(LoanAssumption::PrepayMultiplier, IsSenior));
	if (!HasProperty("LossMultiplier") || OverrideCurrent) SetProperty("LossMultiplier", value.GetAssumption(LoanAssumption::LossMultiplier, IsSenior));
	if (!HasProperty("CPR") || OverrideCurrent) SetProperty("CPR", value.GetAssumption(LoanAssumption::CPR, IsSenior));
	if (!HasProperty("CDR") || OverrideCurrent) SetProperty("CDR", value.GetAssumption(LoanAssumption::CDR, IsSenior));
	if (!HasProperty("LS") || OverrideCurrent) SetProperty("LS", value.GetAssumption(LoanAssumption::LS, IsSenior));
	if (!HasProperty("RecoveryLag") || OverrideCurrent) SetProperty("RecoveryLag", value.GetAssumption(LoanAssumption::RecoveryLag, IsSenior));
	if (!HasProperty("Delinquency") || OverrideCurrent) SetProperty("Delinquency", value.GetAssumption(LoanAssumption::Delinquency, IsSenior));
	if (!HasProperty("DelinquencyLag") || OverrideCurrent) SetProperty("DelinquencyLag", value.GetAssumption(LoanAssumption::DelinquencyLag, IsSenior));
	if (!HasProperty("Price") || OverrideCurrent) SetProperty("Price", value.GetAssumption(LoanAssumption::Price, IsSenior));
}
