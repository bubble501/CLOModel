#ifndef AssumptionSet_h__
#define AssumptionSet_h__
#include <QString>
#include "BackwardCompatibilityInterface.h"
class AssumptionSetPrivate;
class AssumptionSet : public BackwardInterface{
    DECLARE_PUBLIC_COMMONS(AssumptionSet)
    DECLARE_PUBLIC_COMMONS_COPY(AssumptionSet)
protected:
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
public:
	AssumptionSet(
		const QString& CPRscenario,
		const QString& CDRscenario,
		const QString& LSscenario,
		const QString& RecLagScenario,
		const QString& DelinqScenario,
		const QString& DelinqLagScenario
	);
	QString ToString() const;
	AssumptionSet(const QString& a);
	AssumptionSet& operator=(const QString& a);
	const QString& GetCDRscenario() const;
	void SetCDRscenario(const QString& val);
	const QString& GetDelinqLagScenario() const;
	void SetDelinqLagScenario(const QString& val);
	const QString& GetDelinqScenario() const;
	void SetDelinqScenario(const QString& val);
	const QString& GetRecLagScenario() const;
	void SetRecLagScenario(const QString& val);
	const QString& GetLSscenario() const;
	void SetLSscenario(const QString& val);
	const QString& GetCPRscenario() const;
	void SetCPRscenario(const QString& val);
	bool IsValid() const;
	friend uint qHash(const AssumptionSet& key, uint seed);
	friend bool operator==(const AssumptionSet &e1, const AssumptionSet &e2);
	friend QDataStream& operator<<(QDataStream & stream, const AssumptionSet& flows);
	friend QDataStream& operator>>(QDataStream & stream, AssumptionSet& flows);
};
bool operator==(const AssumptionSet &e1, const AssumptionSet &e2);
uint qHash(const AssumptionSet& key, uint seed);
QDataStream& operator<<(QDataStream & stream, const AssumptionSet& flows);
QDataStream& operator>>(QDataStream & stream, AssumptionSet& flows);
#endif // AssumptionSet_h__