#ifndef CentralUnitAPI_h__
#define CentralUnitAPI_h__
#include <QObject>
#include "MtgCalculator.h"
#include "Waterfall.h"
#include "BaseRateTable.h"
class WaterfallCalculator;
class ProgressWidget;
class CentralUnitAPI : public QObject{
	Q_OBJECT
public:
	CentralUnitAPI(QObject* parent=0);
	~CentralUnitAPI();
	void SetStructure(const Waterfall& a){Structure=a;}
	const Waterfall& GetStructure()const{return Structure;}
	Waterfall& GetStructure(){return Structure;}
	void AddLoan(const Mortgage& a){LoansCalculator.AddLoan(a);}
	void ResetLoans(){LoansCalculator.Reset();}
	int LoansCount() const{return LoansCalculator.GetLoans().size();}
	const Mortgage* GetLoan(int index)const;
	Mortgage* GetLoan(int index);
	void Calculate();
	bool GetRunCall() const{return RunCall;}
	void SetRunCall(bool a){RunCall=a;}
	void SetPoolCutOff(const QDate& a){PoolCutOff=a;}
	const QDate& GetPoolCutOff()const{return PoolCutOff;}
	void SetShowProgress(bool a){ShowProgress=a;}
	bool GetShowProgress()const {return ShowProgress;}
	const Waterfall& GetResult()const{return GetStructure();}
	Waterfall& GetResult(){return GetStructure();}
	const Waterfall& GetCallResult()const{return CallStructure;}
	Waterfall& GetCallResult(){return CallStructure;}
	void SaveBaseCase(const QString& FolderPath,const QString& BaseCaseFileName);
	bool LoadBaseCase(const QString& DestPath);
	void SaveLoanPool(const QString& FolderPath,const QString& LoanPoolFileName);
	bool LoadLoanPool(const QString& DestPath);
	void SaveModel(const QString& FolderPath, const QString& ModelFileName);
	void SaveModel(const QString& ModelFileName);
	bool LoadModel(const QString& DestPath);
	void SetSequentialComputation(bool a);
	bool GetSequentialComputation()const {return SequentialComputation;}
	const  QDate& GetLiborUpdateDate() const { return LiborUpdateDate; }
	bool GetUseForwardCurve() const { return m_UseForwardCurve; }
	void CompileBaseRates(ForwardBaseRateTable& Values);
	void CompileBaseRates(ConstantBaseRateTable& Values);
#ifndef NO_DATABASE
	void GetBaseRatesDatabase(ConstantBaseRateTable& Values, bool DownloadAll=false);
	void GetBaseRatesDatabase(ForwardBaseRateTable& Values, bool DownloadAll=false);
#endif
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
	mutable QDate LiborUpdateDate;
	mutable bool m_UseForwardCurve;
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

