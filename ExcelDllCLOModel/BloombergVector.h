#ifndef BloombergVector_h__
#define BloombergVector_h__
#include <QList>
#include <QDataStream>
#include "AbstarctBbgVect.h"
class BloombergVector : public AbstarctBbgVect
{
private:
	QList<double> m_VectVal;
protected:
	virtual void UnpackVector();
	virtual bool IsValid() const;
public:
	BloombergVector(){}
	BloombergVector(const QString& Vec);
	BloombergVector(const BloombergVector& Vec);
	BloombergVector(const QString& Vec,const QDate& Anchor);
	double GetValue(const QDate& index,int Frequency=12) const;
	double GetValue(int index,int Frequency=12) const;
	BloombergVector& operator=(const QString& a){AbstarctBbgVect::operator=(a); return *this;}
	friend QDataStream& operator<<(QDataStream & stream, const BloombergVector& flows);
	friend QDataStream& operator>>(QDataStream & stream, BloombergVector& flows);
};
QDataStream& operator<<(QDataStream & stream, const BloombergVector& flows);
QDataStream& operator>>(QDataStream & stream, BloombergVector& flows);
#endif // BloombergVector_h__

