#pragma once

#include <vector>
#include <queue>
#include <mutex>
#include <stdint.h>
#include "Timer.h"
#include "Event.h"

class Poller;

class EventScheduler
{
public:
    enum PollerType
    {
        POLLER_SELECT,
        POLLER_POLL,
        POLLER_EPOLL
    };
    static EventScheduler* createNew(PollerType type);
    explicit EventScheduler(PollerType type);
    virtual ~EventScheduler();

public:
    bool addTriggerEvent(TriggerEvent* event);
    Timer::TimerId addTimedEventRunAfater(TimerEvent* event, Timer::TimeInterval delay);
    Timer::TimerId addTimedEventRunAt(TimerEvent* event, Timer::Timestamp when);
    Timer::TimerId addTimedEventRunEvery(TimerEvent* event, Timer::TimeInterval interval);
    bool removeTimedEvent(Timer::TimerId timerId);
    bool addIOEvent(IOEvent* event);
    bool updateIOEvent(IOEvent* event);
    bool removeIOEvent(IOEvent* event);
    void loop();
    //    void wakeup();
    Poller* poller();
    void setTimerManagerReadCallback(EventCallback cb, void* arg);

private:
    void handleTriggerEvents();

private:
    bool mQuit;
    Poller* mPoller;//IO多路复用管理对象
    TimerManager* mTimerManager;//定时任务管理对象
    std::vector<TriggerEvent*> mTriggerEvents;//条件触发事件集合
    std::mutex mMtx;

    // WIN系统专用的定时器回调start
    EventCallback mTimerManagerReadCallback;
    void* mTimerManagerArg;
    // WIN系统专用的定时器回调end
};