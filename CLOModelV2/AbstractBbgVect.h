#ifndef AbstarctBbgVect_h__
#define AbstarctBbgVect_h__
#include <QString>
#include <QDate>
#include "BackwardCompatibilityInterface.h"
#include <QDataStream>
#include <QMetaType>
class AbstractBbgVect : public BackwardInterface
{
protected:
	QString m_Vector;
	QDate m_AnchorDate;
	virtual bool IsValid() const =0;
	virtual bool IsValid(const QString& ValidInputs, bool AllowRamps) const;
	virtual void UnpackVector() = 0;
	virtual bool ValidAnchorDate() const;
	virtual bool ExtractAnchorDate();
public:
	virtual void Clear();
	virtual bool SetVector(const QString& Vec);
	AbstractBbgVect(){}
	AbstractBbgVect(const QString& Vec);
	virtual void SetAnchorDate(const QDate& Anchor){m_AnchorDate=Anchor;}
	virtual void RemoveAnchorDate(){m_AnchorDate=QDate();}
	virtual QString GetVector() const;
	virtual const QDate& GetAnchorDate() const{return m_AnchorDate;}
	AbstractBbgVect& operator=(const QString& a){SetVector(a); return *this;}
	virtual bool IsEmpty() const{return m_Vector.isEmpty();}
};
#endif // AbstarctBbgVect_h__

