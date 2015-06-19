#ifndef DayCountVect_h__
#define DayCountVect_h__
#include <QList>
#include <QMetaType>
#include <QDate>
#include "AbstractBbgVect.h"
#include "CommonFunctions.h"
class DayCountVectorPrivate;
class CLOMODELLIB_EXPORT DayCountVector : public AbstractBbgVect
{
    DECLARE_PUBLIC_COMMONS(DayCountVector)
    DECLARE_PUBLIC_COMMONS_COPY(DayCountVector)
protected:
	virtual void UnpackVector() override;
	virtual bool IsValid() const override;
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
public:
	virtual QRegExpValidator* GetValidator(QObject* parent = 0) const override;
	using AbstractBbgVect::IsEmpty;
	DayCountVector(const QString& Vec);
	DayCountVector(const QString& Vec, const QDate& Anchor);
	DayCountVector(DayCountConvention a,const QDate& index=QDate());
	DayCountConvention GetValue(const QDate& index) const;
	DayCountConvention GetValue(int index) const;
	int NumElements() const;
	DayCountVector& operator=(const QString& a);
	DayCountVector& operator=(const DayCountVector& Vec);
	friend QDataStream& operator<<(QDataStream & stream, const DayCountVector& flows);
	friend QDataStream& operator>>(QDataStream & stream, DayCountVector& flows);
};
QDataStream& operator<<(QDataStream & stream, const DayCountVector& flows);
QDataStream& operator>>(QDataStream & stream, DayCountVector& flows);
Q_DECLARE_METATYPE(DayCountVector)
#endif // DayCountVect_h__

