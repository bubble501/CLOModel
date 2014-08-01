#include "AsyncBloombergWorker.h"
#include "BloombergRequest.h"
void AsyncBloombergWorker::StartRequest() {
	m_Requests.SetErrorCode(BloombergRequest::NoErrors);
	m_Requests.ClearResults();

	if (!m_Requests.IsValidReq()) {
		m_Requests.SetErrorCode(BloombergRequest::InvalidInputs);
		return;
	}
	SessionOptions sessionOptions;
	sessionOptions.setServerHost(ServerAddress.toLatin1().data());
	sessionOptions.setServerPort(ServerPort);
	session = new Session(sessionOptions, Hive);
	if (!session->startAsync()) {
		m_Requests.SetErrorCode(BloombergRequest::SessionError);
		return;
	}
}

AsyncBloombergWorker::AsyncBloombergWorker(QObject* parent) 
	:QObject(parent)
	, AbstractBbgWorker()
	, session(NULL)
{
	Hive = new AsyncEventHandler(this);
	connect(Hive, SIGNAL(DataRecieved(qint64)), this, SIGNAL(DataRecieved(qint64)));
}

