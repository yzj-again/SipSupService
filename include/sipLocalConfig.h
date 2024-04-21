#ifndef _SIPLOCALCONFIG_H
#define _SIPLOCALCONFIG_H
#include "confReader.h"
#include "common.h"
#include <list>
struct SupNodeInfo
{
    std::string id;
    std::string ip;
    int port;
    int poto;
    // int auth;
    int expire;
};

class SipLocalConfig
{
public:
    SipLocalConfig();
    ~SipLocalConfig();
    int ReadConf();
    inline std::string localIp() { return m_localIp; }
    inline int sipPort() { return m_sipPort; }
    inline std::string sipId() { return m_sipId; }
    inline std::string sipIp() { return m_sipIp; }
    std::list<SupNodeInfo> upNodeInfoList;

private:
    ConfReader m_conf;
    std::string m_localIp;
    int m_localPort;
    std::string m_sipId;
    std::string m_sipIp;
    int m_sipPort;
    std::string m_supNodeIp;
    int m_supNodePort;
    int m_supNodePoto;
    // int m_supNodeAuth;
    int m_supNodeExpire;
};
#endif // !_SIPLOCALCONFIG_H