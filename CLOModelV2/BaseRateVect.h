#ifndef BaseRateVect_h__
#define BaseRateVect_h__
#include "AbstractBbgVect.h"
#include <QStringList>
#include <QString>
#include <QHash>
#include <QDataStream>
#include "BloombergVector.h"
#include "BaseRateTable.h"
class BaseRateVector : public AbstractBbgVect{
public:
protected:
	QStringList m_VectVal;
	virtual void UnpackVector();
	virtual bool IsValid() const override;
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
public:
	BaseRateVector(){}
	BaseRateVector(const QString& Vec);
	BaseRateVector(const BaseRateVector& Vec);
	BaseRateVector(const QString& Vec,const QDate& Anchor);
	BaseRateVector& operator=(const BaseRateVector& Vec);
	virtual QRegExpValidator* GetValidator(QObject* parent = 0) const override;
	int NumElements() const {return m_VectVal.size();}
	QString GetValue(const QDate& index) const;
	QString GetValue(int index) const;
	BloombergVector CompileReferenceRateValue(ConstantBaseRateTable& Values) const;
	BloombergVector CompileReferenceRateValue(ForwardBaseRateTable& Values) const;
#ifndef NO_BLOOMBERG
	BloombergVector GetRefRateValueFromBloomberg(ConstantBaseRateTable& Values)const;
	BloombergVector GetRefRateValueFromBloomberg(ForwardBaseRateTable& Values)const;
#endif
#ifndef NO_DATABASE
	BloombergVector GetBaseRatesDatabase(ConstantBaseRateTable& ReferencesValues, bool DownloadAll=false) const;
	BloombergVector GetBaseRatesDatabase(ForwardBaseRateTable& ReferencesValues, bool DownloadAll=false) const;
#endif 
	BaseRateVector& operator=(const QString& a){AbstractBbgVect::operator=(a); return *this;}
	friend QDataStream& operator<<(QDataStream & stream, const BaseRateVector& flows);
	friend QDataStream& operator>>(QDataStream & stream, BaseRateVector& flows);
};
QDataStream& operator<<(QDataStream & stream, const BaseRateVector& flows);
QDataStream& operator>>(QDataStream & stream, BaseRateVector& flows);
Q_DECLARE_METATYPE(BaseRateVector)
#endif // BaseRateVect_h__