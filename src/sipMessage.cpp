#include "sipMessage.h"
sipMessage::sipMessage()
{
    memset(fromHeader, 0, sizeof(fromHeader));
    memset(toHeader, 0, sizeof(toHeader));
    memset(requestUrl, 0, sizeof(requestUrl));
    memset(contact, 0, sizeof(contact));
}
sipMessage::~sipMessage()
{
}
void sipMessage::setFrom(const char *fromUsr, const char *fromIp)
{
    sprintf(fromHeader, "<sip:%s@%s>", fromUsr, fromIp);
}
void sipMessage::setTo(const char *toUsr, const char *toIp)
{
    sprintf(toHeader, "<sip:%s@%s>", toUsr, toIp);
}
void sipMessage::setUrl(const char *sipId, const char *url_ip, int url_port, const char *url_poto)
{
    sprintf(requestUrl, "sip:%s@%s:%d;transport=%s", sipId, url_ip, url_port, url_poto);
}
void sipMessage::setContact(const char *sipId, const char *natIp, int natPort)
{
    sprintf(contact, "sip:%s@%s:%d", sipId, natIp, natPort);
}
