
#ifndef AnnuityVector_h__
#define AnnuityVector_h__
#include <QList>
#include "AbstarctBbgVect.h"
class AnnuityVector : public AbstarctBbgVect
{
private:
	QList<char> m_VectVal;
protected:
	virtual void UnpackVector();
	virtual bool IsValid() const;
public:
	AnnuityVector(){}
	AnnuityVector(const AnnuityVector& Vec);
	AnnuityVector(const QString& Vec);
	AnnuityVector(const QString& Vec,const QDate& Anchor);
	char GetValue(const QDate& index) const;
	char GetValue(int index) const;
	AnnuityVector& operator=(const QString& a){AbstarctBbgVect::operator=(a); return *this;}
	friend QDataStream& operator<<(QDataStream & stream, const AnnuityVector& flows);
	friend QDataStream& operator>>(QDataStream & stream, AnnuityVector& flows);
};
#endif // AnnuityVector_h__


