#ifndef NO_BLOOMBERG
#ifndef AsyncEventHandler_h__
#define AsyncEventHandler_h__
#include <QObject>
#include <QStringList>
#include "BloombergRequest.h"
#include <blpapi_correlationid.h>
#include <blpapi_event.h>
#include <blpapi_message.h>
#include <blpapi_request.h>
#include <blpapi_session.h>
using namespace BloombergLP;
using namespace blpapi;
class AsyncBloombergWorker;
class AsyncEventHandler :public QObject, public EventHandler {
	Q_OBJECT
public:
	AsyncEventHandler(AsyncBloombergWorker* parent);
private:
	void handleResponseEvent(const Event& event);
	bool processEvent(const Event& event, Session *session);
signals:
	void SessionTerminated();
	void Started();
	void ErrorOccurred(qint64 GroupID, qint64 RequestID, qint32 ErrorID);
	void DataRecieved(qint64 GroupID, qint64 RequestID, QString Value, QString Header);
	void DataRecieved(qint64 GroupID, qint64 RequestID);
	void DataRowRecieved(qint64 GroupID, qint64 RequestID, QStringList Value, QStringList Header);
};
#endif // AsyncEventHandler_h__
#endif