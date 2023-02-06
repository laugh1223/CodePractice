#pragma once
#include "Poller.h"
#include <vector>
#ifndef WIN32
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#else
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>
#endif // !WIN32

class SelectPoller : public Poller
{
public:
    SelectPoller();
    virtual ~SelectPoller();
    static SelectPoller* createNew();
    virtual bool addIOEvent(IOEvent* event);//更新事件
    virtual bool updateIOEvent(IOEvent* event);//更新事件
    virtual bool removeIOEvent(IOEvent* event);//移除事件
    virtual void handleEvent();//等待事件发生并处理

private:
    fd_set mReadSet;//读事件集合
    fd_set mWriteSet;//写事件集合
    fd_set mExceptionSet;//错误事件集合
    int mMaxNumSockets;
    std::vector<IOEvent*> mIOEvents;// 存储临时活跃的IO事件对象
};