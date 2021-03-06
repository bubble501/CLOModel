#ifndef IntegerVector_h__
#define IntegerVector_h__
#include <QMetaType>
#include "AbstractBbgVect.h"
class IntegerVectorPrivate;
class IntegerVector;
CLOMODELLIB_EXPORT QDataStream&  operator<<(QDataStream & stream, const IntegerVector& flows);
CLOMODELLIB_EXPORT QDataStream&  operator>>(QDataStream & stream, IntegerVector& flows);
class CLOMODELLIB_EXPORT IntegerVector : public AbstractBbgVect
{
    DECLARE_PUBLIC_COMMONS(IntegerVector)
    DECLARE_PUBLIC_COMMONS_COPY(IntegerVector)
protected:
	virtual void UnpackVector();
	virtual bool IsValid() const override;
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
public:
	virtual QRegularExpressionValidator* GetValidator(QObject* parent = 0) const override;
	using AbstractBbgVect::IsEmpty;
	virtual bool IsEmpty(int Lbound, int Ubound=INT_MAX, bool IgnoreShift=false)const;
	IntegerVector(const QString& Vec);
	IntegerVector(const QString& Vec, const QDate& Anchor);
	int GetValue(const QDate& index) const;
	int GetValue(int index) const;
	int NumElements() const;
	IntegerVector& operator=(const QString& a);
	friend CLOMODELLIB_EXPORT QDataStream& operator<<(QDataStream & stream, const IntegerVector& flows);
	friend CLOMODELLIB_EXPORT QDataStream& operator>>(QDataStream & stream, IntegerVector& flows);
	int GetShift() const;
	void SetShift(int val);
};

Q_DECLARE_METATYPE(IntegerVector)
#endif // IntegerVector_h__