#ifndef ManagedCommons_h__
#define ManagedCommons_h__
#include <QDate>
#include <QString>
#include <QList>
#include <QHash>
#include <vcclr.h>
#include "BackwardCompatibilityInterface.h"
using namespace System;
using namespace System::Text;
namespace ManagedCLO {
	/*! Enum defining the type of day count conventions that can be used, as in DAY COUNT CODES - MBS <HELP> 41 <GO> on Bloomberg.<br/>Conventions with "nominal" stop accruing interest regardles if the date is a holiday or not.*/
	enum class ManDayCountConvention {
		ACTACT = 101 /*!< Actual/Actual*/
		, ACT360 = 102 /*!< Actual/360*/
		, ACT365 = 103 /*!< Actual/365*/
		, N30360 = 104 /*!< Nominal 30/360*/
		, NACTACT = 105 /*!< Nominal Actual/Actual*/
		, NACT360 = 106 /*!< Nominal Actual/360*/
		, NACT365 = 107 /*!< Nominal Actual/365*/
		, ISMA30360 = 131  /*!< ISMA 30/360*/
		, ISDAACTACT = 201 /*!< ISDA Actual/Actual for leap years*/
		, AFBACTACT = 202 /*!< AFB Actual/Actual for leap years*/
		, NISDAACTACT = 203 /*!< Nominal ISDA Actual/Actual for leap years*/
		, NAFBACTACT = 204 /*!< Nominal AFB Actual/Actual for leap years*/
	};
	ref class ManBloombergVector;
	const quint32 VersionNumber=ModelVersionNumber; //Maximum 65535
	const quint32 MinSupportedVersion=MinimumSupportedVersion;
	DateTime QDate2DateTime(const QDate& a);
	QDate DateTime2QDate(DateTime a);
	QString String2QString(String^ a);
	String^ QString2String(const QString& a);
	Collections::Generic::List<String^>^ QStringList2ListString(const QList<QString>& a);
	QList<QString> ListString2QStringList(Collections::Generic::List<String^>^ a);
// 	QHash<QString, double> Dictionary2QHashStrDbl(Collections::Generic::Dictionary<String^, double>^ Values);
// 	QHash<QString, BloombergVector> Dictionary2QHashStrBbg(Collections::Generic::Dictionary<String^, ManBloombergVector^>^ Values);
// 	Collections::Generic::Dictionary<String^, double>^ QHashStrDbl2Dictionary(const QHash<QString, double>& Values);
// 	Collections::Generic::Dictionary<String^, ManBloombergVector^>^ QHashStrBbg2Dictionary(const QHash<QString, BloombergVector>& Values);
}
#endif // ManagedCommons_h__


