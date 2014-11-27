#ifndef NO_BLOOMBERG
#ifndef BloombergWorker_h__
#define BloombergWorker_h__
#include "QProjectGlobals.h"
#include "QSingleBbgResult.h"
#include "QBbgResultsIterator.h"
#include "QBbgRequest.h"
#include <QObject>
#include <QMap>
#include <QScopedPointer>
#ifdef QBBG_PRIVATE
#include <blpapi_session.h>
#endif
namespace QBloombergLib {
	class BLOOMBERG_EXPORT QBbgWorker	: public QObject
#ifdef QBBG_PRIVATE
		, private BloombergLP::blpapi::EventHandler
#endif
	{
		Q_OBJECT
	public:
		QBbgWorker(QObject* parent = 0	);
		virtual ~QBbgWorker();
		const QString& GetServerAddress() const;
		void SetServerAddress(const QString& val);
		unsigned short GetServerPort() const;
		void SetServerPort(unsigned short val);
		void SetRequest(const QBbgRequest& a);
		void StartRequestSync();
		void StartRequestSync(const QBbgRequest& a);
		const QSingleBbgResult* GetResult(qint64 ID) const;
		const QBbgRequest& GetRequest() const;
		bool isRunning() const;
	public slots:
		void StopRequest();
		void StartRequestAsync();
		void StartRequestAsync(const QBbgRequest& a);
		QBbgResultsIterator GetResultIterator();
#ifdef QBBG_PRIVATE
	private:
		QBbgWorker(const QBbgWorker &) {}
		QHash<qint64, QList<qint64>* > Groups;
		QString ServerAddress;
		unsigned short ServerPort;
		QBbgRequest m_Requests;
		QMap<qint64, QSingleBbgResult* > m_Results;
		QScopedPointer<BloombergLP::blpapi::Session> session;
		bool m_SessionFinished;
		bool m_UseSyncronous;
		void RunRequest();
		void handleResponseEvent(const BloombergLP::blpapi::Event& event);
		bool processEvent(const BloombergLP::blpapi::Event& event, BloombergLP::blpapi::Session *CurrentSession);
		void SetGlobalError(QSingleBbgResult::BbgErrorCodes Err);
		bool SetError(qint64 GroupID, qint64 RequestID, QSingleBbgResult::BbgErrorCodes Err);
		void Send();
		bool DataRecieved(qint64 GroupID, qint64 RequestID, const QString& Value, const QString& Header);
		bool DataRecieved(qint64 GroupID, qint64 RequestID);
		void DataRowRecieved(qint64 GroupID, qint64 RequestID, const QList<QString>&  Value, const QList<QString>&  Header);
		void EndOfProcess();
#endif
	signals :
		void QRecieved(qint64);
		void QUpdateProgress(qint32);
		void QStarted();
		void QStopped();
		void QFinished();
	};
}

#endif // BloombergWorker_h__
#endif