#include "ExcelOutput.h"
#include <Tranche.h>
#include <MtgCashFlow.h>
#include <StressTest.h>
#include <qmath.h>
#include <QString>
HRESULT ExcelOutput::PrintMortgagesRepLines(
	const MtgCashFlow& source,
	const QString& DestinationAddress,
	bool PrintCounter, 
	bool PrintDates, 
	bool PrintOutstanding, 
	bool PrintInterest, 
	bool PrintScheduled, 
	bool PrintPrepay, 
	bool PrintAccruedInterest, 
	bool PrintTotalPrincipal,
	bool PrintDefaults,
	bool PrintLoss,
	bool PrintLossOnInterest,
	bool PrintDelinquencies,
	bool PrintWAL
	){
    LOGDEBUG("Reached PrintMortgagesRepLines");
		ExcelCommons::InitExcelOLE();
		int NumOfCols=
			PrintCounter
			+ PrintDates
			+ PrintOutstanding
			+ PrintInterest
			+ PrintScheduled
			+ PrintPrepay
			+ PrintAccruedInterest
			+ PrintTotalPrincipal
			+ PrintDefaults
			+ PrintLoss
			+ PrintLossOnInterest
			+ PrintDelinquencies
			+ PrintWAL
		;
		int Numrows=source.Count();
		SAFEARRAYBOUND  Bound[2];
		Bound[0].lLbound   = 1;
		Bound[0].cElements = Numrows;
		Bound[1].lLbound   = 0;
		Bound[1].cElements = NumOfCols;
		SAFEARRAY* saData;
		saData = SafeArrayCreate(VT_VARIANT, 2, Bound);
		VARIANT HUGEP *pdFreq;
		HRESULT hr = SafeArrayAccessData(saData, (void HUGEP* FAR*)&pdFreq);
		if (SUCCEEDED(hr))
		{
			if(PrintCounter){
				for (int i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_I4;
					pdFreq->intVal = i;
					pdFreq++;
				}
			}
			if(PrintDates){
				for (int i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_BSTR;
                    Q_ASSERT(!source.GetDate(i).isNull());      
					pdFreq->bstrVal = SysAllocString(source.GetDate(i).toString(Qt::ISODate).toStdWString().c_str());
					pdFreq++;
				}
			}
			if(PrintOutstanding){
				for (int i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetAmountOut(i);
					pdFreq++;
				}
			}
			if(PrintInterest){
				for (int i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetInterest(i);
					pdFreq++;
				}
			}
			if(PrintScheduled){
				for (int i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetScheduled(i);
					pdFreq++;
				}
			}
			if(PrintPrepay){
				for (int i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetPrepay(i);
					pdFreq++;
				}
			}
			if(PrintAccruedInterest){
				for (int i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetAccruedInterest(i);
					pdFreq++;
				}
			}
			if(PrintTotalPrincipal){
				for (int i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetPrincipal(i);
					pdFreq++;
				}
			}
			if (PrintDefaults) {
				for (int i = 0; i < Numrows; i++) {
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetDefaults(i);
					pdFreq++;
				}
			}
			if(PrintLoss){
				for (int i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetLoss(i);
					pdFreq++;
				}
			}
			if(PrintLossOnInterest){
				for (int i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetLossOnInterest(i);
					pdFreq++;
				}
			}
			if (PrintDelinquencies) {
				for (int i = 0; i < Numrows; i++) {
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetDelinquentShare(i);
					pdFreq++;
				}
			}
			if (PrintWAL) {
				for (int i = 0; i < Numrows; i++) {
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetWAL(i);
					pdFreq++;
				}
			}
			SafeArrayUnaccessData(saData);
		}
		static DISPID dispid = 0;
		DISPPARAMS Params;
		VARIANTARG Command[16];
		int CurrentCmdIndex=16-1;
		if(!ExcelCommons::pExcelDisp)return S_FALSE;
		try
		{
			Command[CurrentCmdIndex].vt = VT_BSTR;
			Command[CurrentCmdIndex--].bstrVal = SysAllocString(L"PrintMortgagesRepLines");
			Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
			Command[CurrentCmdIndex--].parray = saData;
			Command[CurrentCmdIndex].vt = VT_BSTR;
			Command[CurrentCmdIndex--].bstrVal = SysAllocString(DestinationAddress.toStdWString().c_str());
			Command[CurrentCmdIndex].vt = VT_BOOL;
			Command[CurrentCmdIndex--].boolVal=PrintCounter;
			Command[CurrentCmdIndex].vt = VT_BOOL;
			Command[CurrentCmdIndex--].boolVal=PrintDates;
			Command[CurrentCmdIndex].vt = VT_BOOL;
			Command[CurrentCmdIndex--].boolVal=PrintOutstanding;
			Command[CurrentCmdIndex].vt = VT_BOOL;
			Command[CurrentCmdIndex--].boolVal=PrintInterest;
			Command[CurrentCmdIndex].vt = VT_BOOL;
			Command[CurrentCmdIndex--].boolVal=PrintScheduled;
			Command[CurrentCmdIndex].vt = VT_BOOL;
			Command[CurrentCmdIndex--].boolVal=PrintPrepay;
			Command[CurrentCmdIndex].vt = VT_BOOL;
			Command[CurrentCmdIndex--].boolVal=PrintAccruedInterest;
			Command[CurrentCmdIndex].vt = VT_BOOL;
			Command[CurrentCmdIndex--].boolVal=PrintTotalPrincipal;
			Command[CurrentCmdIndex].vt = VT_BOOL;
			Command[CurrentCmdIndex--].boolVal = PrintDefaults;
			Command[CurrentCmdIndex].vt = VT_BOOL;
			Command[CurrentCmdIndex--].boolVal=PrintLoss;
			Command[CurrentCmdIndex].vt = VT_BOOL;
			Command[CurrentCmdIndex--].boolVal=PrintLossOnInterest;
			Command[CurrentCmdIndex].vt = VT_BOOL;
			Command[CurrentCmdIndex--].boolVal = PrintDelinquencies;
			Command[CurrentCmdIndex].vt = VT_BOOL;
			Command[CurrentCmdIndex--].boolVal = PrintWAL;
			Params.rgdispidNamedArgs = NULL;
			Params.rgvarg=Command;
			Params.cArgs = 16;
			Params.cNamedArgs = 0;
			if(dispid == 0)
			{
                LPOLESTR ucName = SysAllocString(L"Run");
				hr = ExcelCommons::pExcelDisp->GetIDsOfNames(IID_NULL, &ucName, 1,
					LOCALE_SYSTEM_DEFAULT, &dispid);
                SysFreeString(ucName);
				if(FAILED(hr))
				{
					SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
					SysFreeString(Params.rgvarg[Params.cArgs-3].bstrVal);
					if(PrintDates){
						SafeArrayAccessData(saData, (void HUGEP* FAR*)&pdFreq);
						if(PrintCounter) pdFreq+=Numrows;
						for (int i = 0; i < Numrows; i++)
						{
							SysFreeString(pdFreq->bstrVal);
							pdFreq++;
						}
						SafeArrayUnaccessData(saData);
					}
					return hr;
				}
			}
			hr = ExcelCommons::pExcelDisp->Invoke(dispid,IID_NULL,LOCALE_SYSTEM_DEFAULT,
				DISPATCH_METHOD, &Params, NULL, NULL, NULL);
			if(FAILED(hr))
			{
				SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-3].bstrVal);
				if(PrintDates){
					SafeArrayAccessData(saData, (void HUGEP* FAR*)&pdFreq);
					if(PrintCounter) pdFreq+=Numrows;
					for (int i = 0; i < Numrows; i++)
					{
						SysFreeString(pdFreq->bstrVal);
						pdFreq++;
					}
					SafeArrayUnaccessData(saData);
				}
				return hr;
			}
		}
		catch(_com_error &ce)
		{
			hr = ce.Error();
            return hr;
		}
		SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
		SysFreeString(Params.rgvarg[Params.cArgs-3].bstrVal);
		if(PrintDates){
			SafeArrayAccessData(saData, (void HUGEP* FAR*)&pdFreq);
			if(PrintCounter) pdFreq+=Numrows;
			for (int i = 0; i < Numrows; i++)
			{
				SysFreeString(pdFreq->bstrVal);
				pdFreq++;
			}
			SafeArrayUnaccessData(saData);
		}
		return hr;
}
HRESULT ExcelOutput::PlotMortgagesFlows(
	const MtgCashFlow& source,
	const QString& DestinationSheet,
	int DestinationIndex,
	bool PrintDates, 
	bool PrintOutstanding, 
	bool PrintInterest, 
	bool PrintScheduled, 
	bool PrintPrepay, 
	bool PrintTotalPrincipal,
	bool PrintLoss
	){
    LOGDEBUG("Reached PlotMortgagesFlows");
		ExcelCommons::InitExcelOLE();
		SAFEARRAYBOUND  Bound;
		Bound.lLbound   = 1;
		Bound.cElements = source.Count();
		VARIANT HUGEP *pdFreq;
		SAFEARRAY* DatesArray = SafeArrayCreate(VT_VARIANT, 1, &Bound);
		HRESULT hr = SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&pdFreq);
		if (SUCCEEDED(hr))
		{
			if(PrintDates){
				for (int i = 0; i < source.Count(); i++)
				{
					pdFreq->vt = VT_BSTR;
					pdFreq->bstrVal = SysAllocString(source.GetDate(i).toString("yyyy-MM-dd").toStdWString().c_str());
					pdFreq++;
				}
			}
			SafeArrayUnaccessData(DatesArray);
		}
		SAFEARRAY* OutstandingArray = SafeArrayCreate(VT_VARIANT, 1, &Bound);
		hr = SafeArrayAccessData(OutstandingArray, (void HUGEP* FAR*)&pdFreq);
		if (SUCCEEDED(hr))
		{
			if(PrintOutstanding){
				for (int i = 0; i < source.Count(); i++)
				{
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetAmountOut(i);
					pdFreq++;
				}
			}
			SafeArrayUnaccessData(OutstandingArray);
		}
		SAFEARRAY* InterestArray = SafeArrayCreate(VT_VARIANT, 1, &Bound);
		hr = SafeArrayAccessData(InterestArray, (void HUGEP* FAR*)&pdFreq);
		if (SUCCEEDED(hr))
		{
			if(PrintInterest){
				for (int i = 0; i < source.Count(); i++)
				{
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetInterest(i);
					pdFreq++;
				}
			}
			SafeArrayUnaccessData(InterestArray);
		}
		SAFEARRAY* ScheduledArray = SafeArrayCreate(VT_VARIANT, 1, &Bound);
		hr = SafeArrayAccessData(ScheduledArray, (void HUGEP* FAR*)&pdFreq);
		if (SUCCEEDED(hr))
		{
			if(PrintScheduled){
				for (int i = 0; i < source.Count(); i++)
				{
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetScheduled(i);
					pdFreq++;
				}
			}
			SafeArrayUnaccessData(ScheduledArray);
		}
		SAFEARRAY* PrepayArray = SafeArrayCreate(VT_VARIANT, 1, &Bound);
		hr = SafeArrayAccessData(PrepayArray, (void HUGEP* FAR*)&pdFreq);
		if (SUCCEEDED(hr))
		{
			if(PrintPrepay){
				for (int i = 0; i < source.Count(); i++)
				{
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetPrepay(i);
					pdFreq++;
				}
			}
			SafeArrayUnaccessData(PrepayArray);
		}
		SAFEARRAY* TotalPrincipalArray = SafeArrayCreate(VT_VARIANT, 1, &Bound);
		hr = SafeArrayAccessData(TotalPrincipalArray, (void HUGEP* FAR*)&pdFreq);
		if (SUCCEEDED(hr))
		{
			if(PrintTotalPrincipal){
				for (int i = 0; i < source.Count(); i++)
				{
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetPrincipal(i);
					pdFreq++;
				}
			}
			SafeArrayUnaccessData(TotalPrincipalArray);
		}
		SAFEARRAY* LossArray = SafeArrayCreate(VT_VARIANT, 1, &Bound);
		hr = SafeArrayAccessData(LossArray, (void HUGEP* FAR*)&pdFreq);
		if (SUCCEEDED(hr))
		{
			if(PrintLoss){
				for (int i = 0; i < source.Count(); i++)
				{
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetLoss(i);
					pdFreq++;
				}
			}
			SafeArrayUnaccessData(LossArray);
		}

		static DISPID dispid = 0;
		DISPPARAMS Params;
		VARIANTARG Command[10];
		int CurrentCmdIndex=10-1;
		if(!ExcelCommons::pExcelDisp)return S_FALSE;
		try
		{
			Command[CurrentCmdIndex].vt = VT_BSTR;
			Command[CurrentCmdIndex--].bstrVal = SysAllocString(L"PlotMortgagesFlows");
			Command[CurrentCmdIndex].vt = VT_BSTR;
			Command[CurrentCmdIndex--].bstrVal = SysAllocString(DestinationSheet.toStdWString().c_str());
			Command[CurrentCmdIndex].vt = VT_I4;
			Command[CurrentCmdIndex--].intVal = DestinationIndex;
			Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
			Command[CurrentCmdIndex--].parray = DatesArray;
			Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
			Command[CurrentCmdIndex--].parray = OutstandingArray;
			Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
			Command[CurrentCmdIndex--].parray = InterestArray;
			Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
			Command[CurrentCmdIndex--].parray = ScheduledArray;
			Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
			Command[CurrentCmdIndex--].parray = PrepayArray;
			Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
			Command[CurrentCmdIndex--].parray = TotalPrincipalArray;
			Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
			Command[CurrentCmdIndex--].parray = LossArray;

			Params.rgdispidNamedArgs = NULL;
			Params.rgvarg=Command;
			Params.cArgs = 10;
			Params.cNamedArgs = 0;
			if(dispid == 0)
			{
                LPOLESTR ucName = SysAllocString(L"Run");
				hr = ExcelCommons::pExcelDisp->GetIDsOfNames(IID_NULL, &ucName, 1,
					LOCALE_SYSTEM_DEFAULT, &dispid);
                SysFreeString(ucName);
				if(FAILED(hr))
				{
					SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
					SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
					if(PrintDates){
						SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&pdFreq);
						for (int i = 0; i < source.Count(); i++)
						{
							SysFreeString(pdFreq->bstrVal);
							pdFreq++;
						}
						SafeArrayUnaccessData(DatesArray);
					}
					return hr;
				}
			}
			hr = ExcelCommons::pExcelDisp->Invoke(dispid,IID_NULL,LOCALE_SYSTEM_DEFAULT,
				DISPATCH_METHOD, &Params, NULL, NULL, NULL);
			if(FAILED(hr))
			{
				SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
				if(PrintDates){
					SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&pdFreq);
					for (int i = 0; i < source.Count(); i++)
					{
						SysFreeString(pdFreq->bstrVal);
						pdFreq++;
					}
					SafeArrayUnaccessData(DatesArray);
				}
				return hr;
			}
		}
		catch(_com_error &ce)
		{
			hr = ce.Error();
            return hr;
		}
		SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
		SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
		if(PrintDates){
			SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&pdFreq);
			for (int i = 0; i < source.Count(); i++)
			{
				SysFreeString(pdFreq->bstrVal);
				pdFreq++;
			}
			SafeArrayUnaccessData(DatesArray);
		}
		return hr;
			
}
HRESULT ExcelOutput::PrintTrancheFlow(
	const Tranche& source,
	const QString& DestinationAddress,
	const QColor& BackgrndCol,
	bool PrintDates, 
	bool PrintOutstanding, 
	bool PrintInterest, 
	bool PrintPrincipal, 
	bool PrintTotalFlow, 
	bool PrintDeferred, 
	bool PrintOCtest, 
	bool PrintICtest,
	bool PrintPDL
	){
    LOGDEBUG("Reached PrintTrancheFlow");
		ExcelCommons::InitExcelOLE();
		QString ColorString=QString("%1,%2,%3").arg(BackgrndCol.red()).arg(BackgrndCol.green()).arg(BackgrndCol.blue());
		int NumOfCols=
			PrintDates + PrintOutstanding + PrintInterest + PrintPrincipal + PrintTotalFlow + PrintDeferred + PrintOCtest + PrintICtest + (2 * PrintPDL);
		int Numrows= source.GetCashFlow().Count();
		SAFEARRAYBOUND  Bound[2];
		Bound[0].lLbound   = 1;
		Bound[0].cElements = Numrows;
		Bound[1].lLbound   = 0;
		Bound[1].cElements = NumOfCols;
		
		SAFEARRAY* saData = SafeArrayCreate(VT_VARIANT, 2, Bound);
		VARIANT HUGEP *pdFreq;
		HRESULT hr = SafeArrayAccessData(saData, (void HUGEP* FAR*)&pdFreq);
		if (SUCCEEDED(hr))
		{
			if(PrintDates){
				for (int i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_BSTR;
					pdFreq->bstrVal = SysAllocString(source.GetCashFlow().GetDate(i).toString("yyyy-MM-dd").toStdWString().c_str());
					pdFreq++;
				}
			}
			if(PrintOutstanding){
                for (int i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetCashFlow().GetAmountOutstanding(i);
					pdFreq++;
				}
			}
			if(PrintInterest){
                for (int i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetCashFlow().GetTotalInterest(i);
					pdFreq++;
				}
			}
			if(PrintPrincipal){
                for (int i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetCashFlow().GetPrincipal(i);
					pdFreq++;
				}
			}
			if(PrintTotalFlow){
                for (int i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetCashFlow().GetTotalFlow(i);
					pdFreq++;
				}
			}
			if(PrintDeferred){
                for (int i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetCashFlow().GetTotalDeferred(i);
					pdFreq++;
				}
			}
			if(PrintOCtest){
                for (int i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetCashFlow().GetOCTest(i);
					pdFreq++;
				}
			}
			if(PrintICtest){
                for (int i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetCashFlow().GetICTest(i);
					pdFreq++;
				}
			}
			if (PrintPDL) {
                for (int i = 0; i < Numrows; i++) {
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetCashFlow().GetPDLOutstanding(i);
					pdFreq++;
				}
                for (int i = 0; i < Numrows; i++) {
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetCashFlow().GetPDLCured(i);
					pdFreq++;
				}
			}
			SafeArrayUnaccessData(saData);
		}
		SAFEARRAYBOUND ICLimitBound;
		SAFEARRAYBOUND OCLimitBound;
		OCLimitBound.lLbound = 1;
		OCLimitBound.cElements = Numrows;
		ICLimitBound.lLbound = 1;
		ICLimitBound.cElements = Numrows;
		SAFEARRAY* OCLimitData = SafeArrayCreate(VT_VARIANT, 1, &OCLimitBound);
		hr = SafeArrayAccessData(OCLimitData, (void HUGEP* FAR*)&pdFreq);
		if (SUCCEEDED(hr)) {
            for (int i = 0; i < Numrows; i++) {
				pdFreq->vt = VT_R8;
				pdFreq->dblVal = source.GetCashFlow().GetOCTarget(i);
				pdFreq++;
			}
			SafeArrayUnaccessData(OCLimitData);
		}
		SAFEARRAY* ICLimitData = SafeArrayCreate(VT_VARIANT, 1, &ICLimitBound);
		hr = SafeArrayAccessData(ICLimitData, (void HUGEP* FAR*)&pdFreq);
		if (SUCCEEDED(hr)) {
            for (int i = 0; i < Numrows; i++) {
				pdFreq->vt = VT_R8;
				pdFreq->dblVal = source.GetCashFlow().GetICTarget(i);
				pdFreq++;
			}
			SafeArrayUnaccessData(ICLimitData);
		}

		static DISPID dispid = 0;
		DISPPARAMS Params;
		VARIANTARG Command[16];
		int CurrentCmdIndex=16-1;
		if(!ExcelCommons::pExcelDisp)return S_FALSE;
		try
		{
			Command[CurrentCmdIndex].vt = VT_BSTR;
			Command[CurrentCmdIndex--].bstrVal = SysAllocString(L"PrintTranche");
			Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
			Command[CurrentCmdIndex--].parray = saData;
			Command[CurrentCmdIndex].vt = VT_BSTR;
			Command[CurrentCmdIndex--].bstrVal = SysAllocString(DestinationAddress.toStdWString().c_str());
			Command[CurrentCmdIndex].vt = VT_BSTR;
			Command[CurrentCmdIndex--].bstrVal = SysAllocString(source.GetTrancheName().toStdWString().c_str());
			Command[CurrentCmdIndex].vt = VT_BSTR;
			Command[CurrentCmdIndex--].bstrVal = SysAllocString(ColorString.toStdWString().c_str());
			Command[CurrentCmdIndex].vt = VT_BOOL;
			Command[CurrentCmdIndex--].boolVal=PrintDates;
			Command[CurrentCmdIndex].vt = VT_BOOL;
			Command[CurrentCmdIndex--].boolVal=PrintOutstanding;
			Command[CurrentCmdIndex].vt = VT_BOOL;
			Command[CurrentCmdIndex--].boolVal=PrintInterest;
			Command[CurrentCmdIndex].vt = VT_BOOL;
			Command[CurrentCmdIndex--].boolVal=PrintPrincipal;
			Command[CurrentCmdIndex].vt = VT_BOOL;
			Command[CurrentCmdIndex--].boolVal=PrintTotalFlow;
			Command[CurrentCmdIndex].vt = VT_BOOL;
			Command[CurrentCmdIndex--].boolVal=PrintDeferred;
			Command[CurrentCmdIndex].vt = VT_BOOL;
			Command[CurrentCmdIndex--].boolVal=PrintOCtest;
			Command[CurrentCmdIndex].vt = VT_BOOL;
			Command[CurrentCmdIndex--].boolVal=PrintICtest;
			Command[CurrentCmdIndex].vt = VT_BOOL;
			Command[CurrentCmdIndex--].boolVal = PrintPDL;
			Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
			Command[CurrentCmdIndex--].parray = ICLimitData;
			Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
			Command[CurrentCmdIndex--].parray = OCLimitData;

			
			Params.rgdispidNamedArgs = NULL;
			Params.rgvarg=Command;
			Params.cArgs = 16;
			Params.cNamedArgs = 0;
			if(dispid == 0)
			{
                LPOLESTR ucName = SysAllocString(L"Run");
				hr = ExcelCommons::pExcelDisp->GetIDsOfNames(IID_NULL, &ucName, 1,
					LOCALE_SYSTEM_DEFAULT, &dispid);
                SysFreeString(ucName);
				if(FAILED(hr))
				{
					SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
					SysFreeString(Params.rgvarg[Params.cArgs-3].bstrVal);
					SysFreeString(Params.rgvarg[Params.cArgs-4].bstrVal);
					SysFreeString(Params.rgvarg[Params.cArgs-5].bstrVal);
					if(PrintDates){
						SafeArrayAccessData(saData, (void HUGEP* FAR*)&pdFreq);
                        for (int i = 0; i < Numrows; i++)
						{
							SysFreeString(pdFreq->bstrVal);
							pdFreq++;
						}
						SafeArrayUnaccessData(saData);
					}
					return hr;
				}
			}
			hr = ExcelCommons::pExcelDisp->Invoke(dispid,IID_NULL,LOCALE_SYSTEM_DEFAULT,
				DISPATCH_METHOD, &Params, NULL, NULL, NULL);
			if(FAILED(hr))
			{
				LOGDEBUG("Failed Invoke " + QString::number(hr, 16));
				SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-3].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-4].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-5].bstrVal);
				if(PrintDates){
					SafeArrayAccessData(saData, (void HUGEP* FAR*)&pdFreq);
                    for (int i = 0; i < Numrows; i++)
					{
						SysFreeString(pdFreq->bstrVal);
						pdFreq++;
					}
					SafeArrayUnaccessData(saData);
				}
				return hr;
			}
		}
		catch(_com_error &ce)
		{
			hr = ce.Error();
			LOGDEBUG("Exception in call" + QString::number(hr, 16));
            return hr;
		}
		SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
		SysFreeString(Params.rgvarg[Params.cArgs-3].bstrVal);
		SysFreeString(Params.rgvarg[Params.cArgs-4].bstrVal);
		SysFreeString(Params.rgvarg[Params.cArgs-5].bstrVal);
		if(PrintDates){
			SafeArrayAccessData(saData, (void HUGEP* FAR*)&pdFreq);
            for (int i = 0; i < Numrows; i++)
			{
				SysFreeString(pdFreq->bstrVal);
				pdFreq++;
			}
			SafeArrayUnaccessData(saData);
		}
		LOGDEBUG("Reached End " + QString::number(hr, 16));
		return hr;
}




HRESULT ExcelOutput::PrintMergedCell(const QString& msg, const QString& TargetCell, int RowDim, int ColDim,const QColor& FillColor){
    LOGDEBUG("Reached PrintMergedCell");
	ExcelCommons::InitExcelOLE();
	QString ColorString=
		QString("%1,%2,%3").arg(FillColor.red()).arg(FillColor.green()).arg(FillColor.blue());
	HRESULT hr;
	static DISPID dispid = 0;
	DISPPARAMS Params;
	VARIANTARG Command[6];
	int CurrentCmdIndex=6-1;
	if(!ExcelCommons::pExcelDisp)return S_FALSE;
	try
	{
		Command[CurrentCmdIndex].vt = VT_BSTR;
		Command[CurrentCmdIndex--].bstrVal = SysAllocString(L"PrintMergedCell");
		Command[CurrentCmdIndex].vt = VT_BSTR;
		Command[CurrentCmdIndex--].bstrVal = SysAllocString(msg.toStdWString().c_str());
		Command[CurrentCmdIndex].vt = VT_BSTR;
		Command[CurrentCmdIndex--].bstrVal = SysAllocString(TargetCell.toStdWString().c_str());
		Command[CurrentCmdIndex].vt = VT_I4;
		Command[CurrentCmdIndex--].intVal=RowDim;
		Command[CurrentCmdIndex].vt = VT_I4;
		Command[CurrentCmdIndex--].intVal=ColDim;
		Command[CurrentCmdIndex].vt = VT_BSTR;
		Command[CurrentCmdIndex--].bstrVal = SysAllocString(ColorString.toStdWString().c_str());
		Params.rgdispidNamedArgs = NULL;
		Params.rgvarg=Command;
		Params.cArgs = 6;
		Params.cNamedArgs = 0;
		if(dispid == 0)
		{
            LPOLESTR ucName = SysAllocString(L"Run");
			hr = ExcelCommons::pExcelDisp->GetIDsOfNames(IID_NULL, &ucName, 1,
				LOCALE_SYSTEM_DEFAULT, &dispid);
            SysFreeString(ucName);
			if(FAILED(hr))
			{
				SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-3].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-6].bstrVal);
				return hr;
			}
		}
		hr = ExcelCommons::pExcelDisp->Invoke(dispid,IID_NULL,LOCALE_SYSTEM_DEFAULT,
			DISPATCH_METHOD, &Params, NULL, NULL, NULL);
		if(FAILED(hr))
		{
			SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
			SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
			SysFreeString(Params.rgvarg[Params.cArgs-3].bstrVal);
			SysFreeString(Params.rgvarg[Params.cArgs-6].bstrVal);
			return hr;
		}
	}
	catch(_com_error &ce)
	{
		hr = ce.Error();
        return hr;
	}
	SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
	SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
	SysFreeString(Params.rgvarg[Params.cArgs-3].bstrVal);
	SysFreeString(Params.rgvarg[Params.cArgs-6].bstrVal);
	return hr;
}
HRESULT ExcelOutput::PrintColumn(const QString& Title, const QList<double>& Values ,const QString& TargetCell,const QString& ValFormat,const QColor& FillColor){
    LOGDEBUG("Reached PrintColumn");
	ExcelCommons::InitExcelOLE();
	QString ColorString=
		QString("%1,%2,%3").arg(FillColor.red()).arg(FillColor.green()).arg(FillColor.blue());
	SAFEARRAYBOUND  Bound;
	Bound.lLbound   = 1;
	Bound.cElements = Values.size();
	SAFEARRAY* saData;
	saData = SafeArrayCreate(VT_VARIANT, 1, &Bound);
	VARIANT HUGEP *pdFreq;
	HRESULT hr = SafeArrayAccessData(saData, (void HUGEP* FAR*)&pdFreq);
	if (SUCCEEDED(hr))
	{
		foreach(const double& singleValue,Values){
			pdFreq->vt = VT_R8;
			pdFreq->dblVal = singleValue;
			pdFreq++;
		}
		SafeArrayUnaccessData(saData);
	}
	static DISPID dispid = 0;
	DISPPARAMS Params;
	VARIANTARG Command[6];
	int CurrentCmdIndex=6-1;
	if(!ExcelCommons::pExcelDisp)return S_FALSE;
	try
	{
		Command[CurrentCmdIndex].vt = VT_BSTR;
		Command[CurrentCmdIndex--].bstrVal = SysAllocString(L"PrintColumn");
		Command[CurrentCmdIndex].vt = VT_BSTR;
		Command[CurrentCmdIndex--].bstrVal = SysAllocString(Title.toStdWString().c_str());
		Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
		Command[CurrentCmdIndex--].parray = saData;
		Command[CurrentCmdIndex].vt = VT_BSTR;
		Command[CurrentCmdIndex--].bstrVal = SysAllocString(TargetCell.toStdWString().c_str());
		Command[CurrentCmdIndex].vt = VT_BSTR;
		Command[CurrentCmdIndex--].bstrVal = SysAllocString(ValFormat.toStdWString().c_str());
		Command[CurrentCmdIndex].vt = VT_BSTR;
		Command[CurrentCmdIndex--].bstrVal = SysAllocString(ColorString.toStdWString().c_str());
		Params.rgdispidNamedArgs = NULL;
		Params.rgvarg=Command;
		Params.cArgs = 6;
		Params.cNamedArgs = 0;
		if(dispid == 0)
		{
            LPOLESTR ucName = SysAllocString(L"Run");
			hr = ExcelCommons::pExcelDisp->GetIDsOfNames(IID_NULL, &ucName, 1,
				LOCALE_SYSTEM_DEFAULT, &dispid);
            SysFreeString(ucName);
			if(FAILED(hr))
			{
				SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-4].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-5].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-6].bstrVal);
				return hr;
			}
		}
		hr = ExcelCommons::pExcelDisp->Invoke(dispid,IID_NULL,LOCALE_SYSTEM_DEFAULT,
			DISPATCH_METHOD, &Params, NULL, NULL, NULL);
		if(FAILED(hr))
		{
			SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
			SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
			SysFreeString(Params.rgvarg[Params.cArgs-4].bstrVal);
			SysFreeString(Params.rgvarg[Params.cArgs-5].bstrVal);
			SysFreeString(Params.rgvarg[Params.cArgs-6].bstrVal);
			return hr;
		}
	}
	catch(_com_error &ce)
	{
		hr = ce.Error();
        return hr;
	}
	SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
	SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
	SysFreeString(Params.rgvarg[Params.cArgs-4].bstrVal);
	SysFreeString(Params.rgvarg[Params.cArgs-5].bstrVal);
	SysFreeString(Params.rgvarg[Params.cArgs-6].bstrVal);
	return hr;
}
HRESULT ExcelOutput::PrintDataColumn(const QList<double>& Values ,const QString& TargetCell,const QString& ValFormat){
    LOGDEBUG("Reached PrintDataColumn");
	ExcelCommons::InitExcelOLE();
	SAFEARRAYBOUND  Bound;
	Bound.lLbound   = 1;
	Bound.cElements = Values.size();
	SAFEARRAY* saData;
	saData = SafeArrayCreate(VT_VARIANT, 1, &Bound);
	VARIANT HUGEP *pdFreq;
	HRESULT hr = SafeArrayAccessData(saData, (void HUGEP* FAR*)&pdFreq);
	if (SUCCEEDED(hr))
	{
		foreach(const double& singleValue,Values){
			pdFreq->vt = VT_R8;
			pdFreq->dblVal = singleValue;
			pdFreq++;
		}
		SafeArrayUnaccessData(saData);
	}
	static DISPID dispid = 0;
	DISPPARAMS Params;
	VARIANTARG Command[4];
	int CurrentCmdIndex=4-1;
	if(!ExcelCommons::pExcelDisp)return S_FALSE;
	try
	{
		Command[CurrentCmdIndex].vt = VT_BSTR;
		Command[CurrentCmdIndex--].bstrVal = SysAllocString(L"PrintDataColumn");
		Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
		Command[CurrentCmdIndex--].parray = saData;
		Command[CurrentCmdIndex].vt = VT_BSTR;
		Command[CurrentCmdIndex--].bstrVal = SysAllocString(TargetCell.toStdWString().c_str());
		Command[CurrentCmdIndex].vt = VT_BSTR;
		Command[CurrentCmdIndex--].bstrVal = SysAllocString(ValFormat.toStdWString().c_str());
		Params.rgdispidNamedArgs = NULL;
		Params.rgvarg=Command;
		Params.cArgs = 4;
		Params.cNamedArgs = 0;
		if(dispid == 0)
		{
            LPOLESTR ucName = SysAllocString(L"Run");
			hr = ExcelCommons::pExcelDisp->GetIDsOfNames(IID_NULL, &ucName, 1,
				LOCALE_SYSTEM_DEFAULT, &dispid);
            SysFreeString(ucName);
			if(FAILED(hr))
			{
				SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-3].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-4].bstrVal);
				return hr;
			}
		}
		hr = ExcelCommons::pExcelDisp->Invoke(dispid,IID_NULL,LOCALE_SYSTEM_DEFAULT,
			DISPATCH_METHOD, &Params, NULL, NULL, NULL);
		if(FAILED(hr))
		{
			SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
			SysFreeString(Params.rgvarg[Params.cArgs-3].bstrVal);
			SysFreeString(Params.rgvarg[Params.cArgs-4].bstrVal);
			return hr;
		}
	}
	catch(_com_error &ce)
	{
		hr = ce.Error();
        return hr;
	}
	SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
	SysFreeString(Params.rgvarg[Params.cArgs-3].bstrVal);
	SysFreeString(Params.rgvarg[Params.cArgs-4].bstrVal);
	return hr;
}

HRESULT ExcelOutput::PlotTranchesDynamic(
	const Waterfall& source,
	const QString& DestinationSheet,
	int DestinationIndex,
	const QDate& CallDate
	){
    LOGDEBUG("Reached PlotTranchesDynamic");
		ExcelCommons::InitExcelOLE();
		SAFEARRAYBOUND  Bound[2];
		Bound[0].lLbound   = 1;
		Bound[0].cElements = source.GetTranche(0)->GetCashFlow().Count();
		Bound[1].lLbound   = 1;
		Bound[1].cElements = source.GetTranchesCount();
		VARIANT HUGEP *pdFreq;
		SAFEARRAY* DatesArray = SafeArrayCreate(VT_VARIANT, 1, Bound);
		HRESULT hr = SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&pdFreq);
		if (SUCCEEDED(hr))
		{
			for (int i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++){
				pdFreq->vt = VT_BSTR;
				pdFreq->bstrVal = SysAllocString(source.GetTranche(0)->GetCashFlow().GetDate(i).toString("yyyy-MM-dd").toStdWString().c_str());
				pdFreq++;
			}
			SafeArrayUnaccessData(DatesArray);
		}
		SAFEARRAY* OutstandingArray = SafeArrayCreate(VT_VARIANT, 2, Bound);
		hr = SafeArrayAccessData(OutstandingArray, (void HUGEP* FAR*)&pdFreq);
		if (SUCCEEDED(hr))
		{
			for (int j=0;j<source.GetTranchesCount();j++){
				for (int i = 0; i < source.GetTranche(j)->GetCashFlow().Count(); i++){
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetTranche(j)->GetCashFlow().GetAmountOutstanding(i);
					pdFreq++;
				}
			}
			SafeArrayUnaccessData(OutstandingArray);
		}
		SAFEARRAY* TitlesArray = SafeArrayCreate(VT_VARIANT, 1, Bound+1);
		hr = SafeArrayAccessData(TitlesArray, (void HUGEP* FAR*)&pdFreq);
		if (SUCCEEDED(hr)){
			for (int j=0;j<source.GetTranchesCount();j++){
				pdFreq->vt = VT_BSTR;
				pdFreq->bstrVal = SysAllocString(source.GetTranche(j)->GetTrancheName().toStdWString().c_str());
				pdFreq++;
			}
		}

		static DISPID dispid = 0;
		DISPPARAMS Params;
		VARIANTARG Command[7];
		int CurrentCmdIndex=7-1;
		if(!ExcelCommons::pExcelDisp)return S_FALSE;
		try
		{
			Command[CurrentCmdIndex].vt = VT_BSTR;
			Command[CurrentCmdIndex--].bstrVal = SysAllocString(L"PlotTranchesDynamic");
			Command[CurrentCmdIndex].vt = VT_BSTR;
			Command[CurrentCmdIndex--].bstrVal = SysAllocString(DestinationSheet.toStdWString().c_str());
			Command[CurrentCmdIndex].vt = VT_I4;
			Command[CurrentCmdIndex--].intVal = DestinationIndex;
			Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
			Command[CurrentCmdIndex--].parray = DatesArray;
			Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
			Command[CurrentCmdIndex--].parray = OutstandingArray;
			Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
			Command[CurrentCmdIndex--].parray = TitlesArray;
			Command[CurrentCmdIndex].vt = VT_BSTR;
			Command[CurrentCmdIndex--].bstrVal = SysAllocString(CallDate.isNull() ? L"" : CallDate.toString("yyyy-MM-dd").toStdWString().c_str());

			Params.rgdispidNamedArgs = NULL;
			Params.rgvarg=Command;
			Params.cArgs = 7;
			Params.cNamedArgs = 0;
			if(dispid == 0)
			{
                LPOLESTR ucName = SysAllocString(L"Run");
				hr = ExcelCommons::pExcelDisp->GetIDsOfNames(IID_NULL, &ucName, 1,
					LOCALE_SYSTEM_DEFAULT, &dispid);
                SysFreeString(ucName);
				if(FAILED(hr))
				{
					SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
					SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
					SysFreeString(Params.rgvarg[Params.cArgs-7].bstrVal);
					SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&pdFreq);
					for (int i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++)
					{
						SysFreeString(pdFreq->bstrVal);
						pdFreq++;
					}
					SafeArrayUnaccessData(DatesArray);
					SafeArrayAccessData(TitlesArray, (void HUGEP* FAR*)&pdFreq);
					for (int i = 0; i < source.GetTranchesCount(); i++)
					{
						SysFreeString(pdFreq->bstrVal);
						pdFreq++;
					}
					SafeArrayUnaccessData(TitlesArray);
					return hr;
				}
			}
			hr = ExcelCommons::pExcelDisp->Invoke(dispid,IID_NULL,LOCALE_SYSTEM_DEFAULT,
				DISPATCH_METHOD, &Params, NULL, NULL, NULL);
			if(FAILED(hr))
			{
				SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-7].bstrVal);
				SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&pdFreq);
				for (int i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++)
				{
					SysFreeString(pdFreq->bstrVal);
					pdFreq++;
				}
				SafeArrayUnaccessData(DatesArray);
				SafeArrayAccessData(TitlesArray, (void HUGEP* FAR*)&pdFreq);
				for (int i = 0; i < source.GetTranchesCount(); i++)
				{
					SysFreeString(pdFreq->bstrVal);
					pdFreq++;
				}
				SafeArrayUnaccessData(TitlesArray);
				return hr;
			}
		}
		catch(_com_error &ce)
		{
			hr = ce.Error();
            return hr;
		}
		SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
		SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
		SysFreeString(Params.rgvarg[Params.cArgs-7].bstrVal);
		SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&pdFreq);
		for (int i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++)
		{
			SysFreeString(pdFreq->bstrVal);
			pdFreq++;
		}
		SafeArrayUnaccessData(DatesArray);
		SafeArrayAccessData(TitlesArray, (void HUGEP* FAR*)&pdFreq);
        for (int i = 0; i < source.GetTranchesCount(); i++)
		{
			SysFreeString(pdFreq->bstrVal);
			pdFreq++;
		}
		SafeArrayUnaccessData(TitlesArray);
		return hr;
}
HRESULT ExcelOutput::PlotOCTest(
	const Waterfall& source,
	const QString& DestinationSheet,
	int DestinationIndex,
	const QDate& CallDate
	){
    LOGDEBUG("Reached PlotOCTest");
		ExcelCommons::InitExcelOLE();
		SAFEARRAYBOUND  Bound[2];
		Bound[0].lLbound   = 1;
		Bound[0].cElements = source.GetTranche(0)->GetCashFlow().Count();
		Bound[1].lLbound   = 1;
		Bound[1].cElements = source.GetTranchesCount();
		VARIANT HUGEP *pdFreq;
		SAFEARRAY* DatesArray = SafeArrayCreate(VT_VARIANT, 1, Bound);
		HRESULT hr = SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&pdFreq);
		if (SUCCEEDED(hr))
		{
			for (int i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++){
				pdFreq->vt = VT_BSTR;
				pdFreq->bstrVal = SysAllocString(source.GetTranche(0)->GetCashFlow().GetDate(i).toString("yyyy-MM-dd").toStdWString().c_str());
				pdFreq++;
			}
			SafeArrayUnaccessData(DatesArray);
		}
		SAFEARRAY* OutstandingArray = SafeArrayCreate(VT_VARIANT, 2, Bound);
		hr = SafeArrayAccessData(OutstandingArray, (void HUGEP* FAR*)&pdFreq);
		if (SUCCEEDED(hr))
		{
			for (int j=0;j<source.GetTranchesCount();j++){
				for (int i = 0; i < source.GetTranche(j)->GetCashFlow().Count(); i++){
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetTranche(j)->GetCashFlow().GetOCTest(i);
					pdFreq++;
				}
			}
			SafeArrayUnaccessData(OutstandingArray);
		}
		SAFEARRAY* TitlesArray = SafeArrayCreate(VT_VARIANT, 1, Bound+1);
		hr = SafeArrayAccessData(TitlesArray, (void HUGEP* FAR*)&pdFreq);
		if (SUCCEEDED(hr)){
			for (int j=0;j<source.GetTranchesCount();j++){
				pdFreq->vt = VT_BSTR;
				pdFreq->bstrVal = SysAllocString(source.GetTranche(j)->GetTrancheName().toStdWString().c_str());
				pdFreq++;
			}
		}

		static DISPID dispid = 0;
		DISPPARAMS Params;
		VARIANTARG Command[7];
		int CurrentCmdIndex=7-1;
		if(!ExcelCommons::pExcelDisp)return S_FALSE;
		try
		{
			Command[CurrentCmdIndex].vt = VT_BSTR;
			Command[CurrentCmdIndex--].bstrVal = SysAllocString(L"PlotOCTest");
			Command[CurrentCmdIndex].vt = VT_BSTR;
			Command[CurrentCmdIndex--].bstrVal = SysAllocString(DestinationSheet.toStdWString().c_str());
			Command[CurrentCmdIndex].vt = VT_I4;
			Command[CurrentCmdIndex--].intVal = DestinationIndex;
			Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
			Command[CurrentCmdIndex--].parray = DatesArray;
			Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
			Command[CurrentCmdIndex--].parray = OutstandingArray;
			Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
			Command[CurrentCmdIndex--].parray = TitlesArray;
			Command[CurrentCmdIndex].vt = VT_BSTR;
			Command[CurrentCmdIndex--].bstrVal = SysAllocString(CallDate.isNull() ? L"" : CallDate.toString("yyyy-MM-dd").toStdWString().c_str());

			Params.rgdispidNamedArgs = NULL;
			Params.rgvarg=Command;
			Params.cArgs = 7;
			Params.cNamedArgs = 0;
			if(dispid == 0)
			{
                LPOLESTR ucName = SysAllocString(L"Run");
				hr = ExcelCommons::pExcelDisp->GetIDsOfNames(IID_NULL, &ucName, 1,
					LOCALE_SYSTEM_DEFAULT, &dispid);
                SysFreeString(ucName);
				if(FAILED(hr))
				{
					SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
					SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
					SysFreeString(Params.rgvarg[Params.cArgs-7].bstrVal);
					SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&pdFreq);
                    for (int i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++)
					{
						SysFreeString(pdFreq->bstrVal);
						pdFreq++;
					}
					SafeArrayUnaccessData(DatesArray);
					SafeArrayAccessData(TitlesArray, (void HUGEP* FAR*)&pdFreq);
                    for (int i = 0; i < source.GetTranchesCount(); i++)
					{
						SysFreeString(pdFreq->bstrVal);
						pdFreq++;
					}
					SafeArrayUnaccessData(TitlesArray);
					return hr;
				}
			}
			hr = ExcelCommons::pExcelDisp->Invoke(dispid,IID_NULL,LOCALE_SYSTEM_DEFAULT,
				DISPATCH_METHOD, &Params, NULL, NULL, NULL);
			if(FAILED(hr))
			{
				SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-7].bstrVal);
				SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&pdFreq);
                for (int i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++)
				{
					SysFreeString(pdFreq->bstrVal);
					pdFreq++;
				}
				SafeArrayUnaccessData(DatesArray);
				SafeArrayAccessData(TitlesArray, (void HUGEP* FAR*)&pdFreq);
                for (int i = 0; i < source.GetTranchesCount(); i++)
				{
					SysFreeString(pdFreq->bstrVal);
					pdFreq++;
				}
				SafeArrayUnaccessData(TitlesArray);
				return hr;
			}
		}
		catch(_com_error &ce)
		{
			hr = ce.Error();
            return hr;
		}
		SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
		SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
		SysFreeString(Params.rgvarg[Params.cArgs-7].bstrVal);
		SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&pdFreq);
        for (int i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++)
		{
			SysFreeString(pdFreq->bstrVal);
			pdFreq++;
		}
		SafeArrayUnaccessData(DatesArray);
		SafeArrayAccessData(TitlesArray, (void HUGEP* FAR*)&pdFreq);
        for (int i = 0; i < source.GetTranchesCount(); i++)
		{
			SysFreeString(pdFreq->bstrVal);
			pdFreq++;
		}
		SafeArrayUnaccessData(TitlesArray);
		return hr;
}
HRESULT ExcelOutput::PlotICTest(
	const Waterfall& source,
	const QString& DestinationSheet,
	int DestinationIndex,
	const QDate& CallDate
	){
    LOGDEBUG("Reached PlotICTest");
		ExcelCommons::InitExcelOLE();
		SAFEARRAYBOUND  Bound[2];
		Bound[0].lLbound   = 1;
		Bound[0].cElements = source.GetTranche(0)->GetCashFlow().Count();
		Bound[1].lLbound   = 1;
		Bound[1].cElements = source.GetTranchesCount();
		VARIANT HUGEP *pdFreq;
		SAFEARRAY* DatesArray = SafeArrayCreate(VT_VARIANT, 1, Bound);
		HRESULT hr = SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&pdFreq);
		if (SUCCEEDED(hr))
		{
			for (int i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++){
				pdFreq->vt = VT_BSTR;
				pdFreq->bstrVal = SysAllocString(source.GetTranche(0)->GetCashFlow().GetDate(i).toString("yyyy-MM-dd").toStdWString().c_str());
				pdFreq++;
			}
			SafeArrayUnaccessData(DatesArray);
		}
		SAFEARRAY* OutstandingArray = SafeArrayCreate(VT_VARIANT, 2, Bound);
		hr = SafeArrayAccessData(OutstandingArray, (void HUGEP* FAR*)&pdFreq);
		if (SUCCEEDED(hr))
		{
			for (int j=0;j<source.GetTranchesCount();j++){
				for (int i = 0; i < source.GetTranche(j)->GetCashFlow().Count(); i++){
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetTranche(j)->GetCashFlow().GetICTest(i);
					pdFreq++;
				}
			}
			SafeArrayUnaccessData(OutstandingArray);
		}
		SAFEARRAY* TitlesArray = SafeArrayCreate(VT_VARIANT, 1, Bound+1);
		hr = SafeArrayAccessData(TitlesArray, (void HUGEP* FAR*)&pdFreq);
		if (SUCCEEDED(hr)){
			for (int j=0;j<source.GetTranchesCount();j++){
				pdFreq->vt = VT_BSTR;
				pdFreq->bstrVal = SysAllocString(source.GetTranche(j)->GetTrancheName().toStdWString().c_str());
				pdFreq++;
			}
		}

		static DISPID dispid = 0;
		DISPPARAMS Params;
		VARIANTARG Command[7];
		int CurrentCmdIndex=7-1;
		if(!ExcelCommons::pExcelDisp)return S_FALSE;
		try
		{
			Command[CurrentCmdIndex].vt = VT_BSTR;
			Command[CurrentCmdIndex--].bstrVal = SysAllocString(L"PlotICTest");
			Command[CurrentCmdIndex].vt = VT_BSTR;
			Command[CurrentCmdIndex--].bstrVal = SysAllocString(DestinationSheet.toStdWString().c_str());
			Command[CurrentCmdIndex].vt = VT_I4;
			Command[CurrentCmdIndex--].intVal = DestinationIndex;
			Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
			Command[CurrentCmdIndex--].parray = DatesArray;
			Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
			Command[CurrentCmdIndex--].parray = OutstandingArray;
			Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
			Command[CurrentCmdIndex--].parray = TitlesArray;
			Command[CurrentCmdIndex].vt = VT_BSTR;
			Command[CurrentCmdIndex--].bstrVal = SysAllocString(CallDate.isNull() ? L"" : CallDate.toString("yyyy-MM-dd").toStdWString().c_str());

			Params.rgdispidNamedArgs = NULL;
			Params.rgvarg=Command;
			Params.cArgs = 7;
			Params.cNamedArgs = 0;
			if(dispid == 0)
			{
                LPOLESTR ucName = SysAllocString(L"Run");
				hr = ExcelCommons::pExcelDisp->GetIDsOfNames(IID_NULL, &ucName, 1,
					LOCALE_SYSTEM_DEFAULT, &dispid);
                SysFreeString(ucName);
				if(FAILED(hr))
				{
					SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
					SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
					SysFreeString(Params.rgvarg[Params.cArgs-7].bstrVal);
					SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&pdFreq);
                    for (int i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++)
					{
						SysFreeString(pdFreq->bstrVal);
						pdFreq++;
					}
					SafeArrayUnaccessData(DatesArray);
					SafeArrayAccessData(TitlesArray, (void HUGEP* FAR*)&pdFreq);
                    for (int i = 0; i < source.GetTranchesCount(); i++)
					{
						SysFreeString(pdFreq->bstrVal);
						pdFreq++;
					}
					SafeArrayUnaccessData(TitlesArray);
					return hr;
				}
			}
			hr = ExcelCommons::pExcelDisp->Invoke(dispid,IID_NULL,LOCALE_SYSTEM_DEFAULT,
				DISPATCH_METHOD, &Params, NULL, NULL, NULL);
			if(FAILED(hr))
			{
				SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-7].bstrVal);
				SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&pdFreq);
                for (int i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++)
				{
					SysFreeString(pdFreq->bstrVal);
					pdFreq++;
				}
				SafeArrayUnaccessData(DatesArray);
				SafeArrayAccessData(TitlesArray, (void HUGEP* FAR*)&pdFreq);
                for (int i = 0; i < source.GetTranchesCount(); i++)
				{
					SysFreeString(pdFreq->bstrVal);
					pdFreq++;
				}
				SafeArrayUnaccessData(TitlesArray);
				return hr;
			}
		}
		catch(_com_error &ce)
		{
			hr = ce.Error();
            return hr;
		}
		SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
		SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
		SysFreeString(Params.rgvarg[Params.cArgs-7].bstrVal);
		SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&pdFreq);
        for (int i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++)
		{
			SysFreeString(pdFreq->bstrVal);
			pdFreq++;
		}
		SafeArrayUnaccessData(DatesArray);
		SafeArrayAccessData(TitlesArray, (void HUGEP* FAR*)&pdFreq);
        for (int i = 0; i < source.GetTranchesCount(); i++)
		{
			SysFreeString(pdFreq->bstrVal);
			pdFreq++;
		}
		SafeArrayUnaccessData(TitlesArray);
		return hr;
}
HRESULT ExcelOutput::PlotAnnualExcess(
	const Waterfall& source,
	const QString& DestinationSheet,
	int DestinationIndex,
	const QDate& CallDate
	){
    LOGDEBUG("Reached PlotAnnualExcess");
		ExcelCommons::InitExcelOLE();
		SAFEARRAYBOUND  Bound[1];
		Bound[0].lLbound   = 1;
		Bound[0].cElements = source.GetTranche(0)->GetCashFlow().Count();
		VARIANT HUGEP *pdFreq;
		SAFEARRAY* DatesArray = SafeArrayCreate(VT_VARIANT, 1, Bound);
		HRESULT hr = SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&pdFreq);
		if (SUCCEEDED(hr))
		{
			for (int i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++){
				pdFreq->vt = VT_BSTR;
				pdFreq->bstrVal = SysAllocString(source.GetTranche(0)->GetCashFlow().GetDate(i).toString("yyyy-MM-dd").toStdWString().c_str());
				pdFreq++;
			}
			SafeArrayUnaccessData(DatesArray);
		}
		SAFEARRAY* OutstandingArray = SafeArrayCreate(VT_VARIANT, 1, Bound);
		hr = SafeArrayAccessData(OutstandingArray, (void HUGEP* FAR*)&pdFreq);
		if (SUCCEEDED(hr))
		{
			for (int i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++){
				pdFreq->vt = VT_R8;
				pdFreq->dblVal = source.GetAnnualizedExcess(i);
				pdFreq++;
			}
			SafeArrayUnaccessData(OutstandingArray);
		}

		static DISPID dispid = 0;
		DISPPARAMS Params;
		VARIANTARG Command[6];
		int CurrentCmdIndex=6-1;
		if(!ExcelCommons::pExcelDisp)return S_FALSE;
		try
		{
			Command[CurrentCmdIndex].vt = VT_BSTR;
			Command[CurrentCmdIndex--].bstrVal = SysAllocString(L"PlotAnnualExcess");
			Command[CurrentCmdIndex].vt = VT_BSTR;
			Command[CurrentCmdIndex--].bstrVal = SysAllocString(DestinationSheet.toStdWString().c_str());
			Command[CurrentCmdIndex].vt = VT_I4;
			Command[CurrentCmdIndex--].intVal = DestinationIndex;
			Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
			Command[CurrentCmdIndex--].parray = DatesArray;
			Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
			Command[CurrentCmdIndex--].parray = OutstandingArray;
			Command[CurrentCmdIndex].vt = VT_BSTR;
			Command[CurrentCmdIndex--].bstrVal = SysAllocString(CallDate.isNull() ? L"" : CallDate.toString("yyyy-MM-dd").toStdWString().c_str());

			Params.rgdispidNamedArgs = NULL;
			Params.rgvarg=Command;
			Params.cArgs = 6;
			Params.cNamedArgs = 0;
			if(dispid == 0)
			{
                LPOLESTR ucName = SysAllocString(L"Run");
				hr = ExcelCommons::pExcelDisp->GetIDsOfNames(IID_NULL, &ucName, 1,
					LOCALE_SYSTEM_DEFAULT, &dispid);
                SysFreeString(ucName);
				if(FAILED(hr))
				{
					SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
					SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
					SysFreeString(Params.rgvarg[Params.cArgs-6].bstrVal);
					SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&pdFreq);
                    for (int i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++)
					{
						SysFreeString(pdFreq->bstrVal);
						pdFreq++;
					}
					SafeArrayUnaccessData(DatesArray);
					return hr;
				}
			}
			hr = ExcelCommons::pExcelDisp->Invoke(dispid,IID_NULL,LOCALE_SYSTEM_DEFAULT,
				DISPATCH_METHOD, &Params, NULL, NULL, NULL);
			if(FAILED(hr))
			{
				SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-6].bstrVal);
				SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&pdFreq);
                for (int i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++)
				{
					SysFreeString(pdFreq->bstrVal);
					pdFreq++;
				}
				SafeArrayUnaccessData(DatesArray);
				return hr;
			}
		}
		catch(_com_error &ce)
		{
			hr = ce.Error();
            return hr;
		}
		SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
		SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
		SysFreeString(Params.rgvarg[Params.cArgs-6].bstrVal);
		SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&pdFreq);
        for (int i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++)
		{
			SysFreeString(pdFreq->bstrVal);
			pdFreq++;
		}
		SafeArrayUnaccessData(DatesArray);
		return hr;
}
HRESULT ExcelOutput::PlotCostFunding(
	const Waterfall& source,
	const QString& DestinationSheet,
	int DestinationIndex,
	const QDate& CallDate
	){
    LOGDEBUG("Reached PlotCostFunding");
		ExcelCommons::InitExcelOLE();
		SAFEARRAYBOUND  Bound[1];
		Bound[0].lLbound   = 1;
		Bound[0].cElements = source.GetTranche(0)->GetCashFlow().Count();
		VARIANT HUGEP *DatesIter;
		VARIANT HUGEP *CostFundingIter;
		VARIANT HUGEP *LoansCouponIter;
		SAFEARRAY* DatesArray = SafeArrayCreate(VT_VARIANT, 1, Bound);
		SAFEARRAY* CostFundingArray = SafeArrayCreate(VT_VARIANT, 1, Bound);
		SAFEARRAY* LoansCouponArray = SafeArrayCreate(VT_VARIANT, 1, Bound);
		HRESULT hr = SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&DatesIter);
		if (SUCCEEDED(hr)) hr = SafeArrayAccessData(CostFundingArray, (void HUGEP* FAR*)&CostFundingIter);
        else {
            SafeArrayUnaccessData(DatesArray);
            return hr;
        }
		if (SUCCEEDED(hr)) hr = SafeArrayAccessData(LoansCouponArray, (void HUGEP* FAR*)&LoansCouponIter);
		else{
			SafeArrayUnaccessData(DatesArray);
			SafeArrayUnaccessData(CostFundingArray);
            return hr;
		}
		if (SUCCEEDED(hr))
		{
			double TempCalc;
			for (int i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++){
				DatesIter->vt = VT_BSTR;
				DatesIter->bstrVal = SysAllocString(source.GetTranche(0)->GetCashFlow().GetDate(i).toString("yyyy-MM-dd").toStdWString().c_str());
				TempCalc= source.GetWACostOfCapital(i);
				if(TempCalc>0.0){
					CostFundingIter->vt = VT_R8;
					CostFundingIter->dblVal = TempCalc;
				}
				TempCalc = source.GetCalculatedMtgPayments().GetWAcoupon(QDate(source.GetTranche(0)->GetCashFlow().GetDate(i).year(), source.GetTranche(0)->GetCashFlow().GetDate(i).month(),15));
				if(TempCalc>0.0){
					LoansCouponIter->vt = VT_R8;
					LoansCouponIter->dblVal = TempCalc;
				}
				DatesIter++;
				CostFundingIter++;
				LoansCouponIter++;
			}
			SafeArrayUnaccessData(DatesArray);
			SafeArrayUnaccessData(CostFundingArray);
			SafeArrayUnaccessData(LoansCouponArray);
		}

		static DISPID dispid = 0;
		DISPPARAMS Params;
		VARIANTARG Command[7];
		int CurrentCmdIndex=7-1;
		if(!ExcelCommons::pExcelDisp)return S_FALSE;
		try
		{
			Command[CurrentCmdIndex].vt = VT_BSTR;
			Command[CurrentCmdIndex--].bstrVal = SysAllocString(L"PlotCostFunding");
			Command[CurrentCmdIndex].vt = VT_BSTR;
			Command[CurrentCmdIndex--].bstrVal = SysAllocString(DestinationSheet.toStdWString().c_str());
			Command[CurrentCmdIndex].vt = VT_I4;
			Command[CurrentCmdIndex--].intVal = DestinationIndex;
			Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
			Command[CurrentCmdIndex--].parray = DatesArray;
			Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
			Command[CurrentCmdIndex--].parray = CostFundingArray;
			Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
			Command[CurrentCmdIndex--].parray = LoansCouponArray;
			Command[CurrentCmdIndex].vt = VT_BSTR;
			Command[CurrentCmdIndex--].bstrVal = SysAllocString(CallDate.isNull() ? L"":CallDate.toString("yyyy-MM-dd").toStdWString().c_str());

			Params.rgdispidNamedArgs = NULL;
			Params.rgvarg=Command;
			Params.cArgs = 7;
			Params.cNamedArgs = 0;
			if(dispid == 0)
			{
                LPOLESTR ucName = SysAllocString(L"Run");
				hr = ExcelCommons::pExcelDisp->GetIDsOfNames(IID_NULL, &ucName, 1,
					LOCALE_SYSTEM_DEFAULT, &dispid);
                SysFreeString(ucName);
				if(FAILED(hr))
				{
					SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
					SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
					SysFreeString(Params.rgvarg[Params.cArgs-7].bstrVal);
					SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&DatesIter);
                    for (int i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++)
					{
						SysFreeString(DatesIter->bstrVal);
						DatesIter++;
					}
					SafeArrayUnaccessData(DatesArray);
					return hr;
				}
			}
			hr = ExcelCommons::pExcelDisp->Invoke(dispid,IID_NULL,LOCALE_SYSTEM_DEFAULT,
				DISPATCH_METHOD, &Params, NULL, NULL, NULL);
			if(FAILED(hr))
			{
				SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-7].bstrVal);
				SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&DatesIter);
                for (int i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++)
				{
					SysFreeString(DatesIter->bstrVal);
					DatesIter++;
				}
				SafeArrayUnaccessData(DatesArray);
				return hr;
			}
		}
		catch(_com_error &ce)
		{
			hr = ce.Error();
            return hr;
		}
		SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
		SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
		SysFreeString(Params.rgvarg[Params.cArgs-7].bstrVal);
		SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&DatesIter);
        for (int i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++)
		{
			SysFreeString(DatesIter->bstrVal);
			DatesIter++;
		}
		SafeArrayUnaccessData(DatesArray);
		return hr;
}

HRESULT ExcelOutput::PlotEquityReturn(
	const Waterfall& source,
	const QString& DestinationSheet,
	int DestinationIndex
	){
    LOGDEBUG("Reached PlotEquityReturn");
		ExcelCommons::InitExcelOLE();
		SAFEARRAYBOUND  Bound[1];
		Bound[0].lLbound   = 1;
		Bound[0].cElements = source.GetTranche(0)->GetCashFlow().Count();
		VARIANT HUGEP *DatesIter;
		VARIANT HUGEP *EquityRetIter;
		VARIANT HUGEP *CumEquityRetIter;
		SAFEARRAY* DatesArray = SafeArrayCreate(VT_VARIANT, 1, Bound);
		SAFEARRAY* EquityRetArray = SafeArrayCreate(VT_VARIANT, 1, Bound);
		SAFEARRAY* CumEquityRetArray = SafeArrayCreate(VT_VARIANT, 1, Bound);
		HRESULT hr = SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&DatesIter);
		if (SUCCEEDED(hr)) hr = SafeArrayAccessData(EquityRetArray, (void HUGEP* FAR*)&EquityRetIter);
        else { 
            SafeArrayUnaccessData(DatesArray); 
            return hr; 
        }
		if (SUCCEEDED(hr)) hr = SafeArrayAccessData(CumEquityRetArray, (void HUGEP* FAR*)&CumEquityRetIter);
		else {
            SafeArrayUnaccessData(DatesArray); 
            SafeArrayUnaccessData(EquityRetArray);
            return hr;
        }
		if (SUCCEEDED(hr))
		{
			for (int i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++){
				DatesIter->vt = VT_BSTR;
				EquityRetIter->vt = VT_R8;
				CumEquityRetIter->vt = VT_R8;
				DatesIter->bstrVal = SysAllocString(source.GetTranche(0)->GetCashFlow().GetDate(i).toString("yyyy-MM-dd").toStdWString().c_str());
				EquityRetIter->dblVal = source.GetEquityReturn(i);
				CumEquityRetIter->dblVal = source.GetCumulativeEquityReturn(i);
				DatesIter++;
				EquityRetIter++;
				CumEquityRetIter++;
			}
			SafeArrayUnaccessData(DatesArray);
			SafeArrayUnaccessData(EquityRetArray);
			SafeArrayUnaccessData(CumEquityRetArray);
		}

		static DISPID dispid = 0;
		DISPPARAMS Params;
		VARIANTARG Command[6];
		int CurrentCmdIndex=6-1;
		if(!ExcelCommons::pExcelDisp)return S_FALSE;
		try
		{
			Command[CurrentCmdIndex].vt = VT_BSTR;
			Command[CurrentCmdIndex--].bstrVal = SysAllocString(L"PlotEquityReturn");
			Command[CurrentCmdIndex].vt = VT_BSTR;
			Command[CurrentCmdIndex--].bstrVal = SysAllocString(DestinationSheet.toStdWString().c_str());
			Command[CurrentCmdIndex].vt = VT_I4;
			Command[CurrentCmdIndex--].intVal = DestinationIndex;
			Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
			Command[CurrentCmdIndex--].parray = DatesArray;
			Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
			Command[CurrentCmdIndex--].parray = EquityRetArray;
			Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
			Command[CurrentCmdIndex--].parray = CumEquityRetArray;

			Params.rgdispidNamedArgs = NULL;
			Params.rgvarg=Command;
			Params.cArgs = 6;
			Params.cNamedArgs = 0;
			if(dispid == 0)
			{
                LPOLESTR ucName = SysAllocString(L"Run");
				hr = ExcelCommons::pExcelDisp->GetIDsOfNames(IID_NULL, &ucName, 1,
					LOCALE_SYSTEM_DEFAULT, &dispid);
                SysFreeString(ucName);
				if(FAILED(hr))
				{
					SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
					SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
					SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&DatesIter);
                    for (int i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++)
					{
						SysFreeString(DatesIter->bstrVal);
						DatesIter++;
					}
					SafeArrayUnaccessData(DatesArray);
					return hr;
				}
			}
			hr = ExcelCommons::pExcelDisp->Invoke(dispid,IID_NULL,LOCALE_SYSTEM_DEFAULT,
				DISPATCH_METHOD, &Params, NULL, NULL, NULL);
			if(FAILED(hr))
			{
				SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
				SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&DatesIter);
                for (int i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++)
				{
					SysFreeString(DatesIter->bstrVal);
					DatesIter++;
				}
				SafeArrayUnaccessData(DatesArray);
				return hr;
			}
		}
		catch(_com_error &ce)
		{
			hr = ce.Error();
            return hr;
		}
		SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
		SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
		SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&DatesIter);
        for (int i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++)
		{
			SysFreeString(DatesIter->bstrVal);
			DatesIter++;
		}
		SafeArrayUnaccessData(DatesArray);
		return hr;
}
HRESULT ExcelOutput::PlotCallToEquity(
	const Waterfall& source,
	const QString& DestinationSheet,
	int DestinationIndex,
	const QDate& CallDate
	){
    LOGDEBUG("Reached PlotCallToEquity");
		ExcelCommons::InitExcelOLE();
		SAFEARRAYBOUND  Bound[1];
		Bound[0].lLbound   = 1;
		Bound[0].cElements = source.GetTranche(0)->GetCashFlow().Count();
		VARIANT HUGEP *pdFreq;
		VARIANT HUGEP *pdCallEqtIter;
		SAFEARRAY* DatesArray = SafeArrayCreate(VT_VARIANT, 1, Bound);
		SAFEARRAY* allEqtArray = SafeArrayCreate(VT_VARIANT, 1, Bound);
		HRESULT hr = SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&pdFreq);
		if (SUCCEEDED(hr)) hr = SafeArrayAccessData(allEqtArray, (void HUGEP* FAR*)&pdCallEqtIter);
        else {
            SafeArrayUnaccessData(DatesArray);
            return hr;
        }
		if (SUCCEEDED(hr))
		{
			for (int i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++){
				pdFreq->vt = VT_BSTR;
				pdCallEqtIter->vt = VT_R8;
				pdFreq->bstrVal = SysAllocString(source.GetTranche(0)->GetCashFlow().GetDate(i).toString("yyyy-MM-dd").toStdWString().c_str());
				pdCallEqtIter->dblVal = source.GetCallEquityRatio(i);
				pdFreq++;
				pdCallEqtIter++;
			}
			SafeArrayUnaccessData(DatesArray);
			SafeArrayUnaccessData(allEqtArray);
		}

		static DISPID dispid = 0;
		DISPPARAMS Params;
		VARIANTARG Command[6];
		int CurrentCmdIndex=6-1;
		if(!ExcelCommons::pExcelDisp)return S_FALSE;
		try
		{
			Command[CurrentCmdIndex].vt = VT_BSTR;
			Command[CurrentCmdIndex--].bstrVal = SysAllocString(L"PlotCallToEquity");
			Command[CurrentCmdIndex].vt = VT_BSTR;
			Command[CurrentCmdIndex--].bstrVal = SysAllocString(DestinationSheet.toStdWString().c_str());
			Command[CurrentCmdIndex].vt = VT_I4;
			Command[CurrentCmdIndex--].intVal = DestinationIndex;
			Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
			Command[CurrentCmdIndex--].parray = DatesArray;
			Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
			Command[CurrentCmdIndex--].parray = allEqtArray;
			Command[CurrentCmdIndex].vt = VT_BSTR;
			Command[CurrentCmdIndex--].bstrVal = SysAllocString(CallDate.isNull() ? L"":CallDate.toString("yyyy-MM-dd").toStdWString().c_str());

			Params.rgdispidNamedArgs = NULL;
			Params.rgvarg=Command;
			Params.cArgs = 6;
			Params.cNamedArgs = 0;
			if(dispid == 0)
			{
                LPOLESTR ucName = SysAllocString(L"Run");
				hr = ExcelCommons::pExcelDisp->GetIDsOfNames(IID_NULL, &ucName, 1,
					LOCALE_SYSTEM_DEFAULT, &dispid);
                SysFreeString(ucName);
				if(FAILED(hr))
				{
					SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
					SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
					SysFreeString(Params.rgvarg[Params.cArgs-6].bstrVal);
					SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&pdFreq);
                    for (int i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++)
					{
						SysFreeString(pdFreq->bstrVal);
						pdFreq++;
					}
					SafeArrayUnaccessData(DatesArray);
					return hr;
				}
			}
			hr = ExcelCommons::pExcelDisp->Invoke(dispid,IID_NULL,LOCALE_SYSTEM_DEFAULT,
				DISPATCH_METHOD, &Params, NULL, NULL, NULL);
			if(FAILED(hr))
			{
				SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-6].bstrVal);
				SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&pdFreq);
                for (int i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++)
				{
					SysFreeString(pdFreq->bstrVal);
					pdFreq++;
				}
				SafeArrayUnaccessData(DatesArray);
				return hr;
			}
		}
		catch(_com_error &ce)
		{
			hr = ce.Error();
            return hr;
		}
		SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
		SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
		SysFreeString(Params.rgvarg[Params.cArgs-6].bstrVal);
		SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&pdFreq);
        for (int i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++)
		{
			SysFreeString(pdFreq->bstrVal);
			pdFreq++;
		}
		SafeArrayUnaccessData(DatesArray);
		return hr;
}
HRESULT ExcelOutput::PlotCPRLS(
	const Waterfall& source,
	const QString& DestinationSheet,
	int DestinationIndex
	){
		ExcelCommons::InitExcelOLE();
		SAFEARRAYBOUND  Bound[1];
		Bound[0].lLbound   = 1;
		Bound[0].cElements = source.GetCalculatedMtgPayments().Count()-1;
		VARIANT HUGEP *DatesIter;
		VARIANT HUGEP *CPRIter;
		VARIANT HUGEP *LSIter;
		SAFEARRAY* DatesArray = SafeArrayCreate(VT_VARIANT, 1, Bound);
		SAFEARRAY* CPRArray = SafeArrayCreate(VT_VARIANT, 1, Bound);
		SAFEARRAY* LSArray = SafeArrayCreate(VT_VARIANT, 1, Bound);
		HRESULT hr = SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&DatesIter);
		if (SUCCEEDED(hr))  hr = SafeArrayAccessData(CPRArray, (void HUGEP* FAR*)&CPRIter);
		else {
            SafeArrayUnaccessData(DatesArray);
            return hr;
        }
		if (SUCCEEDED(hr))  hr = SafeArrayAccessData(LSArray, (void HUGEP* FAR*)&LSIter);
		else{
			SafeArrayUnaccessData(DatesArray);
			SafeArrayUnaccessData(CPRArray);
            return hr;
		}
		if (SUCCEEDED(hr))
		{
			double TempDbl;
			for (int i = 1; i < source.GetCalculatedMtgPayments().Count(); i++){
				DatesIter->vt = VT_BSTR;
				CPRIter->vt = VT_R8;
				LSIter->vt = VT_R8;
				DatesIter->bstrVal = SysAllocString(source.GetCalculatedMtgPayments().GetDate(i).toString("yyyy-MM-dd").toStdWString().c_str());
				TempDbl= 1.0-qPow(1.0-(source.GetCalculatedMtgPayments().GetPrepay(i)/(source.GetCalculatedMtgPayments().GetAmountOut(i-1)-source.GetCalculatedMtgPayments().GetScheduled(i))),12.0);
				if(TempDbl<=1.00){
					CPRIter->vt = VT_R8;
					CPRIter->dblVal = TempDbl;
				}
                TempDbl = 1.0 - qPow(1.0 - (source.GetCalculatedMtgPayments().GetLoss(i) / (source.GetCalculatedMtgPayments().GetAmountOut(i - 1) - source.GetCalculatedMtgPayments().GetScheduled(i) /*- source.GetCalculatedMtgPayments().GetPrepay(i)*/)), 12.0);
				if(TempDbl<=1.00){
					LSIter->vt = VT_R8;
					LSIter->dblVal =  TempDbl;
				}
				DatesIter++;
				CPRIter++;
				LSIter++;
			}
			SafeArrayUnaccessData(DatesArray);
			SafeArrayUnaccessData(CPRArray);
			SafeArrayUnaccessData(LSArray);
		}
        
		static DISPID dispid = 0;
		DISPPARAMS Params;
		VARIANTARG Command[6];
		int CurrentCmdIndex=6-1;
		if(!ExcelCommons::pExcelDisp)return S_FALSE;
		try
		{
			Command[CurrentCmdIndex].vt = VT_BSTR;
			Command[CurrentCmdIndex--].bstrVal = SysAllocString(L"PlotCPRLS");
			Command[CurrentCmdIndex].vt = VT_BSTR;
			Command[CurrentCmdIndex--].bstrVal = SysAllocString(DestinationSheet.toStdWString().c_str());
			Command[CurrentCmdIndex].vt = VT_I4;
			Command[CurrentCmdIndex--].intVal = DestinationIndex;
			Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
			Command[CurrentCmdIndex--].parray = DatesArray;
			Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
			Command[CurrentCmdIndex--].parray = CPRArray;
			Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
			Command[CurrentCmdIndex--].parray = LSArray;
			Params.rgdispidNamedArgs = NULL;
			Params.rgvarg=Command;
			Params.cArgs = 6;
			Params.cNamedArgs = 0;
			if(dispid == 0)
			{
                LPOLESTR ucName = SysAllocString(L"Run");
				hr = ExcelCommons::pExcelDisp->GetIDsOfNames(IID_NULL, &ucName, 1,
					LOCALE_SYSTEM_DEFAULT, &dispid);
                SysFreeString(ucName);
				if(FAILED(hr))
				{
					SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
					SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
					SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&DatesIter);
                    for (int i = 1; i < source.GetCalculatedMtgPayments().Count(); i++)
					{
						SysFreeString(DatesIter->bstrVal);
						DatesIter++;
					}
					SafeArrayUnaccessData(DatesArray);
					return hr;
				}
			}
			hr = ExcelCommons::pExcelDisp->Invoke(dispid,IID_NULL,LOCALE_SYSTEM_DEFAULT,
				DISPATCH_METHOD, &Params, NULL, NULL, NULL);
			if(FAILED(hr))
			{
                LOGDEBUG("Invoke Failed PlotCPRLS");
				SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
				SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&DatesIter);
                for (int i = 1; i < source.GetCalculatedMtgPayments().Count(); i++)
				{
					SysFreeString(DatesIter->bstrVal);
					DatesIter++;
				}
				SafeArrayUnaccessData(DatesArray);
				return hr;
			}
		}
		catch(_com_error &ce)
		{
            LOGDEBUG("Exception trown PlotCPRLS");
			hr = ce.Error();
            return hr;
		}
		SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
		SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
		SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&DatesIter);
        for (int i = 1; i < source.GetCalculatedMtgPayments().Count(); i++)
		{
			SysFreeString(DatesIter->bstrVal);
			DatesIter++;
		}
        
		SafeArrayUnaccessData(DatesArray);
		return hr;
}

/*HRESULT ExcelOutput::PlotCostFunding(
	const Waterfall& source,
	const QString& DestinationSheet,
	int DestinationIndex,
	const QDate& CallDate
	){
		ExcelCommons::InitExcelOLE();
		SAFEARRAYBOUND  Bound[1];
		Bound[0].lLbound   = 1;
		Bound[0].cElements = source.GetTranche(0)->GetCashFlow().Count();
		VARIANT HUGEP *pdFreq;
		SAFEARRAY* DatesArray = SafeArrayCreate(VT_VARIANT, 1, Bound);
		HRESULT hr = SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&pdFreq);
		if (SUCCEEDED(hr))
		{
			for (int i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++){
				pdFreq->vt = VT_BSTR;
				pdFreq->bstrVal = SysAllocString(source.GetTranche(0)->GetCashFlow().GetDate(i).toString("yyyy-MM-dd").toStdWString().c_str());
				pdFreq++;
			}
			SafeArrayUnaccessData(DatesArray);
		}
		SAFEARRAY* OutstandingArray = SafeArrayCreate(VT_VARIANT, 1, Bound);
		hr = SafeArrayAccessData(OutstandingArray, (void HUGEP* FAR*)&pdFreq);
		if (SUCCEEDED(hr))
		{
			for (int i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++){
				if(source.GetWACostOfCapital(i)>0){
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetWACostOfCapital(i);
				}
				pdFreq++;
			}
			SafeArrayUnaccessData(OutstandingArray);
		}

		static DISPID dispid = 0;
		DISPPARAMS Params;
		VARIANTARG Command[6];
		int CurrentCmdIndex=6-1;
		if(!ExcelCommons::pExcelDisp)return S_FALSE;
		try
		{
			Command[CurrentCmdIndex].vt = VT_BSTR;
			Command[CurrentCmdIndex--].bstrVal = SysAllocString(L"PlotCostFunding");
			Command[CurrentCmdIndex].vt = VT_BSTR;
			Command[CurrentCmdIndex--].bstrVal = SysAllocString(DestinationSheet.toStdWString().c_str());
			Command[CurrentCmdIndex].vt = VT_I4;
			Command[CurrentCmdIndex--].intVal = DestinationIndex;
			Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
			Command[CurrentCmdIndex--].parray = DatesArray;
			Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
			Command[CurrentCmdIndex--].parray = OutstandingArray;
			Command[CurrentCmdIndex].vt = VT_BSTR;
			Command[CurrentCmdIndex--].bstrVal = SysAllocString(CallDate.isNull() ? L"":CallDate.toString("yyyy-MM-dd").toStdWString().c_str());

			Params.rgdispidNamedArgs = NULL;
			Params.rgvarg=Command;
			Params.cArgs = 6;
			Params.cNamedArgs = 0;
			if(dispid == 0)
			{
				wchar_t *ucName = L"Run";
				hr = ExcelCommons::pExcelDisp->GetIDsOfNames(IID_NULL, &ucName, 1,
					LOCALE_SYSTEM_DEFAULT, &dispid);
				if(FAILED(hr))
				{
					SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
					SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
					SysFreeString(Params.rgvarg[Params.cArgs-6].bstrVal);
					SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&pdFreq);
					for (DWORD i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++)
					{
						SysFreeString(pdFreq->bstrVal);
						pdFreq++;
					}
					SafeArrayUnaccessData(DatesArray);
					return hr;
				}
			}
			hr = ExcelCommons::pExcelDisp->Invoke(dispid,IID_NULL,LOCALE_SYSTEM_DEFAULT,
				DISPATCH_METHOD, &Params, NULL, NULL, NULL);
			if(FAILED(hr))
			{
				SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-6].bstrVal);
				SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&pdFreq);
				for (DWORD i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++)
				{
					SysFreeString(pdFreq->bstrVal);
					pdFreq++;
				}
				SafeArrayUnaccessData(DatesArray);
				return hr;
			}
		}
		catch(_com_error &ce)
		{
			hr = ce.Error();
		}
		SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
		SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
		SysFreeString(Params.rgvarg[Params.cArgs-6].bstrVal);
		SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&pdFreq);
		for (DWORD i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++)
		{
			SysFreeString(pdFreq->bstrVal);
			pdFreq++;
		}
		SafeArrayUnaccessData(DatesArray);
		return hr;
}*/
