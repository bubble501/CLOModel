#ifndef BaseRateVect_h__
#define BaseRateVect_h__
#include "AbstractBbgVect.h"
#include <QStringList>
#include <QString>
#include <QHash>
#include <QDataStream>
#include <type_traits>
#include "BloombergVector.h"
#include "BaseRateTable.h"
#include "FloorCapVector.h"
class BaseRateVector : public AbstractBbgVect{
public:
protected:
	QStringList m_VectVal;
	virtual void UnpackVector();
	virtual bool IsValid() const override;
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
public:
	virtual FloorCapVector ExtractFloorCapVector() const;
	BaseRateVector(){}
	BaseRateVector(const QString& Vec);
	BaseRateVector(const BaseRateVector& Vec);
	BaseRateVector(const QString& Vec,const QDate& Anchor);
	BaseRateVector& operator=(const BaseRateVector& Vec);
	virtual bool IsZero()const { return m_Vector.trimmed().toUpper() == "ZERO"; }
	virtual QRegExpValidator* GetValidator(QObject* parent = 0) const override;
	int NumElements() const {return m_VectVal.size();}
	template<class T> QString GetValue(const T& index) const {
		static_assert(std::is_same<T, QDate>::value || std::is_same<T, int>::value, "GetValue can be used only with int or QDate");
		QString RawVal = GetValueTemplate(m_VectVal, index, QString()).trimmed();
		RawVal.replace(QRegExp("\\[(?:-?\\d*\\.?\\d+)?(?:,-?\\d*\\.?\\d+)?\\]"), "");
		return RawVal;
	}

	template<class T> QString GetFloor(const T& index) const {
		static_assert(std::is_same<T, QDate>::value || std::is_same<T, int>::value, "GetFloor can be used only with int or QDate");
		QString RawVal = GetValueTemplate(m_VectVal, index, QString()).trimmed();
		QRegExp CaptureFloor("\\[(-?\\d*\\.?\\d+)?(?:,-?\\d*\\.?\\d+)?\\]");
		if (CaptureFloor.indexIn(RawVal) < 0) return QString();
		return CaptureFloor.cap(1);
	}
	template<class T> QString GetCap(const T& index) const {
		static_assert(std::is_same<T, QDate>::value || std::is_same<T, int>::value, "GetCap can be used only with int or QDate");
		QString RawVal = GetValueTemplate(m_VectVal, index, QString()).trimmed();
		QRegExp CaptureFloor("\\[(?:-?\\d*\\.?\\d+)?,(-?\\d*\\.?\\d+)?\\]");
		if (CaptureFloor.indexIn(RawVal) < 0) return QString();
		return CaptureFloor.cap(1);
	}
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