#ifndef AbstrAsyncThread_h__
#define AbstrAsyncThread_h__
#include <QThread>
class AbstrAsyncThread : public QThread {
	Q_OBJECT
public:
	AbstrAsyncThread(QObject* parent = 0) 
		:QThread(parent)
	{
		connect(this, SIGNAL(AnonimCalculated(int)), this, SLOT(EmitFromAnonim()));
		connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
	}
    virtual ~AbstrAsyncThread()
    {
        while (isRunning()) {
            wait();
        }
    }
signals:
	void AnonimCalculated(int);
	void ErrorCalculation(int);
protected slots:
	virtual void EmitFromAnonim()=0;
public slots:
	virtual void run()=0;
	virtual void stop() { exit(0); }
};

#endif // AbstrAsyncThread_h__

