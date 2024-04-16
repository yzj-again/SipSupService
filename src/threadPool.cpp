#include "threadPool.h"

// 1.pthread_mutex_t ThreadPool::m_queueLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ThreadPool::m_queueLock;
std::queue<ThreadTask *> ThreadPool::m_taskQueue;
ThreadPool::ThreadPool()
{
    pthread_mutex_init(&m_queueLock, NULL);
    sem_init(&m_signalSem, 0, 0);
}
ThreadPool::~ThreadPool()
{
    pthread_mutex_destroy(&m_queueLock);
    sem_destroy(&m_signalSem);
}

int ThreadPool::createThreadPool(int threadCount)
{
    if (threadCount <= 0)
    {
        LOG(ERROR) << "thread count error";
        return false;
    }
    for (int i = 0; i < threadCount; i++)
    {
        pthread_t pid;
        // 当前对象的指针作为入参
        if (EC::ECThread::createThread(ThreadPool::mainThread, (void *)this, pid) < 0)
        {
            LOG(ERROR) << "create thread error";
            return false;
        }
        LOG(INFO) << "thread:" << pid << "was created" << std::endl;
    }
    return true;
}

void *ThreadPool::mainThread(void *argc)
{
    ThreadPool *pthis = (ThreadPool *)argc;
    while (1)
    {
        // 阻塞
        int ret = pthis->waitTask();
        if (ret == 0)
        {
            ThreadTask *task = nullptr;
            pthread_mutex_lock(&m_queueLock);
            if (m_taskQueue.size() > 0)
            {
                task = m_taskQueue.front();
                m_taskQueue.pop();
            }
            pthread_mutex_unlock(&m_queueLock);
            if (task)
            {
                task->run();
                delete task;
            }
        }
    }
}

int ThreadPool::waitTask()
{
    int ret = 0;
    ret = sem_wait(&m_signalSem);
    if (ret != 0)
    {
        LOG(ERROR) << "the api exec error";
    }
    return ret;
}
int ThreadPool::postTask(ThreadTask *task)
{
    if (task)
    {
        pthread_mutex_lock(&m_queueLock);
        m_taskQueue.push(task);
        pthread_mutex_unlock(&m_queueLock);
        if (sem_post(&m_signalSem) == 0)
        {
            LOG(INFO) << "add Semaphore success";
        }
        else
        {
            LOG(ERROR) << "add Semaphore error";
        }
    }
    return true;
}