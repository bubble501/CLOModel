#ifndef ExcelCommons_h__
#define ExcelCommons_h__
#ifdef DebugLogging
#include <QDebug>
#define LOGASSERT(CheckExp,LogMsg) ((CheckExp) ? qt_noop() : PrintToTempFile("DebugLog.log",LogMsg)); Q_ASSERT_X(CheckExp,"LOGASSERT",LogMsg)
#define LOGDEBUG(LogMsg) PrintToTempFile("DebugLog",LogMsg,false)
#define LOGTOFILE(LogFile,LogMsg) PrintToTempFile(LogFile,LogMsg)
#define LOGCONDITIONALLY(Condition,LogMsg) ((Condition) ? PrintToTempFile("ConditionalLog",LogMsg): qt_noop())
#define LOGCONSOLE(LogMsg) qDebug() << LogMsg
#else
#define LOGASSERT(CheckExp,LogMsg)
#define LOGDEBUG(LogMsg)
#define LOGTOFILE(LogFile,LogMsg)
#define LOGCONDITIONALLY(Condition,LogMsg)
#define LOGCONSOLE(LogMsg)
#endif
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
