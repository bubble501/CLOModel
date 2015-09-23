#ifndef MtgCalculator_h__
#define MtgCalculator_h__
#include "TemplAsyncCalculator.h"
#include "MtgCashFlow.h"
#include "CommonFunctions.h"
#include "MtgCalculatorThread.h"
class Mortgage;
class MtgCashFlow;
class ForwardBaseRateTable;
class ConstantBaseRateTable;
class MtgCalculatorPrivate;
namespace simstring { class reader; }
class MtgCalculator;
CLOMODELLIB_EXPORT QDataStream&  operator<<(QDataStream & stream, const MtgCalculator& flows);
CLOMODELLIB_EXPORT QDataStream&  operator>>(QDataStream & stream, MtgCalculator& flows);
class CLOMODELLIB_EXPORT MtgCalculator : public TemplAsyncCalculator <MtgCalculatorThread, MtgCashFlow>
{
	Q_OBJECT
protected:
    inline MtgCalculatorPrivate* d_func() { return reinterpret_cast<MtgCalculatorPrivate *>(qGetPtrHelper(BackwardInterface::d_ptr)); }
    inline const MtgCalculatorPrivate* d_func() const { return reinterpret_cast<const MtgCalculatorPrivate *>(qGetPtrHelper(BackwardInterface::d_ptr)); }
    friend class MtgCalculatorPrivate;
    MtgCalculator(MtgCalculatorPrivate *d, QObject* parent = nullptr);
    void ClearTempProperties();
    void AddTempProperty(qint32 LoanID, const QString& PropertyName, const QString& PropertyValue);
    QString GetGeography(const QString& guess, simstring::reader& dbr) const;
    QString GetCountryISOCode(QString name) const;
    virtual void BeeReturned(int Ident, const MtgCashFlow& a) override;
    virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
public slots:
	virtual bool StartCalculation() override;
public:
    virtual bool uploadGeographyToDatabase(const QString& DealName, QDate startDate, QDate endDate) const;
	virtual QHash<QString, double> GetGeographicBreakdown() const;
	virtual QString ReadyToCalculate() const override;
    MtgCalculator(QObject* parent = nullptr);
	virtual ~MtgCalculator();
	virtual void Reset() override;
	virtual void ClearResults() override;
    virtual QList<qint32> GetResultsKeys() const override;
    virtual const std::shared_ptr<MtgCashFlow> GetResult(qint32 key)const override;
	virtual int NumBees() const override;
	void AddLoan(const Mortgage& a, qint32 Index);
    const QHash<qint32, std::shared_ptr<Mortgage> >& GetLoans() const;
    QHash<qint32, std::shared_ptr<Mortgage> >& GetLoans();
	void SetLoans(const QHash<qint32, Mortgage*>& a);
	void SetStartDate(const QDate& a);
	const QDate& GetStartDate()const;
	bool GetUseStoredCashFlows() const;
	void SetUseStoredCashFlows(bool val);
	const QString& GetCPRass() const;
	void SetCPRass(const QString& val);
	const QString& GetCDRass() const;
	void SetCDRass(const QString& val);
	const QString& GetLSass() const;
	void SetLSass(const QString& val);
	const QString& GetRecoveryLag() const;
	void SetRecoveryLag(const QString& val);
	const QString& GetDelinquency() const;
	void SetDelinquency(const QString& val);
	const QString& GetDelinquencyLag() const;
	void SetDelinquencyLag(const QString& val);
	void CompileReferenceRateValue(ForwardBaseRateTable& Values);
	void CompileReferenceRateValue(ConstantBaseRateTable& Values);
	void ClearLoans();
	void GetBaseRatesDatabase(ConstantBaseRateTable& Values, bool DownloadAll=false);
	void GetBaseRatesDatabase(ForwardBaseRateTable& Values, bool DownloadAll=false);
	void DownloadScenarios();
	void GuessLoanScenarios(bool OverrideAss);
	void SetOverrideAssumptions(bool a);
	bool GetOverrideAssumptions()const;
	const MtgCashFlow* GetAggregatedResults()const;
	bool GetDownloadScenario() const;
	void SetDownloadScenario(bool val);
    bool SaveIndividualFlows() const;
    void SaveIndividualFlows(bool val);
	friend CLOMODELLIB_EXPORT QDataStream& operator<<(QDataStream & stream, const MtgCalculator& flows);
	friend CLOMODELLIB_EXPORT QDataStream& operator>>(QDataStream & stream, MtgCalculator& flows);
};

#endif // MtgCalculator_h__