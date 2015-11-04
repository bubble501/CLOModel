#ifndef InternalItems_h__
#define InternalItems_h__
#include <QString>
#include <QMetaType>
#include <typeinfo>
#include <boost/math/policies/error_handling.hpp>
#ifndef NO_DATABASE
#include <QMutex>
extern QMutex Db_Mutex;
#endif // !NO_DATABASE
#ifdef _DEBUG
#include <QDebug>
#define DEBG_LOG(x) qDebug() << x
#define DEBG_LOG_CONDITION(x,check) if(check) qDebug() << x
#define DEBUG_CHECK(item,val) const auto item=val
#else
#define DEBG_LOG_CONDITION(x,check)
#define DEBUG_CHECK(item,val)
#define DEBG_LOG(x)
#endif // _DEBUG
#define MaximumIRRIterations 10000 //INT_MAX-1
#define USE_SOFT_COLOUR //Uses softer color in stress test table rather than pure RGB(255,0,0) style ones
#define Assumptions_ExcelOverDB // If this symbol is defined, the manual input of properties in excel, if any, will overwrite any scenario property set in the database 
//#define SaveLoanTape
#define DebugLogging
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
#define TEMP_FILES_DIR QDir::tempPath() + "/CLOModel/"
#define ENSURE_DIR_EXIST(dirpath) const QDir tmpDir(dirpath); if (!tmpDir.exists()) tmpDir.mkpath(dirpath);
class QSqlQuery;

QString getLastExecutedQuery(const QSqlQuery& query);
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
typedef boost::math::policies::policy<
    boost::math::policies::overflow_error<boost::math::policies::throw_on_error>,
    boost::math::policies::underflow_error<boost::math::policies::throw_on_error>,
    boost::math::policies::evaluation_error<boost::math::policies::throw_on_error>
> optim_policy;
#endif // InternalItems_h__