#ifndef IntegerVector_h__
#define IntegerVector_h__
#include <QList>
#include "AbstractBbgVect.h"
class IntegerVector : public AbstractBbgVect {
protected:
	QList<int> m_VectVal;
	qint32 m_Shift;
	virtual void UnpackVector();
	virtual bool IsValid() const override;
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
public:
	virtual QRegExpValidator* GetValidator(QObject* parent = 0) const override;
	using AbstractBbgVect::IsEmpty;
	virtual bool IsEmpty(int Lbound, int Ubound=INT_MAX, bool IgnoreShift=false)const;
	IntegerVector(): m_Shift(0) { RegisterAsMetaType<IntegerVector>(); }
	IntegerVector(const IntegerVector& Vec);
	IntegerVector(const QString& Vec);
	IntegerVector(const QString& Vec, const QDate& Anchor);
	int GetValue(const QDate& index) const;
	int GetValue(int index) const;
	int NumElements() const { return m_VectVal.size(); }
	IntegerVector& operator=(const QString& a) { AbstractBbgVect::operator=(a); return *this; }
	IntegerVector& operator=(const IntegerVector& Vec);
	friend QDataStream& operator<<(QDataStream & stream, const IntegerVector& flows);
	friend QDataStream& operator>>(QDataStream & stream, IntegerVector& flows);
	int GetShift() const { return m_Shift; }
	void SetShift(int val) { m_Shift = val; }
};
QDataStream& operator<<(QDataStream & stream, const IntegerVector& flows);
QDataStream& operator>>(QDataStream & stream, IntegerVector& flows);
Q_DECLARE_METATYPE(IntegerVector)
#endif // IntegerVector_h__