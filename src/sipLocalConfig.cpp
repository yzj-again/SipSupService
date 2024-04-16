#include "sipLocalConfig.h"

#define CONFIGFILE_PATH "../conf/sipSubService.conf"
#define LOCAL_SECTION "localserver"
#define SIP_SECTION "sipserver"

static const std::string keyLocalIp = "local_ip";
static const std::string keyLocalPort = "local_port";
static const std::string keySipIp = "sip_ip";
static const std::string keySipPort = "sip_port";

static const std::string keySupNodeNum = "supnode_num";
static const std::string keySupNodeIp = "sip_supnode_ip";
static const std::string keySupNodePort = "sip_supnode_port";
static const std::string keySupNodePoto = "sip_supnode_poto";
static const std::string keySupNodeAuth = "sip_supnode_auth";

SipLocalConfig::SipLocalConfig() : m_conf(CONFIGFILE_PATH)
{
    m_localIp = "";
    m_localPort = 0;
    m_sipIp = "";
    m_sipPort = 0;
    m_supNodeIp = "";
    m_supNodePort = 0;
    m_supNodePoto = 0;
    m_supNodeAuth = 0;
}
SipLocalConfig::~SipLocalConfig() {}
int SipLocalConfig::ReadConf()
{
    int ret = 0;
    m_conf.setSection(LOCAL_SECTION);
    m_localIp = m_conf.readStr(keyLocalIp);
    if (m_localIp.empty())
    {
        ret = -1;
        LOG(ERROR) << "local ip is wrong";
    }
    m_localPort = m_conf.readInt(keyLocalPort);
    if (m_localPort <= 0)
    {
        ret = -1;
        LOG(ERROR) << "local port is wrong";
    }

    m_conf.setSection(SIP_SECTION);
    m_sipIp = m_conf.readStr(keySipIp);
    if (m_sipIp.empty())
    {
        ret = -1;
        LOG(ERROR) << "sip ip is wrong";
    }
    m_sipPort = m_conf.readInt(keySipPort);
    if (m_sipPort <= 0)
    {
        ret = -1;
        LOG(ERROR) << "sip port is wrong";
    }
    int num = m_conf.readInt(keySupNodeNum);
    for (int i = 0; i < num; i++)
    {
        std::string ip = keySupNodeIp + to_string(i);
        std::string port = keySipPort + to_string(i);
        std::string poto = keySupNodePoto + to_string(i);
        std::string auth = keySupNodeAuth + to_string(i);
        m_supNodeIp = m_conf.readStr(ip);
        m_supNodePort = m_conf.readInt(port);
        m_supNodePoto = m_conf.readInt(poto);
        m_supNodeAuth = m_conf.readInt(auth);
        LOG(INFO) << "local ip " << m_supNodeIp << "local port " << m_supNodePort;
    }

    return ret;
}