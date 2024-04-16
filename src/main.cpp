#include <cstdio>
#include <iostream>

#include <signal.h>
#include <pjlib-util.h>
#include <pjmedia.h>
#include <pjsip.h>
#include <pjsip_ua.h>
#include <pjsip/sip_auth.h>

#include "rtpsession.h"
#include "rtpsourcedata.h"
#include "rtptcptransmitter.h"
#include "rtptcpaddress.h"
#include "rtpudpv4transmitter.h"
#include "rtpipv4address.h"
#include "rtpsessionparams.h"
#include "rtperrors.h"
#include "rtplibraryversion.h"
#include "rtcpsrpacket.h"

#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/thread.h>
#include "common.h"
#include "sipLocalConfig.h"
#include "globalControl.h"
#include "ecThread.h"
using namespace EC;
class SetGlogLevel
{
public:
    SetGlogLevel(const int type);

    ~SetGlogLevel();
};

SetGlogLevel::SetGlogLevel(const int type)
{
    // 默认从外界传入tab构造
    // 将日志重定向到指定文件中
    google::InitGoogleLogging(LOG_FILE_NAME);
    // 设置输出到控制台的Log等级
    FLAGS_stderrthreshold = type;
    FLAGS_colorlogtostderr = true;
    FLAGS_logbufsecs = 0;
    FLAGS_log_dir = LOG_DIR;
    FLAGS_max_log_size = 4;
    google::SetLogDestination(google::GLOG_WARNING, "");
    google::SetLogDestination(google::GLOG_ERROR, "");
    // 日志输出时屏蔽终止信号
    signal(SIGPIPE, SIG_IGN);
}

SetGlogLevel::~SetGlogLevel()
{
    // 关闭glog日志库
    google::ShutdownGoogleLogging();
}

void *func(void *argc)
{
    pthread_t id = pthread_self();
    LOG(INFO) << "current thread id:" << id;
    return NULL;
}

int main(int argc, const char **argv)
{
    // 忽略信号
    signal(SIGINT, SIG_IGN);
    SetGlogLevel glog(0);
    SipLocalConfig *config = new SipLocalConfig();
    int ret = config->ReadConf();
    if (ret == -1)
    {
        LOG(ERROR) << "read config error";
        return ret;
    }
    bool GCret = GlobalControl::instance()->init(config);
    if (GCret == false)
    {
        LOG(ERROR) << "init error";
        return -1;
    }
    GOBJ(g_config)->localIp();
    pthread_t pid;
    ret = EC::ECThread::createThread(func, NULL, pid);
    if (ret != 0)
    {
        ret = -1;
        LOG(ERROR) << "create thread error";
        return ret;
    }
    LOG(INFO) << "create thread pid: " << pid;
    LOG(INFO) << "main thread pid: " << pthread_self();
    while (true)
    {
        sleep(30);
    }

    return 0;
}