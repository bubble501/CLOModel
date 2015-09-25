#include "AbstrAsyncThread.h"
#include "Private/AbstrAsyncThread_p.h"
AbstrAsyncThreadPrivate::AbstrAsyncThreadPrivate(AbstrAsyncThread *q)
    :q_ptr(q)
    , m_Result(nullptr)
{}
AbstrAsyncThreadPrivate::~AbstrAsyncThreadPrivate()
{
}
AbstrAsyncThread::AbstrAsyncThread(int ID, QObject* parent)
    :AbstrAsyncThread(new AbstrAsyncThreadPrivate(this), ID, parent)
{}
AbstrAsyncThread::AbstrAsyncThread(AbstrAsyncThreadPrivate* d, int ID, QObject* parent)
    :QThread(parent)
    ,d_ptr(d)
{
    d->Identifier = ID;
    connect(this, SIGNAL(AnonimCalculated(int)), this, SLOT(EmitFromAnonim()));
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
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
