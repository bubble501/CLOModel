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
	virtual void RepackVector();
public:
	BloombergVector(){}
	BloombergVector(const QString& Vec);
	BloombergVector(const BloombergVector& Vec);
	BloombergVector(const QString& Vec,const QDate& Anchor);
	double GetValue(const QDate& index,int Frequency=12) const;
	double GetValue(int index,int Frequency=12) const;
	int NumElements() const {return m_VectVal.size();}
	BloombergVector& operator=(const QString& a){AbstarctBbgVect::operator=(a); return *this;}
	BloombergVector& operator=(const BloombergVector& Vec);
	BloombergVector operator+(const BloombergVector& Vec) const;
	BloombergVector operator+(double a) const;
	BloombergVector operator-(double a) const{return operator+(-a);}
	BloombergVector& operator+=(const BloombergVector& a){return operator=(operator+(a));}
	BloombergVector& operator+=(double a){return operator=(operator+(a));}
	BloombergVector& operator-=(double a){return operator=(operator-(a));}
	friend QDataStream& operator<<(QDataStream & stream, const BloombergVector& flows);
	friend QDataStream& operator>>(QDataStream & stream, BloombergVector& flows);
};
QDataStream& operator<<(QDataStream & stream, const BloombergVector& flows);
QDataStream& operator>>(QDataStream & stream, BloombergVector& flows);
#endif // BloombergVector_h__

