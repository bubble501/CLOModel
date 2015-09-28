#ifndef WaterfallCalculator_p_h__
#define WaterfallCalculator_p_h__

#include "Private/AbstrAsyncCalculator_p.h"
#include "WaterfallCalculator.h"
#include <QHash>
#include <QTemporaryDir>
class WaterfallCalculatorPrivate : public AbstrAsyncCalculatorPrivate
{
    DECLARE_PRIVATE_COMMONS(WaterfallCalculator)
    DECLARE_PRIVATE_COMMONS_DATASTREAM(WaterfallCalculator)
public:
    QTemporaryDir m_dataDir;
    QHash<qint32, QString > m_CascadesPath;
    QString writeTempFile(const Waterfall& val) const;
    Waterfall readTempFile(const QString& path) const;
    void removeTempFile(const QString& path) const;
    void clearTempDir();
};





#endif // WaterfallCalculator_p_h__