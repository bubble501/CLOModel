#include "ExcelCommons.h"
#include <QRegExp>
#include <qmath.h>
IDispatch* ExcelCommons::pExcelDisp = NULL;
bool ExcelCommons::xll_initialised = false;
bool ExcelCommons::InitExcelOLE()
{
	if(pExcelDisp)
		return true; // already initialised
	HWND hWnd;
	if((hWnd = FindWindow(L"XLMAIN", 0)) != NULL)
	{
		SendMessage(hWnd, WM_USER + 18, 0, 0);
	}
	OleInitialize(NULL);
	CLSID clsid;
	HRESULT hr;
	char cErr[64];
	IUnknown *pUnk;
	hr = CLSIDFromProgID(L"Excel.Application", &clsid);
	if(FAILED(hr))
	{
		return false;
	}
	hr = GetActiveObject(clsid, NULL, &pUnk);
	if(FAILED(hr))
	{
		return false;
	}
	hr = pUnk->QueryInterface(IID_IDispatch,(void**)&pExcelDisp);
	if(FAILED(hr))
	{
		return false;
	}
	pUnk->Release();
	return true;
}
void ExcelCommons::UninitExcelOLE(){
	pExcelDisp->Release();
	pExcelDisp = NULL;
	OleUninitialize();
}
QString ExcelCommons::CellOffset(const QString& StrtCell,int RowOff, int ColOff){
	QString StartCell(StrtCell);
	StartCell.replace("$","");
	QRegExp PathFinder;
	PathFinder.setPattern("^(.+)!.+$");
	PathFinder.exactMatch(StartCell);
	QString SheetName=PathFinder.cap(1);
	SheetName.replace('\'',"");
	//if(SheetName.at(0)!='\'') SheetName.prepend('\'');
	//if(SheetName.at(SheetName.size()-1)!='\'') SheetName.append('\'');
	QString RawAddress=StartCell;
	PathFinder.setPattern("^(.+)!");
	RawAddress.replace(PathFinder,"");
	QString ColPart=RawAddress.toUpper();
	PathFinder.setPattern("[0-9]+");
	ColPart.replace(PathFinder,"");
	QString RowPart=RawAddress;
	PathFinder.setPattern("[^0-9]+");
	RowPart.replace(PathFinder,"");
	int ColResult=0;
	for(int i=0;i<ColPart.size();i++){
		ColResult+=(ColPart.at(ColPart.size()-1-i).toAscii()-'A'+1)*qPow(26,i);
	}
	return (SheetName+"!%1,%2").arg(RowPart.toInt()+RowOff).arg(ColResult+ColOff);
}