#ifndef BackwardCompatibilityInterface_h__
#define BackwardCompatibilityInterface_h__
#include "clomodellib_global.h"
#include "CommonFunctions.h"
class QDataStream;
class  BackwardInterfacePrivate;
class CLOMODELLIB_EXPORT BackwardInterface{
    DECLARE_PUBLIC_COMMONS(BackwardInterface)
    DECLARE_PUBLIC_COMMONS_COPY(BackwardInterface)
protected:
    BackwardInterfacePrivate* d_ptr;
    virtual QDataStream& LoadOldVersion(QDataStream& stream) = 0;
    virtual void ResetProtocolVersion() final;
public:
    virtual BackwardInterface& operator=(const BackwardInterface& other);
    virtual  ~BackwardInterface();
	virtual void SetLoadProtocolVersion(qint32 VersionNum) final;
    virtual void SetLoadProtocolVersion() final;
    virtual qint32 loadProtocolVersion() const;
};
#endif // BackwardCompatibilityInterface_h__
