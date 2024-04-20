#ifndef _SIPREGISTER_H
#define _SIPREGISTER_H
#include "globalControl.h"
#include "TaskTimer.h"
class sipRegister
{
public:
    sipRegister();
    ~sipRegister();
    static void RegisterProc(void *param);
    int gbRegister(SupDomainInfo &node);
    void gbRegisterServiceStart();

private:
    TaskTimer *m_registerTimer;
};

#endif // !_SIPREGISTER_H