#ifndef BaseRateTable_h__
#define BaseRateTable_h__
#include <QHash>
#include "BackwardCompatibilityInterface.h"
#include "BloombergVector.h"
class QDate;
class ConstantBaseRateTable;
class ForwardBaseRateTable;
class ConstantBaseRateTablePrivate;
class ForwardBaseRateTablePrivate;
class AbstractBaseRateTablePrivate;
class CLOMODELLIB_EXPORT AbstractBaseRateTable : public BackwardInterface
{
    DECLARE_PUBLIC_COMMONS(AbstractBaseRateTable)
    DECLARE_PUBLIC_COMMONS_COPY(AbstractBaseRateTable)
public:
	AbstractBaseRateTable(const QDate& a);
	virtual const QDate& GetUpdateDate() const;
	virtual void SetUpdateDate(const QDate& val);
	virtual bool Contains(const QString& key) const = 0;
	virtual QList<QString> GetAvailableKeys() const = 0;
    virtual void Clear() = 0;
};
class CLOMODELLIB_EXPORT ConstantBaseRateTable : public AbstractBaseRateTable
{
    DECLARE_PUBLIC_COMMONS(ConstantBaseRateTable)
    DECLARE_PUBLIC_COMMONS_COPY(ConstantBaseRateTable)
public:
	ConstantBaseRateTable(const ForwardBaseRateTable& a);
	ConstantBaseRateTable(const QHash<QString, double>& a, const QDate& upd=QDate());
	ConstantBaseRateTable& operator=(const ForwardBaseRateTable& a);
	ConstantBaseRateTable& operator=(const QHash<QString, double>& a);
	ConstantBaseRateTable& operator+=(const ConstantBaseRateTable& a);
	ConstantBaseRateTable& operator+=(const ForwardBaseRateTable& a);
	ConstantBaseRateTable& operator+=(const QHash<QString, double>& a);
	virtual const QHash<QString, double>& GetValues() const;
	virtual QHash<QString, double>& GetValues();
	virtual bool Contains(const QString& key) const override;
	virtual double GetValue(const QString& key) const;
	virtual void SetValue(const QString& key, double a);
	virtual QList<QString> GetAvailableKeys() const override;
    virtual void Clear() override;
protected:
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
private:
	friend QDataStream& operator<<(QDataStream & stream, const ConstantBaseRateTable& flows);
	friend QDataStream& operator>>(QDataStream & stream, ConstantBaseRateTable& flows);

};
class CLOMODELLIB_EXPORT  ForwardBaseRateTable : public AbstractBaseRateTable
{
    DECLARE_PUBLIC_COMMONS(ForwardBaseRateTable)
    DECLARE_PUBLIC_COMMONS_COPY(ForwardBaseRateTable)
public:
	ForwardBaseRateTable(const ConstantBaseRateTable& a);
	ForwardBaseRateTable(const QHash<QString, BloombergVector>& a, const QDate& upd = QDate());
	ForwardBaseRateTable& operator=(const ConstantBaseRateTable& a);
	ForwardBaseRateTable& operator=(const QHash<QString, BloombergVector>& a);
	ForwardBaseRateTable& operator+=(const ForwardBaseRateTable& a);
	ForwardBaseRateTable& operator+=(const ConstantBaseRateTable& a);
	ForwardBaseRateTable& operator+=(const QHash<QString, BloombergVector>& a);
	virtual const QHash<QString, BloombergVector>& GetValues() const;
	virtual QHash<QString, BloombergVector>& GetValues();
	virtual bool Contains(const QString& key) const override;
	virtual BloombergVector GetValue(const QString& key) const;
	virtual void SetValue(const QString& key, const BloombergVector& a);
	virtual void SetValue(const QString& key, const QList<QDate>& RefDates, const QList<double>& Refvals);
	virtual QList<QString> GetAvailableKeys() const override;
    virtual void Clear() override;
protected:
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
private:
	friend QDataStream& operator<<(QDataStream & stream, const ForwardBaseRateTable& flows);
	friend QDataStream& operator>>(QDataStream & stream, ForwardBaseRateTable& flows);
};
QDataStream& operator<<(QDataStream & stream, const ForwardBaseRateTable& flows);
QDataStream& operator>>(QDataStream & stream, ForwardBaseRateTable& flows);
QDataStream& operator<<(QDataStream & stream, const ConstantBaseRateTable& flows);
QDataStream& operator>>(QDataStream & stream, ConstantBaseRateTable& flows);
#endif // BaseRateTable_h__
