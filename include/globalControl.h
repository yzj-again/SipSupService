#ifndef _GLOBALCONTROL_H
#define _GLOBALCONTROL_H
#include "common.h"
#include "sipLocalConfig.h"
#include "threadPool.h"
#include "sipCore.h"
// 前置声明
class GlobalControl;
typedef struct _SupDomainInfo
{
    _SupDomainInfo()
    {
        sipId = "";
        addrIp = "";
        sipPort = 0;
        protocal = 0;
        expire = 0;
        registered = false;
    }
    std::string sipId;
    std::string addrIp;
    int sipPort;
    int protocal;
    int expire;
    bool registered;
} SupDomainInfo;
typedef std::list<SupDomainInfo> SUPDOMAININFOLISTS;
#define GOBJ(obj) GlobalControl::instance()->obj
class GlobalControl
{
public:
    static GlobalControl *instance();
    bool init(void *param);
    SipLocalConfig *g_config = nullptr;
    ThreadPool *g_threadPool = nullptr;
    SipCore *g_sipServer = nullptr;
    SUPDOMAININFOLISTS &getSupNodeDomainInfoLists() { return m_supDomainInfoLists; }

private:
    GlobalControl(){};
    ~GlobalControl();
    GlobalControl(const GlobalControl &globalCtl);
    const GlobalControl &operator=(const GlobalControl &globalCtl);

    static GlobalControl *m_pInstance;
    static SUPDOMAININFOLISTS m_supDomainInfoLists;
};
#endif // !_GLOBALCONTROL_H