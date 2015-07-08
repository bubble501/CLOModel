#ifndef RepaymentVector_p_h__
#define RepaymentVector_p_h__

#include "AbstractBbgVect_p.h"
#include "RepaymentVector.h"
class RepaymentVectorPrivate : public AbstractBbgVectPrivate
{
    DECLARE_PRIVATE_COMMONS(RepaymentVector)
    DECLARE_PRIVATE_COMMONS_COPY(RepaymentVector)
    DECLARE_PRIVATE_COMMONS_DATASTREAM(RepaymentVector)
public:
    QList<RepaymentVector::RepaymentMethods> m_VectVal;
    QString RepaymentMethodsToString(int a) const;
    QString RepaymentMethodsToString(RepaymentVector::RepaymentMethods a) const;
    RepaymentVector::RepaymentMethods StringToRepaymentMethods(const QString& a) const;
    static RepaymentVector::RepaymentMethods ConvertRepayment(QChar a);
};
#endif // RepaymentVector_p_h__