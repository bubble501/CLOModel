#ifndef AssumptionSet_h__
#define AssumptionSet_h__
#include <QString>
#include "BackwardCompatibilityInterface.h"
class AssumptionSet : public BackwardInterface{
protected:
	QString m_CDRscenario;
	QString m_CPRscenario;
	QString m_LSscenario;
	QString m_RecLagScenario;
	QString m_DelinqScenario;
	QString m_DelinqLagScenario;
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
public:
	AssumptionSet() {}
	AssumptionSet(
		const QString& CPRscenario,
		const QString& CDRscenario,
		const QString& LSscenario,
		const QString& RecLagScenario,
		const QString& DelinqScenario,
		const QString& DelinqLagScenario
	);
	QString ToString() const;
	AssumptionSet(const AssumptionSet& a);
	AssumptionSet(const QString& a);
	AssumptionSet& operator=(const AssumptionSet& a);
	AssumptionSet& operator=(const QString& a);
	const QString& GetCDRscenario() const { return m_CDRscenario; }
	void SetCDRscenario(const QString& val) { m_CDRscenario = val; }
	const QString& GetDelinqLagScenario() const { return m_DelinqLagScenario; }
	void SetDelinqLagScenario(const QString& val) { m_DelinqLagScenario = val; }
	const QString& GetDelinqScenario() const { return m_DelinqScenario; }
	void SetDelinqScenario(const QString& val) { m_DelinqScenario = val; }
	const QString& GetRecLagScenario() const { return m_RecLagScenario; }
	void SetRecLagScenario(const QString& val) { m_RecLagScenario = val; }
	const QString& GetLSscenario() const { return m_LSscenario; }
	void SetLSscenario(const QString& val) { m_LSscenario = val; }
	const QString& GetCPRscenario() const { return m_CPRscenario; }
	void SetCPRscenario(const QString& val) { m_CPRscenario = val; }
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