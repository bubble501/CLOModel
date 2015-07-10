#ifndef InternalItems_h__
#define InternalItems_h__
#include <QString>
#include <QMetaType>
#include <typeinfo>
#ifndef NO_DATABASE
#include <QMutex>
extern QMutex Db_Mutex;
#endif // !NO_DATABASE
#ifdef _DEBUG
#define DEBUG_CHECK(item,val) const auto item=val;
#else
#define DEBUG_CHECK(item,val)
#endif // _DEBUG
#define MaximumIRRIterations 10000 //INT_MAX-1
#define USE_SOFT_COLOUR //Uses softer color in stress test table rather than pure RGB(255,0,0) style ones
//#define Assumptions_ExcelOverDB // If this symbol is defined, the manual input of properties in excel, if any, will overwrite any scenario property set in the database 
//#define SAVE_EXCEL_INPUTS
//#define SaveLoanTape
//#define DebugLogging
//#define PrintAggregatedMtgFlows
//#define PrintStressTestExecutionTime
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
enum DayCountConvention : qint16;
extern const QString LoansPropertiesToSearch[2]; //Properties in which scenarios will be searched
double getTimeFactor(QDate PrevIPD /*Interesty start accrual date*/, QDate CurrIPD /*Interest end accrual date*/, DayCountConvention DayCount);
QString InfixToPostfix(const QString& a);
int NumberOfSetBits(quint32 i);
QString NormaliseTriggerStructure(QString a);
QString Commarize(double num, unsigned int precision = 0);
#define RegisterAsMetaType(T) { \
    if (QMetaType::type(#T)==QMetaType::UnknownType) { \
        qRegisterMetaType<T>(typeid(T).name()); \
        qRegisterMetaTypeStreamOperators<T>(typeid(T).name()); \
    } \
}
#endif // InternalItems_h__