#ifndef StressTest_h__
#define StressTest_h__
//#define PrintExecutionTime
#include <QString>
#include <QList>
#include <QHash>
#include <QObject>
#include "Waterfall.h"
class ProgressWidget;
class Mortgage;
class StressTest:public QObject{
	Q_OBJECT
public:
	//! Enum defining what parameter is currently being stressed
	enum StressVariability{
		ChangingCDR, /*!< Constant Default Rate*/
		ChangingLS, /*!< Loss Severity*/
		ChangingCPR /*!< Constant Prepayment Rate*/
	};
private:
#ifdef PrintExecutionTime
	QTime ExecutionTime;
#endif
	ProgressWidget* ProgressForm;
	QList<QString> XSpann;
	QList<QString> YSpann;
	QString ConstantPar;
	QList<Mortgage*> Loans;
	Waterfall Structure;
	QDate StartDate;
	QHash<QString,QHash<QString,Waterfall> >  Results;
	StressVariability StressDimension[2];
	bool SequentialComputation;
	int SentBees;
	int BeesReturned;
	bool ContinueCalculation;
	void CalculateScenario(int XDim,int YDim);
public:
	StressTest(QObject* parent=0);
	~StressTest();
	const QList<QString>& GetXSpann()const{return XSpann;}
	const QList<QString>& GetYSpann()const{return YSpann;}
	const QString& GetConstantPar()const{return ConstantPar;}
	const QList<Mortgage*>& GetLoans()const{return Loans;}
	const Mortgage* GetLoans(int index)const;
	const Waterfall& GetStructure()const{return Structure;}
	const QDate& GetStartDate()const{return StartDate;}
	const QHash<QString,QHash<QString,Waterfall> > & GetResults() const{return Results;}
	StressVariability GetXVariability()const{return StressDimension[0];}
	StressVariability GetYVariability()const{return StressDimension[1];}
	bool UseMultithread()const {return !SequentialComputation;}
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
	static Waterfall GetScenarioFromFile(const QString& DestPath,const QString& XScenario,const QString& YScenario);
	friend QDataStream& operator<<(QDataStream & stream, const StressTest& flows);
	friend QDataStream& operator>>(QDataStream & stream, StressTest& flows);
public slots:
	void RunStressTest();
private slots:
	void RecievedData(int IDx,int IDy,const Waterfall& Res);
	void StopCalculation();
signals:
	void AllFinished();
};
QDataStream& operator<<(QDataStream & stream, const StressTest& flows);
QDataStream& operator>>(QDataStream & stream, StressTest& flows);
#endif // StressTest_h__
