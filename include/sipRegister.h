#ifndef _SIPREGISTER_H
#define _SIPREGISTER_H
#include "globalControl.h"
class sipRegister
{
private:
    /* data */
public:
    sipRegister();
    ~sipRegister();
    int gbRegister(SupDomainInfo &node);
};

#endif // !_SIPREGISTER_H