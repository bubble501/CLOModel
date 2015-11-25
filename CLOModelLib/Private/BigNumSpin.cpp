#include "BigNumSpin.h"
#include <QRegExp>
BigNumSpin::BigNumSpin(QWidget *parent)
    : QDoubleSpinBox(parent)
{

}
QString BigNumSpin::textFromValue(double value) const
{
    return locale().toString(value, 'f', decimals());
}

double BigNumSpin::valueFromText(const QString& text) const
{
    QString baseString = text.mid(prefix().size());
    baseString.chop(suffix().size());
    QRegExp extensionRegExp("([km]|(?:e[-+]?[0-9]+))", Qt::CaseInsensitive);
    if (baseString.indexOf(extensionRegExp) >= 0) {
        baseString = baseString.left(baseString.indexOf(extensionRegExp));
        double baseVal = locale().toDouble(baseString);
        if (extensionRegExp.cap(1).compare("k", Qt::CaseInsensitive) == 0) {
            baseVal *= 1000.0;
        }
        else if (extensionRegExp.cap(1).compare("m", Qt::CaseInsensitive) == 0) {
            baseVal *= 1000000.0;
        }
        else if (extensionRegExp.cap(1).left(1).compare("e", Qt::CaseInsensitive) == 0) {
            int exponents = locale().toInt(extensionRegExp.cap(1).mid(1));
            for (int i = 0; i < qAbs(exponents); ++i) {
                if (exponents > 0)
                    baseVal *= 10.0;
                else
                    baseVal /= 10.0;
            }
        }
        return baseVal;
    }
    return locale().toDouble(baseString);
}

QValidator::State BigNumSpin::validate(QString& text, int& ) const
{
    if (text.isEmpty())
        return QValidator::State::Acceptable;
    if (text.size() == 1 && (text.at(0)=='+' || text.at(0)=='-'))
        return QValidator::State::Intermediate;
    QString baseString = text.mid(prefix().size());
    baseString.chop(suffix().size());
    bool validLeft;
    QRegExp extensionRegExp("([kme])", Qt::CaseInsensitive);
    int indexOfExt = baseString.indexOf(extensionRegExp);
    if (indexOfExt >= 0) {
        locale().toDouble(baseString.left(indexOfExt), &validLeft);
        if (!validLeft)
            return QValidator::State::Invalid;
        if (extensionRegExp.cap(1).compare("k", Qt::CaseInsensitive) == 0 || extensionRegExp.cap(1).compare("m", Qt::CaseInsensitive) == 0) {
            if (indexOfExt == baseString.size() - 1)
                return QValidator::State::Acceptable;
            return QValidator::State::Invalid;
        }
        else if (extensionRegExp.cap(1).compare("e", Qt::CaseInsensitive) == 0){
            if (indexOfExt == baseString.size() - 1)
                return  QValidator::State::Intermediate;
            locale().toInt(baseString.right(indexOfExt), &validLeft);
            return validLeft ? QValidator::State::Acceptable : QValidator::State::Invalid;
        }
        Q_UNREACHABLE();
        return QValidator::State::Invalid;
    }
    locale().toDouble(baseString, &validLeft);
    return validLeft ? QValidator::State::Acceptable : QValidator::State::Invalid;
}

