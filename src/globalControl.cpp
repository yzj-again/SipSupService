#include "globalControl.h"
GlobalControl *GlobalControl::m_pInstance = nullptr;
GlobalControl *GlobalControl::instance()
{
    // 判断类实例是否有效
    if (!m_pInstance)
    {
        m_pInstance = new GlobalControl();
    }
    return m_pInstance;
}

bool GlobalControl::init(void *param)
{
    g_config = (SipLocalConfig *)param;
    if (!g_config)
    {
        return false;
    }
    if (!g_threadPool)
    {
        g_threadPool = new ThreadPool();
        int ret = g_threadPool->createThreadPool(10);
        if (ret < 0)
        {
            LOG(ERROR) << "create threadPool error";
        }
    }
    if (!g_sipServer)
    {
        g_sipServer = new SipCore();
    }
    g_sipServer->initSip(g_config->sipPort());
    return true;
}