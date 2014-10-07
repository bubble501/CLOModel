#include "ExcelOutput.h"
#include "Tranche.h"
#include "MtgCashFlow.h"
#include "StressTest.h"
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
	bool PrintLossOnInterest
	){
		ExcelCommons::InitExcelOLE();
		int NumOfCols=
			PrintCounter+PrintDates+PrintOutstanding+PrintInterest+PrintScheduled+PrintPrepay+PrintAccruedInterest+PrintTotalPrincipal+PrintTotalPrincipal+PrintLoss+PrintLossOnInterest+PrintDefaults;
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
					pdFreq->bstrVal = SysAllocString(source.GetDate(i).toString("yyyy-MM-dd").toStdWString().c_str());
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
			SafeArrayUnaccessData(saData);
		}
		static DISPID dispid = 0;
		DISPPARAMS Params;
		VARIANTARG Command[14];
		int CurrentCmdIndex=14-1;
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

			Params.rgdispidNamedArgs = NULL;
			Params.rgvarg=Command;
			Params.cArgs = 14;
			Params.cNamedArgs = 0;
			if(dispid == 0)
			{
				wchar_t *ucName = L"Run";
				hr = ExcelCommons::pExcelDisp->GetIDsOfNames(IID_NULL, &ucName, 1,
					LOCALE_SYSTEM_DEFAULT, &dispid);
				if(FAILED(hr))
				{
					SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
					SysFreeString(Params.rgvarg[Params.cArgs-3].bstrVal);
					if(PrintDates){
						SafeArrayAccessData(saData, (void HUGEP* FAR*)&pdFreq);
						if(PrintCounter) pdFreq+=Numrows;
						for (DWORD i = 0; i < Numrows; i++)
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
					for (DWORD i = 0; i < Numrows; i++)
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
		}
		SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
		SysFreeString(Params.rgvarg[Params.cArgs-3].bstrVal);
		if(PrintDates){
			SafeArrayAccessData(saData, (void HUGEP* FAR*)&pdFreq);
			if(PrintCounter) pdFreq+=Numrows;
			for (DWORD i = 0; i < Numrows; i++)
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
				wchar_t *ucName = L"Run";
				hr = ExcelCommons::pExcelDisp->GetIDsOfNames(IID_NULL, &ucName, 1,
					LOCALE_SYSTEM_DEFAULT, &dispid);
				if(FAILED(hr))
				{
					SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
					SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
					if(PrintDates){
						SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&pdFreq);
						for (DWORD i = 0; i < source.Count(); i++)
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
					for (DWORD i = 0; i < source.Count(); i++)
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
		}
		SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
		SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
		if(PrintDates){
			SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&pdFreq);
			for (DWORD i = 0; i < source.Count(); i++)
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
		SAFEARRAY* saData;
		saData = SafeArrayCreate(VT_VARIANT, 2, Bound);
		VARIANT HUGEP *pdFreq;
		HRESULT hr = SafeArrayAccessData(saData, (void HUGEP* FAR*)&pdFreq);
		if (SUCCEEDED(hr))
		{
			if(PrintDates){
				for (DWORD i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_BSTR;
					pdFreq->bstrVal = SysAllocString(source.GetCashFlow().GetDate(i).toString("yyyy-MM-dd").toStdWString().c_str());
					pdFreq++;
				}
			}
			if(PrintOutstanding){
				for (DWORD i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetCashFlow().GetAmountOutstanding(i);
					pdFreq++;
				}
			}
			if(PrintInterest){
				for (DWORD i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetCashFlow().GetInterest(i);
					pdFreq++;
				}
			}
			if(PrintPrincipal){
				for (DWORD i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetCashFlow().GetPrincipal(i);
					pdFreq++;
				}
			}
			if(PrintTotalFlow){
				for (DWORD i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetCashFlow().GetTotalFlow(i);
					pdFreq++;
				}
			}
			if(PrintDeferred){
				for (DWORD i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetCashFlow().GetDeferred(i);
					pdFreq++;
				}
			}
			if(PrintOCtest){
				for (DWORD i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetCashFlow().GetOCTest(i);
					pdFreq++;
				}
			}
			if(PrintICtest){
				for (DWORD i = 0; i < Numrows; i++)
				{
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetCashFlow().GetICTest(i);
					pdFreq++;
				}
			}
			if (PrintPDL) {
				for (DWORD i = 0; i < Numrows; i++) {
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetCashFlow().GetPDLOutstanding(i);
					pdFreq++;
				}
				for (DWORD i = 0; i < Numrows; i++) {
					pdFreq->vt = VT_R8;
					pdFreq->dblVal = source.GetCashFlow().GetPDLCured(i);
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
			Command[CurrentCmdIndex].vt = VT_R8;
			Command[CurrentCmdIndex--].dblVal=source.GetMinIClevel();
			Command[CurrentCmdIndex].vt = VT_R8;
			Command[CurrentCmdIndex--].dblVal=source.GetMinOClevel();

			
			Params.rgdispidNamedArgs = NULL;
			Params.rgvarg=Command;
			Params.cArgs = 16;
			Params.cNamedArgs = 0;
			if(dispid == 0)
			{
				wchar_t *ucName = L"Run";
				hr = ExcelCommons::pExcelDisp->GetIDsOfNames(IID_NULL, &ucName, 1,
					LOCALE_SYSTEM_DEFAULT, &dispid);
				if(FAILED(hr))
				{
					SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
					SysFreeString(Params.rgvarg[Params.cArgs-3].bstrVal);
					SysFreeString(Params.rgvarg[Params.cArgs-4].bstrVal);
					SysFreeString(Params.rgvarg[Params.cArgs-5].bstrVal);
					if(PrintDates){
						SafeArrayAccessData(saData, (void HUGEP* FAR*)&pdFreq);
						for (DWORD i = 0; i < Numrows; i++)
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
				SysFreeString(Params.rgvarg[Params.cArgs-4].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-5].bstrVal);
				if(PrintDates){
					SafeArrayAccessData(saData, (void HUGEP* FAR*)&pdFreq);
					for (DWORD i = 0; i < Numrows; i++)
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
		}
		SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
		SysFreeString(Params.rgvarg[Params.cArgs-3].bstrVal);
		SysFreeString(Params.rgvarg[Params.cArgs-4].bstrVal);
		SysFreeString(Params.rgvarg[Params.cArgs-5].bstrVal);
		if(PrintDates){
			SafeArrayAccessData(saData, (void HUGEP* FAR*)&pdFreq);
			for (DWORD i = 0; i < Numrows; i++)
			{
				SysFreeString(pdFreq->bstrVal);
				pdFreq++;
			}
			SafeArrayUnaccessData(saData);
		}
		return hr;
}


HRESULT ExcelOutput::PrintStressTest(const StressTest& stresser, const QString& TrancheTarget, const QString& DestinationAddress, bool SetupConditionalFormatting){
	if(!stresser.GetResults().begin().value().begin().value().GetTranche(TrancheTarget)) return S_FALSE;
	int Numrows=stresser.GetXSpann().size();
	int NumOfCols=stresser.GetYSpann().size();
	if(Numrows<=0 || Numrows<=0) return E_INVALIDARG;
	ExcelCommons::InitExcelOLE();
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
		for(int i=0;i<NumOfCols;i++){
			for(int j=0;j<Numrows;j++){
				pdFreq->vt = VT_R8;
				pdFreq->dblVal = stresser.GetResults().value(stresser.GetXSpann().at(j)).value(stresser.GetYSpann().at(i)).GetTranche(TrancheTarget)->GetLossRate();
				pdFreq++;
			}
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
		Command[CurrentCmdIndex--].bstrVal = SysAllocString(L"PrintStressTest");
		Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
		Command[CurrentCmdIndex--].parray = saData;
		Command[CurrentCmdIndex].vt = VT_BSTR;
		Command[CurrentCmdIndex--].bstrVal = SysAllocString(ExcelCommons::CellOffset(DestinationAddress,0,0).toStdWString().c_str());
		Command[CurrentCmdIndex].vt = VT_BOOL;
		Command[CurrentCmdIndex--].boolVal=SetupConditionalFormatting;
		Params.rgdispidNamedArgs = NULL;
		Params.rgvarg=Command;
		Params.cArgs = 4;
		Params.cNamedArgs = 0;
		if(dispid == 0)
		{
			wchar_t *ucName = L"Run";
			hr = ExcelCommons::pExcelDisp->GetIDsOfNames(IID_NULL, &ucName, 1,
				LOCALE_SYSTEM_DEFAULT, &dispid);
			if(FAILED(hr))
			{
				SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
				SysFreeString(Params.rgvarg[Params.cArgs-3].bstrVal);
				return hr;
			}
		}
		hr = ExcelCommons::pExcelDisp->Invoke(dispid,IID_NULL,LOCALE_SYSTEM_DEFAULT,
			DISPATCH_METHOD, &Params, NULL, NULL, NULL);
		if(FAILED(hr))
		{
			SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
			SysFreeString(Params.rgvarg[Params.cArgs-3].bstrVal);
			return hr;
		}
	}
	catch(_com_error &ce)
	{
		hr = ce.Error();
	}
	SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
	SysFreeString(Params.rgvarg[Params.cArgs-3].bstrVal);
	return hr;
}
HRESULT ExcelOutput::PrintMergedCell(const QString& msg, const QString& TargetCell, int RowDim, int ColDim,const QColor& FillColor){
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
			wchar_t *ucName = L"Run";
			hr = ExcelCommons::pExcelDisp->GetIDsOfNames(IID_NULL, &ucName, 1,
				LOCALE_SYSTEM_DEFAULT, &dispid);
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
	}
	SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
	SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
	SysFreeString(Params.rgvarg[Params.cArgs-3].bstrVal);
	SysFreeString(Params.rgvarg[Params.cArgs-6].bstrVal);
	return hr;
}
HRESULT ExcelOutput::PrintColumn(const QString& Title, const QList<double>& Values ,const QString& TargetCell,const QString& ValFormat,const QColor& FillColor){
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
			wchar_t *ucName = L"Run";
			hr = ExcelCommons::pExcelDisp->GetIDsOfNames(IID_NULL, &ucName, 1,
				LOCALE_SYSTEM_DEFAULT, &dispid);
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
	}
	SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
	SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
	SysFreeString(Params.rgvarg[Params.cArgs-4].bstrVal);
	SysFreeString(Params.rgvarg[Params.cArgs-5].bstrVal);
	SysFreeString(Params.rgvarg[Params.cArgs-6].bstrVal);
	return hr;
}
HRESULT ExcelOutput::PrintDataColumn(const QList<double>& Values ,const QString& TargetCell,const QString& ValFormat){
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
			wchar_t *ucName = L"Run";
			hr = ExcelCommons::pExcelDisp->GetIDsOfNames(IID_NULL, &ucName, 1,
				LOCALE_SYSTEM_DEFAULT, &dispid);
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
				wchar_t *ucName = L"Run";
				hr = ExcelCommons::pExcelDisp->GetIDsOfNames(IID_NULL, &ucName, 1,
					LOCALE_SYSTEM_DEFAULT, &dispid);
				if(FAILED(hr))
				{
					SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
					SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
					SysFreeString(Params.rgvarg[Params.cArgs-7].bstrVal);
					SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&pdFreq);
					for (DWORD i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++)
					{
						SysFreeString(pdFreq->bstrVal);
						pdFreq++;
					}
					SafeArrayUnaccessData(DatesArray);
					SafeArrayAccessData(TitlesArray, (void HUGEP* FAR*)&pdFreq);
					for (DWORD i = 0; i < source.GetTranchesCount(); i++)
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
				for (DWORD i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++)
				{
					SysFreeString(pdFreq->bstrVal);
					pdFreq++;
				}
				SafeArrayUnaccessData(DatesArray);
				SafeArrayAccessData(TitlesArray, (void HUGEP* FAR*)&pdFreq);
				for (DWORD i = 0; i < source.GetTranchesCount(); i++)
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
		}
		SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
		SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
		SysFreeString(Params.rgvarg[Params.cArgs-7].bstrVal);
		SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&pdFreq);
		for (DWORD i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++)
		{
			SysFreeString(pdFreq->bstrVal);
			pdFreq++;
		}
		SafeArrayUnaccessData(DatesArray);
		SafeArrayAccessData(TitlesArray, (void HUGEP* FAR*)&pdFreq);
		for (DWORD i = 0; i < source.GetTranchesCount(); i++)
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
				wchar_t *ucName = L"Run";
				hr = ExcelCommons::pExcelDisp->GetIDsOfNames(IID_NULL, &ucName, 1,
					LOCALE_SYSTEM_DEFAULT, &dispid);
				if(FAILED(hr))
				{
					SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
					SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
					SysFreeString(Params.rgvarg[Params.cArgs-7].bstrVal);
					SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&pdFreq);
					for (DWORD i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++)
					{
						SysFreeString(pdFreq->bstrVal);
						pdFreq++;
					}
					SafeArrayUnaccessData(DatesArray);
					SafeArrayAccessData(TitlesArray, (void HUGEP* FAR*)&pdFreq);
					for (DWORD i = 0; i < source.GetTranchesCount(); i++)
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
				for (DWORD i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++)
				{
					SysFreeString(pdFreq->bstrVal);
					pdFreq++;
				}
				SafeArrayUnaccessData(DatesArray);
				SafeArrayAccessData(TitlesArray, (void HUGEP* FAR*)&pdFreq);
				for (DWORD i = 0; i < source.GetTranchesCount(); i++)
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
		}
		SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
		SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
		SysFreeString(Params.rgvarg[Params.cArgs-7].bstrVal);
		SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&pdFreq);
		for (DWORD i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++)
		{
			SysFreeString(pdFreq->bstrVal);
			pdFreq++;
		}
		SafeArrayUnaccessData(DatesArray);
		SafeArrayAccessData(TitlesArray, (void HUGEP* FAR*)&pdFreq);
		for (DWORD i = 0; i < source.GetTranchesCount(); i++)
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
				wchar_t *ucName = L"Run";
				hr = ExcelCommons::pExcelDisp->GetIDsOfNames(IID_NULL, &ucName, 1,
					LOCALE_SYSTEM_DEFAULT, &dispid);
				if(FAILED(hr))
				{
					SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
					SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
					SysFreeString(Params.rgvarg[Params.cArgs-7].bstrVal);
					SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&pdFreq);
					for (DWORD i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++)
					{
						SysFreeString(pdFreq->bstrVal);
						pdFreq++;
					}
					SafeArrayUnaccessData(DatesArray);
					SafeArrayAccessData(TitlesArray, (void HUGEP* FAR*)&pdFreq);
					for (DWORD i = 0; i < source.GetTranchesCount(); i++)
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
				for (DWORD i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++)
				{
					SysFreeString(pdFreq->bstrVal);
					pdFreq++;
				}
				SafeArrayUnaccessData(DatesArray);
				SafeArrayAccessData(TitlesArray, (void HUGEP* FAR*)&pdFreq);
				for (DWORD i = 0; i < source.GetTranchesCount(); i++)
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
		}
		SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
		SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
		SysFreeString(Params.rgvarg[Params.cArgs-7].bstrVal);
		SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&pdFreq);
		for (DWORD i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++)
		{
			SysFreeString(pdFreq->bstrVal);
			pdFreq++;
		}
		SafeArrayUnaccessData(DatesArray);
		SafeArrayAccessData(TitlesArray, (void HUGEP* FAR*)&pdFreq);
		for (DWORD i = 0; i < source.GetTranchesCount(); i++)
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
}
HRESULT ExcelOutput::PlotCostFunding(
	const Waterfall& source,
	const QString& DestinationSheet,
	int DestinationIndex,
	const QDate& CallDate
	){
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
		else SafeArrayUnaccessData(DatesArray);
		if (SUCCEEDED(hr)) hr = SafeArrayAccessData(LoansCouponArray, (void HUGEP* FAR*)&LoansCouponIter);
		else{
			SafeArrayUnaccessData(DatesArray);
			SafeArrayUnaccessData(CostFundingArray);
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
				wchar_t *ucName = L"Run";
				hr = ExcelCommons::pExcelDisp->GetIDsOfNames(IID_NULL, &ucName, 1,
					LOCALE_SYSTEM_DEFAULT, &dispid);
				if(FAILED(hr))
				{
					SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
					SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
					SysFreeString(Params.rgvarg[Params.cArgs-7].bstrVal);
					SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&DatesIter);
					for (DWORD i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++)
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
				for (DWORD i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++)
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
		}
		SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
		SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
		SysFreeString(Params.rgvarg[Params.cArgs-7].bstrVal);
		SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&DatesIter);
		for (DWORD i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++)
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
		else SafeArrayUnaccessData(DatesArray);
		if (SUCCEEDED(hr)) hr = SafeArrayAccessData(CumEquityRetArray, (void HUGEP* FAR*)&CumEquityRetIter);
		else {SafeArrayUnaccessData(DatesArray); SafeArrayUnaccessData(EquityRetArray);}
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
				wchar_t *ucName = L"Run";
				hr = ExcelCommons::pExcelDisp->GetIDsOfNames(IID_NULL, &ucName, 1,
					LOCALE_SYSTEM_DEFAULT, &dispid);
				if(FAILED(hr))
				{
					SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
					SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
					SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&DatesIter);
					for (DWORD i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++)
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
				for (DWORD i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++)
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
		}
		SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
		SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
		SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&DatesIter);
		for (DWORD i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++)
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
		else SafeArrayUnaccessData(DatesArray);
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
		if (SUCCEEDED(hr)) HRESULT hr = SafeArrayAccessData(CPRArray, (void HUGEP* FAR*)&CPRIter);
		else SafeArrayUnaccessData(DatesArray);
		if (SUCCEEDED(hr)) HRESULT hr = SafeArrayAccessData(LSArray, (void HUGEP* FAR*)&LSIter);
		else{
			SafeArrayUnaccessData(DatesArray);
			SafeArrayUnaccessData(CPRArray);
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
				TempDbl=1.0-qPow(1.0-(source.GetCalculatedMtgPayments().GetLoss(i)/(source.GetCalculatedMtgPayments().GetAmountOut(i-1)-source.GetCalculatedMtgPayments().GetScheduled(i))),12.0);
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
				wchar_t *ucName = L"Run";
				hr = ExcelCommons::pExcelDisp->GetIDsOfNames(IID_NULL, &ucName, 1,
					LOCALE_SYSTEM_DEFAULT, &dispid);
				if(FAILED(hr))
				{
					SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
					SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
					SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&DatesIter);
					for (DWORD i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++)
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
				for (DWORD i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++)
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
		}
		SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
		SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
		SafeArrayAccessData(DatesArray, (void HUGEP* FAR*)&DatesIter);
		for (DWORD i = 0; i < source.GetTranche(0)->GetCashFlow().Count(); i++)
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
HRESULT ExcelOutput::PlotStressMargin(const StressTest& source,const QString& DestinationSheet,int DestinationIndex,const QString& TrancheTarget, double NewPrice){
	const QList<QString>& XAlias=source.GetXSpann();
	const QList<QString>& YAlias=source.GetYSpann();
	if(XAlias.size()<=0 || YAlias.size()<=0) return E_INVALIDARG;
	ExcelCommons::InitExcelOLE();
	
	SAFEARRAYBOUND  Bound[2];
	Bound[0].lLbound   = 1;
	Bound[0].cElements = XAlias.size();
	Bound[1].lLbound   = 1;
	Bound[1].cElements = YAlias.size();
	VARIANT HUGEP *pdFreq;
	SAFEARRAY* Margins = SafeArrayCreate(VT_VARIANT, 2, Bound);
	HRESULT hr = SafeArrayAccessData(Margins, (void HUGEP* FAR*)&pdFreq);
	if (SUCCEEDED(hr))
	{
		for (QList<QString>::const_iterator j=YAlias.begin();j!=YAlias.end();j++){
			for(QList<QString>::const_iterator i=XAlias.begin();i!=XAlias.end();i++){
				pdFreq->vt = VT_R8;
				pdFreq->dblVal = source.GetResults().value(*i).value(*j).GetTranche(TrancheTarget)->GetDiscountMargin(NewPrice);
				pdFreq++;
			}
		}
		SafeArrayUnaccessData(Margins);
	}
	SAFEARRAY* XHeaders = SafeArrayCreate(VT_VARIANT, 1, Bound);
	hr = SafeArrayAccessData(XHeaders, (void HUGEP* FAR*)&pdFreq);
	if (SUCCEEDED(hr)){
		for(QList<QString>::const_iterator i=XAlias.begin();i!=XAlias.end();i++){
			pdFreq->vt = VT_BSTR;
			pdFreq->bstrVal = SysAllocString(i->toStdWString().c_str());
			pdFreq++;
		}
		SafeArrayUnaccessData(XHeaders);
	}
	SAFEARRAY* YHeaders = SafeArrayCreate(VT_VARIANT, 1, Bound+1);
	hr = SafeArrayAccessData(YHeaders, (void HUGEP* FAR*)&pdFreq);
	if (SUCCEEDED(hr)){
		for(QList<QString>::const_iterator i=YAlias.begin();i!=YAlias.end();i++){
			pdFreq->vt = VT_BSTR;
			pdFreq->bstrVal = SysAllocString(i->toStdWString().c_str());
			pdFreq++;
		}
		SafeArrayUnaccessData(YHeaders);
	}


	static DISPID dispid = 0;
	DISPPARAMS Params;
	VARIANTARG Command[8];
	int CurrentCmdIndex=8-1;
	if(!ExcelCommons::pExcelDisp)return S_FALSE;
	try
	{
		Command[CurrentCmdIndex].vt = VT_BSTR;
		Command[CurrentCmdIndex--].bstrVal = SysAllocString(L"PlotStressMargin");
		Command[CurrentCmdIndex].vt = VT_BSTR;
		Command[CurrentCmdIndex--].bstrVal = SysAllocString(DestinationSheet.toStdWString().c_str());
		Command[CurrentCmdIndex].vt = VT_I4;
		Command[CurrentCmdIndex--].intVal = DestinationIndex;
		Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
		Command[CurrentCmdIndex--].parray = Margins;
		Command[CurrentCmdIndex].vt = VT_I4;
		Command[CurrentCmdIndex--].intVal = static_cast<int>(source.GetXVariability());
		Command[CurrentCmdIndex].vt = VT_I4;
		Command[CurrentCmdIndex--].intVal = static_cast<int>(source.GetYVariability());
		Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
		Command[CurrentCmdIndex--].parray = XHeaders;
		Command[CurrentCmdIndex].vt = VT_ARRAY | VT_VARIANT;
		Command[CurrentCmdIndex--].parray = YHeaders;

		Params.rgdispidNamedArgs = NULL;
		Params.rgvarg=Command;
		Params.cArgs = 8;
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
				hr = SafeArrayAccessData(YHeaders, (void HUGEP* FAR*)&pdFreq);
				if (SUCCEEDED(hr)){
					for(QList<QString>::const_iterator i=YAlias.begin();i!=YAlias.end();i++){
						SysFreeString(pdFreq->bstrVal);
						pdFreq++;
					}
					SafeArrayUnaccessData(YHeaders);
				}
				hr = SafeArrayAccessData(XHeaders, (void HUGEP* FAR*)&pdFreq);
				if (SUCCEEDED(hr)){
					for(QList<QString>::const_iterator i=XAlias.begin();i!=XAlias.end();i++){
						SysFreeString(pdFreq->bstrVal);
						pdFreq++;
					}
					SafeArrayUnaccessData(XHeaders);
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
			hr = SafeArrayAccessData(YHeaders, (void HUGEP* FAR*)&pdFreq);
			if (SUCCEEDED(hr)){
				for(QList<QString>::const_iterator i=YAlias.begin();i!=YAlias.end();i++){
					SysFreeString(pdFreq->bstrVal);
					pdFreq++;
				}
				SafeArrayUnaccessData(YHeaders);
			}
			hr = SafeArrayAccessData(XHeaders, (void HUGEP* FAR*)&pdFreq);
			if (SUCCEEDED(hr)){
				for(QList<QString>::const_iterator i=XAlias.begin();i!=XAlias.end();i++){
					SysFreeString(pdFreq->bstrVal);
					pdFreq++;
				}
				SafeArrayUnaccessData(XHeaders);
			}
			return hr;
		}
	}
	catch(_com_error &ce)
	{
		hr = ce.Error();
	}
	SysFreeString(Params.rgvarg[Params.cArgs-1].bstrVal);
	SysFreeString(Params.rgvarg[Params.cArgs-2].bstrVal);
	hr = SafeArrayAccessData(YHeaders, (void HUGEP* FAR*)&pdFreq);
	if (SUCCEEDED(hr)){
		for(QList<QString>::const_iterator i=YAlias.begin();i!=YAlias.end();i++){
			SysFreeString(pdFreq->bstrVal);
			pdFreq++;
		}
		SafeArrayUnaccessData(YHeaders);
	}
	hr = SafeArrayAccessData(XHeaders, (void HUGEP* FAR*)&pdFreq);
	if (SUCCEEDED(hr)){
		for(QList<QString>::const_iterator i=XAlias.begin();i!=XAlias.end();i++){
			SysFreeString(pdFreq->bstrVal);
			pdFreq++;
		}
		SafeArrayUnaccessData(XHeaders);
	}
	return hr;
}