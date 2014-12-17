#ifndef CommonFunctions_h__
#define CommonFunctions_h__
#include <QtGlobal>
#include <QMetaType>
#include <typeinfo>
#include <QString>
class QDate;
class DayCountVector;
class BloombergVector;
template<typename T> class QList;

#define SeniorityStringSeparator '-'
#define MaximumInterestsTypes 8
#define MaximumIRRIterations 10000 //INT_MAX-1
#define CompoundShift 10
#define USE_SOFT_COLOUR //Uses softer color in stress test table rather than pure RGB(255,0,0) style ones
//#define SAVE_EXCEL_INPUTS
//#define SaveLoanTape
//#define DebugLogging
//#define PrintAggregatedMtgFlows

#ifdef DebugLogging
#define LOGASSERT(CheckExp,LogMsg) ((CheckExp) ? qt_noop() : PrintToTempFile("DebugLog.log",LogMsg)); Q_ASSERT_X(CheckExp,"LOGASSERT",LogMsg)
#define LOGDEBUG(LogMsg) PrintToTempFile("DebugLog",LogMsg,false)
#define LOGTOFILE(LogFile,LogMsg) PrintToTempFile(LogFile,LogMsg)
#define LOGCONDITIONALLY(Condition,LogMsg) ((Condition) ? PrintToTempFile("ConditionalLog",LogMsg): qt_noop())
#else
#define LOGASSERT(CheckExp,LogMsg)
#define LOGDEBUG(LogMsg)
#define LOGTOFILE(LogFile,LogMsg)
#define LOGCONDITIONALLY(Condition,LogMsg)
#endif
int MonthDiff(const QDate& FutureDte,const QDate& PresentDte);
QString InfixToPostfix(const QString& a);
enum class DayCountConvention : qint16 {
	Invalid = 0
	//Simple
	, ACTACT = 101
	, ACT360 = 102
	, ACT365 = 103
	, N30360 = 104
	, NACTACT = 105
	, NACT360 = 106
	, NACT365 = 107
	, ISMA30360 = 131
	, ISDAACTACT = 201
	, AFBACTACT = 202
	, NISDAACTACT = 203
	, NAFBACTACT = 204
	//Compounded
	, CompACTACT = (1 << CompoundShift) | 101
	, CompACT360 = (1 << CompoundShift) | 102
	, CompACT365 = (1 << CompoundShift) | 103
	, CompN30360 = (1 << CompoundShift) | 104
	, CompNACTACT = (1 << CompoundShift) | 105
	, CompNACT360 = (1 << CompoundShift) | 106
	, CompNACT365 = (1 << CompoundShift) | 107
	, CompISMA30360 = (1 << CompoundShift) | 131
	, CompISDAACTACT = (1 << CompoundShift) | 201
	, CompAFBACTACT = (1 << CompoundShift) | 202
	, CompNISDAACTACT = (1 << CompoundShift) | 203
	, CompNAFBACTACT = (1 << CompoundShift) | 204
	//Continuously Compounded
	, ContCompACTACT = (1 << (1+CompoundShift)) | 101
	, ContCompACT360 = (1 << (1+CompoundShift)) | 102
	, ContCompACT365 = (1 << (1+CompoundShift)) | 103
	, ContCompN30360 = (1 << (1+CompoundShift)) | 104
	, ContCompNACTACT = (1 << (1+CompoundShift)) | 105
	, ContCompNACT360 = (1 << (1+CompoundShift)) | 106
	, ContCompNACT365 = (1 << (1+CompoundShift)) | 107
	, ContCompISMA30360 = (1 << (1+CompoundShift)) | 131
	, ContCompISDAACTACT = (1 << (1+CompoundShift)) | 201
	, ContCompAFBACTACT = (1 << (1+CompoundShift)) | 202
	, ContCompNISDAACTACT = (1 << (1+CompoundShift)) | 203
	, ContCompNAFBACTACT = (1 << (1+CompoundShift)) | 204

};
double RoundUp(double a);
template<class T> void RegisterAsMetaType() {
	if (!QMetaType::isRegistered(qMetaTypeId<T>())) {
		int TypeID = qRegisterMetaType<T>(typeid(T).name());
		qRegisterMetaTypeStreamOperators<T>(typeid(T).name());
		LOGDEBUG(QString("Meta type registered: %1, ID: %2").arg(typeid(T).name()).arg(TypeID));
	}
}
QString Commarize(double num,unsigned int precision=0);
double CalculateIRR(const QList<QDate>& Dte, const QList<double>& Flws, const DayCountVector& Daycount, double Guess = 0.05);
double CalculateNPV(const QList<QDate>& Dte, const QList<double>& Flws, double Interest, const DayCountVector& Daycount);
double CalculateNPV(const QList<QDate>& Dte, const QList<double>& Flws, const BloombergVector& Interest, const DayCountVector& Daycount);
double CalculateNPV(const QList<QDate>& Dte, const QList<double>& Flws, const QString& Interest, const DayCountVector& Daycount);
double CalculateDM(const QList<QDate>& Dte, const QList<double>& Flws, double BaseRate, const DayCountVector& Daycount, double Guess = 0.05);
double CalculateDM(const QList<QDate>& Dte, const QList<double>& Flws, const BloombergVector& BaseRate, const DayCountVector& Daycount, double Guess = 0.05);
double CalculateDM(const QList<QDate>& Dte, const QList<double>& Flws, const QString& BaseRate, const DayCountVector& Daycount, double Guess = 0.05);
double AdjustCoupon(double AnnualCoupon, QDate PrevIPD, QDate CurrIPD, DayCountConvention DayCount);
bool IsHoliday(const QDate& a/*,const QString& CountryCode*/);
bool removeDir(const QString& dirName);
double GetLoanAssumption(const QString& LoanName, int columnIndex, QDate RefDate);
void PrintToTempFile(const QString& TempFileName, const QString& Message, bool PrintTime = true);
bool ValidDayCount(qint16 a);
QString NormaliseTriggerStructure(QString a);
QString GetFromConfig(const QString& Domain, const QString& Field, const QString& DefaultValue);
#endif // CommonFunctions_h__


