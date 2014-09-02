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
class ConstantBaseRateTable : public BackwardInterface {
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
	const QHash<QString, double>& GetValues() const { return Values; }
	QHash<QString, double>& GetValues() { return Values; }
	bool Contains(const QString& key) const { return Values.contains(key); }
	double GetValue(const QString& key) const { return Values.value(key, -1.0); }
	void SetValue(const QString& key, double a) { Values[key] = a; }
	const QDate& GetUpdateDate() const { return UpdateDate; }
	void SetUpdateDate(const QDate& val) { UpdateDate = val; }
	QList<QString> GetAvailableKeys() const { return Values.keys(); }
protected:
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
private:
	QHash<QString, double> Values;
	QDate UpdateDate;
	friend QDataStream& operator<<(QDataStream & stream, const ConstantBaseRateTable& flows);
	friend QDataStream& operator>>(QDataStream & stream, ConstantBaseRateTable& flows);

};
class ForwardBaseRateTable : public BackwardInterface {
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
	const QHash<QString, BloombergVector>& GetValues() const { return Values; }
	QHash<QString, BloombergVector>& GetValues() { return Values; }
	bool Contains(const QString& key) const { return Values.contains(key); }
	BloombergVector GetValue(const QString& key) const { return Values.value(key, BloombergVector()); }
	void SetValue(const QString& key, const BloombergVector& a) { Values[key] = a; }
	const QDate& GetUpdateDate() const { return UpdateDate; }
	void SetUpdateDate(const QDate& val) { UpdateDate = val; }
	QList<QString> GetAvailableKeys() const { return Values.keys(); }
protected:
	virtual QDataStream& LoadOldVersion(QDataStream& stream) override;
private:
	QHash<QString, BloombergVector> Values;
	QDate UpdateDate;
	friend QDataStream& operator<<(QDataStream & stream, const ForwardBaseRateTable& flows);
	friend QDataStream& operator>>(QDataStream & stream, ForwardBaseRateTable& flows);
};
QDataStream& operator<<(QDataStream & stream, const ForwardBaseRateTable& flows);
QDataStream& operator>>(QDataStream & stream, ForwardBaseRateTable& flows);
QDataStream& operator<<(QDataStream & stream, const ConstantBaseRateTable& flows);
QDataStream& operator>>(QDataStream & stream, ConstantBaseRateTable& flows);
#endif // BaseRateTable_h__
