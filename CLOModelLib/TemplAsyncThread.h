#ifndef TemplAsyncThread_h__
#define TemplAsyncThread_h__
#include "AbstrAsyncThread.h"
#include "CommonFunctions.h"
template <typename ResultType>
class  CLOMODELLIB_EXPORT TemplAsyncThread : public AbstrAsyncThread
{
protected:
	std::shared_ptr<ResultType> castResult(){
        return std::static_pointer_cast<ResultType>(d_func()->m_Result);
    }
    TemplAsyncThread(AbstrAsyncThreadPrivate* d, int ID, QObject* parent = nullptr)
        :AbstrAsyncThread(d,ID, parent)
    {
        static_assert(std::is_default_constructible<ResultType>::value, "ResultType must implement a default constructor");
        static_assert(std::is_assignable<ResultType, const ResultType&>::value, "ResultType must implement an assignment operator");
        d_func()->m_Result = std::make_shared<ResultType>();
        if (QMetaType::type(typeid(ResultType).name()) == QMetaType::UnknownType) {
            qRegisterMetaType<ResultType>(typeid(ResultType).name());
            qRegisterMetaTypeStreamOperators<ResultType>(typeid(ResultType).name());
        }
    }
public:
    virtual ~TemplAsyncThread(){}
	TemplAsyncThread(int ID, QObject* parent = 0) 
        : AbstrAsyncThread(ID,parent)
    {   
		static_assert(std::is_default_constructible<ResultType>::value, "ResultType must implement a default constructor"); 
		static_assert(std::is_assignable<ResultType, const ResultType&>::value, "ResultType must implement an assignment operator");
        d_func()->m_Result = new ResultType();
        if (QMetaType::type(typeid(ResultType).name()) == QMetaType::UnknownType) {
            qRegisterMetaType<ResultType>(typeid(ResultType).name());
            qRegisterMetaTypeStreamOperators<ResultType>(typeid(ResultType).name());
        }
	}
};

#endif // TemplAsyncThread_h__

