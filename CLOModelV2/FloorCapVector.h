#ifndef FloorCapVector_h__
#define FloorCapVector_h__
#include <QList>
#include "AbstractBbgVect.h"
#include <QSharedPointer>
class FloorCapVector : public AbstractBbgVect {
private:
	QList<QSharedPointer<double> > m_FloorVal;
	QList<QSharedPointer<double> > m_CapVal;
protected:
	virtual void UnpackVector() override;
	virtual bool IsValid() const override;
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
public:
	virtual QRegExpValidator* GetValidator(QObject* parent = 0) const override;
	using AbstractBbgVect::IsEmpty;
	FloorCapVector() {}
	FloorCapVector(const FloorCapVector& Vec);
	FloorCapVector(const QString& Vec);
	FloorCapVector(const QString& Vec, const QDate& Anchor);
	template<class T> QSharedPointer<double> GetFloor(const T& index) const {
		static_assert(std::is_same<T, QDate>::value || std::is_same<T, int>::value, "GetFloor can be used only with int or QDate");
		return AbstractBbgVect::GetValueTemplate(m_FloorVal, index, QSharedPointer<double>(NULL));
	}
	template<class T> QSharedPointer<double> GetCap(const T& index) const {
		static_assert(std::is_same<T, QDate>::value || std::is_same<T, int>::value, "GetCap can be used only with int or QDate");
		return AbstractBbgVect::GetValueTemplate(m_CapVal, index, QSharedPointer<double>(NULL));
	}
	int NumElements() const { return m_FloorVal.size(); }
	FloorCapVector& operator=(const QString& a) { AbstractBbgVect::operator=(a); return *this; }
	FloorCapVector& operator=(const FloorCapVector& Vec);
	friend QDataStream& operator<<(QDataStream & stream, const FloorCapVector& flows);
	friend QDataStream& operator>>(QDataStream & stream, FloorCapVector& flows);
};
QDataStream& operator<<(QDataStream & stream, const FloorCapVector& flows);
QDataStream& operator>>(QDataStream & stream, FloorCapVector& flows);
Q_DECLARE_METATYPE(FloorCapVector)
#endif // FloorCapVector_h__

