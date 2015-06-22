#ifndef RepaymentVector_h__
#define RepaymentVector_h__
#include <QList>
#include <QMetaType>
#include "AbstractBbgVect.h"
class RepaymentVectorPrivate;
class CLOMODELLIB_EXPORT RepaymentVector : public AbstractBbgVect
{
    DECLARE_PUBLIC_COMMONS(RepaymentVector)
    DECLARE_PUBLIC_COMMONS_COPY(RepaymentVector)
public:
	enum RepaymentMethods : qint8 {
		Invalid=-1
		, InterestOnly = 0x1
		, Annuity = 0x2
		, Linear = 0x4
		, Capitalization = 0x8
	};
protected:
	virtual QRegExpValidator* GetValidator(QObject* parent = 0) const override;
	virtual void UnpackVector() override;
	virtual bool IsValid() const override;
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
public:
	RepaymentVector(const QString& Vec);
	RepaymentVector(const QString& Vec, const QDate& Anchor);
	RepaymentMethods GetValue(const QDate& index) const;
	RepaymentMethods GetValue(int index) const;
	QString GetValueString(const QDate& index) const;
	QString GetValueString(int index) const;
	int NumElements() const;
	RepaymentVector& operator=(const QString& a);
	friend QDataStream& operator<<(QDataStream & stream, const RepaymentVector& flows);
	friend QDataStream& operator>>(QDataStream & stream, RepaymentVector& flows);
};
QDataStream& operator<<(QDataStream & stream, const RepaymentVector& flows);
QDataStream& operator>>(QDataStream & stream, RepaymentVector& flows);
Q_DECLARE_METATYPE(RepaymentVector)
#endif // RepaymentVector_h__