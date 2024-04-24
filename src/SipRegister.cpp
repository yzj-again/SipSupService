#include "SipRegister.h"
#include "common.h"
#include "sipDefine.h"
#include "globalControl.h"
#include <sys/sysinfo.h>
SipRegister::SipRegister()
{
    m_regTimer = new TaskTimer(10);
}
SipRegister::~SipRegister()
{
    if (m_regTimer)
    {
        delete m_regTimer;
        m_regTimer = NULL;
    }
}
void SipRegister::registerServiceStart()
{
    if (m_regTimer)
    {
        m_regTimer->setTimerFunc(RegisterCheckProc, this);
        m_regTimer->start();
    }
}
void SipRegister::RegisterCheckProc(void *param)
{
    time_t regTime = 0;
    struct sysinfo info;
    memset(&info, 0, sizeof(info));
    int ret = sysinfo(&info);
    if (ret == 0)
    {
        regTime = info.uptime;
    }
    else
    {
        regTime = time(NULL);
    }
    AutoMutexLock lock(&(GlobalControl::g_lock));
    SUBDOMAININFOLISTS::iterator iter = GlobalControl::instance()->getSubNodeDomainInfoLists().begin();
    for (; iter != GlobalControl::instance()->getSubNodeDomainInfoLists().end(); iter++)
    {
        if (iter->registered)
        {
            LOG(INFO) << "regTime: " << regTime << ",lastRegTime: " << iter->lastRegisterTime;
            if (regTime - iter->lastRegisterTime >= iter->expire)
            {
                // 过期了
                iter->registered = false;
                LOG(INFO) << "register time was gone";
            }
        }
    }
}
pj_status_t SipRegister::run(pjsip_rx_data *rdata)
{
    return RegisterRequestMessage(rdata);
}
pj_status_t SipRegister::RegisterRequestMessage(pjsip_rx_data *rdata)
{
    // 上级进行鉴权和非鉴权判断
    pjsip_msg *msg = rdata->msg_info.msg;
    if (GlobalControl::getAuth(parseFromId(msg)))
    {
        return dealWithAuthorRegister(rdata);
    }
    else
    {
        return dealWithRegister(rdata);
    }
}

pj_status_t SipRegister::dealWithAuthorRegister(pjsip_rx_data *rdata)
{
    // 1.解msg 下级发送register 上级会回复一个401，携带一个头www-Authenticate;
    // 然后下级再发送一个register,携带一个验证头
    pjsip_msg *msg = rdata->msg_info.msg;

    string fromId = parseFromId(msg);
    int expiresValue = 0;
    pjsip_hdr hdr_list;
    pj_list_init(&hdr_list);
    int status_code = 401;
    pj_status_t status;
    bool registered = false;
    if (pjsip_msg_find_hdr(msg, PJSIP_H_AUTHORIZATION, NULL) == NULL)
    {
        pjsip_www_authenticate_hdr *hdr = pjsip_www_authenticate_hdr_create(rdata->tp_info.pool);
        hdr->scheme = pj_str((char *)"digest");
        // nonce 随机串
        std::string nonce = GlobalControl::randomNum(32);
        LOG(INFO) << "nonce: " << nonce;
        hdr->challenge.digest.nonce = pj_str((char *)nonce.c_str());
        // realm
        hdr->challenge.digest.realm = pj_str((char *)GOBJ(g_config)->realm().c_str());
        // opaque 放置客户端重复攻击
        std::string opaque = GlobalControl::randomNum(32);
        LOG(INFO) << "opaque:" << opaque;
        hdr->challenge.digest.opaque = pj_str((char *)opaque.c_str());
        // 加密方式
        hdr->challenge.digest.algorithm = pj_str((char *)"MD5");
        // 假如header list
        pj_list_push_back(&hdr_list, hdr);
    }
#if 0
    else
    {

        pjsip_auth_srv auth_srv;
        pj_str_t realm = pj_str((char *)GOBJ(g_config)->realm().c_str());
        status = pjsip_auth_srv_init(rdata->tp_info.pool, &auth_srv, &realm, &auth_cred_callback, 0);
        if (PJ_SUCCESS != status)
        {
            LOG(ERROR) << "pjsip_auth_srv_init failed";
            status_code = 401;
        }
        pjsip_auth_srv_verify(&auth_srv, rdata, &status_code);
        LOG(INFO) << "status_code: " << status_code;
        if (SIP_SUCCESS == status_code)
        {
            pjsip_expires_hdr *expires = (pjsip_expires_hdr *)pjsip_msg_find_hdr(msg, PJSIP_H_EXPIRES, NULL);
            expiresValue = expires->ivalue;
            GlobalControl::setExpires(fromId, expiresValue);

            // data字段hdr部分组织
            time_t t;
            t = time(0);
            char bufT[32] = {0};
            strftime(bufT, sizeof(bufT), "%y-%m-%d%H:%M:%S", localtime(&t));
            pj_str_t value_time = pj_str(bufT);
            pj_str_t key = pj_str("Date");
            pjsip_date_hdr *date_hrd = (pjsip_date_hdr *)pjsip_date_hdr_create(rdata->tp_info.pool, &key, &value_time);
            pj_list_push_back(&hdr_list, date_hrd);
            registered = true;
        }
    }
#endif
    // 回复401
    status = pjsip_endpt_respond(GOBJ(g_sipServer)->getEndpoint(), NULL, rdata, status_code, NULL, &hdr_list, NULL, NULL);
    if (PJ_SUCCESS != status)
    {
        LOG(ERROR) << "pjsip_endpt_respond failed";
        return status;
    }

    if (registered)
    {
        if (expiresValue > 0)
        {
            time_t regTime = 0;
            struct sysinfo info;
            memset(&info, 0, sizeof(info));
            int ret = sysinfo(&info);
            if (ret == 0)
            {
                regTime = info.uptime;
            }
            else
            {
                regTime = time(NULL);
            }
            GlobalControl::setRegister(fromId, true);
            GlobalControl::setLastRegTime(fromId, regTime);
        }
        else if (expiresValue == 0)
        {
            GlobalControl::setRegister(fromId, false);
            GlobalControl::setLastRegTime(fromId, 0);
        }
    }
}

pj_status_t SipRegister::dealWithRegister(pjsip_rx_data *rdata)
{
    // 解出下级id是不是上级分配的
    pjsip_msg *msg = rdata->msg_info.msg;
    int status_code = 200;
    pj_int32_t expiresValue = 0;
    std::string fromId = parseFromId(msg);
    if (!(GlobalControl::checkIsExist(fromId)))
    {
        status_code = statusCode::SIP_FORBIDDEN;
    }
    else
    {
        pjsip_expires_hdr *expires = (pjsip_expires_hdr *)pjsip_msg_find_hdr(msg, PJSIP_H_EXPIRES, NULL);
        expiresValue = expires->ivalue;
        GlobalControl::setExpires(fromId, expiresValue);
    }

    // 发送响应 rx接收，tx发送
    pjsip_tx_data *txdata;
    pj_status_t status = pjsip_endpt_create_response(GOBJ(g_sipServer)->getEndpoint(), rdata, status_code, NULL, &txdata);
    if (PJ_SUCCESS != status)
    {
        LOG(ERROR) << "create response failed";
        return status;
    }
    time_t t;
    t = time(0);
    char bufT[32] = {0};
    strftime(bufT, sizeof(bufT), "%y-%m-%d%H:%M:%S", localtime(&t));
    pj_str_t value_time = pj_str(bufT);
    pj_str_t key = pj_str("Date");
    pjsip_date_hdr *date_hrd = (pjsip_date_hdr *)pjsip_date_hdr_create(rdata->tp_info.pool, &key, &value_time);
    pjsip_msg_add_hdr(txdata->msg, (pjsip_hdr *)date_hrd);
    pjsip_response_addr res_addr;
    // 从rdata获取响应的地址
    status = pjsip_get_response_addr(txdata->pool, rdata, &res_addr);
    if (PJ_SUCCESS != status)
    {
        pjsip_tx_data_dec_ref(txdata);
        LOG(ERROR) << "get response addr failed";
        return status;
    }
    // 发送响应
    status = pjsip_endpt_send_response(GOBJ(g_sipServer)->getEndpoint(), &res_addr, txdata, NULL, NULL);
    if (PJ_SUCCESS != status)
    {
        pjsip_tx_data_dec_ref(txdata);
        LOG(ERROR) << "send response msg failed";
        return status;
    }
    if (status_code == 200)
    {
        if (expiresValue > 0) // 注册
        {
            time_t regTime = 0;
            struct sysinfo info;
            memset(&info, 0, sizeof(info));
            int ret = sysinfo(&info);
            if (ret == 0)
            {
                regTime = info.uptime;
            }
            else
            {
                regTime = time(NULL);
            }
            GlobalControl::setRegister(fromId, true);
            GlobalControl::setLastRegTime(fromId, regTime);
        }
        else if (expiresValue == 0) // 注销
        {
            GlobalControl::setRegister(fromId, false);
            GlobalControl::setLastRegTime(fromId, 0);
        }
    }
    return status;
}