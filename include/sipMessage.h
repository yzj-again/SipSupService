#ifndef _SIPMESSAGE_H
#define _SIPMESSAGE_H
#include "common.h"
#include "globalControl.h"
class sipMessage
{
public:
    sipMessage();
    ~sipMessage();
    void setFrom(const char *fromUsr, const char *fromIp);
    void setTo(const char *toUsr, const char *toIp);
    void setUrl(const char *sipId, const char *url_ip, int url_port, const char *url_poto = (const char *)"udp");
    void setContact(const char *sipId, const char *natIp, int natPort);

    inline char *FromHeader() { return fromHeader; }
    inline char *ToHeader() { return toHeader; }
    inline char *RequestUrl() { return requestUrl; }
    inline char *Contact() { return contact; }

private:
    char fromHeader[128];
    char toHeader[128];
    char requestUrl[128];
    char contact[128];
};

#endif // !_SIPMESSAGE_H
