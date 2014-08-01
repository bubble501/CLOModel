#ifndef NO_BLOOMBERG
#ifndef AbstractBbgWorker_h__
#define AbstractBbgWorker_h__
#include <QString>
#include <QHash>
#include "BloombergRequest.h"
#include <blpapi_correlationid.h>
#include <blpapi_event.h>
#include <blpapi_message.h>
#include <blpapi_request.h>
#include <blpapi_session.h>
using namespace BloombergLP;
using namespace blpapi;
class AsyncEventHandler;
class AbstractBbgWorker {
public:
	AbstractBbgWorker();
	const QString& GetServerAddress() const { return ServerAddress; }
	void SetServerAddress(const QString& val) { ServerAddress = val.trimmed(); }
	quint16 GetServerPort() const { return ServerPort; }
	void SetServerPort(quint16 val) { ServerPort = val; }
	virtual void StartRequest()=0;
	virtual void StartRequest(const BloombergRequest& a) { SetRequest(a); StartRequest(); }
	void SetRequest(const BloombergRequest& a) { m_Requests = a; }
	const BloombergRequest& GetResults() const { return m_Requests; }
protected:
	QHash<qint64, QList<qint64> > Groups;
	QString ServerAddress;
	quint16 ServerPort;
	BloombergRequest m_Requests;
	friend AsyncEventHandler;
};
#endif // AbstractBbgWorker_h__
#endif
