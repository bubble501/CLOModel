#ifndef ExcelCommons_h__
#define ExcelCommons_h__

#include <QString>
#include <comdef.h>
namespace ExcelCommons{
	bool InitExcelOLE();
	void UninitExcelOLE();
	QString CellOffset(const QString& StartCell,int RowOff=0, int ColOff=0);
	extern IDispatch *pExcelDisp;
	extern bool xll_initialised;
}
#endif // ExcelCommons_h__
