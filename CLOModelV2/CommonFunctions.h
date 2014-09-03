#ifndef CommonFunctions_h__
#define CommonFunctions_h__

class QDate;
class QString;
template<typename T> class QList;
#define MaximumIRRIterations 10000//INT_MAX-1
//#define SaveLoanTape
int MonthDiff(const QDate& FutureDte,const QDate& PresentDte);
class BloombergVector;
enum class DayCountConvention : short {
	ACTACT = 101
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

void PrintToTempFile(const QString& TempFileName, const QString& Message);



#endif // CommonFunctions_h__