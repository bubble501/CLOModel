#ifndef NO_BLOOMBERG
#include "SyncBloombergWorker.h"
#include "SingleBbgRequest.h"
#include <blpapi_message.h>
#include <blpapi_request.h>
#include <blpapi_session.h>
#include <blpapi_correlationid.h>

using namespace BloombergLP;
using namespace blpapi;
SyncBloombergWorker::SyncBloombergWorker()
	:AbstractBbgWorker()
{}
void SyncBloombergWorker::handleResponseEvent(const Event& event) {
	MessageIterator iter(event);
	int NumVals,NumFieldExep;
	while (iter.next()) {
		Message message = iter.message();
		if (Groups.contains(message.correlationId().asInteger())) {
			QList<qint64> CurrentGroup = Groups.value(message.correlationId().asInteger());
			if(message.hasElement("responseError")){
				m_Requests.SetErrorCode(BloombergRequest::ResponseError);
				foreach(qint64 SingleReq, CurrentGroup) {
					m_Requests.FindRequest(SingleReq)->SetErrorCode(BloombergRequest::ResponseError);
				}
				return;
			}
			QString Tem=message.messageType().string();
			if(QString(message.messageType().string())=="ReferenceDataResponse"){
				NumVals=message.getElement("securityData").numValues();
				for(int i=0;i<NumVals;i++){
					NumFieldExep = message.getElement("securityData").getValueAsElement(i).getElement("fieldExceptions").numValues();
					for (int j = 0; j < NumFieldExep; j++) {
						foreach(qint64 SingleReq, CurrentGroup) {
							SingleBbgRequest* FoundRequ = m_Requests.FindRequest(SingleReq);
							QString CurrentSecurity = message.getElement("securityData").getValueAsElement(i).getElementAsString("security");
							QString CurrentField = message.getElement("securityData").getValueAsElement(i).getElement("fieldExceptions").getValueAsElement(j).getElementAsString("fieldId");
							if (
								FoundRequ->GetFullSecurity() == CurrentSecurity
								&& FoundRequ->GetField() == CurrentField
							)
								FoundRequ->SetErrorCode(BloombergRequest::FieldError);
						}
					}
					if(message.getElement("securityData").getValueAsElement(i).hasElement("securityError")){
						QString CurrentSecurity = message.getElement("securityData").getValueAsElement(i).getElementAsString("security");
						foreach(qint64 SingleReq, CurrentGroup) {
							SingleBbgRequest* FoundRequ = m_Requests.FindRequest(SingleReq);
							if ((FoundRequ->GetFullSecurity()) == CurrentSecurity)
								FoundRequ->SetErrorCode(BloombergRequest::SecurityError);
						}
					}
					else{
						QString CurrentSecurity = message.getElement("securityData").getValueAsElement(i).getElementAsString("security");
						foreach(qint64 SingleReq, CurrentGroup) {
							SingleBbgRequest* FoundRequ = m_Requests.FindRequest(SingleReq);
							if ((FoundRequ->GetFullSecurity()) == CurrentSecurity) {
								if (message.getElement("securityData").getValueAsElement(i).getElement("fieldData").hasElement(FoundRequ->GetField().toLatin1().data())) {
									const int NumResults = message.getElement("securityData").getValueAsElement(i).getElement("fieldData").getElement(FoundRequ->GetField().toLatin1().data()).numValues();
									if (NumResults == 1) FoundRequ->SetValue(message.getElement("securityData").getValueAsElement(i).getElement("fieldData").getElementAsString(FoundRequ->GetField().toLatin1().data()), FoundRequ->GetField());
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
void SyncBloombergWorker::StartRequest() {
	m_Requests.SetErrorCode(BloombergRequest::NoErrors);
	m_Requests.ClearResults();

	if (!m_Requests.IsValidReq()) {
		m_Requests.SetErrorCode(BloombergRequest::InvalidInputs);
		return;
	}
	SessionOptions sessionOptions;
	sessionOptions.setServerHost(ServerAddress.toLatin1().data());
	sessionOptions.setServerPort(ServerPort);
	Session session(sessionOptions);
	if (!session.start()) {
		m_Requests.SetErrorCode(BloombergRequest::SessionError);
		return;
	}
	if (!session.openService("//blp/refdata")) {
		m_Requests.SetErrorCode(BloombergRequest::ServiceError);
		return;
	}
	Groups = m_Requests.RequestGroups();
	QList<QString> UsedSecur;
	QList<QString> UsedField;
	for (QHash<qint64, QList<qint64> >::const_iterator ReqIter = Groups.constBegin(); ReqIter != Groups.constEnd(); ReqIter++) {
		CorrelationId requestId(ReqIter.key());
		Service refDataSvc = session.getService("//blp/refdata");
		Request request = refDataSvc.createRequest("ReferenceDataRequest");
		QList<qint64> CurrentList = ReqIter.value();
		UsedSecur.clear();
		UsedField.clear();
		for (QList<qint64>::const_iterator GroupIter = CurrentList.constBegin(); GroupIter != CurrentList.constEnd(); GroupIter++) {
			const SingleBbgRequest* CurrentSingle = m_Requests.FindRequest(*GroupIter);
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
		session.sendRequest(request, requestId);
		continueToLoop = true;
		while (continueToLoop) {
			Event event = session.nextEvent();
			if (event.eventType() == Event::RESPONSE) {
				continueToLoop = false;
				handleResponseEvent(event);
			}
		}
	}
	session.stop();
}
#endif