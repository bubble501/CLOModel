#ifndef NO_BLOOMBERG
#ifndef BloombergRequest_h__
#define BloombergRequest_h__
#include "QProjectGlobals.h"
#include "BackwardCompatibilityInterface.h"
#include "QSingleBbgResult.h"
#include <QList> 
#include <QHash>
#ifdef QBBG_PRIVATE
namespace QBloombergLib { class QBbgRequest; }
QDataStream& operator<<(QDataStream& stream, const QBloombergLib::QBbgRequest& flows);
QDataStream& operator>>(QDataStream& stream, QBloombergLib::QBbgRequest& flows);
#endif
namespace QBloombergLib {
	typedef QHash<QString, QString> Overrides;
	class QBbgWorker;
	class QSingleBbgRequest;
	class BLOOMBERG_EXPORT QBbgRequest: public BackwardInterface
	{
	public:
		enum YellowKeys {
			Invalid = -1
			, Govt
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
		static QString YellowKey2String(YellowKeys a);
		static YellowKeys String2YellowKey(QString a);
		QBbgRequest();
		QBbgRequest(const QBbgRequest& a);
		QBbgRequest& operator=(const QBbgRequest& a);
		~QBbgRequest();
		qint32 NumRequests() const;
		const QSingleBbgRequest* GetRequest(quint32 Index) const;
		const QSingleBbgRequest* FindRequest(qint64 ID) const;
		QList<qint64> FindSecurity(QString Secur)const;
		QList<qint64> FindField(QString Field)const;
		QList<qint64> FindSecurityField(QString Secur, QString Field)const;
		QList<qint64> IdList() const;
		void AddRequest(const QSingleBbgRequest& a);
		void AddRequest(qint64 ID, const QString& Secur, const QString& Field, YellowKeys YellowKey = Mtge);
		void AddRequest(const QString& Secur, const QString& Field, YellowKeys YellowKey = Mtge);
		void AddRequest(qint64 ID, const QString& Secur, const QString& Field, const Overrides& Overr, YellowKeys YellowKey = Mtge);
		void AddRequest(const QString& Secur, const QString& Field, const Overrides& Overr, YellowKeys YellowKey = Mtge);
		qint32 GetErrorCode() const;
		QString GetErrorString() const;
		bool HasErrors() const;
		bool IsValidReq() const;
		void SetAutoConstantRates(bool val);
		bool GetAutoConstantRates() const;
		void ClearRequests();
#ifdef QBBG_PRIVATE
	private:
		qint64 MaxID;
		qint32 m_ErrorCode;
		bool m_AutoConstantRates;
		QHash<qint64, QSingleBbgRequest*>  ResultTable;
		bool SameRequest(const QList<qint64>& a, const QList<qint64>& b) const;
	protected:
		QSingleBbgRequest* GetEditRequest(quint32 Index);
		QSingleBbgRequest* FindEditRequest(qint64 ID);
		void RequestGroups(QHash<qint64, QList<qint64>* >& Result, qint64 StartingID = 1)const;
		void ClearResults();
		void SetErrorCode(QSingleBbgResult::BbgErrorCodes ErrCd);
		friend QBbgWorker;
		virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
		friend QDataStream& ::operator<<(QDataStream& stream, const QBloombergLib::QBbgRequest& flows);
		friend QDataStream& ::operator>>(QDataStream& stream, QBloombergLib::QBbgRequest& flows);
#endif
	};
}
#endif // BloombergRequest_h__
#endif