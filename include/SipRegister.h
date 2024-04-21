#ifndef _SIPREGISTER_H
#define _SIPREGISTER_H
#include "SipTaskBase.h"
class SipRegister : public SipTaskBase
{
private:
    /* data */
public:
    SipRegister();
    ~SipRegister();
    virtual pj_status_t run(pjsip_rx_data *rdata);
};

#endif // !_SIPREGISTER_H