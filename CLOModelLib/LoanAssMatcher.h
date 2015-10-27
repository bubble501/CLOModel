#ifndef LOANASSMATCHER_H
#define LOANASSMATCHER_H
#include "TemplAsyncCalculator.h"
#include "LoanAssMatcherThread.h"
#include "LoanAssMatcherResult.h"
class LoanAssMatcherPrivate;
class LoanAssMatcher;
CLOMODELLIB_EXPORT QDataStream&  operator<<(QDataStream & stream, const LoanAssMatcher& flows);
CLOMODELLIB_EXPORT QDataStream&  operator>>(QDataStream & stream, LoanAssMatcher& flows);
class CLOMODELLIB_EXPORT LoanAssMatcher : public TemplAsyncCalculator<LoanAssMatcherThread, LoanAssMatcherResult>
{
	Q_OBJECT
protected:
    inline LoanAssMatcherPrivate* d_func() { return reinterpret_cast<LoanAssMatcherPrivate *>(qGetPtrHelper(BackwardInterface::d_ptr)); }
    inline const LoanAssMatcherPrivate* d_func() const { return reinterpret_cast<const LoanAssMatcherPrivate *>(qGetPtrHelper(BackwardInterface::d_ptr)); }
    friend class LoanAssMatcherPrivate;
    LoanAssMatcher(LoanAssMatcherPrivate *d, QObject* parent = nullptr);
    virtual void BeeReturned(int Ident, const LoanAssMatcherResult& a) override;
    virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
public slots:
    virtual std::tuple<bool, QString> StartCalculation() override;
protected slots :
	void ClearFilesInFolder();
public:
    virtual void Reset() override;
    virtual int NumBees() const override;
    virtual QString ReadyToCalculate() const override;
    LoanAssMatcher(QObject *parent = nullptr);
    virtual ~LoanAssMatcher();
    void GetAssumptionsDatabase();
    void AddAssumption(const QString& key, const LoanAssumption& a);
    void RemoveAssumption(const QString& key);
    const QString& GetFolderToScan() const;
    void SetFolderToScan(const QString& val);
	friend CLOMODELLIB_EXPORT QDataStream& operator<<(QDataStream & stream, const LoanAssMatcher& flows);
	friend CLOMODELLIB_EXPORT QDataStream& operator>>(QDataStream & stream, LoanAssMatcher& flows);
};


#endif // LOANASSMATCHER_H
