#ifndef TemplAsyncThread_h__
#define TemplAsyncThread_h__
#include "AbstrAsyncThread.h"
#include "CommonFunctions.h"
#include "Private/AbstrAsyncThread_p.h"
template <typename ResultType>
class  CLOMODELLIB_EXPORT TemplAsyncThread : public AbstrAsyncThread
{
protected:
	ResultType* castResult(){
        return static_cast<ResultType*>(d_func()->m_Result);
    }
    TemplAsyncThread(AbstrAsyncThreadPrivate* d, int ID, QObject* parent = nullptr)
        :AbstrAsyncThread(d,ID, parent)
    {
        static_assert(std::is_default_constructible<ResultType>::value, "ResultType must implement a default constructor");
        static_assert(std::is_assignable<ResultType, const ResultType&>::value, "ResultType must implement an assignment operator");
        d_func()->m_Result = new ResultType();
        RegisterAsMetaType<ResultType>();
    }
public:
	TemplAsyncThread(int ID, QObject* parent = 0) 
        : AbstrAsyncThread(ID,parent)
    {   
		static_assert(std::is_default_constructible<ResultType>::value, "ResultType must implement a default constructor"); 
		static_assert(std::is_assignable<ResultType, const ResultType&>::value, "ResultType must implement an assignment operator");
        d_func()->m_Result = new ResultType();
		RegisterAsMetaType<ResultType>();
	}
};

#endif // TemplAsyncThread_h__

