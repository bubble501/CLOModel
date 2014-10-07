#ifndef TriggersResults_h__
#define TriggersResults_h__

#include "BackwardCompatibilityInterface.h"
#include <QHash>
#include <QMap>
#include <QDate>
class TriggersResults :public BackwardInterface{
public:
	enum class TrigRes : qint8 {
		trFalse=0
		,trTrue=1
		,trNA=-1
	};
	TriggersResults() {}
	TriggersResults(const TriggersResults& a);
	TriggersResults& operator=(const TriggersResults& a);
	virtual ~TriggersResults() { ClearResults(); }
	void ClearResults();
	TrigRes GetResult(quint32 TrigType, const QDate& RefDate) const; 
	void SetResult(quint32 TrigType, const QDate& RefDate, bool Res);
protected:
	QHash<quint32, QMap<QDate, bool>* > m_Results;
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
	friend QDataStream& operator<<(QDataStream & stream, const TriggersResults& flows);
	friend QDataStream& operator>>(QDataStream & stream, TriggersResults& flows);
};
QDataStream& operator<<(QDataStream & stream, const TriggersResults& flows);
QDataStream& operator>>(QDataStream & stream, TriggersResults& flows);
#endif // TriggersResults_h__