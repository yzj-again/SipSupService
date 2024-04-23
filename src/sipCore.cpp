#include "sipCore.h"
#include "common.h"
#include "sipDefine.h"
#include "globalControl.h"
#include "SipRegister.h"
#include "ecThread.h"
using namespace EC;
static int pollingEvent(void *arg)
{
    LOG(INFO) << "polling event thread start success";
    pjsip_endpoint *ept = (pjsip_endpoint *)arg;
    while (!(GlobalControl::g_stopPool))
    {
        // 轮询处理endPoint事务 时间一到就返回
        pj_time_val timeout = {0, 500};
        pj_status_t status = pjsip_endpt_handle_events(ept, &timeout);
        if (PJ_SUCCESS != status)
        {
            LOG(ERROR) << "polling event faild, code: " << status;
            return -1;
        }
    }
    return 0;
}
void *SipCore::dealTaskThread(void *arg)
{
    threadParam *param = (threadParam *)arg;
    if (!param || param->base == nullptr)
    {
        return nullptr;
    }
    // pjlib线程注册
    pj_thread_desc desc;
    pjcall_thread_register(desc);
    param->base->run(param->data);
    delete param;
    param = nullptr;
}
pj_bool_t on_rx_request(pjsip_rx_data *rdata)
{
    // 回调中接收包，完成业务
    LOG(INFO) << "request msg coming";
    // 根据请求的方法完成不同的业务
    if (nullptr == rdata || nullptr == rdata->msg_info.msg)
    {
        return PJ_FALSE;
    }
    threadParam *param = new threadParam();
    pjsip_rx_data_clone(rdata, 0, &param->data);
    pjsip_msg *msg = rdata->msg_info.msg;
    if (msg->line.req.method.id == PJSIP_REGISTER_METHOD)
    {
        param->base = new SipRegister();
    }
    pthread_t pid;
    int ret = EC::ECThread::createThread(SipCore::dealTaskThread, param, pid);
    if (ret != 0)
    {
        LOG(ERROR) << "create task thread error";
        if (param)
        {
            delete param;
            param = NULL;
        }
        return PJ_FALSE;
    }

    return PJ_SUCCESS;
}
static pjsip_module recv_mod = {
    NULL,
    NULL,
    // 移除常量属性并转换为 char*
    {const_cast<char *>("mod-recv"), 8},
    -1,
    PJSIP_MOD_PRIORITY_APPLICATION,
    NULL,
    NULL,
    NULL,
    NULL,
    on_rx_request,
    NULL,
    NULL,
    NULL,
    NULL,
};
SipCore::SipCore() : m_endpt(nullptr)
{
}
SipCore::~SipCore()
{
    pjsip_endpt_destroy(m_endpt);
    // 释放缓冲池
    pj_caching_pool_destroy(&m_cachingPool);
    pj_shutdown();
    GlobalControl::g_stopPool = true;
}
bool SipCore::initSip(int sipPort)
{
    pj_status_t status;
    // 0-关闭， 6-详细
    pj_log_set_level(0);
    do
    {
        status = pj_init();
        if (PJ_SUCCESS != status)
        {
            LOG(ERROR) << "init pjlib faild, code: " << status;
            break;
        }
        status = pjlib_util_init();
        if (PJ_SUCCESS != status)
        {
            LOG(ERROR) << "init pjlib util faild, code: " << status;
            break;
        }
        // 初始化pjsip内存池

        pj_caching_pool_init(&m_cachingPool, NULL, SIP_STACK_SIZE);
        // 初始化四个重要模块
        // 一个pjsip对象只有一个endpoint对象，其他模块对象都是由endpoint对象来进行管理
        // 内存池工厂对象
        status = pjsip_endpt_create(&m_cachingPool.factory, NULL, &m_endpt);
        if (PJ_SUCCESS != status)
        {
            LOG(ERROR) << "create endpoint faild, code: " << status;
            break;
        }
        // 事务 transaction
        status = pjsip_tsx_layer_init_module(m_endpt);
        if (PJ_SUCCESS != status)
        {
            LOG(ERROR) << "init tsx layer faild, code: " << status;
            break;
        }
        // dialog模块
        status = pjsip_ua_init_module(m_endpt, NULL);
        if (PJ_SUCCESS != status)
        {
            LOG(ERROR) << "init ua layer faild, code: " << status;
            break;
        }
        // 传输层模块
        status = init_transport_layer(sipPort);
        if (PJ_SUCCESS != status)
        {
            LOG(ERROR) << "init transport layer faild, code: " << status;
            break;
        }
        // 应用层模块
        status = pjsip_endpt_register_module(m_endpt, &recv_mod);
        if (PJ_SUCCESS != status)
        {
            LOG(ERROR) << "register recv module faild, code: " << status;
            break;
        }
        // endpoint内存池分配
        pj_pool_t *pool = pjsip_endpt_create_pool(m_endpt, NULL, SIP_ALLOC_POOL_1M, SIP_ALLOC_POOL_1M);
        if (pool == NULL)
        {
            LOG(ERROR) << "create pool faild, code: " << status;
            break;
        }
        pj_thread_t *eventThread = NULL;
        status = pj_thread_create(pool, NULL, pollingEvent, m_endpt, 0, 0, &eventThread);
        if (PJ_SUCCESS != status)
        {
            LOG(ERROR) << "create pjsip thread faild, code: " << status;
            break;
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