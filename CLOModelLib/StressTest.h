#ifndef StressTest_h__
#define StressTest_h__
#include <QObject>
#include "Waterfall.h"
class MtgCalculator;
class PhasedProgressWidget;
class MtgCashFlow;
class WaterfallCalculator;
class ScenarioApplier;
class QProgressDialog;
class StressTestPrivate;
class Mortgage;
class StressTest;
CLOMODELLIB_EXPORT QDataStream&  operator<<(QDataStream & stream, const StressTest& flows);
CLOMODELLIB_EXPORT QDataStream&  operator>>(QDataStream & stream, StressTest& flows);
class CLOMODELLIB_EXPORT StressTest : public QObject, public BackwardInterface{
	Q_OBJECT
protected:
    inline StressTestPrivate* d_func() { return reinterpret_cast<StressTestPrivate *>(qGetPtrHelper(BackwardInterface::d_ptr)); }
    inline const StressTestPrivate* d_func() const { return reinterpret_cast<const StressTestPrivate *>(qGetPtrHelper(BackwardInterface::d_ptr)); }
    friend class StressTestPrivate;
    StressTest(StressTestPrivate *d, QObject* parent = nullptr);
	void ResetCurrentAssumption();
	bool IncreaseCurrentAssumption(int level = NumStressDimentsions-1);
	qint32 CountScenariosCalculated(int level = NumStressDimentsions - 1);
	quint32 RemoveInvalidScenarios();
	void ResetStressTest();
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
public:
    StressTest(QObject* parent = nullptr);
    virtual ~StressTest();
	void SetShowProgress(bool a=true);
	bool GetShowProgress()const;
	const QSet<QString>& GetCDRscenarios() const;
	const QSet<QString>& GetCPRscenarios() const;
	const QSet<QString>& GetLSscenarios() const;
	const QSet<QString>& GetRecLagScenarios() const;
	const QSet<QString>& GetDelinqScenarios() const;
	const QSet<QString>& GetDelinqLagScenarios() const;
	void SetCDRscenarios(const QSet<QString>& val);
	void AddCDRscenarios(const QSet<QString>& val);
	void AddCDRscenarios(const QString& val);
	void RemoveCDRscenarios(const QString& val);
	void ClearCDRscenarios();
	void SetCPRscenarios(const QSet<QString>& val);
	void AddCPRscenarios(const QSet<QString>& val);
	void AddCPRscenarios(const QString& val);
	void RemoveCPRscenarios(const QString& val);
	void ClearCPRscenarios();
	void SetLSscenarios(const QSet<QString>& val);
	void AddLSscenarios(const QSet<QString>& val);
	void AddLSscenarios(const QString& val);
	void RemoveLSscenarios(const QString& val);
	void ClearLSscenarios();
	void SetRecLagScenarios(const QSet<QString>& val);
	void AddRecLagScenarios(const QSet<QString>& val);
	void AddRecLagScenarios(const QString& val);
	void RemoveRecLagScenarios(const QString& val);
	void ClearRecLagScenarios();
	void SetDelinqScenarios(const QSet<QString>& val);
	void AddDelinqScenarios(const QSet<QString>& val);
	void AddDelinqScenarios(const QString& val);
	void RemoveDelinqScenarios(const QString& val);
	void ClearDelinqScenarios();
	void SetDelinqLagScenarios(const QSet<QString>& val);
	void AddDelinqLagScenarios(const QSet<QString>& val);
	void AddDelinqLagScenarios(const QString& val);
	void RemoveDelinqLagScenarios(const QString& val);
	void ClearDelinqLagScenarios();
	void ClearAssumptions();
	const Waterfall& GetStructure()const;
	const QDate& GetStartDate()const;
	const QHash<AssumptionSet, std::shared_ptr<Waterfall> > & GetResults() const;
	bool GetUseMultithread()const;
	void AddLoan(const Mortgage& a);
	void ResetLoans();
	void SetStructure(const Waterfall& a);
	void SetStartDate(const QDate& a);
	void UseMultithread(bool a=true);
	void SaveResults(const QString& DestPath)const;
	bool LoadResultsFromFile(const QString& DestPath);
	void SetUseFastVersion(bool a=true);
	bool GetUseFastVersion() const;
	const MtgCalculator& GetLoans() const;
	static Waterfall GetScenarioFromFile(const QString& DestPath, const QString& CPRscenario, const QString& CDRscenario, const QString& LSscenario, const QString& RecLagScenario, const QString& DelinqScenario, const QString& DelinqLagScenario);
	quint32 CountScenarios()const;
	void ResetResults();
	void ResetScenarios();
	void CompileBaseRates(ConstantBaseRateTable& Values);
	void CompileBaseRates(ForwardBaseRateTable& Values);
	void GetBaseRatesDatabase(ConstantBaseRateTable& Values, bool DownloadAll = false);
	void GetBaseRatesDatabase(ForwardBaseRateTable& Values, bool DownloadAll = false);
public slots:
	void RunStressTest();
	void StopCalculation();
private slots:
	void BaseForFastCalculated();
	void SlowLoansCalculated();
	void FastLoansCalculated();
	void RunCurrentScenario();
	void GoToNextScenario();
	void StoppedCalculation() { ResetStressTest(); }
	void GatherResults();
	void ErrorInCalculation();
	void UpdateSaveProgress(QProgressDialog* prog,int val) const;
signals:
	void CurrentScenarioCalculated();
	void AllLoansCalculated();
	void AllFinished();
	void FinishedWithErrors();
	void ErrorsOccured();
	
    friend CLOMODELLIB_EXPORT QDataStream& operator<<(QDataStream & stream, const StressTest& flows);
    friend CLOMODELLIB_EXPORT QDataStream& operator>>(QDataStream & stream, StressTest& flows);
};

#endif // StressTest_h__
