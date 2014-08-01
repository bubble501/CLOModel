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
			if (QString(message.messageType().string()) == "SessionStarted") {
				session->openServiceAsync("//blp/refdata", CorrelationId(-1i64));
			}
			else if (QString(message.messageType().string()) == "SessionConnectionUp") continue;
			else /*if (QString(message.messageType().string()) == "SessionStartupFailure")*/ {
				AdjParent->m_Requests.SetErrorCode(BloombergRequest::SessionError);
				emit ErrorOccurred(-1i64, BloombergRequest::SessionError);
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
				AdjParent->m_Requests.SetErrorCode(BloombergRequest::ServiceError);
				emit ErrorOccurred(-1i64, BloombergRequest::ServiceError);
			}
		}
	}
	case Event::PARTIAL_RESPONSE:
	case Event::RESPONSE:
		handleResponseEvent(event);
		break;
	default:
		AdjParent->m_Requests.SetErrorCode(BloombergRequest::UnknownError);
		emit ErrorOccurred(-1i64, BloombergRequest::UnknownError);
	}
	return true;
}

AsyncEventHandler::AsyncEventHandler(AsyncBloombergWorker* parent)
	:QObject(parent)
{}

void AsyncEventHandler::handleResponseEvent(const Event& event) {
	MessageIterator iter(event);
	int NumVals, NumFieldExep;
	AsyncBloombergWorker* AdjParent = dynamic_cast<AsyncBloombergWorker*>(parent());
	while (iter.next()) {
		Message message = iter.message();
		if (AdjParent->Groups.contains(message.correlationId().asInteger())) {
			QList<qint64> CurrentGroup = AdjParent->Groups.value(message.correlationId().asInteger());
			if (message.hasElement("responseError")) {
				AdjParent->m_Requests.SetErrorCode(BloombergRequest::ResponseError);
				foreach(qint64 SingleReq, CurrentGroup) {
					AdjParent->m_Requests.FindRequest(SingleReq)->SetErrorCode(BloombergRequest::ResponseError);
					emit DataRecieved(SingleReq);
				}
				return;
			}
			QString Tem = message.messageType().string();
			if (QString(message.messageType().string()) == "ReferenceDataResponse") {
				NumVals = message.getElement("securityData").numValues();
				for (int i = 0; i < NumVals; i++) {
					NumFieldExep = message.getElement("securityData").getValueAsElement(i).getElement("fieldExceptions").numValues();
					for (int j = 0; j < NumFieldExep; j++) {
						foreach(qint64 SingleReq, CurrentGroup) {
							SingleBbgRequest* FoundRequ = AdjParent->m_Requests.FindRequest(SingleReq);
							QString CurrentSecurity = message.getElement("securityData").getValueAsElement(i).getElementAsString("security");
							QString CurrentField = message.getElement("securityData").getValueAsElement(i).getElement("fieldExceptions").getValueAsElement(j).getElementAsString("fieldId");
							if (
								FoundRequ->GetFullSecurity() == CurrentSecurity
								&& FoundRequ->GetField() == CurrentField
							) {
								FoundRequ->SetErrorCode(BloombergRequest::FieldError); 
								emit DataRecieved(SingleReq);
							}
						}
					}
					if (message.getElement("securityData").getValueAsElement(i).hasElement("securityError")) {
						QString CurrentSecurity = message.getElement("securityData").getValueAsElement(i).getElementAsString("security");
						foreach(qint64 SingleReq, CurrentGroup) {
							SingleBbgRequest* FoundRequ = AdjParent->m_Requests.FindRequest(SingleReq);
							if ((FoundRequ->GetFullSecurity()) == CurrentSecurity) {
								FoundRequ->SetErrorCode(BloombergRequest::SecurityError);
								emit DataRecieved(SingleReq);
							}
						}
					}
					else {
						QString CurrentSecurity = message.getElement("securityData").getValueAsElement(i).getElementAsString("security");
						foreach(qint64 SingleReq, CurrentGroup) {
							SingleBbgRequest* FoundRequ = AdjParent->m_Requests.FindRequest(SingleReq);
							if ((FoundRequ->GetFullSecurity()) == CurrentSecurity) {
								if (message.getElement("securityData").getValueAsElement(i).getElement("fieldData").hasElement(FoundRequ->GetField().toLatin1().data())) {
									const int NumResults = message.getElement("securityData").getValueAsElement(i).getElement("fieldData").getElement(FoundRequ->GetField().toLatin1().data()).numValues();
									if (NumResults == 1) {
										FoundRequ->SetValue(message.getElement("securityData").getValueAsElement(i).getElement("fieldData").getElementAsString(FoundRequ->GetField().toLatin1().data()), FoundRequ->GetField());
										emit DataRecieved(SingleReq);
									}
									else {
										QStringList CurrentRow, CurrentHead;
										const int NumCols =
											message.getElement("securityData").getValueAsElement(i).getElement("fieldData").getElement(FoundRequ->GetField().toLatin1().data()).getValueAsElement(0).numElements();
										for (int TableIter = 0; TableIter < NumResults; TableIter++) {
											CurrentRow.clear(); CurrentHead.clear();
											for (int ColIter = 0; ColIter < NumCols; ColIter++) {
												CurrentRow << message.getElement("securityData").getValueAsElement(i).getElement("fieldData").getElement(FoundRequ->GetField().toLatin1().data()).getValueAsElement(TableIter).getElement(ColIter).getValueAsString();
												CurrentHead << message.getElement("securityData").getValueAsElement(i).getElement("fieldData").getElement(FoundRequ->GetField().toLatin1().data()).getValueAsElement(TableIter).getElement(ColIter).name().string();
											}
											FoundRequ->AddValueRow(CurrentRow, CurrentHead);
											emit DataRecieved(SingleReq);
										}
									}
								}

							}
						}
					}
				}
			}
		}
	}
}
