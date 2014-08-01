#ifndef NO_BLOOMBERG
#ifndef AsyncBloombergWorker_h__
#define AsyncBloombergWorker_h__
#include <QObject>
#include "AbstractBbgWorker.h"
#include "AsyncEventHandler.h"
class AsyncBloombergWorker :public QObject, public AbstractBbgWorker{
	Q_OBJECT
public:
	AsyncBloombergWorker(QObject* parent=0);
	using AbstractBbgWorker::StartRequest;
	virtual void StartRequest();
private:
	AsyncEventHandler* Hive;
	Session* session;
	friend AsyncEventHandler;
signals:
	void DataRecieved(qint64);
};
#endif // AsyncBloombergWorker_h__

#endif