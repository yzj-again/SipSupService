#ifndef _ECTHREAD_H
#define _ECTHREAD_H
#include <pthread.h>
#include <string>
#include <sys/prctl.h>
namespace EC
{
    // 线程主函数入口
    typedef void *(*ECThreadFunc)(void *);
    // 无需对外提供实例，方法全是私有的
    class ECThread
    {

    public:
        static int createThread(ECThreadFunc startRoutine, void *args, pthread_t &id);
        static int detachSelf();
        static void exitSelf(void *rval);
        static int waitThread(const pthread_t &id, void **rval);
        static int terminateThread(const pthread_t &id);

    private:
        ECThread(){};
        ~ECThread(){};
    };
} // namespace EC

#endif // !_ECTHREAD_H