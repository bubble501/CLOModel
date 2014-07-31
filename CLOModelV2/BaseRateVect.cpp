#include "BaseRateVect.h"
#include <QRegExp>
#include <QStringList>
#include "CommonFunctions.h"
#include "BloombergWorker.h"
#ifndef NO_DATABASE
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSettings>
#include <QVariant>
#include <QSqlTableModel>
#include <QMap>
#endif
QString BaseRateVector::AvailableRatesToString(int a) const{
	/*
	To amend paste the enum value and then find and replace.
	Find pattern: :b*{.+},
	Replace Pattern: case \1: return \"\1\";
	*/
	switch(a){
		case EUR001W: return "EUR001W";
		case EUR002W: return "EUR002W";
		case EUR003W: return "EUR003W";
		case EUR001M: return "EUR001M";
		case EUR002M: return "EUR002M";
		case EUR003M: return "EUR003M";
		case EUR004M: return "EUR004M";
		case EUR005M: return "EUR005M";
		case EUR006M: return "EUR006M";
		case EUR007M: return "EUR007M";
		case EUR008M: return "EUR008M";
		case EUR009M: return "EUR009M";
		case EUR010M: return "EUR010M";
		case EUR011M: return "EUR011M";
		case EUR012M: return "EUR012M";
		case BP00ON: return "BP00ON";
		case BP0001W: return "BP0001W";
		case BP0001M: return "BP0001M";
		case BP0002M: return "BP0002M";
		case BP0003M: return "BP0003M";
		case BP0006M: return "BP0006M";
		case BP0012M: return "BP0012M";
		case US00ON: return "US00ON";
		case US0001W: return "US0001W";
		case US0001M: return "US0001M";
		case US0002M: return "US0002M";
		case US0003M: return "US0003M";
		case US0006M: return "US0006M";
		case US0012M: return "US0012M";
		case LIBOR01W: return "LIBOR01W";
		case LIBOR01M: return "LIBOR01M";
		case LIBOR02M: return "LIBOR02M";
		case LIBOR03M: return "LIBOR03M";
		case LIBOR06M: return "LIBOR06M";
		case LIBOR12M: return "LIBOR12M";
		case ZeroFlat: return "ZERO";
		default: return "";
	}
}
BaseRateVector::AvailableRates BaseRateVector::StringToAvailableRates(const QString& a) const{
	/*
	To amend paste the enum value and then find and replace.
	Find pattern: :b*{.+},
	Replace Pattern: else if(a==\"\1\") return \1;
	*/
	if(a=="EUR001W") return EUR001W;
	else if(a=="EUR002W") return EUR002W;
	else if(a=="EUR003W") return EUR003W;
	else if(a=="EUR001M") return EUR001M;
	else if(a=="EUR002M") return EUR002M;
	else if(a=="EUR003M") return EUR003M;
	else if(a=="EUR004M") return EUR004M;
	else if(a=="EUR005M") return EUR005M;
	else if(a=="EUR006M") return EUR006M;
	else if(a=="EUR007M") return EUR007M;
	else if(a=="EUR008M") return EUR008M;
	else if(a=="EUR009M") return EUR009M;
	else if(a=="EUR010M") return EUR010M;
	else if(a=="EUR011M") return EUR011M;
	else if(a=="EUR012M") return EUR012M;
	else if(a=="BP00ON") return BP00ON;
	else if(a=="BP0001W") return BP0001W;
	else if(a=="BP0001M") return BP0001M;
	else if(a=="BP0002M") return BP0002M;
	else if(a=="BP0003M") return BP0003M;
	else if(a=="BP0006M") return BP0006M;
	else if(a=="BP0012M") return BP0012M;
	else if(a=="US00ON") return US00ON;
	else if(a=="US0001W") return US0001W;
	else if(a=="US0001M") return US0001M;
	else if(a=="US0002M") return US0002M;
	else if(a=="US0003M") return US0003M;
	else if(a=="US0006M") return US0006M;
	else if(a=="US0012M") return US0012M;
	else if(a=="LIBOR01W") return LIBOR01W;
	else if(a=="LIBOR01M") return LIBOR01M;
	else if(a=="LIBOR02M") return LIBOR02M;
	else if(a=="LIBOR03M") return LIBOR03M;
	else if(a=="LIBOR06M") return LIBOR06M;
	else if(a=="LIBOR12M") return LIBOR12M;
	else if (a == "ZERO") return ZeroFlat;
	else return Invalid;
}
BaseRateVector::BaseRateVector(const QString& Vec)
	:AbstarctBbgVect(Vec)
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
	:AbstarctBbgVect(Vec)
{
	if (IsValid()){ UnpackVector(); m_AnchorDate = Anchor;}
	else {
		RemoveAnchorDate();
		m_Vector = "";
	}
	
}
bool BaseRateVector::IsValid() const{
	QString PossibleRatesPattern="(";
	QString TempPart;
	for(int i=0;true;i++){
		TempPart=AvailableRatesToString(i);
		if(TempPart.isEmpty()) break;
		if(i>0) PossibleRatesPattern+='|';
		PossibleRatesPattern+=TempPart;
	}
	PossibleRatesPattern+=')';
	return AbstarctBbgVect::IsValid(PossibleRatesPattern, true);
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
			m_VectVal.append(StringToAvailableRates(StringParts.at(i-1)));
		}
	}
	m_VectVal.append(StringToAvailableRates(StringParts.last()));
}
QDataStream& operator<<(QDataStream & stream, const BaseRateVector& flows){
	stream << flows.m_Vector
		<< flows.m_AnchorDate
		;
	return stream;
}
QDataStream& operator>>(QDataStream & stream, BaseRateVector& flows) { return flows.LoadOldVersion(stream); }
QDataStream& BaseRateVector::LoadOldVersion(QDataStream& stream) {
	stream
		>> m_Vector
		>> m_AnchorDate
		;
	UnpackVector();
	ResetProtocolVersion();
	return stream;
}

BaseRateVector::AvailableRates BaseRateVector::GetValue(const QDate& index)const{
	QDate ValidDate(m_AnchorDate);
	if(m_AnchorDate.isNull()) ValidDate=QDate::currentDate();
	return GetValue(MonthDiff(index,ValidDate));
}
BaseRateVector::AvailableRates BaseRateVector::GetValue(int index)const{
	if(m_VectVal.isEmpty() || index<0) return Invalid;
	return m_VectVal.at(qMin(index,m_VectVal.size()-1));
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
	return Result;
}
BloombergVector BaseRateVector::CompileReferenceRateValue(ConstantBaseRateTable& Values) const {
	if (IsEmpty())return BloombergVector();
	QString ResultingVector("");
	int StepCounter;
	double PreviousVal, TempValue;
	for (int i = 0; i < m_VectVal.size(); i++) {
		if (!Values.GetValues().contains(GetValueString(m_VectVal.at(i))))
#ifdef NO_BLOOMBERG
		return BloombergVector();
#else
		return GetRefRateValueFromBloomberg(Values);
#endif
		if (i > 0) {
			TempValue = Values.GetValues().value(GetValueString(m_VectVal.at(i)), 0.0);
			if (TempValue == PreviousVal) StepCounter++;
			else {
				ResultingVector += QString(" %1S %2").arg(StepCounter).arg(100.0*TempValue);
				PreviousVal = TempValue;
				StepCounter = 1;
			}

		}
		else {
			PreviousVal = Values.GetValues().value(GetValueString(m_VectVal.at(i)), 0.0);
			ResultingVector += QString("%1").arg(100.0*PreviousVal);
			StepCounter = 1;
		}

	}
	return BloombergVector(ResultingVector, m_AnchorDate);
}
#ifndef NO_BLOOMBERG
BloombergVector BaseRateVector::GetRefRateValueFromBloomberg(ConstantBaseRateTable& Values)const {
	if(IsEmpty())return BloombergVector();
	QStringList RatesToDownload;
	for(int i=0;i<m_VectVal.size();i++){
		if(
			!RatesToDownload.contains(GetValueString(i))
			&& !Values.GetValues().contains(GetValueString(i))
		) RatesToDownload.append(GetValueString(i));
	}
	if (!RatesToDownload.isEmpty()) {
		BloombergWorker Bee;
		QString CurrentResult;
		QDate MinUpdateDate, CurrentUpdateDate;
		foreach(const QString& SingleRate, RatesToDownload)
			Bee.AddSecurity(SingleRate, "Index");
		Bee.AddField("PX_LAST");
		Bee.AddField("PX_SETTLE_LAST_DT");
		BloombergResult ReturnedValues = Bee.StartRequest();
		for (int i = 0; i < m_VectVal.size(); i++) {
			CurrentResult = ReturnedValues.GetValue(GetValueString(i), "PX_LAST");
			if (!CurrentResult.isEmpty()) {
				Values.GetValues().insert(
					GetValueString(i),
					CurrentResult.toDouble() / 100.0
					);
				CurrentResult = ReturnedValues.GetValue(GetValueString(i), "PX_SETTLE_LAST_DT");
				if (!CurrentResult.isEmpty()) {
					CurrentUpdateDate = QDate::fromString(CurrentResult, "yyyy-MM-dd");
					if (MinUpdateDate.isNull() || MinUpdateDate > CurrentUpdateDate) MinUpdateDate = CurrentUpdateDate;
				}
			}
		}
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
	for (int i = 0; i < m_VectVal.size() && AllRefFound; i++) {
		if (!ReferencesValues.GetValues().contains(GetValueString(i))) AllRefFound = false;
	}
	if (AllRefFound) return CompileReferenceRateValue(ReferencesValues);
	QDate MinUpdateDate;
	QSettings ConfigIni(":/Configs/GlobalConfigs.ini", QSettings::IniFormat);
	ConfigIni.beginGroup("Database");
	QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
	db.setDatabaseName(
		"Driver={" + ConfigIni.value("Driver","SQL Server").toString() 
		+ "}; "
		+ ConfigIni.value("DataSource", "Server=SYNSERVER2\\SQLExpress;Initial Catalog=ABSDB;Integrated Security=SSPI;Trusted_Connection=Yes;").toString()
	);
	if (db.open()) {
		QSqlQuery query;
		query.setForwardOnly(true);
		query.prepare("{CALL "+ ConfigIni.value("ConstBaseRatesStoredProc", "getLatestIndexPrices").toString()+"}");
		if (query.exec()) {
			while (query.next()) {
				if (
					!ReferencesValues.GetValues().contains(query.value(0).toString())
					&& (DownloadAll || m_VectVal.contains(StringToAvailableRates(query.value(0).toString())))
					) {
					ReferencesValues.GetValues().insert(query.value(0).toString(), query.value(1).toDouble());
					if (MinUpdateDate.isNull() || query.value(2).toDateTime().date() < MinUpdateDate) MinUpdateDate = query.value(2).toDateTime().date();
				}
					
			}
			db.close();
			ConfigIni.endGroup();
			ReferencesValues.SetUpdateDate(MinUpdateDate);
			return CompileReferenceRateValue(ReferencesValues);
		}
	}
	return BloombergVector();
}
BloombergVector BaseRateVector::GetBaseRatesDatabase(ForwardBaseRateTable& ReferencesValues, bool DownloadAll) const {
	if (IsEmpty()) return BloombergVector();
	bool AllRefFound = true;
	for (int i = 0; i < m_VectVal.size() && AllRefFound; i++) {
		if (!ReferencesValues.GetValues().contains(GetValueString(i))) AllRefFound = false;
	}
	if (AllRefFound) return CompileReferenceRateValue(ReferencesValues);
	QDate MinUpdateDate;
	QSettings ConfigIni(":/Configs/GlobalConfigs.ini", QSettings::IniFormat);
	ConfigIni.beginGroup("Database");
	QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
	db.setDatabaseName(
		"Driver={" + ConfigIni.value("Driver", "SQL Server").toString()
		+ "}; "
		+ ConfigIni.value("DataSource", "Server=SYNSERVER2\\SQLExpress;Initial Catalog=ABSDB;Integrated Security=SSPI;Trusted_Connection=Yes;").toString()
		);
	if (db.open()) {
		QSqlQuery query;
		query.setForwardOnly(true);
		query.prepare("{CALL " + ConfigIni.value("ForwardBaseRatesStoredProc", "getForwardCurveMatrix").toString() + "}");
		if (query.exec()) {
			//QHash < QString, QMap<QDate, double> > QueryResults;
			QHash < QString, QHash<QDate, double> > QueryResults;
			while (query.next()) {
				if (
					!ReferencesValues.GetValues().contains(query.value(0).toString().trimmed().toUpper())
					&& (DownloadAll || m_VectVal.contains(StringToAvailableRates(query.value(0).toString())))
				) {
					QueryResults[query.value(0).toString().trimmed().toUpper()][query.value(1).toDateTime().date()] = query.value(2).toDouble();
					if (MinUpdateDate.isNull() || query.value(3).toDateTime().date() < MinUpdateDate) MinUpdateDate = query.value(3).toDateTime().date();
				}
					
			}
			db.close();
			ConfigIni.endGroup();
			const QList<QString> ReferencesList = QueryResults.keys();
			for (QList<QString>::ConstIterator i = ReferencesList.constBegin(); i != ReferencesList.constEnd(); i++) {
				/*const QMap<QDate, double> CurentCurve = QueryResults.value(*i);
				QString CurveVector = QString("%1").arg(100.0*CurentCurve.first());
				for (QMap<QDate, double>::ConstIterator CurveIter = CurentCurve.constBegin()+1; CurveIter != CurentCurve.constEnd(); CurveIter++) {
					CurveVector += QString(" %1R %2").arg(MonthDiff(CurveIter.key(), (CurveIter - 1).key())).arg(100.0*CurveIter.value());
				}
				ReferencesValues.GetValues().insert(*i, BloombergVector(CurveVector, CurentCurve.firstKey()));*/
				ReferencesValues.GetValues().insert(*i, BloombergVector(QueryResults.value(*i).keys(), QueryResults.value(*i).values()));
			}
			ReferencesValues.SetUpdateDate(MinUpdateDate);
			return CompileReferenceRateValue(ReferencesValues);
		}
	}
	return BloombergVector();
}

#endif