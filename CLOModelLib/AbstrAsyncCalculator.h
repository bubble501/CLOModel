#ifndef AbstrAsyncCalculator_h__
#define AbstrAsyncCalculator_h__
#include "BackwardCompatibilityInterface.h"
#include <QObject>
#include <memory>
#include <QString>
#include <QFile>
#include <tuple>
#ifndef RETURN_WHEN_RUNNING
#define RETURN_WHEN_RUNNING(rvr,retval) if( ContinueCalculation() == rvr) return retval;
#endif
class AbstrAsyncCalculatorPrivate;
namespace ConcurrentFunctions { 
    QString checkReadyToCalculateLoan(const QString& path); 
    QString checkReadyToCalculateWaterfall(const QString& path);
}
class CLOMODELLIB_EXPORT AbstrAsyncCalculator : public QObject, public BackwardInterface
{
	Q_OBJECT	
protected:
    friend QString ConcurrentFunctions::checkReadyToCalculateLoan(const QString& path);
    friend QString ConcurrentFunctions::checkReadyToCalculateWaterfall(const QString& path);
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
    template <class T> bool editTempFile(const QString& path, const T& val) const
    {
        QFile destFile(path);
        if (destFile.open(QIODevice::WriteOnly)) {
            QDataStream out(&destFile);
            out.setVersion(StreamVersionUsed);
            out << val;
            destFile.close();
            return true;
        }
        PrintToTempFile("PermissionError", QString("Could not write temporary file %1").arg(path));
        Q_ASSERT_X(false, "writeTempFile", QString("Unable to write file: %1").arg(path).toLatin1().data());
        return false;
    }
    template <class T> QString writeTempFile(const T& val) const
    { 
        QString tempFileName;
        // Generate random file name
        for (int i = 0; i < 5;++i)
            tempFileName.append(static_cast<char>('a' + qrand() % (1 + 'z' - 'a')));
        while (QFile::exists(getDataDirPath() + '/' + tempFileName)) 
            tempFileName.append(static_cast<char>('a' + qrand() % (1 + 'z' - 'a')));
        QFile destFile(getDataDirPath() + '/' + tempFileName );
        if (Q_LIKELY(destFile.open(QIODevice::WriteOnly))) {
            QDataStream out(&destFile);
            out.setVersion(StreamVersionUsed);
            out << val;
            destFile.close();
            return destFile.fileName();
        }
        PrintToTempFile("PermissionError", QString("Could not write temporary file %1 to save Waterfall").arg(tempFileName));
        Q_ASSERT_X(false, "writeTempFile", QString("Unable to write file: %1").arg(getDataDirPath() + '/' + tempFileName + ".aactf").toLatin1().data());
        return QString();
    }
    static void removeTempFile(const QString& path);
    template <class T> static T readTempFile(const QString& path)
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
    const QMultiHash<int, QString>& errors() const;
    void clearErrors();
    QString allErrors() const;
signals :
	void Calculated();
	void BeeCalculated(int);
	void BeeError(int,QString);
	void Progress(double);
	void ProgressPct(int);
	void Stopped();
private slots:
	void SendPctSignal(double a);
    void addError(int id, QString err);
public slots:
	virtual void StopCalculation();
	virtual std::tuple<bool,QString> StartCalculation()=0;
};
#endif // AbstrAsyncCalculator_h__

