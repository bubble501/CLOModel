#ifndef BackwardCompatibilityInterface_p_h__
#define BackwardCompatibilityInterface_p_h__
#include <QtGlobal>

#include "BackwardCompatibilityInterface.h"
class BackwardInterfacePrivate {
    DECLARE_PRIVATE_COMMONS(BackwardInterface)
    DECLARE_PRIVATE_COMMONS_COPY(BackwardInterface)
protected:
    BackwardInterface* q_ptr;
public:
    qint32 m_LoadProtocolVersion;
};



#endif // BackwardCompatibilityInterface_p_h__

