#ifndef NO_BLOOMBERG
#ifndef ManagedClasses_h__
#define ManagedClasses_h__
#include "QBbgRequest.h"
#include "QSingleBbgResult.h"
#include "QSingleBbgRequest.h"
#include "QBbgWorker.h"
using namespace System;
using namespace Collections::Generic;
namespace QBloombergLib {
	String^ Stdstr2String(const QString& a);
	QString String2Stdstr(String^ a);
	Dictionary<String^, String^>^ Map2DictionaryStr(const QHash<QString, QString>& a);
	QHash<QString, QString> Dictionary2MapStr(Dictionary<String^, String^>^ a);
	List<qint64>^ Deque2List(const QList<qint64>& a);
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	ref class ManQSingleBbgRequest;
	ref class ManBloombergResult;
	ref class ManQSingleBbgRequest;
	ref class ManQBbgResultsIterator;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	public ref class ManBloombergResult {
	internal:
		ManBloombergResult(ManBloombergResult^ a) { Unmanaged = new QSingleBbgResult(*(a->Unmanaged)); }
		ManBloombergResult(const QSingleBbgResult& a) { Unmanaged = new QSingleBbgResult(a); }
		QSingleBbgResult* Unmanaged;
	public:
		enum class BbgErrorCodes {
			NoErrors = 0
			, ResponseError = 0x1
			, SecurityError = 0x2
			, InvalidInputs = 0x4
			, SessionError = 0x8
			, ServiceError = 0x10
			, FieldError = 0x20
			, UnknownError = 0x40
			, SessionStopped = 0x80
			, NoData = 0x100
		};
		ManBloombergResult() { Unmanaged = new QSingleBbgResult(); }
		virtual ~ManBloombergResult() { delete Unmanaged; }
		bool IsEmpty() { return Unmanaged->IsEmpty(); }
		bool HasString() { return Unmanaged->HasString(); }
		bool HasDate() { return Unmanaged->HasDate(); }
		bool HasDouble() { return Unmanaged->HasDouble(); }
		bool HasTable() { return Unmanaged->HasTable(); }
		bool HasValue() { return Unmanaged->HasValue(); }
		String^ GetHeader() { return Stdstr2String(Unmanaged->GetHeader()); }
		String^ GetString() { return Stdstr2String(Unmanaged->GetString()); }
		DateTime GetDate() { return DateTime(Unmanaged->GetDate().year(), Unmanaged->GetDate().month(), Unmanaged->GetDate().day()); }
		double GetDouble() { return Unmanaged->GetDouble(); }
		qint32 GetNumRows() { return Unmanaged->GetNumRows(); }
		qint32 GetNumCols() { return Unmanaged->GetNumCols(); }
		qint32 GetErrorCode() { return Unmanaged->GetErrorCode(); }
		String^ GetErrorString() { return Stdstr2String(Unmanaged->GetErrorString()); }
		bool HasErrors() { return Unmanaged->HasErrors(); }
		ManBloombergResult^ GetTableResult(quint32 r, quint32 c) {
			const QSingleBbgResult* Temp = Unmanaged->GetTableResult(r, c);
			if (Temp) return gcnew ManBloombergResult(*Temp);
			return nullptr;
		}
	};
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	public ref class ManQBbgRequest {
	internal:
		ManQBbgRequest(ManQBbgRequest^ a) { Unmanaged = new QBbgRequest(*(a->Unmanaged)); }
		ManQBbgRequest(const QBbgRequest& a) { Unmanaged = new QBbgRequest(a); }
		QBbgRequest* Unmanaged;
	public:
		enum class YellowKeys {
			Invalid = -1
			, Govt
			, Corp
			, Mtge
			, MMkt
			, Muni
			, Pfd
			, Equity
			, Comdty
			, Index
			, Curncy
		};
		ManQBbgRequest() { Unmanaged = new QBbgRequest(); }
		virtual ~ManQBbgRequest() { delete Unmanaged; }
		qint32 NumRequests() { return Unmanaged->NumRequests(); }
		ManQSingleBbgRequest^ GetRequest(quint32 Index);
		ManQSingleBbgRequest^ FindRequest(qint64 ID);
		List<qint64>^ FindSecurity(String^ Secur) { return Deque2List(Unmanaged->FindSecurity(String2Stdstr(Secur))); }
		List<qint64>^ FindField(String^ Field) { return Deque2List(Unmanaged->FindField(String2Stdstr(Field))); }
		List<qint64>^ FindSecurityField(String^ Secur, String^ Field) { return Deque2List(Unmanaged->FindSecurityField(String2Stdstr(Secur), String2Stdstr(Field))); }
		List<qint64>^ IdList() { return Deque2List(Unmanaged->IdList()); }
		void AddRequest(ManQSingleBbgRequest^ a);
		void AddRequest(qint64 ID, String^ Secur, String^ Field, YellowKeys YellowKey) { Unmanaged->AddRequest(ID, String2Stdstr(Secur), String2Stdstr(Field), static_cast<QBbgRequest::YellowKeys>(static_cast<qint32>(YellowKey))); }
		void AddRequest(String^ Secur, String^ Field, YellowKeys YellowKey) { Unmanaged->AddRequest(String2Stdstr(Secur), String2Stdstr(Field), static_cast<QBbgRequest::YellowKeys>(static_cast<qint32>(YellowKey))); }
		void AddRequest(qint64 ID, String^ Secur, String^ Field, Dictionary<String^, String^>^ Overrides, YellowKeys YellowKey) { Unmanaged->AddRequest(ID, String2Stdstr(Secur), String2Stdstr(Field), Dictionary2MapStr(Overrides), static_cast<QBbgRequest::YellowKeys>(static_cast<qint32>(YellowKey))); }
		void AddRequest(String^ Secur, String^ Field, Dictionary<String^, String^>^ Overrides, YellowKeys YellowKey) { Unmanaged->AddRequest(String2Stdstr(Secur), String2Stdstr(Field), Dictionary2MapStr(Overrides), static_cast<QBbgRequest::YellowKeys>(static_cast<qint32>(YellowKey))); }
		void AddRequest(qint64 ID, String^ Secur, String^ Field) { AddRequest(ID, Secur, Field, YellowKeys::Mtge); }
		void AddRequest(String^ Secur, String^ Field) { AddRequest(Secur, Field, YellowKeys::Mtge); }
		void AddRequest(qint64 ID, String^ Secur, String^ Field, Dictionary<String^, String^>^ Overrides) { AddRequest(ID, Secur, Field, Overrides, YellowKeys::Mtge); }
		void AddRequest(String^ Secur, String^ Field, Dictionary<String^, String^>^ Overrides) { AddRequest(Secur, Field, Overrides, YellowKeys::Mtge); }
		bool IsValidReq() { return Unmanaged->IsValidReq(); }
		void SetAutoConstantRates(bool val) { Unmanaged->SetAutoConstantRates(val); }
		bool GetAutoConstantRates() { return Unmanaged->GetAutoConstantRates(); }
		void ClearRequests() { Unmanaged->ClearRequests(); }

	};
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	public ref class ManQSingleBbgRequest {
	internal:
		ManQSingleBbgRequest(ManQSingleBbgRequest^ a) { Unmanaged = new QSingleBbgRequest(*(a->Unmanaged)); }
		ManQSingleBbgRequest(const QSingleBbgRequest& a) { Unmanaged = new QSingleBbgRequest(a); }
		QSingleBbgRequest* Unmanaged;
	public:
		ManQSingleBbgRequest() { Unmanaged = new QSingleBbgRequest(); }
		virtual ~ManQSingleBbgRequest() { delete Unmanaged; }
		qint64 GetResultID() { return Unmanaged->GetResultID(); }
		String^ GetSecurity() { return Stdstr2String(Unmanaged->GetSecurity()); }
		String^ GetField() { return Stdstr2String(Unmanaged->GetField()); }
		bool GetAutoConstantRates() { return Unmanaged->GetAutoConstantRates(); }
		void SetAutoConstantRates(bool val) { Unmanaged->SetAutoConstantRates(val); }
		void SetResultID(qint64 val) { Unmanaged->SetResultID(val); }
		void SetSecurity(String^ val) { Unmanaged->SetSecurity(String2Stdstr(val)); }
		void SetField(String^ val) { Unmanaged->SetField(String2Stdstr(val)); }
		Dictionary<String^, String^>^ GetOverrides() { return Map2DictionaryStr(Unmanaged->GetOverrides()); }
		void SetOverrides(Dictionary<String^, String^>^ Overrides) { Unmanaged->SetOverrides(Dictionary2MapStr(Overrides)); }
		void SetOverride(String^ Name, String^ Value) { Unmanaged->SetOverride(String2Stdstr(Name), String2Stdstr(Value)); }
		void ClearOverrides() { Unmanaged->ClearOverrides(); }
		bool IsValidReq() { return Unmanaged->IsValidReq(); }
		String^ GetExtensionString() { return Stdstr2String(Unmanaged->GetExtensionString()); }
		String^ GetFullSecurity() { return Stdstr2String(Unmanaged->GetFullSecurity()); }
		ManQBbgRequest::YellowKeys GetExtension() { return static_cast<ManQBbgRequest::YellowKeys>(static_cast<qint32>(Unmanaged->GetExtension())); }
		void SetExtension(String^ val) { Unmanaged->SetField(String2Stdstr(val)); }
		void SetExtension(ManQBbgRequest::YellowKeys val) { Unmanaged->SetExtension(static_cast<QBbgRequest::YellowKeys>(static_cast<qint32>(val))); }
	};
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	public ref class  ManQBbgResultsIterator {
	internal:
		QBbgResultsIterator* Unmanaged;
		ManQBbgResultsIterator(ManQBbgResultsIterator^ a) { Unmanaged = new QBbgResultsIterator(*(a->Unmanaged)); }
		ManQBbgResultsIterator(const QBbgResultsIterator& a) { Unmanaged = new QBbgResultsIterator(a); }
	public:
		//ManQBbgResultsIterator() { Unmanaged = new QBbgResultsIterator(); }
		virtual ~ManQBbgResultsIterator() { delete Unmanaged; }
		ManQBbgResultsIterator^ operator++() { ++(*Unmanaged); return this; }
		ManQBbgResultsIterator^ operator--() { --(*Unmanaged); return this; }
		ManQBbgResultsIterator^ operator++(int) { return gcnew ManQBbgResultsIterator((*Unmanaged)++); }
		ManQBbgResultsIterator^ operator--(int) { return gcnew ManQBbgResultsIterator((*Unmanaged)--); }
		bool IsValid() { return Unmanaged->IsValid(); }
		qint64 ResultID() { return Unmanaged->ResultID(); }
		ManBloombergResult^ Value() { return gcnew ManBloombergResult(*(Unmanaged->operator*())); }
	};
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	public ref class ManQBbgWorker {
	internal:
		QBbgWorker* Unmanaged;
	public:
		ManQBbgWorker() { Unmanaged = new QBbgWorker(); }
		virtual ~ManQBbgWorker() { delete Unmanaged; }
		String^ GetServerAddress() { return Stdstr2String(Unmanaged->GetServerAddress()); }
		void SetServerAddress(String^ val) { Unmanaged->SetServerAddress(String2Stdstr(val)); }
		unsigned short GetServerPort() { return Unmanaged->GetServerPort(); }
		void SetServerPort(unsigned short val) { Unmanaged->SetServerPort(val); }
		void SetRequest(ManQBbgRequest^ a) { Unmanaged->SetRequest(*(a->Unmanaged)); }
		void StartRequestSync() {
			Unmanaged->StartRequestSync();
		}
		void StartRequestSync(ManQBbgRequest^ a) { Unmanaged->StartRequestSync(*(a->Unmanaged)); }
		ManQBbgRequest^ GetRequest() { return gcnew ManQBbgRequest(Unmanaged->GetRequest()); }
		ManBloombergResult^ GetResult(qint64 ID) {
			const QSingleBbgResult* Temp = Unmanaged->GetResult(ID);
			if (Temp)
				return gcnew ManBloombergResult(*Temp);
			return nullptr;
		}
		bool isRunning() { return Unmanaged->isRunning(); }
		void StopRequest() { Unmanaged->StopRequest(); }
		void StartRequestAsync() { Unmanaged->StartRequestAsync(); }
		void StartRequestAsync(ManQBbgRequest^ a) { Unmanaged->StartRequestAsync(*(a->Unmanaged)); }
		ManQBbgResultsIterator^ GetResultIterator() { return gcnew ManQBbgResultsIterator(Unmanaged->GetResultIterator()); }
	};
	
}
#endif // ManagedClasses_h__
#endif