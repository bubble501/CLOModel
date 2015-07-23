#include "LoanAssumption.h"
#include "Private/LoanAssumption_p.h"
#include "AbstractBbgVect.h"
#include "Private/AbstractBbgVect_p.h"
#include <QRegularExpression>
DEFINE_PUBLIC_COMMONS(LoanAssumption)
DEFINE_PUBLIC_COMMONS_COPY(LoanAssumption)
LoanAssumptionPrivate::LoanAssumptionPrivate(LoanAssumption *q,const LoanAssumptionPrivate& other)
	:BackwardInterfacePrivate(q,other)
    ,m_ScenarioName(other.m_ScenarioName)
    , m_Aliases(other.m_Aliases)
{
    for (int j = 0; j < LoanAssumptionPrivate::SenioritySize; j++) {
        for (int i = 0; i < LoanAssumptionPrivate::AssumptionTypeSize; i++) {
            m_Assumptions[i][j] = other.m_Assumptions[i][j];
        }
        m_LastUpdate[j] = other.m_LastUpdate[j];
    }
}
LoanAssumptionPrivate::LoanAssumptionPrivate(LoanAssumption *q)
	:BackwardInterfacePrivate(q)
{}
LoanAssumption::LoanAssumption(LoanAssumptionPrivate *d, const LoanAssumption& other)
	:BackwardInterface(d,other)
{}
LoanAssumption& LoanAssumption::operator=(const LoanAssumption& other){
	Q_D(LoanAssumption);
	BackwardInterface::operator=(other);
    for (int j = 0; j < LoanAssumptionPrivate::SenioritySize; j++) {
        for (int i = 0; i < LoanAssumptionPrivate::AssumptionTypeSize; i++) {
            d->m_Assumptions[i][j] = other.d_func()->m_Assumptions[i][j];
        }
        d->m_LastUpdate[j] = other.d_func()->m_LastUpdate[j];
    }
    d->m_ScenarioName = other.d_func()->m_ScenarioName;
    d->m_Aliases = other.d_func()->m_Aliases;
	return *this;
}
LoanAssumption::LoanAssumption(LoanAssumptionPrivate *d)
	:BackwardInterface(d)
{}

LoanAssumption::LoanAssumption(const QString& ScenarioName)
    :LoanAssumption()
{
    Q_D(LoanAssumption);
    d->m_ScenarioName = ScenarioName;
}

const QString& LoanAssumption::GetScenarioName() const
{
    Q_D(const LoanAssumption);
    return d->m_ScenarioName;
}

void LoanAssumption::SetScenarioName(const QString& val)
{
    Q_D(LoanAssumption);
    d->m_ScenarioName = val;
}

void LoanAssumption::SetSeniorLastUpdate(const QDate& a)
{
    SetLastUpdate(Senior, a);
}

void LoanAssumption::SetMezzLastUpdate(const QDate& a)
{
    SetLastUpdate(Mezz, a);
}

const QDate& LoanAssumption::GetSeniorLastUpdate() const
{
    return GetLastUpdate(Senior);
}

const QDate& LoanAssumption::GetMezzLastUpdate() const
{
    return GetLastUpdate(Mezz);
}

void LoanAssumption::SetSeniorMaturityExtension(const QString& a)
{
    SetAssumption(MaturityExtension, Senior, a);
}

void LoanAssumption::SetSeniorInitialHaircut(const QString& a)
{
    SetAssumption(InitialHaircut, Senior, a);
}

void LoanAssumption::SetSeniorPrepaymentFee(const QString& a)
{
    SetAssumption(PrepaymentFee, Senior, a);
}

void LoanAssumption::SetSeniorDayCount(const QString& a)
{
    SetAssumption(DayCount, Senior, a);
}

void LoanAssumption::SetSeniorCPR(const QString& a)
{
    SetAssumption(CPR, Senior, a);
}

void LoanAssumption::SetSeniorCDR(const QString& a)
{
    SetAssumption(CDR, Senior, a);
}

void LoanAssumption::SetSeniorLS(const QString& a)
{
    SetAssumption(LS, Senior, a);
}

void LoanAssumption::SetSeniorRecoveryLag(const QString& a)
{
    SetAssumption(RecoveryLag, Senior, a);
}

void LoanAssumption::SetSeniorDelinquency(const QString& a)
{
    SetAssumption(Delinquency, Senior, a);
}

void LoanAssumption::SetSeniorDelinquencyLag(const QString& a)
{
    SetAssumption(DelinquencyLag, Senior, a);
}

void LoanAssumption::SetSeniorPrice(const QString& a)
{
    SetAssumption(Price, Senior, a);
}

void LoanAssumption::SetSeniorHaircut(const QString& a)
{
    SetAssumption(Haircut, Senior, a);
}

void LoanAssumption::SetSeniorPrepayMultiplier(const QString& a)
{
    SetAssumption(PrepayMultiplier, Senior, a);
}

void LoanAssumption::SetSeniorLossMultiplier(const QString& a)
{
    SetAssumption(LossMultiplier, Senior, a);
}

void LoanAssumption::SetMezzMaturityExtension(const QString& a)
{
    SetAssumption(MaturityExtension, Mezz, a);
}

void LoanAssumption::SetMezzInitialHaircut(const QString& a)
{
    SetAssumption(InitialHaircut, Mezz, a);
}

void LoanAssumption::SetMezzPrepaymentFee(const QString& a)
{
    SetAssumption(PrepaymentFee, Mezz, a);
}

void LoanAssumption::SetMezzDayCount(const QString& a)
{
    SetAssumption(DayCount, Mezz, a);
}

void LoanAssumption::SetMezzCPR(const QString& a)
{
    SetAssumption(CPR, Mezz, a);
}

void LoanAssumption::SetMezzCDR(const QString& a)
{
    SetAssumption(CDR, Mezz, a);
}

void LoanAssumption::SetMezzLS(const QString& a)
{
    SetAssumption(LS, Mezz, a);
}

void LoanAssumption::SetMezzRecoveryLag(const QString& a)
{
    SetAssumption(RecoveryLag, Mezz, a);
}

void LoanAssumption::SetMezzDelinquency(const QString& a)
{
    SetAssumption(Delinquency, Mezz, a);
}

void LoanAssumption::SetMezzDelinquencyLag(const QString& a)
{
    SetAssumption(DelinquencyLag, Mezz, a);
}

void LoanAssumption::SetMezzPrice(const QString& a)
{
    SetAssumption(Price, Mezz, a);
}

void LoanAssumption::SetMezzHaircut(const QString& a)
{
    SetAssumption(Haircut, Mezz, a);
}

void LoanAssumption::SetMezzPrepayMultiplier(const QString& a)
{
    SetAssumption(PrepayMultiplier, Mezz, a);
}

void LoanAssumption::SetMezzLossMultiplier(const QString& a)
{
    SetAssumption(LossMultiplier, Mezz, a);
}

QString LoanAssumption::GetSeniorMaturityExtension() const
{
    return GetAssumption(MaturityExtension, Senior);
}

QString LoanAssumption::GetSeniorInitialHaircut() const
{
    return GetAssumption(InitialHaircut, Senior);
}

QString LoanAssumption::GetSeniorPrepaymentFee() const
{
    return GetAssumption(PrepaymentFee, Senior);
}

QString LoanAssumption::GetSeniorDayCount() const
{
    return GetAssumption(DayCount, Senior);
}

QString LoanAssumption::GetSeniorCPR() const
{
    return GetAssumption(CPR, Senior);
}

QString LoanAssumption::GetSeniorCDR() const
{
    return GetAssumption(CDR, Senior);
}

QString LoanAssumption::GetSeniorLS() const
{
    return GetAssumption(LS, Senior);
}

QString LoanAssumption::GetSeniorRecoveryLag() const
{
    return GetAssumption(RecoveryLag, Senior);
}

QString LoanAssumption::GetSeniorDelinquency() const
{
    return GetAssumption(Delinquency, Senior);
}

QString LoanAssumption::GetSeniorDelinquencyLag() const
{
    return GetAssumption(DelinquencyLag, Senior);
}

QString LoanAssumption::GetSeniorPrice() const
{
    return GetAssumption(Price, Senior);
}

QString LoanAssumption::GetSeniorHaircut() const
{
    return GetAssumption(Haircut, Senior);
}

QString LoanAssumption::GetSeniorPrepayMultiplier() const
{
    return GetAssumption(PrepayMultiplier, Senior);
}

QString LoanAssumption::GetSeniorLossMultiplier() const
{
    return GetAssumption(LossMultiplier, Senior);
}

QString LoanAssumption::GetMezzMaturityExtension() const
{
    return GetAssumption(MaturityExtension, Mezz);
}

QString LoanAssumption::GetMezzInitialHaircut() const
{
    return GetAssumption(InitialHaircut, Mezz);
}

QString LoanAssumption::GetMezzPrepaymentFee() const
{
    return GetAssumption(PrepaymentFee, Mezz);
}

QString LoanAssumption::GetMezzDayCount() const
{
    return GetAssumption(DayCount, Mezz);
}

QString LoanAssumption::GetMezzCPR() const
{
    return GetAssumption(CPR, Mezz);
}

QString LoanAssumption::GetMezzCDR() const
{
    return GetAssumption(CDR, Mezz);
}

QString LoanAssumption::GetMezzLS() const
{
    return GetAssumption(LS, Mezz);
}

QString LoanAssumption::GetMezzRecoveryLag() const
{
    return GetAssumption(RecoveryLag, Mezz);
}

QString LoanAssumption::GetMezzDelinquency() const
{
    return GetAssumption(Delinquency, Mezz);
}

QString LoanAssumption::GetMezzDelinquencyLag() const
{
    return GetAssumption(DelinquencyLag, Mezz);
}

QString LoanAssumption::GetMezzPrice() const
{
    return GetAssumption(Price, Mezz);
}

QString LoanAssumption::GetMezzHaircut() const
{
    return GetAssumption(Haircut, Mezz);
}

QString LoanAssumption::GetMezzPrepayMultiplier() const
{
    return GetAssumption(PrepayMultiplier, Mezz);
}

QString LoanAssumption::GetMezzLossMultiplier() const
{
    return GetAssumption(LossMultiplier, Mezz);
}

QString LoanAssumption::GetRawSeniorMaturityExtension() const
{
    return GetRawAssumption(MaturityExtension, Senior);
}

QString LoanAssumption::GetRawSeniorInitialHaircut() const
{
    return GetRawAssumption(InitialHaircut, Senior);
}

QString LoanAssumption::GetRawSeniorPrepaymentFee() const
{
    return GetRawAssumption(PrepaymentFee, Senior);
}

QString LoanAssumption::GetRawSeniorDayCount() const
{
    return GetRawAssumption(DayCount, Senior);
}

QString LoanAssumption::GetRawSeniorCPR() const
{
    return GetRawAssumption(CPR, Senior);
}

QString LoanAssumption::GetRawSeniorCDR() const
{
    return GetRawAssumption(CDR, Senior);
}

QString LoanAssumption::GetRawSeniorLS() const
{
    return GetRawAssumption(LS, Senior);
}

QString LoanAssumption::GetRawSeniorRecoveryLag() const
{
    return GetRawAssumption(RecoveryLag, Senior);
}

QString LoanAssumption::GetRawSeniorDelinquency() const
{
    return GetRawAssumption(Delinquency, Senior);
}

QString LoanAssumption::GetRawSeniorDelinquencyLag() const
{
    return GetRawAssumption(DelinquencyLag, Senior);
}

QString LoanAssumption::GetRawSeniorPrice() const
{
    return GetRawAssumption(Price, Senior);
}

QString LoanAssumption::GetRawSeniorHaircut() const
{
    return GetRawAssumption(Haircut, Senior);
}

QString LoanAssumption::GetRawSeniorPrepayMultiplier() const
{
    return GetRawAssumption(PrepayMultiplier, Senior);
}

QString LoanAssumption::GetRawSeniorLossMultiplier() const
{
    return GetRawAssumption(LossMultiplier, Senior);
}

QString LoanAssumption::GetRawMezzMaturityExtension() const
{
    return GetRawAssumption(MaturityExtension, Mezz);
}

QString LoanAssumption::GetRawMezzInitialHaircut() const
{
    return GetRawAssumption(InitialHaircut, Mezz);
}

QString LoanAssumption::GetRawMezzPrepaymentFee() const
{
    return GetRawAssumption(PrepaymentFee, Mezz);
}

QString LoanAssumption::GetRawMezzDayCount() const
{
    return GetRawAssumption(DayCount, Mezz);
}

QString LoanAssumption::GetRawMezzCPR() const
{
    return GetRawAssumption(CPR, Mezz);
}

QString LoanAssumption::GetRawMezzCDR() const
{
    return GetRawAssumption(CDR, Mezz);
}

QString LoanAssumption::GetRawMezzLS() const
{
    return GetRawAssumption(LS, Mezz);
}

QString LoanAssumption::GetRawMezzRecoveryLag() const
{
    return GetRawAssumption(RecoveryLag, Mezz);
}

QString LoanAssumption::GetRawMezzDelinquency() const
{
    return GetRawAssumption(Delinquency, Mezz);
}

QString LoanAssumption::GetRawMezzDelinquencyLag() const
{
    return GetRawAssumption(DelinquencyLag, Mezz);
}

QString LoanAssumption::GetRawMezzPrice() const
{
    return GetRawAssumption(Price, Mezz);
}

QString LoanAssumption::GetRawMezzHaircut() const
{
    return GetRawAssumption(Haircut, Mezz);
}

QString LoanAssumption::GetRawMezzPrepayMultiplier() const
{
    return GetRawAssumption(PrepayMultiplier, Mezz);
}

QString LoanAssumption::GetRawMezzLossMultiplier() const
{
    return GetRawAssumption(LossMultiplier, Mezz);
}

bool LoanAssumption::MatchPattern(const QString& Pat) const
{
    Q_D(const LoanAssumption);
	if (Pat.isEmpty())return false;
	QString Pattern("(?:");
    if (QRegularExpression(d->m_ScenarioName).isValid())
        Pattern += "(?:\\b" + d->m_ScenarioName + "\\b)";
    for (auto i = d->m_Aliases.constBegin(); i != d->m_Aliases.constEnd(); ++i)
		Pattern +="|(?:\\b" + *i + "\\b)";
	Pattern += ')';
	if (Pattern == "(?:)")
        return false;
    QRegularExpression MatchFinder(Pattern, QRegularExpression::CaseInsensitiveOption);
    Q_ASSERT(MatchFinder.isValid());
	return MatchFinder.match(Pat).hasMatch();
}


QString LoanAssumption::GetAssumption(AssumptionType at, Seniority sn) const {
    Q_D(const LoanAssumption);
	QString Result( d->m_Assumptions[at][sn].trimmed());
	if (Result.isEmpty()) 
        return Result;
    if (d->m_LastUpdate[sn].isNull()) 
        return Result;
    QRegularExpression AnchorMatch(QString("^(?:A\\s+") + VectorAnchorDateFormat + "\\s+)");
    Q_ASSERT(AnchorMatch.isValid());
	if (AnchorMatch.match(Result).hasMatch()) 
        return Result;
    return "A " + d->m_LastUpdate[sn].toString("MM/dd/yyyy") + ' ' + Result;
}

int LoanAssumption::GetAliasesCount() const
{
    Q_D(const LoanAssumption);
    return d->m_Aliases.size();
}

const QStringList& LoanAssumption::GetAliases() const
{
    Q_D(const LoanAssumption);
    return d->m_Aliases;
}

void LoanAssumption::SetAliases(const QString& val)
{
	ClearAliases();
	auto TempAl = val.split("$,$");
	for (auto i = TempAl.constBegin(); i != TempAl.constEnd();++i) {
		AddAlias(*i);
	}
}

void LoanAssumption::AddAlias(const QString& val) {
    Q_D(LoanAssumption);
	if (
		QRegularExpression(val).isValid()
		&& !d->m_Aliases.contains(val, Qt::CaseInsensitive)
        ) 
        d->m_Aliases.append(val);
}

void LoanAssumption::RemoveAlias(int index)
{
    Q_D(LoanAssumption);
    if (index >= 0 && index < d->m_Aliases.size()) 
        d->m_Aliases.removeAt(index);
}

void LoanAssumption::ClearAliases()
{
    Q_D(LoanAssumption);
    d->m_Aliases.clear();
}

QString LoanAssumption::GetRawAssumption(AssumptionType at, Seniority sn) const
{
    Q_D(const LoanAssumption);
    return d->m_Assumptions[at][sn].trimmed();
}

void LoanAssumption::SetAssumption(AssumptionType at, Seniority sn, const QString& Val)
{
    Q_D(LoanAssumption);
    d->m_Assumptions[at][sn] = Val;
}

const QDate& LoanAssumption::GetLastUpdate(Seniority sn) const
{
    Q_D(const LoanAssumption);
    return d->m_LastUpdate[sn];
}

void LoanAssumption::SetLastUpdate(Seniority sn, const QDate& a)
{
    Q_D(LoanAssumption);
    d->m_LastUpdate[sn] = a;
}

bool LoanAssumption::operator==(const LoanAssumption& a) const
{
    Q_D(const LoanAssumption);
    bool Result = d->m_ScenarioName == a.d_func()->m_ScenarioName;
    for (int j = 0; j < LoanAssumptionPrivate::SenioritySize && Result; j++) {
        Result = d->m_LastUpdate[j] == a.d_func()->m_LastUpdate[j];
        for (int i = 0; i < LoanAssumptionPrivate::AssumptionTypeSize && Result; i++) {
            Result = d->m_Assumptions[i][j] == a.d_func()->m_Assumptions[i][j];
		}
	}
    Result = Result &&  d->m_Aliases.size() == a.d_func()->m_Aliases.size();
    for (auto i = d->m_Aliases.constBegin(); i != d->m_Aliases.constEnd() && Result; ++i) {
        Result = a.d_func()->m_Aliases.contains(*i);
	}
    for (auto i = a.d_func()->m_Aliases.constBegin(); i != a.d_func()->m_Aliases.constEnd() && Result; ++i) {
        Result = d->m_Aliases.contains(*i);
	}
	return Result;
}

QString LoanAssumption::GelAliasString() const {
    Q_D(const LoanAssumption);
	QString Result;
    for (auto i = d->m_Aliases.constBegin(); i != d->m_Aliases.constEnd(); ++i) {
		if (!Result.isEmpty())Result += "$,$";
		Result += *i;
	}
	return Result;
}

QDataStream& LoanAssumption::LoadOldVersion(QDataStream& stream) {
    Q_D(LoanAssumption);
	qint32 OldAssumptionTypeSize, OldSenioritySize;
	stream >> OldAssumptionTypeSize >> OldSenioritySize
        >> d->m_ScenarioName >> d->m_Aliases;
    for (int i = 0; i < qMin(static_cast<qint32>(LoanAssumptionPrivate::SenioritySize), OldSenioritySize); ++i) {
        stream >> d->m_LastUpdate[i];
	}
    for (int i = 0; i < qMin(static_cast<qint32>(LoanAssumptionPrivate::AssumptionTypeSize), OldAssumptionTypeSize); ++i) {
        for (int j = 0; j < qMin(static_cast<qint32>(LoanAssumptionPrivate::SenioritySize), OldSenioritySize); ++j) {
            stream >> d->m_Assumptions[i][j];
		}
	}
	ResetProtocolVersion();
	return stream;
}

QDataStream& operator<<(QDataStream & stream, const LoanAssumption& flows) {
    stream << static_cast<qint32>(LoanAssumptionPrivate::AssumptionTypeSize) << static_cast<qint32>(LoanAssumptionPrivate::SenioritySize)
        << flows.d_func()->m_ScenarioName
        << flows.d_func()->m_Aliases
	;
    for (const QDate& SingleDate : flows.d_func()->m_LastUpdate) {
		stream << SingleDate;
	}
    for (auto i = std::begin(flows.d_func()->m_Assumptions); i != std::end(flows.d_func()->m_Assumptions); ++i) {
		for (auto j = std::begin(*i); j != std::end(*i); ++j) {
			stream << *j;
		}
	}
	return stream;
}

QDataStream& operator>>(QDataStream & stream, LoanAssumption& flows) {
	return flows.LoadOldVersion(stream);
}
