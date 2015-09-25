#ifndef LOANASSMATCHERTHREAD_H
#define LOANASSMATCHERTHREAD_H
#include "TemplAsyncThread.h"
#include "LoanAssMatcherResult.h"
#include <memory>
class LoanAssumption;
class LoanAssMatcherThreadPrivate;
class CLOMODELLIB_EXPORT LoanAssMatcherThread : public TemplAsyncThread<LoanAssMatcherResult>
{
	Q_OBJECT
protected:
    Q_DECLARE_PRIVATE(LoanAssMatcherThread)
    LoanAssMatcherThread(LoanAssMatcherThreadPrivate* d, int ID, QObject* parent = nullptr); 
public:
    LoanAssMatcherThread(int ID, QObject *parent = nullptr);
	const QString& GetModelToScan() const;
	void SetModelToScan(const QString& val);
	void SetAvailableAssumptions(const QHash<QString, std::shared_ptr<LoanAssumption> >& a);
signals:
	void Calculated(int, const LoanAssMatcherResult&);
protected slots:
	virtual void EmitFromAnonim()override;
public slots :
	virtual void run() override;
private:
	void ReturnError();
};

#endif // LOANASSMATCHERTHREAD_H
