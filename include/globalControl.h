#ifndef _GLOBALCONTROL_H
#define _GLOBALCONTROL_H
#include "common.h"
#include "sipLocalConfig.h"
#include "threadPool.h"
// 前置声明
class GlobalControl;
#define GOBJ(obj) GlobalControl::instance()->obj
class GlobalControl
{
public:
    static GlobalControl *instance();
    bool init(void *param);
    SipLocalConfig *g_config = nullptr;
    ThreadPool *g_threadPool = nullptr;

private:
    GlobalControl(){};
    ~GlobalControl();
    GlobalControl(const GlobalControl &globalCtl);
    const GlobalControl &operator=(const GlobalControl &globalCtl);

    static GlobalControl *m_pInstance;
};
#endif // !_GLOBALCONTROL_H