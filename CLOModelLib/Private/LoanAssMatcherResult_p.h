#include "LoanAssMatcherResult.h"
#include "Private/BackwardCompatibilityInterface_p.h"
#include <QList>
class LoanAssMatcherResultPrivate : public BackwardInterfacePrivate
{
    DECLARE_PRIVATE_COMMONS(LoanAssMatcherResult)
    DECLARE_PRIVATE_COMMONS_DATASTREAM(LoanAssMatcherResult)
public:
    QString m_FilePath;
    QString m_DealName;
    QList<QString> m_DetectedScen;
    QList<QString> m_CurrScen;
    QList<QString> m_Facility;
    QList<QString> m_Issuer;
    QList<qint32> m_LoanID;
};