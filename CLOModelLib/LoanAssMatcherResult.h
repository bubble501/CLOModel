#ifndef LoanAssMatcherResult_h__
#define LoanAssMatcherResult_h__
#include <QString>
#include <QList>
#include "BackwardCompatibilityInterface.h"
class QDataStream;
class LoanAssMatcherResultPrivate;
class LoanAssMatcherResult : public BackwardInterface {
    DECLARE_PUBLIC_COMMONS(LoanAssMatcherResult)
    DECLARE_PUBLIC_COMMONS_COPY(LoanAssMatcherResult)
public:
	virtual bool isNull() const;
	virtual bool isValid() const;
	const QString& GetDealName() const;
	void SetDealName(const QString& val);
	const QString& GetFilePath() const;
	void SetFilePath(const QString& val);
	void AddScenario(const QString& DetectedScen, const QString& CurrScen, const QString& Facility, const QString& Issuer, qint32 LoanID);
	void RemoveScenario(int index);
	int ScenarioCount() const;
	QString GetDetectScen(int index) const;
	QString GetCurrScen(int index)const;
	QString GetFacility(int index)const;
	QString GetIssuer(int index) const;
	qint32 GetLoanID(int index) const;
	void Clear();
protected:
	virtual QDataStream& LoadOldVersion(QDataStream & stream) override;
	friend QDataStream& operator<<(QDataStream & stream, const LoanAssMatcherResult& flows);
	friend QDataStream& operator>>(QDataStream & stream, LoanAssMatcherResult& flows);
};
Q_DECLARE_METATYPE(LoanAssMatcherResult)
QDataStream& operator<<(QDataStream & stream, const LoanAssMatcherResult& flows);
QDataStream& operator>>(QDataStream & stream, LoanAssMatcherResult& flows);
#endif // LoanAssMatcherResult_h__