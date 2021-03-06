#ifndef LoanAssMatcherResult_h__
#define LoanAssMatcherResult_h__
#include <QString>
#include "BackwardCompatibilityInterface.h"
class QDataStream;
class LoanAssMatcherResultPrivate;
class LoanAssMatcherResult;
CLOMODELLIB_EXPORT QDataStream&  operator<<(QDataStream & stream, const LoanAssMatcherResult& flows);
CLOMODELLIB_EXPORT QDataStream&  operator>>(QDataStream & stream, LoanAssMatcherResult& flows);
class CLOMODELLIB_EXPORT LoanAssMatcherResult : public BackwardInterface
{
    DECLARE_PUBLIC_COMMONS(LoanAssMatcherResult)
    DECLARE_PUBLIC_COMMONS_COPY(LoanAssMatcherResult)
public:
	virtual bool isNull() const;
	virtual bool isValid() const;
	const QString& GetDealName() const;
	void SetDealName(const QString& val);
	const QString& GetFilePath() const;
	void SetFilePath(const QString& val);
	void AddScenario(const QString& DetectedScen, const QString& CurrScen, const QString& Facility, const QString& Issuer, qint32 LoanID, double sze);
	void RemoveScenario(int index);
	int ScenarioCount() const;
	QString GetDetectScen(int index) const;
	QString GetCurrScen(int index)const;
	QString GetFacility(int index)const;
	QString GetIssuer(int index) const;
	qint32 GetLoanID(int index) const;
    double GetSize(int index) const;
	void Clear();
protected:
	virtual QDataStream& LoadOldVersion(QDataStream & stream) override;
	friend CLOMODELLIB_EXPORT QDataStream& operator<<(QDataStream & stream, const LoanAssMatcherResult& flows);
	friend CLOMODELLIB_EXPORT QDataStream& operator>>(QDataStream & stream, LoanAssMatcherResult& flows);
};
Q_DECLARE_METATYPE(LoanAssMatcherResult)

#endif // LoanAssMatcherResult_h__