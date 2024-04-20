#ifndef _TASKTIMER_H
#define _TASKTIMER_H
#include <unistd.h>
#include <sys/time.h>
#include "common.h"
typedef void (*timerCallBack)(void *param);
class TaskTimer
{

public:
    TaskTimer(int timeSecond);
    ~TaskTimer();
    void start();
    void stop();
    // 设置一个定时执行任务的函数指针
    void setTimerFunc(timerCallBack fn, void *param);
    // 指定一个线程入口
    static void *timer(void *context);

private:
    timerCallBack m_timerFn;
    void *m_fnParam;
    int m_timeSecond;
    bool m_flag;
};

#endif // !_TASKTIMER_H
