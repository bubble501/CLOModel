#ifndef NO_BLOOMBERG
#ifndef BloombergRequest_h__
#define BloombergRequest_h__
#include <QHash>
#include <QDataStream>
#include "BackwardCompatibilityInterface.h"
class SyncBloombergWorker;
class SingleBbgRequest;
class AsyncBloombergWorker;
class AsyncEventHandler;
class BloombergRequest : public BackwardInterface {
public:
	enum YellowKeys {
		Invalid=-1
		,Govt
		, Corp
		, Mtge
		, MMkt
		, Muni
		, Pfd
		, Equity
		, Comdty
		, Index
		, Curncy
	};
	enum BbgErrorCodes {
		NoErrors=0
		,ResponseError=0x1
		,SecurityError=0x2
		,InvalidInputs=0x4
		, SessionError = 0x8
		, ServiceError = 0x10
		, FieldError = 0x20
		, UnknownError = 0x40
		, SessionStopped = 0x80
		, NoData = 0x100
	};
	static QString YellowKey2String(YellowKeys a);
	static YellowKeys String2YellowKey(const QString& a);
	BloombergRequest() : m_ErrorCode(NoErrors), MaxID(std::numeric_limits<qint64>::min()) {}
	BloombergRequest(const BloombergRequest& a);
	BloombergRequest& operator=(const BloombergRequest& a);
	~BloombergRequest();
	int NumRequests() const { return ResultTable.size(); }
	const SingleBbgRequest* GetRequest(int Index) const;
	const SingleBbgRequest* FindRequest(qint64 ID) const;
	QList<qint64> FindSecurity(const QString& Secur)const;
	QList<qint64> FindField(const QString& Field)const;
	QList<qint64> FindSecurityField(const QString& Secur, const QString& Field)const;
	QList<qint64> IdList() const;
	void AddRequest(const SingleBbgRequest& a);
	void AddRequest(qint64 ID, const QString& Secur, const QString& Field, YellowKeys YellowKey = Mtge);
	void AddRequest(const QString& Secur, const QString& Field, YellowKeys YellowKey = Mtge);
	void AddRequest(qint64 ID, const QString& Secur, const QString& Field, const QHash<QString, QString>& Overrides, YellowKeys YellowKey = Mtge);
	void AddRequest(const QString& Secur, const QString& Field, const QHash<QString, QString>& Overrides, YellowKeys YellowKey = Mtge);
	int GetErrorCode() const { return m_ErrorCode; }
	bool HasErrors() const { return m_ErrorCode != NoErrors; }
	QHash<qint64, QList<qint64> > RequestGroups(qint64 StartingID=1)const;
	bool IsValidReq() const;
	void SetAutoConstantRates(bool val);
	bool GetAutoConstantRates() const { return m_AutoConstantRates; }
	void ClearRequests();
private:
	qint64 MaxID;
protected:
	int m_ErrorCode;
	bool m_AutoConstantRates;
	QHash<qint64, SingleBbgRequest*>  ResultTable;
	SingleBbgRequest* GetRequest(int Index);
	SingleBbgRequest* FindRequest(qint64 ID);
	void ClearResults();
	void SetErrorCode(BbgErrorCodes ErrCd = NoErrors);
	void SetResult(qint64 ID, const QString& Value);
	virtual QDataStream& LoadOldVersion(QDataStream& stream);
	friend QDataStream& operator<<(QDataStream & stream, const BloombergRequest& flows);
	friend QDataStream& operator>>(QDataStream & stream, BloombergRequest& flows);
	friend SyncBloombergWorker;
	friend AsyncBloombergWorker;
	friend SingleBbgRequest;
	friend AsyncEventHandler;
};

QDataStream& operator<<(QDataStream & stream, const BloombergRequest& flows);
QDataStream& operator>>(QDataStream & stream, BloombergRequest& flows);

#endif // BloombergRequest_h__
#endif // NO_BLOOMBERG
