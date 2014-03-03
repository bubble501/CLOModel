#ifndef StressTest_h__
#define StressTest_h__
#include <QObject>
#include <QHash>
#include <QString>
#include <QList>
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
public:
	StressTest(QObject* parent=0);
	~StressTest();
	void SetShowProgress(bool a=true);
	bool GetShowProgress() const;
	const QList<QString>& GetXSpann()const;
	const QList<QString>& GetYSpann()const;
	const QString& GetConstantPar()const;
	const QList<Mortgage*>& GetLoans()const;
	const Mortgage* GetLoans(int index)const;
	const Waterfall& GetStructure()const;
	const QDate& GetStartDate()const;
	const QHash<QString,QHash<QString,Waterfall> > & GetResults() const;
	StressVariability GetXVariability()const;
	StressVariability GetYVariability()const;
	bool GetUseMultithread()const;
	void SetXSpann(const QList<QString>& a);
	void AddXSpann(const QList<QString>& a);
	void AddXSpann(const QString& a);
	void ResetXSpann();
	void SetYSpann(const QList<QString>& a);
	void AddYSpann(const QList<QString>& a);
	void AddYSpann(const QString& a);
	void ResetYSpann();
	void ResetResult();
	void SetConstantPar(const QString& a);
	void AddLoan(const Mortgage& a);
	void ResetLoans();
	void SetStructure(const Waterfall& a);
	void SetStartDate(const QDate& a);
	void SetXVariability(StressVariability a);
	void SetYVariability(StressVariability a);
	void UseMultithread(bool a=true);
	void ResetStressLevels();
	void SaveResults(const QString& DestPath)const;
	bool LoadResultsFromFile(const QString& DestPath);
	static Waterfall GetScenarioFromFile(const QString& DestPath,const QString& XScenario,const QString& YScenario);
public slots:
	void RunStressTest();
signals:
	void AllFinished();
	void ProgressStatus(double);
};
#endif // StressTest_h__
