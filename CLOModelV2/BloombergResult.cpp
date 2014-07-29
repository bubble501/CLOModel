#ifndef NO_BLOOMBERG
#include "BloombergResult.h"
QStringList BloombergResult::AvailableFields(const QString& Security) {
	if (!ResultTable.contains(Security.trimmed().toUpper())) return QStringList();
	return ResultTable.value(Security.trimmed().toUpper()).keys();
}

QString BloombergResult::GetValue(const QString& Security, const QString& Field) {
	if (!ResultTable.contains(Security.trimmed().toUpper())) return QString();
	if (!ResultTable.value(Security.trimmed().toUpper()).contains(Field.trimmed().toUpper())) return QString();
	return ResultTable.value(Security.trimmed().toUpper()).value(Field.trimmed().toUpper());
}
void BloombergResult::SetErrorCode(BbgErrorCodes ErrCd) { 
	if (ErrCd == NoErrors) m_ErrorCode = NoErrors;
	else m_ErrorCode = static_cast<BbgErrorCodes>(static_cast<int>(m_ErrorCode) | static_cast<int>(ErrCd));
}
void BloombergResult::SetResult(const QString& Security, const QString& Field, const QString& Value) { 
	ResultTable[Security.trimmed().toUpper()][Field.trimmed().toUpper()] = Value; 
}
QDataStream& operator<<(QDataStream & stream, const BloombergResult& flows) {
	stream << flows.ResultTable
		<< static_cast<qint32>(flows.m_ErrorCode)
		;
	return stream;
}
QDataStream& operator>>(QDataStream & stream, BloombergResult& flows) { return flows.LoadOldVersion(stream); }
QDataStream& BloombergResult::LoadOldVersion(QDataStream& stream) {
	qint32 TempInt;
	stream >> ResultTable
		>> TempInt
		;
	m_ErrorCode = static_cast<BbgErrorCodes>(TempInt);
	ResetProtocolVersion();
	return stream;
}
#endif

