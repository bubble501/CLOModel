#include "CentralUnit.h"
#include "ExcelCommons.h"
#include "ExcelOutput.h"
#include <CommonFunctions.h>
#include <CumulativeLossTrigger.h>
#include <DateTrigger.h>
#include <DeferredInterestTrigger.h>
#include <DelinquencyTrigger.h>
#include <DuringStressTestTrigger.h>
#include <LoanAssumptionsEditor.h>
#include <Mortgage.h>
#include <PDLtrigger.h>
#include <PoolSizeTrigger.h>
#include <QApplication>
#include <QDate>
#include <QFile>
#include <QHBoxLayout>
#include <QIcon>
#include <QString>
#include <QTextStream>
#include <SummaryView.h>
#include <Tranche.h>
#include <TrancheTrigger.h>
#include <TriggerHelperDialog.h>
#include <VectorTrigger.h>
#include <WatFalPrior.h>
#include <WaterfallStepHelperDialog.h>
void __stdcall RunModel(LPSAFEARRAY *ArrayData){
	bool RunStress;
	CentralUnit TempUnit;
	VARIANT HUGEP *pdFreq;
	HRESULT hr = SafeArrayAccessData(*ArrayData, (void HUGEP* FAR*)&pdFreq);
	if (!SUCCEEDED(hr))return;
    TempUnit.SaveInputs(pdFreq->boolVal); pdFreq++;
	int NumElements;
	{ //Loans
		QDate Matur;
		double sze;
		QString Intr, frq, lossm, prem, Ann, Hairc, BaseRte;
		QString Properties;
		NumElements=pdFreq->intVal;pdFreq++;
		for(int i=0;i<NumElements;i++){
			Matur=QDate::fromString(QString::fromWCharArray(pdFreq->bstrVal),"yyyy-MM-dd");pdFreq++;
			sze=pdFreq->dblVal;pdFreq++;
			Intr=QString::fromWCharArray(pdFreq->bstrVal);pdFreq++;
			BaseRte = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
			Ann=QString::fromWCharArray(pdFreq->bstrVal);pdFreq++;
			frq = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
			Properties = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
			if (sze >= 0.01) TempUnit.AddLoan(Matur, sze, Intr, Ann, frq, BaseRte, Properties);
		}
	}
	{//Tranches
		QString TrName, Curr, BasRt, TrancheISIN, IPDfrq, ProRat;
        QList<QString> RefRt, coup, DayCnt;
		int TempSize;
		double origOut,currOut,OClim,IClim,Price,Exchan,startingDeferred;
		QDate PrevIPD,SettDate;
		NumElements=pdFreq++->intVal;
		LOGDEBUG(QString("Numero Tranches: %1").arg(NumElements));
		for(int i=0;i<NumElements;i++){
			RefRt.clear(); coup.clear();
			TrName=QString::fromWCharArray(pdFreq->bstrVal);pdFreq++;
			TrancheISIN=QString::fromWCharArray(pdFreq->bstrVal);pdFreq++;
			ProRat = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
			origOut=pdFreq->dblVal;pdFreq++;
			Curr=QString::fromWCharArray(pdFreq->bstrVal);pdFreq++;
			currOut=pdFreq->dblVal;pdFreq++;
			TempSize = pdFreq->intVal; pdFreq++;
            LOGDEBUG(QString("Reached Coupons %1").arg(TempSize));
			for (int i = 0; i < TempSize; ++i)
				{coup.append(QString::fromWCharArray(pdFreq->bstrVal)); pdFreq++;}
            LOGDEBUG("Finished Coupons");
			TempSize = pdFreq->intVal; pdFreq++;
            LOGDEBUG(QString("Reached Ref Rate %1").arg(TempSize));
			for (int i = 0; i < TempSize; ++i)
				{RefRt.append(QString::fromWCharArray(pdFreq->bstrVal)); pdFreq++;}
            LOGDEBUG("Reached IPD");
			PrevIPD=QDate::fromString(QString::fromWCharArray(pdFreq->bstrVal),"yyyy-MM-dd");pdFreq++;
			BasRt=QString::fromWCharArray(pdFreq->bstrVal);pdFreq++;
			IPDfrq=QString::fromWCharArray(pdFreq->bstrVal);pdFreq++;
            LOGDEBUG("Reached Tests");
			OClim=pdFreq->dblVal;pdFreq++;
			IClim=pdFreq->dblVal;pdFreq++;
			Price=pdFreq->dblVal;pdFreq++;
			Exchan=pdFreq->dblVal;pdFreq++;
			SettDate=QDate::fromString(QString::fromWCharArray(pdFreq->bstrVal),"yyyy-MM-dd");pdFreq++;
            TempSize = pdFreq->intVal; pdFreq++;
            for (int i = 0; i < TempSize; ++i) {
                DayCnt.append(QString::fromWCharArray(pdFreq->bstrVal)); pdFreq++;
            }
			startingDeferred = pdFreq->dblVal; pdFreq++;

			TempUnit.AddTranche(TrName, TrancheISIN, ProRat, origOut, Curr, currOut, coup, RefRt, PrevIPD, BasRt, IPDfrq, SettDate, startingDeferred, OClim, IClim, Price, Exchan, "Mtge", DayCnt);
            LOGDEBUG("Tranche Added");
        }
	}
	
	{ //Waterfall Steps
		int Prior,ArgAccruePay;
		QString ArgTrigger, ArgRedemptionShare, ArgAdditionalCollateralShare, ArgTestTargetOverride, ArgIRRtoEquityTarget, ArgSeniorityGroup, ArgSeniorityGroupLevel, ArgRedemptionGroup, ArgRedemptionGroupLevel, ArgSourceofFunding, ArgCouponIndex, ArgReserveIndex;
		NumElements = pdFreq->intVal; pdFreq++;
		LOGDEBUG(QString("Numero Steps: %1").arg(NumElements));
		for(int i=0;i<NumElements;i++){
			Prior=pdFreq->intVal; pdFreq++;
			ArgSeniorityGroup = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
			ArgSeniorityGroupLevel = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
			ArgRedemptionGroup = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
			ArgRedemptionGroupLevel = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
			ArgRedemptionShare = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
			ArgAdditionalCollateralShare = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
			ArgSourceofFunding = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
			ArgCouponIndex = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
			ArgTestTargetOverride = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
			ArgIRRtoEquityTarget = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
			ArgReserveIndex = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
			ArgTrigger = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
			ArgAccruePay = pdFreq->intVal; pdFreq++;
			LOGDEBUG(QString("Seniority Group: %1\nSeniority Group Level: %2\nRedemption Group: %3\nRedemption Group Level: %4\nRedemption Share: %5\nAdd. Collateral Share %6\nSource of Funding: %7\nCoupon Index: %8\nTest Target Ovr: %9\nIIR To Equity: %10\nReserve Index: %11\nTriggers: %12\nAccrue-Pay: %13\n")
				.arg(ArgSeniorityGroup)
				.arg(ArgSeniorityGroupLevel)
				.arg(ArgRedemptionGroup)
				.arg(ArgRedemptionGroupLevel)
				.arg(ArgRedemptionShare)
				.arg(ArgAdditionalCollateralShare)
				.arg(ArgSourceofFunding)
				.arg(ArgCouponIndex)
				.arg(ArgTestTargetOverride)
				.arg(ArgIRRtoEquityTarget)
				.arg(ArgReserveIndex)
				.arg(ArgTrigger)
				.arg(ArgAccruePay)
			);
			TempUnit.AddWaterfallStep(static_cast<WatFalPrior::WaterfallStepType>(Prior), ArgSeniorityGroup, ArgSeniorityGroupLevel, ArgRedemptionGroup, ArgRedemptionGroupLevel, ArgRedemptionShare, ArgAdditionalCollateralShare, ArgSourceofFunding, ArgCouponIndex, ArgTestTargetOverride, ArgIRRtoEquityTarget, ArgReserveIndex, ArgTrigger, ArgAccruePay);
		}
	}
	{ //Triggers
		int TriggerCount,TriggerTpe;
		TriggerCount = pdFreq->intVal; pdFreq++;
		std::shared_ptr<AbstractTrigger> TempTrigger;
		for (int i = 0; i < TriggerCount; i++) {
			TriggerTpe = pdFreq->intVal; pdFreq++;
			switch (TriggerTpe) {
                // Keep this in sync with the relevant override of AbstractTriggerSettingWidget::parameters()
            case static_cast<int>(AbstractTrigger::TriggerType::DateTrigger) :
                TempTrigger.reset(new DateTrigger(QString::fromWCharArray(pdFreq->bstrVal))); pdFreq++;
                std::dynamic_pointer_cast<DateTrigger>(TempTrigger)->SetLimitDate(QDate::fromString(QString::fromWCharArray(pdFreq->bstrVal), "yyyy-MM-dd")); pdFreq++;
                std::dynamic_pointer_cast<DateTrigger>(TempTrigger)->SetSide(static_cast<DateTrigger::TriggerSide>(pdFreq->intVal)); pdFreq++;
                TempUnit.SetTrigger(i + 1, TempTrigger);
                break;
            case static_cast<int>(AbstractTrigger::TriggerType::VectorTrigger) :
                TempTrigger.reset(new VectorTrigger(QString::fromWCharArray(pdFreq->bstrVal))); pdFreq++;
                std::dynamic_pointer_cast<VectorTrigger>(TempTrigger)->SetTrigVector(QString::fromWCharArray(pdFreq->bstrVal)); pdFreq++;
                TempUnit.SetTrigger(i + 1, TempTrigger);
                break;
            case static_cast<int>(AbstractTrigger::TriggerType::PoolSizeTrigger) :
                TempTrigger.reset(new PoolSizeTrigger(QString::fromWCharArray(pdFreq->bstrVal))); pdFreq++;
                std::dynamic_pointer_cast<PoolSizeTrigger>(TempTrigger)->SetTargetSize(QString::fromWCharArray(pdFreq->bstrVal)); pdFreq++;
                std::dynamic_pointer_cast<PoolSizeTrigger>(TempTrigger)->SetSide(static_cast<PoolSizeTrigger::TriggerSide>(pdFreq->intVal)); pdFreq++;
                TempUnit.SetTrigger(i + 1, TempTrigger);
                break;
            case static_cast<int>(AbstractTrigger::TriggerType::CumulativeLossTrigger) :
                TempTrigger.reset(new CumulativeLossTrigger(QString::fromWCharArray(pdFreq->bstrVal))); pdFreq++;
                std::dynamic_pointer_cast<CumulativeLossTrigger>(TempTrigger)->SetTargetSize(QString::fromWCharArray(pdFreq->bstrVal)); pdFreq++;
                std::dynamic_pointer_cast<CumulativeLossTrigger>(TempTrigger)->SetSide(static_cast<CumulativeLossTrigger::TriggerSide>(pdFreq->intVal)); pdFreq++;
                TempUnit.SetTrigger(i + 1, TempTrigger);
                break;
            case static_cast<int>(AbstractTrigger::TriggerType::TrancheTrigger) :
                TempTrigger.reset(new TrancheTrigger(QString::fromWCharArray(pdFreq->bstrVal))); pdFreq++;
                std::dynamic_pointer_cast<TrancheTrigger>(TempTrigger)->SetTargetSeniority(QString::fromWCharArray(pdFreq->bstrVal)); pdFreq++;
                std::dynamic_pointer_cast<TrancheTrigger>(TempTrigger)->SetTargetSeniorityLevel(QString::fromWCharArray(pdFreq->bstrVal)); pdFreq++;
                std::dynamic_pointer_cast<TrancheTrigger>(TempTrigger)->SetTargetSize(QString::fromWCharArray(pdFreq->bstrVal)); pdFreq++;
                std::dynamic_pointer_cast<TrancheTrigger>(TempTrigger)->SetSenioritySide(static_cast<TrancheTrigger::TriggerSenioritySide>(pdFreq->intVal)); pdFreq++;
                std::dynamic_pointer_cast<TrancheTrigger>(TempTrigger)->SetSizeSide(static_cast<TrancheTrigger::TriggerSizeSide>(pdFreq->intVal)); pdFreq++;
                std::dynamic_pointer_cast<TrancheTrigger>(TempTrigger)->SetSizeMultiplier(pdFreq->dblVal); pdFreq++;
                TempUnit.SetTrigger(i + 1, TempTrigger);
                break;
            case static_cast<int>(AbstractTrigger::TriggerType::DeferredInterestTrigger) :
                TempTrigger.reset(new DeferredInterestTrigger(QString::fromWCharArray(pdFreq->bstrVal))); pdFreq++;
                std::dynamic_pointer_cast<DeferredInterestTrigger>(TempTrigger)->SetTargetSeniority(QString::fromWCharArray(pdFreq->bstrVal)); pdFreq++;
                std::dynamic_pointer_cast<DeferredInterestTrigger>(TempTrigger)->SetTargetSeniorityLevel(QString::fromWCharArray(pdFreq->bstrVal)); pdFreq++;
                std::dynamic_pointer_cast<DeferredInterestTrigger>(TempTrigger)->SetTargetSize(QString::fromWCharArray(pdFreq->bstrVal)); pdFreq++;
                std::dynamic_pointer_cast<DeferredInterestTrigger>(TempTrigger)->SetSenioritySide(static_cast<DeferredInterestTrigger::TriggerSenioritySide>(pdFreq->intVal)); pdFreq++;
                std::dynamic_pointer_cast<DeferredInterestTrigger>(TempTrigger)->SetSizeSide(static_cast<DeferredInterestTrigger::TriggerSizeSide>(pdFreq->intVal)); pdFreq++;
                std::dynamic_pointer_cast<DeferredInterestTrigger>(TempTrigger)->SetSizeMultiplier(pdFreq->dblVal); pdFreq++;
                std::dynamic_pointer_cast<DeferredInterestTrigger>(TempTrigger)->SetTargetCoupon(QString::fromWCharArray(pdFreq->bstrVal)); pdFreq++;
                std::dynamic_pointer_cast<DeferredInterestTrigger>(TempTrigger)->SetCouponSide(static_cast<DeferredInterestTrigger::TriggerCouponSide>(pdFreq->intVal)); pdFreq++;
                TempUnit.SetTrigger(i + 1, TempTrigger);
                break;
            case static_cast<int>(AbstractTrigger::TriggerType::DelinquencyTrigger) :
                TempTrigger.reset(new DelinquencyTrigger(QString::fromWCharArray(pdFreq->bstrVal))); pdFreq++;
                std::dynamic_pointer_cast<DelinquencyTrigger>(TempTrigger)->SetTarget(QString::fromWCharArray(pdFreq->bstrVal)); pdFreq++;
                TempUnit.SetTrigger(i + 1, TempTrigger);
                break;
            case static_cast<int>(AbstractTrigger::TriggerType::DuringStressTestTrigger) :
                TempTrigger.reset(new DuringStressTestTrigger(QString::fromWCharArray(pdFreq->bstrVal))); pdFreq++;
                TempUnit.SetTrigger(i + 1, TempTrigger);
                break;
            case static_cast<int>(AbstractTrigger::TriggerType::PDLTrigger) :
                TempTrigger.reset(new PDLTrigger(QString::fromWCharArray(pdFreq->bstrVal))); pdFreq++;
                std::dynamic_pointer_cast<PDLTrigger>(TempTrigger)->SetTargetSeniority(QString::fromWCharArray(pdFreq->bstrVal)); pdFreq++;
                std::dynamic_pointer_cast<PDLTrigger>(TempTrigger)->SetTargetSeniorityLevel(QString::fromWCharArray(pdFreq->bstrVal)); pdFreq++;
                std::dynamic_pointer_cast<PDLTrigger>(TempTrigger)->SetTargetSize(QString::fromWCharArray(pdFreq->bstrVal)); pdFreq++;
                std::dynamic_pointer_cast<PDLTrigger>(TempTrigger)->SetSenioritySide(static_cast<PDLTrigger::TriggerSenioritySide>(pdFreq->intVal)); pdFreq++;
                std::dynamic_pointer_cast<PDLTrigger>(TempTrigger)->SetSizeSide(static_cast<PDLTrigger::TriggerSizeSide>(pdFreq->intVal)); pdFreq++;
                std::dynamic_pointer_cast<PDLTrigger>(TempTrigger)->SetSizeMultiplier(pdFreq->dblVal); pdFreq++;
                TempUnit.SetTrigger(i + 1, TempTrigger);
                break;
            default:
                PrintToTempFile("Error", "Unhanded Trigger Type in Input");
			}
		}
	}
	{ //General Inputs
		LOGDEBUG(QString("General Inputs"));
        const QStringList dealNames = QString::fromWCharArray(pdFreq->bstrVal).split(";", QString::SkipEmptyParts); pdFreq++;
        for (auto singleDN = dealNames.constBegin(); singleDN != dealNames.constEnd();++singleDN)
            TempUnit.AddDealName(*singleDN);
		TempUnit.SetStartingDeferredJunFees(pdFreq->dblVal); pdFreq++;
		TempUnit.SetGICinterest(QString::fromWCharArray(pdFreq->bstrVal)); pdFreq++;
		TempUnit.SetGICBaseRate(QString::fromWCharArray(pdFreq->bstrVal)); pdFreq++;
		TempUnit.SetDealDayCountConvention(QString::fromWCharArray(pdFreq->bstrVal)); pdFreq++;
		TempUnit.SetSeniorExpenses(QString::fromWCharArray(pdFreq->bstrVal)); pdFreq++;
		TempUnit.SetSeniorFees(QString::fromWCharArray(pdFreq->bstrVal)); pdFreq++;
		TempUnit.SetJuniorFees(QString::fromWCharArray(pdFreq->bstrVal)); pdFreq++;
		TempUnit.SetSeniorExpensesFixed(QString::fromWCharArray(pdFreq->bstrVal)); pdFreq++;
		TempUnit.SetSeniorFeesFixed(QString::fromWCharArray(pdFreq->bstrVal)); pdFreq++;
		TempUnit.SetJuniorFeesFixed(QString::fromWCharArray(pdFreq->bstrVal)); pdFreq++;
		TempUnit.SetJuniorFeesCoupon(pdFreq->dblVal);pdFreq++;
		TempUnit.SetPaymentFrequency(QString::fromWCharArray(pdFreq->bstrVal));pdFreq++;
		TempUnit.SetFirstIPDdate(QDate::fromString(QString::fromWCharArray(pdFreq->bstrVal),"yyyy-MM-dd"));pdFreq++;
		TempUnit.SetLastIPDdate(QDate::fromString(QString::fromWCharArray(pdFreq->bstrVal),"yyyy-MM-dd"));pdFreq++;
		TempUnit.SetCCCcurve(QString::fromWCharArray(pdFreq->bstrVal));pdFreq++;
		TempUnit.SetCCChaircut(pdFreq->dblVal);pdFreq++;
		TempUnit.SetCCCTestLimit(pdFreq->dblVal);pdFreq++;
		TempUnit.SetCallDate(QDate::fromString(QString::fromWCharArray(pdFreq->bstrVal),"yyyy-MM-dd"));pdFreq++;
		TempUnit.SetCallMultiple(pdFreq->dblVal);pdFreq++;
		TempUnit.SetCallReserve(pdFreq->dblVal);pdFreq++;
		TempUnit.SetPoolValueAtCall(QString::fromWCharArray(pdFreq->bstrVal)); pdFreq++;
		TempUnit.SetLegalFinal(QDate::fromString(QString::fromWCharArray(pdFreq->bstrVal), "yyyy-MM-dd")); pdFreq++;
		{//Reserve Fund
			QString RFtarget,RFmultiple,RFfloor,RFcap;
			double RFcurrent;
			int RFfree,NumReserves;
			bool RFtoIntres;
			NumReserves = pdFreq->intVal; pdFreq++;
			for(int RFiter=0;RFiter<NumReserves;RFiter++){
				RFtarget=QString::fromWCharArray(pdFreq->bstrVal);pdFreq++;
				RFmultiple=QString::fromWCharArray(pdFreq->bstrVal);pdFreq++;
				RFfloor=QString::fromWCharArray(pdFreq->bstrVal);pdFreq++;
				RFcap = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
				RFcurrent=pdFreq->dblVal; pdFreq++;
				RFfree=pdFreq->intVal; pdFreq++;
				RFtoIntres=pdFreq->boolVal; pdFreq++;
				if (
					(!BloombergVector(RFtarget).IsEmpty(0.001) && !BloombergVector(RFmultiple).IsEmpty(0.001))
					|| !BloombergVector(RFfloor).IsEmpty(0.001)
					|| RFcurrent>0.0
				)
				TempUnit.AddReserveFund(RFtarget, RFmultiple, RFfloor, RFcap, RFcurrent, RFfree, RFtoIntres);
			}
			bool CumRes=pdFreq->boolVal; pdFreq++;
			TempUnit.SetCumulativeReserves(CumRes);
		}
		{// Base Case Infos
			bool TempBool = pdFreq->boolVal; pdFreq++;
			TempUnit.SetBaseCaseToCall(TempBool);
			TempBool = pdFreq->boolVal; pdFreq++;
			TempUnit.SetSaveBaseCase(TempBool);
		}
		{// Reinvestment Test
			QDate ReinPer;
			ReinPer=QDate::fromString(QString::fromWCharArray(pdFreq->bstrVal),"yyyy-MM-dd");pdFreq++;
			TempUnit.SetReinvestementPeriod(ReinPer);
			QString Intr, CPR, CDR, LS, WAL, Delay, Timespread, Pric, Frq, BaseVal, ReinvBondAnnuit,RecLag,Dlq,DlqLag;
			Intr=QString::fromWCharArray(pdFreq->bstrVal);pdFreq++;
			CPR=QString::fromWCharArray(pdFreq->bstrVal);pdFreq++;
			CDR=QString::fromWCharArray(pdFreq->bstrVal);pdFreq++;
			LS=QString::fromWCharArray(pdFreq->bstrVal);pdFreq++;
			WAL=QString::fromWCharArray(pdFreq->bstrVal);pdFreq++;
			Delay = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
			Timespread = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
			Pric = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
			Frq = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
			BaseVal = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
			ReinvBondAnnuit = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
			RecLag = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
			Dlq = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
			DlqLag = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
			LOGDEBUG(
				QString("Interest: %1\nCPR: %2\nCDR: %3\nLS: %4\nWAL: %5\nFrequency: %6\nAnnuity: %7\nPrice: %8\nDelay: %9\nBase Rate: %10\nRecovery Lag: %11\nDelinquency: %12\nDelinquency Lag: %13\nReinvestement Spread Over Time: %14")
				.arg(Intr)
				.arg(CPR)
				.arg(CDR)
				.arg(LS)
				.arg(WAL)
				.arg(Frq)
				.arg(ReinvBondAnnuit)
				.arg(Pric)
				.arg(Delay)
				.arg(BaseVal)
				.arg(RecLag)
				.arg(Dlq)
				.arg(DlqLag)
				.arg(Timespread)
			);
			TempUnit.SetupReinvBond(Intr, CPR, CDR, LS, WAL, Frq, ReinvBondAnnuit, Pric, Delay, Timespread, BaseVal, RecLag, Dlq, DlqLag);
            LOGDEBUG("reinvestment bond set up");
		}
        LOGDEBUG(QString("Available Princ %1").arg(pdFreq->dblVal));
		TempUnit.SetSchedPrincAvailable(pdFreq->dblVal);pdFreq++; //TODO starting Prepay
        LOGDEBUG("Available principal set up");
		TempUnit.SetInterestAvailable(pdFreq->dblVal);pdFreq++;
        LOGDEBUG("Available interest set up");
		TempUnit.SetPoolCutOff(QDate::fromString(QString::fromWCharArray(pdFreq->bstrVal),"yyyy-MM-dd"));pdFreq++;
		TempUnit.SetRunCall(pdFreq->boolVal);pdFreq++;
		LOGDEBUG(QString("Use Call: %1").arg(TempUnit.GetRunCall()));
		TempUnit.SetMtgOutputAddress(QString::fromWCharArray(pdFreq->bstrVal));pdFreq++;
		TempUnit.SetTranchesOutputAddress(QString::fromWCharArray(pdFreq->bstrVal));pdFreq++;
		TempUnit.SetPlotsSheet(QString::fromWCharArray(pdFreq->bstrVal));pdFreq++;
		TempUnit.SetPlotIndexes(pdFreq->intVal,0);pdFreq++;
		TempUnit.SetPlotIndexes(pdFreq->intVal,1);pdFreq++;
		TempUnit.SetPlotIndexes(pdFreq->intVal,2);pdFreq++;
		TempUnit.SetPlotIndexes(pdFreq->intVal,3);pdFreq++;
		TempUnit.SetPlotIndexes(pdFreq->intVal,4);pdFreq++;
		TempUnit.SetPlotIndexes(pdFreq->intVal,5);pdFreq++;
		TempUnit.SetPlotIndexes(pdFreq->intVal,6);pdFreq++;
		TempUnit.SetPlotIndexes(pdFreq->intVal,7);pdFreq++;
		TempUnit.SetPlotIndexes(pdFreq->intVal,8);pdFreq++;
		TempUnit.SetLossOutputAddress(QString::fromWCharArray(pdFreq->bstrVal));pdFreq++;
		TempUnit.SetLossOnCallOutputAddress(QString::fromWCharArray(pdFreq->bstrVal));pdFreq++;
		TempUnit.SetCreditEnanAddress(QString::fromWCharArray(pdFreq->bstrVal));pdFreq++;
		TempUnit.SetFolderPath(QString::fromWCharArray(pdFreq->bstrVal));pdFreq++;
		{//Base Rates Compilation
			bool UseForwardRates = pdFreq->boolVal; pdFreq++;
			LOGDEBUG(QString("use Forward: %1").arg(UseForwardRates));
			int HowManyFwdCurves = pdFreq->intVal; pdFreq++;
			LOGDEBUG(QString("Numero Curve Forward: %1").arg(HowManyFwdCurves));
			if (UseForwardRates) {
				ForwardBaseRateTable CompilationTable;
				QList<QDate> TempRefDates;
				QList<double> TempRefVals;
				QString TempKey;
				int HowManyDates;
				for (int FwdCrvIte = 0; FwdCrvIte < HowManyFwdCurves; ++FwdCrvIte) {
					TempRefDates.clear();
					TempRefVals.clear();
					TempKey = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
					LOGDEBUG(TempKey);
					HowManyDates = pdFreq->intVal; pdFreq++;
					LOGDEBUG(QString("Numero Date Forward: %1").arg(HowManyDates));
					for (int FwdDtsIte = 0; FwdDtsIte < HowManyDates; ++FwdDtsIte) {
						TempRefDates.append(QDate::fromString(QString::fromWCharArray(pdFreq->bstrVal), "yyyy-MM-dd")); pdFreq++;
						TempRefVals.append(pdFreq->dblVal / 100.0); pdFreq++;
					}
					CompilationTable.SetValue(TempKey, TempRefDates, TempRefVals);
				}
#ifndef NO_DATABASE
				TempUnit.GetBaseRatesDatabase(CompilationTable);
#else
#ifndef NO_BLOOMBERG
				TempUnit.CompileBaseRates(CompilationTable);
#else
				int NumOfBaseRates = pdFreq->intVal; pdFreq++;
				QString BaseName;
				double BaseVal;
				for (int i = 0; i < NumOfBaseRates; i++) {
					BaseName = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
					BaseVal = pdFreq->dblVal; pdFreq++;
					CompilationTable.insert(BaseName.trimmed().toUpper(), BaseVal);
				}
				TempUnit.CompileBaseRates(CompilationTable);
#endif // NO_BLOOMBERG
#endif // NO_DATABASE
			}
			else {
				ConstantBaseRateTable CompilationTable;
				QString TempKey; double TempValue;
				for (int FwdCrvIte = 0; FwdCrvIte < HowManyFwdCurves; ++FwdCrvIte) {
					TempKey = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
					TempValue = pdFreq->dblVal; pdFreq++;
					CompilationTable.SetValue(TempKey, TempValue / 100.0);
				}
#ifndef NO_DATABASE
				TempUnit.GetBaseRatesDatabase(CompilationTable);
#else
#ifndef NO_BLOOMBERG
				TempUnit.CompileBaseRates(CompilationTable);
#else
				int NumOfBaseRates = pdFreq->intVal; pdFreq++;
				QString BaseName;
				double BaseVal;
				for (int i = 0; i < NumOfBaseRates; i++) {
					BaseName = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
					BaseVal = pdFreq->dblVal; pdFreq++;
					CompilationTable.insert(BaseName.trimmed().toUpper(), BaseVal);
				}
				TempUnit.CompileBaseRates(CompilationTable);
#endif // NO_BLOOMBERG
#endif // NO_DATABASE
			}
		}
		RunStress=pdFreq->boolVal;pdFreq++;
		if(RunStress){// Stress Test
			QString ConstPar=QString::fromWCharArray(pdFreq->bstrVal);pdFreq++;
			LOGDEBUG(QString("Stress ConstPar: %1").arg(ConstPar));
			int XVar=pdFreq->intVal;pdFreq++;
			LOGDEBUG(QString("Stress XVar: %1").arg(XVar));
			int YVar=pdFreq->intVal;pdFreq++;
			LOGDEBUG(QString("Stress YVar: %1").arg(YVar));
			int NumElem=pdFreq->intVal;pdFreq++;
			QList<QString> XSpann;
			for(int i=0;i<NumElem;i++){
				XSpann.append(QString::fromWCharArray(pdFreq->bstrVal));pdFreq++;
			}
			NumElem=pdFreq->intVal;pdFreq++;
			QList<QString> YSpann;
			for(int i=0;i<NumElem;i++){
				YSpann.append(QString::fromWCharArray(pdFreq->bstrVal));pdFreq++;
			}
			TempUnit.SetStressToCall(pdFreq->boolVal);pdFreq++;
			TempUnit.SetFastStress(pdFreq->boolVal); pdFreq++;
			TempUnit.SetupStress(ConstPar,XSpann,YSpann,XVar,YVar);
			
		}
	}
	SafeArrayUnaccessData(*ArrayData);
    if (TempUnit.SaveInputs()) {
        QString Filename = "C:\\Temp\\.SavedInputs.clo";
        QFile file(Filename);
        if (file.open(QIODevice::WriteOnly)) {
            QDataStream out(&file);
            out.setVersion(StreamVersionUsed);
            out << qint32(ModelVersionNumber) << TempUnit.GetBaseCaseToCall() << TempUnit.GetStructure() << Waterfall();
            file.close();
        }
    }


	if(RunStress) TempUnit.CalculateStress();
	else TempUnit.Calculate();

}

double __stdcall CLOReturnRate(LPSAFEARRAY *ArrayData){
	VARIANT HUGEP *pdFreq;
	HRESULT hr = SafeArrayAccessData(*ArrayData, (void HUGEP* FAR*)&pdFreq);
	if (!SUCCEEDED(hr)) return 0.0;
	QString FolderPath=QString::fromStdWString(pdFreq->bstrVal);pdFreq++;
	QString DealName = QString::fromStdWString(pdFreq->bstrVal); pdFreq++;
	QString TrancheName=QString::fromStdWString(pdFreq->bstrVal);pdFreq++;
	bool ToCall=pdFreq->boolVal;pdFreq++;
	double NewPrice=pdFreq->dblVal;pdFreq++;
	SafeArrayUnaccessData(*ArrayData);
	Waterfall TempWaterfall;
	QString Filename=FolderPath+"/BaseCase.clo";
	QFile file(Filename);
	bool UsingClom = false;
	if (!file.exists()) {
        const auto dealNames = DealName.split(';', QString::SkipEmptyParts);
        for (auto i = dealNames.constBegin(); i != dealNames.constEnd(); ++i) {
            file.setFileName(GetFromConfig("Folders", "UnifiedResultsFolder") + '/' + *i + ".clom");
            if (file.exists()){
                UsingClom = true;
                break;
            } 
        }
        if (!UsingClom)
		    return 0.0;
	}
	if (!file.open(QIODevice::ReadOnly)){
		return 0.0;
	}
	qint32 VersionChecker;
	QDataStream out(&file);
	out.setVersion(StreamVersionUsed);
	out >> VersionChecker;
	//if(VersionChecker!=qint32(ModelVersionNumber)) return 0.0;
	if(VersionChecker<qint32(MinimumSupportedVersion) || VersionChecker>qint32(ModelVersionNumber)) return 0.0;
	if (UsingClom) {
		{QDate Junk; out >> Junk; }
		{bool Junk; out >> Junk; }
	}
	{bool Junk; out >> Junk; }
	TempWaterfall.SetLoadProtocolVersion(VersionChecker);
	out >> TempWaterfall;
	if(ToCall){
		TempWaterfall.SetLoadProtocolVersion(VersionChecker);
		out >> TempWaterfall;
	}
	file.close();
    if (TempWaterfall.GetTranchesCount() == 0)
        return 0.0;
    int argc =  1;
    QApplication ComputationLoop(argc, NULL);
	const auto TranchPoint=TempWaterfall.GetTranche(TrancheName);
    ComputationLoop.quit();
	if(Q_LIKELY(TranchPoint))
        return TranchPoint->GetIRR(NewPrice);
	return 0.0;
}
double __stdcall CLODiscountMargin(LPSAFEARRAY *ArrayData){
	VARIANT HUGEP *pdFreq;
	HRESULT hr = SafeArrayAccessData(*ArrayData, (void HUGEP* FAR*)&pdFreq);
	if (!SUCCEEDED(hr)) return 0.0;
	QString FolderPath=QString::fromStdWString(pdFreq->bstrVal);pdFreq++;
	QString DealName = QString::fromStdWString(pdFreq->bstrVal); pdFreq++;
	QString TrancheName=QString::fromStdWString(pdFreq->bstrVal);pdFreq++;
	bool ToCall=pdFreq->boolVal;pdFreq++;
	double NewPrice=pdFreq->dblVal;pdFreq++;
	SafeArrayUnaccessData(*ArrayData);
	Waterfall TempWaterfall;
	QString Filename=FolderPath+"/BaseCase.clo";
	QFile file(Filename);
	bool UsingClom = false;
	if(!file.exists()){
        const auto dealNames = DealName.split(';', QString::SkipEmptyParts);
        for (auto i = dealNames.constBegin(); i != dealNames.constEnd(); ++i) {
            file.setFileName(GetFromConfig("Folders", "UnifiedResultsFolder") + '/' + *i + ".clom");
            if (file.exists()) {
                UsingClom = true;
                break;
            }
        }
        if (!UsingClom)
            return 0.0;
	}
	if (!file.open(QIODevice::ReadOnly)){
		return 0.0;
	}
	qint32 VersionChecker;
	QDataStream out(&file);
	out.setVersion(StreamVersionUsed);
	out >> VersionChecker;
	if(VersionChecker<qint32(MinimumSupportedVersion) || VersionChecker>qint32(ModelVersionNumber)) return 0.0;
	if (UsingClom) {
		{QDate Junk; out >> Junk; }
		{bool Junk; out >> Junk; }
	}
	{bool Junk; out >> Junk; }
	TempWaterfall.SetLoadProtocolVersion(VersionChecker);
	out >> TempWaterfall;
	if(ToCall){
		TempWaterfall.SetLoadProtocolVersion(VersionChecker);
		out >> TempWaterfall;
	}
    file.close();
    if (TempWaterfall.GetTranchesCount() == 0)
        return 0.0;
    int argc =  1;
    QApplication ComputationLoop(argc, NULL);
	const auto TranchPoint=TempWaterfall.GetTranche(TrancheName);
    ComputationLoop.quit();
	if(Q_LIKELY(TranchPoint))
		return TranchPoint->GetDiscountMargin(NewPrice);
	return 0.0;
}
double __stdcall CLOWALife(LPSAFEARRAY *ArrayData){
	qint32 VersionChecker;
	VARIANT HUGEP *pdFreq;
	HRESULT hr = SafeArrayAccessData(*ArrayData, (void HUGEP* FAR*)&pdFreq);
	if (!SUCCEEDED(hr)) return 0.0;
	QString FolderPath=QString::fromStdWString(pdFreq->bstrVal);pdFreq++;
	QString DealName = QString::fromStdWString(pdFreq->bstrVal); pdFreq++;
	QString TrancheName=QString::fromStdWString(pdFreq->bstrVal);pdFreq++;
	QDate StartDate=QDate::fromString(QString::fromStdWString(pdFreq->bstrVal),"yyyy-MM-dd");pdFreq++;
	bool ToCall=pdFreq->boolVal;pdFreq++;
	SafeArrayUnaccessData(*ArrayData);
	Waterfall TempWaterfall;
	QString Filename=FolderPath+"\\BaseCase.clo";
	QFile file(Filename);
	bool UsingClom = false;
	if (!file.exists()){
        const auto dealNames = DealName.split(';', QString::SkipEmptyParts);
        for (auto i = dealNames.constBegin(); i != dealNames.constEnd(); ++i) {
            file.setFileName(GetFromConfig("Folders", "UnifiedResultsFolder") + '/' + *i + ".clom");
            if (file.exists()) {
                UsingClom = true;
                break;
            }
        }
        if (!UsingClom)
            return 0.0;
	}
	if (!file.open(QIODevice::ReadOnly))return 0.0;
	QDataStream out(&file);
	out.setVersion(StreamVersionUsed);
	out >> VersionChecker;
	if(VersionChecker<qint32(MinimumSupportedVersion) || VersionChecker>qint32(ModelVersionNumber)) return 0.0;
	if (UsingClom) {
		{QDate Junk; out >> Junk; }
		{bool Junk; out >> Junk; }
	}
	{bool Junk; out >> Junk; }
	TempWaterfall.SetLoadProtocolVersion(VersionChecker);
	out >> TempWaterfall;
	if(ToCall){
		TempWaterfall.SetLoadProtocolVersion(VersionChecker);
		out >> TempWaterfall;
	}
	file.close();
    if (TempWaterfall.GetTranchesCount() == 0)
        return 0.0;

    int argc =  1;
    QApplication ComputationLoop(argc, NULL);
	const auto TranchPoint=TempWaterfall.GetTranche(TrancheName);
    ComputationLoop.quit();
	if(Q_LIKELY(TranchPoint)) 
        return TranchPoint->GetWALife(/*StartDate*/);
    return 0.0;
}
double __stdcall GetStressLoss(LPSAFEARRAY *ArrayData) {
	QString FolderPath;
	QString CPRscenario;
	QString CDRscenario;
	QString LSscenario;
	QString RecLagScenario;
	QString DelinqScenario;
	QString DelinqLagScenario;
	QString TrancheName;
	VARIANT HUGEP *pdFreq;
	HRESULT hr = SafeArrayAccessData(*ArrayData, (void HUGEP* FAR*)&pdFreq);
	if (!SUCCEEDED(hr)) return -1.0;
	FolderPath = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
	TrancheName = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
	CPRscenario = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
	CDRscenario = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
	LSscenario = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
	RecLagScenario = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
	DelinqScenario = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
	DelinqLagScenario = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
	SafeArrayUnaccessData(*ArrayData);
	if (*(FolderPath.end() - 1) != '\\' && *(FolderPath.end() - 1) != '/') FolderPath += '/';
	FolderPath += "StressResult.fcsr";
	if (!QFile::exists(FolderPath)) { LOGDEBUG("Returned -1"); return -1.0; }
	Waterfall TempRes = StressTest::GetScenarioFromFile(FolderPath, CPRscenario, CDRscenario, LSscenario, RecLagScenario, DelinqScenario, DelinqLagScenario);
    int argc =1;
    QApplication ComputationLoop(argc, NULL);
	const auto Result = TempRes.GetTranche(TrancheName);
    ComputationLoop.quit();
	if (Result)
		return Result->GetLossRate();
	return -1.0;
}
double __stdcall GetStressDM(LPSAFEARRAY *ArrayData) {
	QString FolderPath;
	QString CPRscenario;
	QString CDRscenario;
	QString LSscenario;
	QString RecLagScenario;
	QString DelinqScenario;
	QString DelinqLagScenario;
	QString TrancheName;
	double NewPrice;
	VARIANT HUGEP *pdFreq;
	HRESULT hr = SafeArrayAccessData(*ArrayData, (void HUGEP* FAR*)&pdFreq);
	if (!SUCCEEDED(hr)) return 0.0;
	FolderPath = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
	TrancheName = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
	CPRscenario = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
	CDRscenario = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
	LSscenario = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
	RecLagScenario = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
	DelinqScenario = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
	DelinqLagScenario = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
	NewPrice = pdFreq->dblVal; pdFreq++;
	SafeArrayUnaccessData(*ArrayData);
	if (*(FolderPath.end() - 1) != '\\' && *(FolderPath.end() - 1) != '/') FolderPath += '/';
	FolderPath += "StressResult.fcsr";
	if (!QFile::exists(FolderPath)) { return 0.0; }
	Waterfall TempRes = StressTest::GetScenarioFromFile(FolderPath, CPRscenario, CDRscenario, LSscenario, RecLagScenario, DelinqScenario, DelinqLagScenario);
    int argc =  1;
    QApplication ComputationLoop(argc, NULL);
    const auto Result = TempRes.GetTranche(TrancheName);
    ComputationLoop.quit();
	if (Result)
		return Result->GetDiscountMargin(NewPrice);
	return 0.0;
}
BSTR __stdcall WatFallStepEdit(LPSAFEARRAY *ArrayData) {
	QHash<quint32, std::shared_ptr<AbstractTrigger> >AvailableTriggers;
	VARIANT HUGEP *pdFreq;
	HRESULT hr = SafeArrayAccessData(*ArrayData, (void HUGEP* FAR*)&pdFreq);
	if (!SUCCEEDED(hr))return NULL;
	//QString DestinationSheet = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
	//QString DestinationAddress = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
	bool IsInterestWF = pdFreq->boolVal; pdFreq++;
	QString CurrentStep = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
    LOGDEBUG("Step Parameters: " + CurrentStep);
	{ //Triggers
		int TriggerTpe;
		const int TriggerCount = pdFreq->intVal; pdFreq++;
        LOGDEBUG(QString("Num Triggers: %1").arg(TriggerCount));
        QHash<quint32, std::shared_ptr<AbstractTrigger> >::iterator TempIter;
		for (int i = 0; i < TriggerCount; i++) {
			TriggerTpe = pdFreq->intVal; pdFreq++;
            LOGDEBUG(QString("Loading Trigger Type: %1").arg(TriggerTpe));
			switch (TriggerTpe) {
			case static_cast<int>(AbstractTrigger::TriggerType::DateTrigger) :
				TempIter = AvailableTriggers.insert(i, std::make_shared< DateTrigger>(QString::fromWCharArray(pdFreq->bstrVal))); pdFreq++;
                std::static_pointer_cast<DateTrigger>( *TempIter)->SetLimitDate(QDate::fromString(QString::fromWCharArray(pdFreq->bstrVal), "yyyy-MM-dd")); pdFreq++;
                std::static_pointer_cast < DateTrigger>(*TempIter)->SetSide(static_cast<DateTrigger::TriggerSide>(pdFreq->intVal)); pdFreq++;
				break;
			case static_cast<int>(AbstractTrigger::TriggerType::VectorTrigger) :
                TempIter = AvailableTriggers.insert(i, std::make_shared< VectorTrigger>(QString::fromWCharArray(pdFreq->bstrVal))); pdFreq++;
                std::static_pointer_cast<VectorTrigger>(*TempIter)->SetTrigVector(QString::fromWCharArray(pdFreq->bstrVal)); pdFreq++;
				break;
			case static_cast<int>(AbstractTrigger::TriggerType::PoolSizeTrigger) :
                TempIter = AvailableTriggers.insert(i, std::make_shared< PoolSizeTrigger>(QString::fromWCharArray(pdFreq->bstrVal))); pdFreq++;
                std::static_pointer_cast<PoolSizeTrigger>(*TempIter)->SetTargetSize(QString::fromWCharArray(pdFreq->bstrVal)); pdFreq++;
                std::static_pointer_cast<PoolSizeTrigger>(*TempIter)->SetSide(static_cast<PoolSizeTrigger::TriggerSide>(pdFreq->intVal)); pdFreq++;
				break;
			case static_cast<int>(AbstractTrigger::TriggerType::CumulativeLossTrigger) :
                TempIter = AvailableTriggers.insert(i, std::make_shared< CumulativeLossTrigger>(QString::fromWCharArray(pdFreq->bstrVal))); pdFreq++;
                std::static_pointer_cast<CumulativeLossTrigger>(*TempIter)->SetTargetSize(QString::fromWCharArray(pdFreq->bstrVal)); pdFreq++;
                std::static_pointer_cast<CumulativeLossTrigger>(*TempIter)->SetSide(static_cast<CumulativeLossTrigger::TriggerSide>(pdFreq->intVal)); pdFreq++;
				break;
			case static_cast<int>(AbstractTrigger::TriggerType::TrancheTrigger) :
                TempIter = AvailableTriggers.insert(i, std::make_shared< TrancheTrigger>(QString::fromWCharArray(pdFreq->bstrVal))); pdFreq++;
                std::static_pointer_cast<TrancheTrigger>(*TempIter)->SetTargetSeniority(QString::fromWCharArray(pdFreq->bstrVal)); pdFreq++;
                std::static_pointer_cast<TrancheTrigger>(*TempIter)->SetTargetSeniorityLevel(QString::fromWCharArray(pdFreq->bstrVal)); pdFreq++;
                std::static_pointer_cast<TrancheTrigger>(*TempIter)->SetTargetSize(QString::fromWCharArray(pdFreq->bstrVal)); pdFreq++;
                std::static_pointer_cast<TrancheTrigger>(*TempIter)->SetSenioritySide(static_cast<TrancheTrigger::TriggerSenioritySide>(pdFreq->intVal)); pdFreq++;
                std::static_pointer_cast<TrancheTrigger>(*TempIter)->SetSizeSide(static_cast<TrancheTrigger::TriggerSizeSide>(pdFreq->intVal)); pdFreq++;
                std::static_pointer_cast<TrancheTrigger>(*TempIter)->SetSizeMultiplier(pdFreq->dblVal); pdFreq++;
				break;
			case static_cast<int>(AbstractTrigger::TriggerType::DeferredInterestTrigger) :
                TempIter = AvailableTriggers.insert(i, std::make_shared< DeferredInterestTrigger>(QString::fromWCharArray(pdFreq->bstrVal))); pdFreq++;
                std::static_pointer_cast<DeferredInterestTrigger>(*TempIter)->SetTargetSeniority(QString::fromWCharArray(pdFreq->bstrVal)); pdFreq++;
                std::static_pointer_cast<DeferredInterestTrigger>(*TempIter)->SetTargetSeniorityLevel(QString::fromWCharArray(pdFreq->bstrVal)); pdFreq++;
                std::static_pointer_cast<DeferredInterestTrigger>(*TempIter)->SetTargetSize(QString::fromWCharArray(pdFreq->bstrVal)); pdFreq++;
                std::static_pointer_cast<DeferredInterestTrigger>(*TempIter)->SetSenioritySide(static_cast<DeferredInterestTrigger::TriggerSenioritySide>(pdFreq->intVal)); pdFreq++;
                std::static_pointer_cast<DeferredInterestTrigger>(*TempIter)->SetSizeSide(static_cast<DeferredInterestTrigger::TriggerSizeSide>(pdFreq->intVal)); pdFreq++;
                std::static_pointer_cast<DeferredInterestTrigger>(*TempIter)->SetSizeMultiplier(pdFreq->dblVal); pdFreq++;
                std::static_pointer_cast<DeferredInterestTrigger>(*TempIter)->SetTargetCoupon(QString::fromWCharArray(pdFreq->bstrVal)); pdFreq++;
                std::static_pointer_cast<DeferredInterestTrigger>(*TempIter)->SetCouponSide(static_cast<DeferredInterestTrigger::TriggerCouponSide>(pdFreq->intVal)); pdFreq++;
				break;
			case static_cast<int>(AbstractTrigger::TriggerType::DelinquencyTrigger) :
                TempIter = AvailableTriggers.insert(i, std::make_shared< DelinquencyTrigger>(QString::fromWCharArray(pdFreq->bstrVal))); pdFreq++;
                std::static_pointer_cast<DelinquencyTrigger>(*TempIter)->SetTarget(QString::fromWCharArray(pdFreq->bstrVal)); pdFreq++;
				break;
			case static_cast<int>(AbstractTrigger::TriggerType::DuringStressTestTrigger) :
                AvailableTriggers.insert(i, std::make_shared< DuringStressTestTrigger>(QString::fromWCharArray(pdFreq->bstrVal))); pdFreq++;
				break;
            case static_cast<int>(AbstractTrigger::TriggerType::PDLTrigger) :
                TempIter = AvailableTriggers.insert(i, std::make_shared< PDLTrigger>(QString::fromWCharArray(pdFreq->bstrVal))); pdFreq++;
                std::static_pointer_cast<PDLTrigger>(*TempIter)->SetTargetSeniority(QString::fromWCharArray(pdFreq->bstrVal)); pdFreq++;
                std::static_pointer_cast<PDLTrigger>(*TempIter)->SetTargetSeniorityLevel(QString::fromWCharArray(pdFreq->bstrVal)); pdFreq++;
                std::static_pointer_cast<PDLTrigger>(*TempIter)->SetTargetSize(QString::fromWCharArray(pdFreq->bstrVal)); pdFreq++;
                std::static_pointer_cast<PDLTrigger>(*TempIter)->SetSenioritySide(static_cast<PDLTrigger::TriggerSenioritySide>(pdFreq->intVal)); pdFreq++;
                std::static_pointer_cast<PDLTrigger>(*TempIter)->SetSizeSide(static_cast<PDLTrigger::TriggerSizeSide>(pdFreq->intVal)); pdFreq++;
                std::static_pointer_cast<PDLTrigger>(*TempIter)->SetSizeMultiplier(pdFreq->dblVal); pdFreq++;
                break;
			default:
                Q_UNREACHABLE(); // "Unhandled trigger type"
				return NULL;
			}
            LOGDEBUG("Trigger Loaded");
		}
	}
	SafeArrayUnaccessData(*ArrayData);
	int argc = 1;
	{
		QApplication a(argc, NULL);
		WaterfallStepHelperDialog WatfDialog;
		WatfDialog.SetInterestWF(IsInterestWF);
        LOGDEBUG("InterestWF loaded");
		WatfDialog.SetAvailableTriggers(AvailableTriggers);
        LOGDEBUG("Available triggers loaded");
		WatfDialog.SetCurrentPars(CurrentStep);
        LOGDEBUG("Pars Loaded");
		if (WatfDialog.exec() == QDialog::Accepted) {
			QString Result = WatfDialog.GetParameters();
			a.quit();
			return SysAllocStringByteLen(Result.toStdString().c_str(), Result.size());
		}
		a.quit();
	}
	return NULL; 
}
BSTR __stdcall TriggerEdit(LPSAFEARRAY *ArrayData) {
	QHash<quint32, QSharedPointer<AbstractTrigger> >AvailableTriggers;
	VARIANT HUGEP *pdFreq;
	HRESULT hr = SafeArrayAccessData(*ArrayData, (void HUGEP* FAR*)&pdFreq);
	if (!SUCCEEDED(hr))return NULL;
	QString CurrentTrig = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
    LOGDEBUG(CurrentTrig);
	SafeArrayUnaccessData(*ArrayData);
    int argc = 1;
	{
		QApplication a(argc, NULL);
		TriggerHelperDialog TrigDialog;
		TrigDialog.SetCurrentPars(CurrentTrig);
        const auto DialogRes = TrigDialog.exec();
        if (DialogRes == QDialog::Accepted) {
			const QString Result = TrigDialog.GetParameters();
			a.quit();
			return SysAllocStringByteLen(Result.toStdString().c_str(), Result.size());
		}
        else if (DialogRes == QDialog::Rejected)
		    a.quit();
        else{ //Clear
            a.quit();
            return SysAllocStringByteLen("#", 1);
        }
	}
	return NULL;
}
//SAFEARRAY* __stdcall LoadLoanScenario(LPSAFEARRAY *ArrayData) {
BSTR __stdcall LoadLoanScenario(LPSAFEARRAY *ArrayData) {
	VARIANT HUGEP *pdFreq;
	HRESULT hr = SafeArrayAccessData(*ArrayData, (void HUGEP* FAR*)&pdFreq);
	if (!SUCCEEDED(hr))return NULL;
	const int NumOfLoans = pdFreq->intVal; pdFreq++;
	Mortgage TempMtg;
	int argc = 1;
	QApplication a(argc, 0);
    QDialog TempDialog;
	TempDialog.setWindowIcon(QIcon(":/Icons/Logo.png"));
	TempDialog.setWindowTitle(QObject::tr("Loan Scenarios Editor"));
    TempDialog.setModal(true);
	QHBoxLayout* DialogLay = new QHBoxLayout(&TempDialog);
	LoanAssumptionsEditor* ScenariosEditor = new LoanAssumptionsEditor(&TempDialog);
	QObject::connect(ScenariosEditor, &LoanAssumptionsEditor::PoolSaved, &TempDialog, &QDialog::accept);
	DialogLay->addWidget(ScenariosEditor);
	ScenariosEditor->SetEnableLoad(false);
	ScenariosEditor->FillFromQuery();
	
	QString CurrField;
	for (int i = 0; i < NumOfLoans; ++i) {
		CurrField = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
		if (CurrField.isEmpty()) TempMtg.RemoveProperty("Issuer");
		else TempMtg.SetProperty("Issuer", CurrField);
		CurrField = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
		if (CurrField.isEmpty()) TempMtg.RemoveProperty("Facility");
		else TempMtg.SetProperty("Facility", CurrField);
		CurrField = QString::fromWCharArray(pdFreq->bstrVal); pdFreq++;
		if (CurrField.isEmpty()) TempMtg.RemoveProperty("Scenario");
		else TempMtg.SetProperty("Scenario", CurrField);
		ScenariosEditor->AddLoanToPool(ScenariosEditor->LoanCount(), TempMtg);
	}
	SafeArrayUnaccessData(*ArrayData);
    if (TempDialog.exec() == QDialog::Accepted) {
		auto Result = ScenariosEditor->GetScenarios();
		auto ResultKeys = Result.keys();
		std::sort(ResultKeys.begin(), ResultKeys.end());
		QString TotalString;
		for (auto i = ResultKeys.constBegin(); i != ResultKeys.constEnd(); ++i) {
			if (i != ResultKeys.constBegin()) TotalString.append("#,#");
			TotalString.append(Result.value(*i));
		}
        a.quit();
		return SysAllocStringByteLen(TotalString.toStdString().c_str(), TotalString.size());
	}
    a.quit();
	return NULL;
}


/*!
\file ExcelInput.cpp
\brief Excel exported functions
\author Luca Beldi
\date November 2013

\details File containing the definition of the function exported into the dll and available through Excel
*/


/*! \fn void __stdcall RunModel(LPSAFEARRAY *ArrayData)
\brief Function that runs the cash flow model, and the stress test saves and prints the results in excel
\param ArrayData Excel Variant Array of Inputs
\details This function takes in input all the parameters of the model, runs the cash flows and saves the results. It then calls, when appropriate:
- ExcelOutput::PrintMortgagesRepLines
- ExcelOutput::PlotMortgagesFlows
- ExcelOutput::PrintTrancheFlow
- ExcelOutput::PlotTranchesDynamic
- ExcelOutput::PlotOCTest
- ExcelOutput::PlotICTest
- ExcelOutput::PlotAnnualExcess
- ExcelOutput::PlotCostFunding

To send the output to excel.

ArrayData must contain, in order:
<ol><li>A Long representing the number of loans in the pool</li>
<li>For each loan in the pool:</li>
	<ol type="a"><li>A String containing the maturity date of the loan in the ISO 8601 format (yyyy-mm-dd)</li>
	<li>A Double representing the current amount outstanding of the loan</li>
	<li>A String containing a Bloomberg-like vector that represents the interest rate payed on the loan</li>
	<li>A String containing a Bloomberg-like vector that represent if the loan is an annuity. The values must be either Y or N</li>
	<li>A Long representing the number of months between two loans payments (e.g. if the loan pays quarterly this must be 3)</li>
	<li>A Double representing the multiplier by which the loans is more likely to prepay compared to the base hypothesis. 100 is the base case.</li>
	<li>A Double representing the multiplier by which the loans is more likely to experience losses compared to the base hypothesis. 100 is the base case.</li></ol>
<li>A Long representing the number of tranches of the deal</li>
<li>For each tranche in the deal:</li>
	<ol type="a"><li>A String containing the name of the tranche</li>
	<li>A Long representing the redemption priority level of the note (pro rata group)</li>
	<li>A Double containing the original amount outstanding of the note</li>
	<li>A String containing the Bloomberg code of the note currency (e.g. USD, EUR, GBP)</li>
	<li>A double containing the current amount outstanding of the note</li>
	<li>A Long representing the Tranche::TrancheInterestType code determining if the notes pays fixed or floating interest</li>
	<li>A Double containing the margin or the interest rate paid by the note (in basis points)</li>
	<li>A String containing the Bloomberg ticker of the of the reference rate for the interest (if the note pays fixed interest this will be ignored)</li>
	<li>A String containing the last IPD in the ISO 8601 format (yyyy-mm-dd)</li>
	<li>A String containing the Bloomberg ticker of the base rate on which discount margin will be calculated for fixed rate notes (for floating rate notes this will be ignored)</li>
	<li>A Long containing the the number of months between two IPDs (e.g. if the deal pays quarterly this must be 3)</li>
	<li>A Double containing the value of the reference rate for the note. -1.0 will cause the program to get the value from Bloomberg</li>
	<li>A Double containing the limit of the OC test for the tranche (100% = 1.0)</li>
	<li>A Double containing the limit of the IC test for the tranche (100% = 1.0)</li>
	<li>A Double containing the price for the note in a 100 basis</li>
	<li>A Double containing the exchange rate for the note currency (1.0 for the currency in which the loans are denominated)</li>
	<li>A String containing the settlement date in the ISO 8601 format (yyyy-mm-dd)</li>
	<li>A Double representing the interest accrued on the note from the last IPD</li></ol>
<li>A Long containing the number of steps in the waterfall</li>
<li>For each step in the waterfall:</li>
	<ol type="a"><li>A Long representing the WatFalPrior::WaterfallStepType code for the step</li>
	<li>A Long representing the pro rata group to which the step is referred to</li>
	<li>A Long representing the source or destination of the payments due to this step (e.g. For Senior Expenses a 1 will deduct the expenses from interest while a 2 will deduct them from principal. For OC test this is the tranche that will be redeemed if the test fails)</li>
	<li>A Double, used only in OC test and ignored otherwise, that determines what share of available funds will go toward the redemption of the note specified in the previous argument. The rest will go on top of the principal waterfall.</li></ol>
<li>A Double containing senior expenses rate as a share of loans outstanding</li>
<li>A Double containing senior fees rate as a share of loans outstanding</li>
<li>A Double containing junior fees rate as a share of loans outstanding</li>
<li>A Double containing the interest rate at which unpaid junior fees are compounded</li>
<li>A Long containing the the number of months between two IPDs (e.g. if the deal pays quarterly this must be 3)</li>
<li>A String containing the next IPD in the ISO 8601 format (yyyy-mm-dd)</li>
<li>A String containing the previous IPD in the ISO 8601 format (yyyy-mm-dd)</li>
<li>A Boolean: if set to false, reinvestment test and OC test turbo features will not be used</li>
<li>A String containing a Bloomberg-like vector that represents the evoulution of CCC loans as a share of total loans</li>
<li>A Double representing the haircut inflicted to CCC loans if the CCC test is failed</li>
<li>A Double representing the CCC test limit</li>
<li>A String containing the call date in the ISO 8601 format (yyyy-mm-dd). Empty string ("") if not applicable</li>
<li>A Double by which the call reserve (see below) is multiplied for computing the call event. 0 if not applicable</li>
<li>A Double containing the call reserve. It can be either a seniority level or an absolute amount. When the outstanding amount of the notes gets below this amount the deal will be called. 0 if not applicable</li>
<li>A Double representing the value of the loan pool at the call date (in basis 100)</li>
<li>A String containing the reinvestment period end date in the ISO 8601 format (yyyy-mm-dd).</li>
<li>A Double representing the reinvestment test limit</li>
<li>A Double representing how much interest will be diverted to notes redemption during the reinvestment period in case the reinvestment test is breached</li>
<li>A Double representing how much interest will be diverted to acquire new collateral during the reinvestment period in case the reinvestment test is breached</li>
<li>A Double representing how much interest will be diverted to notes redemption after the reinvestment period in case the reinvestment test is breached</li>
<li>A Double representing how much interest will be diverted to acquire new collateral after the reinvestment period in case the reinvestment test is breached</li>
<li>A String containing a Bloomberg-like vector that represents the spread payed by new collateral coming from reinvestment</li>
<li>A String containing a Bloomberg-like vector that represents the CPR assumption for the model</li>
<li>A String containing a Bloomberg-like vector that represents the CDR assumption for the model</li>
<li>A String containing a Bloomberg-like vector that represents the LS assumption for the model</li>
<li>A String containing a Bloomberg-like vector that represents the 
(in years) of the reinvestment bond</li>
<li>A Long representing the number of months between two payments made by newly acquired loans (e.g. if the loans pay quarterly this must be 3)</li>
<li>A String containing the Bloomberg ticker of the of the reference rate for the new loans coming from reinvestment</li>
<li>A Double containing the principal available amounts currently in the cash account of the issuer</li>
<li>A Double containing the interest available amounts currently in the cash account of the issuer</li>
<li>A String containing the pool cut off date in the ISO 8601 format (yyyy-mm-dd).</li>
<li>A Boolean that determines if the call hypothesis should be evaluated or not</li>
<li>A String containing the address of the cell where the loans cash flows should be printed. Empty string ("") if you don't want them to be printed</li>
<li>A String containing the address of the cell where the tranches cash flows should be printed. Empty string ("") if you don't want them to be printed</li>
<li>A String containing the name of the sheet where the charts are located</li>
<li>A Long representing the index of the chart object where the loans cash flow graph should be printed on</li>
<li>A Long representing the index of the chart object where the tranches cash flow graph should be printed on</li>
<li>A Long representing the index of the chart object where the OC test graph should be printed on</li>
<li>A Long representing the index of the chart object where the IC test graph should be printed on</li>
<li>A Long representing the index of the chart object where the excess spread graph should be printed on</li>
<li>A Long representing the index of the chart object where the WA cost of capital graph should be printed on</li>
<li>A String containing the address of the cell where the loss rates of the notes should be printed. Empty string ("") if you don't want them to be printed</li>
<li>A String containing the address of the cell where the loss rates of the notes, in the call scenario, should be printed. Empty string ("") if you don't want them to be printed</li>
<li>A String containing the address of the cell where the credit enhancement of the notes should be printed. Empty string ("") if you don't want them to be printed</li>
<li>A String containing the path to the folder where the results should be saved to</li>
<li>A Boolean representing if the stress test should be run or just the base model</li></ol>

In case the stress test should be run, this additional inputs, in order, are necessary:
<ol start="53"><li>A String containing a Bloomberg-like vector that represents the parameter that will be kept constant in the stress test</li>
<li>A Long representing the StressTest::StressVariability code for the rows variation dimension</li>
<li>A Long representing the StressTest::StressVariability code for the columns variation dimension</li>
<li>A Long containing the number of scenarios on the rows variation dimension</li>
<li>For scenario on the rows variation dimension:</li>
	<ol type="a"><li>A String containing a Bloomberg-like vector that represents the variable stress value</li></ol>
<li>A Long containing the number of scenarios on the columns variation dimension</li>
<li>For scenario on the columns variation dimension:</li>
	<ol type="a"><li>A String containing a Bloomberg-like vector that represents the variable stress value</li></ol>
<li>A Boolean determining if the stress test should consider the call scenario</li>
</ol>
\warning Long refers to 32 bits integers. Using 16 bits one (Integer instead of Long in VBA) will lead to a crash
*/
/*! \fn double __stdcall CLODiscountMargin(LPSAFEARRAY *ArrayData)
\brief Function that gets the discount margin for a tranche from a result file
\param ArrayData Excel Variant Array of Inputs
\return The discount margin of the selected tranche
\details This function loads the results of the model and calculates the discount margin of a tranche associated with the given price

ArrayData must contain, in order:
-# A String containing the path to the stress result file
-# A String containing the name of the tranche for which you want to display the results
-# A Boolean defining if the calculation should consider the call option as exercised
-# A Double containing the price of the tranche
 */
/*! \fn double __stdcall CLOWALife(LPSAFEARRAY *ArrayData)
\brief Function that gets the weighted average life for a tranche from a result file
\param ArrayData Excel Variant Array of Inputs
\return The weighted average life of the selected tranche
\details This function loads the results of the model and calculates the weighted average life of a tranche

ArrayData must contain, in order:
-# A String containing the path to the stress result file
-# A String containing the name of the tranche for which you want to display the results
-# A string containing the date from which the WAL starts. It must be in the ISO 8601 format (yyyy-mm-dd)
-# A Boolean defining if the calculation should consider the call option as exercised
 */
/*! \fn void __stdcall StressTargetChanged(LPSAFEARRAY *ArrayData)
\brief Function that has to be called to get the stress results for a different tranche
\param ArrayData Excel Variant Array of Inputs
\details This function loads the results of the stress test and invokes ExcelOutput::PrintStressTest() to send the stress test results of the appropriate tranche back to excel.

ArrayData must contain, in order:
-# A String containing the path to the stress result file
-# A String containing the name of the tranche for which you want to display the results
-# A string containing the address of the cell where the stress results should be printed
-# A Long representing the StressTest::StressVariability dimention for the X parameter
-# A Long representing the StressTest::StressVariability dimention for the Y parameter
-# A double representing the price of the tranche
-# A String containing the name of the sheet where the discount margin plot should appear
-# A Long containing the index of the Chart where the discount margin plot should be displayed

\sa ExcelOutput::PrintStressTest()
 */
/*! \fn void __stdcall InspectStress(LPSAFEARRAY *ArrayData)
\brief Function that displays the whole waterfall results for a given stress scenario
\param ArrayData Excel Variant Array of Inputs
\details This function creates a Widget containing the waterfall results for the given stress scenario

ArrayData must contain, in order:
-# A String containing the path to the stress result file
-# A String containing the value for the X Parameter
-# A String containing the value for the Y Parameter
-# A Long representing the StressTest::StressVariability dimention for the X parameter
-# A Long representing the StressTest::StressVariability dimention for the Y parameter

\sa SummaryView
 */
/*! \fn void __stdcall InspectWaterfall(LPSAFEARRAY *ArrayData)
\brief Function that displays the two waterfalls
\param ArrayData Excel Variant Array of Inputs
\details This function creates a Widget containing two tables each of them showing one of the waterfalls.

ArrayData must contain, in order:
<ol><li>The number of steps in the aggregated waterfall</li>
<li>For each step in the waterfall</li>
	<li>Long representing the WatFalPrior::WaterfallStepType of the waterfall</li>
	<li>Long representing the target seniority group of the step</li>
	<li>Long representing funds sources or destination for the step</li>
	<li>Double between 0 and 1 representing the share of funds going toward redemption of the destination step (used only in OC Test)</li></ol></ol>

\sa WaterfallViewer
 */