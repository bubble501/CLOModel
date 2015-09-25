#include "BackwardCompatibilityInterface.h"
#include "Private/BackwardCompatibilityInterface_p.h"

DEFINE_PUBLIC_COMMONS(BackwardInterface)
DEFINE_PUBLIC_COMMONS_COPY(BackwardInterface)

BackwardInterfacePrivate::~BackwardInterfacePrivate()
{

}
BackwardInterfacePrivate::BackwardInterfacePrivate(BackwardInterface* q)
    : q_ptr(q)
    , m_LoadProtocolVersion(ModelVersionNumber)
{
    static_assert(ModelVersionNumber >= MinimumSupportedVersion, "ModelVersionNumber must be greater or equal to MinimumSupportedVersion");
}

BackwardInterfacePrivate::BackwardInterfacePrivate(BackwardInterface* q, const BackwardInterfacePrivate& other)
    :q_ptr(q)
    , m_LoadProtocolVersion(other.m_LoadProtocolVersion)
{}

BackwardInterface::BackwardInterface(BackwardInterfacePrivate *d)
    : d_ptr(d)
{}
BackwardInterface::BackwardInterface(BackwardInterfacePrivate *d, const BackwardInterface& ) 
    : d_ptr(d)
{}


BackwardInterface& BackwardInterface::operator=(const BackwardInterface& other)
{
    Q_D(BackwardInterface);
    d->m_LoadProtocolVersion = other.d_func()->m_LoadProtocolVersion;
    return *this;
}

BackwardInterface::~BackwardInterface()
{
    delete d_ptr;
}

void BackwardInterface::SetLoadProtocolVersion(qint32 VersionNum /*= ModelVersionNumber*/)
{
    Q_D(BackwardInterface);
    if (VersionNum >= MinimumSupportedVersion && VersionNum <= ModelVersionNumber) 
        d->m_LoadProtocolVersion = VersionNum;
}

void BackwardInterface::SetLoadProtocolVersion()
{
    ResetProtocolVersion();
}

qint32 BackwardInterface::loadProtocolVersion() const
{
    Q_D(const BackwardInterface);
    return d->m_LoadProtocolVersion;
}

void BackwardInterface::ResetProtocolVersion()
{
    Q_D(BackwardInterface);
    d->m_LoadProtocolVersion = ModelVersionNumber;
}