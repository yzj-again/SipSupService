#ifndef _SIPLOCALCONFIG_H
#define _SIPLOCALCONFIG_H
#include "confReader.h"
#include "common.h"
class SipLocalConfig
{
public:
    SipLocalConfig();
    ~SipLocalConfig();
    int ReadConf();
    inline std::string localIp() { return m_localIp; }
    inline int sipPort() { return m_sipPort; }

private:
    ConfReader m_conf;
    std::string m_localIp;
    int m_localPort;
    std::string m_sipIp;
    int m_sipPort;
    std::string m_supNodeIp;
    int m_supNodePort;
    int m_supNodePoto;
    int m_supNodeAuth;
};
#endif // !_SIPLOCALCONFIG_H