#ifndef NO_BLOOMBERG
#include "QBbgWorker.h"
#include <blpapi_correlationid.h>
#include <blpapi_event.h>
#include <blpapi_message.h>
#include <blpapi_request.h>
#include <blpapi_session.h>
#include "QSingleBbgRequest.h"
using namespace BloombergLP::blpapi;
using namespace QBloombergLib;
void QBbgWorker::SetServerAddress(const QString& val) {
	ServerAddress = val.trimmed();
}
QBbgWorker::QBbgWorker(QObject* parent)
	: ServerAddress("localhost")
	, ServerPort(8194)
	, session(NULL)
	, m_SessionFinished(false)
	, m_UseSyncronous(false)
	, QObject(parent)
{}

void QBbgWorker::RunRequest() {
	if (session) return;
	m_SessionFinished = false;
	SetGlobalError(QSingleBbgResult::NoErrors);
	for (QMap<qint64, QSingleBbgResult* >::iterator i = m_Results.begin(); i != m_Results.end(); i++)
		delete (i.value());
	m_Results.clear();
	if (!m_Requests.IsValidReq()) {
		SetGlobalError(QSingleBbgResult::InvalidInputs);
		return;
	}
	m_Requests.RequestGroups(Groups);
	SessionOptions sessionOptions;
	sessionOptions.setServerHost(ServerAddress.toLatin1().data());
	sessionOptions.setServerPort(ServerPort);
	sessionOptions.setMaxPendingRequests(INT_MAX - 2);
	if (m_UseSyncronous) {
		session= new Session(sessionOptions);
		if (!session->start()) {
			SetGlobalError(QSingleBbgResult::SessionError);
			return;
		}
		if (!session->openService("//blp/refdata")) {
			SetGlobalError(QSingleBbgResult::ServiceError);
			return;
		}
		Send();
	}
	else {
		session= new Session(sessionOptions, this);
		if (!session->startAsync()) {
			SetGlobalError(QSingleBbgResult::SessionError);
			return;
		}
	}
}

void QBbgWorker::Send() {
	QList<QString> UsedSecur;
	QList<QString> UsedField;
	Service refDataSvc = session->getService("//blp/refdata");
	for (QHash<qint64, QList<qint64>* >::const_iterator ReqIter = Groups.constBegin(); ReqIter != Groups.constEnd(); ReqIter++) {
		CorrelationId requestId(ReqIter.key());
		Request request = refDataSvc.createRequest("ReferenceDataRequest");
		const QList<qint64>* CurrentList = ReqIter.value();
		UsedSecur.clear();
		UsedField.clear();
		for (QList<qint64>::const_iterator GroupIter = CurrentList->constBegin(); GroupIter != CurrentList->constEnd(); GroupIter++) {
			const QSingleBbgRequest* CurrentSingle = m_Requests.FindRequest(*GroupIter);
			if (!CurrentSingle) continue;
			if (!CurrentSingle->IsValidReq()) {
				if (SetError(ReqIter.key(), *GroupIter, QSingleBbgResult::InvalidInputs))
					return EndOfProcess(); //All Invalid Inputs
			}
			if (!UsedSecur.contains(CurrentSingle->GetSecurity())) {
				request.append("securities", CurrentSingle->GetFullSecurity().toLatin1().data());
				UsedSecur.append(CurrentSingle->GetSecurity());
			}
			if (!UsedField.contains(CurrentSingle->GetField())) {
				request.append("fields", CurrentSingle->GetField().toLatin1().data());
				UsedField.append(CurrentSingle->GetField());
			}
			if (GroupIter == CurrentList->begin()) {
				for (QHash<QString, QString>::const_iterator OvrIter = CurrentSingle->GetOverrides().constBegin(); OvrIter != CurrentSingle->GetOverrides().constEnd(); OvrIter++) {
					Element CurrentOverrides = request.getElement("overrides").appendElement();
					CurrentOverrides.setElement("fieldId", OvrIter.value().toLatin1().data());
					CurrentOverrides.setElement("value", OvrIter.value().toLatin1().data());
				}
			}
		}
		session->sendRequest(request, requestId);
		bool ContinueToLoop = m_UseSyncronous;
		while (ContinueToLoop) {
			Event event = session->nextEvent();
			if (event.eventType() == Event::RESPONSE || event.eventType() == Event::PARTIAL_RESPONSE) {
				ContinueToLoop = event.eventType() != Event::RESPONSE;
				handleResponseEvent(event);
			}
		}
	}
	bool ContinueToLoop = m_UseSyncronous;
	while (ContinueToLoop) {
		Event event = session->nextEvent();
		if (event.eventType() == Event::SESSION_STATUS) {
			MessageIterator iter(event);
			while (iter.next()) {
				Message message = iter.message();
				if (QString(message.messageType().string()) == "SessionTerminated") {
					ContinueToLoop = false;
				}
			}
		}
	}
	if (m_UseSyncronous) return EndOfProcess();
}

bool QBbgWorker::processEvent(const BloombergLP::blpapi::Event& event, BloombergLP::blpapi::Session *CurrentSession) {
	switch (event.eventType()) {
	case Event::SESSION_STATUS: {
		MessageIterator iter(event);
		while (iter.next()) {
			Message message = iter.message();
			QString MessageType = message.messageType().string();
			if (MessageType == "SessionStarted") {
				CurrentSession->openServiceAsync("//blp/refdata", CorrelationId(-1i64));
				emit QStarted();
			}
			else if (MessageType == "SessionConnectionUp") continue;
			else if (MessageType == "SessionConnectionDown") continue;
			else if (MessageType == "SessionTerminated") {
				EndOfProcess();
			}
			else /*if (MessageType == "SessionStartupFailure")*/ {
				SetGlobalError(QSingleBbgResult::SessionError);
			}
		}
		break;
	}
	case Event::SERVICE_STATUS: {
		MessageIterator iter(event);
		while (iter.next()) {
			Message message = iter.message();
			if (message.correlationId().asInteger() == -1 && QString(message.messageType().string()) == "ServiceOpened") {
				Send();
			}
			else {
				SetGlobalError(QSingleBbgResult::ServiceError);
			}
		}
		break;
	}
	case Event::PARTIAL_RESPONSE:
	case Event::RESPONSE:
		handleResponseEvent(event);
		break;
	default:
		SetGlobalError(QSingleBbgResult::UnknownError);
	}
	return true;
}

void QBbgWorker::handleResponseEvent(const BloombergLP::blpapi::Event& event) {
	MessageIterator iter(event);
	qint32 NumVals, NumFieldExep;
	while (iter.next()) {
		Message message = iter.message();
		const QList<qint64>* CurrentGroup = Groups.value(message.correlationId().asInteger(), NULL);
		if (CurrentGroup) {
			if (message.hasElement("responseError")) {
				for (QList<qint64>::const_iterator SingleReq = CurrentGroup->constBegin(); SingleReq != CurrentGroup->constEnd(); SingleReq++) {
					if (SetError(message.correlationId().asInteger(), *SingleReq, QSingleBbgResult::ResponseError))return;
				}
				return;
			}
			if (QString(message.messageType().string()) == "ReferenceDataResponse") {
				NumVals = message.getElement("securityData").numValues();
				for (qint32 i = 0; i < NumVals; i++) {
					NumFieldExep = message.getElement("securityData").getValueAsElement(i).getElement("fieldExceptions").numValues();
					for (qint32 j = 0; j < NumFieldExep; j++) {
						for (QList<qint64>::const_iterator SingleReq = CurrentGroup->constBegin(); SingleReq != CurrentGroup->constEnd(); SingleReq++) {
							const QSingleBbgRequest* FoundRequ = m_Requests.FindRequest(*SingleReq);
							QString CurrentSecurity = message.getElement("securityData").getValueAsElement(i).getElementAsString("security");
							QString CurrentField = message.getElement("securityData").getValueAsElement(i).getElement("fieldExceptions").getValueAsElement(j).getElementAsString("fieldId");
							if (
								FoundRequ->GetFullSecurity() == CurrentSecurity
								&& FoundRequ->GetField() == CurrentField
								) {
								if (SetError(message.correlationId().asInteger(), *SingleReq, QSingleBbgResult::FieldError))return;
							}
						}
					}
					if (message.getElement("securityData").getValueAsElement(i).hasElement("securityError")) {
						QString CurrentSecurity = message.getElement("securityData").getValueAsElement(i).getElementAsString("security");
						for (QList<qint64>::const_iterator SingleReq = CurrentGroup->constBegin(); SingleReq != CurrentGroup->constEnd(); SingleReq++) {
							const QSingleBbgRequest* FoundRequ = m_Requests.FindRequest(*SingleReq);
							if ((FoundRequ->GetFullSecurity()) == CurrentSecurity) {
								if (SetError(message.correlationId().asInteger(), *SingleReq, QSingleBbgResult::SecurityError))return;
							}
						}
					}
					else {
						QString CurrentSecurity = message.getElement("securityData").getValueAsElement(i).getElementAsString("security");
						for (QList<qint64>::const_iterator SingleReq = CurrentGroup->constBegin(); SingleReq != CurrentGroup->constEnd(); SingleReq++) {
							const QSingleBbgRequest* FoundRequ = m_Requests.FindRequest(*SingleReq);
							if ((FoundRequ->GetFullSecurity()) == CurrentSecurity) {
								if (message.getElement("securityData").getValueAsElement(i).getElement("fieldData").numElements() != 0) {
									if (message.getElement("securityData").getValueAsElement(i).getElement("fieldData").hasElement(FoundRequ->GetField().toLatin1().data())) {
										if (!message.getElement("securityData").getValueAsElement(i).getElement("fieldData").getElement(FoundRequ->GetField().toLatin1().data()).isArray()) {
											if (DataRecieved(message.correlationId().asInteger(), *SingleReq
												, message.getElement("securityData").getValueAsElement(i).getElement("fieldData").getElementAsString(FoundRequ->GetField().toLatin1().data())
												, FoundRequ->GetField()
												)) return;
										}
										else {
											QList<QString> CurrentRow, CurrentHead;
											const qint32 NumResults =
												message.getElement("securityData").getValueAsElement(i).getElement("fieldData").getElement(FoundRequ->GetField().toLatin1().data()).numValues();
											const qint32 NumCols =
												message.getElement("securityData").getValueAsElement(i).getElement("fieldData").getElement(FoundRequ->GetField().toLatin1().data()).getValueAsElement(0).numElements();
											for (qint32 TableIter = 0; TableIter < NumResults; TableIter++) {
												CurrentRow.clear(); CurrentHead.clear();
												for (qint32 ColIter = 0; ColIter < NumCols; ColIter++) {
													CurrentRow.append(message.getElement("securityData").getValueAsElement(i).getElement("fieldData").getElement(FoundRequ->GetField().toLatin1().data()).getValueAsElement(TableIter).getElement(ColIter).getValueAsString());
													CurrentHead.append(message.getElement("securityData").getValueAsElement(i).getElement("fieldData").getElement(FoundRequ->GetField().toLatin1().data()).getValueAsElement(TableIter).getElement(ColIter).name().string());
												}
												DataRowRecieved(message.correlationId().asInteger(), *SingleReq, CurrentRow, CurrentHead);
											}
											if (DataRecieved(message.correlationId().asInteger(), *SingleReq)) return;
										}
									}
									else {
										if (SetError(message.correlationId().asInteger(), *SingleReq, QSingleBbgResult::NoData))return;
									}
								}
								else {
									if (SetError(message.correlationId().asInteger(), *SingleReq, QSingleBbgResult::NoData)) return;
								}
							}
						}
					}
				}
			}
		}
	}
}

bool QBbgWorker::SetError(qint64 GroupID, qint64 RequestID, QSingleBbgResult::BbgErrorCodes Err) {
	if (GroupID < 0) {
		m_Requests.SetErrorCode(Err);
		if (Err != QSingleBbgResult::NoErrors) {
			m_SessionFinished = true;
			if (m_UseSyncronous)
				if (Err != QSingleBbgResult::SessionError) session->stop();
				else {
					emit QUpdateProgress(100);
					emit QFinished();	
					if (Err != QSingleBbgResult::SessionError) session->stopAsync();
				}
				return true;
		}
		return false;
	}
	else {
		if (m_Results.contains(RequestID)) return false;
		QSingleBbgResult* TempRes=new QSingleBbgResult();
		TempRes->SetErrorCode(Err);
		m_Results.insert(RequestID, TempRes);
		return DataRecieved(GroupID, RequestID);
	}
}

bool QBbgWorker::DataRecieved(qint64 GroupID, qint64 RequestID, const QString& Value, const QString& Header) {
	if (m_Results.contains(RequestID)) return false;
	QSingleBbgResult* TempRes=new QSingleBbgResult();
	TempRes->SetValue(Value, Header);
	m_Results.insert(RequestID, TempRes);
	return DataRecieved(GroupID, RequestID);
}

bool QBbgWorker::DataRecieved(qint64 GroupID, qint64 RequestID) {
	if (!m_UseSyncronous) {
		emit QRecieved(RequestID);
		emit QUpdateProgress(100 * m_Results.size() / m_Requests.NumRequests());
	}
	if (m_Results.size() == m_Requests.NumRequests()) {
		m_SessionFinished = true;
		if (m_UseSyncronous) {
			session->stop();
		}
		else {
			emit QFinished();
			session->stopAsync();
		}
		return true;
	}
	return false;
}

void QBbgWorker::DataRowRecieved(qint64 GroupID, qint64 RequestID, const QList<QString>&  Value, const QList<QString>&  Header) {
	QMap<qint64, QSingleBbgResult* >::iterator TempIter = m_Results.find(RequestID);
	if (TempIter == m_Results.end()) {
		QSingleBbgResult* TempRes=new QSingleBbgResult();
		TempRes->AddValueRow(Value, Header);
		m_Results.insert(RequestID, TempRes);
	}
	else {
		TempIter.value()->AddValueRow(Value, Header);
	}
	DataRecieved(GroupID, RequestID);
}

void QBbgWorker::EndOfProcess() {
	if (!session) return;
	m_SessionFinished = false;
	delete session;
	session=NULL;
	if (m_UseSyncronous && (m_Requests.GetErrorCode() & static_cast<qint32>(QSingleBbgResult::SessionStopped))) {
		emit QStopped();
	}

}
void QBbgWorker::StopRequest() {
	if (m_SessionFinished || m_UseSyncronous) return;
	if (session) {
		m_SessionFinished = true;
		m_Requests.SetErrorCode(QSingleBbgResult::SessionStopped);
		session->stopAsync();
	}
}
QBbgWorker::~QBbgWorker() {
	if (session) {
		session->stop();
		delete session;
	}
	while (!m_Results.empty()) {
		delete (m_Results.begin().value());
		m_Results.erase(m_Results.begin());
	}
	while (!Groups.empty()) {
		delete (Groups.begin().value());
		Groups.erase(Groups.begin());
	}
}
const QString& QBbgWorker::GetServerAddress() const {
	return ServerAddress;
}

unsigned short QBbgWorker::GetServerPort() const {
	return ServerPort;
}

void QBbgWorker::SetServerPort(unsigned short val) {
	ServerPort = val;
}

void QBbgWorker::SetRequest(const QBbgRequest& a) {
	if (!session) m_Requests = a;
}

void QBbgWorker::StartRequestSync() {
	m_UseSyncronous = true;
	RunRequest();
}

void QBbgWorker::StartRequestSync(const QBbgRequest& a) {
	SetRequest(a);
	StartRequestSync();
}

const QSingleBbgResult* QBbgWorker::GetResult(qint64 ID) const {
	return m_Results.value(ID, NULL);
}

const QBbgRequest& QBbgWorker::GetRequest() const {
	return m_Requests;
}

QBbgResultsIterator QBbgWorker::GetResultIterator() {
	return QBbgResultsIterator(m_Results);
}

bool QBbgWorker::isRunning() const {
	return session && !m_SessionFinished;
}

void QBbgWorker::StartRequestAsync() {
	m_UseSyncronous = false; RunRequest();
}

void QBbgWorker::StartRequestAsync(const QBbgRequest& a) {
	SetRequest(a); StartRequestAsync();
}

void QBbgWorker::SetGlobalError(QSingleBbgResult::BbgErrorCodes Err) {
	SetError(-1i64, -1i64, Err);
}

#endif