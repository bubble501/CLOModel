#ifndef TemplAsyncThread_h__
#define TemplAsyncThread_h__
#include "AbstrAsyncThread.h"
template <typename ResultType>
class TemplAsyncThread : public AbstrAsyncThread {
protected:
	int Identifier;
	ResultType m_Result;
public:
	TemplAsyncThread(int ID, QObject* parent = 0) : Identifier(ID), AbstrAsyncThread(parent) { 
		static_assert(std::is_default_constructible<ResultType>::value, "ResultType must implement a default constructor"); 
		static_assert(std::is_assignable<ResultType, const ResultType&>::value, "ResultType must implement an assignment operator");
		RegisterAsMetaType<ResultType>();
	}
};

#endif // TemplAsyncThread_h__

