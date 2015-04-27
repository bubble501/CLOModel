#include "BaseRateVect.h"
#include <QRegExp>
#include <QStringList>
#include "CommonFunctions.h"
#ifndef NO_BLOOMBERG
#include <QbbgReferenceDataRequest.h>
#include <QBbgReferenceDataResponse.h>
#include <QBbgRequestGroup.h>
#include <QBbgManager.h>
#endif // !NO_BLOOMBERG
#ifndef NO_DATABASE
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#endif


BaseRateVector::BaseRateVector(const QString& Vec)
	:AbstractBbgVect(Vec)
{
	if(IsValid()) UnpackVector();
	else {
		RemoveAnchorDate();
		m_Vector = "";
	}
}
BaseRateVector::BaseRateVector(const BaseRateVector& Vec)
	:m_VectVal(Vec.m_VectVal)
{
	m_Vector=Vec.m_Vector;
	m_AnchorDate=Vec.m_AnchorDate;
}
BaseRateVector& BaseRateVector::operator=(const BaseRateVector& Vec) {
	m_VectVal = Vec.m_VectVal;
	m_Vector = Vec.m_Vector;
	m_AnchorDate = Vec.m_AnchorDate;
	return *this;
}
BaseRateVector::BaseRateVector(const QString& Vec,const QDate& Anchor)
	:AbstractBbgVect(Vec)
{
	if (IsValid()){ UnpackVector(); m_AnchorDate = Anchor;}
	else {
		RemoveAnchorDate();
		m_Vector = "";
	}
	
}
bool BaseRateVector::IsValid() const{
	if (!AbstractBbgVect::IsValid(R"**([^\s,\[\]]+(?:\[(?:(?:-?\d*\.?\d+)|(?:(?:-?\d*\.?\d+)?(?:,-?\d*\.?\d+)))\])?)**", false)) return false;
	QRegExp rx("\\[(-?\\d*\\.?\\d+),(-?\\d*\\.?\\d+)\\]");
	for (int pos = 0; (pos = rx.indexIn(m_Vector, pos)) >= 0; pos += rx.matchedLength()) {
		if (rx.cap(1).toDouble() > rx.cap(2).toDouble()) return false;
	}
	return true;
}
QRegExpValidator* BaseRateVector::GetValidator(QObject* parent) const {
	return AbstractBbgVect::GetValidator(R"**([^\s,\[\]]+(?:\[(?:(?:-?\d*\.?\d+)|(?:(?:-?\d*\.?\d+)?(?:,-?\d*\.?\d+)))\])?)**", false, parent);
}
void BaseRateVector::UnpackVector(){
	m_VectVal.clear();
	if(m_Vector.isEmpty()) return;
	ExtractAnchorDate();
	QString TempVec(m_Vector.trimmed().toUpper());
	QStringList StringParts=TempVec.trimmed().toUpper().split(QRegExp("\\s"),QString::SkipEmptyParts);
	int StepLen;
	QString TempStr;
	for (int i=1;i<StringParts.size();i+=2){
		TempStr=StringParts.at(i);
		TempStr.replace(QRegExp("\\D"),"");
		StepLen=TempStr.toInt();
		for (int j=0;j<StepLen;j++){
			m_VectVal.append(StringParts.at(i-1));
		}
	}
	m_VectVal.append(StringParts.last());
}
QDataStream& operator<<(QDataStream & stream, const BaseRateVector& flows){
	stream 
		<< flows.m_VectVal
		<< flows.m_Vector
		<< flows.m_AnchorDate
		;
	return stream;
}
QDataStream& operator>>(QDataStream & stream, BaseRateVector& flows) { return flows.LoadOldVersion(stream); }
QDataStream& BaseRateVector::LoadOldVersion(QDataStream& stream) {
	stream
		>> m_VectVal
		>> m_Vector
		>> m_AnchorDate
		;
	ResetProtocolVersion();
	return stream;
}
BloombergVector BaseRateVector::CompileReferenceRateValue(ForwardBaseRateTable& Values) const {
	BloombergVector Result;
	if (IsEmpty()) return Result;
	QStringList StringParts = m_Vector.trimmed().toUpper().split(QRegExp("\\s"), QString::SkipEmptyParts);
	int StepLen = 0;
	QString TempStr;
	if (!Values.GetValues().contains(StringParts.first()) || Values.GetValues().value(StringParts.first()).IsEmpty())
#ifdef NO_BLOOMBERG
		return BloombergVector();
#else
		return GetRefRateValueFromBloomberg(Values);
#endif
	Result = Values.GetValues().value(StringParts.first());
	for (int i = 1; i < StringParts.size(); i += 2) {
		TempStr = StringParts.at(i);
		TempStr.replace(QRegExp("\\D"), "");
		StepLen = TempStr.toInt();
		if (!Values.GetValues().contains(StringParts.at(i + 1)) || Values.GetValues().value(StringParts.at(i + 1)).IsEmpty())
#ifdef NO_BLOOMBERG
			return BloombergVector();
#else
			return GetRefRateValueFromBloomberg(Values);
#endif
		Result.Combine(Values.GetValues().value(StringParts.at(i + 1)), StepLen);
	}
	Result.ApplyFloorCap(ExtractFloorCapVector());
	return Result;
}
BloombergVector BaseRateVector::CompileReferenceRateValue(ConstantBaseRateTable& Values) const {
	if (IsEmpty())return BloombergVector();
	QString ResultingVector("");
	for (int i = 0; i < NumElements(); i++) {
		if (!Values.Contains(GetValue(i))) {
#ifdef NO_BLOOMBERG
			return BloombergVector();
#else
			return GetRefRateValueFromBloomberg(Values);
#endif
		}
	}
	const QHash<QString, double>& Sobstitutes = Values.GetValues();
	QString NewVector = m_Vector;
	for (QHash<QString, double>::const_iterator i = Sobstitutes.constBegin(); i != Sobstitutes.constEnd(); i++) {
		NewVector.replace(QRegExp(i.key() +"(?:\\[\\S+\\])?"), QString::number(i.value()*100.0,'f'));
	}
	BloombergVector Reslt(NewVector, m_AnchorDate);
	Reslt.ApplyFloorCap(ExtractFloorCapVector());
	return Reslt;
}
#ifndef NO_BLOOMBERG
BloombergVector BaseRateVector::GetRefRateValueFromBloomberg(ConstantBaseRateTable& Values)const {
	if(IsEmpty())return BloombergVector();
	QStringList RatesToDownload;
	for (int i = 0; i < NumElements();i++){
		if(
			!RatesToDownload.contains(GetValue(i))
			&& !Values.GetValues().contains(GetValue(i))
			) RatesToDownload.append(GetValue(i));
	}
	if (!RatesToDownload.isEmpty()) {
        QBbgLib::QBbgRequestGroup BbgReq;
        QHash<qint32, QPair<qint64, qint64> > RequestHash;
		QDate MinUpdateDate;
        QBbgLib::QBbgSecurity TempSecurity;
        TempSecurity.setExtension(QBbgLib::QBbgSecurity::Index);
        QBbgLib::QBbgReferenceDataRequest TempPriceRq, TempUpdateRq;
        TempPriceRq.setField("PX_LAST");
        TempUpdateRq.setField("LAST_UPDATE");
        QPair<qint64, qint64> TempIndexPair;
        for (int i = 0; i < RatesToDownload.size(); ++i) {
            TempSecurity.setName(RatesToDownload.at(i));
            TempPriceRq.setSecurity(TempSecurity);
            TempUpdateRq.setSecurity(TempSecurity);
            TempIndexPair.first = BbgReq.addRequest(TempPriceRq);
            TempIndexPair.second = BbgReq.addRequest(TempUpdateRq);
            RequestHash.insert(i, TempIndexPair);
        }
        QBbgLib::QBbgManager bbgMan;
        const auto & bbgResults=bbgMan.processRequest(BbgReq);
        const QBbgLib::QBbgReferenceDataResponse* PriceResponse;
        const QBbgLib::QBbgReferenceDataResponse* DateResponse;
        for (auto i = RequestHash.constBegin(); i != RequestHash.constEnd(); ++i) {
            PriceResponse = dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(bbgResults.value(i.value().first,nullptr));
            DateResponse = dynamic_cast<const QBbgLib::QBbgReferenceDataResponse*>(bbgResults.value(i.value().second, nullptr));
            if (Q_UNLIKELY(!PriceResponse)) 
                return BloombergVector();
            if (!PriceResponse->hasErrors()) {
                Values.GetValues().insert(RatesToDownload.at(i.key()), PriceResponse->value().toDouble() / 100.0);
                if (Q_UNLIKELY(!DateResponse))
                    continue;
                if (!DateResponse->hasErrors()) {
                    if (MinUpdateDate.isNull() || MinUpdateDate > DateResponse->value().toDate()) 
                        MinUpdateDate = DateResponse->value().toDate();
                }
            }
            else {
                return BloombergVector();
            }
        }
        if(MinUpdateDate.isValid())
            Values.SetUpdateDate(MinUpdateDate);
	}
	return CompileReferenceRateValue(Values);
}
BloombergVector BaseRateVector::GetRefRateValueFromBloomberg(ForwardBaseRateTable& Values)const {
	/*As currently the API does not support request for forward curves, the missing rates will be filled with constant values*/
	ConstantBaseRateTable BloombergRates(Values);
	GetRefRateValueFromBloomberg(BloombergRates);
	Values += BloombergRates;
	return CompileReferenceRateValue(Values);
}
#endif
#ifndef NO_DATABASE
BloombergVector BaseRateVector::GetBaseRatesDatabase(ConstantBaseRateTable& ReferencesValues, bool DownloadAll) const {
	if (IsEmpty()) return BloombergVector();
	bool AllRefFound = true;
	for (int i = 0; i < NumElements() && AllRefFound; i++) {
		if (!ReferencesValues.GetValues().contains(GetValue(i))) AllRefFound = false;
	}
	if (AllRefFound) return CompileReferenceRateValue(ReferencesValues);
	QDate MinUpdateDate;;
	QMutexLocker DbLocker(&Db_Mutex);
	QSqlDatabase db = QSqlDatabase::database("TwentyFourDB", false);
	if (!db.isValid()) {
		db = QSqlDatabase::addDatabase(GetFromConfig("Database", "DBtype"), "TwentyFourDB");
		db.setDatabaseName(
			"Driver={" + GetFromConfig("Database", "Driver")
			+ "}; "
			+ GetFromConfig("Database", "DataSource")
			);
		
	}
	bool DbOpen = db.isOpen();
	if (!DbOpen) DbOpen = db.open();
	if (DbOpen) {
		QSqlQuery query(db);
		query.setForwardOnly(true);
		query.prepare("{CALL " + GetFromConfig("Database", "ConstBaseRatesStoredProc") + "}");
		if (query.exec()) {
			while (query.next()) {
				if (
					!ReferencesValues.GetValues().contains(query.value(0).toString())
					&& (DownloadAll || m_VectVal.indexOf(QRegExp(query.value(0).toString() + "(?:\\[\\S+\\])?"))>=0)
					) {
					ReferencesValues.GetValues().insert(query.value(0).toString(), query.value(1).toDouble());
					if (MinUpdateDate.isNull() || query.value(2).toDateTime().date() < MinUpdateDate) MinUpdateDate = query.value(2).toDateTime().date();
				}
					
			}
			db.close();
			ReferencesValues.SetUpdateDate(MinUpdateDate);
			return CompileReferenceRateValue(ReferencesValues);
		}
		db.close();
	}
	return BloombergVector();
}
BloombergVector BaseRateVector::GetBaseRatesDatabase(ForwardBaseRateTable& ReferencesValues, bool DownloadAll) const {
	if (IsEmpty()) return BloombergVector();
	bool AllRefFound = true;
	for (int i = 0; i < NumElements() && AllRefFound; i++) {
		if (!ReferencesValues.GetValues().contains(GetValue(i))) AllRefFound = false;
	}
	if (AllRefFound) return CompileReferenceRateValue(ReferencesValues);
	QDate MinUpdateDate;
	QMutexLocker DbLocker(&Db_Mutex);
	QSqlDatabase db = QSqlDatabase::database("TwentyFourDB", false);
	if (!db.isValid()) {
		db = QSqlDatabase::addDatabase(GetFromConfig("Database", "DBtype"), "TwentyFourDB");
		db.setDatabaseName(
			"Driver={" + GetFromConfig("Database", "Driver")
			+ "}; "
			+ GetFromConfig("Database",  "DataSource")
			);
		
	}
	bool DbOpen = db.isOpen();
	if (!DbOpen) DbOpen = db.open();
	if (DbOpen) {
		QSqlQuery query(db);
		query.setForwardOnly(true);
		query.prepare("{CALL " + GetFromConfig("Database", "ForwardBaseRatesStoredProc") + "}");
		if (query.exec()) {
			QHash < QString, QHash<QDate, double> > QueryResults;
			while (query.next()) {
				if (
					!ReferencesValues.GetValues().contains(query.value(0).toString().trimmed().toUpper())
					&& (DownloadAll || m_VectVal.indexOf(QRegExp(query.value(0).toString() + "(?:\\[\\S+\\])?")) >= 0)
				) {
					QueryResults[query.value(0).toString().trimmed().toUpper()][query.value(1).toDateTime().date()] = query.value(2).toDouble();
					if (MinUpdateDate.isNull() || query.value(3).toDateTime().date() < MinUpdateDate) MinUpdateDate = query.value(3).toDateTime().date();
				}
					
			}
			db.close();
			for (QHash < QString, QHash<QDate, double> >::const_iterator i = QueryResults.constBegin(); i != QueryResults.constEnd(); ++i) {
				ReferencesValues.SetValue(i.key(), i.value().keys(), i.value().values());
			}
			/*const QList<QString> ReferencesList = QueryResults.keys();
			for (QList<QString>::ConstIterator i = ReferencesList.constBegin(); i != ReferencesList.constEnd(); i++) {
				QHash<QDate, double> CurrentResult = QueryResults.value(*i);
				ReferencesValues.GetValues().insert(*i, BloombergVector(CurrentResult.keys(), CurrentResult.values()));
			}*/
			ReferencesValues.SetUpdateDate(MinUpdateDate);
			return CompileReferenceRateValue(ReferencesValues);
		}
		db.close();
	}
	return BloombergVector();
}
#endif

FloorCapVector BaseRateVector::ExtractFloorCapVector() const {
	QString ResultStr=m_Vector;
	for (int i = 0; i < NumElements(); i++) {
		ResultStr.replace(
			QRegExp(GetValue(i) + "(?:\\[(\\S+,?\\S*)\\])?")
			, "[\\1]");
	}
	FloorCapVector Result(ResultStr);
	Result.SetAnchorDate(m_AnchorDate);
	return Result;
}



