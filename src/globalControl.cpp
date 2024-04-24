#include "globalControl.h"
GlobalControl *GlobalControl::m_pInstance = nullptr;
SUBDOMAININFOLISTS GlobalControl::m_subDomainInfoLists;
pthread_mutex_t GlobalControl::g_lock = PTHREAD_MUTEX_INITIALIZER;
bool GlobalControl::g_stopPool = false;
// 随机数种子对象
std::random_device rd;
// 随机数生成器 依赖种子
mt19937 gen(rd());
// 分布器类模板  设定一个0-15的均匀整数分布的区间范围
uniform_int_distribution<> dis(0, 15);
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
        info.auth = it.auth;
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
bool GlobalControl::checkIsExist(string id)
{
    AutoMutexLock lck(&g_lock);
    SUBDOMAININFOLISTS::iterator it;
    it = std::find(m_subDomainInfoLists.begin(), m_subDomainInfoLists.end(), id);
    if (it != m_subDomainInfoLists.end())
    {
        return true;
    }
    return false;
}

void GlobalControl::setExpires(string id, int expires)
{
    AutoMutexLock lck(&g_lock);
    SUBDOMAININFOLISTS::iterator it;
    it = std::find(m_subDomainInfoLists.begin(), m_subDomainInfoLists.end(), id);
    if (it != m_subDomainInfoLists.end())
    {
        it->expire = expires;
    }
}
void GlobalControl::setRegister(string id, bool registered)
{
    AutoMutexLock lck(&g_lock);
    SUBDOMAININFOLISTS::iterator it;
    it = std::find(m_subDomainInfoLists.begin(), m_subDomainInfoLists.end(), id);
    if (it != m_subDomainInfoLists.end())
    {
        it->registered = registered;
    }
}

void GlobalControl::setLastRegTime(string id, time_t t)
{
    AutoMutexLock lck(&g_lock);
    SUBDOMAININFOLISTS::iterator it;
    it = std::find(m_subDomainInfoLists.begin(), m_subDomainInfoLists.end(), id);
    if (it != m_subDomainInfoLists.end())
    {
        it->lastRegisterTime = t;
    }
}
bool GlobalControl::getAuth(string id)
{
    AutoMutexLock lck(&g_lock);
    SUBDOMAININFOLISTS::iterator it;
    it = std::find(m_subDomainInfoLists.begin(), m_subDomainInfoLists.end(), id);
    if (it != m_subDomainInfoLists.end())
    {
        return it->auth;
    }
}

string GlobalControl::randomNum(int length)
{

    stringstream ss;
    for (int i = 0; i < length; ++i)
    {
        // 每次使用随机数生成器在指定的分布范围内获取一个随机数
        int value = dis(gen);
        ss << std::hex << value;
    }
    // stringstream ss;
    // for (int i = 0; i < length; ++i)
    // {
    //     int value = random() % 15;
    //     ss << std::hex << value;
    // }

    return ss.str();
}