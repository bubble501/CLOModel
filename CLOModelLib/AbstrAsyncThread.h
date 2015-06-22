#ifndef AbstrAsyncThread_h__
#define AbstrAsyncThread_h__
#include <QThread>
#include "CommonFunctions.h"
class AbstrAsyncThreadPrivate;
class  CLOMODELLIB_EXPORT AbstrAsyncThread : public QThread
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(AbstrAsyncThread)
protected:
    AbstrAsyncThreadPrivate* d_ptr;
    AbstrAsyncThread(AbstrAsyncThreadPrivate* d, int ID, QObject* parent = nullptr);
public:
    AbstrAsyncThread(int ID, QObject* parent = nullptr);
    virtual ~AbstrAsyncThread();
signals:
	void AnonimCalculated(int);
	void ErrorCalculation(int);
protected slots:
	virtual void EmitFromAnonim()=0;
public slots:
	virtual void run()=0;
	virtual void stop();
};

#endif // AbstrAsyncThread_h__

