#include "AbstarctBbgVect.h"
bool AbstarctBbgVect::SetVector(const QString& Vec){
	QString OldVec(m_Vector);
	m_Vector=Vec;
	if(IsValid()){
		UnpackVector();
		return true;
	}
	m_Vector=OldVec;
	return false;
}
AbstarctBbgVect::AbstarctBbgVect(const QString& Vec)
	:m_Vector(Vec)
{}
QString AbstarctBbgVect::GetVector() const{
	if(m_AnchorDate.isNull())
		return m_Vector;
	return "A "+m_AnchorDate.toString("MM/dd/yyyy")+' ' +m_Vector;
}