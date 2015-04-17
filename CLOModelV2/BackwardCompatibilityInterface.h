#ifndef BackwardCompatibilityInterface_h__
#define BackwardCompatibilityInterface_h__
#include "CommonFunctions.h"
#include <QtGlobal>
//! The current Version of the Model
#define ModelVersionNumber 188
#define MinimumSupportedVersion 188

class BackwardInterface {
public:
	BackwardInterface() : m_LoadProtocolVersion(ModelVersionNumber) { static_assert(ModelVersionNumber >= MinimumSupportedVersion, "ModelVersionNumber must be greater or equal to MinimumSupportedVersion"); }
	virtual void SetLoadProtocolVersion(qint32 VersionNum = ModelVersionNumber) final { if (VersionNum >= MinimumSupportedVersion && VersionNum <= ModelVersionNumber) m_LoadProtocolVersion = VersionNum; }
protected:
	virtual QDataStream& LoadOldVersion(QDataStream& stream) = 0;
	virtual void ResetProtocolVersion() final { m_LoadProtocolVersion = ModelVersionNumber; }
	qint32 m_LoadProtocolVersion;
};
#endif // BackwardCompatibilityInterface_h__
