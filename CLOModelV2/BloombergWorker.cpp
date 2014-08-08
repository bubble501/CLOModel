#ifndef NO_BLOOMBERG
#include "BloombergWorker.h"
#include <blpapi_correlationid.h>
#include <blpapi_event.h>
#include <blpapi_message.h>
#include <blpapi_request.h>
#include <blpapi_session.h>
#include "SingleBbgRequest.h"
#include <QStringList>
using namespace BloombergLP::blpapi;
BloombergWorker::BloombergWorker(QObject* parent) 
	:QObject(parent)
	, ServerAddress("localhost")
	, ServerPort(8194) 
	, session(NULL)
	, m_SessionFinished(false)
	, m_UseSyncronous(false)
	, CountInvalids(0)
{

}

void BloombergWorker::RunRequest() {
	if (session) return;
	m_SessionFinished = false;
	RecievedGroups.clear();
	SetGlobalError(BloombergRequest::NoErrors);
	m_Requests.ClearResults();

	if (!m_Requests.IsValidReq()) {
		SetGlobalError(BloombergRequest::InvalidInputs);
		return;
	}
	Groups = m_Requests.RequestGroups();
	SessionOptions sessionOptions;
	sessionOptions.setServerHost(ServerAddress.toLatin1().data());
	sessionOptions.setServerPort(ServerPort);
	if (m_UseSyncronous) {
		session = new Session(sessionOptions);
		if (!session->start()) {
			SetGlobalError(BloombergRequest::SessionError);
			return;
		}
		if (!session->openService("//blp/refdata")) {
			SetGlobalError(BloombergRequest::ServiceError);
			return;
		}
		Send();
	}
	else {
		session = new Session(sessionOptions, this);
		if (!session->startAsync()) {
			SetGlobalError(BloombergRequest::SessionError);
			return;
		}
	}
}

void BloombergWorker::Send() {
	Groups = m_Requests.RequestGroups();
	CountInvalids = 0;
	QList<QString> UsedSecur;
	QList<QString> UsedField;
	Service refDataSvc = session->getService("//blp/refdata");
	for (QHash<qint64, QList<qint64> >::const_iterator ReqIter = Groups.constBegin(); ReqIter != Groups.constEnd(); ReqIter++) {
		CorrelationId requestId(ReqIter.key());
		Request request = refDataSvc.createRequest("ReferenceDataRequest");
		QList<qint64> CurrentList = ReqIter.value();
		UsedSecur.clear();
		UsedField.clear();
		for (QList<qint64>::const_iterator GroupIter = CurrentList.constBegin(); GroupIter != CurrentList.constEnd(); GroupIter++) {
			const SingleBbgRequest* CurrentSingle =m_Requests.FindRequest(*GroupIter);
			if (!CurrentSingle) continue;
			if (!CurrentSingle->IsValidReq()) {
				CountInvalids++; continue;
			}
			if (!UsedSecur.contains(CurrentSingle->GetSecurity())) {
				request.append("securities", (CurrentSingle->GetFullSecurity()).toLatin1().data());
				UsedSecur.append(CurrentSingle->GetSecurity());
			}
			if (!UsedField.contains(CurrentSingle->GetField())) {
				request.append("fields", CurrentSingle->GetField().toLatin1().data());
				UsedField.append(CurrentSingle->GetField());
			}
			if (GroupIter == CurrentList.constBegin()) {
				for (QHash<QString, QString>::const_iterator OvrIter = CurrentSingle->GetOverrides().constBegin(); OvrIter != CurrentSingle->GetOverrides().constEnd(); OvrIter++) {
					Element CurrentOverrides = request.getElement("overrides").appendElement();
					CurrentOverrides.setElement("fieldId", OvrIter.key().toLatin1().data());
					CurrentOverrides.setElement("value", OvrIter.value().toLatin1().data());
				}
			}
		}
		session->sendRequest(request, requestId);
		bool continueToLoop = m_UseSyncronous;
		while (continueToLoop) {
			Event event = session->nextEvent();
			if (event.eventType() == Event::RESPONSE || event.eventType() == Event::PARTIAL_RESPONSE) {
				continueToLoop = (event.eventType() != Event::RESPONSE);
				handleResponseEvent(event);
			}
		}
	}
}

bool BloombergWorker::processEvent(const BloombergLP::blpapi::Event& event, BloombergLP::blpapi::Session *UseLess) {
	Q_UNUSED(UseLess);
	switch (event.eventType()) {
	case Event::SESSION_STATUS: {
		MessageIterator iter(event);
		while (iter.next()) {
			Message message = iter.message();
			QString Testy = message.messageType().string();
			if (QString(message.messageType().string()) == "SessionStarted") {
				session->openServiceAsync("//blp/refdata", CorrelationId(-1i64));
				emit Started();
			}
			else if (QString(message.messageType().string()) == "SessionConnectionUp") continue;
			else if (QString(message.messageType().string()) == "SessionConnectionDown") continue;
			else if (QString(message.messageType().string()) == "SessionTerminated") emit EndOfProcess();
			else /*if (QString(message.messageType().string()) == "SessionStartupFailure")*/ {
				SetGlobalError(BloombergRequest::SessionError);
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
				SetGlobalError(BloombergRequest::ServiceError);
			}
		}
		break;
	}
	case Event::PARTIAL_RESPONSE:
	case Event::RESPONSE:
		handleResponseEvent(event);
		break;
	default:
		SetGlobalError(BloombergRequest::UnknownError);
	}
	return true;
}

void BloombergWorker::handleResponseEvent(const BloombergLP::blpapi::Event& event) {
	MessageIterator iter(event);
	int NumVals, NumFieldExep;
	while (iter.next()) {
		Message message = iter.message();
		if (Groups.contains(message.correlationId().asInteger())) {
			QList<qint64> CurrentGroup = Groups.value(message.correlationId().asInteger());
			if (message.hasElement("responseError")) {
				foreach(qint64 SingleReq, CurrentGroup) {
					SetError(message.correlationId().asInteger(), SingleReq, BloombergRequest::ResponseError);
				}
				return;
			}
			if (QString(message.messageType().string()) == "ReferenceDataResponse") {
				NumVals = message.getElement("securityData").numValues();
				for (int i = 0; i < NumVals; i++) {
					NumFieldExep = message.getElement("securityData").getValueAsElement(i).getElement("fieldExceptions").numValues();
					for (int j = 0; j < NumFieldExep; j++) {
						foreach(qint64 SingleReq, CurrentGroup) {
							const SingleBbgRequest* FoundRequ = m_Requests.FindRequest(SingleReq);
							QString CurrentSecurity = message.getElement("securityData").getValueAsElement(i).getElementAsString("security");
							QString CurrentField = message.getElement("securityData").getValueAsElement(i).getElement("fieldExceptions").getValueAsElement(j).getElementAsString("fieldId");
							if (
								FoundRequ->GetFullSecurity() == CurrentSecurity
								&& FoundRequ->GetField() == CurrentField
								) {
								SetError(message.correlationId().asInteger(), SingleReq, BloombergRequest::FieldError);
							}
						}
					}
					if (message.getElement("securityData").getValueAsElement(i).hasElement("securityError")) {
						QString CurrentSecurity = message.getElement("securityData").getValueAsElement(i).getElementAsString("security");
						foreach(qint64 SingleReq, CurrentGroup) {
							const SingleBbgRequest* FoundRequ = m_Requests.FindRequest(SingleReq);
							if ((FoundRequ->GetFullSecurity()) == CurrentSecurity) {
								SetError(message.correlationId().asInteger(), SingleReq, BloombergRequest::SecurityError);
							}
						}
					}
					else {
						QString CurrentSecurity = message.getElement("securityData").getValueAsElement(i).getElementAsString("security");
						foreach(qint64 SingleReq, CurrentGroup) {
							const SingleBbgRequest* FoundRequ = m_Requests.FindRequest(SingleReq);
							if ((FoundRequ->GetFullSecurity()) == CurrentSecurity) {
								if (message.getElement("securityData").getValueAsElement(i).getElement("fieldData").numElements() != 0) {
									if (message.getElement("securityData").getValueAsElement(i).getElement("fieldData").hasElement(FoundRequ->GetField().toLatin1().data())) {
										if (!message.getElement("securityData").getValueAsElement(i).getElement("fieldData").getElement(FoundRequ->GetField().toLatin1().data()).isArray()) {
											DataRecieved(message.correlationId().asInteger(), SingleReq
												, message.getElement("securityData").getValueAsElement(i).getElement("fieldData").getElementAsString(FoundRequ->GetField().toLatin1().data())
												, FoundRequ->GetField()
												);
										}
										else {
											QStringList CurrentRow, CurrentHead;
											const int NumResults =
												message.getElement("securityData").getValueAsElement(i).getElement("fieldData").getElement(FoundRequ->GetField().toLatin1().data()).numValues();
											const int NumCols =
												message.getElement("securityData").getValueAsElement(i).getElement("fieldData").getElement(FoundRequ->GetField().toLatin1().data()).getValueAsElement(0).numElements();
											for (int TableIter = 0; TableIter < NumResults; TableIter++) {
												CurrentRow.clear(); CurrentHead.clear();
												for (int ColIter = 0; ColIter < NumCols; ColIter++) {
													CurrentRow << message.getElement("securityData").getValueAsElement(i).getElement("fieldData").getElement(FoundRequ->GetField().toLatin1().data()).getValueAsElement(TableIter).getElement(ColIter).getValueAsString();
													CurrentHead << message.getElement("securityData").getValueAsElement(i).getElement("fieldData").getElement(FoundRequ->GetField().toLatin1().data()).getValueAsElement(TableIter).getElement(ColIter).name().string();
												}
												DataRowRecieved(message.correlationId().asInteger(), SingleReq, CurrentRow, CurrentHead);
											}
											DataRecieved(message.correlationId().asInteger(), SingleReq);
										}
									}
									else {
										SetError(message.correlationId().asInteger(), SingleReq, BloombergRequest::NoData);
									}
								}
								else {
									SetError(message.correlationId().asInteger(), SingleReq, BloombergRequest::NoData);
								}
							}
						}
					}
				}
			}
		}
	}
}

void BloombergWorker::SetError(qint64 GroupID, qint64 RequestID, BloombergRequest::BbgErrorCodes Err) {
	if (GroupID < 0) {
		m_Requests.SetErrorCode(Err);
		if (Err!=BloombergRequest::NoErrors){
			m_SessionFinished = true;
			if (m_UseSyncronous)
				EndOfProcess();
			else {
				emit UpdateProgress(100);
				emit Finished();
				session->stopAsync();
			}
		}
	}
	else {
		m_Requests.SetResultError(RequestID, Err);
		DataRecieved(GroupID, RequestID);
	}
}

void BloombergWorker::DataRecieved(qint64 GroupID, qint64 RequestID, const QString& Value, const QString& Header) {
	m_Requests.SetResult(RequestID,Value, Header);
	DataRecieved(GroupID, RequestID);
}

void BloombergWorker::DataRecieved(qint64 GroupID, qint64 RequestID) {
	if (RecievedGroups.contains(RequestID)) return;
	RecievedGroups.append(RequestID);
	if (!m_UseSyncronous) {
		emit Recieved(RequestID);
		emit UpdateProgress(100 * RecievedGroups.size() / m_Requests.NumRequests());
	}
	if (RecievedGroups.size() == m_Requests.NumRequests() - CountInvalids) {
		m_SessionFinished = true;
		if (m_UseSyncronous) {
			session->stop();
			EndOfProcess();
		}
		else {
			emit Finished();
			session->stopAsync();
		}
	}
}

void BloombergWorker::DataRowRecieved(qint64 GroupID, qint64 RequestID, const QStringList& Value, const QStringList& Header) {
	m_Requests.SetResultRow(RequestID, Value, Header);
}

void BloombergWorker::EndOfProcess() {
	if (!session) return;
	delete session;
	session = NULL;
	m_SessionFinished = false;
	if (m_UseSyncronous && (m_Requests.GetErrorCode() & static_cast<qint32>(BloombergRequest::SessionStopped))) emit Stopped();
}
void BloombergWorker::StopRequest() {
	if (m_SessionFinished || m_UseSyncronous) return;
	if (session) {
		m_SessionFinished = true;
		m_Requests.SetErrorCode(BloombergRequest::SessionStopped);
		session->stopAsync();
	}
}
BloombergWorker::~BloombergWorker() {
	if (session) {
		session->stop();
		delete session;
	}
}
#endif