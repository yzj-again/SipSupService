#include "SipRegister.h"
#include "common.h"
#include "sipDefine.h"
#include "globalControl.h"
#include <time.h>
SipRegister::SipRegister()
{
}
SipRegister::~SipRegister()
{
}
pj_status_t SipRegister::run(pjsip_rx_data *rdata)
{
    return RegisterRequestMessage(rdata);
}
pj_status_t SipRegister::RegisterRequestMessage(pjsip_rx_data *rdata)
{
    // 上级进行鉴权和非鉴权判断
    pjsip_msg *msg = rdata->msg_info.msg;

    return dealWithRegister(rdata);
}
pj_status_t SipRegister::dealWithRegister(pjsip_rx_data *rdata)
{
    // 解出下级id是不是上级分配的
    pjsip_msg *msg = rdata->msg_info.msg;
    int status_code = 200;
    pjsip_from_hdr *from = (pjsip_from_hdr *)pjsip_msg_find_hdr(msg, PJSIP_H_FROM, NULL);
    if (NULL == from)
    {
        return -1;
    }
    char buf[1024] = {0};
    std::string fromId = "";
    pj_int32_t expiresValue = 0;
    int size = from->vptr->print_on(from, buf, sizeof(buf));
    if (size > 0)
    {
        fromId = buf;
        fromId = fromId.substr(11, 20);
    }
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
}