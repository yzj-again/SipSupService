#ifndef _THREADPOOL_H
#define _THREADPOOL_H
#include "common.h"
#include "ecThread.h"
#include <queue>
#include <unistd.h>
#include <semaphore.h>
using namespace EC;

class ThreadTask
{
public:
    ThreadTask(){};
    virtual ~ThreadTask(){};
    virtual void run() = 0;
};

// 线程池的类
class ThreadPool
{
public:
    ThreadPool();
    ~ThreadPool();

    int createThreadPool(int threadCount);
    // 线程入口函数
    static void *mainThread(void *argc);
    // 阻塞
    int waitTask();
    // 添加任务，通知解除阻塞
    int postTask(ThreadTask *task);
    static std::queue<ThreadTask *> m_taskQueue;
    static pthread_mutex_t m_queueLock;

private:
    // 信号量
    sem_t m_signalSem;
};
#endif // !_THREADPOOL_H