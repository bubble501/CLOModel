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
QHash<QString, QHash<QString,QString> > BloombergWorker::handleResponseEvent(const Event& event)const{
	MessageIterator iter(event);
	QHash<QString, QHash<QString,QString> > Result;
	int NumVals;
	while (iter.next()) {
		Message message = iter.message();
		if(RequestID==message.correlationId().asInteger()){
			if(message.hasElement("responseError")){
				QHash<QString,QString> Temp;
				Temp.insert("Error","#N/A N/A");
				Result.insert("Error",Temp);
				return Result;
			}
			QString Tem=message.messageType().string();
			if(QString(message.messageType().string())=="ReferenceDataResponse"){
				NumVals=message.getElement("securityData").numValues();
				QHash<QString,QString> Temp;
				for(int i=0;i<NumVals;i++){
					Temp.clear();
					if(message.getElement("securityData").getValueAsElement(i).hasElement("securityError")){
						for (int j=0;j<RequiredField.size();j++)
							Temp.insert(RequiredField.at(j),"#N/A Security Error");
						Result.insert(RequiredSecurity.at(i),Temp);
					}
					else{
						for (int j=0;j<RequiredField.size();j++){
							if(message.getElement("securityData").getValueAsElement(i).getElement("fieldData").hasElement(RequiredField.at(j).toLocal8Bit().data())){
								Temp.insert(RequiredField.at(j),message.getElement("securityData").getValueAsElement(i).getElement("fieldData").getElementAsString(RequiredField.at(j).toLocal8Bit().data()));
								Result.insert(RequiredSecurity.at(i),Temp);
							}
						}
					}
				}
			}
		}
	}
	return Result;
}
QHash<QString, QHash<QString,QString> > BloombergWorker::StartRequest()const{
	QHash<QString, QHash<QString,QString> > Result;
	if (!IsValid()) {
		QHash<QString,QString> Temp;
		Temp.insert("Error","#N/A Invalid Inputs");
		Result.insert("Error",Temp);
		return Result;
	}
	bool Valid;
	SessionOptions sessionOptions;
	sessionOptions.setServerHost("localhost");
	sessionOptions.setServerPort(8194);
	Session session(sessionOptions);
	Valid=session.start();
	if(!Valid) {
		QHash<QString,QString> Temp;
		Temp.insert("Error","#N/A Can't start Bloomberg session.");
		Result.insert("Error",Temp);
		return Result;
	}
	Valid=session.openService("//blp/refdata");
	if(!Valid) {
		QHash<QString,QString> Temp;
		Temp.insert("Error","#N/A Can't open Bloomberg service.");
		Result.insert("Error",Temp);
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