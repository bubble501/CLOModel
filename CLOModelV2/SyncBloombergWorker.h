#ifndef NO_BLOOMBERG
#ifndef BloombergWorker_h__
#define BloombergWorker_h__
#include <blpapi_event.h>
#include <QStringList>
#include <QHash>
#include "BloombergRequest.h"
#include "AbstractBbgWorker.h"
class SyncBloombergWorker :public AbstractBbgWorker {
protected:
	bool continueToLoop;
	void handleResponseEvent(const BloombergLP::blpapi::Event& event) ;
public:
	using AbstractBbgWorker::StartRequest;
	virtual void StartRequest();
	const BloombergRequest& GetResults() { StartRequest(); return m_Requests; }
	const BloombergRequest& GetResults(const BloombergRequest& Req) { StartRequest(Req); return m_Requests; }
	SyncBloombergWorker();
};
#endif // BloombergWorker_h__
#endif // NO_BLOOMBERG
