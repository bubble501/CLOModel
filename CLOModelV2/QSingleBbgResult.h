#ifndef NO_BLOOMBERG
#ifndef BloombergResult_h__
#define BloombergResult_h__
#include "QProjectGlobals.h"
#include "BackwardCompatibilityInterface.h"
#include <QList>
#include <QString>
#include <QDate>
#ifdef QBBG_PRIVATE
namespace QBloombergLib {class QSingleBbgResult;}
QDataStream& operator<<(QDataStream& stream, const QBloombergLib::QSingleBbgResult& flows);
QDataStream& operator>>(QDataStream& stream, QBloombergLib::QSingleBbgResult& flows);
#endif
namespace QBloombergLib {
	class QSingleBbgResult;
	class QBbgWorker;
	class BLOOMBERG_EXPORT QSingleBbgResult : public BackwardInterface
	{
	public:
		enum BbgErrorCodes {
			NoErrors = 0
			, ResponseError = 0x1
			, SecurityError = 0x2
			, InvalidInputs = 0x4
			, SessionError = 0x8
			, ServiceError = 0x10
			, FieldError = 0x20
			, UnknownError = 0x40
			, SessionStopped = 0x80
			, NoData = 0x100
		};
		QSingleBbgResult();
		QSingleBbgResult(const QSingleBbgResult& a);
		QSingleBbgResult& operator= (const QSingleBbgResult& a);
		~QSingleBbgResult();
		bool IsEmpty() const;
		bool HasString() const;
		bool HasDate() const;
		bool HasDouble() const;
		bool HasTable() const;
		bool HasValue()const;
		const QString& GetHeader()const;
		QString GetString()const;
		QDate GetDate() const;
		double GetDouble()const;
		qint32 GetNumRows() const;
		qint32 GetNumCols() const;
		const QSingleBbgResult* GetTableResult(quint32 r, quint32 c) const;
		qint32 GetErrorCode() const;
		QString GetErrorString() const;
		bool HasErrors() const;
		static QString BbgErrorCodes2String(qint32 a);
#ifdef QBBG_PRIVATE
	private:
		qint32 m_ErrorCode;
		QString m_Header;
		QString* m_StringValue;
		QDate* m_DateValue;
		double* m_DoubleValue;
		QList<QSingleBbgResult*> m_TableResultRows;
		quint32 TableResultCols;
	protected:
		void Clear();
		void SetValue(const QString& val, const QString& Header = QString());
		void AddValueRow(const QList<QString>& val, const QList<QString>& Headers = QList<QString>());
		void SetErrorCode(BbgErrorCodes ErrCd);
		friend class QBbgWorker;
		virtual QDataStream& LoadOldVersion(QDataStream& stream);
		friend QDataStream& ::operator<<(QDataStream& stream, const QBloombergLib::QSingleBbgResult& flows);
		friend QDataStream& ::operator>>(QDataStream& stream, QBloombergLib::QSingleBbgResult& flows);
#endif
	};
}
#endif // BloombergResult_h__
#endif