#ifndef BaseRateTable_h__
#define BaseRateTable_h__
#include <QHash>
#include <QDate>
#include <QString>
#include <QDataStream>
#include "BackwardCompatibilityInterface.h"
#include "BloombergVector.h"
class ConstantBaseRateTable;
class ForwardBaseRateTable;
class AbstractBaseRateTable : public BackwardInterface {
protected:
	QDate UpdateDate;
public:
	AbstractBaseRateTable(const QDate& a) : UpdateDate(a) {}
	AbstractBaseRateTable() {}
	virtual const QDate& GetUpdateDate() const { return UpdateDate; }
	virtual void SetUpdateDate(const QDate& val) { UpdateDate = val; }
	virtual bool Contains(const QString& key) const = 0;
	virtual QList<QString> GetAvailableKeys() const = 0;
};
class ConstantBaseRateTable : public AbstractBaseRateTable {
public:
	ConstantBaseRateTable() {}
	ConstantBaseRateTable(const ConstantBaseRateTable& a);
	ConstantBaseRateTable(const ForwardBaseRateTable& a);
	ConstantBaseRateTable(const QHash<QString, double>& a, const QDate& upd=QDate());
	ConstantBaseRateTable& operator=(const ConstantBaseRateTable& a);
	ConstantBaseRateTable& operator=(const ForwardBaseRateTable& a);
	ConstantBaseRateTable& operator=(const QHash<QString, double>& a);
	ConstantBaseRateTable& operator+=(const ConstantBaseRateTable& a);
	ConstantBaseRateTable& operator+=(const ForwardBaseRateTable& a);
	ConstantBaseRateTable& operator+=(const QHash<QString, double>& a);
	virtual const QHash<QString, double>& GetValues() const { return Values; }
	virtual QHash<QString, double>& GetValues() { return Values; }
	virtual bool Contains(const QString& key) const override { return Values.contains(key); }
	virtual double GetValue(const QString& key) const { return Values.value(key, -1.0); }
	virtual void SetValue(const QString& key, double a) { Values[key] = a; }
	virtual QList<QString> GetAvailableKeys() const override { return Values.keys(); }
protected:
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
private:
	QHash<QString, double> Values;
	friend QDataStream& operator<<(QDataStream & stream, const ConstantBaseRateTable& flows);
	friend QDataStream& operator>>(QDataStream & stream, ConstantBaseRateTable& flows);

};
class ForwardBaseRateTable : public AbstractBaseRateTable {
public:
	ForwardBaseRateTable() {}
	ForwardBaseRateTable(const ForwardBaseRateTable& a);
	ForwardBaseRateTable(const ConstantBaseRateTable& a);
	ForwardBaseRateTable(const QHash<QString, BloombergVector>& a, const QDate& upd = QDate());
	ForwardBaseRateTable& operator=(const ForwardBaseRateTable& a);
	ForwardBaseRateTable& operator=(const ConstantBaseRateTable& a);
	ForwardBaseRateTable& operator=(const QHash<QString, BloombergVector>& a);
	ForwardBaseRateTable& operator+=(const ForwardBaseRateTable& a);
	ForwardBaseRateTable& operator+=(const ConstantBaseRateTable& a);
	ForwardBaseRateTable& operator+=(const QHash<QString, BloombergVector>& a);
	virtual const QHash<QString, BloombergVector>& GetValues() const { return Values; }
	virtual QHash<QString, BloombergVector>& GetValues() { return Values; }
	virtual bool Contains(const QString& key) const override { return Values.contains(key); }
	virtual BloombergVector GetValue(const QString& key) const { return Values.value(key, BloombergVector()); }
	virtual void SetValue(const QString& key, const BloombergVector& a) { if (!a.IsEmpty()) Values[key] = a; }
	virtual void SetValue(const QString& key, const QList<QDate>& RefDates, const QList<double>& Refvals) { SetValue(key, BloombergVector(RefDates, Refvals)); }
	virtual QList<QString> GetAvailableKeys() const override{ return Values.keys(); }
protected:
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
private:
	QHash<QString, BloombergVector> Values;
	friend QDataStream& operator<<(QDataStream & stream, const ForwardBaseRateTable& flows);
	friend QDataStream& operator>>(QDataStream & stream, ForwardBaseRateTable& flows);
};
QDataStream& operator<<(QDataStream & stream, const ForwardBaseRateTable& flows);
QDataStream& operator>>(QDataStream & stream, ForwardBaseRateTable& flows);
QDataStream& operator<<(QDataStream & stream, const ConstantBaseRateTable& flows);
QDataStream& operator>>(QDataStream & stream, ConstantBaseRateTable& flows);
#endif // BaseRateTable_h__
