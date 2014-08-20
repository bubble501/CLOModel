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


