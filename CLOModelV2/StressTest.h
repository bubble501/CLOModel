#ifndef StressTest_h__
#define StressTest_h__
//#define PrintExecutionTime
#include <QString>
#include <QList>
#include <QHash>
#include <QObject>
#include "Waterfall.h"
class MtgCalculator;
class ProgressWidget;
class Mortgage;
class StressThread;
class StressTest:public QObject , public BackwardInterface{
	Q_OBJECT
public:
	//! Enum defining what parameter is currently being stressed
	enum StressVariability{
		ChangingCDR=0x1, /*!< Constant Default Rate*/
		ChangingLS=0x2, /*!< Loss Severity*/
		ChangingCPR=0x4 /*!< Constant Prepayment Rate*/
	};
private:
	quint64 ConcatenateIDs(qint32 a, qint32 b){return (static_cast<quint64>(a) << 32) | static_cast<quint64>(b);}
#ifdef PrintExecutionTime
	QTime ExecutionTime;
#endif
protected:
	ProgressWidget* ProgressForm;
	MtgCalculator* BaseCalculator;
	QList<QString> XSpann;
	QList<QString> YSpann;
	QString ConstantPar;
	//QList<Mortgage*> Loans;
	Waterfall Structure;
	QDate StartDate;
	QHash<quint64, StressThread*> ThreadsStack;
	QHash<QString,QHash<QString,Waterfall> >  Results;
	StressVariability StressDimension[2];
	bool SequentialComputation;
	int SentBees;
	int BeesReturned;
	bool ContinueCalculation;
	bool ShowProgress;
	bool UseFastVersion;
	//void CalculateScenario(int XDim,int YDim);
public:
	StressTest(QObject* parent=0);
	~StressTest();
	void SetShowProgress(bool a=true){ShowProgress=a;}
	bool GetShowProgress()const{return ShowProgress;}
	const QList<QString>& GetXSpann()const{return XSpann;}
	const QList<QString>& GetYSpann()const{return YSpann;}
	const QString& GetConstantPar()const{return ConstantPar;}
	const QList<Mortgage*>& GetLoans()const;
	const Mortgage* GetLoans(int index)const;
	Mortgage* GetLoans(int index);
	const Waterfall& GetStructure()const{return Structure;}
	const QDate& GetStartDate()const{return StartDate;}
	const QHash<QString,QHash<QString,Waterfall> > & GetResults() const{return Results;}
	StressVariability GetXVariability()const{return StressDimension[0];}
	StressVariability GetYVariability()const{return StressDimension[1];}
	bool GetUseMultithread()const {return !SequentialComputation;}
	void SetXSpann(const QList<QString>& a);
	void AddXSpann(const QList<QString>& a);
	void AddXSpann(const QString& a);
	void ResetXSpann() {XSpann.clear();}
	void SetYSpann(const QList<QString>& a);
	void AddYSpann(const QList<QString>& a);
	void AddYSpann(const QString& a);
	void ResetYSpann() {YSpann.clear();}
	void ResetResult() {Results.clear();}
	void SetConstantPar(const QString& a);
	void AddLoan(const Mortgage& a);
	void ResetLoans();
	void SetStructure(const Waterfall& a){Structure=a;}
	void SetStartDate(const QDate& a){StartDate=a;}
	void SetXVariability(StressVariability a){StressDimension[0]=a;}
	void SetYVariability(StressVariability a){StressDimension[1]=a;}
	void UseMultithread(bool a=true){SequentialComputation=!a;}
	void ResetStressLevels(){XSpann.clear(); YSpann.clear();}
	void SaveResults(const QString& DestPath)const;
	bool LoadResultsFromFile(const QString& DestPath);
	void SetUseFastVersion(bool a=true) { UseFastVersion = a; }
	bool GetUseFastVersion() { return UseFastVersion; }
	static Waterfall GetScenarioFromFile(const QString& DestPath,const QString& XScenario,const QString& YScenario);
	friend QDataStream& operator<<(QDataStream & stream, const StressTest& flows);
	friend QDataStream& operator>>(QDataStream & stream, StressTest& flows);
public slots:
	void RunStressTest();
private slots:
	void StartStresses();
	void RecievedData(int IDx,int IDy,const Waterfall& Res);
	void StopCalculation();
protected:
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
signals:
	void ProgressStatus(double);
	void AllFinished();
};
QDataStream& operator<<(QDataStream & stream, const StressTest& flows);
QDataStream& operator>>(QDataStream & stream, StressTest& flows);
#endif // StressTest_h__
