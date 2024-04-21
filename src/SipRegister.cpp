#include "SipRegister.h"
#include "common.h"
SipRegister::SipRegister()
{
}
SipRegister::~SipRegister()
{
}
pj_status_t SipRegister::run(pjsip_rx_data *rdata)
{
    return PJ_SUCCESS;
}