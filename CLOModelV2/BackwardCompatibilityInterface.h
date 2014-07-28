#ifndef BackwardCompatibilityInterface_h__
#define BackwardCompatibilityInterface_h__
#include "CommonFunctions.h"
class BackwardInterface {
public:
	BackwardInterface() : m_LoadProtocolVersion(ModelVersionNumber) {}
	virtual void SetLoadProtocolVersion(qint32 VersionNum = ModelVersionNumber) { if (VersionNum >= MinimumSupportedVersion && VersionNum <= ModelVersionNumber) m_LoadProtocolVersion = VersionNum; }
protected:
	virtual QDataStream& LoadOldVersion(QDataStream& stream) = 0;
	virtual void ResetProtocolVersion() { m_LoadProtocolVersion = ModelVersionNumber; }
	qint32 m_LoadProtocolVersion;
};
#endif // BackwardCompatibilityInterface_h__
