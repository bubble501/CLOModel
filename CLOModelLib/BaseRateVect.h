#ifndef BaseRateVect_h__
#define BaseRateVect_h__
#include "AbstractBbgVect.h"
#include <QString>
#include <QDataStream>
#include "BloombergVector.h"
#include "BaseRateTable.h"
#include "FloorCapVector.h"
class BaseRateVectorPrivate;
class CLOMODELLIB_EXPORT  BaseRateVector : public AbstractBbgVect
{
    Q_DECLARE_PRIVATE(BaseRateVector)
protected:
	virtual void UnpackVector() override;
	virtual bool IsValid() const override;
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
    virtual void RepackVector();
public:
    virtual int replaceValue(const QString& oldVal, BaseRateVector newVal, bool replaceFloorCaps = true);
	virtual FloorCapVector ExtractFloorCapVector() const;
    BaseRateVector();
	BaseRateVector(const QString& Vec);
	BaseRateVector(const BaseRateVector& Vec);
	BaseRateVector(const QString& Vec,const QDate& Anchor);
	BaseRateVector& operator=(const BaseRateVector& Vec);
	virtual bool IsZero()const;
	virtual QRegExpValidator* GetValidator(QObject* parent = 0) const override;
	int NumElements() const;
    QString GetValue(const QDate& index) const;
    QString GetValue(int index) const;
    QString GetFloor(const QDate& index) const;
    QString GetFloor(int index) const;
    QString GetCap(const QDate& index) const;
    QString GetCap(int index) const;
	BloombergVector CompileReferenceRateValue(ConstantBaseRateTable& Values) const;
	BloombergVector CompileReferenceRateValue(ForwardBaseRateTable& Values) const;
	BloombergVector GetRefRateValueFromBloomberg(ConstantBaseRateTable& Values)const;
	BloombergVector GetRefRateValueFromBloomberg(ForwardBaseRateTable& Values)const;
	BloombergVector GetBaseRatesDatabase(ConstantBaseRateTable& ReferencesValues, bool DownloadAll=false) const;
	BloombergVector GetBaseRatesDatabase(ForwardBaseRateTable& ReferencesValues, bool DownloadAll=false) const;
	BaseRateVector& operator=(const QString& a);
	friend QDataStream& operator<<(QDataStream & stream, const BaseRateVector& flows);
	friend QDataStream& operator>>(QDataStream & stream, BaseRateVector& flows);
};
QDataStream& operator<<(QDataStream & stream, const BaseRateVector& flows);
QDataStream& operator>>(QDataStream & stream, BaseRateVector& flows);
Q_DECLARE_METATYPE(BaseRateVector)
#endif // BaseRateVect_h__