#ifndef NO_BLOOMBERG
#ifndef BloombergWorker_h__
#define BloombergWorker_h__
#include <blpapi_event.h>
#include <QStringList>
#include <QHash>
class BloombergWorker {
private:
	mutable bool continueToLoop;
	long long RequestID;
	QStringList SecExt;
	QStringList RequiredSecurity;
	QStringList RequiredField;
	QStringList RequiredOvrdNames;
	QStringList RequiredOvrdValues;
	QHash<QString, QHash<QString,QString> > handleResponseEvent(const BloombergLP::blpapi::Event& event) const;
public:
	QHash<QString, QHash<QString,QString> > StartRequest()const;
	bool IsValid() const;
	void AddSecurity(const QString& Secur, const QString& Extension="Mtge");
	void AddField(const QString& field){RequiredField.append(field);}
	void AddOverride(const QString& OvrName, const QString& OvrVal);
	BloombergWorker(long long ReqID=1);
};
#endif // BloombergWorker_h__
#endif // NO_BLOOMBERG
