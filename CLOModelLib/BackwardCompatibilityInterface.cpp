#include "BackwardCompatibilityInterface.h"
#include "BackwardCompatibilityInterface_p.h"
BackwardInterfacePrivate::BackwardInterfacePrivate(BackwardInterface* q)
    : q_ptr(q)
    , m_LoadProtocolVersion(ModelVersionNumber)
{
    static_assert(ModelVersionNumber >= MinimumSupportedVersion, "ModelVersionNumber must be greater or equal to MinimumSupportedVersion");
}

BackwardInterface::BackwardInterface()
    : d_ptr(new BackwardInterfacePrivate(this))
{}

BackwardInterface::BackwardInterface(BackwardInterfacePrivate *d)
    : d_ptr(d)
{

}

BackwardInterface::BackwardInterface(const BackwardInterface& other)
    : d_ptr(new BackwardInterfacePrivate(this,*other.d_func()))
{}

BackwardInterface& BackwardInterface::operator=(const BackwardInterface& other)
{
    Q_D(BackwardInterface);
    d->operator=(*(other.d_func()));
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

BackwardInterfacePrivate::BackwardInterfacePrivate(BackwardInterface* q, const BackwardInterfacePrivate& other)
    :q_ptr(q)
    , m_LoadProtocolVersion(other.m_LoadProtocolVersion)
{}

BackwardInterfacePrivate& BackwardInterfacePrivate::operator=(const BackwardInterfacePrivate& other)
{
    m_LoadProtocolVersion = other.m_LoadProtocolVersion;
    return *this;
}