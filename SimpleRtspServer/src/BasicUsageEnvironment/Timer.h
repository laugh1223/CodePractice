#pragma once
#include <map>
#include <stdint.h>

class EventScheduler;
class Poller;
class TimerEvent;
class IOEvent;

class Timer
{
public:
    typedef uint32_t TimerId;
    typedef int64_t Timestamp; //ms 某个点的时间
    typedef uint32_t TimeInterval; //ms 时间间隔
    ~Timer();
    static Timestamp getCurTime();// 获取当前系统启动以来的毫秒数
    static Timestamp getCurTimestamp();// 获取毫秒级时间戳（13位）

private:
    friend class TimerManager;
    Timer(TimerEvent* event, Timestamp timestamp, TimeInterval timeInterval, TimerId timerId);
    bool handleEvent();

private:
    TimerEvent* mTimerEvent;//定时任务对象
    Timestamp mTimestamp; //某个点的时间
    TimeInterval mTimeInterval;//时间间隔
    TimerId mTimerId;//定时器ID
    bool mRepeat;//是否重复执行标识
};

class TimerManager
{
public:
    static TimerManager* createNew(EventScheduler *scheduler);
    TimerManager(EventScheduler* scheduler);
    ~TimerManager();
    Timer::TimerId addTimer(TimerEvent* event, Timer::Timestamp timestamp, Timer::TimeInterval timeInterval);
    bool removeTimer(Timer::TimerId timerId);

private:
    static void readCallback(void* arg);
    void handleRead();
    void modifyTimeout();

private:
    Poller* mPoller;//IO复用对象
    std::map<Timer::TimerId, Timer> mTimers;
    std::multimap<Timer::Timestamp, Timer> mEvents;//某个时间节点的定时任务
    uint32_t mLastTimerId;//计算器
#ifndef WIN32
    int mTimerFd;
    IOEvent* mTimerIOEvent;
#endif // !WIN32
};
