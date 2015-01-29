#ifndef LOANASSMATCHER_H
#define LOANASSMATCHER_H
#include "TemplAsyncCalculator.h"
#include "LoanAssMatcherThread.h"
#include "LoanAssMatcherResult.h"
#include <QFileInfoList>
class LoanAssMatcher : public TemplAsyncCalculator<LoanAssMatcherThread,LoanAssMatcherResult>
{
	Q_OBJECT
public slots:
	virtual bool StartCalculation() override;
public:
	virtual void Reset() override;
	virtual int NumBees() const override;
	virtual QString ReadyToCalculate() const override;
	LoanAssMatcher(QObject *parent=nullptr);
	void GetAssumptionsDatabase();
	void AddAssumption(const QString& key,const LoanAssumption& a);
	void RemoveAssumption(const QString& key);
	const QString& GetFolderToScan() const { return m_FolderToScan; }
	void SetFolderToScan(const QString& val);
protected:
	QHash<QString, QSharedPointer<LoanAssumption> > m_AvailableAssumptions;
	QString m_FolderToScan;
	virtual void BeeReturned(int Ident, const LoanAssMatcherResult& a) override;
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
private slots:
	void ClearFilesInFolder() { RETURN_WHEN_RUNNING(true, ) m_FilesInFolder.clear(); }
private:
	QFileInfoList m_FilesInFolder;
	friend QDataStream& operator<<(QDataStream & stream, const LoanAssMatcher& flows);
	friend QDataStream& operator>>(QDataStream & stream, LoanAssMatcher& flows);
};
QDataStream& operator<<(QDataStream & stream, const LoanAssMatcher& flows);
QDataStream& operator>>(QDataStream & stream, LoanAssMatcher& flows);

#endif // LOANASSMATCHER_H
