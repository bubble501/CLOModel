#ifndef BackwardCompatibilityInterface_p_h__
#define BackwardCompatibilityInterface_p_h__
#include <QtGlobal>
//! The current Version of the Model
#define ModelVersionNumber 188
#define MinimumSupportedVersion 188
#include "BackwardCompatibilityInterface.h"
class BackwardInterfacePrivate {
    DECLARE_PRIVATE_COMMONS(BackwardInterface)
protected:
    BackwardInterface* q_ptr;
public:
    qint32 m_LoadProtocolVersion;
};



#endif // BackwardCompatibilityInterface_p_h__

