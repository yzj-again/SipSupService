#include "sipRegister.h"
#include "common.h"
#include "sipMessage.h"

void client_cb(struct pjsip_regc_cbparam *param)
{
    LOG(INFO) << "code " << param->code;
    if (param->code == 200)
    {
        SupDomainInfo *info = (SupDomainInfo *)param->token;
        info->registered = true;
    }
    return;
}
void sipRegister::RegisterProc(void *param)
{
    sipRegister *pthis = (sipRegister *)param;
    // this全局共享
    // GlobalControl::getGlobalLock();
    AutoMutexLock lock(&(GlobalControl::g_lock));
    for (auto it : GlobalControl::instance()->getSupNodeDomainInfoLists())
    {
        if (!it.registered)
        {
            if (pthis->gbRegister(it) < 0)
            {
                LOG(ERROR) << "register error for " << it.sipId;
            }
        }
    }
    // GlobalControl::freeGlobalLock();
}
sipRegister::sipRegister()
{
    m_registerTimer = new TaskTimer(3);
}
sipRegister::~sipRegister()
{
    if (m_registerTimer)
    {
        delete m_registerTimer;
        m_registerTimer = nullptr;
    }
}

void sipRegister::gbRegisterServiceStart()
{
    if (m_registerTimer)
    {
        m_registerTimer->setTimerFunc(sipRegister::RegisterProc, (void *)this);
        m_registerTimer->start();
    }
}
int sipRegister::gbRegister(SupDomainInfo &node)
{
    // 循环的对list发送请求
    sipMessage msg;
    msg.setFrom(GOBJ(g_config)->sipId().c_str(), GOBJ(g_config)->sipIp().c_str());
    msg.setTo(GOBJ(g_config)->sipId().c_str(), GOBJ(g_config)->sipIp().c_str());
    if (node.protocal == 1)
    {
        msg.setUrl(node.sipId.c_str(), node.addrIp.c_str(), node.sipPort, (char *)"tcp");
    }
    else
    {
        msg.setUrl(node.sipId.c_str(), node.addrIp.c_str(), node.sipPort);
    }
    msg.setContact(GOBJ(g_config)->sipId().c_str(), GOBJ(g_config)->sipIp().c_str(), GOBJ(g_config)->sipPort());

    pj_str_t from = pj_str(msg.FromHeader());
    pj_str_t to = pj_str(msg.ToHeader());
    pj_str_t line = pj_str(msg.RequestUrl());
    pj_str_t contact = pj_str(msg.Contact());
    // 定义一个sip客户端注册结构体的变量
    pj_status_t status = PJ_SUCCESS;
    do
    {
        pjsip_regc *regc;
        status = pjsip_regc_create(GOBJ(g_sipServer)->getEndpoint(), &node, &client_cb, &regc);
        if (PJ_SUCCESS != status)
        {
            LOG(ERROR) << "pjsip_regc_create error";
            break;
        }
        status = pjsip_regc_init(regc, &line, &from, &to, 1, &contact, node.expire);
        if (PJ_SUCCESS != status)
        {
            pjsip_regc_destroy(regc);
            LOG(ERROR) << "pjsip_regc_init error";
            break;
        }
        // 生成请求消息体
        pjsip_tx_data *tdata = NULL;
        status = pjsip_regc_register(regc, PJ_TRUE, &tdata);
        if (PJ_SUCCESS != status)
        {
            pjsip_regc_destroy(regc);
            LOG(ERROR) << "pjsip_regc_register error";
            break;
        }
        status = pjsip_regc_send(regc, tdata);
        if (PJ_SUCCESS != status)
        {
            pjsip_regc_destroy(regc);
            LOG(ERROR) << "pjsip_regc_send error";
            break;
        }
    } while (0);
    int ret = 0;
    if (PJ_SUCCESS != status)
    {
        ret = -1;
    }
    return ret;
}