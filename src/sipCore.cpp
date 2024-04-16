#include "sipCore.h"
#include "common.h"
#include "sipDefine.h"
SipCore::SipCore() : m_endpt(nullptr) {}
SipCore::~SipCore()
{
    pjsip_endpt_destroy(m_endpt);
}
bool SipCore::initSip(int sipPort)
{
    pj_status_t status;
    // 0-关闭， 6-详细
    pj_log_set_level(6);
    do
    {
        status = pj_init();
        if (PJ_SUCCESS != status)
        {
            LOG(ERROR) << "init pjlib faild, code: " << status;
        }
        status = pjlib_util_init();
        if (PJ_SUCCESS != status)
        {
            LOG(ERROR) << "init pjlib util faild, code: " << status;
        }
        // 初始化pjsip内存池
        pj_caching_pool cachingPool;
        pj_caching_pool_init(&cachingPool, NULL, SIP_STACK_SIZE);
        // 初始化四个重要模块
        // 一个pjsip对象只有一个endpoint对象，其他模块对象都是由endpoint对象来进行管理
        // 内存池工厂对象
        status = pjsip_endpt_create(&cachingPool.factory, NULL, &m_endpt);
        if (PJ_SUCCESS != status)
        {
            LOG(ERROR) << "create endpoint faild, code: " << status;
        }
        // 事务 transaction
        status = pjsip_tsx_layer_init_module(m_endpt);
        if (PJ_SUCCESS != status)
        {
            LOG(ERROR) << "init tsx layer faild, code: " << status;
        }
        // dialog模块
        status = pjsip_ua_init_module(m_endpt, NULL);
        if (PJ_SUCCESS != status)
        {
            LOG(ERROR) << "init ua layer faild, code: " << status;
        }
        // 传输层模块
        status = init_transport_layer(sipPort);
        if (PJ_SUCCESS != status)
        {
            LOG(ERROR) << "init transport layer faild, code: " << status;
        }
    } while (0);
    bool bret = true;
    if (PJ_SUCCESS != status)
    {
        bret = false;
    }
    return bret;
}
pj_status_t SipCore::init_transport_layer(int sipPort)
{
    // 负责将sip消息发送到网络，并接收来自网络的sip消息
    pj_sockaddr_in addr;
    pj_memset(&addr, 0, sizeof(addr));
    addr.sin_family = pj_AF_INET();
    addr.sin_addr.s_addr = 0;
    addr.sin_port = pj_htons((pj_uint16_t)sipPort);

    // 启动服务
    pj_status_t status;
    do
    {
        status = pjsip_udp_transport_start(m_endpt, &addr, NULL, 1, NULL);
        if (PJ_SUCCESS != status)
        {
            LOG(ERROR) << "start udp server faild, code: " << status;
            break;
        }
        status = pjsip_tcp_transport_start(m_endpt, &addr, 1, NULL);
        if (PJ_SUCCESS != status)
        {
            LOG(ERROR) << "start tcp server faild, code: " << status;
            break;
        }
        LOG(INFO) << "sip tcp: " << sipPort << "udp: " << sipPort;
    } while (0);
    return status;
}