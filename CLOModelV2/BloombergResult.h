#ifndef NO_BLOOMBERG
#ifndef BloombergResult_h__
#define BloombergResult_h__
#include <QHash>
#include <QStringList>
#include <QDataStream>
#include "BackwardCompatibilityInterface.h"
class BloombergWorker;
class BloombergResult : public BackwardInterface {
public:
	enum BbgErrorCodes {
		NoErrors=0
		,ResponseError=1
		,SecurityError=2
		,InvalidInputs=4
		,SessionError=8
		,ServiceError=16
	};
	BloombergResult() : m_ErrorCode(NoErrors) {}
	QStringList AvailableSecurities() const { return ResultTable.keys(); }
	QStringList AvailableFields(const QString& Security);
	QString GetValue(const QString& Security, const QString& Field);
	BbgErrorCodes GetErrorCode() const { return m_ErrorCode; }
	bool HasErrors() const { return m_ErrorCode == NoErrors; }
protected:
	BbgErrorCodes m_ErrorCode;
	void SetErrorCode(BbgErrorCodes ErrCd = NoErrors);
	QHash<QString, QHash<QString, QString> > ResultTable;
	void SetResult(const QString& Security, const QString& Field, const QString& Value);
	virtual QDataStream& LoadOldVersion(QDataStream& stream);
	friend BloombergWorker;
	friend QDataStream& operator<<(QDataStream & stream, const BloombergResult& flows);
	friend QDataStream& operator>>(QDataStream & stream, BloombergResult& flows);
};
QDataStream& operator<<(QDataStream & stream, const BloombergResult& flows);
QDataStream& operator>>(QDataStream & stream, BloombergResult& flows);
#endif // BloombergResult_h__
#endif // NO_BLOOMBERG
