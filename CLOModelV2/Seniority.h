#include "BackwardCompatibilityInterface.h"
#include <QList>
#include <QMetaType>
#define SeniorityStringSeparator ','
class Seniority : public BackwardInterface{
	QList<quint32> m_SeniorityScale;
	int m_CurrnetLevel;
public:
	void Clear() { m_CurrnetLevel = -1; m_SeniorityScale.clear(); }
	int GetCurrentLevel() const { return m_CurrnetLevel; }
	bool IncreaseLevel() { return SetCurrentLevel(m_CurrnetLevel + 1); }
	bool DecreaseLevel() { return SetCurrentLevel(m_CurrnetLevel - 1); }
	bool SetCurrentLevel(int a);
	quint32 GetCurrentSeniority() const { return m_SeniorityScale.at(m_CurrnetLevel); }
	quint32 GetSeniorityAtLevel(int a) const { if (a < 0 || a >= m_SeniorityScale.size()) return 0; return  m_SeniorityScale.at(a); }
	bool AddSeniorityLevel(quint32 a);
	bool SetSeniorityScale(const QString& a);
	QString ToString() const;
	Seniority() :m_CurrnetLevel(-1){}
	Seniority(const QString& a) :m_CurrnetLevel(-1) { SetSeniorityScale(a); }
protected:
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
	friend QDataStream& operator<<(QDataStream & stream, const Seniority& flows);
	friend QDataStream& operator>>(QDataStream & stream, Seniority& flows);
};
Q_DECLARE_METATYPE(Seniority)
QDataStream& operator<<(QDataStream & stream, const Seniority& flows);
QDataStream& operator>>(QDataStream & stream, Seniority& flows);

