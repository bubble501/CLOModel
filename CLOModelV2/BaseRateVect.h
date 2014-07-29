#ifndef BaseRateVect_h__
#define BaseRateVect_h__
#include "AbstarctBbgVect.h"
#include <QList>
#include <QString>
#include <QHash>
#include <QDataStream>
#include "BloombergVector.h"
#include "BaseRateTable.h"
class BaseRateVector : public AbstarctBbgVect{
public:
	enum AvailableRates{ //Do NOT assign values to entries
		Invalid=-1,
		ZeroFlat,
		EUR001W,
		EUR002W,
		EUR003W,
		EUR001M,
		EUR002M,
		EUR003M,
		EUR004M,
		EUR005M,
		EUR006M,
		EUR007M,
		EUR008M,
		EUR009M,
		EUR010M,
		EUR011M,
		EUR012M,
		BP00ON,
		BP0001W,
		BP0001M,
		BP0002M,
		BP0003M,
		BP0006M,
		BP0012M,
		US00ON,
		US0001W,
		US0001M,
		US0002M,
		US0003M,
		US0006M,
		US0012M,
		LIBOR01W,
		LIBOR01M,
		LIBOR02M,
		LIBOR03M,
		LIBOR06M,
		LIBOR12M,
	};
protected:
	QList<AvailableRates> m_VectVal;
	QString AvailableRatesToString(int a) const;
	QString AvailableRatesToString(AvailableRates a) const { return AvailableRatesToString(static_cast<int>(a)); }
	AvailableRates StringToAvailableRates(const QString& a) const;
	virtual void UnpackVector();
	virtual bool IsValid() const;
	virtual QDataStream& LoadOldVersion(QDataStream& stream);
public:
	BaseRateVector(){}
	BaseRateVector(const QString& Vec);
	BaseRateVector(const BaseRateVector& Vec);
	BaseRateVector(const QString& Vec,const QDate& Anchor);
	BaseRateVector& operator=(const BaseRateVector& Vec);
	int NumElements() const {return m_VectVal.size();}
	AvailableRates GetValue(const QDate& index) const;
	AvailableRates GetValue(int index) const;
	QString GetValueString(const QDate& index) const{return AvailableRatesToString(GetValue(index));}
	QString GetValueString(int index) const{return AvailableRatesToString(GetValue(index));}
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
	BaseRateVector& operator=(const QString& a){AbstarctBbgVect::operator=(a); return *this;}
	friend QDataStream& operator<<(QDataStream & stream, const BaseRateVector& flows);
	friend QDataStream& operator>>(QDataStream & stream, BaseRateVector& flows);
};
QDataStream& operator<<(QDataStream & stream, const BaseRateVector& flows);
QDataStream& operator>>(QDataStream & stream, BaseRateVector& flows);
#endif // BaseRateVect_h__