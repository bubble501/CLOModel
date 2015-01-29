#include "DeferredInterestTrigger.h"
#include <QDataStream>

QDataStream& DeferredInterestTrigger::WriteToStream(QDataStream& stream) const {
	stream
		<< m_TargetSeniority
		<< m_TargetSeniorityLevel
		<< m_TargetCoupon
		<< static_cast<quint8>(m_SizeSide)
		<< static_cast<quint8>(m_SenioritySide)
		<< static_cast<quint8>(m_CouponSide)
		<< m_TargetSize
		;
	return stream;
}
QDataStream& DeferredInterestTrigger::LoadOldVersion(QDataStream& stream) {
	quint8 TempChar;
	m_TargetSeniority.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_TargetSeniority;
	m_TargetSeniorityLevel.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_TargetSeniorityLevel;
	m_TargetCoupon.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_TargetCoupon;
	stream>> TempChar; m_SizeSide = static_cast<TriggerSizeSide>(TempChar);
	stream >> TempChar; m_SenioritySide = static_cast<TriggerSenioritySide>(TempChar);
	stream >> TempChar; m_CouponSide = static_cast<TriggerCouponSide>(TempChar);
	m_TargetSize.SetLoadProtocolVersion(m_LoadProtocolVersion); stream >> m_TargetSize;
	return AbstractTrigger::LoadOldVersion(stream);
}

QString DeferredInterestTrigger::ReadyToCalculate() const {
	if (
		m_TargetSeniority.IsEmpty(1)
		|| m_SizeSide == TriggerSizeSide::Invalid
		|| m_SenioritySide == TriggerSenioritySide::Invalid
		|| m_CouponSide == TriggerCouponSide::Invalid
		|| m_TargetSeniorityLevel.IsEmpty(0)
		|| m_TargetCoupon.IsEmpty(0)
		|| m_TargetSize.IsEmpty(0.0)
		)
		return "Trigger " + m_TriggerLabel + " is invalid\n";
	return QString();
}
DeferredInterestTrigger::DeferredInterestTrigger(const DeferredInterestTrigger& a)
	: m_TargetSeniorityLevel(a.m_TargetSeniorityLevel)
	, m_TargetSeniority(a.m_TargetSeniority)
	, m_SizeSide(a.m_SizeSide)
	, m_SenioritySide(a.m_SenioritySide)
	, m_CouponSide(a.m_CouponSide)
	, m_TargetSize(a.m_TargetSize)
	, m_TargetCoupon(a.m_TargetCoupon)
	, AbstractTrigger(a) {}

DeferredInterestTrigger::DeferredInterestTrigger(
	QString TargetSenior
	, QString TargetSeniorLevel
	, QString TargetSize
	, QString TargetCoupon
	, TriggerSizeSide szd
	, TriggerSenioritySide ssd 
	, TriggerCouponSide scd
	, const QString& lab
)
	:m_TargetSeniority(TargetSenior)
	, m_TargetSeniorityLevel(TargetSeniorLevel)
	, m_SenioritySide(ssd)
	, m_SizeSide(szd)
	, m_CouponSide(scd)
	, m_TargetSize(TargetSize)
	, m_TargetCoupon(TargetCoupon)
	, AbstractTrigger(AbstractTrigger::TriggerType::DeferredInterestTrigger, lab) {
	m_TargetSeniorityLevel.SetShift(-1);
	m_TargetCoupon.SetShift(-1);
}

DeferredInterestTrigger::DeferredInterestTrigger(const QString& lab)
	: DeferredInterestTrigger("0", "1", "0", "1", TriggerSizeSide::Invalid, TriggerSenioritySide::Invalid, TriggerCouponSide::Invalid,lab)
{}
	/*:m_TargetSeniority("0")
	, m_TargetSeniorityLevel("1")
	, m_TargetCoupon("0")
	, m_SenioritySide(TriggerSenioritySide::Invalid)
	, m_SizeSide(TriggerSizeSide::Invalid)
	, m_CouponSide(TriggerCouponSide::Invalid)
	, m_TargetSize("0")
	, AbstractTrigger(AbstractTrigger::TriggerType::DeferredInterestTrigger, lab) {
	m_TargetSeniorityLevel.SetShift(-1);
	m_TargetCoupon.SetShift(-1);
}*/

QString DeferredInterestTrigger::ToString() const {
	return AbstractTrigger::ToString()
		+ "\nTarget Seniority: " + m_TargetSeniority.GetVector()
		+ "\nTarget Seniority Level: " + m_TargetSeniorityLevel.GetVector()
		+ "\nTarget Coupon: " + m_TargetCoupon.GetVector()
		+ "\nSize Limit: " + m_TargetSize.GetVector()
		+ "\nSize Side: " + TriggerSizeSideToString(m_SizeSide)
		+ "\nSeniority Side: " + TriggerSenioritySideToString(m_SenioritySide)
		+ "\nCoupon Side: " + TriggerCouponSideToString(m_CouponSide)
	;
}
QString DeferredInterestTrigger::TriggerCouponSideToString(TriggerCouponSide a) const {
	switch (a) {
	case TriggerCouponSide::Invalid: return "Invalid";
	case TriggerCouponSide::Bigger: return "Bigger";
	case TriggerCouponSide::Smaller: return "Smaller";
	case TriggerCouponSide::Exactly: return "Exactly";
	case TriggerCouponSide::BiggerOrEqual: return "Bigger or Equal";
	case TriggerCouponSide::SmallerOrEqual: return "Smaller or Equal";
	default: return QString();
	}
}
QString DeferredInterestTrigger::TriggerSizeSideToString(TriggerSizeSide a) const {
	switch (a) {
	case TriggerSizeSide::Invalid: return "Invalid";
	case TriggerSizeSide::Bigger: return "Bigger";
	case TriggerSizeSide::Smaller: return "Smaller";
	case TriggerSizeSide::Exactly: return "Exactly";
	case TriggerSizeSide::BiggerOrEqual: return "Bigger or Equal";
	case TriggerSizeSide::SmallerOrEqual: return "Smaller or Equal";
	default: return QString();
	}
}
QString DeferredInterestTrigger::TriggerSenioritySideToString(TriggerSenioritySide a) const {
	switch (a) {
	case TriggerSenioritySide::Invalid: return "Invalid";
	case TriggerSenioritySide::Senior: return "Senior";
	case TriggerSenioritySide::Junior: return "Junior";
	case TriggerSenioritySide::Exactly: return "Exactly";
	case TriggerSenioritySide::SeniorOrEqual: return "Senior or Equal";
	case TriggerSenioritySide::JuniorOrEqual: return "Junior or Equal";
	default: return QString();
	}
}

bool DeferredInterestTrigger::HasAnchor() const {
	return !(
		m_TargetSize.GetAnchorDate().isNull()
		|| m_TargetSeniorityLevel.GetAnchorDate().isNull()
		|| m_TargetSeniority.GetAnchorDate().isNull()
		|| m_TargetCoupon.GetAnchorDate().isNull()
		);
}

void DeferredInterestTrigger::FillMissingAnchorDate(const QDate& a) {
	if (m_TargetSize.GetAnchorDate().isNull()) m_TargetSize.SetAnchorDate(a);
	if (m_TargetSeniorityLevel.GetAnchorDate().isNull()) m_TargetSeniorityLevel.SetAnchorDate(a);
	if (m_TargetSeniority.GetAnchorDate().isNull()) m_TargetSeniority.SetAnchorDate(a);
	if (m_TargetCoupon.GetAnchorDate().isNull()) m_TargetCoupon.SetAnchorDate(a);
}

void DeferredInterestTrigger::SetSizeMultiplier(double mult) {
	 ResetSizeMultiplier();
	 if (mult != 0.0) m_TargetSize *= mult;
}
