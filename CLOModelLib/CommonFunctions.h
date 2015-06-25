#ifndef CommonFunctions_h__
#define CommonFunctions_h__
#include <QtGlobal>
#include <QMetaType>
#include <QDataStream>
#include "clomodellib_global.h"
#define CompoundShift 10
#define MaximumInterestsTypes 8
#define NumStressDimentsions 6
class QDate;
class QString;
class DayCountVector;
class BloombergVector;
template<typename T> class QList;
int CLOMODELLIB_EXPORT MonthDiff(const QDate& FutureDte, const QDate& PresentDte);
void CLOMODELLIB_EXPORT BuildDBCountries(const QString& path);
enum DayCountConvention : qint16
{
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
    , ContCompACTACT = (1 << (1 + CompoundShift)) | 101
    , ContCompACT360 = (1 << (1 + CompoundShift)) | 102
    , ContCompACT365 = (1 << (1 + CompoundShift)) | 103
    , ContCompN30360 = (1 << (1 + CompoundShift)) | 104
    , ContCompNACTACT = (1 << (1 + CompoundShift)) | 105
    , ContCompNACT360 = (1 << (1 + CompoundShift)) | 106
    , ContCompNACT365 = (1 << (1 + CompoundShift)) | 107
    , ContCompISMA30360 = (1 << (1 + CompoundShift)) | 131
    , ContCompISDAACTACT = (1 << (1 + CompoundShift)) | 201
    , ContCompAFBACTACT = (1 << (1 + CompoundShift)) | 202
    , ContCompNISDAACTACT = (1 << (1 + CompoundShift)) | 203
    , ContCompNAFBACTACT = (1 << (1 + CompoundShift)) | 204

};
double CLOMODELLIB_EXPORT RoundUp(double a);
double CLOMODELLIB_EXPORT CalculateIRR(const QList<QDate>& Dte, const QList<double>& Flws, const DayCountVector& Daycount, double Guess = 0.05);
double CLOMODELLIB_EXPORT CalculateNPV(const QList<QDate>& Dte, const QList<double>& Flws, double Interest, const DayCountVector& Daycount);
double CLOMODELLIB_EXPORT CalculateNPV(const QList<QDate>& Dte, const QList<double>& Flws, const BloombergVector& Interest, const DayCountVector& Daycount);
double CLOMODELLIB_EXPORT CalculateNPV(const QList<QDate>& Dte, const QList<double>& Flws, const QString& Interest, const DayCountVector& Daycount);
double CLOMODELLIB_EXPORT CalculateDM(const QList<QDate>& Dte, const QList<double>& Flws, double BaseRate, const DayCountVector& Daycount, double Guess = 0.05);
double CLOMODELLIB_EXPORT CalculateDM(const QList<QDate>& Dte, const QList<double>& Flws, const BloombergVector& BaseRate, const DayCountVector& Daycount, double Guess = 0.05);
double CLOMODELLIB_EXPORT CalculateDM(const QList<QDate>& Dte, const QList<double>& Flws, const QString& BaseRate, const DayCountVector& Daycount, double Guess = 0.05);
double CLOMODELLIB_EXPORT AdjustCoupon(double AnnualCoupon, QDate PrevIPD, QDate CurrIPD, DayCountConvention DayCount);
bool CLOMODELLIB_EXPORT IsHoliday(const QDate& a/*,const QString& CountryCode*/);
bool CLOMODELLIB_EXPORT removeDir(const QString& dirName);
bool CLOMODELLIB_EXPORT ValidDayCount(qint16 a);

#define DECLARE_PUBLIC_QOBJECT_COMMONS(Class) protected: \
    Q_DECLARE_PRIVATE(Class) \
    Class(Class ## Private *d, QObject* parent =nullptr); \
    public: \
    Class(QObject* parent =nullptr);
#define DECLARE_PUBLIC_COMMONS(Class) protected: \
    Q_DECLARE_PRIVATE(Class) \
    Class(Class ## Private *d); \
    public: \
    Class();
#define DECLARE_PUBLIC_COMMONS_COPY(Class) protected: \
    Class(Class ## Private *d, const Class& other); \
    public: \
    Class(const Class& other); \
    Class& operator=(const Class& other);
#define DEFINE_PUBLIC_QOBJECT_COMMONS(Class) Class::Class(QObject* parent) \
    : Class(new Class ## Private(this),parent) \
    {}
#define DEFINE_PUBLIC_COMMONS(Class) Class::Class() \
    : Class(new Class ## Private(this)) \
    {} 
#define DEFINE_PUBLIC_COMMONS_COPY(Class) Class::Class(const Class& other) \
     : Class(new Class ## Private(this), other) \
    {} 
#define DECLARE_PRIVATE_COMMONS(Class) protected: \
    Q_DECLARE_PUBLIC(Class) \
    public: \
    Class ## Private(Class *q); \
    private: \
    Class ## Private(const Class ## Private &other) =delete; \
    Class ## Private& operator=(const Class ## Private &other) = delete;
// #TODO Use this instead of assignment operators in public class as copy constructor
#define DECLARE_PRIVATE_COMMONS_COPY(Class) public: \
    Class ## Private(Class *q,const Class ## Private &other);
#define DECLARE_PRIVATE_COMMONS_DATASTREAM(Class) friend QDataStream& operator<<(QDataStream & stream, const Class& flows); \
friend QDataStream& operator>>(QDataStream & stream, Class& flows);


#endif // CommonFunctions_h__


