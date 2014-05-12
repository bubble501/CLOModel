#ifndef ManagedCommons_h__
#define ManagedCommons_h__
using namespace System;
using namespace System::Text;
#include <QDate>
#include <QString>
#include <QList>
#include <QHash>
#include <vcclr.h>
#include "CommonFunctions.h"
namespace ManagedCLO {
	const quint32 VersionNumber=ModelVersionNumber; //Maximum 65535
	const quint32 MinSupportedVersion=MinimumSupportedVersion;
	inline DateTime QDate2DateTime(const QDate& a){
		if(a.isNull()) return DateTime();
		return DateTime(a.year(),a.month(),a.day());
	}
	inline QDate DateTime2QDate(DateTime a){
		return QDate(a.Year,a.Month,a.Day);
	}
	inline QString String2QString(String^ a){
		QString Result("");
		for(int i=0;i<a->Length;i++) Result+=(*a)[i];
		return Result;
		/*pin_ptr<const wchar_t> wchstr=PtrToStringChars(a);
		return QString::fromWCharArray(const_cast<wchar_t*>(wchstr),a->Length);*/
	}
	inline String^ QString2String(const QString& a){
		StringBuilder^ Result=gcnew StringBuilder();
		Char Tmp;
		for(int i=0;i<a.size();i++){
			Tmp= a.at(i).unicode();
			Result->Append(Tmp);
		}
		return Result->ToString();
	}
	inline Collections::Generic::List<String^>^ QStringList2ListString(const QList<QString>& a){
		Collections::Generic::List<String^>^ Result=gcnew Collections::Generic::List<String^>();
		foreach(const QString& SinlgeStr,a)
			Result->Add(QString2String(SinlgeStr));
		return Result;
	}
	inline QList<QString> ListString2QStringList(Collections::Generic::List<String^>^ a){
		QList<QString> Result;
		for each(String^ SinlgeStr in a)
			Result.append(String2QString(SinlgeStr));
		return Result;
	}
	inline QHash<QString,double> Dictionary2QHash(Collections::Generic::Dictionary<String^,double>^ Values){
		QHash<QString,double> ConvertDict;
		for each(Collections::Generic::KeyValuePair<String^,double>^ entry in Values){
			ConvertDict.insert(
				String2QString(entry->Key)
				,entry->Value
				);
		}
		return ConvertDict;
	}
}
#endif // ManagedCommons_h__


