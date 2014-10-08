#include <QString>
class AssumptionSet {
protected:
	QString m_CDRscenario;
	QString m_CPRscenario;
	QString m_LSscenario;
	QString m_RecLagScenario;
	QString m_DelinqScenario;
	QString m_DelinqLagScenario;
public:
	AssumptionSet() {}
	AssumptionSet(
		const QString& CDRscenario,
		const QString& CPRscenario,
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
	friend uint qHash(const AssumptionSet& key, uint seed);
	friend bool operator==(const AssumptionSet &e1, const AssumptionSet &e2);
};
bool operator==(const AssumptionSet &e1, const AssumptionSet &e2);
uint qHash(const AssumptionSet& key, uint seed);