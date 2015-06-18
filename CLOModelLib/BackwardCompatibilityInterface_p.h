#ifndef BackwardCompatibilityInterface_p_h__
#define BackwardCompatibilityInterface_p_h__
#include <QtGlobal>
//! The current Version of the Model
#define ModelVersionNumber 188
#define MinimumSupportedVersion 188
#include "BackwardCompatibilityInterface.h"
class BackwardInterfacePrivate {
    Q_DECLARE_PUBLIC(BackwardInterface)
protected:
    BackwardInterface* q_ptr;
public:
    BackwardInterfacePrivate(BackwardInterface* q);
    BackwardInterfacePrivate(BackwardInterface* q, const BackwardInterfacePrivate& other);
    virtual BackwardInterfacePrivate& operator=(const BackwardInterfacePrivate& other);
private:
    BackwardInterfacePrivate(const BackwardInterfacePrivate& other);
	qint32 m_LoadProtocolVersion;
};



#endif // BackwardCompatibilityInterface_p_h__

