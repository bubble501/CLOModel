#ifndef TriggersResults_h__
#define TriggersResults_h__
#include "BackwardCompatibilityInterface.h"
#include <QHash>
class QDate;
class TriggersResultsPrivate;
class QDataStream;
class CLOMODELLIB_EXPORT TriggersResults :public BackwardInterface
{
    DECLARE_PUBLIC_COMMONS(TriggersResults)
    DECLARE_PUBLIC_COMMONS_COPY(TriggersResults)
public:
	enum TrigRes : qint8 {
		trFalse=0
		,trTrue=1
		,trNA=-1
	};
	virtual ~TriggersResults();
	virtual void ClearResults();
	TrigRes GetResult(quint32 TrigType, const QDate& RefDate) const; 
	void SetResult(quint32 TrigType, const QDate& RefDate, bool Res);
protected:
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
	friend QDataStream& operator<<(QDataStream & stream, const TriggersResults& flows);
	friend QDataStream& operator>>(QDataStream & stream, TriggersResults& flows);
};
QDataStream& operator<<(QDataStream & stream, const TriggersResults& flows);
QDataStream& operator>>(QDataStream & stream, TriggersResults& flows);
#endif // TriggersResults_h__