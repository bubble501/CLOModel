#include "BaseRateTable.h"
#include "BaseRateTable_p.h"
#include "BloombergVector.h"
QDataStream& operator<<(QDataStream & stream, const ConstantBaseRateTable& flows) { 
    stream << flows.d_func()->Values << flows.d_func()->UpdateDate; 
    return stream;
}
ConstantBaseRateTable::ConstantBaseRateTable(const ConstantBaseRateTable& a) 
    :AbstractBaseRateTable(new ConstantBaseRateTablePrivate(this,*a.d_func()))
{}
ConstantBaseRateTable::ConstantBaseRateTable(const ForwardBaseRateTable& a)
	: AbstractBaseRateTable(a.GetUpdateDate())
{
    Q_D(ConstantBaseRateTable);
	for (QHash<QString, BloombergVector>::ConstIterator i = a.GetValues().constBegin(); i != a.GetValues().constEnd(); i++) {
		d->Values.insert(i.key(), i.value().GetValue(0));
	}
}

ConstantBaseRateTable::ConstantBaseRateTable()
    :AbstractBaseRateTable(new ConstantBaseRateTablePrivate(this))
{}

QHash<QString, double>& ConstantBaseRateTable::GetValues()
{
    Q_D(ConstantBaseRateTable);
    return d->Values;
}

const QHash<QString, double>& ConstantBaseRateTable::GetValues() const
{
    Q_D(const ConstantBaseRateTable);
    return d->Values;
}

bool ConstantBaseRateTable::Contains(const QString& key) const
{
    Q_D(const ConstantBaseRateTable);
    return d->Values.contains(key);
}

double ConstantBaseRateTable::GetValue(const QString& key) const
{
    Q_D(const ConstantBaseRateTable);
    return d->Values.value(key, -1.0);
}

void ConstantBaseRateTable::SetValue(const QString& key, double a)
{
    Q_D(ConstantBaseRateTable);
    d->Values[key] = a;
}

QList<QString> ConstantBaseRateTable::GetAvailableKeys() const
{
    Q_D(const ConstantBaseRateTable);
    return d->Values.keys();
}

void ConstantBaseRateTable::Clear()
{
    Q_D(ConstantBaseRateTable);
    d->Values.clear(); 
    SetUpdateDate(QDate());
}

ConstantBaseRateTable& ConstantBaseRateTable::operator= (const ConstantBaseRateTable& a)
{
    Q_D(ConstantBaseRateTable);
    d->operator=(*a.d_func());
    return *this; 
}
ConstantBaseRateTable& ConstantBaseRateTable::operator += (const ConstantBaseRateTable& a) {
    Q_D(ConstantBaseRateTable);
    for (QHash<QString, double>::ConstIterator i = a.d_func()->Values.constBegin(); i != a.d_func()->Values.constEnd(); i++) {
        if (!d->Values.contains(i.key()))
            d->Values.insert(i.key(), i.value());
	}
    if (!a.d_func()->UpdateDate.isNull()) {
        if (!d->UpdateDate.isNull())
            d->UpdateDate = qMin(d->UpdateDate, a.d_func()->UpdateDate);
        else 
            d->UpdateDate = a.d_func()->UpdateDate;
	}
	return *this;
}
ConstantBaseRateTable& ConstantBaseRateTable::operator += (const QHash<QString, double>& a) {
    Q_D(ConstantBaseRateTable);
	for (QHash<QString, double>::ConstIterator i = a.constBegin(); i != a.constEnd(); i++) {
        if (!d->Values.contains(i.key())) 
            d->Values.insert(i.key(), i.value());
	}
	return *this;
}
ConstantBaseRateTable& ConstantBaseRateTable::operator += (const ForwardBaseRateTable& a) {
    Q_D(ConstantBaseRateTable);
	for (QHash<QString, BloombergVector>::ConstIterator i = a.GetValues().constBegin(); i != a.GetValues().constEnd(); i++) {
        if (!d->Values.contains(i.key())) d->Values.insert(i.key(), i.value().GetValue(0));
	}
	if (!a.GetUpdateDate().isNull()) {
        if (!d->UpdateDate.isNull())
            d->UpdateDate = qMin(d->UpdateDate, a.GetUpdateDate());
        else d->UpdateDate = a.GetUpdateDate();
	}
	return *this;
}
ConstantBaseRateTable& ConstantBaseRateTable::operator = (const ForwardBaseRateTable& a) {
    Q_D(ConstantBaseRateTable);
	for (QHash<QString, BloombergVector>::ConstIterator i = a.GetValues().constBegin(); i != a.GetValues().constEnd(); i++) {
        d->Values.insert(i.key(), i.value().GetValue(0));
	}
    d->UpdateDate = a.GetUpdateDate();
	return *this;
}
QDataStream& ConstantBaseRateTable::LoadOldVersion(QDataStream& stream) {
    Q_D(ConstantBaseRateTable);
    stream >> d->Values >> d->UpdateDate; 
    ResetProtocolVersion(); 
    return stream;
}
QDataStream& operator>>(QDataStream & stream, ConstantBaseRateTable& flows) {
	return flows.LoadOldVersion(stream); 
}
ConstantBaseRateTable::ConstantBaseRateTable(const QHash<QString, double>& a, const QDate& upd) 
	:AbstractBaseRateTable(upd)
{
    Q_D(ConstantBaseRateTable);
    d->Values = a;
}
ConstantBaseRateTable& ConstantBaseRateTable::operator = (const QHash<QString, double>& a) {
    Q_D(ConstantBaseRateTable);
	d->Values = a;
	return *this;
}
ConstantBaseRateTablePrivate::ConstantBaseRateTablePrivate(ConstantBaseRateTable* q)
    :AbstractBaseRateTablePrivate(q)
{}

ConstantBaseRateTablePrivate::ConstantBaseRateTablePrivate(ConstantBaseRateTable* q, const ConstantBaseRateTablePrivate& other)
    : AbstractBaseRateTablePrivate(q, other)
    , Values(other.Values)
{}

ConstantBaseRateTablePrivate& ConstantBaseRateTablePrivate::operator=(const ConstantBaseRateTablePrivate& other)
{
    AbstractBaseRateTablePrivate::operator=(other);
    Values = other.Values;
    return *this;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ForwardBaseRateTable::ForwardBaseRateTable(const ForwardBaseRateTable& a) 
    :AbstractBaseRateTable(new ForwardBaseRateTablePrivate(this,*a.d_func()))
{}

const QHash<QString, BloombergVector>& ForwardBaseRateTable::GetValues() const
{
    Q_D(const ForwardBaseRateTable);
    return d->Values;
}

QHash<QString, BloombergVector>& ForwardBaseRateTable::GetValues()
{
    Q_D(ForwardBaseRateTable);
    return d->Values;
}

bool ForwardBaseRateTable::Contains(const QString& key) const
{
    Q_D(const ForwardBaseRateTable);
    return d->Values.contains(key);
}

BloombergVector ForwardBaseRateTable::GetValue(const QString& key) const
{
    Q_D(const ForwardBaseRateTable);
    return d->Values.value(key, BloombergVector());
}

void ForwardBaseRateTable::SetValue(const QString& key, const QList<QDate>& RefDates, const QList<double>& Refvals)
{
    Q_D(ForwardBaseRateTable);
    SetValue(key, BloombergVector(RefDates, Refvals));
}

void ForwardBaseRateTable::SetValue(const QString& key, const BloombergVector& a)
{
    Q_D(ForwardBaseRateTable);
    if (!a.IsEmpty()) 
        d->Values[key] = a;
}

QList<QString> ForwardBaseRateTable::GetAvailableKeys() const
{
    Q_D(const ForwardBaseRateTable);
    return d->Values.keys();
}

void ForwardBaseRateTable::Clear()
{
    Q_D(ForwardBaseRateTable);
    d->Values.clear(); 
    SetUpdateDate(QDate());
}

ForwardBaseRateTable& ForwardBaseRateTable::operator = (const ForwardBaseRateTable& a)
{
    Q_D(ForwardBaseRateTable);
    d->operator=(*a.d_func());
    return *this;
}
ForwardBaseRateTable::ForwardBaseRateTable(const QHash<QString, BloombergVector>& a, const QDate& upd)
	: AbstractBaseRateTable(upd) 
{
    Q_D(ForwardBaseRateTable);
    d->Values=a;
}
ForwardBaseRateTable& ForwardBaseRateTable::operator= (const QHash<QString, BloombergVector>& a) {
    Q_D(ForwardBaseRateTable);
    d->Values = a;
	return *this;
}
ForwardBaseRateTable& ForwardBaseRateTable::operator= (const ConstantBaseRateTable& a) {
    Q_D(ForwardBaseRateTable);
	for (QHash<QString, double>::ConstIterator i = a.GetValues().constBegin(); i != a.GetValues().constEnd(); i++) {
        d->Values.insert(i.key(), QString("%1").arg(i.value()));
	}
    d->UpdateDate = a.GetUpdateDate();
	return *this;
}
ForwardBaseRateTable::ForwardBaseRateTable(const ConstantBaseRateTable& a)
	: AbstractBaseRateTable(a.GetUpdateDate())
{
    Q_D(ForwardBaseRateTable);
	for (QHash<QString, double>::ConstIterator i = a.GetValues().constBegin(); i != a.GetValues().constEnd(); i++) {
        d->Values.insert(i.key(), QString("%1").arg(i.value()));
	}
}
QDataStream& operator>>(QDataStream & stream, ForwardBaseRateTable& flows) {
	return flows.LoadOldVersion(stream); 
}
QDataStream& ForwardBaseRateTable::LoadOldVersion(QDataStream& stream) { 
    Q_D(ForwardBaseRateTable);
	qint32 HashSize;
	QString TempKey;
	BloombergVector TempValue;
	stream >> HashSize;
	for (int i = 0; i < HashSize; i++) {
		stream >> TempKey;
        TempValue.SetLoadProtocolVersion(loadProtocolVersion()); 
        stream >> TempValue;
		d->Values.insert(TempKey, TempValue);
	}
	ResetProtocolVersion(); return stream; 
}
QDataStream& operator<<(QDataStream & stream, const ForwardBaseRateTable& flows) { 
	stream << qint32(flows.d_func()->Values.size());
    for (QHash<QString, BloombergVector>::const_iterator i = flows.d_func()->Values.constBegin(); i != flows.d_func()->Values.constEnd(); i++)
		stream << i.key() << i.value();
	return stream;
}
ForwardBaseRateTable& ForwardBaseRateTable::operator += (const ForwardBaseRateTable& a) {
    Q_D(ForwardBaseRateTable);
    for (QHash<QString, BloombergVector>::ConstIterator i = a.d_func()->Values.constBegin(); i != a.d_func()->Values.constEnd(); i++) {
        if (!d->Values.contains(i.key())) 
            d->Values.insert(i.key(), i.value());
	}
    if (!a.d_func()->UpdateDate.isNull()) {
        if (!d->UpdateDate.isNull())
            d->UpdateDate = qMin(d->UpdateDate, a.d_func()->UpdateDate);
        else
            d->UpdateDate = a.d_func()->UpdateDate;
	}
	return *this;
}
ForwardBaseRateTable& ForwardBaseRateTable::operator += (const QHash<QString, BloombergVector>& a) {
    Q_D(ForwardBaseRateTable);
	for (QHash<QString, BloombergVector>::ConstIterator i = a.constBegin(); i != a.constEnd(); i++) {
        if (!d->Values.contains(i.key())) 
            d->Values.insert(i.key(), i.value());
	}
	return *this;
}
ForwardBaseRateTable& ForwardBaseRateTable::operator+= (const ConstantBaseRateTable& a) {
    Q_D(ForwardBaseRateTable);
	for (QHash<QString, double>::ConstIterator i = a.GetValues().constBegin(); i != a.GetValues().constEnd(); i++) {
        if (!d->Values.contains(i.key()))
            d->Values.insert(i.key(), QString("%1").arg(i.value()));
	}
	if (!a.GetUpdateDate().isNull()) {
        if (!d->UpdateDate.isNull())
            d->UpdateDate = qMin(d->UpdateDate, a.GetUpdateDate());
        else d->UpdateDate = a.GetUpdateDate();
	}
	return *this;
}
ForwardBaseRateTablePrivate::ForwardBaseRateTablePrivate(ForwardBaseRateTable* q)
    :AbstractBaseRateTablePrivate(q)
{}

ForwardBaseRateTablePrivate::ForwardBaseRateTablePrivate(ForwardBaseRateTable* q, const ForwardBaseRateTablePrivate& other)
    : AbstractBaseRateTablePrivate(q, other)
    , Values(other.Values)
{}

ForwardBaseRateTablePrivate& ForwardBaseRateTablePrivate::operator=(const ForwardBaseRateTablePrivate& other)
{
    AbstractBaseRateTablePrivate::operator=(other);
    Values = other.Values;
    return *this;
}
//////////////////////////////////////////////////////////////////////////
AbstractBaseRateTable::AbstractBaseRateTable(const QDate& a) 
    : AbstractBaseRateTable()
{
    SetUpdateDate(a);
}

AbstractBaseRateTable::AbstractBaseRateTable()
    :BackwardInterface(new AbstractBaseRateTablePrivate(this))
{

}

AbstractBaseRateTable::AbstractBaseRateTable(AbstractBaseRateTablePrivate* d)
    :BackwardInterface(d)
{}

const QDate& AbstractBaseRateTable::GetUpdateDate() const
{
    Q_D(const AbstractBaseRateTable);
    return d->UpdateDate;
}

void AbstractBaseRateTable::SetUpdateDate(const QDate& val)
{
    Q_D(AbstractBaseRateTable);
    d->UpdateDate = val;
}

AbstractBaseRateTablePrivate::AbstractBaseRateTablePrivate(AbstractBaseRateTable* q)
    :BackwardInterfacePrivate(q)
{}

AbstractBaseRateTablePrivate::AbstractBaseRateTablePrivate(AbstractBaseRateTable* q, const AbstractBaseRateTablePrivate& other)
    : BackwardInterfacePrivate(q, other)
    , UpdateDate(other.UpdateDate)
{}

AbstractBaseRateTablePrivate& AbstractBaseRateTablePrivate::operator=(const AbstractBaseRateTablePrivate& other)
{
    BackwardInterfacePrivate::operator=(other);
    UpdateDate = other.UpdateDate;
    return *this;
}