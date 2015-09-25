#include "DeferredInterestTrigger.h"
#include "Private/DeferredInterestTrigger_p.h"
#include <QDataStream>
#include "Private/DeferredInterestTriggerSettingWidget.h"
DEFINE_PUBLIC_COMMONS(DeferredInterestTrigger)
DEFINE_PUBLIC_COMMONS_COPY(DeferredInterestTrigger)
DeferredInterestTrigger::~DeferredInterestTrigger()
{

}
DeferredInterestTriggerPrivate::~DeferredInterestTriggerPrivate()
{

}
DeferredInterestTriggerPrivate::DeferredInterestTriggerPrivate(DeferredInterestTrigger *q,const DeferredInterestTriggerPrivate& other)
	:AbstractTriggerPrivate(q,other)
    , m_TargetSeniorityLevel(other.m_TargetSeniorityLevel)
    , m_TargetSeniority(other.m_TargetSeniority)
    , m_SizeSide(other.m_SizeSide)
    , m_SenioritySide(other.m_SenioritySide)
    , m_CouponSide(other.m_CouponSide)
    , m_TargetSize(other.m_TargetSize)
    , m_TargetCoupon(other.m_TargetCoupon)
{}
DeferredInterestTriggerPrivate::DeferredInterestTriggerPrivate(DeferredInterestTrigger *q)
	:AbstractTriggerPrivate(q)
    , m_TargetSeniority("0")
    , m_TargetSeniorityLevel("1")
    , m_TargetSize("0")
    , m_TargetCoupon("1")
    , m_SenioritySide(DeferredInterestTrigger::TriggerSenioritySide::Invalid)
    , m_SizeSide(DeferredInterestTrigger::TriggerSizeSide::Invalid)
    , m_CouponSide(DeferredInterestTrigger::TriggerCouponSide::Invalid)
{
    m_TargetSeniorityLevel.SetShift(-1);
    m_TargetCoupon.SetShift(-1);
}

DeferredInterestTrigger::DeferredInterestTrigger(DeferredInterestTriggerPrivate *d, const DeferredInterestTrigger& other)
	:AbstractTrigger(d,other)
{}
DeferredInterestTrigger& DeferredInterestTrigger::operator=(const DeferredInterestTrigger& other){
	Q_D(DeferredInterestTrigger);
	AbstractTrigger::operator=(other);
    d->m_TargetSeniority = other.d_func()->m_TargetSeniority;
    d->m_TargetSeniorityLevel = other.d_func()->m_TargetSeniorityLevel;
    d->m_TargetSize = other.d_func()->m_TargetSize;
    d->m_TargetCoupon = other.d_func()->m_TargetCoupon;
    d->m_SenioritySide = other.d_func()->m_SenioritySide;
    d->m_SizeSide = other.d_func()->m_SizeSide;
    d->m_CouponSide = other.d_func()->m_CouponSide;
	return *this;
}
DeferredInterestTrigger::DeferredInterestTrigger(DeferredInterestTriggerPrivate *d)
    :AbstractTrigger(d, AbstractTrigger::DeferredInterestTrigger)
{}

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
    : DeferredInterestTrigger(lab)
{
    Q_D(DeferredInterestTrigger);
    d->m_TargetSeniority = TargetSenior;
    d->m_TargetSeniorityLevel = TargetSeniorLevel;
    d->m_TargetSize = TargetSize;
    d->m_TargetCoupon = TargetCoupon;
    d->m_SenioritySide = ssd;
    d->m_SizeSide = szd;
    d->m_CouponSide = scd;
}

DeferredInterestTrigger::DeferredInterestTrigger(const QString& lab)
    : DeferredInterestTrigger()
{
    Q_D(DeferredInterestTrigger);
    d->m_TriggerLabel = lab;
}

QDataStream& DeferredInterestTrigger::WriteToStream(QDataStream& stream) const {
    Q_D(const DeferredInterestTrigger);
	stream
        << d->m_TargetSeniority
        << d->m_TargetSeniorityLevel
        << d->m_TargetCoupon
        << static_cast<quint8>(d->m_SizeSide)
        << static_cast<quint8>(d->m_SenioritySide)
        << static_cast<quint8>(d->m_CouponSide)
        << d->m_TargetSize
		;
	return stream;
}
QDataStream& DeferredInterestTrigger::LoadOldVersion(QDataStream& stream) {
    Q_D(DeferredInterestTrigger);
	quint8 TempChar;
    d->m_TargetSeniority.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->m_TargetSeniority;
    d->m_TargetSeniorityLevel.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->m_TargetSeniorityLevel;
    d->m_TargetCoupon.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->m_TargetCoupon;
    stream >> TempChar; d->m_SizeSide = static_cast<TriggerSizeSide>(TempChar);
    stream >> TempChar; d->m_SenioritySide = static_cast<TriggerSenioritySide>(TempChar);
    stream >> TempChar; d->m_CouponSide = static_cast<TriggerCouponSide>(TempChar);
    d->m_TargetSize.SetLoadProtocolVersion(loadProtocolVersion()); stream >> d->m_TargetSize;
	return AbstractTrigger::LoadOldVersion(stream);
}

bool DeferredInterestTrigger::Failing(const QList<std::shared_ptr< Tranche> >& currentTranches, const QDate& CurrentPeriod) const
{
    return !Passing(currentTranches, CurrentPeriod);
}

bool DeferredInterestTrigger::Failing(const QList<std::shared_ptr<Tranche> >& currentTranches, int CurrentPeriod) const
{
    return !Passing(currentTranches, CurrentPeriod);
}

QString DeferredInterestTrigger::ReadyToCalculate() const
{
    Q_D(const DeferredInterestTrigger);
	if (
        d->m_TargetSeniority.IsEmpty(1)
        || d->m_SizeSide == TriggerSizeSide::Invalid
        || d->m_SenioritySide == TriggerSenioritySide::Invalid
        || d->m_CouponSide == TriggerCouponSide::Invalid
        || d->m_TargetSeniorityLevel.IsEmpty(0)
        || d->m_TargetCoupon.IsEmpty(0)
        || d->m_TargetSize.IsEmpty(0.0)
		)
        return "Trigger " + d->m_TriggerLabel + " is invalid\n";
	return QString();
}

const DeferredInterestTrigger::TriggerSizeSide& DeferredInterestTrigger::GetSizeSide() const
{
    Q_D(const DeferredInterestTrigger);
    return d->m_SizeSide;
}

void DeferredInterestTrigger::SetSizeSide(const TriggerSizeSide& val)
{
    Q_D( DeferredInterestTrigger);
    d->m_SizeSide = val;
}

const DeferredInterestTrigger::TriggerSenioritySide& DeferredInterestTrigger::GetSenioritySide() const
{
    Q_D(const DeferredInterestTrigger);
    return d->m_SenioritySide;
}

void DeferredInterestTrigger::SetSenioritySide(const TriggerSenioritySide& val)
{
    Q_D( DeferredInterestTrigger);
    d->m_SenioritySide = val;
}

const IntegerVector& DeferredInterestTrigger::GetTargetSeniorityLevel() const
{
    Q_D(const DeferredInterestTrigger);
    return d->m_TargetSeniorityLevel;
}

void DeferredInterestTrigger::SetTargetSeniorityLevel(const QString& val)
{
    Q_D( DeferredInterestTrigger);
    d->m_TargetSeniorityLevel = val;
}

const IntegerVector& DeferredInterestTrigger::GetTargetSeniority() const
{
    Q_D(const DeferredInterestTrigger);
    return d->m_TargetSeniority;
}

void DeferredInterestTrigger::SetTargetSeniority(const QString& val)
{
    Q_D( DeferredInterestTrigger);
    d->m_TargetSeniority = val;
}

const BloombergVector& DeferredInterestTrigger::GetTargetSize() const
{
    Q_D(const DeferredInterestTrigger);
    return d->m_TargetSize;
}

void DeferredInterestTrigger::SetTargetSize(const QString& val)
{
    Q_D( DeferredInterestTrigger);
    d->m_TargetSize = val;
}

const DeferredInterestTrigger::TriggerCouponSide& DeferredInterestTrigger::GetCouponSide() const
{
    Q_D(const DeferredInterestTrigger);
    return d->m_CouponSide;
}

void DeferredInterestTrigger::SetCouponSide(const TriggerCouponSide& val)
{
    Q_D( DeferredInterestTrigger);
    d->m_CouponSide = val;
}

const IntegerVector& DeferredInterestTrigger::GetTargetCoupon() const
{
    Q_D(const DeferredInterestTrigger);
    return d->m_TargetCoupon;
}

void DeferredInterestTrigger::SetTargetCoupon(const QString& val)
{
    Q_D( DeferredInterestTrigger);
    d->m_TargetCoupon = val;
}

bool DeferredInterestTrigger::Passing(const QList<std::shared_ptr<Tranche> >& currentTranches, const QDate& CurrentPeriod) const
{
    Q_D(const DeferredInterestTrigger);
    return d->passingTemplate(currentTranches, CurrentPeriod);
}

bool DeferredInterestTrigger::Passing(const QList<std::shared_ptr<Tranche> >& currentTranches, int CurrentPeriod) const
{
    Q_D(const DeferredInterestTrigger);
    return d->passingTemplate(currentTranches, CurrentPeriod);
}


QString DeferredInterestTrigger::ToString() const {
    Q_D(const DeferredInterestTrigger);
	return AbstractTrigger::ToString()
        + "\nTarget Seniority: " + d->m_TargetSeniority.GetVector()
        + "\nTarget Seniority Level: " + d->m_TargetSeniorityLevel.GetVector()
        + "\nTarget Coupon: " + d->m_TargetCoupon.GetVector()
        + "\nSize Limit: " + d->m_TargetSize.GetVector()
        + "\nSize Side: " + TriggerSizeSideToString(d->m_SizeSide)
        + "\nSeniority Side: " + TriggerSenioritySideToString(d->m_SenioritySide)
        + "\nCoupon Side: " + TriggerCouponSideToString(d->m_CouponSide)
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
    Q_D(const DeferredInterestTrigger);
	return !(
        d->m_TargetSize.GetAnchorDate().isNull()
        || d->m_TargetSeniorityLevel.GetAnchorDate().isNull()
        || d->m_TargetSeniority.GetAnchorDate().isNull()
        || d->m_TargetCoupon.GetAnchorDate().isNull()
		);
}

void DeferredInterestTrigger::FillMissingAnchorDate(const QDate& a) {
    Q_D( DeferredInterestTrigger);
    if (d->m_TargetSize.GetAnchorDate().isNull()) 
        d->m_TargetSize.SetAnchorDate(a);
    if (d->m_TargetSeniorityLevel.GetAnchorDate().isNull()) 
        d->m_TargetSeniorityLevel.SetAnchorDate(a);
    if (d->m_TargetSeniority.GetAnchorDate().isNull()) 
        d->m_TargetSeniority.SetAnchorDate(a);
    if (d->m_TargetCoupon.GetAnchorDate().isNull()) 
        d->m_TargetCoupon.SetAnchorDate(a);
}

void DeferredInterestTrigger::SetAnchorDate(const QDate& a)
{
    Q_D( DeferredInterestTrigger);
    d->m_TargetSize.SetAnchorDate(a);
    d->m_TargetSeniorityLevel.SetAnchorDate(a); 
    d->m_TargetSeniority.SetAnchorDate(a);
}

void DeferredInterestTrigger::RemoveAnchorDate()
{
    Q_D( DeferredInterestTrigger);
    d->m_TargetSize.RemoveAnchorDate(); 
    d->m_TargetSeniorityLevel.RemoveAnchorDate();
    d->m_TargetSeniority.RemoveAnchorDate();
}

DeferredInterestTriggerSettingWidget* DeferredInterestTrigger::createSettingsWidget(QWidget* parent /*= nullptr*/)
{
    return new DeferredInterestTriggerSettingWidget(parent);
}

void DeferredInterestTrigger::SetSizeMultiplier(double mult)
{
    Q_D( DeferredInterestTrigger);
	 ResetSizeMultiplier();
	 if (mult != 0.0)
         d->m_TargetSize *= mult;
}

void DeferredInterestTrigger::ResetSizeMultiplier()
{
    Q_D( DeferredInterestTrigger);
    d->m_TargetSize.SetDivisor(1.0);
}
