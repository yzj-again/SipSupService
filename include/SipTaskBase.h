#ifndef _SIPTASKBASE_H
#define _SIPTASKBASE_H
#include <pjlib-util.h>
#include <pjmedia.h>
#include <pjsip.h>
#include <pjsip_ua.h>
#include <pjsip/sip_auth.h>
#include <pjlib.h>
#include "common.h"
class SipTaskBase
{
private:
    /* data */
public:
    SipTaskBase() {}
    virtual ~SipTaskBase(){};
    virtual pj_status_t run(pjsip_rx_data *rdata) = 0;

protected:
    // 只能由派生类调用
    std::string parseFromId(pjsip_msg *msg);
};

#endif // !_SIPTASKBASE_H
