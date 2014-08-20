#ifndef NO_BLOOMBERG
#include "QBbgRequest.h"
#include "QSingleBbgRequest.h"
#include <QDataStream>
using namespace QBloombergLib;
QBbgRequest::QBbgRequest()
	: m_AutoConstantRates(false)
	, MaxID(std::numeric_limits<qint64>::min()) {}
qint32 QBbgRequest::NumRequests() const { return ResultTable.size(); }
bool QBbgRequest::GetAutoConstantRates() const { return m_AutoConstantRates; }
QBbgRequest::~QBbgRequest() {
	ClearRequests();
}
QBbgRequest::QBbgRequest(const QBbgRequest& a)
	:MaxID(a.MaxID)
	, m_AutoConstantRates(a.m_AutoConstantRates) {
	for (QHash<qint64, QSingleBbgRequest*>::const_iterator i = a.ResultTable.constBegin(); i != a.ResultTable.constEnd(); i++) {
		ResultTable.insert(i.key(), new QSingleBbgRequest(*(i.value())));
	}
}
QBbgRequest& QBbgRequest::operator= (const QBbgRequest& a) {
	MaxID = a.MaxID;
	m_AutoConstantRates = a.m_AutoConstantRates;
	for (QHash<qint64, QSingleBbgRequest*>::const_iterator i = a.ResultTable.constBegin(); i != a.ResultTable.constEnd(); i++) {
		ResultTable.insert(i.key(), new QSingleBbgRequest(*(i.value())));
	}
	return *this;
}
void QBbgRequest::ClearRequests() {
	for (QHash<qint64, QSingleBbgRequest*>::iterator i = ResultTable.begin(); i != ResultTable.end(); i++) {
		delete i.value();
	}
	ResultTable.clear();
}

void QBbgRequest::AddRequest(const QSingleBbgRequest& a) {
	if (ResultTable.contains(a.GetResultID())) return;
	for (QHash<qint64, QSingleBbgRequest*>::const_iterator i = ResultTable.constBegin(); i != ResultTable.constEnd(); i++) {
		if (i.value()->operator==(a)) return;
	}
	QSingleBbgRequest* TempRes=new QSingleBbgRequest(a);
	if (m_AutoConstantRates) {
		TempRes->SetAutoConstantRates(true);
	}
	if (TempRes->GetResultID() < 1i64) {
		++MaxID;
		TempRes->SetResultID((MaxID<1i64) ? 1i64 : MaxID);
	}
	if (TempRes->GetResultID() > MaxID)
		MaxID = TempRes->GetResultID();
	ResultTable.insert(TempRes->GetResultID(), TempRes);
}
const QSingleBbgRequest* QBbgRequest::FindRequest(qint64 ID) const {
	return ResultTable.value(ID, NULL);
}
const QSingleBbgRequest* QBbgRequest::GetRequest(quint32 Index) const {
	if (Index > ResultTable.size()) return NULL;
	return (ResultTable.begin() + Index).value();
}
QSingleBbgRequest* QBbgRequest::GetEditRequest(quint32 Index) {
	if (Index > ResultTable.size()) return NULL;
	return (ResultTable.begin() + Index).value();
}
QSingleBbgRequest* QBbgRequest::FindEditRequest(qint64 ID) {
	if (ResultTable.contains(ID))
		return ResultTable[ID];
	return NULL;
}
bool QBbgRequest::IsValidReq() const {
	for (QHash<qint64, QSingleBbgRequest*>::const_iterator i = ResultTable.constBegin(); i != ResultTable.constEnd(); i++) {
		if (!i.value()->IsValidReq()) return false;
	}
	return true;
}
QList<qint64> QBbgRequest::FindSecurity(QString Secur)const {
	QList<qint64> Result;
	Secur = Secur.trimmed().toUpper();
	for (QHash<qint64, QSingleBbgRequest*>::const_iterator i = ResultTable.constBegin(); i != ResultTable.constEnd(); i++) {
		if (i.value()->GetSecurity() == Secur || i.value()->GetFullSecurity().toUpper() == Secur) Result.append(i.key());
	}
	return Result;
}
QList<qint64> QBbgRequest::FindField(QString Field)const {
	QList<qint64> Result;
	Field = Field.trimmed().toUpper();
	Field.replace(" ", "_");
	for (QHash<qint64, QSingleBbgRequest*>::const_iterator i = ResultTable.constBegin(); i != ResultTable.constEnd(); i++) {
		if (i.value()->GetField() == Field) Result.append(i.key());
	}
	return Result;
}
QList<qint64> QBbgRequest::FindSecurityField(QString Secur, QString Field)const {
	QList<qint64> Result;
	Secur = Secur.trimmed().toUpper();
	Field = Field.trimmed().toUpper();
	Field.replace(" ", "_");
	for (QHash<qint64, QSingleBbgRequest*>::const_iterator i = ResultTable.constBegin(); i != ResultTable.constEnd(); i++) {
		if (
			i.value()->GetField() == Field && 
			(i.value()->GetSecurity() == Secur || i.value()->GetFullSecurity().toUpper() == Secur)
		) Result.append(i.key());
	}
	return Result;
}
QList<qint64> QBbgRequest::IdList() const {
	QList<qint64> Result;
	for (QHash<qint64, QSingleBbgRequest*>::const_iterator i = ResultTable.constBegin(); i != ResultTable.constEnd(); i++) {
		Result.append(i.key());
	}
	return Result;
}
void QBbgRequest::AddRequest(qint64 ID, const QString& Secur, const QString& Field, YellowKeys YellowKey) {
	QSingleBbgRequest Temp;
	Temp.SetField(Field);
	Temp.SetSecurity(Secur);
	Temp.SetResultID(ID);
	Temp.SetExtension(YellowKey);
	AddRequest(Temp);
}
void QBbgRequest::AddRequest(const QString& Secur, const QString& Field, YellowKeys YellowKey) { AddRequest(++MaxID, Secur, Field, YellowKey); }
void QBbgRequest::AddRequest(qint64 ID, const QString& Secur, const QString& Field, const QHash<QString, QString>& Overrides, YellowKeys YellowKey) {
	QSingleBbgRequest Temp;
	Temp.SetField(Field);
	Temp.SetSecurity(Secur);
	Temp.SetResultID(ID);
	Temp.SetOverrides(Overrides);
	Temp.SetExtension(YellowKey);
	AddRequest(Temp);
}
void QBbgRequest::AddRequest(const QString& Secur, const QString& Field, const QHash<QString, QString>& Overrides, YellowKeys YellowKey) { AddRequest(++MaxID, Secur, Field, Overrides, YellowKey); }
void  QBbgRequest::RequestGroups(QHash<qint64, QList<qint64>* >& Result, qint64 StartingID)const {
	for (QHash<qint64, QList<qint64>* >::iterator i = Result.begin(); i != Result.end(); i++)
		delete (i.value());
	Result.clear();
	QList<qint64> UsedIDs;
	QList<QString> UsedFields;
	for (QHash<qint64, QSingleBbgRequest*>::const_iterator MainIter = ResultTable.constBegin(); MainIter != ResultTable.constEnd(); MainIter++) {
		if (UsedIDs.contains(MainIter.key()))continue;
		UsedIDs.append(MainIter.key());
		UsedFields.clear();
		UsedFields.push_back(MainIter.value()->GetField());
		QList<qint64>* CurrentGroup=new QList<qint64>();
		CurrentGroup->push_back(MainIter.key());
		for (QHash<qint64, QSingleBbgRequest*>::const_iterator SecondIter = MainIter+1; SecondIter != ResultTable.constEnd(); SecondIter++) {
			if (MainIter.value()->GetFullSecurity() == SecondIter.value()->GetFullSecurity()) {
				if (MainIter.value()->SameOverrides(*SecondIter.value())) {
					UsedIDs.append(SecondIter.key());
					CurrentGroup->append(SecondIter.key());
					UsedFields.append(SecondIter.value()->GetField());
				}
			}
		}
		Result.insert(StartingID, CurrentGroup);
		StartingID++;
	}
	for (QHash<qint64, QList<qint64>* >::iterator MainIter = Result.begin(); MainIter != Result.end();) {
		if (MainIter.value()->empty()) {
			MainIter = Result.erase(MainIter);
			continue;
		}
		for (QHash<qint64, QList<qint64>* >::iterator SecondIter = MainIter+1; SecondIter != Result.end(); ) {
			if (SameRequest(*(MainIter.value()), *(SecondIter.value()))) {
				MainIter.value()->append(*(SecondIter.value()));
				delete SecondIter.value();
				SecondIter = Result.erase(SecondIter);
				continue;
			}
			SecondIter++;
		}
		MainIter++;
	}
}
QString QBbgRequest::YellowKey2String(YellowKeys a) {
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

qint32 QBbgRequest::GetErrorCode() const { return m_ErrorCode; }
bool QBbgRequest::HasErrors() const { return m_ErrorCode != QSingleBbgResult::NoErrors; }
void QBbgRequest::SetErrorCode(QSingleBbgResult::BbgErrorCodes val) {
	if (val == QSingleBbgResult::NoErrors) m_ErrorCode = QSingleBbgResult::NoErrors;
	else m_ErrorCode = m_ErrorCode | static_cast<qint32>(val);
}
QBbgRequest::YellowKeys QBbgRequest::String2YellowKey(QString a) {
	a=a.toLower();
	if (a == "govt") return Govt;
	else if (a == "corp") return Corp;
	else if (a == "mtge") return Mtge;
	else if (a == "mmkt") return MMkt;
	else if (a == "muni") return Muni;
	else if (a == "pfd") return Pfd;
	else if (a == "equity") return Equity;
	else if (a == "comdty") return Comdty;
	else if (a == "index") return Index;
	else if (a == "curncy") return Curncy;
	else return Invalid;
}

void QBbgRequest::SetAutoConstantRates(bool val) {
	for (QHash<qint64, QSingleBbgRequest*>::iterator i = ResultTable.begin(); i != ResultTable.end(); i++) {
		i.value()->SetAutoConstantRates(val);
	}
	m_AutoConstantRates = val;
}

bool QBbgRequest::SameRequest(const QList<qint64>& a, const QList<qint64>& b) const {
	QList<QString> FiledsA;
	QList<QString> FiledsB;
	if (a.empty()) return false;
	if (b.empty()) return false;
	if (!FindRequest(*a.begin())->SameOverrides(*FindRequest(*b.begin()))) return false;
	for (QList<qint64>::const_iterator i = a.constBegin(); i != a.constEnd(); i++) {
		FiledsA.append(FindRequest(*i)->GetField());
	}
	for (QList<qint64>::const_iterator i = b.constBegin(); i != b.constEnd(); i++) {
		FiledsB.append(FindRequest(*i)->GetField());
	}
	if (FiledsA.size() != FiledsB.size()) return false;
	for (QList<QString>::const_iterator i = FiledsA.constBegin(); i != FiledsA.constEnd(); i++) {
		if (!FiledsB.contains(*i)) return false;
	}
	return true;
}
QDataStream& operator>>(QDataStream& stream, QBbgRequest& flows) {
	qint16 CheckVersion;
	stream >> CheckVersion;
	if (CheckVersion<MinimumSupportedVersion || CheckVersion>ModelVersionNumber) return stream;
	flows.SetLoadProtocolVersion(CheckVersion);
	return flows.LoadOldVersion(stream);
}
QDataStream& QBbgRequest::LoadOldVersion(QDataStream& stream) {
	quint32 ListSize;
	stream >> m_ErrorCode >> ListSize >> m_AutoConstantRates;
	QSingleBbgRequest TempReq;
	for (quint32 i = 0; i < ListSize; i++) {
		stream >> TempReq;
		ResultTable.insert(TempReq.GetResultID(),new QSingleBbgRequest(TempReq));
	}
	ResetProtocolVersion();
	return stream;
}
QDataStream& operator<<(QDataStream & stream, const QBbgRequest& flows) {
	stream
		<< static_cast<qint16>(ModelVersionNumber)
		<< flows.m_ErrorCode
		<< static_cast<quint32>(flows.ResultTable.size())
		<< flows.m_AutoConstantRates
		;
	for (QHash<qint64, QBloombergLib::QSingleBbgRequest*>::const_iterator i = flows.ResultTable.constBegin(); i != flows.ResultTable.constEnd(); i++) {
		stream << (*(i.value()));
	}
	return stream;
}

#endif