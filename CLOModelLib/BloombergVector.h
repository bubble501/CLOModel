#ifndef BloombergVector_h__
#define BloombergVector_h__
#include <QList>
#include <QMetaType>
#include "AbstractBbgVect.h"
class FloorCapVector;
class BloombergVectorPrivate;
class CLOMODELLIB_EXPORT BloombergVector : public AbstractBbgVect
{
    DECLARE_PUBLIC_COMMONS(BloombergVector)
    DECLARE_PUBLIC_COMMONS_COPY(BloombergVector)
protected:
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
	virtual void UnpackVector();
	virtual bool IsValid() const override;
	virtual void RepackVector();
public:
	virtual QRegExpValidator* GetValidator(QObject* parent = 0) const override;
	QString BloombergSafeVector(QDate CurrentDate=QDate::currentDate()) const;
	using AbstractBbgVect::IsEmpty;
	virtual bool IsEmpty(double Lbound, double Ubound = std::numeric_limits<double>::max())const;
	static BloombergVector Combine(const BloombergVector& StartVect, const BloombergVector& SwitchVect, quint32 Periods);
	void Combine(const BloombergVector& SwitchVect, quint32 Periods);
	BloombergVector(const QString& Vec);
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
	virtual void ApplyFloorCap(const FloorCapVector& fc);
	int NumElements() const;
	void SetDivisor(double a);
	double GetDivisor() const;
	BloombergVector& operator=(const QString& a);
	BloombergVector operator+(const BloombergVector& Vec) const;
	BloombergVector operator+(double a) const;
	BloombergVector operator*(double a) const;
	BloombergVector operator/(double a) const;
	BloombergVector operator-(double a) const;
	BloombergVector& operator+=(const BloombergVector& a);
	BloombergVector& operator+=(double a);
	BloombergVector& operator-=(double a);
	BloombergVector& operator*=(double a);
	BloombergVector& operator/=(double a);
	friend QDataStream& operator<<(QDataStream & stream, const BloombergVector& flows);
	friend QDataStream& operator>>(QDataStream & stream, BloombergVector& flows);
};
QDataStream& operator<<(QDataStream & stream, const BloombergVector& flows);
QDataStream& operator>>(QDataStream & stream, BloombergVector& flows);
Q_DECLARE_METATYPE(BloombergVector)
#endif // BloombergVector_h__

