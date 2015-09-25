#ifndef FloorCapVector_h__
#define FloorCapVector_h__
#include "AbstractBbgVect.h"
#include <memory>
#include <QMetaType>
class FloorCapVectorPrivate;
class FloorCapVector;
CLOMODELLIB_EXPORT QDataStream&  operator<<(QDataStream & stream, const FloorCapVector& flows);
CLOMODELLIB_EXPORT QDataStream&  operator>>(QDataStream & stream, FloorCapVector& flows);
class CLOMODELLIB_EXPORT FloorCapVector : public AbstractBbgVect
{
    DECLARE_PUBLIC_COMMONS(FloorCapVector)
    DECLARE_PUBLIC_COMMONS_COPY(FloorCapVector)
protected:
	virtual void UnpackVector() override;
	virtual bool IsValid() const override;
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
public:
	virtual QRegularExpressionValidator* GetValidator(QObject* parent = 0) const override;
	using AbstractBbgVect::IsEmpty;
	FloorCapVector(const QString& Vec);
	FloorCapVector(const QString& Vec, const QDate& Anchor);
	std::shared_ptr<double> GetFloor(const QDate& index) const;
    std::shared_ptr<double> GetCap(const QDate& index) const;
    std::shared_ptr<double> GetFloor(int index) const;
    std::shared_ptr<double> GetCap(int index) const;
	int NumElements() const;
	FloorCapVector& operator=(const QString& a);
	friend CLOMODELLIB_EXPORT QDataStream& operator<<(QDataStream & stream, const FloorCapVector& flows);
	friend CLOMODELLIB_EXPORT QDataStream& operator>>(QDataStream & stream, FloorCapVector& flows);
};

Q_DECLARE_METATYPE(FloorCapVector)
#endif // FloorCapVector_h__

