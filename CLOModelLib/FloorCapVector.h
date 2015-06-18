#ifndef FloorCapVector_h__
#define FloorCapVector_h__
#include <QList>
#include "AbstractBbgVect.h"
#include <QSharedPointer>
#include <QMetaType>
class FloorCapVectorPrivate;
class CLOMODELLIB_EXPORT FloorCapVector : public AbstractBbgVect
{
protected:
    Q_DECLARE_PRIVATE(FloorCapVector)
	virtual void UnpackVector() override;
	virtual bool IsValid() const override;
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
public:
	virtual QRegExpValidator* GetValidator(QObject* parent = 0) const override;
	using AbstractBbgVect::IsEmpty;
    FloorCapVector();
	FloorCapVector(const FloorCapVector& Vec);
	FloorCapVector(const QString& Vec);
	FloorCapVector(const QString& Vec, const QDate& Anchor);
	QSharedPointer<double> GetFloor(const QDate& index) const;
    QSharedPointer<double> GetCap(const QDate& index) const;
    QSharedPointer<double> GetFloor(int index) const;
    QSharedPointer<double> GetCap(int index) const;
	int NumElements() const;
	FloorCapVector& operator=(const QString& a);
	FloorCapVector& operator=(const FloorCapVector& Vec);
	friend QDataStream& operator<<(QDataStream & stream, const FloorCapVector& flows);
	friend QDataStream& operator>>(QDataStream & stream, FloorCapVector& flows);
};
QDataStream& operator<<(QDataStream & stream, const FloorCapVector& flows);
QDataStream& operator>>(QDataStream & stream, FloorCapVector& flows);
Q_DECLARE_METATYPE(FloorCapVector)
#endif // FloorCapVector_h__

