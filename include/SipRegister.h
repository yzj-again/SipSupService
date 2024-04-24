#ifndef _SIPREGISTER_H
#define _SIPREGISTER_H
#include "SipTaskBase.h"
#include "TaskTimer.h"
class SipRegister : public SipTaskBase
{
public:
    SipRegister();
    ~SipRegister();
    virtual pj_status_t run(pjsip_rx_data *rdata);
    pj_status_t RegisterRequestMessage(pjsip_rx_data *rdata);
    pj_status_t dealWithRegister(pjsip_rx_data *rdata);
    pj_status_t dealWithAuthorRegister(pjsip_rx_data *rdata);
    static void RegisterCheckProc(void *param);
    void registerServiceStart();

private:
    TaskTimer *m_regTimer;
};

#endif // !_SIPREGISTER_H