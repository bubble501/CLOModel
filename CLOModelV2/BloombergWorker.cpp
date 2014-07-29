#ifndef NO_BLOOMBERG
#include "BloombergWorker.h"
#include <blpapi_message.h>
#include <blpapi_request.h>
#include <blpapi_session.h>
#include <blpapi_correlationid.h>

using namespace BloombergLP;
using namespace blpapi;
BloombergWorker::BloombergWorker(long long ReqID)
	:RequestID(ReqID)
{}
void BloombergWorker::AddOverride(const QString& OvrName, const QString& OvrVal){
	RequiredOvrdNames.append(OvrName);
	RequiredOvrdValues.append(OvrVal);
}
void BloombergWorker::AddSecurity(const QString& Secur, const QString& Extension){
	RequiredSecurity.append(Secur.trimmed().toUpper());
	QString AsjExt(Extension.trimmed().toLower());
	AsjExt[0]=AsjExt[0].toUpper();
	SecExt.append(AsjExt);
}
BloombergResult BloombergWorker::handleResponseEvent(const Event& event)const {
	MessageIterator iter(event);
	BloombergResult Result;
	int NumVals;
	while (iter.next()) {
		Message message = iter.message();
		if(RequestID==message.correlationId().asInteger()){
			if(message.hasElement("responseError")){
				Result.SetErrorCode(BloombergResult::ResponseError);
				return Result;
			}
			QString Tem=message.messageType().string();
			if(QString(message.messageType().string())=="ReferenceDataResponse"){
				NumVals=message.getElement("securityData").numValues();
				for(int i=0;i<NumVals;i++){
					if(message.getElement("securityData").getValueAsElement(i).hasElement("securityError")){
						for (int j = 0; j < RequiredField.size(); j++)
						{
							Result.SetErrorCode(BloombergResult::SecurityError);
						}
					}
					else{
						for (int j=0;j<RequiredField.size();j++){
							if(message.getElement("securityData").getValueAsElement(i).getElement("fieldData").hasElement(RequiredField.at(j).toLocal8Bit().data())){
								Result.SetResult(RequiredSecurity.at(i), RequiredField.at(j), message.getElement("securityData").getValueAsElement(i).getElement("fieldData").getElementAsString(RequiredField.at(j).toLocal8Bit().data()));
							}
						}
					}
				}
			}
		}
	}
	return Result;
}
BloombergResult BloombergWorker::StartRequest()const {
	BloombergResult Result;
	if (!IsValid()) {
		Result.SetErrorCode(BloombergResult::InvalidInputs);
		return Result;
	}
	bool Valid;
	SessionOptions sessionOptions;
	sessionOptions.setServerHost("localhost");
	sessionOptions.setServerPort(8194);
	Session session(sessionOptions);
	Valid=session.start();
	if(!Valid) {
		Result.SetErrorCode(BloombergResult::SessionError);
		return Result;
	}
	Valid=session.openService("//blp/refdata");
	if(!Valid) {
		Result.SetErrorCode(BloombergResult::ServiceError);
		return Result;
	}
	CorrelationId requestId(RequestID);
	Service refDataSvc = session.getService("//blp/refdata");
	Request request = refDataSvc.createRequest("ReferenceDataRequest");
	for (int i=0;i<RequiredSecurity.size();i++){
		request.append("securities", QString(RequiredSecurity.at(i)+' '+SecExt.at(i)).toLocal8Bit().data());
	}
	foreach(const QString& SilgleField, RequiredField){
		request.append("fields", SilgleField.toLocal8Bit().data());
	}
	for (int i=0;i<RequiredOvrdNames.count();i++){
		Element CurrentOverrides= request.getElement("overrides").appendElement();
		CurrentOverrides.setElement("fieldId",RequiredOvrdNames.at(i).toLocal8Bit().data());
		CurrentOverrides.setElement("value",RequiredOvrdValues.at(i).toLocal8Bit().data());
	}
	session.sendRequest(request, requestId);
	continueToLoop = true;
	while (continueToLoop) {
		Event event = session.nextEvent();
		if(event.eventType()==Event::RESPONSE) {
			continueToLoop = false;
			Result=handleResponseEvent(event);
		}
	}
	session.stop();
	return Result;
}
bool BloombergWorker::IsValid()const{
	return
		!RequiredField.isEmpty()
		&& !RequiredSecurity.isEmpty()
		&& RequiredOvrdNames.size()==RequiredOvrdValues.size()
		&& RequiredSecurity.size()==SecExt.size()
	;
}
#endif