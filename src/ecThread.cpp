#include "ecThread.h"
using namespace EC;
int ECThread::createThread(ECThreadFunc startRoutine, void *args, pthread_t &id)
{
    int ret = 0;
    pthread_attr_t threadAttr;
    pthread_attr_init(&threadAttr);
    do
    {
        // 线程可分离
        ret = pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED);
        if (ret != 0)
        {
            break;
        }
        ret = pthread_create(&id, &threadAttr, startRoutine, args);
        if (ret != 0)
        {
            break;
        }
    } while (0);
    pthread_attr_destroy(&threadAttr);
    if (ret != 0)
    {
        ret = -1;
    }
    return ret;
}
int ECThread::detachSelf()
{
    int ret = pthread_detach(pthread_self());
    if (ret != 0)
    {
        ret = -1;
    }
    return 0;
}
void ECThread::exitSelf(void *rval)
{
    pthread_exit(rval);
}
int ECThread::waitThread(const pthread_t &id, void **rval)
{
    int ret = pthread_join(id, rval);
    if (ret != 0)
    {
        ret = -1;
    }
    return 0;
}
// 向指定线程发送退出信号
int ECThread::terminateThread(const pthread_t &id)
{
    int ret = pthread_cancel(id);
    if (ret != 0)
    {
        ret = -1;
    }
    return 0;
}