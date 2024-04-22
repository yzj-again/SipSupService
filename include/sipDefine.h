#ifndef _SIPDEFINE_H
#define _SIPDEFINE_H
#include <string>
#include <string.h>
// 大小1024*256
#define SIP_STACK_SIZE 1024 * 256
#define SIP_ALLOC_POOL_1M 1024 * 1024 * 1
enum statusCode
{
    SIP_SUCCESS = 200,
    SIP_FORBIDDEN = 404,
};
#endif // !_SIPDEFINE_H
