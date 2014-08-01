#ifndef NO_BLOOMBERG
#include "BloombergResult.h"
BloombergResult::BloombergResult()
	: m_StringValue(NULL)
	,m_DateValue(NULL)
	,m_DoubleValue(NULL)
	, TableResultCols(0)
{

}

BloombergResult::BloombergResult(const BloombergResult& a) 
	: m_StringValue(NULL)
	, m_DateValue(NULL)
	, m_DoubleValue(NULL)
	, TableResultCols(0) 
{
	operator=(a);
}
BloombergResult& BloombergResult::operator= (const BloombergResult& a) {
	Clear();
	m_Header = a.m_Header;
	if (a.m_StringValue) m_StringValue = new QString(*(a.m_StringValue));
	else m_StringValue = NULL;
	if (a.m_DateValue) m_DateValue = new QDate(*(a.m_DateValue));
	else m_DateValue = NULL;
	if (a.m_DoubleValue) m_DoubleValue = new double(*(a.m_DoubleValue));
	else m_DoubleValue = NULL;
	if (a.HasTable()) {
		TableResultCols = a.TableResultCols;
		for (QList<BloombergResult*>::const_iterator i = a.m_TableResultRows.constBegin(); i != a.m_TableResultRows.constEnd(); i++) {
			m_TableResultRows.append(new BloombergResult[TableResultCols]);
			for (int j = 0; j < TableResultCols; j++) {
				if (((*i)->m_StringValue + j))
					(m_TableResultRows.last() + j)->SetValue(*((*i)->m_StringValue + j));
			}
		}
	}
	else {
		TableResultCols = 0;
	}
	return *this;
}

BloombergResult::~BloombergResult() {
	Clear();
}
void BloombergResult::Clear() {
	if (m_StringValue) delete m_StringValue;
	if (m_DateValue)delete m_DateValue;
	if (m_DoubleValue)delete m_DoubleValue;
	while (!m_TableResultRows.isEmpty())
		delete [] m_TableResultRows.takeFirst();
	m_StringValue = NULL;
	m_DateValue = NULL;
	m_DoubleValue = NULL;
	TableResultCols = 0;
	m_Header = "";
}
void BloombergResult::SetValue(const QString& val, const QString& Header) {
	if (val.isEmpty()) return;
	Clear();
	m_Header = Header;
	m_StringValue = new QString(val);
	QDate TempDate=	QDate::fromString(val, "yyyy-MM-dd");
	if (!TempDate.isNull()) m_DateValue = new QDate(TempDate);
	bool DoubleCheck;
	double TempDouble = val.toDouble(&DoubleCheck);
	if (DoubleCheck) m_DoubleValue = new double(TempDouble);
}
void BloombergResult::AddValueRow(const QStringList& val, const QStringList& Headers) {
	if (val.isEmpty()) return;
	bool GoodHead = Headers.size() == val.size();
	if (m_TableResultRows.isEmpty()) {
		Clear();
		TableResultCols = val.size();
	}
	else {
		if (TableResultCols != val.size()) return;
	}
	m_TableResultRows.append(new BloombergResult[TableResultCols]);
	for (int i = 0; i < TableResultCols; i++) {
		(m_TableResultRows.last() + i)->SetValue(val.at(i), GoodHead ? Headers.at(i):"");
	}
}

QDataStream& BloombergResult::LoadOldVersion(QDataStream& stream) {
	Clear();
	bool DateFound, StringFound, DoubleFound;
	int Rows;
	QString TempStr;
	QDate TempDte;
	double Tempdbl;
	stream
		>> StringFound
		>> DateFound
		>> DoubleFound
		>> Rows
		>> TableResultCols
		>> m_Header
		;
	if (StringFound) {
		stream >> TempStr;
		m_StringValue = new QString(TempStr);
	}
	if (DateFound) {
		stream >> TempDte;
		m_DateValue = new QDate(TempDte);
	}
	if (DoubleFound) {
		stream >> Tempdbl;
		m_DoubleValue = new double(Tempdbl);
	}
	BloombergResult* TempRes = new BloombergResult[TableResultCols];
	for (int i = 0; i < Rows; i++) {
		for (int j = 0; j < TableResultCols; j++) {
			stream >> TempRes[j];
		}
		m_TableResultRows.append(TempRes);
	}
	return stream;
}

QDataStream& operator<<(QDataStream & stream, const BloombergResult& flows) {
	stream << flows.HasString()
		<< flows.HasDate()
		<< flows.HasDouble()
		<< flows.m_TableResultRows.size()
		<< flows.TableResultCols
		<< flows.m_Header
		;
	if (flows.HasString()) stream << *(flows.m_StringValue);
	if (flows.HasDate()) stream << *(flows.m_DateValue);
	if (flows.HasDouble()) stream << *(flows.m_DoubleValue);
	foreach(const BloombergResult* TblElement, flows.m_TableResultRows) {
		for (int j = 0; j < flows.TableResultCols; j++) {
			stream << *(TblElement + j);
		}
	}
	return stream;
}
QDataStream& operator>>(QDataStream & stream, BloombergResult& flows) { return flows.LoadOldVersion(stream); }

#endif

