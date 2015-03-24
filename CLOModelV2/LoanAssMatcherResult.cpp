#include "LoanAssMatcherResult.h"
#include <QDataStream>
LoanAssMatcherResult& LoanAssMatcherResult::operator=(const LoanAssMatcherResult& a) {
	m_DealName = a.m_DealName;
	m_LoanID = a.m_LoanID;
	m_Issuer = a.m_Issuer;
	m_Facility = a.m_Facility;
	m_CurrScen = a.m_CurrScen;
	m_DetectedScen = a.m_DetectedScen;
	m_FilePath = a.m_FilePath;
	return *this;
}

LoanAssMatcherResult::LoanAssMatcherResult(const LoanAssMatcherResult& a) 
    : m_DealName(a.m_DealName)
    , m_FilePath(a.m_FilePath)
    , m_LoanID(a.m_LoanID)
    , m_Issuer(a.m_Issuer)
    , m_Facility(a.m_Facility)
    , m_CurrScen(a.m_CurrScen)
    , m_DetectedScen(a.m_DetectedScen)
{}

void LoanAssMatcherResult::AddScenario(const QString& DetectedScen, const QString& CurrScen, const QString& Facility, const QString& Issuer, qint32 LoanID) {
	for (int i = 0; i < m_LoanID.size(); ++i) {
		if (m_LoanID.at(i) == LoanID) {
			RemoveScenario(i);
			break;
		}
	}
	m_DetectedScen.append(DetectedScen);
	m_CurrScen.append(CurrScen);
	m_Facility.append(Facility);
	m_Issuer.append(Issuer);
	m_LoanID.append(LoanID);
}

void LoanAssMatcherResult::RemoveScenario(int index) {
	if (index < 0 || index >= ScenarioCount())
		m_DetectedScen.removeAt(index);
	m_CurrScen.removeAt(index);
	m_Facility.removeAt(index);
	m_Issuer.removeAt(index);
	m_LoanID.removeAt(index);
}

void LoanAssMatcherResult::Clear() {
	m_FilePath.clear();
	m_DealName.clear();
	m_DetectedScen.clear();
	m_CurrScen.clear();
	m_Facility.clear();
	m_Issuer.clear();
	m_LoanID.clear();
}

QString LoanAssMatcherResult::GetDetectScen(int index) const {
	if (index < 0 || index >= m_DetectedScen.size()) return QString(); return m_DetectedScen.at(index);
}

QString LoanAssMatcherResult::GetCurrScen(int index) const {
	if (index < 0 || index >= m_CurrScen.size()) return QString(); return m_CurrScen.at(index);
}

QString LoanAssMatcherResult::GetFacility(int index) const {
	if (index < 0 || index >= m_Facility.size()) return QString(); return m_Facility.at(index);
}

QString LoanAssMatcherResult::GetIssuer(int index) const {
	if (index < 0 || index >= m_Issuer.size()) return QString(); return m_Issuer.at(index);
}

qint32 LoanAssMatcherResult::GetLoanID(int index) const {
	if (index < 0 || index >= m_LoanID.size()) return -1; return m_LoanID.at(index);
}

QDataStream& LoanAssMatcherResult::LoadOldVersion(QDataStream & stream) {
	stream
		>> m_FilePath
		>> m_DealName
		>> m_DetectedScen
		>> m_CurrScen
		>> m_Facility
		>> m_Issuer
		>> m_LoanID
	;
	ResetProtocolVersion();
	return stream;
}

QDataStream& operator<<(QDataStream & stream, const LoanAssMatcherResult& flows) {
	stream
		<< flows.m_FilePath
		<< flows.m_DealName
		<< flows.m_DetectedScen
		<< flows.m_CurrScen
		<< flows.m_Facility
		<< flows.m_Issuer
		<< flows.m_LoanID
	;
	return stream;
}

QDataStream& operator>>(QDataStream & stream, LoanAssMatcherResult& flows) {
	return flows.LoadOldVersion(stream);
}
