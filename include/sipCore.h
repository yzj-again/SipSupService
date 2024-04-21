#ifndef _SIPCORE_H
#define _SIPCORE_H
#include "SipTaskBase.h"
typedef struct _threadParam
{
    _threadParam()
    {
        base = nullptr;
        data = nullptr;
    }
    ~_threadParam()
    {
        if (base)
        {
            delete base;
            base = nullptr;
        }
        if (data)
        {
            pjsip_rx_data_free_cloned(data);
            base = nullptr;
        }
    }
    SipTaskBase *base;
    pjsip_rx_data *data;
} threadParam;
class SipCore
{
public:
    SipCore();
    ~SipCore();
    bool initSip(int sipPort);
    pj_status_t init_transport_layer(int sipPort);
    inline pjsip_endpoint *getEndpoint() { return m_endpt; }

public:
    static void *dealTaskThread(void *arg);

private:
    pjsip_endpoint *m_endpt;
    pj_caching_pool m_cachingPool;
};
#endif // !_SIPCORE_H
