
#ifndef AbstrAsyncCalculator_h__
#define AbstrAsyncCalculator_h__
#include "BackwardCompatibilityInterface.h"
#include <QObject>
#include <memory>
#include <QTemporaryFile>
#ifndef RETURN_WHEN_RUNNING
#define RETURN_WHEN_RUNNING(rvr,retval) if( ContinueCalculation() == rvr) return retval;
#endif
class AbstrAsyncCalculatorPrivate;
class CLOMODELLIB_EXPORT AbstrAsyncCalculator : public QObject, public BackwardInterface
{
	Q_OBJECT	
protected:
    inline AbstrAsyncCalculatorPrivate* d_func() { return reinterpret_cast<AbstrAsyncCalculatorPrivate *>(qGetPtrHelper(BackwardInterface::d_ptr)); }
    inline const AbstrAsyncCalculatorPrivate* d_func() const { return reinterpret_cast<const AbstrAsyncCalculatorPrivate *>(qGetPtrHelper(BackwardInterface::d_ptr)); }
    friend class AbstrAsyncCalculatorPrivate;
    AbstrAsyncCalculator(AbstrAsyncCalculatorPrivate *d, QObject* parent = nullptr);
    virtual const QHash<qint32, QString >& getResultPaths() const;
    virtual QHash<qint32, QString >& getResultPaths();
    virtual QHash<qint32, QPointer<QObject> >& getThreadPool();
    virtual const QHash<qint32, QPointer<QObject> >& getThreadPool() const;
    virtual const QSet<qint32>& getBeeSent() const;
    virtual QSet<qint32>& getBeeSent();
    virtual qint32& getBeesReturned();
    virtual const qint32& getBeesReturned() const;
    virtual QString getResultPaths(qint32 key)const;
    virtual bool ContinueCalculation() const;
    virtual void setContinueCalculation(bool val);
    virtual int availableThreads() const;
    void insertResult(qint32 Key, const QString& path);
    QString getDataDirPath() const;
    template <class T> QString writeTempFile(const T& val) const
    {
        QTemporaryFile destFile(getDataDirPath() + '/');
        destFile.setAutoRemove(false);
        if (destFile.open()) {
            QDataStream out(&destFile);
            out.setVersion(StreamVersionUsed);
            out << val;
            destFile.close();
            return destFile.fileName();
        }
        PrintToTempFile("PermissionError", "Could not write temporary file to save Waterfall");
        return QString();
    }
    void removeTempFile(const QString& path) const;
    template <class T> T readTempFile(const QString& path) const
    {
        
        static_assert(std::is_base_of<BackwardInterface, T >::value, "T must inherit from BackwardInterface");
        static_assert(std::is_default_constructible< T >::value, "T must have a default constructor");
        T result;
        if (path.isEmpty())
            return result;
        QFile sourceFile(path);
        if (sourceFile.open(QIODevice::ReadOnly)) {
            QDataStream in(&sourceFile);
            in.setVersion(StreamVersionUsed);
            in >> result;
            sourceFile.close();
        }
        else
            PrintToTempFile("PermissionError", "Could not read temporary file to load Waterfall");
        return result;
    }
public:
    virtual quint8 operativity() const;
    virtual void setOperativity(quint8 val);
    AbstrAsyncCalculator(QObject* parent =nullptr);
    virtual ~AbstrAsyncCalculator();
    virtual QString ReadyToCalculate() const = 0;
    virtual void SetSequentialComputation(bool a);
    virtual bool GetSequentialComputation()const;
    virtual QList<qint32> GetResultKeys() const;
    virtual int NumBees() const = 0;
signals :
	void Calculated();
	void BeeCalculated(int);
	void BeeError(int);
	void Progress(double);
	void ProgressPct(int);
	void Stopped();
private slots:
	void SendPctSignal(double a);
public slots:
	virtual void StopCalculation();
	virtual bool StartCalculation()=0;
};
#endif // AbstrAsyncCalculator_h__

