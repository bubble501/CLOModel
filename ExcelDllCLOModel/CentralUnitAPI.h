#ifndef CentralUnitAPI_h__
#define CentralUnitAPI_h__
#include "unamangedclomodel_global.h"
#include <QObject>
#include "MtgCalculator.h"
#include "Waterfall.h"
class WaterfallCalculator;
class ProgressWidget;
class UNAMANGEDCLOMODEL_EXPORT CentralUnitAPI : public QObject{
	Q_OBJECT
public:
	CentralUnitAPI(QObject* parent=0);
	~CentralUnitAPI();
	void SetStructure(const Waterfall& a){Structure=a;}
	const Waterfall& GetStructure()const{return Structure;}
	void AddLoan(const Mortgage& a){LoansCalculator.AddLoan(a);}
	void ResetLoans(){LoansCalculator.Reset();}
	int LoansCount() const{return LoansCalculator.GetLoans().size();}
	const Mortgage* GetLoan(int index);
	void Calculate();
	bool GetRunCall() const{return RunCall;}
	void SetRunCall(bool a){RunCall=a;}
	void SetPoolCutOff(const QDate& a){PoolCutOff=a;}
	const QDate& GetPoolCutOff()const{return PoolCutOff;}
	void SetShowProgress(bool a){ShowProgress=a;}
	bool GetShowProgress()const {return ShowProgress;}
	const Waterfall& GetResult(){return Structure;}
	const Waterfall& GetCallResult(){return CallStructure;}
	void SaveBaseCase(const QString& FolderPath,const QString& BaseCaseFileName);
	bool LoadBaseCase(const QString& DestPath);
	void SaveLoanPool(const QString& FolderPath,const QString& LoanPoolFileName);
	bool LoadLoanPool(const QString& DestPath);
	void SetSequentialComputation(bool a);
	bool GetSequentialComputation()const {return SequentialComputation;}
private:
	ProgressWidget* MtgsProgress;
	WaterfallCalculator* ParallWatFalls;
	MtgCalculator LoansCalculator;
	Waterfall Structure;
	Waterfall CallStructure;
	QDate PoolCutOff;
	bool RunCall;
	bool ShowProgress;
	bool SequentialComputation;
signals:
	void LoopStarted();
	void ProgressUpdate(double);
	void CalculationFinished();
private slots:
	void CalculationStep2();
	void CalculationStep1();
	void CheckCalculationDone();
	void HandleProgress(int a);
};
#endif // CentralUnitAPI_h__

