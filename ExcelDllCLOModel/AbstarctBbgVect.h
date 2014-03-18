#ifndef AbstarctBbgVect_h__
#define AbstarctBbgVect_h__
#include <QString>
#include <QDate>

class AbstarctBbgVect
{
protected:
	QString m_Vector;
	QDate m_AnchorDate;
	virtual bool IsValid() const =0;
public:
	virtual bool SetVector(const QString& Vec);
	AbstarctBbgVect(){}
	AbstarctBbgVect(const QString& Vec);
	virtual void SetAnchorDate(const QDate& Anchor){m_AnchorDate=Anchor;}
	virtual void RemoveAnchorDate(){m_AnchorDate=QDate();}
	virtual QString GetVector() const;
	virtual const QDate& GetAnchorDate() const{return m_AnchorDate;}
	AbstarctBbgVect& operator=(const QString& a){SetVector(a); return *this;}
	virtual bool IsEmpty() const{return m_Vector.isEmpty();}
protected:
	virtual void UnpackVector()=0;
};
#endif // AbstarctBbgVect_h__

