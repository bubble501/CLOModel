#ifndef NO_BLOOMBERG
#ifndef BloombergRequest_h__
#define BloombergRequest_h__
#include <QMap>
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
		,ResponseError=1
		,SecurityError=2
		,InvalidInputs=4
		,SessionError=8
		,ServiceError=16
		,FieldError=32
		,UnknownError=64
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
	void AddRequest(qint64 ID, const QString& Secur, const QString& Field, const QMap<QString, QString>& Overrides, YellowKeys YellowKey = Mtge);
	void AddRequest(const QString& Secur, const QString& Field, const QMap<QString, QString>& Overrides, YellowKeys YellowKey = Mtge);
	int GetErrorCode() const { return m_ErrorCode; }
	bool HasErrors() const { return m_ErrorCode != NoErrors; }
	QHash<qint64, QList<qint64> > RequestGroups(qint64 StartingID=1)const;
	bool IsValidReq() const;
private:
	qint64 MaxID;
protected:
	int m_ErrorCode;
	QList<SingleBbgRequest*>  ResultTable;
	SingleBbgRequest* GetRequest(int Index);
	SingleBbgRequest* FindRequest(qint64 ID);
	void ClearResults();
	void ClearRequests();
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
