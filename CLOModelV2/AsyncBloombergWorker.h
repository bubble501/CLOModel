#ifndef NO_BLOOMBERG
#ifndef AsyncBloombergWorker_h__
#define AsyncBloombergWorker_h__
#include <QObject>
#include <QStringList>
#include "AbstractBbgWorker.h"
#include "AsyncEventHandler.h"
class AsyncBloombergWorker :public QObject, public AbstractBbgWorker{
	Q_OBJECT
public:
	AsyncBloombergWorker(QObject* parent=0);
	~AsyncBloombergWorker();
	using AbstractBbgWorker::StartRequest;
	virtual void StartRequest();
	void StopRequest();
private:
	AsyncEventHandler* Hive;
	Session* session;
	QList<qint64> RecievedGroups;
	friend AsyncEventHandler;
private slots:
	void ErrorOccurredInHadle(qint64 GroupID, qint64 RequestID, qint32 ErrorID);
	void DataRecievedInHadle(qint64 GroupID, qint64 RequestID, const QString& Value, const QString& Header);
	void DataRecievedInHadle(qint64 GroupID, qint64 RequestID);
	void DataRowRecievedInHadle(qint64 GroupID, qint64 RequestID, const QStringList& Value, const QStringList& Header);
	void SessionTerminated();
signals:
	void Recieved(qint64);
	void UpdateProgress(qint32);
	void Started();
	void Stopped();
	void Finished();
};
#endif // AsyncBloombergWorker_h__

#endif