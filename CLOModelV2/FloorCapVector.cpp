#include "FloorCapVector.h"
#include <QRegExp>
#include <QStringList>
#include "CommonFunctions.h"
FloorCapVector::FloorCapVector(const QString& Vec)
	:AbstractBbgVect(Vec) {
	if (IsValid()) UnpackVector();
	else {
		RemoveAnchorDate();
		m_Vector = "";
	}
}
FloorCapVector::FloorCapVector(const FloorCapVector& Vec)
{
	operator=(Vec);
}
FloorCapVector& FloorCapVector::operator=(const FloorCapVector& Vec) {
	m_FloorVal.clear();
	m_CapVal.clear();
	foreach(const QSharedPointer<double>& SingleFloor, Vec.m_FloorVal) {
		if (SingleFloor) m_FloorVal.append(QSharedPointer<double>(new double(*SingleFloor)));
		else  m_FloorVal.append(QSharedPointer<double>(NULL));
	}
	foreach(const QSharedPointer<double>& SingleCap, Vec.m_CapVal) {
		if (SingleCap) m_CapVal.append(QSharedPointer<double>(new double(*SingleCap)));
		else  m_CapVal.append(QSharedPointer<double>(NULL));
	}
	m_Vector = Vec.m_Vector;
	m_AnchorDate = Vec.m_AnchorDate;
	return *this;
}
FloorCapVector::FloorCapVector(const QString& Vec, const QDate& Anchor)
	:AbstractBbgVect(Vec) {
	if (IsValid()) { UnpackVector(); m_AnchorDate = Anchor; }
	else {
		RemoveAnchorDate();
		m_Vector = "";
	}

}
void FloorCapVector::UnpackVector() {
	m_FloorVal.clear();
	m_CapVal.clear();
	if (m_Vector.isEmpty()) return;
	ExtractAnchorDate();
	QString TempVec(m_Vector.trimmed().toUpper());
	QStringList StringParts = TempVec.trimmed().toUpper().split(QRegExp("\\s"), QString::SkipEmptyParts);
	int StepLen;
	QString TempStr;
	for (int i = 1; i < StringParts.size(); i += 2) {
		TempStr = StringParts.at(i);
		TempStr.replace(QRegExp("\\D"), "");
		StepLen = TempStr.toInt();
		TempStr = StringParts.at(i);
		TempStr.replace(QRegExp("\\d"), "");
		for (int j = 0; j < StepLen; j++) {
			QString RawVal = StringParts.at(i - 1);
			RawVal.replace("[", "");
			RawVal.replace("]", "");
			auto CapFloor = RawVal.split(',', QString::KeepEmptyParts);
			if (CapFloor.size() > 0) {
				if (CapFloor.at(0).isEmpty()) m_FloorVal.append(QSharedPointer<double>(NULL));
				else m_FloorVal.append(QSharedPointer<double>(new double(CapFloor.at(0).toDouble())));
			}
			else m_FloorVal.append(QSharedPointer<double>(NULL));
			if (CapFloor.size() > 1) {
				if (CapFloor.at(1).isEmpty()) m_CapVal.append(QSharedPointer<double>(NULL));
				else m_CapVal.append(QSharedPointer<double>(new double(CapFloor.at(1).toDouble())));
			}
			else m_CapVal.append(QSharedPointer<double>(NULL));
		}
	}
	{
		QString RawVal = StringParts.last();
		RawVal.replace("[", "");
		RawVal.replace("]", "");
		auto CapFloor = RawVal.split(',', QString::KeepEmptyParts);
		if (CapFloor.size() > 0) {
			if (CapFloor.at(0).isEmpty()) m_FloorVal.append(QSharedPointer<double>(NULL));
			else m_FloorVal.append(QSharedPointer<double>(new double(CapFloor.at(0).toDouble()))); 
		}
		else m_FloorVal.append(QSharedPointer<double>(NULL));
		if (CapFloor.size() > 1) {
			if (CapFloor.at(1).isEmpty()) m_CapVal.append(QSharedPointer<double>(NULL));
			else m_CapVal.append(QSharedPointer<double>(new double(CapFloor.at(1).toDouble())));
		}
		else m_CapVal.append(QSharedPointer<double>(NULL));
	}
}
bool FloorCapVector::IsValid() const {
	return AbstractBbgVect::IsValid("\\[(?:-?\\d*\\.?\\d+)?(?:,-?\\d*\\.?\\d+)?\\]", false);
}
QRegExpValidator* FloorCapVector::GetValidator(QObject* parent) const {
	return AbstractBbgVect::GetValidator("\\[(?:-?\\d*\\.?\\d+)?(?:,-?\\d*\\.?\\d+)?\\]", false, parent);
}

QDataStream& operator<<(QDataStream & stream, const FloorCapVector& flows) {
	stream << flows.m_Vector;
	stream << flows.m_AnchorDate;
	return stream;
}
QDataStream& operator>>(QDataStream & stream, FloorCapVector& flows) { return flows.LoadOldVersion(stream); }
QDataStream& FloorCapVector::LoadOldVersion(QDataStream& stream) {
	stream
		>> m_Vector
		>> m_AnchorDate
		;
	UnpackVector();
	ResetProtocolVersion();
	return stream;
}
