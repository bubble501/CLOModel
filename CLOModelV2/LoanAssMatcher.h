#ifndef LOANASSMATCHER_H
#define LOANASSMATCHER_H
#include "TemplAsyncCalculator.h"
#include "LoanAssMatcherThread.h"
#include "LoanAssMatcherResult.h"
class LoanAssMatcher : public TemplAsyncCalculator<LoanAssMatcherThread,LoanAssMatcherResult>
{
	Q_OBJECT

public:
	LoanAssMatcher(QObject *parent=nullptr);
	void GetAssumptionsDatabase();
	void AddAssumption(const QString& key,const LoanAssumption& a);
	void RemoveAssumption(const QString& key);
	void Clear();
	const QString& GetFolderToScan() const { return m_FolderToScan; }
	void SetFolderToScan(const QString& val) { m_FolderToScan = val; }
private:
	QHash<QString, QSharedPointer<LoanAssumption> > m_AvailableAssumptions;
	QString m_FolderToScan;
	
};

#endif // LOANASSMATCHER_H
