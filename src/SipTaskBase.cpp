#include "SipTaskBase.h"

std::string SipTaskBase::parseFromId(pjsip_msg *msg)
{
    pjsip_from_hdr *from = (pjsip_from_hdr *)pjsip_msg_find_hdr(msg, PJSIP_H_FROM, NULL);
    if (NULL == from)
    {
        return "";
    }
    char buf[1024] = {0};
    std::string fromId = "";
    int size = from->vptr->print_on(from, buf, 1024);
    if (size > 0)
    {
        fromId = buf;
        fromId = fromId.substr(11, 20);
    }
    return fromId;
}