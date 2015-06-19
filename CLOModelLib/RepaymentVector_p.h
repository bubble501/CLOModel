#ifndef RepaymentVector_p_h__
#define RepaymentVector_p_h__

#include "AbstractBbgVect_p.h"
#include "RepaymentVector.h"
class RepaymentVectorPrivate : public AbstractBbgVectPrivate
{
    Q_DECLARE_PUBLIC(RepaymentVector)
private:
    RepaymentVectorPrivate(const RepaymentVectorPrivate& other);
public:
    RepaymentVectorPrivate(RepaymentVector* q);
    RepaymentVectorPrivate(RepaymentVector* q, const RepaymentVectorPrivate& other);
    virtual RepaymentVectorPrivate& operator=(const RepaymentVectorPrivate& other);
    QList<RepaymentVector::RepaymentMethods> m_VectVal;
    QString RepaymentMethodsToString(int a) const;
    QString RepaymentMethodsToString(RepaymentVector::RepaymentMethods a) const;
    RepaymentVector::RepaymentMethods StringToRepaymentMethods(const QString& a) const;
    static RepaymentVector::RepaymentMethods ConvertRepayment(QChar a);
    friend QDataStream& operator<<(QDataStream & stream, const RepaymentVector& flows);
    friend QDataStream& operator>>(QDataStream & stream, RepaymentVector& flows);
};
#endif // RepaymentVector_p_h__