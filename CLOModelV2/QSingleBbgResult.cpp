#ifndef NO_BLOOMBERG
#include "QSingleBbgResult.h"
#include <QDataStream>
using namespace QBloombergLib;
QSingleBbgResult::QSingleBbgResult()
	: m_ErrorCode(NoErrors)
	, TableResultCols(0) {
	m_StringValue=nullptr;
	m_DateValue = nullptr;
	m_DoubleValue = nullptr;
}
qint32 QSingleBbgResult::GetErrorCode() const { return m_ErrorCode; }
bool QSingleBbgResult::HasErrors() const { return m_ErrorCode != NoErrors; }
void QSingleBbgResult::SetErrorCode(BbgErrorCodes val) {
	if (val == NoErrors) m_ErrorCode = NoErrors;
	else m_ErrorCode = m_ErrorCode | static_cast<qint32>(val);
}
QSingleBbgResult::QSingleBbgResult(const QSingleBbgResult& a)
	: TableResultCols(0) {
	operator=(a);
}
QString QSingleBbgResult::BbgErrorCodes2String(qint32 a) {
	if (a == 0) return "NoErrors";
	QString Result = "";
	if (a & ResponseError) Result += "Response Error, ";
	if (a & SecurityError) Result += "Invalid Security, ";
	if (a & InvalidInputs) Result += "Invalid Inputs, ";
	if (a & SessionError) Result += "Session Error, ";
	if (a & ServiceError) Result += "Service Error, ";
	if (a & FieldError) Result += "Invalid Field, ";
	if (a & UnknownError) Result += "Unknown Error, ";
	if (a & SessionStopped) Result += "Session Stopped, ";
	if (a & NoData) Result += "No Data, ";
	return Result.remove(Result.length() - 2,2);
}
QString QSingleBbgResult::GetErrorString() const {
	return BbgErrorCodes2String(m_ErrorCode);
}
QSingleBbgResult& QSingleBbgResult::operator= (const QSingleBbgResult& a) {
	Clear();
	m_ErrorCode = a.m_ErrorCode;
	m_Header = a.m_Header;
	if (a.m_StringValue) m_StringValue=new QString(*(a.m_StringValue));
	if (a.m_DateValue) m_DateValue= new QDate(*(a.m_DateValue));
	if (a.m_DoubleValue) m_DoubleValue=new double(*(a.m_DoubleValue));
	if (a.HasTable()) {
		TableResultCols = a.TableResultCols;
		for (QList<QSingleBbgResult*>::const_iterator i = m_TableResultRows.constBegin(); i != m_TableResultRows.constEnd(); i++) {
			QSingleBbgResult* TableRow=new QSingleBbgResult[TableResultCols];
			for (quint32 j = 0; j < TableResultCols; j++) {
				if ((*i)[j].m_StringValue)
					TableRow[j].SetValue(*((*i)[j].m_StringValue), (*i)[j].m_Header);
			}
			m_TableResultRows.append(TableRow);
		}
	}
	else {
		TableResultCols = 0;
	}
	return *this;
}

QSingleBbgResult::~QSingleBbgResult() {
	Clear();
}

void QSingleBbgResult::Clear() {

	if (m_StringValue) delete m_StringValue;
	if (m_DateValue)delete m_DateValue;
	if (m_DoubleValue)delete m_DoubleValue;
	while (!m_TableResultRows.isEmpty()) {
		delete[] m_TableResultRows.takeFirst();
	}

	m_StringValue = nullptr;
	m_DateValue = nullptr;
	m_DoubleValue = nullptr;
	TableResultCols = 0;
	m_Header = "";
	m_ErrorCode = NoErrors;
}
void QSingleBbgResult::SetValue(const QString& val, const QString& Header) {
	if (val.isEmpty()) return;
	Clear();
	m_Header = Header;
	m_StringValue= new QString(val);
	QDate TempDate = QDate::fromString(val, "yyyy-MM-dd");
	if (!TempDate.isNull()) {
		m_DateValue = new QDate(TempDate);
	}
	bool DoubleCheck=false;
	double TempDouble = val.toDouble(&DoubleCheck);
	if (DoubleCheck) {
		m_DoubleValue = new double(TempDouble);
	}	
}
void QSingleBbgResult::AddValueRow(const QList<QString>&  val, const QList<QString>&  Headers) {
	if (val.isEmpty()) return;
	bool GoodHead = Headers.size() == val.size();
	if (m_TableResultRows.empty()) {
		Clear();
		TableResultCols = val.size();
	}
	else {
		if (TableResultCols != val.size()) return;
	}
	QSingleBbgResult* TableRow=new QSingleBbgResult[TableResultCols];
	for (quint32 i = 0; i < TableResultCols; i++) {
		TableRow[i].SetValue(val.at(i), GoodHead ? Headers.at(i) : "");
	}
	m_TableResultRows.append(TableRow);
}

const QSingleBbgResult* QSingleBbgResult::GetTableResult(quint32 r, quint32 c) const {
	if (r >= m_TableResultRows.size() || c >= TableResultCols)
		return nullptr;
	QSingleBbgResult* const & TempRow = m_TableResultRows.at(r);
	return TempRow+c;

}

qint32 QSingleBbgResult::GetNumCols() const {
	return TableResultCols;
}

qint32 QSingleBbgResult::GetNumRows() const {
	return m_TableResultRows.size();
}

double QSingleBbgResult::GetDouble() const {
	if (m_DoubleValue) return *m_DoubleValue; else return 0.0;
}

QDate QSingleBbgResult::GetDate() const {
	if (m_DateValue) return *m_DateValue; else return QDate();
}

QString QSingleBbgResult::GetString() const {
	if (m_StringValue) return *m_StringValue; else return QString();
}

const QString& QSingleBbgResult::GetHeader() const {
	return m_Header;
}

bool QSingleBbgResult::HasValue() const {
	return HasString();
}

bool QSingleBbgResult::HasTable() const {
	return (!m_TableResultRows.empty()) && TableResultCols > 0;
}

bool QSingleBbgResult::HasDouble() const {
	return !m_DoubleValue;
}

bool QSingleBbgResult::HasDate() const {
	return !m_DateValue;
}

bool QSingleBbgResult::HasString() const {
	return !m_StringValue;
}

bool QSingleBbgResult::IsEmpty() const {
	return !HasString() && !HasTable();
}


QDataStream& operator<<(QDataStream& stream, const QBloombergLib::QSingleBbgResult& flows) {
	stream << static_cast<qint16>(ModelVersionNumber)
		<< flows.HasString()
		<< flows.HasDate()
		<< flows.HasDouble()
		<< static_cast<quint32>(flows.m_TableResultRows.size())
		<< flows.TableResultCols
		<< flows.m_ErrorCode
		<< flows.m_Header
		;
	if (flows.HasString()) stream << *(flows.m_StringValue);
	if (flows.HasDate()) stream << *(flows.m_DateValue);
	if (flows.HasDouble()) stream << *(flows.m_DoubleValue);
	foreach(const QSingleBbgResult* TblElement, flows.m_TableResultRows) {
		for (int j = 0; j < flows.TableResultCols; j++) {
			stream << *(TblElement + j);
		}
	}
	return stream;
}
QDataStream& operator>>(QDataStream& stream, QBloombergLib::QSingleBbgResult& flows) {
	qint16 CheckVersion;
	stream >> CheckVersion;
	if (CheckVersion<MinimumSupportedVersion || CheckVersion>ModelVersionNumber) return stream;
	flows.SetLoadProtocolVersion(CheckVersion);
	return flows.LoadOldVersion(stream);
}
QDataStream& QSingleBbgResult::LoadOldVersion(QDataStream& stream) {
	Clear();
	bool DateFound, StringFound, DoubleFound;
	quint32 Rows;
	QString TempStr;
	QDate TempDte;
	double Tempdbl;
	stream
		>> StringFound
		>> DateFound
		>> DoubleFound
		>> Rows
		>> TableResultCols
		>> m_ErrorCode
		>> m_Header
		;
	if (StringFound) {
		stream >> TempStr;
		m_StringValue = new QString(TempStr);
	}
	if (DateFound) {
		stream >> TempDte;
		m_DateValue = new QDate(TempDte);
	}
	if (DoubleFound) {
		stream >> Tempdbl;
		m_DoubleValue = new double(Tempdbl);
	}
	QSingleBbgResult* TempRes;
	for (int i = 0; i < Rows; i++) {
		TempRes = new QSingleBbgResult[TableResultCols];
		for (int j = 0; j < TableResultCols; j++) {
			stream >> TempRes[j];
		}
		m_TableResultRows.append(TempRes);
	}
	return stream;
}

void QBloombergLib::QSingleBbgResult::SetHeader(const QString& Header /*= QString()*/) {
	m_Header = Header;
}


#endif