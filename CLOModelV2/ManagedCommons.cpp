#include "ManagedCommons.h"
#include "ManBloombergVector.h"
using namespace ManagedCLO;
DateTime ManagedCLO::QDate2DateTime(const QDate& a) {
	if (a.isNull()) return DateTime();
	return DateTime(a.year(), a.month(), a.day());
}
QDate ManagedCLO::DateTime2QDate(DateTime a) {
	return QDate(a.Year, a.Month, a.Day);
}
QString ManagedCLO::String2QString(String^ a) {
	QString Result("");
	for (int i = 0; i<a->Length; i++) Result += (*a)[i];
	return Result;
	/*pin_ptr<const wchar_t> wchstr=PtrToStringChars(a);
	return QString::fromWCharArray(const_cast<wchar_t*>(wchstr),a->Length);*/
}
String^ ManagedCLO::QString2String(const QString& a) {
	StringBuilder^ Result = gcnew StringBuilder();
	Char Tmp;
	for (int i = 0; i<a.size(); i++) {
		Tmp = a.at(i).unicode();
		Result->Append(Tmp);
	}
	return Result->ToString();
}
Collections::Generic::List<String^>^ ManagedCLO::QStringList2ListString(const QList<QString>& a) {
	Collections::Generic::List<String^>^ Result = gcnew Collections::Generic::List<String^>();
	foreach(const QString& SinlgeStr, a)
		Result->Add(QString2String(SinlgeStr));
	return Result;
}
QList<QString> ManagedCLO::ListString2QStringList(Collections::Generic::List<String^>^ a) {
	QList<QString> Result;
	for each(String^ SinlgeStr in a)
		Result.append(String2QString(SinlgeStr));
	return Result;
}
/*
QHash<QString, double> ManagedCLO::Dictionary2QHashStrDbl(Collections::Generic::Dictionary<String^, double>^ Values) {
	QHash<QString, double> ConvertDict;
	for each(Collections::Generic::KeyValuePair<String^, double>^ entry in Values) {
		ConvertDict.insert(
			String2QString(entry->Key)
			, entry->Value
			);
	}
	return ConvertDict;
}
Collections::Generic::Dictionary<String^, double>^ ManagedCLO::QHashStrDbl2Dictionary(const QHash<QString, double>& Values) {
	Collections::Generic::Dictionary<String^, double>^ ConvertDict=gcnew Collections::Generic::Dictionary<String^, double>();
	for (QHash<QString, double>::ConstIterator i = Values.constBegin(); i != Values.constEnd(); i++) {
		ConvertDict->Add(QString2String(i.key()), i.value());
	}
	return ConvertDict;
}
QHash<QString, BloombergVector> ManagedCLO::Dictionary2QHashStrBbg(Collections::Generic::Dictionary<String^, ManBloombergVector^>^ Values) {
	QHash<QString, BloombergVector> ConvertDict;
	for each(Collections::Generic::KeyValuePair<String^, ManBloombergVector^>^ entry in Values) {
		ConvertDict.insert(
			String2QString(entry->Key)
			, *(entry->Value->Unmanaged)
			);
	}
	return ConvertDict;
}
Collections::Generic::Dictionary<String^, ManBloombergVector^>^ QHashStrBbg2Dictionary(const QHash<QString, BloombergVector>& Values) {
	Collections::Generic::Dictionary<String^, ManBloombergVector^>^ ConvertDict = gcnew Collections::Generic::Dictionary<String^, ManBloombergVector^>();
	for (QHash<QString, BloombergVector>::ConstIterator i = Values.constBegin(); i != Values.constEnd(); i++) {
		ConvertDict->Add(QString2String(i.key()), gcnew ManBloombergVector(i.value()));
	}
	return ConvertDict;
}*/