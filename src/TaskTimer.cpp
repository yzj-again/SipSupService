#include "TaskTimer.h"
#include "ecThread.h"
using namespace EC;
void *TaskTimer::timer(void *context)
{
    TaskTimer *pthis = (TaskTimer *)context;
    if (pthis == nullptr)
    {
        return nullptr;
    }
    unsigned long curTime = 0;
    unsigned long lastTime = 0;
    while (pthis->m_flag)
    {
        struct timeval current;
        gettimeofday(&current, NULL);
        curTime = current.tv_sec * 1000 + current.tv_usec / 1000;
        if (curTime - lastTime >= ((pthis->m_timeSecond) * 1000UL))
        {
            // 更新时间，执行定时器函数
            lastTime = curTime;
            if (pthis->m_timerFn != nullptr)
                pthis->m_timerFn(pthis->m_fnParam);
        }
        else
        {
            usleep(1000 * 1000);
            continue;
        }
    }
    return nullptr;
}
TaskTimer::TaskTimer(int timeSecond) : m_timeSecond(timeSecond)
{
    m_timerFn = nullptr;
    m_fnParam = nullptr;
    m_flag = true;
}
TaskTimer::~TaskTimer()
{
    stop();
}
void TaskTimer::start()
{
    pthread_t pid;
    int ret = EC::ECThread::createThread(TaskTimer::timer, (void *)this, pid);
    if (ret != 0)
    {
        LOG(ERROR) << "create thread failed";
    }
}
void TaskTimer::stop()
{
    m_flag = false;
}

// 设置一个定时执行任务的函数指针
void TaskTimer::setTimerFunc(timerCallBack fn, void *param)
{
    m_timerFn = fn;
    m_fnParam = param;
}