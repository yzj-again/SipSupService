#ifndef _GLOBALCONTROL_H
#define _GLOBALCONTROL_H
#include "common.h"
#include "sipLocalConfig.h"
#include "threadPool.h"
#include "sipCore.h"
#include <time.h>
#include <random>
#include <sstream>
// 前置声明
class GlobalControl;
typedef struct _SubDomainInfo
{
    _SubDomainInfo()
    {
        sipId = "";
        addrIp = "";
        sipPort = 0;
        protocal = 0;
        expire = 0;
        registered = false;
        lastRegisterTime = 0;
        auth = false;
    }
    bool operator==(std::string id)
    {
        return (this->sipId == id);
    }
    std::string sipId;
    std::string addrIp;
    int sipPort;
    int protocal;
    int expire;
    bool registered;
    time_t lastRegisterTime;
    bool auth;
} SubDomainInfo;
typedef std::list<SubDomainInfo> SUBDOMAININFOLISTS;
#define GOBJ(obj) GlobalControl::instance()->obj
static pj_status_t pjcall_thread_register(pj_thread_desc &desc)
{
    pj_thread_t *thread = nullptr;
    if (!pj_thread_is_registered())
    {
        return pj_thread_register(NULL, desc, &thread);
    }
    return PJ_SUCCESS;
}
class GlobalControl
{
public:
    static GlobalControl *instance();
    bool init(void *param);
    SipLocalConfig *g_config = nullptr;
    ThreadPool *g_threadPool = nullptr;
    SipCore *g_sipServer = nullptr;
    SUBDOMAININFOLISTS &getSubNodeDomainInfoLists() { return m_subDomainInfoLists; }
    static void getGlobalLock() { pthread_mutex_lock(&g_lock); };
    static void freeGlobalLock() { pthread_mutex_unlock(&g_lock); };
    static pthread_mutex_t g_lock;
    // 控制服务进程退出标识
    static bool g_stopPool;

public:
    static bool checkIsExist(std::string id);
    static void setExpires(std::string id, int expires);
    static void setRegister(string id, bool registered);
    static void setLastRegTime(string id, time_t t);
    static bool getAuth(string id);
    static string randomNum(int length);

private:
    GlobalControl(){};
    ~GlobalControl();
    GlobalControl(const GlobalControl &globalCtl);
    const GlobalControl &operator=(const GlobalControl &globalCtl);

    static GlobalControl *m_pInstance;
    static SUBDOMAININFOLISTS m_subDomainInfoLists;
};
#endif // !_GLOBALCONTROL_H