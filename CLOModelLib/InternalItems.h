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
#define DEBUG_ITEM auto
#endif // _DEBUG
#define MaximumInterestsTypes 8
#define MaximumIRRIterations 10000 //INT_MAX-1
#define USE_SOFT_COLOUR //Uses softer color in stress test table rather than pure RGB(255,0,0) style ones
//#define Assumptions_ExcelOverDB // If this symbol is defined, the manual input of properties in excel, if any, will overwrite any scenario property set in the database 
//#define SAVE_EXCEL_INPUTS
//#define SaveLoanTape
//#define DebugLogging
//#define PrintAggregatedMtgFlows
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
extern const QString LoansPropertiesToSearch[2]; //Properties in which scenarios will be searched
QString InfixToPostfix(const QString& a);
template<class T> void RegisterAsMetaType()
{
    if (!QMetaType::isRegistered(qMetaTypeId<T>())) {
        int TypeID = qRegisterMetaType<T>(typeid(T).name());
        qRegisterMetaTypeStreamOperators<T>(typeid(T).name());
        LOGDEBUG(QString("Meta type registered: %1, ID: %2").arg(typeid(T).name()).arg(TypeID));
    }
}
void PrintToTempFile(const QString& TempFileName, const QString& Message, bool PrintTime = true);
int NumberOfSetBits(quint32 i);
QString NormaliseTriggerStructure(QString a);
QString GetFromConfig(const QString& Domain, const QString& Field, const QString& DefaultValue = QString());
#endif // InternalItems_h__