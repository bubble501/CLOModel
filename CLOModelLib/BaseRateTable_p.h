
#ifndef BaseRateTable_p_h__
#define BaseRateTable_p_h__

#include "BaseRateTable.h"
#include "BackwardCompatibilityInterface_p.h"
class AbstractBaseRateTablePrivate : public BackwardInterfacePrivate
{
    DECLARE_PRIVATE_COMMONS(AbstractBaseRateTable)
    DECLARE_PRIVATE_COMMONS_DATASTREAM(AbstractBaseRateTable)
public:
    QDate UpdateDate;
};
class ConstantBaseRateTablePrivate : public AbstractBaseRateTablePrivate
{
    DECLARE_PRIVATE_COMMONS(ConstantBaseRateTable)
    DECLARE_PRIVATE_COMMONS_DATASTREAM(ConstantBaseRateTable)
public:
    QHash<QString, double> Values;
};
class ForwardBaseRateTablePrivate : public AbstractBaseRateTablePrivate
{
    DECLARE_PRIVATE_COMMONS(ForwardBaseRateTable)
    DECLARE_PRIVATE_COMMONS_DATASTREAM(ForwardBaseRateTable)
public:
    QHash<QString, BloombergVector> Values;

};
#endif // BaseRateTable_p_h__