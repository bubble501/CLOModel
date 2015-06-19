
#ifndef BaseRateTable_p_h__
#define BaseRateTable_p_h__

#include "BaseRateTable.h"
#include "BackwardCompatibilityInterface_p.h"
class AbstractBaseRateTablePrivate : public BackwardInterfacePrivate
{
    Q_DECLARE_PUBLIC(AbstractBaseRateTable)
private:
    AbstractBaseRateTablePrivate(const AbstractBaseRateTablePrivate& other);
public:
    AbstractBaseRateTablePrivate(AbstractBaseRateTable* q);
    AbstractBaseRateTablePrivate(AbstractBaseRateTable* q, const AbstractBaseRateTablePrivate& other);
    virtual AbstractBaseRateTablePrivate& operator=(const AbstractBaseRateTablePrivate& other);
    QDate UpdateDate;
    friend QDataStream& operator<<(QDataStream & stream, const AbstractBaseRateTable& flows);
    friend QDataStream& operator>>(QDataStream & stream, AbstractBaseRateTable& flows);
};
class ConstantBaseRateTablePrivate : public AbstractBaseRateTablePrivate
{
    Q_DECLARE_PUBLIC(ConstantBaseRateTable)
private:
    ConstantBaseRateTablePrivate(const AbstractBaseRateTablePrivate& other);
public:
    ConstantBaseRateTablePrivate(ConstantBaseRateTable* q);
    ConstantBaseRateTablePrivate(ConstantBaseRateTable* q, const ConstantBaseRateTablePrivate& other);
    virtual ConstantBaseRateTablePrivate& operator=(const ConstantBaseRateTablePrivate& other);
    QHash<QString, double> Values;
    friend QDataStream& operator<<(QDataStream & stream, const ConstantBaseRateTable& flows);
    friend QDataStream& operator>>(QDataStream & stream, ConstantBaseRateTable& flows);
};
class ForwardBaseRateTablePrivate : public AbstractBaseRateTablePrivate
{
    Q_DECLARE_PUBLIC(ForwardBaseRateTable)
private:
    ForwardBaseRateTablePrivate(const AbstractBaseRateTablePrivate& other);
public:
    ForwardBaseRateTablePrivate(ForwardBaseRateTable* q);
    ForwardBaseRateTablePrivate(ForwardBaseRateTable* q, const ForwardBaseRateTablePrivate& other);
    virtual ForwardBaseRateTablePrivate& operator=(const ForwardBaseRateTablePrivate& other);
    QHash<QString, BloombergVector> Values;
    friend QDataStream& operator<<(QDataStream & stream, const ForwardBaseRateTable& flows);
    friend QDataStream& operator>>(QDataStream & stream, ForwardBaseRateTable& flows);
};
#endif // BaseRateTable_p_h__