#ifndef NO_BLOOMBERG
#include "SingleBbgRequest.h"
SingleBbgRequest::SingleBbgRequest()
	:m_ResultID(-1)
	, m_ErrorCode(BloombergRequest::NoErrors)
	, m_AutoConstantRates(false)
	, m_Extension(BloombergRequest::Mtge) 
{}
SingleBbgRequest::SingleBbgRequest(const SingleBbgRequest& a)
	: m_ResultID(a.m_ResultID)
	, m_Security(a.m_Security)
	, m_Field(a.m_Field)
	, m_ErrorCode(a.m_ErrorCode)
	, m_Value(a.m_Value)
	, m_Overrides(a.m_Overrides)
	, m_AutoConstantRates(a.m_AutoConstantRates)
	, m_Extension(a.m_Extension) {}
void SingleBbgRequest::SetExtension(const QString& val) {
	m_Extension =  BloombergRequest::String2YellowKey( val.trimmed());
}

void SingleBbgRequest::SetOverrides(const QMap<QString, QString>& Overrides) {
	ClearOverrides();
	for (QMap<QString, QString>::const_iterator i = Overrides.constBegin(); i != Overrides.constEnd(); i++) {
		QString Temp = i.key();
		m_Overrides.insert(Temp.replace(' ', '_').trimmed().toUpper(), i.value().trimmed().toUpper());
	}
}
void SingleBbgRequest::SetOverride(QString Name, const QString& Value) {
	Name = Name.replace(' ', '_').trimmed().toUpper();
	m_Overrides[Name] = Value;
}
void SingleBbgRequest::ClearOverrides() {
	m_Overrides.clear();
	if (m_AutoConstantRates) {
		if (!m_Overrides.contains("APPLY_FWD_RATE")) {
			m_Overrides.insert("APPLY_FWD_RATE", "N");
		}
	}
}
void SingleBbgRequest::SetAutoConstantRates(bool val) {
	m_AutoConstantRates = val;
	if (m_AutoConstantRates) {
		if (!m_Overrides.contains("APPLY_FWD_RATE")) {
			m_Overrides.insert("APPLY_FWD_RATE", "N");
		}
	}
	else {
		if (m_Overrides.value("APPLY_FWD_RATE") == "N") {
			m_Overrides.remove("APPLY_FWD_RATE");
		}
	}
}
QDataStream& operator<<(QDataStream & stream, const SingleBbgRequest& flows) {
	stream << flows.m_ResultID
		<< flows.m_Security
		<< flows.m_Field
		<< static_cast<qint32>(flows.m_ErrorCode)
		<< flows.m_Value
		<< flows.m_Overrides
		<< flows.m_AutoConstantRates
		;
	return stream;
}
QDataStream& operator>>(QDataStream & stream, SingleBbgRequest& flows) { return flows.LoadOldVersion(stream); }
QDataStream& SingleBbgRequest::LoadOldVersion(QDataStream& stream) {
	qint32 TempError;
	stream >> m_ResultID
		>> m_Security
		>> m_Field
		>> TempError
		>> m_Value
		>> m_Overrides
		>> m_AutoConstantRates
		;
	m_ErrorCode = static_cast<BloombergRequest::BbgErrorCodes>(TempError);
	return stream;
}
bool SingleBbgRequest::SameOverrides(const SingleBbgRequest& a)const { return m_Overrides == a.m_Overrides; }

bool SingleBbgRequest::IsValidReq() const {
	return
		!m_Field.isEmpty()
		&& !m_Security.isEmpty()
		&& !(m_Extension == BloombergRequest::Invalid)
		;
}
void SingleBbgRequest::SetErrorCode(BloombergRequest::BbgErrorCodes val) {
	if (val == BloombergRequest::NoErrors) m_ErrorCode = BloombergRequest::NoErrors;
	else m_ErrorCode = m_ErrorCode | static_cast<int>(val);
}
#endif