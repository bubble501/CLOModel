#include "BaseRateTable.h"
#include "BloombergVector.h"
QDataStream& operator<<(QDataStream & stream, const ConstantBaseRateTable& flows) { 
	stream << flows.Values << flows.UpdateDate; return stream; 
}
ConstantBaseRateTable::ConstantBaseRateTable(const ConstantBaseRateTable& a) 
	: Values(a.Values), AbstractBaseRateTable(a.UpdateDate) 
{}
ConstantBaseRateTable::ConstantBaseRateTable(const ForwardBaseRateTable& a)
	: AbstractBaseRateTable(a.GetUpdateDate())
{
	for (QHash<QString, BloombergVector>::ConstIterator i = a.GetValues().constBegin(); i != a.GetValues().constEnd(); i++) {
		Values.insert(i.key(), i.value().GetValue(0));
	}
}
ConstantBaseRateTable& ConstantBaseRateTable::operator = (const ConstantBaseRateTable& a) {
	Values = a.Values; UpdateDate = a.UpdateDate; return *this; 
}
ConstantBaseRateTable& ConstantBaseRateTable::operator += (const ConstantBaseRateTable& a) {
	for (QHash<QString, double>::ConstIterator i = a.Values.constBegin(); i != a.Values.constEnd(); i++) {
		if (!Values.contains(i.key())) Values.insert(i.key(), i.value());
	}
	if (!a.UpdateDate.isNull()) {
		if (!UpdateDate.isNull()) 
			UpdateDate = qMin(UpdateDate, a.UpdateDate);
		else UpdateDate = a.UpdateDate;
	}
	return *this;
}
ConstantBaseRateTable& ConstantBaseRateTable::operator += (const QHash<QString, double>& a) {
	for (QHash<QString, double>::ConstIterator i = a.constBegin(); i != a.constEnd(); i++) {
		if (!Values.contains(i.key())) Values.insert(i.key(), i.value());
	}
	return *this;
}
ConstantBaseRateTable& ConstantBaseRateTable::operator += (const ForwardBaseRateTable& a) {
	for (QHash<QString, BloombergVector>::ConstIterator i = a.GetValues().constBegin(); i != a.GetValues().constEnd(); i++) {
		if (!Values.contains(i.key())) Values.insert(i.key(), i.value().GetValue(0));
	}
	if (!a.GetUpdateDate().isNull()) {
		if (!UpdateDate.isNull())
			UpdateDate = qMin(UpdateDate, a.GetUpdateDate());
		else UpdateDate = a.GetUpdateDate();
	}
	return *this;
}
ConstantBaseRateTable& ConstantBaseRateTable::operator = (const ForwardBaseRateTable& a) {
	for (QHash<QString, BloombergVector>::ConstIterator i = a.GetValues().constBegin(); i != a.GetValues().constEnd(); i++) {
		Values.insert(i.key(), i.value().GetValue(0));
	}
	UpdateDate = a.GetUpdateDate();
	return *this;
}
QDataStream& ConstantBaseRateTable::LoadOldVersion(QDataStream& stream) {
	stream >> Values >> UpdateDate; ResetProtocolVersion(); return stream; 
}
QDataStream& operator>>(QDataStream & stream, ConstantBaseRateTable& flows) {
	return flows.LoadOldVersion(stream); 
}
ConstantBaseRateTable::ConstantBaseRateTable(const QHash<QString, double>& a, const QDate& upd) 
	:Values(a), AbstractBaseRateTable(upd)
{}
ConstantBaseRateTable& ConstantBaseRateTable::operator = (const QHash<QString, double>& a) {
	Values = a;
	return *this;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ForwardBaseRateTable::ForwardBaseRateTable(const ForwardBaseRateTable& a) 
	: Values(a.Values), AbstractBaseRateTable(a.UpdateDate) {}
ForwardBaseRateTable& ForwardBaseRateTable::operator = (const ForwardBaseRateTable& a) { 
	Values = a.Values; UpdateDate = a.UpdateDate; return *this; 
}
ForwardBaseRateTable::ForwardBaseRateTable(const QHash<QString, BloombergVector>& a, const QDate& upd)
	: Values(a), AbstractBaseRateTable(upd) {}
ForwardBaseRateTable& ForwardBaseRateTable::operator= (const QHash<QString, BloombergVector>& a) {
	Values = a;
	return *this;
}
ForwardBaseRateTable& ForwardBaseRateTable::operator= (const ConstantBaseRateTable& a) {
	for (QHash<QString, double>::ConstIterator i = a.GetValues().constBegin(); i != a.GetValues().constEnd(); i++) {
		Values.insert(i.key(), QString("%1").arg(i.value()));
	}
	UpdateDate = a.GetUpdateDate();
	return *this;
}
ForwardBaseRateTable::ForwardBaseRateTable(const ConstantBaseRateTable& a)
	: AbstractBaseRateTable(a.GetUpdateDate())
{
	for (QHash<QString, double>::ConstIterator i = a.GetValues().constBegin(); i != a.GetValues().constEnd(); i++) {
		Values.insert(i.key(), QString("%1").arg(i.value()));
	}
}
QDataStream& operator>>(QDataStream & stream, ForwardBaseRateTable& flows) {
	return flows.LoadOldVersion(stream); 
}
QDataStream& ForwardBaseRateTable::LoadOldVersion(QDataStream& stream) { 
	qint32 HashSize;
	QString TempKey;
	BloombergVector TempValue;
	stream >> HashSize;
	for (int i = 0; i < HashSize; i++) {
		stream >> TempKey;
		TempValue.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> TempValue;
		Values.insert(TempKey, TempValue);
	}
	ResetProtocolVersion(); return stream; 
}
QDataStream& operator<<(QDataStream & stream, const ForwardBaseRateTable& flows) { 
	stream << qint32(flows.Values.size());
	for (QHash<QString, BloombergVector>::const_iterator i = flows.Values.constBegin(); i != flows.Values.constEnd(); i++) 
		stream << i.key() << i.value();
	return stream;
}
ForwardBaseRateTable& ForwardBaseRateTable::operator += (const ForwardBaseRateTable& a) {
	for (QHash<QString, BloombergVector>::ConstIterator i = a.Values.constBegin(); i != a.Values.constEnd(); i++) {
		if (!Values.contains(i.key())) Values.insert(i.key(), i.value());
	}
	if (!a.UpdateDate.isNull()) {
		if (!UpdateDate.isNull())
			UpdateDate = qMin(UpdateDate, a.UpdateDate);
		else UpdateDate = a.UpdateDate;
	}
	return *this;
}
ForwardBaseRateTable& ForwardBaseRateTable::operator += (const QHash<QString, BloombergVector>& a) {
	for (QHash<QString, BloombergVector>::ConstIterator i = a.constBegin(); i != a.constEnd(); i++) {
		if (!Values.contains(i.key())) Values.insert(i.key(), i.value());
	}
	return *this;
}
ForwardBaseRateTable& ForwardBaseRateTable::operator+= (const ConstantBaseRateTable& a) {
	for (QHash<QString, double>::ConstIterator i = a.GetValues().constBegin(); i != a.GetValues().constEnd(); i++) {
		if (!Values.contains(i.key())) Values.insert(i.key(), QString("%1").arg(i.value()));
	}
	if (!a.GetUpdateDate().isNull()) {
		if (!UpdateDate.isNull())
			UpdateDate = qMin(UpdateDate, a.GetUpdateDate());
		else UpdateDate = a.GetUpdateDate();
	}
	return *this;
}
