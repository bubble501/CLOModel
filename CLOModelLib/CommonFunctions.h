#ifndef CommonFunctions_h__
#define CommonFunctions_h__
#include <QtGlobal>
#include <QMetaType>
#include <QDataStream>
#include "clomodellib_global.h"
#define CompoundShift 10
#define MaximumInterestsTypes 8
#define NumStressDimentsions 6
#define StreamVersionUsed QDataStream::Qt_5_3
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
    // Corporate and Government bonds conventions
    , FiACTACT = 1
    , FiACT360 = 2
    , FiACT365 = 3
    , Fi30ACT = 4
    , FiSIA30360 = 5
    , Fi30365 = 6
    , FiNLACT = 7
    , FiNL360 = 8
    , FiNL365 = 9
    , FiACTACTneom = 10
    , FiACT360neom = 11
    , FiACT365neom = 12
    , Fi30ACTneom = 13
    , FiSIA30360neom = 14
    , Fi30365neom = 15
    , FiNLACTneom = 16
    , FiNL360neom = 17
    , FiNL365neom = 18
    , FiISMA30ACT = 19
    , FiISMA30360 = 20
    , FiISMA30365 = 21
    , FiISMA30ACTneom = 22
    , FiISMA30360neom = 23
    , FiISMA30365neom = 24
    , FiACT364 = 27
    // , FiUSMUNI30360 = 29
    // , FiBusiness252 = 33
    , FiGerman30360 = 35
    // , FiBusiness252neom = 36
    , FiGerman30360neom = 38
    // , FiUSWITACTACTneom = 40
    // , FiUSIWIBACT360neom = 41
    //ABS Conventions
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
    , CompFiACTACT = (1 << CompoundShift) | 1
    , CompFiACT360 = (1 << CompoundShift) | 2
    , CompFiACT365 = (1 << CompoundShift) | 3
    , CompFi30ACT = (1 << CompoundShift) | 4
    , CompFiSIA30360 = (1 << CompoundShift) | 5
    , CompFi30365 = (1 << CompoundShift) | 6
    , CompFiNLACT = (1 << CompoundShift) | 7
    , CompFiNL360 = (1 << CompoundShift) | 8
    , CompFiNL365 = (1 << CompoundShift) | 9
    , CompFiACTACTneom = (1 << CompoundShift) | 10
    , CompFiACT360neom = (1 << CompoundShift) | 11
    , CompFiACT365neom = (1 << CompoundShift) | 12
    , CompFi30ACTneom = (1 << CompoundShift) | 13
    , CompFiSIA30360neom = (1 << CompoundShift) | 14
    , CompFi30365neom = (1 << CompoundShift) | 15
    , CompFiNLACTneom = (1 << CompoundShift) | 16
    , CompFiNL360neom = (1 << CompoundShift) | 17
    , CompFiNL365neom = (1 << CompoundShift) | 18
    , CompFiISMA30ACT = (1 << CompoundShift) | 19
    , CompFiISMA30360 = (1 << CompoundShift) | 20
    , CompFiISMA30365 = (1 << CompoundShift) | 21
    , CompFiISMA30ACTneom = (1 << CompoundShift) | 22
    , CompFiISMA30360neom = (1 << CompoundShift) | 23
    , CompFiISMA30365neom = (1 << CompoundShift) | 24
    , CompFiACT364 = (1 << CompoundShift) | 27
    //, CompFiUSMUNI30360 = (1 << CompoundShift) | 29
    //, CompFiBusiness252 = (1 << CompoundShift) | 33
    , CompFiGerman30360 = (1 << CompoundShift) | 35
    // , CompFiBusiness252neom = (1 << CompoundShift) | 36
    , CompFiGerman30360neom = (1 << CompoundShift) | 38
    //, CompFiUSWITACTACTneom = (1 << CompoundShift) | 40
    //, CompFiUSIWIBACT360neom = (1 << CompoundShift) | 41
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
    , ContCompFiACTACT = (1 << (1 + CompoundShift)) | 1
    , ContCompFiACT360 = (1 << (1 + CompoundShift)) | 2
    , ContCompFiACT365 = (1 << (1 + CompoundShift)) | 3
    , ContCompFi30ACT = (1 << (1 + CompoundShift)) | 4
    , ContCompFiSIA30360 = (1 << (1 + CompoundShift)) | 5
    , ContCompFi30365 = (1 << (1 + CompoundShift)) | 6
    , ContCompFiNLACT = (1 << (1 + CompoundShift)) | 7
    , ContCompFiNL360 = (1 << (1 + CompoundShift)) | 8
    , ContCompFiNL365 = (1 << (1 + CompoundShift)) | 9
    , ContCompFiACTACTneom = (1 << (1 + CompoundShift)) | 10
    , ContCompFiACT360neom = (1 << (1 + CompoundShift)) | 11
    , ContCompFiACT365neom = (1 << (1 + CompoundShift)) | 12
    , ContCompFi30ACTneom = (1 << (1 + CompoundShift)) | 13
    , ContCompFiSIA30360neom = (1 << (1 + CompoundShift)) | 14
    , ContCompFi30365neom = (1 << (1 + CompoundShift)) | 15
    , ContCompFiNLACTneom = (1 << (1 + CompoundShift)) | 16
    , ContCompFiNL360neom = (1 << (1 + CompoundShift)) | 17
    , ContCompFiNL365neom = (1 << (1 + CompoundShift)) | 18
    , ContCompFiISMA30ACT = (1 << (1 + CompoundShift)) | 19
    , ContCompFiISMA30360 = (1 << (1 + CompoundShift)) | 20
    , ContCompFiISMA30365 = (1 << (1 + CompoundShift)) | 21
    , ContCompFiISMA30ACTneom = (1 << (1 + CompoundShift)) | 22
    , ContCompFiISMA30360neom = (1 << (1 + CompoundShift)) | 23
    , ContCompFiISMA30365neom = (1 << (1 + CompoundShift)) | 24
    , ContCompFiACT364 = (1 << (1 + CompoundShift)) | 27
    // , ContCompFiUSMUNI30360 = (1 << (1 + CompoundShift)) | 29
    // , ContCompFiBusiness252 = (1 << (1 + CompoundShift)) | 33
    , ContCompFiGerman30360 = (1 << (1 + CompoundShift)) | 35
    // , ContCompFiBusiness252neom = (1 << (1 + CompoundShift)) | 36
    , ContCompFiGerman30360neom = (1 << (1 + CompoundShift)) | 38
    // , ContCompFiUSWITACTACTneom = (1 << (1 + CompoundShift)) | 40
    // , ContCompFiUSIWIBACT360neom = (1 << (1 + CompoundShift)) | 41
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
QString CLOMODELLIB_EXPORT GetFromConfig(const QString& Domain, const QString& Field, const QString& DefaultValue = QString());
void CLOMODELLIB_EXPORT PrintToTempFile(const QString& TempFileName, const QString& Message, bool PrintTime = true);
double CLOMODELLIB_EXPORT CalculateIRR(const QList<QDate>& Dte, const QList<double>& Flws, const DayCountVector& Daycount, double Guess = 0.05);
double CLOMODELLIB_EXPORT CalculateNPV(const QList<QDate>& Dte, const QList<double>& Flws, double Interest, const DayCountVector& Daycount);
double CLOMODELLIB_EXPORT CalculateNPV(const QList<QDate>& Dte, const QList<double>& Flws, const BloombergVector& Interest, const DayCountVector& Daycount);
double CLOMODELLIB_EXPORT CalculateNPV(const QList<QDate>& Dte, const QList<double>& Flws, const QString& Interest, const DayCountVector& Daycount);
double CLOMODELLIB_EXPORT CalculateDM(const QList<QDate>& Dte, const QList<double>& Flws, double BaseRate, const DayCountVector& Daycount, double Guess = 0.05);
double CLOMODELLIB_EXPORT CalculateDM(const QList<QDate>& Dte, const QList<double>& Flws, const BloombergVector& BaseRate, const DayCountVector& Daycount, double Guess = 0.05);
double CLOMODELLIB_EXPORT CalculateDM(const QList<QDate>& Dte, const QList<double>& Flws, const QString& BaseRate, const DayCountVector& Daycount, double Guess = 0.05);
double CLOMODELLIB_EXPORT deannualiseCoupon(double AnnualCoupon, QDate PrevIPD, QDate CurrIPD, DayCountConvention DayCount);
double CLOMODELLIB_EXPORT annualiseCoupon(double periodicCoupon, QDate PrevIPD, QDate CurrIPD, DayCountConvention DayCount);
bool CLOMODELLIB_EXPORT IsHoliday(const QDate& a/*,const QString& CountryCode*/);
bool CLOMODELLIB_EXPORT removeDir(const QString& dirName);
bool CLOMODELLIB_EXPORT ValidDayCount(qint16 a);

#define DECLARE_PUBLIC_QOBJECT_COMMONS(Class) protected: \
    Q_DECLARE_PRIVATE(Class) \
    Class(Class ## Private *d, QObject* parent =nullptr); \
    public: \
    Class(QObject* parent =nullptr); \
    virtual ~Class();
#define DECLARE_PUBLIC_COMMONS(Class) protected: \
    Q_DECLARE_PRIVATE(Class) \
    Class(Class ## Private *d); \
    public: \
    Class(); \
    virtual ~Class();
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
// If you change this change #PublicCopies
#define DEFINE_PUBLIC_COMMONS_COPY(Class) Class::Class(const Class& other) \
     : Class(new Class ## Private(this,*(other.d_func())), other) \
    {} 
#define DECLARE_PRIVATE_COMMONS(Class) protected: \
    Q_DECLARE_PUBLIC(Class) \
    public: \
    Class ## Private(Class *q); \
    virtual ~Class ## Private(); \
    private: \
    Class ## Private(const Class ## Private &other) =delete; \
    Class ## Private& operator=(const Class ## Private &other) = delete;
// #TODO Use this instead of assignment operators in public class as copy constructor
#define DECLARE_PRIVATE_COMMONS_COPY(Class) public: \
    Class ## Private(Class *q,const Class ## Private &other);
#define DECLARE_PRIVATE_COMMONS_DATASTREAM(Class) friend CLOMODELLIB_EXPORT QDataStream& operator<<(QDataStream & stream, const Class& flows); \
friend CLOMODELLIB_EXPORT QDataStream& operator>>(QDataStream & stream, Class& flows);


#endif // CommonFunctions_h__
