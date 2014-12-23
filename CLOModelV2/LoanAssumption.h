#ifndef LoanAssumption_h__
#define LoanAssumption_h__
#include <QString>
#include <QStringList>
#include <QDate>
class LoanAssumption {
public:
	enum AssumptionType {
		MaturityExtension = 0
		, InitialHaircut
		, PrepaymentFee
		, DayCount
		, CPR
		, CDR
		, LS
		, RecoveryLag
		, Delinquency
		, DelinquencyLag
		, Price
		, Haircut
		, PrepayMultiplier
		, LossMultiplier
	};
	enum Seniority { Senior = 0, Mezz };
private:
	enum {SenioritySize=2};
	enum { AssumptionTypeSize = 14};
	QString m_ScenarioName;
	QStringList m_Aliases;
	QString m_Assumptions[AssumptionTypeSize][SenioritySize];
	QDate m_LastUpdate[SenioritySize];
public:
	QString GetAssumption(AssumptionType at, Seniority sn)const;
	QString GetRawAssumption(AssumptionType at, Seniority sn)const;
	void SetAssumption(AssumptionType at, Seniority sn, const QString& Val) { m_Assumptions[at][sn] = Val; }
	const QDate& GetLastUpdate(Seniority sn)const { return m_LastUpdate[sn]; }
	void SetLastUpdate(Seniority sn, const QDate& a) {m_LastUpdate[sn] = a;}
	bool operator==(const LoanAssumption& a) const;
	bool operator!=(const LoanAssumption& a) const { return !operator==(a); }
	int GetAliasesCount() const { return m_Aliases.size(); }
	const QStringList& GetAliases() const { return m_Aliases; }
	void SetAliases(const QString& val);
	void AddAlias(const QString& val);
	void RemoveAlias(int index) { if (index >= 0 && index < m_Aliases.size()) m_Aliases.removeAt(index); }
	void ClearAliases() { m_Aliases.clear(); }
	QString GelAliasString() const;
	bool MatchPattern(const QString& Pat) const;
	LoanAssumption(const LoanAssumption& a);
	LoanAssumption& operator=(const LoanAssumption& a);
	LoanAssumption(const QString& ScenarioName);
	const QString& GetScenarioName() const { return m_ScenarioName; }
	void SetScenarioName(const QString& val) { m_ScenarioName = val; }
	void SetSeniorLastUpdate(const QDate& a) { SetLastUpdate(Senior, a); }
	void SetMezzLastUpdate(const QDate& a) { SetLastUpdate(Mezz, a); }
	const QDate& GetSeniorLastUpdate()const { return GetLastUpdate(Senior); }
	const QDate& GetMezzLastUpdate() const { return GetLastUpdate(Mezz); }

	void SetSeniorMaturityExtension(const QString& a) { SetAssumption(MaturityExtension, Senior, a); }
	void SetSeniorInitialHaircut(const QString& a) { SetAssumption(InitialHaircut, Senior, a); }
	void SetSeniorPrepaymentFee(const QString& a) { SetAssumption(PrepaymentFee, Senior, a); }
	void SetSeniorDayCount(const QString& a) { SetAssumption(DayCount, Senior, a); }
	void SetSeniorCPR(const QString& a) { SetAssumption(CPR, Senior, a); }
	void SetSeniorCDR(const QString& a) { SetAssumption(CDR, Senior, a); }
	void SetSeniorLS(const QString& a) { SetAssumption(LS, Senior, a); }
	void SetSeniorRecoveryLag(const QString& a) { SetAssumption(RecoveryLag, Senior, a); }
	void SetSeniorDelinquency(const QString& a) { SetAssumption(Delinquency, Senior, a); }
	void SetSeniorDelinquencyLag(const QString& a) { SetAssumption(DelinquencyLag, Senior, a); }
	void SetSeniorPrice(const QString& a) { SetAssumption(Price, Senior, a); }
	void SetSeniorHaircut(const QString& a) { SetAssumption(Haircut, Senior, a); }
	void SetSeniorPrepayMultiplier(const QString& a) { SetAssumption(PrepayMultiplier, Senior, a); }
	void SetSeniorLossMultiplier(const QString& a) { SetAssumption(LossMultiplier, Senior, a); }
	void SetMezzMaturityExtension(const QString& a) { SetAssumption(MaturityExtension, Mezz, a); }
	void SetMezzInitialHaircut(const QString& a) { SetAssumption(InitialHaircut, Mezz, a); }
	void SetMezzPrepaymentFee(const QString& a) { SetAssumption(PrepaymentFee, Mezz, a); }
	void SetMezzDayCount(const QString& a) { SetAssumption(DayCount, Mezz, a); }
	void SetMezzCPR(const QString& a) { SetAssumption(CPR, Mezz, a); }
	void SetMezzCDR(const QString& a) { SetAssumption(CDR, Mezz, a); }
	void SetMezzLS(const QString& a) { SetAssumption(LS, Mezz, a); }
	void SetMezzRecoveryLag(const QString& a) { SetAssumption(RecoveryLag, Mezz, a); }
	void SetMezzDelinquency(const QString& a) { SetAssumption(Delinquency, Mezz, a); }
	void SetMezzDelinquencyLag(const QString& a) { SetAssumption(DelinquencyLag, Mezz, a); }
	void SetMezzPrice(const QString& a) { SetAssumption(Price, Mezz, a); }
	void SetMezzHaircut(const QString& a) { SetAssumption(Haircut, Mezz, a); }
	void SetMezzPrepayMultiplier(const QString& a) { SetAssumption(PrepayMultiplier, Mezz, a); }
	void SetMezzLossMultiplier(const QString& a) { SetAssumption(LossMultiplier, Mezz, a); }
	QString GetSeniorMaturityExtension() const { return GetAssumption(MaturityExtension, Senior); }
	QString GetSeniorInitialHaircut()const { return GetAssumption(InitialHaircut, Senior); }
	QString GetSeniorPrepaymentFee() const { return GetAssumption(PrepaymentFee, Senior); }
	QString GetSeniorDayCount() const { return GetAssumption(DayCount, Senior); }
	QString GetSeniorCPR() const { return GetAssumption(CPR, Senior); }
	QString GetSeniorCDR()const { return GetAssumption(CDR, Senior); }
	QString GetSeniorLS()const { return GetAssumption(LS, Senior); }
	QString GetSeniorRecoveryLag() const { return GetAssumption(RecoveryLag, Senior); }
	QString GetSeniorDelinquency() const { return GetAssumption(Delinquency, Senior); }
	QString GetSeniorDelinquencyLag()const { return GetAssumption(DelinquencyLag, Senior); }
	QString GetSeniorPrice()const { return GetAssumption(Price, Senior); }
	QString GetSeniorHaircut() const { return GetAssumption(Haircut, Senior); }
	QString GetSeniorPrepayMultiplier() const { return GetAssumption(PrepayMultiplier, Senior); }
	QString GetSeniorLossMultiplier()const { return GetAssumption(LossMultiplier, Senior); }
	QString GetMezzMaturityExtension()const { return GetAssumption(MaturityExtension, Mezz); }
	QString GetMezzInitialHaircut() const { return GetAssumption(InitialHaircut, Mezz); }
	QString GetMezzPrepaymentFee() const { return GetAssumption(PrepaymentFee, Mezz); }
	QString GetMezzDayCount()const { return GetAssumption(DayCount, Mezz); }
	QString GetMezzCPR() const { return GetAssumption(CPR, Mezz); }
	QString GetMezzCDR()const { return GetAssumption(CDR, Mezz); }
	QString GetMezzLS()const { return GetAssumption(LS, Mezz); }
	QString GetMezzRecoveryLag()const { return GetAssumption(RecoveryLag, Mezz); }
	QString GetMezzDelinquency() const { return GetAssumption(Delinquency, Mezz); }
	QString GetMezzDelinquencyLag() const { return GetAssumption(DelinquencyLag, Mezz); }
	QString GetMezzPrice()const { return GetAssumption(Price, Mezz); }
	QString GetMezzHaircut() const { return GetAssumption(Haircut, Mezz); }
	QString GetMezzPrepayMultiplier()const { return GetAssumption(PrepayMultiplier, Mezz); }
	QString GetMezzLossMultiplier()const { return GetAssumption(LossMultiplier, Mezz); }
	QString GetRawSeniorMaturityExtension() const { return GetRawAssumption(MaturityExtension, Senior); }
	QString GetRawSeniorInitialHaircut()const { return GetRawAssumption(InitialHaircut, Senior); }
	QString GetRawSeniorPrepaymentFee() const { return GetRawAssumption(PrepaymentFee, Senior); }
	QString GetRawSeniorDayCount() const { return GetRawAssumption(DayCount, Senior); }
	QString GetRawSeniorCPR() const { return GetRawAssumption(CPR, Senior); }
	QString GetRawSeniorCDR()const { return GetRawAssumption(CDR, Senior); }
	QString GetRawSeniorLS()const { return GetRawAssumption(LS, Senior); }
	QString GetRawSeniorRecoveryLag() const { return GetRawAssumption(RecoveryLag, Senior); }
	QString GetRawSeniorDelinquency() const { return GetRawAssumption(Delinquency, Senior); }
	QString GetRawSeniorDelinquencyLag()const { return GetRawAssumption(DelinquencyLag, Senior); }
	QString GetRawSeniorPrice()const { return GetRawAssumption(Price, Senior); }
	QString GetRawSeniorHaircut() const { return GetRawAssumption(Haircut, Senior); }
	QString GetRawSeniorPrepayMultiplier() const { return GetRawAssumption(PrepayMultiplier, Senior); }
	QString GetRawSeniorLossMultiplier()const { return GetRawAssumption(LossMultiplier, Senior); }
	QString GetRawMezzMaturityExtension()const { return GetRawAssumption(MaturityExtension, Mezz); }
	QString GetRawMezzInitialHaircut() const { return GetRawAssumption(InitialHaircut, Mezz); }
	QString GetRawMezzPrepaymentFee() const { return GetRawAssumption(PrepaymentFee, Mezz); }
	QString GetRawMezzDayCount()const { return GetRawAssumption(DayCount, Mezz); }
	QString GetRawMezzCPR() const { return GetRawAssumption(CPR, Mezz); }
	QString GetRawMezzCDR()const { return GetRawAssumption(CDR, Mezz); }
	QString GetRawMezzLS()const { return GetRawAssumption(LS, Mezz); }
	QString GetRawMezzRecoveryLag()const { return GetRawAssumption(RecoveryLag, Mezz); }
	QString GetRawMezzDelinquency() const { return GetRawAssumption(Delinquency, Mezz); }
	QString GetRawMezzDelinquencyLag() const { return GetRawAssumption(DelinquencyLag, Mezz); }
	QString GetRawMezzPrice()const { return GetRawAssumption(Price, Mezz); }
	QString GetRawMezzHaircut() const { return GetRawAssumption(Haircut, Mezz); }
	QString GetRawMezzPrepayMultiplier()const { return GetRawAssumption(PrepayMultiplier, Mezz); }
	QString GetRawMezzLossMultiplier()const { return GetRawAssumption(LossMultiplier, Mezz); }
};
#endif // LoanAssumption_h__
