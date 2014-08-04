#ifndef NO_BLOOMBERG
#include "AsyncEventHandler.h"
#include "AsyncBloombergWorker.h"
#include "SingleBbgRequest.h"
bool AsyncEventHandler::processEvent(const Event& event, Session *session) {
	AsyncBloombergWorker* AdjParent = dynamic_cast<AsyncBloombergWorker*>(parent());
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
			else if (QString(message.messageType().string()) == "SessionTerminated") emit SessionTerminated();
			else /*if (QString(message.messageType().string()) == "SessionStartupFailure")*/ {
				AdjParent->m_Requests.SetErrorCode(BloombergRequest::SessionError);
				emit ErrorOccurred(-1i64, -1i64, BloombergRequest::SessionError);
			}
		}
		break;
	}
	case Event::SERVICE_STATUS: {
		
		MessageIterator iter(event);
		while (iter.next()) {
			Message message = iter.message();
			if (message.correlationId().asInteger() == -1 && QString(message.messageType().string()) == "ServiceOpened") {
				AdjParent->Groups = AdjParent->m_Requests.RequestGroups();
				QList<QString> UsedSecur;
				QList<QString> UsedField;
				Service refDataSvc = session->getService("//blp/refdata");
				for (QHash<qint64, QList<qint64> >::const_iterator ReqIter = AdjParent->Groups.constBegin(); ReqIter != AdjParent->Groups.constEnd(); ReqIter++) {
					CorrelationId requestId(ReqIter.key());
					Request request = refDataSvc.createRequest("ReferenceDataRequest");
					QList<qint64> CurrentList = ReqIter.value();
					UsedSecur.clear();
					UsedField.clear();
					for (QList<qint64>::const_iterator GroupIter = CurrentList.constBegin(); GroupIter != CurrentList.constEnd(); GroupIter++) {
						const SingleBbgRequest* CurrentSingle = AdjParent->m_Requests.FindRequest(*GroupIter);
						if (!UsedSecur.contains(CurrentSingle->GetSecurity())) {
							request.append("securities", (CurrentSingle->GetFullSecurity()).toLatin1().data());
							UsedSecur.append(CurrentSingle->GetSecurity());
						}
						if (!UsedField.contains(CurrentSingle->GetField())) {
							request.append("fields", CurrentSingle->GetField().toLatin1().data());
							UsedField.append(CurrentSingle->GetField());
						}
						if (GroupIter == CurrentList.constBegin()) {
							for (QMap<QString, QString>::const_iterator OvrIter = CurrentSingle->GetOverrides().constBegin(); OvrIter != CurrentSingle->GetOverrides().constEnd(); OvrIter++) {
								Element CurrentOverrides = request.getElement("overrides").appendElement();
								CurrentOverrides.setElement("fieldId", OvrIter.key().toLatin1().data());
								CurrentOverrides.setElement("value", OvrIter.value().toLatin1().data());
							}
						}
					}
					session->sendRequest(request, requestId);
				}
			}
			else {
				emit ErrorOccurred(-1i64, -1i64, BloombergRequest::ServiceError);
			}
		}
		break;
	}
	case Event::PARTIAL_RESPONSE:
	case Event::RESPONSE:
		handleResponseEvent(event);
		break;
	default:
		emit ErrorOccurred(-1i64, -1i64, BloombergRequest::UnknownError);
	}
	return true;
}

AsyncEventHandler::AsyncEventHandler(AsyncBloombergWorker* parent)
	:QObject(parent)
{}

void AsyncEventHandler::handleResponseEvent(const Event& event) {
	MessageIterator iter(event);
	int NumVals, NumFieldExep;
	const AsyncBloombergWorker* AdjParent = dynamic_cast<AsyncBloombergWorker*>(parent());
	while (iter.next()) {
		Message message = iter.message();
		if (AdjParent->Groups.contains(message.correlationId().asInteger())) {
			QList<qint64> CurrentGroup = AdjParent->Groups.value(message.correlationId().asInteger());
			if (message.hasElement("responseError")) {
				foreach(qint64 SingleReq, CurrentGroup) {
					emit ErrorOccurred(message.correlationId().asInteger(), SingleReq, BloombergRequest::ResponseError);
				}
				return;
			}
			if (QString(message.messageType().string()) == "ReferenceDataResponse") {
				NumVals = message.getElement("securityData").numValues();
				for (int i = 0; i < NumVals; i++) {
					NumFieldExep = message.getElement("securityData").getValueAsElement(i).getElement("fieldExceptions").numValues();
					for (int j = 0; j < NumFieldExep; j++) {
						foreach(qint64 SingleReq, CurrentGroup) {
							const SingleBbgRequest* FoundRequ = AdjParent->m_Requests.FindRequest(SingleReq);
							QString CurrentSecurity = message.getElement("securityData").getValueAsElement(i).getElementAsString("security");
							QString CurrentField = message.getElement("securityData").getValueAsElement(i).getElement("fieldExceptions").getValueAsElement(j).getElementAsString("fieldId");
							if (
								FoundRequ->GetFullSecurity() == CurrentSecurity
								&& FoundRequ->GetField() == CurrentField
							) {
								emit ErrorOccurred(message.correlationId().asInteger(), SingleReq, BloombergRequest::FieldError);
							}
						}
					}
					if (message.getElement("securityData").getValueAsElement(i).hasElement("securityError")) {
						QString CurrentSecurity = message.getElement("securityData").getValueAsElement(i).getElementAsString("security");
						foreach(qint64 SingleReq, CurrentGroup) {
							const SingleBbgRequest* FoundRequ = AdjParent->m_Requests.FindRequest(SingleReq);
							if ((FoundRequ->GetFullSecurity()) == CurrentSecurity) {
								emit ErrorOccurred(message.correlationId().asInteger(), SingleReq, BloombergRequest::SecurityError);
							}
						}
					}
					else {
						QString CurrentSecurity = message.getElement("securityData").getValueAsElement(i).getElementAsString("security");
						foreach(qint64 SingleReq, CurrentGroup) {
							const SingleBbgRequest* FoundRequ = AdjParent->m_Requests.FindRequest(SingleReq);
							if ((FoundRequ->GetFullSecurity()) == CurrentSecurity) {
								if (!message.getElement("securityData").getValueAsElement(i).getElement("fieldData").isNull()) {
									if (message.getElement("securityData").getValueAsElement(i).getElement("fieldData").hasElement(FoundRequ->GetField().toLatin1().data())) {
										if (!message.getElement("securityData").getValueAsElement(i).getElement("fieldData").getElement(FoundRequ->GetField().toLatin1().data()).isArray()) {
											emit DataRecieved(message.correlationId().asInteger(), SingleReq
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
												emit DataRowRecieved(message.correlationId().asInteger(), SingleReq, CurrentRow, CurrentHead);
											}
											emit DataRecieved(message.correlationId().asInteger(), SingleReq);
										}
									}
								}
								else {
									emit ErrorOccurred(message.correlationId().asInteger(), SingleReq, BloombergRequest::NoData);
								}
							}
						}
					}
				}
			}
		}
	}
}
#endif