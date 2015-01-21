#ifndef LoanAssMatcherResult_h__
#define LoanAssMatcherResult_h__
#include <QString>
#include <QList>
#include "BackwardCompatibilityInterface.h"
class QDataStream;
class LoanAssMatcherResult : public BackwardInterface {
public:
	LoanAssMatcherResult(){}
	LoanAssMatcherResult(const LoanAssMatcherResult& a);
	LoanAssMatcherResult& operator=(const LoanAssMatcherResult& a);
	virtual bool isNull() const { return m_FilePath.isEmpty(); }
	virtual bool isValid() const { return !isNull(); }
	const QString& GetDealName() const { return m_DealName; }
	void SetDealName(const QString& val) { m_DealName = val; }
	const QString& GetFilePath() const { return m_FilePath; }
	void SetFilePath(const QString& val) { m_FilePath = val; }
	void AddScenario(const QString& DetectedScen, const QString& CurrScen, const QString& Facility, const QString& Issuer, qint32 LoanID);
	void RemoveScenario(int index);
	int ScenarioCount() const { return m_DetectedScen.size(); }
	QString GetDetectScen(int index) const;
	QString GetCurrScen(int index)const;
	QString GetFacility(int index)const;
	QString GetIssuer(int index) const;
	qint32 GetLoanID(int index) const;
	void Clear();
protected:
	QString m_FilePath;
	QString m_DealName;
	QList<QString> m_DetectedScen;
	QList<QString> m_CurrScen;
	QList<QString> m_Facility;
	QList<QString> m_Issuer;
	QList<qint32> m_LoanID;
	virtual QDataStream& LoadOldVersion(QDataStream & stream) override;
	friend QDataStream& operator<<(QDataStream & stream, const LoanAssMatcherResult& flows);
	friend QDataStream& operator>>(QDataStream & stream, LoanAssMatcherResult& flows);
};
Q_DECLARE_METATYPE(LoanAssMatcherResult)
QDataStream& operator<<(QDataStream & stream, const LoanAssMatcherResult& flows);
QDataStream& operator>>(QDataStream & stream, LoanAssMatcherResult& flows);
#endif // LoanAssMatcherResult_h__