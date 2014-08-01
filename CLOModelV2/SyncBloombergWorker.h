#ifndef NO_BLOOMBERG
#ifndef BloombergWorker_h__
#define BloombergWorker_h__
#include <blpapi_event.h>
#include <QStringList>
#include <QHash>
#include "BloombergRequest.h"
class SyncBloombergWorker {
private:
	bool continueToLoop;
	QHash<qint64, QList<qint64> > Groups;
	QString ServerAddress;
	quint16 ServerPort;
	BloombergRequest m_Requests;
	void handleResponseEvent(const BloombergLP::blpapi::Event& event) ;
public:
	const QString& GetServerAddress() const { return ServerAddress; }
	void SetServerAddress(const QString& val) { ServerAddress = val.trimmed(); }
	quint16 GetServerPort() const { return ServerPort; }
	void SetServerPort(quint16 val) { ServerPort = val; }
	const BloombergRequest& StartRequest();
	const BloombergRequest& StartRequest(const BloombergRequest& a) { SetRequest(a); return StartRequest(); }
	void SetRequest(const BloombergRequest& a) { m_Requests = a; }
	const BloombergRequest& GetResults() const { return m_Requests; }
	SyncBloombergWorker();
};
#endif // BloombergWorker_h__
#endif // NO_BLOOMBERG
