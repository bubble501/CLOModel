#include "Mortgage.h"
#include "Private/Mortgage_p.h"
#include "Private/InternalItems.h"
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
DEFINE_PUBLIC_COMMONS(Mortgage)
DEFINE_PUBLIC_COMMONS_COPY(Mortgage)

MortgagePrivate::MortgagePrivate(Mortgage *q)
	:BackwardInterfacePrivate(q)
{}
Mortgage::Mortgage(MortgagePrivate *d, const Mortgage& other)
	:BackwardInterface(d,other)
{
    d->m_FloatRateBase = other.d_func()->m_FloatRateBase;
    d->m_FloatingRateBaseValue = other.d_func()->m_FloatingRateBaseValue;
    d->m_PaymentFreq = other.d_func()->m_PaymentFreq;
    d->m_AnnuityVect = other.d_func()->m_AnnuityVect;
    d->m_InterestVect = other.d_func()->m_InterestVect;
    d->m_Size = other.d_func()->m_Size;
    d->m_MaturityDate = other.d_func()->m_MaturityDate;
    d->m_CashFlows = other.d_func()->m_CashFlows;
    d->m_UseForwardCurve = other.d_func()->m_UseForwardCurve;
    d->m_Properties = other.d_func()->m_Properties;
}
Mortgage& Mortgage::operator=(const Mortgage& other){
	Q_D(Mortgage);
	BackwardInterface::operator=(other);
    d->m_FloatRateBase = other.d_func()->m_FloatRateBase;
    d->m_FloatingRateBaseValue = other.d_func()->m_FloatingRateBaseValue;
    d->m_PaymentFreq = other.d_func()->m_PaymentFreq;
    d->m_AnnuityVect = other.d_func()->m_AnnuityVect;
    d->m_InterestVect = other.d_func()->m_InterestVect;
    d->m_Size = other.d_func()->m_Size;
    d->m_MaturityDate = other.d_func()->m_MaturityDate;
    d->m_CashFlows = other.d_func()->m_CashFlows;
    d->m_UseForwardCurve = other.d_func()->m_UseForwardCurve;
    d->m_Properties = other.d_func()->m_Properties;
	return *this;
}
Mortgage::Mortgage(MortgagePrivate *d)
	:BackwardInterface(d)
{
     d->m_PaymentFreq="1";
     d->m_AnnuityVect = "N";
     d->m_InterestVect = "0";
     d->m_FloatRateBase = "ZERO";
     d->m_FloatingRateBaseValue = "0";
     d->m_Size = 0.0;
     d->m_UseForwardCurve = false;
     d->m_Properties = "";
}

void Mortgage::SetAnnuity(const QString& a){
    Q_D(Mortgage);
	d->m_AnnuityVect=a;
}
void Mortgage::SetInterest(const QString& a){
    Q_D(Mortgage);
    d->m_InterestVect = a;
}

double Mortgage::GetSize() const
{
    Q_D(const Mortgage);
    return d->m_Size;
}

void Mortgage::SetSize(double a)
{
    Q_D(Mortgage);
    if (a >= 0) 
        d->m_Size = a;
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
     Q_D(Mortgage);
	 QDate AdjStartDate(StartDate.year(), StartDate.month(), 15);
	 MtgCashFlow DelinquenciesFlows; //Delinquencies will be calculated separately and joined at the end
	 d->m_CashFlows.Clear();
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
	 
     QDate AdjMaturityDate = QDate(d->m_MaturityDate.year(), d->m_MaturityDate.month(), 15).addMonths(MaturityExtension);

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
	 
     if (Delinquency.GetValue(AdjStartDate)>0.0) d->m_CashFlows.AddFlow(AdjStartDate, d->m_Size* Delinquency.GetValue(AdjStartDate), MtgCashFlow::MtgFlowType::DelinquentOutstanding);
	 if (MonthDiff(AdjMaturityDate, AdjStartDate) < 1) {
         d->m_CashFlows.AddFlow(AdjStartDate, d->m_Size, MtgCashFlow::MtgFlowType::PrincipalFlow);
		 return true;
	 }
	 
	 
	 //if (!ReadyToCalculate().isEmpty()) return false;
	 bool NullAnchorDates[] = {
         d->m_InterestVect.GetAnchorDate().isNull()
         , d->m_AnnuityVect.GetAnchorDate().isNull()
         , d->m_PaymentFreq.GetAnchorDate().isNull()
	 };
     if (NullAnchorDates[0]) d->m_InterestVect.SetAnchorDate(AdjStartDate);
     if (NullAnchorDates[1]) d->m_AnnuityVect.SetAnchorDate(AdjStartDate);
     if (NullAnchorDates[2])  d->m_PaymentFreq.SetAnchorDate(AdjStartDate);


	 
	 double CurrentInterest, TempFlow1, TempFlow2;
	 int TempStep;
     double CurrentAmtOut = d->m_Size*qMax(0.0, 1.0 - StartingHaircut - HaircutVector.GetValue(AdjStartDate));
     d->m_CashFlows.SetFlow(AdjStartDate, d->m_Size*qMin(1.0, StartingHaircut + HaircutVector.GetValue(AdjStartDate)), MtgCashFlow::MtgFlowType::LossFlow);
     d->m_CashFlows.SetFlow(AdjStartDate, CurrentAmtOut, MtgCashFlow::MtgFlowType::AmountOutstandingFlow);
     d->m_CashFlows.SetFlow(AdjStartDate, CurrentAmtOut*GetInterest(AdjStartDate), MtgCashFlow::MtgFlowType::WACouponFlow);
     QDate NextPaymentDate = AdjStartDate.addMonths(d->m_PaymentFreq.GetValue(AdjStartDate));
	 if (NextPaymentDate > AdjMaturityDate) NextPaymentDate = AdjMaturityDate;
     d->m_CashFlows.AddFlow(AdjStartDate, CurrentAmtOut*static_cast<double>(MonthDiff(AdjMaturityDate, AdjStartDate)) / 12.0, MtgCashFlow::MtgFlowType::WALlevel);
	 for (QDate CurrentMonth = AdjStartDate.addMonths(1); CurrentMonth <= AdjMaturityDate; CurrentMonth = CurrentMonth.addMonths(1)) {
		 
		 

		 //Accrue Interest for the month
		 CurrentInterest = GetInterest(CurrentMonth,CurrentMonth.addMonths(-1),CurrentMonth,CurrentDayCountConvention.GetValue(CurrentMonth));
         TempFlow1 = (CurrentInterest*(CurrentAmtOut + d->m_CashFlows.GetAccruedInterest(CurrentMonth))) + d->m_CashFlows.GetInterestRecoveries(CurrentMonth);
         d->m_CashFlows.AddFlow(CurrentMonth, TempFlow1, MtgCashFlow::MtgFlowType::AccruedInterestFlow);
		 //Add back the recoveries
         CurrentAmtOut += d->m_CashFlows.GetRecoveries(CurrentMonth);
		 if (CurrentMonth >= NextPaymentDate) { //It's a payment date
             TempFlow1 = d->m_CashFlows.GetAccruedInterest(CurrentMonth); // Total accrued Interest
             if (!(d->m_AnnuityVect.GetValue(CurrentMonth) & RepaymentVector::Capitalization) || CurrentMonth >= AdjMaturityDate) { //If repayment type is capitalization pay only at maturity
                 d->m_CashFlows.AddFlow(CurrentMonth, TempFlow1*(1.0 - Delinquency.GetValue(CurrentMonth)), MtgCashFlow::MtgFlowType::InterestFlow);// Non delinquent share of interest
                 d->m_CashFlows.AddFlow(CurrentMonth, -TempFlow1*(1.0 - Delinquency.GetValue(CurrentMonth)), MtgCashFlow::MtgFlowType::AccruedInterestFlow);
				 TempStep = DelinquencyLag.GetValue(CurrentMonth);
				 if (TempStep == 0) { //Delinquencies are not recovered
                     d->m_CashFlows.AddFlow(CurrentMonth, TempFlow1*Delinquency.GetValue(CurrentMonth), MtgCashFlow::MtgFlowType::LossOnInterestFlow);
                     d->m_CashFlows.AddFlow(CurrentMonth, -TempFlow1*Delinquency.GetValue(CurrentMonth), MtgCashFlow::MtgFlowType::AccruedInterestFlow);
				 }
				 else { //Accrue interest on delinquent part and pay it out
					 MtgCashFlow CurrentDelinq;
					 QDate RollingdateCounter;
					 CurrentDelinq.AddFlow(CurrentMonth, TempFlow1*Delinquency.GetValue(CurrentMonth), MtgCashFlow::MtgFlowType::AccruedInterestFlow);
                     d->m_CashFlows.AddFlow(CurrentMonth, -TempFlow1*Delinquency.GetValue(CurrentMonth), MtgCashFlow::MtgFlowType::AccruedInterestFlow);
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
                 (d->m_AnnuityVect.GetValue(CurrentMonth) & (RepaymentVector::InterestOnly | RepaymentVector::Capitalization))
			){
				 if (CurrentMonth == AdjMaturityDate) TempFlow2 = CurrentAmtOut;
				 else TempFlow2 = 0;
			 }
             else if (d->m_AnnuityVect.GetValue(CurrentMonth) & (RepaymentVector::Linear | RepaymentVector::Annuity)) {
				 //Count the payments left
				 int CountPeriods = 0;
				 QDate RollingdateCounter = CurrentMonth;
				 while (RollingdateCounter < AdjMaturityDate) {
					 CountPeriods++;
                     RollingdateCounter = RollingdateCounter.addMonths(d->m_PaymentFreq.GetValue(RollingdateCounter));
				 }CountPeriods++;
                 if (d->m_AnnuityVect.GetValue(CurrentMonth) & RepaymentVector::Linear) {
					 TempFlow2 = CurrentAmtOut / static_cast<double>(CountPeriods);
				 }
				 else { // RepaymentVector::Annuity
                     TempFlow2 = qAbs(pmt(GetInterest(CurrentMonth, CurrentMonth.addMonths(-d->m_PaymentFreq.GetValue(CurrentMonth)), CurrentMonth, CurrentDayCountConvention.GetValue(CurrentMonth)), CountPeriods, CurrentAmtOut)) - TempFlow1;
				 }
			 }
             d->m_CashFlows.AddFlow(CurrentMonth, TempFlow2, MtgCashFlow::MtgFlowType::PrincipalFlow);
			 CurrentAmtOut = CurrentAmtOut - TempFlow2;
             NextPaymentDate = NextPaymentDate.addMonths(d->m_PaymentFreq.GetValue(CurrentMonth));
			 if (NextPaymentDate > AdjMaturityDate) NextPaymentDate = AdjMaturityDate;
		 }
		 if (CurrentAmtOut > 0) {
			 //////////////////////////////////////////////////////////////////////////
			 // Haircut
			 if (HaircutVector.GetValue(CurrentMonth.addMonths(-1)) == 1.0) {
				 TempFlow1 = CurrentAmtOut * HaircutVector.GetValue(CurrentMonth);
                 TempFlow2 = d->m_CashFlows.GetAccruedInterest(CurrentMonth) * HaircutVector.GetValue(CurrentMonth);
			 }
			 else {
				 TempFlow1 = CurrentAmtOut - (CurrentAmtOut * (1.0 - HaircutVector.GetValue(CurrentMonth)) / (1.0 - HaircutVector.GetValue(CurrentMonth.addMonths(-1))));
                 TempFlow2 = d->m_CashFlows.GetAccruedInterest(CurrentMonth) *(1.0 - ((1.0 - HaircutVector.GetValue(CurrentMonth)) / (1.0 - HaircutVector.GetValue(CurrentMonth.addMonths(-1)))));
			 }
             d->m_CashFlows.AddFlow(CurrentMonth, qMin(TempFlow1, CurrentAmtOut), MtgCashFlow::MtgFlowType::LossFlow);
			 CurrentAmtOut = qMax(0.0, CurrentAmtOut - TempFlow1);
             d->m_CashFlows.AddFlow(CurrentMonth, qMin(TempFlow2, d->m_CashFlows.GetAccruedInterest(CurrentMonth)), MtgCashFlow::MtgFlowType::LossOnInterestFlow);
             d->m_CashFlows.AddFlow(CurrentMonth, -qMin(TempFlow2, d->m_CashFlows.GetAccruedInterest(CurrentMonth)), MtgCashFlow::MtgFlowType::AccruedInterestFlow);
			 //////////////////////////////////////////////////////////////////////////
			 //Prepayments
			 TempFlow1 = qMin(CurrentAmtOut, CurrentAmtOut * CPRVec.GetSMM(CurrentMonth, 1) * PrepayMultiplier.GetValue(CurrentMonth));
             d->m_CashFlows.AddFlow(CurrentMonth, TempFlow1, MtgCashFlow::MtgFlowType::PrepaymentFlow);
			 CurrentAmtOut -= TempFlow1;
			 TempFlow2 = TempFlow1*PrepaymentFee.GetValue(CurrentMonth);
             TempFlow1 = d->m_CashFlows.GetAccruedInterest(CurrentMonth) * qMin(1.0, CPRVec.GetSMM(CurrentMonth, 1) * PrepayMultiplier.GetValue(CurrentMonth));
             d->m_CashFlows.AddFlow(CurrentMonth, TempFlow1 + TempFlow2, MtgCashFlow::MtgFlowType::InterestFlow);
             d->m_CashFlows.AddFlow(CurrentMonth, TempFlow2, MtgCashFlow::MtgFlowType::PrepaymentFees);
             d->m_CashFlows.AddFlow(CurrentMonth, -TempFlow1, MtgCashFlow::MtgFlowType::AccruedInterestFlow);
			//////////////////////////////////////////////////////////////////////////
			//Defaults
			 TempFlow1 = qMin(CurrentAmtOut, CurrentAmtOut * CDRVec.GetSMM(CurrentMonth, 1));
             d->m_CashFlows.AddFlow(CurrentMonth, TempFlow1, MtgCashFlow::MtgFlowType::PrincipalDefault);
			 CurrentAmtOut -= TempFlow1;
			 //Assign losses and recoveries
             d->m_CashFlows.AddFlow(CurrentMonth.addMonths(RecoveryLag.GetValue(CurrentMonth)), qMin(TempFlow1, TempFlow1*LossVec.GetValue(CurrentMonth)*LossMultiplier.GetValue(CurrentMonth)), MtgCashFlow::MtgFlowType::LossFlow);
             d->m_CashFlows.AddFlow(CurrentMonth.addMonths(RecoveryLag.GetValue(CurrentMonth)), qMin(TempFlow1, TempFlow1*(1.0 - (LossVec.GetValue(CurrentMonth)*LossMultiplier.GetValue(CurrentMonth)))), MtgCashFlow::MtgFlowType::PrincipalRecovered);
			 //In case of no lag in recoveries add the principal back straight away
			 if (RecoveryLag.GetValue(CurrentMonth) == 0) CurrentAmtOut += qMin(TempFlow1, TempFlow1*(1.0 - (LossVec.GetValue(CurrentMonth)*LossMultiplier.GetValue(CurrentMonth))));
			 //Calculate defaulted interest
             TempFlow1 = d->m_CashFlows.GetAccruedInterest(CurrentMonth)* qMin(1.0, CDRVec.GetSMM(CurrentMonth, 1));
             d->m_CashFlows.AddFlow(CurrentMonth, -TempFlow1, MtgCashFlow::MtgFlowType::AccruedInterestFlow);
			 //Assign losses and recoveries on interest
             d->m_CashFlows.AddFlow(CurrentMonth.addMonths(RecoveryLag.GetValue(CurrentMonth)), qMin(TempFlow1, TempFlow1*(1.0 - (LossVec.GetValue(CurrentMonth)*LossMultiplier.GetValue(CurrentMonth)))), MtgCashFlow::MtgFlowType::InterestRecovered);
             d->m_CashFlows.AddFlow(CurrentMonth.addMonths(RecoveryLag.GetValue(CurrentMonth)), qMin(TempFlow1, TempFlow1*(LossVec.GetValue(CurrentMonth)*LossMultiplier.GetValue(CurrentMonth))), MtgCashFlow::MtgFlowType::LossOnInterestFlow);
			 if (RecoveryLag.GetValue(CurrentMonth) == 0) 
                 d->m_CashFlows.AddFlow(CurrentMonth, qMin(TempFlow1, TempFlow1*(1.0 - (LossVec.GetValue(CurrentMonth)*LossMultiplier.GetValue(CurrentMonth)))), MtgCashFlow::MtgFlowType::AccruedInterestFlow);
			 //////////////////////////////////////////////////////////////////////////

		 }

         d->m_CashFlows.SetFlow(CurrentMonth, CurrentAmtOut, MtgCashFlow::MtgFlowType::AmountOutstandingFlow);
         if (Delinquency.GetValue(AdjStartDate)>0.0) 
             d->m_CashFlows.SetFlow(CurrentMonth, CurrentAmtOut*Delinquency.GetValue(CurrentMonth), MtgCashFlow::MtgFlowType::DelinquentOutstanding);
         d->m_CashFlows.SetFlow(CurrentMonth, CurrentAmtOut*GetInterest(CurrentMonth), MtgCashFlow::MtgFlowType::WACouponFlow);
         d->m_CashFlows.SetFlow(CurrentMonth, CurrentAmtOut*PrepayMultiplier.GetValue(CurrentMonth), MtgCashFlow::MtgFlowType::WAPrepayMult);
         d->m_CashFlows.SetFlow(CurrentMonth, CurrentAmtOut*LossMultiplier.GetValue(CurrentMonth), MtgCashFlow::MtgFlowType::WALossMult);
         d->m_CashFlows.SetFlow(CurrentMonth, CurrentAmtOut*PrepaymentFee.GetValue(CurrentMonth), MtgCashFlow::MtgFlowType::WAPrepayFees);
         d->m_CashFlows.SetFlow(CurrentMonth, CurrentAmtOut*static_cast<double>(MonthDiff(AdjMaturityDate, CurrentMonth)) / 12.0, MtgCashFlow::MtgFlowType::WALlevel);
		 if (CurrentAmtOut < 0.01) break;
	 }
     for (QDate CurrentMonth = AdjMaturityDate.addMonths(1); CurrentMonth <= d->m_CashFlows.MaturityDate(); CurrentMonth = CurrentMonth.addMonths(1)) {
		 //Recoveries after maturity get in in cash
         if (d->m_CashFlows.GetInterestRecoveries(CurrentMonth)>0.0)
             d->m_CashFlows.AddFlow(CurrentMonth, d->m_CashFlows.GetInterestRecoveries(CurrentMonth), MtgCashFlow::MtgFlowType::InterestFlow);
         if (d->m_CashFlows.GetRecoveries(CurrentMonth)>0.0) 
             d->m_CashFlows.AddFlow(CurrentMonth, d->m_CashFlows.GetRecoveries(CurrentMonth), MtgCashFlow::MtgFlowType::PrincipalFlow);
	 }
     d->m_CashFlows.AddFlow(DelinquenciesFlows);

	 {
		 double LoanPrice;
		 if (HasProperty("Price")) LoanPrice=GetProperty("Price").toDouble()/100.0;
		 else LoanPrice = 1.0;
         for (int WAPriceIter = 0; WAPriceIter < d->m_CashFlows.Count(); ++WAPriceIter) {
             d->m_CashFlows.SetFlow(
                 d->m_CashFlows.GetDate(WAPriceIter)
                 , LoanPrice*d->m_CashFlows.GetFlow(WAPriceIter, MtgCashFlow::MtgFlowType::AmountOutstandingFlow)
				 , MtgCashFlow::MtgFlowType::WAPrice
			);
		 }
	 }

     if (NullAnchorDates[0]) d->m_InterestVect.RemoveAnchorDate();
     if (NullAnchorDates[1]) d->m_AnnuityVect.RemoveAnchorDate();
     if (NullAnchorDates[2]) d->m_PaymentFreq.RemoveAnchorDate();
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


 void Mortgage::CompileReferenceRateValue(ConstantBaseRateTable& Values)
 {
     Q_D(Mortgage);
     d->m_FloatingRateBaseValue = d->m_FloatRateBase.CompileReferenceRateValue(Values);
     d->m_UseForwardCurve = false;
 }

 void Mortgage::CompileReferenceRateValue(ForwardBaseRateTable& Values)
 {
     Q_D(Mortgage);
     d->m_FloatingRateBaseValue = d->m_FloatRateBase.CompileReferenceRateValue(Values);
     d->m_UseForwardCurve = true;
 }

 bool Mortgage::GetUseForwardCurve() const
 {
     Q_D(const Mortgage);
     return d->m_UseForwardCurve;
 }

 const BloombergVector& Mortgage::GetFloatingRateValue() const
 {
     Q_D(const Mortgage);
     return d->m_FloatingRateBaseValue;
 }

 const BaseRateVector& Mortgage::GetFloatingRateBase() const
 {
     Q_D(const Mortgage);
     return d->m_FloatRateBase;
 }

 void Mortgage::SetPaymentFreq(const QString& a)
 {
     Q_D(Mortgage);
     d->m_PaymentFreq = a;
 }

 const IntegerVector& Mortgage::GetPaymentFreq() const
 {
     Q_D(const Mortgage);
     return d->m_PaymentFreq;
 }
 QDataStream& operator<<(QDataStream & stream, const Mortgage& flows)
 {
     stream
         << flows.d_func()->m_MaturityDate
         << flows.d_func()->m_AnnuityVect
         << flows.d_func()->m_Size
         << flows.d_func()->m_InterestVect
         << flows.d_func()->m_CashFlows
         << flows.d_func()->m_FloatRateBase
         << flows.d_func()->m_FloatingRateBaseValue
         << flows.d_func()->m_PaymentFreq
         << flows.d_func()->m_UseForwardCurve
         << flows.d_func()->m_Properties
         ;
     return stream;
 }
 QDataStream& Mortgage::LoadOldVersion(QDataStream& stream) {
     Q_D(Mortgage);
	 stream >> d->m_MaturityDate;
     d->m_AnnuityVect.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->m_AnnuityVect;
     stream >> d->m_Size;
     d->m_InterestVect.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->m_InterestVect;
     d->m_CashFlows.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->m_CashFlows;
     d->m_FloatRateBase.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->m_FloatRateBase;
     d->m_FloatingRateBaseValue.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->m_FloatingRateBaseValue;
     d->m_PaymentFreq.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->m_PaymentFreq;
     stream >> d->m_UseForwardCurve >> d->m_Properties;
	 ResetProtocolVersion();
	 return stream;
 }
 QDataStream& operator>>(QDataStream & stream, Mortgage& flows) {
	 return flows.LoadOldVersion(stream);
 }

 void Mortgage::AddCashFlow(const MtgCashFlow& a)
 {
     Q_D(Mortgage);
     d->m_CashFlows.AddFlow(a);
 }

 void Mortgage::AddCashFlow(const QDate& Dte, double Amt, MtgCashFlow::MtgFlowType FlowTpe)
 {
     Q_D(Mortgage);
     d->m_CashFlows.AddFlow(Dte, Amt, FlowTpe);
 }

 QString Mortgage::ReadyToCalculate()const
 {
     Q_D(const Mortgage);
     QString Result;
     if (d->m_MaturityDate < QDate(2000, 1, 1)) Result += "Loan Maturity Date\n";
     if (d->m_AnnuityVect.IsEmpty())Result += "Loan Annuity Vector\n";
     if (d->m_Size < 0.0)Result += "Loan Size\n";
     if (HasProperty("LossMultiplier")) { if (BloombergVector(GetProperty("LossMultiplier")).IsEmpty(0.0)) Result += "Loss Multiplier\n"; }
     if (HasProperty("PrepayMultiplier")) { if (BloombergVector(GetProperty("PrepayMultiplier")).IsEmpty(0.0)) Result += "Prepay Multiplier\n"; }
     if (d->m_InterestVect.IsEmpty())Result += "Loan Coupon\n";
     if (HasProperty("Haircut")) { if (BloombergVector(GetProperty("Haircut")).IsEmpty(0.0, 1.0))Result += "Haircut Vector\n"; }
     if (d->m_PaymentFreq.IsEmpty(1))Result += "Loan Payment Frequency\n";
     if (d->m_FloatRateBase.IsEmpty())Result += "Loan Base Rate\n";
     if (HasProperty("DayCount")) { if (DayCountVector(GetProperty("DayCount")).IsEmpty()) Result += "Loan Day Count Convention\n"; }
     if (HasProperty("CPR")) { if (BloombergVector(GetProperty("CPR")).IsEmpty(0.0, 1.0)) Result += "Loan CPR Assumption\n"; }
     if (HasProperty("CDR")) { if (BloombergVector(GetProperty("CDR")).IsEmpty(0.0, 1.0)) Result += "Loan CDR Assumption\n"; }
     if (HasProperty("LS")) { if (BloombergVector(GetProperty("LS")).IsEmpty(0.0, 1.0)) Result += "Loan LS Assumption\n"; }
     if (HasProperty("RecoveryLag")) { if (IntegerVector(GetProperty("RecoveryLag")).IsEmpty(0)) Result += "Loan Recovery Lag Assumption\n"; }
     if (HasProperty("Delinquency")) { if (BloombergVector(GetProperty("Delinquency")).IsEmpty(0.0, 1.0)) Result += "Loan Delinquency Assumption\n"; }
     if (HasProperty("DelinquencyLag")) { if (IntegerVector(GetProperty("DelinquencyLag")).IsEmpty(0)) Result += "Loan Delinquency Lag Assumption\n"; }
     if (HasProperty("Price")) { if (BloombergVector(GetProperty("Price")).IsEmpty(0.0)) Result += "Loan Price\n"; }
     if (HasProperty("MaturityExtension")) { if (GetProperty("MaturityExtension").toInt() < 0) Result += "Loan Maturity Extension\n"; }
     if (HasProperty("StartingHaircut")) { double TempHC = GetProperty("StartingHaircut").toDouble(); if (TempHC< 0.0 || TempHC>1.0) Result += "Loan Starting Haircut\n"; }
     if (HasProperty("PrepaymentFee")) { if (BloombergVector(GetProperty("PrepaymentFee")).IsEmpty(0.0)) Result += "Loan Prepayment Fee\n"; }
     if (!Result.isEmpty()) return Result.left(Result.size() - 1);
     return Result;
 }

 void Mortgage::ResetFlows()
 {
     Q_D(Mortgage);
     d->m_CashFlows.Clear();
 }

 double Mortgage::GetInterest(const QDate& a, const QDate& StartAccrue, const QDate& EndAccrue, DayCountConvention DayCnt)
 {
     Q_D(Mortgage);
     if (d->m_FloatingRateBaseValue.IsEmpty()) {
#ifdef NO_BLOOMBERG
			 m_FloatingRateBaseValue="0";
#else
         d->m_FloatingRateBaseValue = d->m_FloatRateBase.GetRefRateValueFromBloomberg(ConstantBaseRateTable());
         d->m_UseForwardCurve = false;
#endif
		 }
     return  AdjustCoupon((d->m_InterestVect + d->m_FloatingRateBaseValue).GetValue(a), StartAccrue, EndAccrue, DayCnt);
 }
 double Mortgage::GetInterest(int a, const QDate& StartAccrue, const QDate& EndAccrue, DayCountConvention DayCnt) {
     Q_D(Mortgage);
     if (d->m_FloatingRateBaseValue.IsEmpty()) {
#ifdef NO_BLOOMBERG
         d->m_FloatingRateBaseValue = "0";
#else
         d->m_FloatingRateBaseValue = d->m_FloatRateBase.GetRefRateValueFromBloomberg(ConstantBaseRateTable());
         d->m_UseForwardCurve = false;
#endif
		 }
     return  AdjustCoupon((d->m_InterestVect + d->m_FloatingRateBaseValue).GetValue(a), StartAccrue, EndAccrue, DayCnt);
 }
 double Mortgage::GetInterest(const QDate& a) {
     Q_D(Mortgage);
     if (d->m_FloatingRateBaseValue.IsEmpty()) {
#ifdef NO_BLOOMBERG
			 m_FloatingRateBaseValue = "0";
#else
         d->m_FloatingRateBaseValue = d->m_FloatRateBase.GetRefRateValueFromBloomberg(ConstantBaseRateTable());
             d->m_UseForwardCurve = false;
#endif
		 }
     return  (d->m_InterestVect + d->m_FloatingRateBaseValue).GetValue(a);
 }
 double Mortgage::GetInterest(int a) {
     Q_D(Mortgage);
     if (d->m_FloatingRateBaseValue.IsEmpty()) {
#ifdef NO_BLOOMBERG
         d->m_FloatingRateBaseValue = "0";
#else
         d->m_FloatingRateBaseValue = d->m_FloatRateBase.GetRefRateValueFromBloomberg(ConstantBaseRateTable());
             d->m_UseForwardCurve = false;
#endif
		 }
     return  (d->m_InterestVect + d->m_FloatingRateBaseValue).GetValue(a);
 }

 QString Mortgage::GetInterest() const
 {
     Q_D(const Mortgage);
     return d->m_InterestVect.GetVector();
 }

 void Mortgage::SetProperty(const QString& PropName, const QString& Value)
 {
     Q_D(Mortgage);
	 if (PropName.isEmpty() || Value.isEmpty() || PropName.contains(QRegExp("#[<>=]#")) || Value.contains(QRegExp("#[<>=]#"))) return;
	 if (HasProperty(PropName)) {
		 QRegExp TempReg("#<#" + PropName.trimmed() + "#=#(.+)#>#", Qt::CaseInsensitive);
		 TempReg.setMinimal(true);
         d->m_Properties.replace(TempReg, "#<#" + PropName.trimmed() + "#=#" + Value.trimmed() + "#>#");
	 }
	 else {
         d->m_Properties += "#<#" + PropName.trimmed() + "#=#" + Value.trimmed() + "#>#";
		 //LOGDEBUG("Property Set: "+m_Properties);
	 }
 }

QString Mortgage::GetProperty(const QString& PropName) const {
    Q_D(const Mortgage);
	QRegExp TempReg("#<#" + PropName.trimmed() + "#=#(.+)#>#", Qt::CaseInsensitive);
	TempReg.setMinimal(true);
    if (TempReg.indexIn(d->m_Properties) != -1) {
		return TempReg.cap(1);
	}
	return QString();
}

QString Mortgage::GetProperty(qint32 PropIndex, bool PropValue) const {
    Q_D(const Mortgage);
	if (PropIndex < 0) return QString();
	qint32 pos=0;
	QRegExp TempReg("#<#(.+)#=#(.+)#>#", Qt::CaseInsensitive);
	TempReg.setMinimal(true);
	for (qint32 i = 0; i <= PropIndex; ++i) {
        if ((pos = TempReg.indexIn(d->m_Properties, pos)) == -1) return QString();
	}
	return TempReg.cap(PropValue ? 2:1);
}

QString Mortgage::GetPropertyValue(qint32 PropIndex) const
{
    return GetProperty(PropIndex, true);
}

QString Mortgage::GetPropertyName(qint32 PropIndex) const
{
    return GetProperty(PropIndex, false);
}

bool Mortgage::HasProperty(const QString& PropName) const
{
    Q_D(const Mortgage);
	QRegExp TempReg('#<#' + PropName.trimmed() + "#=#(.+)#>#", Qt::CaseInsensitive);
	TempReg.setMinimal(true);
	return  TempReg.indexIn(d->m_Properties) != -1;
}

qint32 Mortgage::GetNumProperties() const {
    Q_D(const Mortgage);
	return d->m_Properties.count("#=#");
}

void Mortgage::RemoveProperty(const QString& PropName) {
    Q_D( Mortgage);
	if (HasProperty(PropName)) {
		QRegExp TempReg("#<#" + PropName.trimmed() + "#=#.+#>#", Qt::CaseInsensitive);
		TempReg.setMinimal(true);
		d->m_Properties.replace(TempReg, "");
	}
}
void Mortgage::RemoveProperty(qint32 PropIndex) {
    Q_D(Mortgage);
	if (PropIndex < 0) return;
	qint32 pos = 0;
	QRegExp TempReg("#<#(.+)#=#.+#>#", Qt::CaseInsensitive);
	TempReg.setMinimal(true);
	for (qint32 i = 0; i <= PropIndex; ++i) {
		if ((pos=TempReg.indexIn(d->m_Properties, pos)) == -1) return;
	}
	return RemoveProperty(TempReg.cap(1));
}

void Mortgage::FillDiscountOutstanding() {
    Q_D(Mortgage);
	if (!(HasProperty("PurchasePrice") && HasProperty("DiscountLimit"))) return;
	double PurchPrice = GetProperty("PurchasePrice").toDouble();
	double DiscountLimit = GetProperty("DiscountLimit").toDouble();
	if (PurchPrice > 1) PurchPrice /= 100.0;
	if (DiscountLimit > 1) DiscountLimit /= 100.0;
	if (PurchPrice >= DiscountLimit) return;
    for (int i = 0; i < d->m_CashFlows.Count(); ++i) {
        d->m_CashFlows.AddFlow(d->m_CashFlows.GetDate(i), d->m_CashFlows.GetAmountOut(i)*PurchPrice, static_cast<qint32>(MtgCashFlow::MtgFlowType::OutstandingForOC));
	}
}

void Mortgage::GetBaseRatesDatabase(ConstantBaseRateTable& Values, bool DownloadAll /*= false*/)
{
    Q_D(Mortgage);
    d->m_FloatingRateBaseValue = d->m_FloatRateBase.GetBaseRatesDatabase(Values, DownloadAll); 
    d->m_UseForwardCurve = false;
}

void Mortgage::GetBaseRatesDatabase(ForwardBaseRateTable& Values, bool DownloadAll /*= false*/)
{
    Q_D(Mortgage);
    d->m_FloatingRateBaseValue = d->m_FloatRateBase.GetBaseRatesDatabase(Values, DownloadAll); 
    d->m_UseForwardCurve = true;
}

void Mortgage::SetCashFlows(const MtgCashFlow& a)
{
    Q_D(Mortgage);
    d->m_CashFlows = a;
}

void Mortgage::SetScenario(const LoanAssumption& value, bool OverrideCurrent)
{
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

void Mortgage::SetFloatingRateBase(const QString& a)
{
    Q_D(Mortgage);
    d->m_FloatRateBase = BaseRateVector(a);
}

void Mortgage::setFixedRate()
{
    Q_D(Mortgage);
    d->m_FloatingRateBaseValue = "0"; 
    d->m_FloatRateBase = "ZERO";
}

const QDate& Mortgage::GetMaturityDate() const
{
    Q_D(const Mortgage);
    return d->m_MaturityDate;
}

void Mortgage::SetMaturityDate(const QDate& a)
{
    Q_D(Mortgage);
    d->m_MaturityDate = a;
}

const MtgCashFlow& Mortgage::GetCashFlow() const
{
    Q_D(const Mortgage);
    return d->m_CashFlows;
}

MtgCashFlow& Mortgage::GetCashFlow()
{
    Q_D(Mortgage);
    return d->m_CashFlows;
}

QString Mortgage::GetAnnuity() const
{
    Q_D(const Mortgage);
    return d->m_AnnuityVect.GetVector();
}
