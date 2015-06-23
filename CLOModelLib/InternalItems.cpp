#include "Private/InternalItems.h"
#include <QRegExp>
#include <QStringList>
#include <QStack>
#include <QFile>
#include <QXmlStreamReader>
#include <QTextStream>
#include <QDateTime>
#ifndef NO_DATABASE
QMutex Db_Mutex;
#endif
const QString LoansPropertiesToSearch[] = { "Issuer", "Facility" };
QString InfixToPostfix(const QString& a)
{
    if (a.isEmpty()) return QString();
    QString ops("-+/*");
    {
        //Prevent Operators at the beginning of the string
        QRegExp CheckDoubleOperator("^[" + ops + ']', Qt::CaseInsensitive);
        if (CheckDoubleOperator.indexIn(a) >= 0) return QString();
        //Prevent Unmatched parenthesis
        if (a.count('(') != a.count(')')) return QString();
        //Prevent two operators one after another
        CheckDoubleOperator.setPattern('[' + ops + "]\\s*[" + ops + ']');
        if (CheckDoubleOperator.indexIn(a) >= 0) return QString();
        //Prevent operator just after NOT
        CheckDoubleOperator.setPattern("![" + ops + ']');
        if (CheckDoubleOperator.indexIn(a) >= 0) return QString();

        ops += '!';

        //Prevent operators just before close of parenthesis
        CheckDoubleOperator.setPattern('[' + ops + "]\\)");
        if (CheckDoubleOperator.indexIn(a) >= 0) return QString();
        //Prevent Operators at the end of the string
        CheckDoubleOperator.setPattern('[' + ops + "]$");
        if (CheckDoubleOperator.indexIn(a) >= 0) return QString();
    }
    QRegExp CheckValidNumber("(?:\\d+|F|T)", Qt::CaseInsensitive);
    QString Spaced("");
    for (auto i = a.constBegin(); i != a.constEnd(); ++i) {
        if (i->isSpace()) continue;
        if (Spaced.isEmpty()) {
            Spaced.append(*i);
        }
        else {
            if (!(i->isDigit() && ((i - 1)->isDigit()))) Spaced.append(' ');
            Spaced.append(*i);
        }
    }
    QStack<qint32> s;
    QString sb("");
    const QStringList parts = Spaced.split(QRegExp("\\s"));
    for (auto token = parts.constBegin(); token != parts.constEnd();++token)
    {
        int idx = ops.indexOf(token->at(0));
        if (idx != -1 && token->size() == 1) {
            if (s.isEmpty())
                s.push(idx);
            else {
                while (!s.isEmpty()) {
                    int prec2 = s.top() / 2;
                    int prec1 = idx / 2;
                    if (prec2 > prec1 || (prec2 == prec1))
                        sb.append(ops.at(s.pop())).append(' ');
                    else break;
                }
                s.push(idx);
            }
        }
        else if (token->at(0) == '(') {
            s.push(-2);
        }
        else if (token->at(0) == ')') {
            while (s.top() != -2)
                sb.append(ops.at(s.pop())).append(' ');
            s.pop();
        }
        else {
            if (!CheckValidNumber.exactMatch(*token))
                return QString();
            sb.append(*token).append(' ');
        }
    }
    while (!s.isEmpty()) {
        sb.append(ops.at(s.pop())).append(' ');
    }
    return sb.trimmed();
}
QString NormaliseTriggerStructure(QString a)
{
    a = a.toLower();
    a.replace("nand", "/");
    a.replace("and", "*");
    a.replace("nor", "-");
    a.replace("or", "+");
    a.replace("not", "!");
    a.replace("^", "!");
    a.replace(QRegExp("&?&"), "*");
    a.replace(QRegExp("|?|"), "+");
    a.replace("false", "f");
    a.replace("true", "t");
    a.replace(QRegExp("\\s"), "");
    return a.toUpper();
}
QString GetFromConfig(const QString& Domain, const QString& Field, const QString& DefaultValue)
{
    QFile Source(":/Configs/GlobalConfigs.xml");
    Source.open(QIODevice::ReadOnly);
    QXmlStreamReader xml(&Source);
    bool DomainFound = false;
    bool FieldFound = false;
    while (!xml.atEnd() && !xml.hasError()) {
        xml.readNext();
        if (xml.isStartElement()) {
            if (xml.name() == Domain) {
                if (DomainFound) return DefaultValue;
                DomainFound = true;
            }
            else if (DomainFound && xml.name() == Field) {
                if (FieldFound) return DefaultValue;
                FieldFound = true;
            }
        }
        else if (xml.isEndElement()) {
            if (xml.name() == Domain) {
                if (!DomainFound) return DefaultValue;
                DomainFound = false;
            }
            else if (xml.name() == Field) {
                if (!FieldFound) return DefaultValue;
                FieldFound = false;
            }
        }
        else if (xml.isCharacters() && FieldFound) {
            return xml.text().toString();
        }
    }
    return DefaultValue;
}
int NumberOfSetBits(quint32 i)
{
    i = i - ((i >> 1) & 0x55555555);
    i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
    return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}
void PrintToTempFile(const QString& TempFileName, const QString& Message, bool PrintTime)
{
    QFile TempFile("C:/Temp/" + TempFileName + ".log");
    if (!TempFile.open(QIODevice::Append | QIODevice::Text)) return;
    QTextStream  TempWrite(&TempFile);
    TempWrite << (PrintTime ? QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm ") : QString()) + Message + '\n';
    TempFile.close();
}