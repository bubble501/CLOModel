#include "AbstractBbgVect.h"
#include "Private/AbstractBbgVect_p.h"
#include <QRegularExpression>
#include <QStringList>
#include <QRegularExpressionValidator>
#include <QDate>
DEFINE_PUBLIC_COMMONS(AbstractBbgVect)
DEFINE_PUBLIC_COMMONS_COPY(AbstractBbgVect)
AbstractBbgVectPrivate::AbstractBbgVectPrivate(AbstractBbgVect *q,const AbstractBbgVectPrivate& other)
    :BackwardInterfacePrivate(q, other)
    , m_Vector(other.m_Vector)
    , m_AnchorDate(other.m_AnchorDate)
{}
AbstractBbgVectPrivate::AbstractBbgVectPrivate(AbstractBbgVect *q)
	:BackwardInterfacePrivate(q)
{}
AbstractBbgVect::AbstractBbgVect(AbstractBbgVectPrivate *d, const AbstractBbgVect& other)
	:BackwardInterface(d,other)
{}
AbstractBbgVect::AbstractBbgVect(AbstractBbgVectPrivate *d)
	:BackwardInterface(d)
{}
AbstractBbgVect& AbstractBbgVect::operator=(const AbstractBbgVect& other)
{
    BackwardInterface::operator=(other);
    Q_D(AbstractBbgVect);
    d->m_Vector = other.d_func()->m_Vector;
    d->m_AnchorDate = other.d_func()->m_AnchorDate;
    return *this;
}

void AbstractBbgVect::SetVector(const QString& Vec){
    Q_D(AbstractBbgVect);
	d->m_Vector = Vec;
    if (!IsValid()) 
        d->m_Vector.clear();
    UnpackVector();
}
AbstractBbgVect::AbstractBbgVect(const QString& Vec)
    :AbstractBbgVect()
{
    SetVector(Vec);
}



void AbstractBbgVect::SetAnchorDate(const QDate& Anchor)
{
    Q_D(AbstractBbgVect);
    d->m_AnchorDate = Anchor;
}

void AbstractBbgVect::RemoveAnchorDate()
{
    Q_D(AbstractBbgVect);
    d->m_AnchorDate = QDate();
}

QString AbstractBbgVect::GetVector() const
{
    Q_D(const AbstractBbgVect);
    if (d->m_AnchorDate.isNull())
        return d->m_Vector;
    return "A " + d->m_AnchorDate.toString("MM/dd/yyyy") + ' ' + d->m_Vector;
}

const QDate& AbstractBbgVect::GetAnchorDate() const
{
    Q_D(const AbstractBbgVect);
    return d->m_AnchorDate;
}

bool AbstractBbgVect::IsEmpty() const
{
    Q_D(const AbstractBbgVect);
    return d->m_Vector.isEmpty();
}

AbstractBbgVect& AbstractBbgVect::operator=(const QString& a)
{
    SetVector(a); 
    return *this;
}

bool AbstractBbgVect::ValidAnchorDate() const
{
    Q_D(const AbstractBbgVect);
    QRegularExpression AnchorCheck(QString("^A\\s+") + VectorAnchorDateFormat + "\\s+.+", QRegularExpression::CaseInsensitiveOption);
    Q_ASSERT(AnchorCheck.isValid());
    const auto anchorMatch = AnchorCheck.match(d->m_Vector.trimmed());
    if (anchorMatch.hasMatch())
        return QDate::isValid(anchorMatch.captured(3).toInt(), anchorMatch.captured(1).toInt(), anchorMatch.captured(2).toInt());
	return true;
}
bool AbstractBbgVect::IsValid(const QString& ValidInputs, bool AllowRamps) const {
    Q_D(const AbstractBbgVect);
	QString PatternString = QString("^(?:A\\s+") + VectorAnchorDateFormat + "\\s+){0,1}" + ValidInputs + "(?:\\s+[1-9][0-9]*";
	if (AllowRamps) 
        PatternString += "[RS]";
	else 
        PatternString += 'S';
	PatternString += "\\s+" + ValidInputs + ")*$";
    QRegularExpression Vigil(PatternString, QRegularExpression::CaseInsensitiveOption | QRegularExpression::DontCaptureOption);
    Q_ASSERT(Vigil.isValid());
    return (Vigil.match(d->m_Vector.trimmed()).hasMatch() && ValidAnchorDate()) || d->m_Vector.isEmpty();
}
QRegularExpressionValidator* AbstractBbgVect::GetValidator(const QString& ValidInputs, bool AllowRamps, QObject* parent) const {
	QString PatternString = QString("(^$|^(?:A\\s+") + VectorAnchorDateFormat + "\\s+){0,1}" + ValidInputs + "(?:\\s+[1-9][0-9]*";
	if (AllowRamps) PatternString += "[RS]";
	else PatternString += 'S';
	PatternString += "\\s+" + ValidInputs + ")*$)";
    QRegularExpression ValidReg(PatternString, QRegularExpression::CaseInsensitiveOption | QRegularExpression::DontCaptureOption);
    return new QRegularExpressionValidator(ValidReg, parent);
}
bool AbstractBbgVect::ExtractAnchorDate() {
    Q_D(AbstractBbgVect);
    QString TempVec(d->m_Vector.trimmed().toUpper());
    QRegularExpression AnchorCheck(QString("^A\\s+") + VectorAnchorDateFormat + "\\s+(.+)", QRegularExpression::CaseInsensitiveOption);
    Q_ASSERT(AnchorCheck.isValid());
    const auto anchorMatch = AnchorCheck.match(TempVec);
    if (anchorMatch.hasMatch()) {
        d->m_AnchorDate.setDate(anchorMatch.captured(3).toInt(), anchorMatch.captured(1).toInt(), anchorMatch.captured(2).toInt());
        d->m_Vector = anchorMatch.captured(4);
		return true;
	}
    d->m_AnchorDate = QDate();
	return false;
}
void AbstractBbgVect::Clear() {
    Q_D(AbstractBbgVect);
    d->m_AnchorDate = QDate();
    d->m_Vector = "";
	UnpackVector();
}




