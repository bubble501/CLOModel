#ifndef NO_BLOOMBERG
#include "AsyncBloombergWorker.h"
#include "BloombergRequest.h"
#include "SingleBbgRequest.h"
void AsyncBloombergWorker::StartRequest() {
	if (session) return;
	RecievedGroups.clear();
	m_Requests.SetErrorCode(BloombergRequest::NoErrors);
	m_Requests.ClearResults();

	if (!m_Requests.IsValidReq()) {
		m_Requests.SetErrorCode(BloombergRequest::InvalidInputs);
		return;
	}
	Groups = m_Requests.RequestGroups();
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
	connect(Hive, SIGNAL(DataRecieved(qint64, qint64, QString, QString)), this, SLOT(DataRecievedInHadle(qint64, qint64, QString, QString)));
	connect(Hive, SIGNAL(DataRecieved(qint64, qint64)), this, SLOT(DataRecievedInHadle(qint64, qint64)));
	connect(Hive, SIGNAL(DataRowRecieved(qint64, qint64, QStringList, QStringList)), this, SLOT(DataRowRecievedInHadle(qint64, qint64, QStringList, QStringList)));
	connect(Hive, SIGNAL(ErrorOccurred(qint64, qint64, qint32)), this, SLOT(ErrorOccurredInHadle(qint64, qint64, qint32)));
	connect(Hive, SIGNAL(SessionTerminated()), this, SLOT(SessionTerminated()),Qt::QueuedConnection);
	connect(Hive, SIGNAL(Started()), this, SIGNAL(Started()));
}
void AsyncBloombergWorker::SessionTerminated() {
	if (!session) return;
	delete session;
	session = NULL;
	if (m_Requests.GetErrorCode() | static_cast<qint32>(BloombergRequest::SessionStopped)) emit Stopped();
}

void AsyncBloombergWorker::StopRequest() {
	if (session) {
		m_Requests.SetErrorCode(BloombergRequest::SessionStopped);
		session->stopAsync();
	}
}
void AsyncBloombergWorker::ErrorOccurredInHadle(qint64 GroupID, qint64 RequestID, qint32 ErrorID) {
	if (GroupID == -1i64) {
		m_Requests.SetErrorCode(static_cast<BloombergRequest::BbgErrorCodes>(ErrorID));
		emit UpdateProgress(100);
		emit Finished();
		return session->stopAsync();
	}
	m_Requests.FindRequest(RequestID)->SetErrorCode(static_cast<BloombergRequest::BbgErrorCodes>(ErrorID));
	DataRecievedInHadle(GroupID, RequestID);
}
void AsyncBloombergWorker::DataRecievedInHadle(qint64 GroupID, qint64 RequestID, const QString& Value, const QString& Header) {
	SingleBbgRequest* CurrentResult = m_Requests.FindRequest(RequestID);
	if (CurrentResult){
		CurrentResult->SetValue(Value, Header);
		DataRecievedInHadle(GroupID, RequestID);
	}
}
void AsyncBloombergWorker::DataRowRecievedInHadle(qint64 GroupID, qint64 RequestID, const QStringList& Value, const QStringList& Header) {
	SingleBbgRequest* CurrentResult = m_Requests.FindRequest(RequestID);
	if (CurrentResult) 
		CurrentResult->AddValueRow(Value, Header);
}
AsyncBloombergWorker::~AsyncBloombergWorker() {
	if (session) {
		session->stop();
		delete session;
	}
}
void AsyncBloombergWorker::DataRecievedInHadle(qint64 GroupID, qint64 RequestID) {
	if (RecievedGroups.contains(RequestID)) return;
	RecievedGroups.append(RequestID);
	emit Recieved(RequestID);
	emit UpdateProgress(100 * RecievedGroups.size() / m_Requests.NumRequests());

	/*const SingleBbgRequest* CurrentReq = m_Requests.FindRequest(RequestID);
	if (CurrentReq->HasErrors()) {
		emit DataRecievedString(
			QString("%1 - ").arg(CurrentReq->GetResultID()) +
			CurrentReq->GetSecurity() + " - " + CurrentReq->GetField() + QString(": Error %1").arg(CurrentReq->GetError())
			);
	}
	else{
		if (CurrentReq->GetValue().HasTable()) 
			emit DataRecievedString(
			QString("%1 - ").arg(CurrentReq->GetResultID()) +
			CurrentReq->GetSecurity() + " - " + CurrentReq->GetField() + QString(": Table %1 Rows, %2 Columns").arg(CurrentReq->GetValue().GetNumRows()).arg(CurrentReq->GetValue().GetNumCols())
			);
		else
			emit DataRecievedString(
			QString("%1 - ").arg(CurrentReq->GetResultID()) +
			CurrentReq->GetSecurity() + " - " + CurrentReq->GetField() + ": " + CurrentReq->GetValue().GetString()
			);
	}*/

	if (RecievedGroups.size() == m_Requests.NumRequests()) {
		emit Finished();
		session->stopAsync();
	}
}

#endif
