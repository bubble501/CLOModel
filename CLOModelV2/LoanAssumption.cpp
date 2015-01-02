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
	if (Result.isEmpty()) return Result;
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

bool LoanAssumption::operator==(const LoanAssumption& a) const {
	bool Result = m_ScenarioName == a.m_ScenarioName;
	for (int j = 0; j < SenioritySize && Result; j++) {
		Result = m_LastUpdate[j] == a.m_LastUpdate[j];
		for (int i = 0; i < AssumptionTypeSize && Result; i++) {
			Result=m_Assumptions[i][j] == a.m_Assumptions[i][j];
		}
	}
	Result = Result && m_Aliases.size() == a.m_Aliases.size();
	for (auto i = m_Aliases.constBegin(); i != m_Aliases.constEnd() && Result; ++i) {
		Result = a.m_Aliases.contains(*i);
	}
	for (auto i = a.m_Aliases.constBegin(); i != a.m_Aliases.constEnd() && Result; ++i) {
		Result = m_Aliases.contains(*i);
	}
	return Result;
}

QString LoanAssumption::GelAliasString() const {
	QString Result;
	for (auto i = m_Aliases.constBegin(); i != m_Aliases.constEnd(); ++i) {
		if (!Result.isEmpty())Result += "$,$";
		Result += *i;
	}
	return Result;
}

QDataStream& LoanAssumption::LoadOldVersion(QDataStream& stream) {
	qint32 OldAssumptionTypeSize, OldSenioritySize;
	stream >> OldAssumptionTypeSize >> OldSenioritySize
			>> m_ScenarioName >> m_Aliases;
	for (int i = 0; i < qMin(static_cast<qint32>(SenioritySize), OldSenioritySize); ++i) {
		stream >> m_LastUpdate[i];
	}
	for (int i = 0; i < qMin(static_cast<qint32>(AssumptionTypeSize), OldAssumptionTypeSize); ++i) {
		for (int j = 0; j < qMin(static_cast<qint32>(SenioritySize), OldSenioritySize); ++j) {
			stream >> m_Assumptions[i][j];
		}
	}
	ResetProtocolVersion();
	return stream;
}

QDataStream& operator<<(QDataStream & stream, const LoanAssumption& flows) {
	stream << static_cast<qint32>(flows.AssumptionTypeSize) << static_cast<qint32>(flows.SenioritySize)
		<< flows.m_ScenarioName
		<< flows.m_Aliases
	;
	for (const QDate& SingleDate : flows.m_LastUpdate) {
		stream << SingleDate;
	}
	for (auto i = std::begin(flows.m_Assumptions); i != std::end(flows.m_Assumptions); ++i) {
		for (auto j = std::begin(*i); j != std::end(*i); ++j) {
			stream << *j;
		}
	}
	return stream;
}

QDataStream& operator>>(QDataStream & stream, LoanAssumption& flows) {
	return flows.LoadOldVersion(stream);
}
