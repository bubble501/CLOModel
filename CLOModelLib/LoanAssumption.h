#ifndef LoanAssumption_h__
#define LoanAssumption_h__
#include <QString>
#include <QStringList>
#include <QDate>
#include "BackwardCompatibilityInterface.h"
class LoanAssumptionPrivate;
class CLOMODELLIB_EXPORT LoanAssumption :public BackwardInterface
{
    DECLARE_PUBLIC_COMMONS(LoanAssumption)
    DECLARE_PUBLIC_COMMONS_COPY(LoanAssumption)
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
public:
	QString GetAssumption(AssumptionType at, Seniority sn)const;
	QString GetRawAssumption(AssumptionType at, Seniority sn)const;
	void SetAssumption(AssumptionType at, Seniority sn, const QString& Val);
	const QDate& GetLastUpdate(Seniority sn)const;
	void SetLastUpdate(Seniority sn, const QDate& a);
	bool operator==(const LoanAssumption& a) const;
	bool operator!=(const LoanAssumption& a) const { return !operator==(a); }
	int GetAliasesCount() const;
	const QStringList& GetAliases() const;
	void SetAliases(const QString& val);
	void AddAlias(const QString& val);
	void RemoveAlias(int index);
	void ClearAliases();
	QString GelAliasString() const;
	bool MatchPattern(const QString& Pat) const;
	LoanAssumption(const QString& ScenarioName);
	const QString& GetScenarioName() const;
	void SetScenarioName(const QString& val);
	void SetSeniorLastUpdate(const QDate& a);
	void SetMezzLastUpdate(const QDate& a);
	const QDate& GetSeniorLastUpdate()const;
	const QDate& GetMezzLastUpdate() const;

	void SetSeniorMaturityExtension(const QString& a);
	void SetSeniorInitialHaircut(const QString& a);
	void SetSeniorPrepaymentFee(const QString& a);
	void SetSeniorDayCount(const QString& a);
	void SetSeniorCPR(const QString& a);
	void SetSeniorCDR(const QString& a);
	void SetSeniorLS(const QString& a);
	void SetSeniorRecoveryLag(const QString& a);
	void SetSeniorDelinquency(const QString& a);
	void SetSeniorDelinquencyLag(const QString& a);
	void SetSeniorPrice(const QString& a);
	void SetSeniorHaircut(const QString& a);
	void SetSeniorPrepayMultiplier(const QString& a);
	void SetSeniorLossMultiplier(const QString& a);
	void SetMezzMaturityExtension(const QString& a);
	void SetMezzInitialHaircut(const QString& a);
	void SetMezzPrepaymentFee(const QString& a);
	void SetMezzDayCount(const QString& a);
	void SetMezzCPR(const QString& a);
	void SetMezzCDR(const QString& a);
	void SetMezzLS(const QString& a);
	void SetMezzRecoveryLag(const QString& a);
	void SetMezzDelinquency(const QString& a);
	void SetMezzDelinquencyLag(const QString& a);
	void SetMezzPrice(const QString& a);
	void SetMezzHaircut(const QString& a);
	void SetMezzPrepayMultiplier(const QString& a);
	void SetMezzLossMultiplier(const QString& a);
	QString GetSeniorMaturityExtension() const;
	QString GetSeniorInitialHaircut()const;
	QString GetSeniorPrepaymentFee() const;
	QString GetSeniorDayCount() const;
	QString GetSeniorCPR() const;
	QString GetSeniorCDR()const;
	QString GetSeniorLS()const;
	QString GetSeniorRecoveryLag() const;
	QString GetSeniorDelinquency() const;
	QString GetSeniorDelinquencyLag()const;
	QString GetSeniorPrice()const;
	QString GetSeniorHaircut() const;
	QString GetSeniorPrepayMultiplier() const;
	QString GetSeniorLossMultiplier()const;
	QString GetMezzMaturityExtension()const;
	QString GetMezzInitialHaircut() const;
	QString GetMezzPrepaymentFee() const;
	QString GetMezzDayCount()const;
	QString GetMezzCPR() const;
	QString GetMezzCDR()const;
	QString GetMezzLS()const;
	QString GetMezzRecoveryLag()const;
	QString GetMezzDelinquency() const;
	QString GetMezzDelinquencyLag() const;
	QString GetMezzPrice()const;
	QString GetMezzHaircut() const;
	QString GetMezzPrepayMultiplier()const;
	QString GetMezzLossMultiplier()const;
	QString GetRawSeniorMaturityExtension() const;
	QString GetRawSeniorInitialHaircut()const;
	QString GetRawSeniorPrepaymentFee() const;
	QString GetRawSeniorDayCount() const;
	QString GetRawSeniorCPR() const;
	QString GetRawSeniorCDR()const;
	QString GetRawSeniorLS()const;
	QString GetRawSeniorRecoveryLag() const;
	QString GetRawSeniorDelinquency() const;
	QString GetRawSeniorDelinquencyLag()const;
	QString GetRawSeniorPrice()const;
	QString GetRawSeniorHaircut() const;
	QString GetRawSeniorPrepayMultiplier() const;
	QString GetRawSeniorLossMultiplier()const;
	QString GetRawMezzMaturityExtension()const;
	QString GetRawMezzInitialHaircut() const;
	QString GetRawMezzPrepaymentFee() const;
	QString GetRawMezzDayCount()const;
	QString GetRawMezzCPR() const;
	QString GetRawMezzCDR()const;
	QString GetRawMezzLS()const;
	QString GetRawMezzRecoveryLag()const;
	QString GetRawMezzDelinquency() const;
	QString GetRawMezzDelinquencyLag() const;
	QString GetRawMezzPrice()const;
	QString GetRawMezzHaircut() const;
	QString GetRawMezzPrepayMultiplier()const;
	QString GetRawMezzLossMultiplier()const;
protected:
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
	friend QDataStream& operator<<(QDataStream & stream, const LoanAssumption& flows);
	friend QDataStream& operator>>(QDataStream & stream, LoanAssumption& flows);
};
QDataStream& operator<<(QDataStream & stream, const LoanAssumption& flows);
QDataStream& operator>>(QDataStream & stream, LoanAssumption& flows);
#endif // LoanAssumption_h__
