#ifndef NO_BLOOMBERG
#include "QSingleBbgRequest.h"
#include <QList> 
#include <QHash>
#include <QDataStream>
using namespace QBloombergLib;
void QSingleBbgRequest::SetSecurity(const QString& val) {
	m_Security = val.trimmed().toUpper();
}
void QSingleBbgRequest::SetField(const QString& val) {
	m_Field = val.trimmed().toUpper();
	m_Field.replace(" ", "_");
}
QSingleBbgRequest::QSingleBbgRequest()
	:m_ResultID(-1i64)
	, m_AutoConstantRates(false)
	, m_Extension(QBbgRequest::Mtge) {}
QSingleBbgRequest::QSingleBbgRequest(const QSingleBbgRequest& a)
	: m_ResultID(a.m_ResultID)
	, m_Security(a.m_Security)
	, m_Field(a.m_Field)
	, m_Overrides(a.m_Overrides)
	, m_AutoConstantRates(a.m_AutoConstantRates)
	, m_Extension(a.m_Extension) {}
QSingleBbgRequest& QSingleBbgRequest::operator=(const QSingleBbgRequest& a) {
	m_ResultID = a.m_ResultID;
	m_Security = a.m_Security;
	m_Field = a.m_Field;
	m_Overrides = a.m_Overrides;
	m_AutoConstantRates = a.m_AutoConstantRates;
	m_Extension = a.m_Extension;
	return *this;
}
void QSingleBbgRequest::SetExtension(const QString& val) {
	m_Extension = QBbgRequest::String2YellowKey(val.trimmed());
}

void QSingleBbgRequest::SetOverrides(const Overrides& Overr) {
	ClearOverrides();
	for (auto i = Overr.constBegin(); i != Overr.constEnd(); i++) {
		if (i.value().isEmpty()) continue;
		if (i.key().isEmpty()) continue;
		QString TempKey = i.key().trimmed().toUpper();
		TempKey.replace(" ", "_");
		m_Overrides.insert(TempKey, i.value().trimmed());
	}
}
void QSingleBbgRequest::SetOverride(QString Name, const QString& Value) {
	Name = Name.trimmed().toUpper();
	Name.replace(" ", "_");
	m_Overrides.insert(Name,Value.trimmed());
}
void QSingleBbgRequest::ClearOverrides() {
	m_Overrides.clear();
	if (m_AutoConstantRates) {
		if (!m_Overrides.contains("APPLY_FWD_RATE")) {
			m_Overrides.insert("APPLY_FWD_RATE", "N");
		}
	}
}
void QSingleBbgRequest::SetAutoConstantRates(bool val) {
	m_AutoConstantRates = val;
	if (m_AutoConstantRates) {
		if (!m_Overrides.contains("APPLY_FWD_RATE")) {
			m_Overrides.insert("APPLY_FWD_RATE", "N");
		}
	}
	else {
		auto CheckIter = m_Overrides.find("APPLY_FWD_RATE");
		if (CheckIter != m_Overrides.end()) {
			if (CheckIter.value() == "N") {
				m_Overrides.erase(CheckIter);
			}
		}
	}
}
bool QSingleBbgRequest::SameOverrides(const QSingleBbgRequest& a)const { return m_Overrides == a.m_Overrides; }

bool QSingleBbgRequest::IsValidReq() const {
	return
		!m_Field.isEmpty()
		&& !m_Security.isEmpty()
		&& m_Extension != QBbgRequest::Invalid
		;
}


bool QSingleBbgRequest::operator==(const QSingleBbgRequest& a) const {
	if (m_Security != a.m_Security) return false;
	if (m_Field != a.m_Field) return false;
	if (m_Extension != a.m_Extension) return false;
	return m_Overrides == a.m_Overrides;
}

qint64 QSingleBbgRequest::GetResultID() const {
	return m_ResultID;
}

const QString& QSingleBbgRequest::GetSecurity() const {
	return m_Security;
}

const QString& QSingleBbgRequest::GetField() const {
	return m_Field;
}
bool QSingleBbgRequest::GetAutoConstantRates() const {
	return m_AutoConstantRates;
}

void QSingleBbgRequest::SetResultID(qint64 val) {
	m_ResultID = val;
}

const Overrides& QSingleBbgRequest::GetOverrides() const {
	return m_Overrides;
}

QString QSingleBbgRequest::GetExtensionString() const {
	return QBbgRequest::YellowKey2String(m_Extension);
}

QString QSingleBbgRequest::GetFullSecurity() const {
	return m_Security + ' ' + QBbgRequest::YellowKey2String(m_Extension);
}

QBbgRequest::YellowKeys QSingleBbgRequest::GetExtension() const {
	return m_Extension;
}

void QSingleBbgRequest::SetExtension(QBbgRequest::YellowKeys val) {
	m_Extension = val;
}
QDataStream& operator<<(QDataStream & stream, const QSingleBbgRequest& flows) {
	stream << static_cast<qint16>(ModelVersionNumber)
		<< flows.m_ResultID
		<< flows.m_Security
		<< flows.m_Field
		<< flows.m_AutoConstantRates
		<< static_cast<quint32>(flows.m_Overrides.size())
		;
	for (auto i = flows.m_Overrides.constBegin(); i != flows.m_Overrides.constEnd(); i++) {
		stream << i.key()
			<< i.value()
			;
	}
	return stream;
}
QDataStream& operator>>(QDataStream & stream, QSingleBbgRequest& flows) {
	qint16 CheckVersion;
	stream >> CheckVersion;
	if (CheckVersion<MinimumSupportedVersion || CheckVersion>ModelVersionNumber) return stream;
	flows.SetLoadProtocolVersion(CheckVersion);
	return flows.LoadOldVersion(stream);
}
QDataStream& QSingleBbgRequest::LoadOldVersion(QDataStream& stream) {
	quint32 TempSize;
	QString TempStr, TempStr2;
	stream >> m_ResultID;
	stream >> m_Security;
	stream >> m_Field;
	stream 
		>> m_AutoConstantRates
		>> TempSize
		;
	for (quint32 i = 0; i < TempSize; i++) {
		stream >> TempStr >> TempStr2;
		m_Overrides.insert(TempStr, TempStr2);
	}
	return stream;
}


#endif