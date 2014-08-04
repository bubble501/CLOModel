#ifndef NO_BLOOMBERG
#ifndef SingleBbgRequest_h__
#define SingleBbgRequest_h__
#include <QMap>
#include <QHash>
#include <QDataStream>
#include "BackwardCompatibilityInterface.h"
#include "BloombergRequest.h"
#include "BloombergResult.h"
class AsyncBloombergWorker;
class AsyncEventHandler;
class SingleBbgRequest : public BackwardInterface {
public:
	SingleBbgRequest();
	SingleBbgRequest(const SingleBbgRequest& a);
	bool operator==(const SingleBbgRequest& a) const;
	SingleBbgRequest& operator=(const SingleBbgRequest& a);
	qint64 GetResultID() const { return m_ResultID; }
	const QString& GetSecurity() const { return m_Security; }
	const QString& GetField() const { return m_Field; }
	const BloombergResult& GetValue() const { return m_Value; }
	int GetError() const { return m_ErrorCode; }
	bool HasErrors() const { return m_ErrorCode != BloombergRequest::NoErrors; }
	bool HasResult() const { return !m_Value.IsEmpty(); }
	bool GetAutoConstantRates() const { return m_AutoConstantRates; }
	void SetAutoConstantRates(bool val);
	void SetResultID(qint64 val) { m_ResultID = val; }
	void SetSecurity(const QString& val) { m_Security = val.trimmed().toUpper(); }
	void SetField(const QString& val) { m_Field = val.trimmed().toUpper();  m_Field.replace(' ', '_'); }
	void SetErrorCode(BloombergRequest::BbgErrorCodes val);
	const QMap<QString, QString>& GetOverrides() const { return m_Overrides; }
	void SetOverrides(const QMap<QString, QString>& Overrides);
	void SetOverride(QString Name, const QString& Value);
	void ClearOverrides();
	bool IsValidReq() const;
	QString GetExtensionString() const { return BloombergRequest::YellowKey2String(m_Extension); }
	QString GetFullSecurity() const { return m_Security + ' ' + BloombergRequest::YellowKey2String(m_Extension); }
	BloombergRequest::YellowKeys GetExtension()const { return m_Extension; }
	void SetExtension(const QString& val);
	void SetExtension(BloombergRequest::YellowKeys val) { m_Extension = val; }
private:
	qint64 m_ResultID;
	QString m_Security;
	BloombergRequest::YellowKeys m_Extension;
	QString m_Field;
	int m_ErrorCode;
	BloombergResult m_Value;
	QMap<QString, QString> m_Overrides;
	bool m_AutoConstantRates;
protected:
	bool SameOverrides(const SingleBbgRequest& a)const;
	void SetValue(const QString& val, const QString& Header = "") { m_Value.SetValue(val,Header); }
	void AddValueRow(const QStringList& val, const QStringList& Headers = QStringList()) { m_Value.AddValueRow(val, Headers); }
	virtual QDataStream& LoadOldVersion(QDataStream& stream);
	friend QDataStream& operator<<(QDataStream & stream, const SingleBbgRequest& flows);
	friend QDataStream& operator>>(QDataStream & stream, SingleBbgRequest& flows);
	friend SyncBloombergWorker;
	friend AsyncBloombergWorker;
	friend AsyncEventHandler;
	friend BloombergRequest;
};
QDataStream& operator<<(QDataStream & stream, const SingleBbgRequest& flows);
QDataStream& operator>>(QDataStream & stream, SingleBbgRequest& flows);
#endif // SingleBbgRequest_h__
#endif