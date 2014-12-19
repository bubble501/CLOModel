#include "LoanAssumption.h"
#include "AbstractBbgVect.h"
LoanAssumption::LoanAssumption(const LoanAssumption& a) 
	:m_ScenarioName(a.m_ScenarioName)
	, m_Aliases(a.m_Aliases)
{
	for (int j = 0; j < SenioritySize; j++) {
		for (int i = 0; i < AssumptionTypeSize; i++) {
			m_Assumptions[i][j] = a.m_Assumptions[i][j];
		}
		m_LastUpdate[j] = a.m_LastUpdate[j];
	}
}
LoanAssumption::LoanAssumption(const QString& ScenarioName)
	:m_ScenarioName(ScenarioName)
{}

bool LoanAssumption::MatchPattern(const QString& Pat) const {
	if (Pat.isEmpty())return false;
	QString Pattern("(?:");
	if (QRegExp(m_ScenarioName).isValid()) Pattern+="(?:\\b" + m_ScenarioName + "\\b)";
	for (auto i = m_Aliases.constBegin(); i != m_Aliases.constEnd(); ++i) {
		Pattern +="|(?:\\b" + *i + "\\b)";
	}
	Pattern += ')';
	if (Pattern == "(?:)") return false;
	QRegExp MatchFinder(Pattern, Qt::CaseInsensitive);
	return MatchFinder.indexIn(Pat) >= 0;
}

LoanAssumption& LoanAssumption::operator=(const LoanAssumption& a) {
	for (int j = 0; j < SenioritySize; j++) {
		for (int i = 0; i < AssumptionTypeSize; i++) {
			m_Assumptions[i][j] = a.m_Assumptions[i][j];
		}
		m_LastUpdate[j] = a.m_LastUpdate[j];
	}
	m_ScenarioName=a.m_ScenarioName;
	m_Aliases=a.m_Aliases;
	return *this;
}

QString LoanAssumption::GetAssumption(AssumptionType at, Seniority sn) const {
	QString Result( m_Assumptions[at][sn].trimmed());
	if (m_LastUpdate[sn].isNull()) return Result;
	QRegExp AnchorMatch(QString("^(?:A\\s+") + VectorAnchorDateFormat + "\\s+)");
	if (AnchorMatch.indexIn(Result) >= 0) return Result;
	return "A " + m_LastUpdate[sn].toString("MM/dd/yyyy") +' '+ Result;
}

void LoanAssumption::SetAliases(const QString& val) {
	ClearAliases();
	auto TempAl = val.split("$,$");
	for (auto i = TempAl.constBegin(); i != TempAl.constEnd();++i) {
		AddAlias(*i);
	}
}

void LoanAssumption::AddAlias(const QString& val) {
	if (
		QRegExp(val).isValid()
		&& !m_Aliases.contains(val, Qt::CaseInsensitive)
	) m_Aliases.append(val);
}

QString LoanAssumption::GetRawAssumption(AssumptionType at, Seniority sn) const {
	return m_Assumptions[at][sn].trimmed();
}
