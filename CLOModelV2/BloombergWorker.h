#ifndef NO_BLOOMBERG
#ifndef BloombergWorker_h__
#define BloombergWorker_h__
#include <QObject>
#include <QHash>
#include "BloombergRequest.h"
#include <blpapi_session.h>
class BloombergWorker : public QObject, private BloombergLP::blpapi::EventHandler {
	Q_OBJECT
public:
	BloombergWorker(QObject* parent = 0); 
	~BloombergWorker();
	const QString& GetServerAddress() const { return ServerAddress; }
	void SetServerAddress(const QString& val) { ServerAddress = val.trimmed(); }
	quint16 GetServerPort() const { return ServerPort; }
	void SetServerPort(quint16 val) { ServerPort = val; }
	void SetRequest(const BloombergRequest& a) { if(!session) m_Requests = a; }
	const BloombergRequest& StartRequestSync() { m_UseSyncronous = true; RunRequest(); return m_Requests; }
	const BloombergRequest& StartRequestSync(const BloombergRequest& a) { SetRequest(a); return StartRequestSync(); }
	const BloombergRequest& GetResults() const { return m_Requests; }
	bool isRunning() const { return session && !m_SessionFinished; }
public slots:
	void StopRequest();
	void StartRequestAsync() { m_UseSyncronous = false; RunRequest(); }
	void StartRequestAsync(const BloombergRequest& a) { SetRequest(a); StartRequestAsync(); }
private:
	qint32 CountInvalids;
	QHash<qint64, QList<qint64> > Groups;
	QString ServerAddress;
	quint16 ServerPort;
	BloombergRequest m_Requests;
	BloombergLP::blpapi::Session* session;
	QList<qint64> RecievedGroups;
	bool m_SessionFinished;
	bool m_UseSyncronous;
	void RunRequest();
	void handleResponseEvent(const BloombergLP::blpapi::Event& event);
	bool processEvent(const BloombergLP::blpapi::Event& event, BloombergLP::blpapi::Session *UseLess);
	void SetGlobalError(BloombergRequest::BbgErrorCodes Err) { SetError(-1i64, -1i64, Err); }
	void SetError(qint64 GroupID, qint64 RequestID, BloombergRequest::BbgErrorCodes Err);
	void Send();
	void DataRecieved(qint64 GroupID, qint64 RequestID, const QString& Value, const QString& Header);
	void DataRecieved(qint64 GroupID, qint64 RequestID);
	void DataRowRecieved(qint64 GroupID, qint64 RequestID, const QStringList& Value, const QStringList& Header);
	void EndOfProcess();
signals:
	void Recieved(qint64);
	void UpdateProgress(qint32);
	void Started();
	void Stopped();
	void Finished();
};
#endif // BloombergWorker_h__
#endif