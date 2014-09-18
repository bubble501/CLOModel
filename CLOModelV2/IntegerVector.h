#ifndef IntegerVector_h__
#define IntegerVector_h__
#include <QList>
#include "AbstarctBbgVect.h"
class IntegerVector : public AbstarctBbgVect {
private:
	QList<int> m_VectVal;
protected:
	virtual void UnpackVector();
	virtual bool IsValid() const override;
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
public:
	using AbstarctBbgVect::IsEmpty;
	virtual bool IsEmpty(int Lbound, int Ubound=INT_MAX)const;
	IntegerVector() {}
	IntegerVector(const IntegerVector& Vec);
	IntegerVector(const QString& Vec);
	IntegerVector(const QString& Vec, const QDate& Anchor);
	int GetValue(const QDate& index) const;
	int GetValue(int index) const;
	int NumElements() const { return m_VectVal.size(); }
	IntegerVector& operator=(const QString& a) { AbstarctBbgVect::operator=(a); return *this; }
	IntegerVector& operator=(const IntegerVector& Vec);
	friend QDataStream& operator<<(QDataStream & stream, const IntegerVector& flows);
	friend QDataStream& operator>>(QDataStream & stream, IntegerVector& flows);
};
QDataStream& operator<<(QDataStream & stream, const IntegerVector& flows);
QDataStream& operator>>(QDataStream & stream, IntegerVector& flows);
#endif // IntegerVector_h__