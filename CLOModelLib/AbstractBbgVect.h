#ifndef AbstarctBbgVect_h__
#define AbstarctBbgVect_h__
#include <QString>
#include "BackwardCompatibilityInterface.h"
class QDate;
class QObject;
class QRegExpValidator;
class AbstractBbgVectPrivate;
class CLOMODELLIB_EXPORT AbstractBbgVect : public BackwardInterface
{
    DECLARE_PUBLIC_COMMONS(AbstractBbgVect)
    DECLARE_PUBLIC_COMMONS_COPY(AbstractBbgVect)
protected:
    virtual bool IsValid() const = 0;
    virtual bool IsValid(const QString& ValidInputs, bool AllowRamps) const;
    virtual QRegExpValidator* GetValidator(const QString& ValidInputs, bool AllowRamps, QObject* parent = 0) const;
    virtual void UnpackVector() = 0;
    virtual bool ValidAnchorDate() const;
    virtual bool ExtractAnchorDate();
public:
	virtual QRegExpValidator* GetValidator(QObject* parent = 0) const =0;
	virtual void Clear();
	virtual void SetVector(const QString& Vec);
	AbstractBbgVect(const QString& Vec);
	virtual void SetAnchorDate(const QDate& Anchor);
	virtual void RemoveAnchorDate();
	virtual QString GetVector() const;
	virtual const QDate& GetAnchorDate() const;
	AbstractBbgVect& operator=(const QString& a);
	virtual bool IsEmpty() const;
};



#endif // AbstarctBbgVect_h__

