#include "LoanAssMatcherResult.h"
#include "Private/LoanAssMatcherResult_p.h"
#include <QDataStream>
DEFINE_PUBLIC_COMMONS(LoanAssMatcherResult)
DEFINE_PUBLIC_COMMONS_COPY(LoanAssMatcherResult)
LoanAssMatcherResult::~LoanAssMatcherResult()
{

}
LoanAssMatcherResultPrivate::~LoanAssMatcherResultPrivate()
{

}
LoanAssMatcherResultPrivate::LoanAssMatcherResultPrivate(LoanAssMatcherResult *q)
	:BackwardInterfacePrivate(q)
{}
LoanAssMatcherResultPrivate::LoanAssMatcherResultPrivate(LoanAssMatcherResult *q,const LoanAssMatcherResultPrivate& other)
	:BackwardInterfacePrivate(q,other)
    , m_DealName(other.m_DealName)
    , m_FilePath(other.m_FilePath)
    , m_LoanID(other.m_LoanID)
    , m_Issuer(other.m_Issuer)
    , m_Facility(other.m_Facility)
    , m_CurrScen(other.m_CurrScen)
    , m_DetectedScen(other.m_DetectedScen)
    , m_size(other.m_size)
{}
LoanAssMatcherResult::LoanAssMatcherResult(LoanAssMatcherResultPrivate *d, const LoanAssMatcherResult& other)
	:BackwardInterface(d,other)
{}
LoanAssMatcherResult& LoanAssMatcherResult::operator=(const LoanAssMatcherResult& other){
	Q_D(LoanAssMatcherResult);
	BackwardInterface::operator=(other);
    d->m_DealName = other.d_func()->m_DealName;
    d->m_LoanID = other.d_func()->m_LoanID;
    d->m_Issuer = other.d_func()->m_Issuer;
    d->m_Facility = other.d_func()->m_Facility;
    d->m_CurrScen = other.d_func()->m_CurrScen;
    d->m_DetectedScen = other.d_func()->m_DetectedScen;
    d->m_FilePath = other.d_func()->m_FilePath;
    d->m_size = other.d_func()->m_size;
	return *this;
}
LoanAssMatcherResult::LoanAssMatcherResult(LoanAssMatcherResultPrivate *d)
	:BackwardInterface(d)
{}


bool LoanAssMatcherResult::isNull() const
{
    Q_D(const LoanAssMatcherResult);
    return d->m_FilePath.isEmpty();
}

bool LoanAssMatcherResult::isValid() const
{
    return !isNull();
}

const QString& LoanAssMatcherResult::GetDealName() const
{
    Q_D(const LoanAssMatcherResult);
    return d->m_DealName;
}

void LoanAssMatcherResult::SetDealName(const QString& val)
{
    Q_D( LoanAssMatcherResult);
    d->m_DealName = val;
}

const QString& LoanAssMatcherResult::GetFilePath() const
{
    Q_D(const LoanAssMatcherResult);
    return d->m_FilePath;
}

void LoanAssMatcherResult::SetFilePath(const QString& val)
{
    Q_D( LoanAssMatcherResult);
    d->m_FilePath = val;
}

void LoanAssMatcherResult::AddScenario(const QString& DetectedScen, const QString& CurrScen, const QString& Facility, const QString& Issuer, qint32 LoanID, double sze)
{
    Q_D( LoanAssMatcherResult);
    for (int i = 0; i < d->m_LoanID.size(); ++i) {
        if (d->m_LoanID.at(i) == LoanID) {
			RemoveScenario(i);
			break;
		}
	}
    d->m_DetectedScen.append(DetectedScen);
    d->m_CurrScen.append(CurrScen);
    d->m_Facility.append(Facility);
    d->m_Issuer.append(Issuer);
    d->m_LoanID.append(LoanID);
    d->m_size.append(sze);
}

void LoanAssMatcherResult::RemoveScenario(int index) {
    Q_D( LoanAssMatcherResult);
	if (index < 0 || index >= ScenarioCount())
        d->m_DetectedScen.removeAt(index);
    d->m_CurrScen.removeAt(index);
    d->m_Facility.removeAt(index);
    d->m_Issuer.removeAt(index);
    d->m_LoanID.removeAt(index);
    d->m_size.removeAt(index);
}

int LoanAssMatcherResult::ScenarioCount() const
{
    Q_D(const LoanAssMatcherResult);
    return d->m_DetectedScen.size();
}

void LoanAssMatcherResult::Clear()
{
    Q_D( LoanAssMatcherResult);
    d->m_FilePath.clear();
    d->m_DealName.clear();
    d->m_DetectedScen.clear();
    d->m_CurrScen.clear();
    d->m_Facility.clear();
    d->m_Issuer.clear();
    d->m_LoanID.clear();
    d->m_size.clear();
}

QString LoanAssMatcherResult::GetDetectScen(int index) const
{
    Q_D(const LoanAssMatcherResult);
    return d->m_DetectedScen.value(index);
}

QString LoanAssMatcherResult::GetCurrScen(int index) const
{
    Q_D(const LoanAssMatcherResult);
    return d->m_CurrScen.value(index);
}

QString LoanAssMatcherResult::GetFacility(int index) const
{
    Q_D(const LoanAssMatcherResult);
    return d->m_Facility.value(index);
}

QString LoanAssMatcherResult::GetIssuer(int index) const
{
    Q_D(const LoanAssMatcherResult);
    return d->m_Issuer.value(index);
}

qint32 LoanAssMatcherResult::GetLoanID(int index) const
{
    Q_D(const LoanAssMatcherResult);
    return d->m_LoanID.value(index,-1);
}

double LoanAssMatcherResult::GetSize(int index) const
{
    Q_D(const LoanAssMatcherResult);
    return d->m_size.value(index, 0.0);
}

QDataStream& LoanAssMatcherResult::LoadOldVersion(QDataStream & stream)
{
    Q_D( LoanAssMatcherResult);
	stream
        >> d->m_FilePath
        >> d->m_DealName
        >> d->m_DetectedScen
        >> d->m_CurrScen
        >> d->m_Facility
        >> d->m_Issuer
        >> d->m_LoanID
	;
    if (loadProtocolVersion() >= 192) {
        stream >> d->m_size;
    }
    else {
        while (d->m_size.size() < d->m_LoanID.size())
            d->m_size.append(0.0);
    }
	ResetProtocolVersion();
	return stream;
}

QDataStream& operator<<(QDataStream & stream, const LoanAssMatcherResult& flows)
{
	stream
		<< flows.d_func()->m_FilePath
        << flows.d_func()->m_DealName
        << flows.d_func()->m_DetectedScen
        << flows.d_func()->m_CurrScen
        << flows.d_func()->m_Facility
        << flows.d_func()->m_Issuer
        << flows.d_func()->m_LoanID
        << flows.d_func()->m_size
	;
	return stream;
}

QDataStream& operator>>(QDataStream & stream, LoanAssMatcherResult& flows) {
	return flows.LoadOldVersion(stream);
}
