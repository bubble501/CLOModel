#ifndef BoolVector_h__
#define BoolVector_h__
#include <QList>
#include <QMetaType>
#include "AbstractBbgVect.h"
class BoolVectorPrivate;
class CLOMODELLIB_EXPORT BoolVector : public AbstractBbgVect
{
    DECLARE_PUBLIC_COMMONS(BoolVector)
    DECLARE_PUBLIC_COMMONS_COPY(BoolVector)
protected:
	virtual void UnpackVector();
	virtual bool IsValid() const override;
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
public:
	virtual QRegExpValidator* GetValidator(QObject* parent = 0) const override;
	BoolVector(const QString& Vec);
	BoolVector(const QString& Vec,const QDate& Anchor);
	bool GetValue(const QDate& index) const;
	bool GetValue(int index) const;
	int NumElements() const;
	BoolVector& operator=(const QString& a);
	friend QDataStream& operator<<(QDataStream & stream, const BoolVector& flows);
	friend QDataStream& operator>>(QDataStream & stream, BoolVector& flows);
};
QDataStream& operator<<(QDataStream & stream, const BoolVector& flows);
QDataStream& operator>>(QDataStream & stream, BoolVector& flows);
Q_DECLARE_METATYPE(BoolVector)
#endif // BoolVector_h__


