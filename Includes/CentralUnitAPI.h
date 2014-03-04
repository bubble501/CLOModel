#ifndef CentralUnitAPI_h__
#define CentralUnitAPI_h__
#include <QObject>
#include <Waterfall.h>
#include <Mortgage.h>
class CentralUnitAPI : public QObject{
	Q_OBJECT
public:
	CentralUnitAPI(QObject* parent=0);
	~CentralUnitAPI();
	void SetStructure(const Waterfall& a);
	const Waterfall& GetStructure()const;
	void AddLoan(const Mortgage& a);
	void ResetLoans();
	int LoansCount() const;
	const Mortgage* GetLoan(int index);
	void Calculate();
	bool GetRunCall() const;
	void SetRunCall(bool a);
	void SetPoolCutOff(const QDate& a);
	const QDate& GetPoolCutOff()const;
	void SetShowProgress(bool a);
	bool GetShowProgress()const;
	const Waterfall& GetResult();
	const Waterfall& GetCallResult();
	void SaveBaseCase(const QString& FolderPath,const QString& BaseCaseFileName);
	bool LoadBaseCase(const QString& DestPath);
	void SaveLoanPool(const QString& FolderPath,const QString& LoanPoolFileName);
	bool LoadLoanPool(const QString& DestPath);
	void SetSequentialComputation(bool a);
	bool GetSequentialComputation()const;
signals:
	void LoopStarted();
	void ProgressUpdate(double);
	void CalculationFinished();
};
#endif // CentralUnitAPI_h__

