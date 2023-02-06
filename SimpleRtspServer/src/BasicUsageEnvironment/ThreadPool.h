﻿#pragma once
#include <queue>
#include <vector>
#include "Thread.h"
#include <mutex> 
#include <condition_variable>

class ThreadPool
{
public:
    class Task
    {
    public:
        typedef void (*TaskCallback)(void*);
        Task():mTaskCallback(NULL),mArg(NULL) {};

        void setTaskCallback(TaskCallback cb, void* arg)
        {
            mTaskCallback = cb; mArg = arg;
        }

        //线程池开始任务
        void handle() 
        {
            if(mTaskCallback)
                mTaskCallback(mArg);
        }

        bool operator=(const Task& task) 
        {
            this->mTaskCallback = task.mTaskCallback;
            this->mArg = task.mArg;
        }

    private:
        TaskCallback mTaskCallback;
        void* mArg;
    };

    static ThreadPool* createNew(int num);
    explicit ThreadPool(int num);
    ~ThreadPool();
    void addTask(Task& task);

private:
    void loop();

    class MThread : public Thread
    {
    protected:
        virtual void run(void *arg);
    };

    void createThreads();
    void cancelThreads();

private:
    std::queue<Task> mTaskQueue;
    std::mutex mMtx; // 互斥锁.
    std::condition_variable mCon; // 条件变量.
    std::vector<MThread> mThreads;
    bool mQuit;
};