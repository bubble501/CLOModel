#ifndef StressTest_h__
#define StressTest_h__
//#define PrintExecutionTime
#define NumStressDimentsions 6
#include <QString>
#include <QSet>
#include <QHash>
#include <QObject>
#include <QDataStream>
#include "Waterfall.h"
#include "AssumptionSet.h"
#include "ProgressWidget.h"
#include <QScopedPointer>
#include <QSharedPointer>
#include <QPointer>
class MtgCalculator;
class ProgressWidget;
class MtgCashFlow;
class StressTest:public QObject , public BackwardInterface{
	Q_OBJECT
private:
#ifdef PrintExecutionTime
	QTime ExecutionTime;
#endif
	qint32 CurrentAssumption[NumStressDimentsions];
	enum {
		AssCPR=0
		, AssCDR = 1
		, AssLS = 2
		, AssRecLag = 3
		, AssDelinq = 4
		, AssDelinqLag = 5
	};
	void ResetCurrentAssumption() { for (int i = 0; i < NumStressDimentsions; ++i) CurrentAssumption[i] = 0; }
	const QSet<QString>* m_AssumptionsRef[NumStressDimentsions];
	bool IncreaseCurrentAssumption(int level = NumStressDimentsions-1);
	qint32 CountScenariosCalculated(int level = NumStressDimentsions - 1);
protected:
	QPointer<ProgressWidget> ProgressForm;
	MtgCalculator* BaseCalculator;
	QScopedPointer<MtgCashFlow> BaseFlows;
	QSet<QString> m_CDRscenarios;
	QSet<QString> m_CPRscenarios;
	QSet<QString> m_LSscenarios;
	QSet<QString> m_RecLagScenarios;
	QSet<QString> m_DelinqScenarios;
	QSet<QString> m_DelinqLagScenarios;
	Waterfall Structure;
	QDate StartDate;
	QHash<AssumptionSet, QSharedPointer<Waterfall> >  Results;
	bool SequentialComputation;
	bool ContinueCalculation;
	bool ShowProgress;
	bool UseFastVersion;
	quint32 RemoveInvalidScenarios();
	void ResetStressTest();
	friend QDataStream& operator<<(QDataStream & stream, const StressTest& flows);
	friend QDataStream& operator>>(QDataStream & stream, StressTest& flows);
public:
	StressTest(QObject* parent=0);
	void SetShowProgress(bool a=true){ShowProgress=a;}
	bool GetShowProgress()const{return ShowProgress;}
	const QSet<QString>& GetCDRscenarios() const { return m_CDRscenarios; }
	const QSet<QString>& GetCPRscenarios() const { return m_CPRscenarios; }
	const QSet<QString>& GetLSscenarios() const { return m_LSscenarios; }
	const QSet<QString>& GetRecLagScenarios() const { return m_RecLagScenarios; }
	const QSet<QString>& GetDelinqScenarios() const { return m_DelinqScenarios; }
	const QSet<QString>& GetDelinqLagScenarios() const { return m_DelinqLagScenarios; }
	void SetCDRscenarios(const QSet<QString>& val) { m_CDRscenarios = val; }
	void AddCDRscenarios(const QSet<QString>& val) { m_CDRscenarios.unite(val); }
	void AddCDRscenarios(const QString& val) { m_CDRscenarios.insert(val); }
	void RemoveCDRscenarios(const QString& val) { m_CDRscenarios.remove(val); }
	void ClearCDRscenarios() { m_CDRscenarios.clear(); }
	void SetCPRscenarios(const QSet<QString>& val) { m_CPRscenarios = val; }
	void AddCPRscenarios(const QSet<QString>& val) { m_CPRscenarios.unite(val); }
	void AddCPRscenarios(const QString& val) { m_CPRscenarios.insert(val); }
	void RemoveCPRscenarios(const QString& val) { m_CPRscenarios.remove(val); }
	void ClearCPRscenarios() { m_CPRscenarios.clear(); }
	void SetLSscenarios(const QSet<QString>& val) { m_LSscenarios = val; }
	void AddLSscenarios(const QSet<QString>& val) { m_LSscenarios.unite(val); }
	void AddLSscenarios(const QString& val) { m_LSscenarios.insert(val); }
	void RemoveLSscenarios(const QString& val) { m_LSscenarios.remove(val); }
	void ClearLSscenarios() { m_LSscenarios.clear(); }
	void SetRecLagScenarios(const QSet<QString>& val) { m_RecLagScenarios = val; }
	void AddRecLagScenarios(const QSet<QString>& val) { m_RecLagScenarios.unite(val); }
	void AddRecLagScenarios(const QString& val) { m_RecLagScenarios.insert(val); }
	void RemoveRecLagScenarios(const QString& val) { m_RecLagScenarios.remove(val); }
	void ClearRecLagScenarios() { m_RecLagScenarios.clear(); }
	void SetDelinqScenarios(const QSet<QString>& val) { m_DelinqScenarios = val; }
	void AddDelinqScenarios(const QSet<QString>& val) { m_DelinqScenarios.unite(val); }
	void AddDelinqScenarios(const QString& val) { m_DelinqScenarios.insert(val); }
	void RemoveDelinqScenarios(const QString& val) { m_DelinqScenarios.remove(val); }
	void ClearDelinqScenarios() { m_DelinqScenarios.clear(); }
	void SetDelinqLagScenarios(const QSet<QString>& val) { m_DelinqLagScenarios = val; }
	void AddDelinqLagScenarios(const QSet<QString>& val) { m_DelinqLagScenarios.unite(val); }
	void AddDelinqLagScenarios(const QString& val) { m_DelinqLagScenarios.insert(val); }
	void RemoveDelinqLagScenarios(const QString& val) { m_DelinqLagScenarios.remove(val); }
	void ClearDelinqLagScenarios() { m_DelinqLagScenarios.clear(); }
	void ClearAssumptions() { ClearCDRscenarios(); ClearCPRscenarios(); ClearLSscenarios(); ClearRecLagScenarios(); ClearDelinqScenarios(); ClearDelinqLagScenarios(); }
	const Waterfall& GetStructure()const{return Structure;}
	const QDate& GetStartDate()const{return StartDate;}
	const QHash<AssumptionSet, QSharedPointer<Waterfall> > & GetResults() const { return Results; }
	bool GetUseMultithread()const {return !SequentialComputation;}
	void ResetResult();
	void AddLoan(const Mortgage& a);
	void ResetLoans();
	void SetStructure(const Waterfall& a){Structure=a;}
	void SetStartDate(const QDate& a){StartDate=a;}
	void UseMultithread(bool a=true){SequentialComputation=!a;}
	void SaveResults(const QString& DestPath)const {}
	bool LoadResultsFromFile(const QString& DestPath) { return true; }
	void SetUseFastVersion(bool a=true) { UseFastVersion = a; }
	bool GetUseFastVersion() { return UseFastVersion; }
	const MtgCalculator& GetLoans() const;
	static Waterfall GetScenarioFromFile(const QString& DestPath, const QString& CPRscenario, const QString& CDRscenario, const QString& LSscenario, const QString& RecLagScenario, const QString& DelinqScenario, const QString& DelinqLagScenario);
	quint32 CountScenarios()const;
	void ResetResults() { Results.clear(); }
public slots:
	void RunStressTest();
	void StopCalculation();
private slots:
	void BaseForFastCalculated();
	void SlowLoansCalculated();
	void RunCurrentScenario();
	void GoToNextScenario();
	void HandleError() {}
	void StoppedCalculation() { ResetStressTest(); }
protected:
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override { return stream; }
signals:
	void CurrentScenarioCalculated();
	void ErrorInCalculation();
	void ProgressStatus(double);
	void AllFinished();

};
QDataStream& operator<<(QDataStream & stream, const StressTest& flows);
QDataStream& operator>>(QDataStream & stream, StressTest& flows);
#endif // StressTest_h__
