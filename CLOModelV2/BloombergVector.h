#ifndef BloombergVector_h__
#define BloombergVector_h__
#include <QList>
#include "AbstractBbgVect.h"
class BloombergVector : public AbstractBbgVect
{
protected:
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
	QList<double> m_VectVal;
	double m_Divisor;
	virtual void UnpackVector();
	virtual bool IsValid() const override;
	virtual void RepackVector();
public:
	virtual QRegExpValidator* GetValidator(QObject* parent = 0) const override;
	QString BloombergSafeVector(QDate CurrentDate=QDate::currentDate()) const;
	using AbstractBbgVect::IsEmpty;
	virtual bool IsEmpty(double Lbound, double Ubound = std::numeric_limits<double>::max())const;
	static BloombergVector Combine(const BloombergVector& StartVect, const BloombergVector& SwitchVect, quint32 Periods);
	void Combine(const BloombergVector& SwitchVect, quint32 Periods) { operator=(Combine(*this, SwitchVect, Periods)); }
	BloombergVector();
	BloombergVector(const QString& Vec);
	BloombergVector(const BloombergVector& Vec);
	BloombergVector(const QString& Vec,const QDate& Anchor);
	BloombergVector(const QList<double>& Values, const QDate& Anchor = QDate());
	BloombergVector(const QList<QDate>& Dates, const QList<double>& Values);
	using::AbstractBbgVect::SetVector;
	bool SetVector(const QList<double>& Values, const QDate& Anchor);
	bool SetVector(const QList<double>& Values);
	bool SetVector(const QList<QDate>& Dates, const QList<double>& Values);
	virtual double GetValue(const QDate& index,int Frequency=12) const;
	virtual double GetValue(int index,int Frequency=12) const;
	virtual double GetSMM(const QDate& index, int Frequency = 12) const;
	virtual double GetSMM(int index, int Frequency = 12) const;
	int NumElements() const {return m_VectVal.size();}
	void SetDivisor(double a) { if (a > 0.0) m_Divisor = a; UnpackVector(); }
	BloombergVector& operator=(const QString& a){AbstractBbgVect::operator=(a); return *this;}
	BloombergVector& operator=(const BloombergVector& Vec);
	BloombergVector operator+(const BloombergVector& Vec) const;
	BloombergVector operator+(double a) const;
	BloombergVector operator*(double a) const { BloombergVector Result(*this); Result.SetDivisor(a / m_Divisor); return Result; }
	BloombergVector operator/(double a) const { BloombergVector Result(*this); Result.SetDivisor(a * m_Divisor); return Result; }
	BloombergVector operator-(double a) const{return operator+(-a);}
	BloombergVector& operator+=(const BloombergVector& a){return operator=(operator+(a));}
	BloombergVector& operator+=(double a){return operator=(operator+(a));}
	BloombergVector& operator-=(double a){return operator+=(-a);}
	BloombergVector& operator*=(double a) { SetDivisor(a / m_Divisor); return *this; }
	BloombergVector& operator/=(double a) { SetDivisor(a * m_Divisor); return *this; }
	friend QDataStream& operator<<(QDataStream & stream, const BloombergVector& flows);
	friend QDataStream& operator>>(QDataStream & stream, BloombergVector& flows);
};
QDataStream& operator<<(QDataStream & stream, const BloombergVector& flows);
QDataStream& operator>>(QDataStream & stream, BloombergVector& flows);
Q_DECLARE_METATYPE(BloombergVector)
#endif // BloombergVector_h__

