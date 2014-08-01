#ifndef NO_BLOOMBERG
#ifndef AsyncEventHandler_h__
#define AsyncEventHandler_h__
#include <QObject>
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
	void ErrorOccurred(qint64, BloombergRequest::BbgErrorCodes);
	void DataRecieved(qint64);

	

};
#endif // AsyncEventHandler_h__
#endif