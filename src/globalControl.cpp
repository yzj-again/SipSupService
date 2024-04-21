#include "globalControl.h"
GlobalControl *GlobalControl::m_pInstance = nullptr;
SUBDOMAININFOLISTS GlobalControl::m_subDomainInfoLists;
pthread_mutex_t GlobalControl::g_lock = PTHREAD_MUTEX_INITIALIZER;
bool GlobalControl::g_stopPool = false;
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
    SubDomainInfo info;
    for (auto it : g_config->supNodeInfoList)
    {
        info.sipId = it.id;
        info.addrIp = it.ip;
        info.sipPort = it.port;
        info.protocal = it.poto;
        m_subDomainInfoLists.push_back(info);
    }
    LOG(INFO) << "m_supDomainInfoLists.size()" << m_subDomainInfoLists.size();
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