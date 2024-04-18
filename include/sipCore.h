#ifndef _SIPCORE_H
#define _SIPCORE_H
#include "pjlib-util.h"
#include "pjmedia.h"
#include "pjsip.h"
#include "pjsip_ua.h"
#include "pjsip/sip_auth.h"
#include "pjlib.h"
class SipCore
{
public:
    SipCore();
    ~SipCore();
    bool initSip(int sipPort);
    pj_status_t init_transport_layer(int sipPort);
    inline pjsip_endpoint *getEndpoint() { return m_endpt; }

private:
    pjsip_endpoint *m_endpt;
    pj_caching_pool m_cachingPool;
};
#endif // !_SIPCORE_H
