#include "sipRegister.h"
#include "common.h"

static void client_cb(struct pjsip_regc_cbparam *param)
{
    LOG(INFO) << "code " << param->code;
    if (param->code == 200)
    {
        SupDomainInfo *info = (SupDomainInfo *)param->token;
        info->registered = true;
    }
    return;
}
sipRegister::sipRegister()
{
    for (auto it : GlobalControl::instance()->getSupNodeDomainInfoLists())
    {
        if (!it.registered)
        {
            if (gbRegister(it) < 0)
            {
                LOG(ERROR) << "register error for " << it.sipId;
            }
        }
    }
}
sipRegister::~sipRegister()
{
}
int sipRegister::gbRegister(SupDomainInfo &node)
{
    // 循环的对list发送请求
    char fromHeader[128] = {0};
    sprintf(fromHeader, "<sip:%s@%s>", GOBJ(g_config)->sipId().c_str(), GOBJ(g_config)->sipIp().c_str());
    char toHeader[128] = {0};
    sprintf(toHeader, "<sip:%s@%s>", GOBJ(g_config)->sipId().c_str(), GOBJ(g_config)->sipIp().c_str());
    char requestUrl[128] = {0};
    if (node.protocal == 1)
    {
        sprintf(requestUrl, "sip:%s@%s:%d;transport=%s", node.sipId.c_str(), node.addrIp.c_str(), node.sipPort, "tcp");
    }
    else
    {
        sprintf(requestUrl, "sip:%s@%s:%d;transport=%s", node.sipId.c_str(), node.addrIp.c_str(), node.sipPort, "udp");
    }
    char contactUrl[128] = {0};
    sprintf(contactUrl, "sip:%s@%s:%d", GOBJ(g_config)->sipId().c_str(), GOBJ(g_config)->sipIp().c_str(), GOBJ(g_config)->sipPort());

    pj_str_t from = pj_str(fromHeader);
    pj_str_t to = pj_str(toHeader);
    pj_str_t line = pj_str(requestUrl);
    pj_str_t contact = pj_str(contactUrl);
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
    return true;
}