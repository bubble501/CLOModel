#ifndef NO_BLOOMBERG
#include "BloombergRequest.h"
#include "SingleBbgRequest.h"
#include <QtAlgorithms>
#include <QStringList>
void BloombergRequest::ClearResults() {
	for (QHash<qint64, SingleBbgRequest*>::iterator i = ResultTable.begin(); i != ResultTable.end(); i++) {
		(*i)->SetValue("");
		(*i)->SetErrorCode(NoErrors);
	}
}
void BloombergRequest::SetErrorCode(BbgErrorCodes val) {
	if (val == NoErrors) m_ErrorCode = NoErrors;
	else m_ErrorCode = m_ErrorCode | static_cast<int>(val);
}
BloombergRequest::~BloombergRequest() {
	ClearRequests();
}
BloombergRequest::BloombergRequest(const BloombergRequest& a) 
	:MaxID(a.MaxID)
	, m_ErrorCode(a.m_ErrorCode)
	, m_AutoConstantRates(a.m_AutoConstantRates)
{
	foreach(const SingleBbgRequest* SigleReq, a.ResultTable) {
		ResultTable.insert(SigleReq->GetResultID(),new SingleBbgRequest(*SigleReq));
	}
}
BloombergRequest& BloombergRequest::operator= (const BloombergRequest& a) {
	MaxID = a.MaxID;
	m_ErrorCode = a.m_ErrorCode;
	m_AutoConstantRates = a.m_AutoConstantRates;
	foreach(const SingleBbgRequest* SigleReq, a.ResultTable) {
		ResultTable.insert(SigleReq->GetResultID(), new SingleBbgRequest(*SigleReq));
	}
	return *this;
}
void BloombergRequest::ClearRequests() {
	for (QHash<qint64, SingleBbgRequest*>::iterator i = ResultTable.begin(); i != ResultTable.end(); i++) {
		delete (*i);
	}
	ResultTable.clear();
}
QDataStream& operator<<(QDataStream & stream, const BloombergRequest& flows) {
	stream 
		<< flows.m_ErrorCode
		<< static_cast<qint32>(flows.ResultTable.size())
		<< flows.m_AutoConstantRates
	;
	foreach(const SingleBbgRequest* SingleReq, flows.ResultTable)
		stream << (*SingleReq);
	return stream;
}
QDataStream& operator>>(QDataStream & stream, BloombergRequest& flows) { return flows.LoadOldVersion(stream); }
QDataStream& BloombergRequest::LoadOldVersion(QDataStream& stream) {
	qint32 ListSize;
	stream >> m_ErrorCode >> ListSize >> m_AutoConstantRates;
	SingleBbgRequest TempReq;
	for (qint32 i = 0; i < ListSize; i++) {
		stream >> TempReq;
		ResultTable.insert(TempReq.GetResultID(),new SingleBbgRequest(TempReq));
	}
	ResetProtocolVersion();
	return stream;
}
void BloombergRequest::AddRequest(const SingleBbgRequest& a) {
	if (ResultTable.contains(a.GetResultID())) return;
	foreach(const SingleBbgRequest* SingleRequ, ResultTable) {
		if (SingleRequ->operator==(a)) return;
	}
	QHash<qint64, SingleBbgRequest*>::iterator CurrentResult = ResultTable.insert(a.GetResultID(), new SingleBbgRequest(a));
	if (m_AutoConstantRates) {
		(*CurrentResult)->SetAutoConstantRates(true);
	}
	if ((*CurrentResult)->GetResultID() < 1)
		(*CurrentResult)->SetResultID(qMax(1i64, ++MaxID));
	if ((*CurrentResult)->GetResultID()>MaxID)
		MaxID = (*CurrentResult)->GetResultID();
	
}
const SingleBbgRequest* BloombergRequest::FindRequest(qint64 ID) const {
	return ResultTable.value(ID, NULL);
}
const SingleBbgRequest* BloombergRequest::GetRequest(int Index) const {
	if (Index<0 || Index>ResultTable.size()) return NULL;
	return *(ResultTable.constBegin()+Index);
}
SingleBbgRequest* BloombergRequest::GetRequest(int Index) {
	if (Index<0 || Index>ResultTable.size()) return NULL;
	return *(ResultTable.begin() + Index);
}
SingleBbgRequest* BloombergRequest::FindRequest(qint64 ID) {
	if (!ResultTable.contains(ID)) return NULL;
	return ResultTable[ID];
}
bool BloombergRequest::IsValidReq() const {
	foreach(const SingleBbgRequest* SingleRequ, ResultTable) {
		if (!SingleRequ->IsValidReq()) return false;
	}
	return true;
}
QList<qint64> BloombergRequest::FindSecurity(const QString& Secur)const {
	QList<qint64> Result;
	foreach(const SingleBbgRequest* SingleRequ, ResultTable) {
		if (SingleRequ->GetSecurity() == Secur.trimmed().toUpper()) Result.append(SingleRequ->GetResultID());
	}
	return Result;
}
QList<qint64> BloombergRequest::FindField(const QString& Field)const {
	QList<qint64> Result;
	foreach(const SingleBbgRequest* SingleRequ, ResultTable) {
		if (SingleRequ->GetField() == Field.trimmed().toUpper()) Result.append(SingleRequ->GetResultID());
	}
	return Result;
}
QList<qint64> BloombergRequest::FindSecurityField(const QString& Secur, const QString& Field)const {
	QList<qint64> Result;
	foreach(const SingleBbgRequest* SingleRequ, ResultTable) {
		if (SingleRequ->GetField() == Field.trimmed().toUpper() && SingleRequ->GetSecurity() == Secur.trimmed().toUpper()) Result.append(SingleRequ->GetResultID());
	}
	return Result;
}
QList<qint64> BloombergRequest::IdList() const {
	return ResultTable.keys();
}
void BloombergRequest::AddRequest(qint64 ID, const QString& Secur, const QString& Field, YellowKeys YellowKey) {
	SingleBbgRequest Temp;
	Temp.SetField(Field);
	Temp.SetSecurity(Secur);
	Temp.SetResultID(ID);
	Temp.SetExtension(YellowKey);
	AddRequest(Temp);
}
void BloombergRequest::AddRequest(const QString& Secur, const QString& Field, YellowKeys YellowKey) { AddRequest(++MaxID, Secur, Field, YellowKey); }
void BloombergRequest::AddRequest(qint64 ID, const QString& Secur, const QString& Field, const QMap<QString, QString>& Overrides, YellowKeys YellowKey) {
	SingleBbgRequest Temp;
	Temp.SetField(Field);
	Temp.SetSecurity(Secur);
	Temp.SetResultID(ID);
	Temp.SetOverrides(Overrides);
	Temp.SetExtension(YellowKey);
	AddRequest(Temp);
}
void BloombergRequest::AddRequest(const QString& Secur, const QString& Field, const QMap<QString, QString>& Overrides, YellowKeys YellowKey) { AddRequest(++MaxID, Secur, Field, Overrides, YellowKey); }
QHash<qint64, QList<qint64> >  BloombergRequest::RequestGroups(qint64 StartingID)const {
	QHash<qint64, QList<qint64> > Result;
	QList<qint64> UsedIDs;
	QStringList UsedFields;
	for (QHash<qint64, SingleBbgRequest*>::const_iterator MainIter = ResultTable.constBegin(); MainIter != ResultTable.constEnd(); MainIter++) {
		if (UsedIDs.contains(MainIter.key())) continue;
		UsedIDs.append(MainIter.key());
		UsedFields.clear();
		UsedFields.append(MainIter.value()->GetField());
		Result.insert(StartingID, QList<qint64>());
		Result[StartingID].append(MainIter.value()->GetResultID());
		for (QHash<qint64, SingleBbgRequest*>::const_iterator SecondIter = MainIter + 1; SecondIter != ResultTable.constEnd(); SecondIter++) {
			if (MainIter.value()->GetFullSecurity() == SecondIter.value()->GetFullSecurity()) {
				if (MainIter.value()->SameOverrides(*SecondIter.value())) {
					UsedIDs.append(SecondIter.value()->GetResultID());
					Result[StartingID].append(SecondIter.value()->GetResultID());
					UsedFields.append(SecondIter.value()->GetField());
				}
			}
		}
		for (QHash<qint64, SingleBbgRequest*>::const_iterator SecondIter = MainIter + 1; SecondIter != ResultTable.constEnd(); SecondIter++) {
			if (MainIter.value()->GetSecurity() != SecondIter.value()->GetSecurity()) {
				if (UsedFields.contains(SecondIter.value()->GetField())) {
					if (MainIter.value()->SameOverrides(*SecondIter.value())) {
						UsedIDs.append(SecondIter.value()->GetResultID());
						Result[StartingID].append(SecondIter.value()->GetResultID());
					}
				}
			}	
		}
		StartingID++;
	}
	return Result;
}
QString BloombergRequest::YellowKey2String(YellowKeys a) {
		switch (a) {
		case Govt: return "Govt";
		case Corp: return "Corp";
		case Mtge: return "Mtge";
		case MMkt: return "MMkt";
		case Muni: return "Muni";
		case Pfd: return "Pfd";
		case Equity: return "Equity";
		case Comdty: return "Comdty";
		case Index: return "Index";
		case Curncy: return "Curncy";
		default: return "";
		}
}
BloombergRequest::YellowKeys BloombergRequest::String2YellowKey(const QString& a) {
	if (a.compare("Govt", Qt::CaseInsensitive) == 0) return Govt;
	else if (a.compare("Corp", Qt::CaseInsensitive) == 0) return Corp;
	else if (a.compare("Mtge", Qt::CaseInsensitive) == 0) return Mtge;
	else if (a.compare("MMkt", Qt::CaseInsensitive) == 0) return MMkt;
	else if (a.compare("Muni", Qt::CaseInsensitive) == 0) return Muni;
	else if (a.compare("Pfd", Qt::CaseInsensitive) == 0) return Pfd;
	else if (a.compare("Equity", Qt::CaseInsensitive) == 0) return Equity;
	else if (a.compare("Comdty", Qt::CaseInsensitive) == 0) return Comdty;
	else if (a.compare("Index", Qt::CaseInsensitive) == 0) return Index;
	else if (a.compare("Curncy", Qt::CaseInsensitive) == 0) return Curncy;
	else return Invalid;
}

void BloombergRequest::SetAutoConstantRates(bool val) {
	for (QHash<qint64, SingleBbgRequest*>::iterator i = ResultTable.begin(); i != ResultTable.end(); i++) {
		(*i)->SetAutoConstantRates(val);
	}
	m_AutoConstantRates = val;
}

#endif