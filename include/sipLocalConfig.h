#ifndef _SIPLOCALCONFIG_H
#define _SIPLOCALCONFIG_H
#include "confReader.h"
#include "common.h"
#include <list>
#include <algorithm>
#define CONFIGFILE_PATH "/home/zijianye/cpp_project/SipSupService/conf/sipSupService.conf"

struct SupNodeInfo
{
    std::string id;
    std::string ip;
    int port;
    int poto;
    int auth;
};
class SipLocalConfig
{
public:
    SipLocalConfig();
    ~SipLocalConfig();
    int ReadConf();
    inline std::string localIp() { return m_localIp; }
    inline int sipPort() { return m_sipPort; }
    std::list<SupNodeInfo> supNodeInfoList;

private:
    ConfReader m_conf;
    std::string m_localIp;
    int m_localPort;
    std::string m_sipId;
    std::string m_sipIp;
    int m_sipPort;
    std::string m_subNodeIp;
    int m_subNodePort;
    int m_subNodePoto;
    int m_subNodeAuth;
};
#endif // !_SIPLOCALCONFIG_H