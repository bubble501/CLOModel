#ifndef NO_BLOOMBERG
#include "ManQBbg.h"
String^ QBloombergLib::Stdstr2String(const QString& a) {
	return gcnew String(a.toLatin1().data());
}
QString QBloombergLib::String2Stdstr(String^ a) {
	QString Result("");
	for (CharEnumerator^ i = a->GetEnumerator(); i->MoveNext();) {
		Result += i->Current;
	}
	return Result;
}
Dictionary<String^, String^>^ QBloombergLib::Map2DictionaryStr(const QHash<QString, QString>& a) {
	Dictionary<String^, String^>^ Result = gcnew Dictionary<String^, String^>();
	for (QHash<QString, QString>::const_iterator i = a.begin(); i != a.end(); i++) {
		Result[Stdstr2String(i.key())] = Stdstr2String(i.value());
	}
	return Result;
}
QHash<QString, QString> QBloombergLib::Dictionary2MapStr(Dictionary<String^, String^>^ a) {
	QHash<QString, QString> Result;
	for each(KeyValuePair<String^, String^> kvp in a) {
		Result[String2Stdstr(kvp.Key)] = String2Stdstr(kvp.Value);
	}
	return Result;
}
List<qint64>^ QBloombergLib::Deque2List(const QList<qint64>& a) {
	List<qint64>^ Result = gcnew List < qint64 > ;
	for (QList<qint64>::const_iterator i = a.constBegin(); i != a.constEnd(); i++) {
		Result->Add(*i);
	}
	return Result;
}
QBloombergLib::ManQSingleBbgRequest^ QBloombergLib::ManQBbgRequest::FindRequest(qint64 ID) {
	const QSingleBbgRequest* Temp = static_cast<const QBbgRequest*>(Unmanaged)->FindRequest(ID);
	if (Temp)
		return gcnew ManQSingleBbgRequest(*Temp);
	return nullptr;
}
QBloombergLib::ManQSingleBbgRequest^ QBloombergLib::ManQBbgRequest::GetRequest(quint32 Index) {
	const QSingleBbgRequest* Temp = static_cast<const QBbgRequest*>(Unmanaged)->GetRequest(Index);
	if (Temp)
		return gcnew ManQSingleBbgRequest(*Temp);
	return nullptr;
}
inline void QBloombergLib::ManQBbgRequest::AddRequest(QBloombergLib::ManQSingleBbgRequest^ a) { Unmanaged->AddRequest(*(a->Unmanaged)); }
#endif


