#include "AbstrAsyncThread.h"
#include "Private/AbstrAsyncThread_p.h"
AbstrAsyncThreadPrivate::~AbstrAsyncThreadPrivate(){}
AbstrAsyncThreadPrivate::AbstrAsyncThreadPrivate(AbstrAsyncThread *q)
    :q_ptr(q)
    , m_Result(nullptr)
{}

AbstrAsyncThread::AbstrAsyncThread(int ID, QObject* parent)
    :AbstrAsyncThread(new AbstrAsyncThreadPrivate(this), ID, parent)
{}
AbstrAsyncThread::AbstrAsyncThread(AbstrAsyncThreadPrivate* d, int ID, QObject* parent)
    :QThread(parent)
    ,d_ptr(d)
{
    d->Identifier = ID;
    connect(this, &AbstrAsyncThread::AnonimCalculated, this, &AbstrAsyncThread::EmitFromAnonim);
    connect(this, &AbstrAsyncThread::finished, this, &AbstrAsyncThread::deleteLater);
}

AbstrAsyncThread::~AbstrAsyncThread()
{
    while (isRunning()) {
        wait();
    }
    delete d_ptr;
}

void AbstrAsyncThread::stop()
{
    exit(0);
}
