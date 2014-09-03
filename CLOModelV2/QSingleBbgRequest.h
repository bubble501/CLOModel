#ifndef NO_BLOOMBERG
#ifndef SingleBbgRequest_h__
#define SingleBbgRequest_h__
#include "QProjectGlobals.h"
#include "QBbgRequest.h"
#include "BackwardCompatibilityInterface.h"
#include "QSingleBbgResult.h"
#include <QHash>
#ifdef QBBG_PRIVATE
namespace QBloombergLib { class QSingleBbgRequest; }
QDataStream& operator<<(QDataStream& stream, const QBloombergLib::QSingleBbgRequest& flows);
QDataStream& operator>>(QDataStream& stream, QBloombergLib::QSingleBbgRequest& flows);
#endif
namespace QBloombergLib {
	class QBbgWorker;
	class BLOOMBERG_EXPORT QSingleBbgRequest: public BackwardInterface
	{
	public:
		QSingleBbgRequest();
		QSingleBbgRequest(const QSingleBbgRequest& a);
		bool operator==(const QSingleBbgRequest& a) const;
		QSingleBbgRequest& operator=(const QSingleBbgRequest& a);
		qint64 GetResultID() const;
		const QString& GetSecurity() const;
		const QString& GetField() const;
		bool GetAutoConstantRates() const;
		void SetAutoConstantRates(bool val);
		void SetResultID(qint64 val);
		void SetSecurity(const QString& val);
		void SetField(const QString& val);
		const QHash<QString, QString>& GetOverrides() const;
		void SetOverrides(const QHash<QString, QString>& Overrides);
		void SetOverride(QString Name, const QString& Value);
		void ClearOverrides();
		bool IsValidReq() const;
		QString GetExtensionString() const;
		QString GetFullSecurity() const;
		QBbgRequest::YellowKeys GetExtension()const;
		void SetExtension(const QString& val);
		void SetExtension(QBbgRequest::YellowKeys val);
#ifdef QBBG_PRIVATE
	private:
		qint64 m_ResultID;
		QString m_Security;
		QBbgRequest::YellowKeys m_Extension;
		QString m_Field;
		QHash<QString, QString> m_Overrides;
		bool m_AutoConstantRates;
	protected:
		bool SameOverrides(const QSingleBbgRequest& a)const;
		friend class QBbgRequest;
		virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
		friend QDataStream& ::operator<<(QDataStream & stream, const QBloombergLib::QSingleBbgRequest& flows);
		friend QDataStream& ::operator>>(QDataStream & stream, QBloombergLib::QSingleBbgRequest& flows);
#endif
	};
}
#endif // SingleBbgRequest_h__
#endif
