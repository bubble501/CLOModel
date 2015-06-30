#include "BaseRateVect.h"
#include "Private/BaseRateVect_p.h"
#include <QRegExp>
#include <QStringList>
#include "CommonFunctions.h"
#include "Private/InternalItems.h"
#include "BaseRateTable.h"
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
DEFINE_PUBLIC_COMMONS(BaseRateVector)
DEFINE_PUBLIC_COMMONS_COPY(BaseRateVector)
BaseRateVectorPrivate::BaseRateVectorPrivate(BaseRateVector *q,const BaseRateVectorPrivate& other)
	:AbstractBbgVectPrivate(q,other)
    , m_VectVal(other.m_VectVal)
{}
BaseRateVectorPrivate::BaseRateVectorPrivate(BaseRateVector *q)
	:AbstractBbgVectPrivate(q)
{}
BaseRateVector::BaseRateVector(BaseRateVectorPrivate *d, const BaseRateVector& other)
	:AbstractBbgVect(d,other)
{}
BaseRateVector::BaseRateVector(BaseRateVectorPrivate *d)
	:AbstractBbgVect(d)
{}
BaseRateVector& BaseRateVector::operator=(const BaseRateVector& other)
{
    AbstractBbgVect::operator=(other);
    Q_D(BaseRateVector);
    d->m_VectVal = other.d_func()->m_VectVal;
    return *this;
}

BaseRateVector::BaseRateVector(const QString& Vec)
    :BaseRateVector()
{
    SetVector(Vec);
}


BaseRateVector::BaseRateVector(const QString& Vec,const QDate& Anchor)
    :BaseRateVector(Vec)
{
    SetAnchorDate(Anchor);
}
bool BaseRateVector::IsValid() const{
    Q_D(const BaseRateVector);
	if (!AbstractBbgVect::IsValid(R"**([^\s,\[\]]+(?:\[(?:(?:-?\d*\.?\d+)|(?:(?:-?\d*\.?\d+)?(?:,-?\d*\.?\d+)))\])?)**", false)) return false;
	QRegExp rx("\\[(-?\\d*\\.?\\d+),(-?\\d*\\.?\\d+)\\]");
	for (int pos = 0; (pos = rx.indexIn(d->m_Vector, pos)) >= 0; pos += rx.matchedLength()) {
		if (rx.cap(1).toDouble() > rx.cap(2).toDouble()) return false;
	}
	return true;
}

bool BaseRateVector::IsZero() const
{
    Q_D(const BaseRateVector);
    return d->m_Vector.trimmed().toUpper() == "ZERO";
}

QRegExpValidator* BaseRateVector::GetValidator(QObject* parent) const
{
	return AbstractBbgVect::GetValidator(R"**([^\s,\[\]]+(?:\[(?:(?:-?\d*\.?\d+)|(?:(?:-?\d*\.?\d+)?(?:,-?\d*\.?\d+)))\])?)**", false, parent);
}

int BaseRateVector::NumElements() const
{
    Q_D(const BaseRateVector);
    return d->m_VectVal.size();
}

QString BaseRateVector::GetValue(const QDate& index) const
{
    Q_D(const BaseRateVector);
    return d->GetValue(index);
}
QString BaseRateVector::GetValue(int index) const
{
    Q_D(const BaseRateVector);
    return d->GetValue(index);
}
QString BaseRateVector::GetFloor(const QDate& index) const
{
    Q_D(const BaseRateVector);
    return d->GetFloor(index);
}
QString BaseRateVector::GetFloor(int index) const
{
    Q_D(const BaseRateVector);
    return d->GetFloor(index);
}
QString BaseRateVector::GetCap(const QDate& index) const
{
    Q_D(const BaseRateVector);
    return d->GetCap(index);
}
QString BaseRateVector::GetCap(int index) const
{
    Q_D(const BaseRateVector);
    return d->GetCap(index);
}

void BaseRateVector::UnpackVector()
{
    Q_D(BaseRateVector);
    d->m_VectVal.clear();
    if (d->m_Vector.isEmpty()) return;
	ExtractAnchorDate();
    QString TempVec(d->m_Vector.trimmed().toUpper());
	QStringList StringParts=TempVec.trimmed().toUpper().split(QRegExp("\\s"),QString::SkipEmptyParts);
	int StepLen;
	QString TempStr;
	for (int i=1;i<StringParts.size();i+=2){
		TempStr=StringParts.at(i);
		TempStr.replace(QRegExp("\\D"),"");
		StepLen=TempStr.toInt();
		for (int j=0;j<StepLen;j++){
            d->m_VectVal.append(StringParts.at(i - 1));
		}
	}
    d->m_VectVal.append(StringParts.last());
}
QDataStream& operator<<(QDataStream & stream, const BaseRateVector& flows){
	stream 
		<< flows.d_func()->m_VectVal
        << flows.d_func()->m_Vector
        << flows.d_func()->m_AnchorDate
		;
	return stream;
}
QDataStream& operator>>(QDataStream & stream, BaseRateVector& flows) { return flows.LoadOldVersion(stream); }
QDataStream& BaseRateVector::LoadOldVersion(QDataStream& stream) {
    Q_D(BaseRateVector);
	stream
        >> d->m_VectVal
        >> d->m_Vector
        >> d->m_AnchorDate
		;
	ResetProtocolVersion();
	return stream;
}

void BaseRateVector::RepackVector()
{
    Q_D(BaseRateVector);
    QString newVector;
    QString lastUsed;
    int stepCounter = 1;
    for (auto i = d->m_VectVal.constBegin(); i != d->m_VectVal.constEnd(); ++i) {
        if (lastUsed.isEmpty()) {
            lastUsed = *i;
            newVector.append(lastUsed);
        }
        else if (*i==lastUsed) {
            ++stepCounter;
        }
        else {
            newVector += QString(" %1S %2").arg(stepCounter).arg(*i);
            lastUsed = *i;
            stepCounter = 1;
        }
    }
    d->m_Vector = newVector;
    Q_ASSERT(IsValid());
}

BloombergVector BaseRateVector::CompileReferenceRateValue(ForwardBaseRateTable& Values) const
{
    Q_D(const BaseRateVector);
	BloombergVector Result;
	if (IsEmpty()) return Result;
    QStringList StringParts = d->m_Vector.trimmed().toUpper().split(QRegExp("\\s"), QString::SkipEmptyParts);
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
    Q_D(const BaseRateVector);
	if (IsEmpty())return BloombergVector();
	QString ResultingVector("");
	for (int i = 0; i < NumElements(); i++) {
		if (!Values.Contains(GetValue(i))) {
			return GetRefRateValueFromBloomberg(Values);
		}
	}
	const QHash<QString, double>& Sobstitutes = Values.GetValues();
	QString NewVector = d->m_Vector;
	for (QHash<QString, double>::const_iterator i = Sobstitutes.constBegin(); i != Sobstitutes.constEnd(); i++) {
		NewVector.replace(QRegExp(i.key() +"(?:\\[\\S+\\])?"), QString::number(i.value()*100.0,'f'));
	}
    BloombergVector Reslt(NewVector, d->m_AnchorDate);
	Reslt.ApplyFloorCap(ExtractFloorCapVector());
	return Reslt;
}

BloombergVector BaseRateVector::GetRefRateValueFromBloomberg(ConstantBaseRateTable& Values)const {
#ifdef NO_BLOOMBERG
    return BloombergVector();
#else
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
#endif
}
BloombergVector BaseRateVector::GetRefRateValueFromBloomberg(ForwardBaseRateTable& Values)const {
#ifdef NO_BLOOMBERG
    return BloombergVector();
#else
	/*As currently the API does not support request for forward curves, the missing rates will be filled with constant values*/
	ConstantBaseRateTable BloombergRates(Values);
	GetRefRateValueFromBloomberg(BloombergRates);
	Values += BloombergRates;
	return CompileReferenceRateValue(Values);
#endif
}


BloombergVector BaseRateVector::GetBaseRatesDatabase(ConstantBaseRateTable& ReferencesValues, bool DownloadAll) const {
#ifdef NO_DATABASE
    return BloombergVector();
#else
    Q_D(const BaseRateVector);
	if (IsEmpty()) return BloombergVector();
	bool AllRefFound = true;
	for (int i = 0; i < NumElements() && AllRefFound; i++) {
		if (!ReferencesValues.GetValues().contains(GetValue(i))) AllRefFound = false;
	}
	if (AllRefFound) 
        return CompileReferenceRateValue(ReferencesValues);
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
					&& (DownloadAll || d->m_VectVal.indexOf(QRegExp(query.value(0).toString() + "(?:\\[\\S+\\])?"))>=0)
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
#endif
}
BloombergVector BaseRateVector::GetBaseRatesDatabase(ForwardBaseRateTable& ReferencesValues, bool DownloadAll) const {
#ifdef NO_DATABASE
    return BloombergVector();
#else
    Q_D(const BaseRateVector);
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
                    && (DownloadAll || d->m_VectVal.indexOf(QRegExp(query.value(0).toString() + "(?:\\[\\S+\\])?")) >= 0)
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
#endif
}

BaseRateVector& BaseRateVector::operator=(const QString& a)
{
    AbstractBbgVect::operator=(a); return *this;
}


int BaseRateVector::replaceValue(const QString& oldVal, BaseRateVector newVal, bool replaceFloorCaps /*= true*/)
{
    Q_D(BaseRateVector);
    if (!BaseRateVector(oldVal).IsValid() || !newVal.IsValid() || !d->m_Vector.contains(oldVal))
        return 0;
    QRegExp subOld(oldVal.trimmed());
    if (replaceFloorCaps){
        subOld.setPattern(oldVal.trimmed()+ R"**((?:\[(?:(?:-?\d*\.?\d+)|(?:(?:-?\d*\.?\d+)?(?:,-?\d*\.?\d+)))\])?)**");
    }
    if (d->m_AnchorDate.isNull()) 
        newVal.RemoveAnchorDate();
    else if(newVal.GetAnchorDate().isNull())
        newVal.SetAnchorDate(d->m_AnchorDate);
    QStringList newVector;
    QString newValue;
    int countReplaced = 0;
    for (qint32 i = 0; i < d->m_VectVal.size(); ++i) {
        if (subOld.indexIn(d->m_VectVal.at(i)) >= 0) {
            ++countReplaced;
            if (d->m_AnchorDate.isNull()) {
                if (replaceFloorCaps)
                    newValue = newVal.d_func()->m_VectVal.at(qMin(i, newVal.d_func()->m_VectVal.size() - 1));
                else
                    newValue = newVal.GetValue(i);
            }
            else {
                newValue = newVal.GetValue(d->m_AnchorDate.addMonths(i));
                if (replaceFloorCaps) {
                    QString curCap = newVal.GetCap(d->m_AnchorDate.addMonths(i));
                    QString curFloor = newVal.GetFloor(d->m_AnchorDate.addMonths(i));
                    if (!(curCap.isEmpty() && curFloor.isEmpty()))
                        newValue += QString("[%1%2%3]").arg(curFloor).arg(curCap.isEmpty() ? "" : ",").arg(curCap);
                }
            }
            d->m_VectVal[i].replace(subOld, newValue);
        }
    }
    RepackVector();
    return countReplaced;
}

FloorCapVector BaseRateVector::ExtractFloorCapVector() const
{
    Q_D(const BaseRateVector);
    QString ResultStr = d->m_Vector;
	for (int i = 0; i < NumElements(); i++) {
		ResultStr.replace(
			QRegExp(GetValue(i) + "(?:\\[(\\S+,?\\S*)\\])?")
			, "[\\1]");
	}
	FloorCapVector Result(ResultStr);
    Result.SetAnchorDate(d->m_AnchorDate);
	return Result;
}



