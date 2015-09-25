#ifndef TriggersResults_p_h__
#define TriggersResults_p_h__

#include "TriggersResults.h"
#include "Private/BackwardCompatibilityInterface_p.h"
#include <QHash>
#include <QMap>
#include <QDate>
#include <memory>
class TriggersResultsPrivate : public BackwardInterfacePrivate
{
    DECLARE_PRIVATE_COMMONS(TriggersResults)
    DECLARE_PRIVATE_COMMONS_COPY(TriggersResults)
    DECLARE_PRIVATE_COMMONS_DATASTREAM(TriggersResults)
public:
    QHash<quint32, std::shared_ptr<QMap<QDate, bool> > > m_Results;
};
#endif // TriggersResults_p_h__