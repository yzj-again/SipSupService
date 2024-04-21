#ifndef _COMMON_H
#define _COMMON_H
#include <glog/logging.h>
#include "tinyxml2.h"
#include <json/json.h>
#include <cstdio>
#include <cstdlib>
#include <string>
#include "threadPool.h"
#define LOG_DIR "/home/zijianye/cpp_project/SipSupService/log"
#define LOG_FILE_NAME "SipServer.log"
// 栈上定义智能锁
class AutoMutexLock
{
public:
    AutoMutexLock(pthread_mutex_t *lock) : lock(lock) { getLock(); };
    ~AutoMutexLock() { freeLock(); };

private:
    AutoMutexLock();
    AutoMutexLock(const AutoMutexLock &);
    AutoMutexLock &operator=(const AutoMutexLock &);
    void getLock() { pthread_mutex_lock(lock); }
    void freeLock() { pthread_mutex_unlock(lock); }
    pthread_mutex_t *lock;
};

#endif // !_COMMON_H