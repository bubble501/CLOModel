#include "DateTrigger.h"
#include <QDataStream>
bool DateTrigger::Passing(const QDate& CurrentDate) const {
	if (m_LimitDate.isNull()) return false;
	bool Result = false;
	if (static_cast<quint8>(m_Side)& static_cast<quint8>(TriggerSide::BeforeExcluding)) {
		Result = Result || CurrentDate < m_LimitDate;
	}
	if (static_cast<quint8>(m_Side)& static_cast<quint8>(TriggerSide::AfterExcluding)) {
		Result = Result || CurrentDate > m_LimitDate;
	}
	if (static_cast<quint8>(m_Side)& static_cast<quint8>(TriggerSide::Exactly)) {
		Result = Result || CurrentDate == m_LimitDate;
	}
	return Result;
}

QDataStream& DateTrigger::WriteToStream(QDataStream& stream) const {
	stream
		<< m_LimitDate
		<< static_cast<quint8>(m_Side)
		;
	return stream;
}
QDataStream& DateTrigger::LoadOldVersion(QDataStream& stream) {
	quint8 TempChar;
	stream
		>> m_LimitDate
		>> TempChar
		;
	m_Side = static_cast<TriggerSide>(TempChar);
	return AbstractTrigger::LoadOldVersion(stream);
}

DateTrigger::DateTrigger(const QDate& dte, TriggerSide sd , const QString& lab ) 
	:AbstractTrigger(TriggerType::DateTrigger, lab)
	, m_Side(sd)
	, m_LimitDate(dte) 
{

}

DateTrigger::DateTrigger(const QString& lab) 
	:AbstractTrigger(TriggerType::DateTrigger, lab)
	, m_Side(TriggerSide::Invalid) 
{

}
QString DateTrigger::ReadyToCalculate() const {
	if (m_LimitDate.isNull() || m_Side == TriggerSide::Invalid) return "Trigger " + m_TriggerLabel + " is invalid\n";
	return QString();
}