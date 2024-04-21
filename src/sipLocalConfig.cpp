#include "sipLocalConfig.h"

#define CONFIGFILE_PATH "../conf/sipSubService.conf"
#define LOCAL_SECTION "localserver"
#define SIP_SECTION "sipserver"

static const std::string keyLocalIp = "local_ip";
static const std::string keyLocalPort = "local_port";
static const std::string keySipId = "sip_id";
static const std::string keySipIp = "sip_ip";
static const std::string keySipPort = "sip_port";

static const std::string keySupNodeNum = "supnode_num";
static const std::string keySupNodeId = "sip_supnode_id";
static const std::string keySupNodeIp = "sip_supnode_ip";
static const std::string keySupNodePort = "sip_supnode_port";
static const std::string keySupNodePoto = "sip_supnode_poto";
static const std::string keySupNodeAuth = "sip_supnode_auth";
static const std::string keySupNodeExpire = "sip_supNode_expire";

SipLocalConfig::SipLocalConfig() : m_conf(CONFIGFILE_PATH)
{
    m_localIp = "";
    m_localPort = 0;
    m_sipId = "";
    m_sipIp = "";
    m_sipPort = 0;
    m_supNodeIp = "";
    m_supNodePort = 0;
    m_supNodePoto = 0;
    // m_supNodeAuth = 0;
    m_supNodeExpire = 0;
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
    LOG(INFO) << "localip:" << m_localIp << ",localport:" << m_localPort << ",sipid:" << m_sipId << ",sipip:" << m_sipIp
              << ",sipport:" << m_sipPort;
    int num = m_conf.readInt(keySupNodeNum);
    SupNodeInfo info;
    for (int i = 0; i < num; i++)
    {
        std::string id = keySupNodeId + to_string(i);
        std::string ip = keySupNodeIp + to_string(i);
        std::string port = keySupNodePort + to_string(i);
        std::string poto = keySupNodePoto + to_string(i);
        // std::string auth = keySupNodeAuth + to_string(i);
        std::string expire = keySupNodeExpire + to_string(i);
        info.id = m_conf.readStr(id);
        info.ip = m_conf.readStr(ip);
        info.port = m_conf.readInt(port);
        info.poto = m_conf.readInt(poto);
        // info.auth = m_conf.readInt(auth);
        info.expire = m_conf.readInt(expire);
        upNodeInfoList.push_back(info);
        // LOG(INFO) << "subnodeip:" << m_supNodeIp << ",subnodeport:" << m_supNodePort
        //           << ",subnodepoto:" << m_supNodePoto << ",subnodeauth:" << m_supNodeAuth;
    }
    LOG(INFO) << "upNodeInfoList.SIZE:" << upNodeInfoList.size();
    return ret;
}