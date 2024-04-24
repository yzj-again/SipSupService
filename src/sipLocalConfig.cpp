#include "sipLocalConfig.h"

#define CONFIGFILE_PATH "../conf/sipSupService.conf"
#define LOCAL_SECTION "localserver"
#define SIP_SECTION "sipserver"

static const std::string keyLocalIp = "local_ip";
static const std::string keyLocalPort = "local_port";
static const std::string keySipId = "sip_id";
static const std::string keySipIp = "sip_ip";
static const std::string keySipPort = "sip_port";
static const std::string keySipRealm = "sip_realm";

static const std::string keySubNodeNum = "subnode_num";
static const std::string keySubNodeId = "sip_subnode_id";
static const std::string keySubNodeIp = "sip_subnode_ip";
static const std::string keySubNodePort = "sip_subnode_port";
static const std::string keySubNodePoto = "sip_subnode_poto";
static const std::string keySubNodeAuth = "sip_subnode_auth";

SipLocalConfig::SipLocalConfig() : m_conf(CONFIGFILE_PATH)
{
    m_localIp = "";
    m_localPort = 0;
    m_sipId = "";
    m_sipIp = "";
    m_sipPort = 0;
    m_sipRealm = "";
    m_subNodeIp = "";
    m_subNodePort = 0;
    m_subNodePoto = 0;
    m_subNodeAuth = 0;
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
    m_sipId = m_conf.readStr(keySipId);
    if (m_sipId.empty())
    {
        ret = -1;
        LOG(ERROR) << "sip id is wrong";
    }
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
    m_sipRealm = m_conf.readStr(keySipRealm);
    if (m_sipRealm.empty())
    {
        ret = -1;
        LOG(ERROR) << "sip realm is wrong";
        return ret;
    }
    int num = m_conf.readInt(keySubNodeNum);
    SupNodeInfo info;
    for (int i = 0; i < num; i++)
    {
        std::string id = keySubNodeId + to_string(i);
        std::string ip = keySubNodeIp + to_string(i);
        std::string port = keySipPort + to_string(i);
        std::string poto = keySubNodePoto + to_string(i);
        std::string auth = keySubNodeAuth + to_string(i);
        info.id = m_conf.readStr(id);
        info.ip = m_conf.readStr(ip);
        info.port = m_conf.readInt(port);
        info.poto = m_conf.readInt(poto);
        info.auth = m_conf.readInt(auth);
        supNodeInfoList.push_back(info);
    }

    return ret;
}