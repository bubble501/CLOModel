
#ifndef AnnuityVector_h__
#define AnnuityVector_h__
#include <QList>
#include "AbstractBbgVect.h"
class BoolVector : public AbstractBbgVect
{
private:
	QList<bool> m_VectVal;
protected:
	virtual void UnpackVector();
	virtual bool IsValid() const override;
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
public:
	BoolVector(){}
	BoolVector(const BoolVector& Vec);
	BoolVector(const QString& Vec);
	BoolVector(const QString& Vec,const QDate& Anchor);
	bool GetValue(const QDate& index) const;
	bool GetValue(int index) const;
	int NumElements() const {return m_VectVal.size();}
	BoolVector& operator=(const QString& a){AbstractBbgVect::operator=(a); return *this;}
	friend QDataStream& operator<<(QDataStream & stream, const BoolVector& flows);
	friend QDataStream& operator>>(QDataStream & stream, BoolVector& flows);
};
QDataStream& operator<<(QDataStream & stream, const BoolVector& flows);
QDataStream& operator>>(QDataStream & stream, BoolVector& flows);
Q_DECLARE_METATYPE(BoolVector)
#endif // AnnuityVector_h__


