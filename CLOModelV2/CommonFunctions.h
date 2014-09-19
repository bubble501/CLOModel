#ifndef CommonFunctions_h__
#define CommonFunctions_h__
#include <QtGlobal>
class QDate;
class QString;
template<typename T> class QList;
//#define DebugLogging
#define MaximumIRRIterations 10000//INT_MAX-1
#define MaximumInterestsTypes 8
#define CompoundShift 10
//#define SaveLoanTape
int MonthDiff(const QDate& FutureDte,const QDate& PresentDte);
class BloombergVector;
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
template<class T> bool LessThanPoint(T* a,T* b){return (*a)<(*b);}
QString Commarize(double num,unsigned int precision=0);
double CalculateIRR(const QList<QDate>& Dte, const QList<double>& Flws, DayCountConvention Daycount = DayCountConvention::ACT360, double Guess = 0.05);
double CalculateNPV(const QList<QDate>& Dte, const QList<double>& Flws, double Interest, DayCountConvention Daycount = DayCountConvention::ACT360);
double CalculateNPV(const QList<QDate>& Dte, const QList<double>& Flws, const BloombergVector& Interest, DayCountConvention Daycount = DayCountConvention::ACT360);
double CalculateNPV(const QList<QDate>& Dte, const QList<double>& Flws, const QString& Interest, DayCountConvention Daycount = DayCountConvention::ACT360);
double CalculateDM(const QList<QDate>& Dte, const QList<double>& Flws, double BaseRate, DayCountConvention Daycount = DayCountConvention::ACT360, double Guess = 0.05);
double CalculateDM(const QList<QDate>& Dte, const QList<double>& Flws, const BloombergVector& BaseRate, DayCountConvention Daycount = DayCountConvention::ACT360, double Guess = 0.05);
double CalculateDM(const QList<QDate>& Dte, const QList<double>& Flws, const QString& BaseRate, DayCountConvention Daycount = DayCountConvention::ACT360, double Guess = 0.05);
double AdjustCoupon(double AnnualCoupon, QDate PrevIPD, QDate CurrIPD, DayCountConvention DayCount);
bool IsHoliday(const QDate& a/*,const QString& CountryCode*/);
bool removeDir(const QString& dirName);
double GetLoanAssumption(const QString& LoanName, int columnIndex, QDate RefDate);
void PrintToTempFile(const QString& TempFileName, const QString& Message, bool PrintTime = true);
bool VaidDayCount(qint16 a);
#ifdef DebugLogging
#define LOGASSERT(CheckExp,LogMsg) ((CheckExp) ? qt_noop() : PrintToTempFile("DebugLog.log",LogMsg)); Q_ASSERT_X(CheckExp,"LOGASSERT",LogMsg)
#define LOGDEBUG(LogMsg) PrintToTempFile("DebugLog",LogMsg,false)
#define LOGTOFILE(LogFile,LogMsg) PrintToTempFile(QString(LogFile) + ".log",LogMsg)
#else
#define LOGASSERT(CheckExp,LogMsg)
#define LOGDEBUG(LogMsg)
#define LOGTOFILE(LogFile,LogMsg)
#endif


#endif // CommonFunctions_h__